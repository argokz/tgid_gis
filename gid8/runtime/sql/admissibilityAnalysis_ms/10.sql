WITH BaseData AS (
    SELECT
        l.id,
        CONCAT(
            N'Участок ', ec1.name, N' ', n1.externalNodeName, 
            N' -> ', ec2.name, N' ', n2.externalNodeName
        ) AS [Наименование участка],

        hps.diameterCondit                                    AS d_mm,
        utP.a10                                               AS v_pod,     -- скорость подача
        utO.a10                                               AS v_obr,     -- скорость обратка
        utP.a14                                               AS dp_pod,    -- потери подача, мм.в.ст./м
        utO.a14                                               AS dp_obr     -- потери обратка

    FROM linesobj l
        INNER JOIN heatpipesections hps ON hps.lineID = l.id
        INNER JOIN nodes n1 ON n1.id = l.nodeid1 AND n1.removed = 0
        INNER JOIN nodes n2 ON n2.id = l.nodeid2 AND n2.removed = 0
        LEFT JOIN externalcodes ec1 ON ec1.id = n1.externalcodeid
        LEFT JOIN externalcodes ec2 ON ec2.id = n2.externalcodeid

        -- Последний расчёт по fileID
        OUTER APPLY (
            SELECT TOP 1 c.id AS last_calc_id
            FROM CALCULATION c
            WHERE c.fileID = n1.fileID
            ORDER BY c.id DESC
        ) calc

        LEFT JOIN UT_OUT utP 
               ON utP.lineID = l.id 
              AND utP.calculationID = calc.last_calc_id 
              AND utP.externalSignLineID = 2   -- подающий

        LEFT JOIN UT_OUT utO 
               ON utO.lineID = l.id 
              AND utO.calculationID = calc.last_calc_id 
              AND utO.externalSignLineID = 3   -- обратный

    WHERE l.removed = 0
      AND n1.fileID = $fileID$
      AND utP.id IS NOT NULL
      AND utO.id IS NOT NULL
),
WithNorms AS (
    SELECT *,
        -- Нормы скорости в зависимости от диаметра
        CASE 
            WHEN d_mm > 300 THEN N'1.0–2.5'
            WHEN d_mm >= 100 THEN N'1.2–3.0'
            ELSE N'1.5–3.5'
        END AS norm_v_pod,

        CASE 
            WHEN d_mm > 300 THEN N'0.8–2.0'
            WHEN d_mm >= 100 THEN N'1.0–2.5'
            ELSE N'1.5–3.0'
        END AS norm_v_obr,

        -- Границы для сравнения
        CASE WHEN d_mm > 300 THEN 1.0 WHEN d_mm >= 100 THEN 1.2 ELSE 1.5 END AS v_min_pod,
        CASE WHEN d_mm > 300 THEN 2.5 WHEN d_mm >= 100 THEN 3.0 ELSE 3.5 END AS v_max_pod,
        CASE WHEN d_mm > 300 THEN 0.8 WHEN d_mm >= 100 THEN 1.0 ELSE 1.5 END AS v_min_obr,
        CASE WHEN d_mm > 300 THEN 2.0 WHEN d_mm >= 100 THEN 2.5 ELSE 3.0 END AS v_max_obr
    FROM BaseData
)
SELECT
    id,
    [Наименование участка],
    d_mm                                          AS [Диаметр, мм],

    v_pod                                         AS [Скорость потока (подача), м/с],
    v_obr                                         AS [Скорость потока (обратка), м/с],

    norm_v_pod                                    AS [Рекоменд. скорость (подача), м/с],
    norm_v_obr                                    AS [Рекоменд. скорость (обратка), м/с],

    -- Анализ скорости
    CASE 
        WHEN v_pod < v_min_pod THEN N'Ниже рекомендуемого диапазона'
        WHEN v_pod > v_max_pod THEN N'Выше рекомендуемого диапазона'
        ELSE N'В пределах нормы'
    END AS [Анализ скорости (подача)],

    CASE 
        WHEN v_obr < v_min_obr THEN N'Ниже рекомендуемого диапазона'
        WHEN v_obr > v_max_obr THEN N'Выше рекомендуемого диапазона'
        ELSE N'В пределах нормы'
    END AS [Анализ скорости (обратка)],

    dp_pod                                        AS [Уд. потери (подача), мм.в.ст./м],
    dp_obr                                        AS [Уд. потери (обратка), мм.в.ст./м],

    N'8–10 (при V=1.5–3.5 м/с)'                   AS [Уд. допуст. потери, мм.в.ст./м],

    -- Анализ потерь с учётом скорости
    CASE 
        WHEN v_pod < 1.5 THEN N'Ниже рекомендуемого диапазона'
        WHEN v_pod > 3.5 THEN N'Выше рекомендуемого диапазона'
        WHEN dp_pod < 8  THEN N'Ниже рекомендуемого диапазона'
        WHEN dp_pod > 10 THEN N'Выше рекомендуемого диапазона'
        ELSE N'В пределах нормы'
    END AS [Анализ потерь (подача)],

    CASE 
        WHEN v_obr < 1.5 THEN N'Ниже рекомендуемого диапазона'
        WHEN v_obr > 3.5 THEN N'Выше рекомендуемого диапазона'
        WHEN dp_obr < 8  THEN N'Ниже рекомендуемого диапазона'
        WHEN dp_obr > 10 THEN N'Выше рекомендуемого диапазона'
        ELSE N'В пределах нормы'
    END AS [Анализ потерь (обратка)]

FROM WithNorms

-- Только проблемные участки
WHERE
    v_pod NOT BETWEEN v_min_pod AND v_max_pod
    OR v_obr NOT BETWEEN v_min_obr AND v_max_obr
    OR (v_pod BETWEEN 1.5 AND 3.5 AND dp_pod NOT BETWEEN 8 AND 10)
    OR (v_obr BETWEEN 1.5 AND 3.5 AND dp_obr NOT BETWEEN 8 AND 10)

ORDER BY 
    -- Сначала самые критичные: скорость вне нормы
    CASE WHEN v_pod NOT BETWEEN v_min_pod AND v_max_pod THEN 1 ELSE 0 END 
    + CASE WHEN v_obr NOT BETWEEN v_min_obr AND v_max_obr THEN 1 ELSE 0 END DESC,
    ABS(v_pod - (v_min_pod + v_max_pod)/2) + ABS(v_obr - (v_min_obr + v_max_obr)/2) DESC,
    [Наименование участка];