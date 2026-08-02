WITH Consumers AS (
    SELECT
        n.id,
        n.externalNodeName,
        usP.pih AS pihP,        -- напор на подаче
        usO.pih AS pihO,        -- напор на обратке
        rc.schemeNum,
        rc.buildHeight,

        -- Определяем тип схемы подключения
        CASE 
            WHEN rc.schemeNum IN ('1.5', '1.6')                                  THEN N'Независимая'
            WHEN rc.schemeNum IN ('1.3', '1.4') 
                 OR TRY_CAST(RIGHT(rc.schemeNum, LEN(rc.schemeNum) - CHARINDEX('.', rc.schemeNum)) AS INT) IN (1,2,3,4,5,6,7,8) 
                                                                                 THEN N'Зависимая'
            WHEN TRY_CAST(RIGHT(rc.schemeNum, LEN(rc.schemeNum) - CHARINDEX('.', rc.schemeNum)) AS INT) IN (9,10,11,12) 
                                                                                 THEN N'Независимая'
            ELSE N'Неопределённо'
        END AS тип_схемы,

        PT_OUT.Gneob AS Hnr,
        (usP.pih - usO.pih) AS Hpr,

        -- Нижний допустимый пьезонапор (высота здания + 5 м)
        (rc.buildHeight + 5.0) AS Hpr_min,

        -- Верхний допустимый пьезонапор в зависимости от схемы
        CASE 
            WHEN rc.schemeNum IN ('1.5', '1.6')                                  THEN 100
            WHEN rc.schemeNum IN ('1.3', '1.4')
                 OR TRY_CAST(RIGHT(rc.schemeNum, LEN(rc.schemeNum) - CHARINDEX('.', rc.schemeNum)) AS INT) IN (1,2,3,4,5,6,7,8) 
                                                                                 THEN 60
            WHEN TRY_CAST(RIGHT(rc.schemeNum, LEN(rc.schemeNum) - CHARINDEX('.', rc.schemeNum)) AS INT) IN (9,10,11,12) 
                                                                                 THEN 100
            ELSE NULL
        END AS Hpr_max

    FROM nodes n
        INNER JOIN realConsumers rc ON rc.nodeID = n.id

        LEFT JOIN generalizedConsumers gc ON gc.nodeID = n.id
        LEFT JOIN heatSources         ist ON ist.nodeID = n.id
        LEFT JOIN pumpStations         hs ON hs.nodeID  = n.id
        LEFT JOIN threeWayValves       c3 ON c3.nodeID  = n.id
        LEFT JOIN connectNodes        us2 ON us2.nodeID = n.id

        -- Самый свежий расчёт по fileID
        OUTER APPLY (
            SELECT TOP 1 c.id AS last_calc_id
            FROM CALCULATION c
            WHERE c.fileID = n.fileID
            ORDER BY c.id DESC
        ) calc

        LEFT JOIN US_OUT usP ON usP.nodeID = n.id 
                            AND usP.externalSign = 1 
                            AND usP.calculationID = calc.last_calc_id

        LEFT JOIN US_OUT usO ON usO.nodeID = n.id 
                            AND usO.externalSign = 2 
                            AND usO.calculationID = calc.last_calc_id

        LEFT JOIN PT_OUT ON PT_OUT.nodeID = n.id 
                        AND PT_OUT.calculationID = calc.last_calc_id

        LEFT JOIN DR_OUT ON DR_OUT.nodeID = n.id 
                        AND DR_OUT.calculationID = calc.last_calc_id

    WHERE 
        n.removed = 0
        AND n.fileID = $fileID$
        AND n.externalSignID = 1
        AND n.internalNodeID IS NULL
)
-- Основной SELECT с определением режима
SELECT
    id,
    externalNodeName,
    pihO AS [Получ. напор на подаче, м.вд.ст.],
    pihP AS [Получ. напор на обратке, м.вд.ст.],
    Hpr  AS [Полученный расп. напор, м.вд.ст.],
    Hpr_min AS [Нижний доп. пьез.напор, м.вод.ст.],
    Hpr_max AS [Верхний доп. пьез.напор, м.вод.ст.],
    тип_схемы AS [Тип Схемы],
    CASE 
        WHEN pihP IS NULL AND pihO IS NULL                                      THEN N'Потребитель не подключен'
        WHEN pihP IS NULL OR pihO IS NULL                                       THEN N'Недостаточно данных'
        WHEN pihP < pihO                                                        THEN N'Отрицательные перепады'
        WHEN Hpr BETWEEN ISNULL(Hpr_min, -999999) AND ISNULL(Hpr_max, 999999)   THEN N'допустимо'
        WHEN Hpr > Hpr_max                                                      THEN N'Выше допустимого предела'
        WHEN Hpr < Hpr_min                                                      THEN N'Ниже допустимого предела'
        ELSE                                                                      N'недопустимо'
    END AS [Режим напора потребителя]

FROM Consumers
WHERE
    -- Показываем только проблемные узлы + те, где нет данных
    (pihP IS NULL OR pihO IS NULL)
    OR (Hpr < Hpr_min)
    OR (Hpr > ISNULL(Hpr_max, Hpr + 1))  -- если Hpr_max = NULL, то считаем превышением
ORDER BY 
    [Режим напора потребителя], externalNodeName
