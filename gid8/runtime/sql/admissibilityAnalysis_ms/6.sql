WITH Data AS (
    SELECT
        n.id,
        n.externalNodeName,

        rc.calctemphr                                 AS [t в помещ., °C (расч.)],
        ptOut.tv                                      AS [t в помещ., °C (факт.)],
        rc.calcTempVS                                 AS [t нагрет. возд. в сист.вент., °C (исх.)],
        ptOut.dop8                                    AS [t нагрет. возд. в сист.вент.,°C(расч.)],
        usP.t                                         AS [t сет. воды, подача,°C],
        usO.t                                         AS [t сет. воды, обратка,°C],
        tg.t2                                         AS [t сет. воды, обратка,°C (темп. график)],
        ist2.name                                     AS [Источник теплоснабжения],

        -- Формируем единый текстовый анализ режима
        CONCAT(
            CASE WHEN ptOut.tv < rc.calctemphr - 0.5 
                 THEN N'Факт. температура в помещениях ниже расчетной, ' ELSE N'' END,

            CASE WHEN ptOut.tv > 1.05 * rc.calctemphr 
                 THEN N'Факт. температура в помещениях выше допустимой (больше 5% от расчетной), ' ELSE N'' END,

            CASE WHEN ptOut.dop8 IS NOT NULL AND ptOut.dop8 <> 0 
                      AND ptOut.dop8 < 0.9 * rc.calcTempVS
                 THEN N'Расч. температура нагретого воздуха в системах вентиляции ниже допустимой (более 10% от исходной), ' ELSE N'' END,

            CASE WHEN ptOut.dop8 IS NOT NULL AND ptOut.dop8 <> 0 
                      AND ptOut.dop8 > 1.1 * rc.calcTempVS
                 THEN N'Расч. температура нагретого воздуха в системах вентиляции выше допустимой (более 10% от исходной), ' ELSE N'' END,

            CASE WHEN ROUND(usO.t, 0) < tg.t2 - 0.5 
                 THEN N'В обратном трубопроводе температура сетевой воды ниже температурного графика, ' ELSE N'' END,

            CASE WHEN ROUND(usO.t, 0) > tg.t2 + 0.5 
                 THEN N'В обратном трубопроводе температура сетевой воды выше температурного графика, ' ELSE N'' END
        ) AS [Анализ режима]

    FROM nodes n
        INNER JOIN externalcodes ec on ec.id=n.externalcodeid
        INNER JOIN realConsumers rc ON rc.nodeID = n.id

        -- Последний расчёт
        OUTER APPLY (
            SELECT TOP 1 c.id AS last_calc_id, c.tn
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

        LEFT JOIN PT_OUT ptOut ON ptOut.nodeID = n.id 
                              AND ptOut.calculationID = calc.last_calc_id

        LEFT JOIN DR_OUT ON DR_OUT.nodeID = n.id 
                        AND DR_OUT.calculationID = calc.last_calc_id

        -- Температурный график источника
        LEFT JOIN deployedtempgraphs tg 
               ON tg.hsourceid = ec.heatsourceid --usP.ist 
              AND tg.tn = calc.tn

        LEFT JOIN heatSources ist2 ON ist2.id = ec.heatsourceid -- usP.ist

    WHERE n.removed = 0
      AND n.fileID = $fileID$
--      AND n.fileID = 5
      AND n.externalSignID = 1
      AND n.internalNodeID IS NULL
)
-- Финальный вывод только проблемных потребителей
SELECT
    id,
    externalNodeName,
    [t в помещ., °C (расч.)],
    [t в помещ., °C (факт.)],
    [t нагрет. возд. в сист.вент., °C (исх.)],
    [t нагрет. возд. в сист.вент.,°C(расч.)],
    [t сет. воды, подача,°C],
    [t сет. воды, обратка,°C],
    [t сет. воды, обратка,°C (темп. график)],
    [Источник теплоснабжения],
    LTRIM(RTRIM([Анализ режима])) AS [Анализ режима]

FROM Data
WHERE [Анализ режима] != '' 
   AND [Анализ режима] IS NOT NULL

ORDER BY 
    [Анализ режима] DESC,
    externalNodeName
