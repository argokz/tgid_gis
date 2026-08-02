SELECT
    n.id,
    n.externalNodeName,
    usP.t AS [t сет. воды, подача,°C],
    usO.t AS [t сет. воды, обратка,°C],
    tg.t2 AS [t сет. воды, обратка,°C (темп. график)],
    ist2.name AS [Источник теплоснабжения],

    -- Анализ отклонения от температурного графика
    CONCAT(
        CASE 
            WHEN ROUND(usO.t, 0) < tg.t2 - 0.5 
            THEN N'Температура в обратке ниже графика на ' + 
                 FORMAT(ABS(ROUND(usO.t, 0) - tg.t2), '0.0') + 
                 N'°C, ' 
            ELSE N'' 
        END,
        CASE 
            WHEN ROUND(usO.t, 0) > tg.t2 + 0.5 
            THEN N'Температура в обратке выше графика на ' + 
                 FORMAT(ABS(ROUND(usO.t, 0) - tg.t2), '0.0') + 
                 N'°C' 
            ELSE N'' 
        END
    ) AS [Анализ режима]

FROM nodes n
    INNER JOIN externalcodes ec on ec.id=n.externalcodeid
    INNER JOIN generalizedConsumers gc ON gc.nodeID = n.id  -- только обобщённые потребители

    -- Последний расчёт (самый быстрый способ)
    OUTER APPLY (
        SELECT TOP 1 c.id AS last_calc_id, c.tn
        FROM CALCULATION c
        WHERE c.fileID = n.fileID
        ORDER BY c.id DESC
    ) calc

    LEFT JOIN US_OUT usP 
           ON usP.nodeID = n.id 
          AND usP.externalSign = 1 
          AND usP.calculationID = calc.last_calc_id

    LEFT JOIN US_OUT usO 
           ON usO.nodeID = n.id 
          AND usO.externalSign = 2 
          AND usO.calculationID = calc.last_calc_id

    -- Температурный график источника
--    LEFT JOIN deployedtempgraphs tg 
--           ON tg.hsourceid = ec.heatsourceid --usP.ist 
--          AND tg.tn = calc.tn

    -- Температурный график источника
    LEFT JOIN 
    (
    SELECT *, ROW_NUMBER()  OVER (PARTITION BY hsourceid, tn ORDER BY hsourceid, tn) as rn FROM  deployedtempgraphs
    ) tg
           ON tg.hsourceid = ec.heatsourceid --usP.ist 
          AND tg.tn = calc.tn
          AND tg.rn=1




    LEFT JOIN heatSources ist2 ON ist2.id = ec.heatsourceid -- usP.ist

WHERE 
    n.removed = 0
    AND n.fileID = $fileID$
    AND n.externalSignID = 1
    AND n.internalNodeID IS NULL

    -- Показываем только проблемные узлы с отклонением от графика
    AND usP.t IS NOT NULL 
    AND usO.t IS NOT NULL 
    AND tg.t2 IS NOT NULL
    AND (ROUND(usO.t, 0) < tg.t2 - 0.5 OR ROUND(usO.t, 0) > tg.t2 + 0.5)

ORDER BY 
    ABS(ROUND(usO.t, 0) - tg.t2) DESC,  -- сначала самые большие отклонения
    externalNodeName

