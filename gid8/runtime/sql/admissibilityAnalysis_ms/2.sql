SELECT 
    n.id,
    n.externalNodeName,
    usP.pih AS [Верхний доп. пьез.напор, м.вод.ст.],
    5 AS [Нижний доп. пьез.напор, м.вод.ст.],
    usO.pih AS [Полученный напор, м.вод.ст.],
    CASE 
        WHEN usO.pih <= usP.pih AND usO.pih >= 5                     THEN N'Режим допустим'
        WHEN usO.pih > usP.pih                                      THEN N'Превышает допустимый'
        WHEN usO.pih < 5                                            THEN N'Ниже допустимого'
        WHEN EXISTS (
            SELECT 1 
            FROM nodes n2 
            WHERE n2.removed = 0 
              AND n2.internalnodeid = n.id
        )                                                           THEN N'Узел с внутренней схемой'
        WHEN usP.pih IS NULL AND usO.pih IS NULL                    THEN N'Узел не подключен'
        WHEN usP.pih IS NULL AND usO.pih IS NOT NULL                THEN N'Только обратка'
        ELSE N'Нет данных'
    END AS [Режим]
FROM 
    nodes n
    LEFT JOIN realConsumers       rc  ON rc.nodeID  = n.id
    LEFT JOIN generalizedConsumers gc ON gc.nodeID  = n.id
    LEFT JOIN heatSources         ist ON ist.nodeID = n.id
    LEFT JOIN pumpStations        hs  ON hs.nodeID  = n.id
    LEFT JOIN threeWayValves      c3  ON c3.nodeID  = n.id
    LEFT JOIN connectNodes        us2 ON us2.nodeID = n.id

    -- Получаем ID последнего расчёта для файла
    OUTER APPLY (
        SELECT TOP 1 c.id AS last_calc_id
        FROM CALCULATION c
        LEFT JOIN fragments fr ON fr.id = c.fileID
        WHERE c.fileID = n.fileID
        ORDER BY c.id DESC
    ) calc

    LEFT JOIN US_OUT usP ON usP.nodeID       = n.id
                        AND usP.externalSign = 1
                        AND usP.calculationID = calc.last_calc_id

    LEFT JOIN US_OUT usO ON usO.nodeID       = n.id
                        AND usO.externalSign = 2
                        AND usO.calculationID = calc.last_calc_id

    LEFT JOIN PT_OUT      ON PT_OUT.nodeID       = n.id AND PT_OUT.calculationID = calc.last_calc_id
    LEFT JOIN DR_OUT      ON DR_OUT.nodeID       = n.id AND DR_OUT.calculationID = calc.last_calc_id

WHERE 
    n.removed = 0
    AND n.fileID = $fileID$                  -- или $fileID$, если используете в приложении
    AND n.internalNodeID IS NULL
    AND n.externalSignID IN (1, 3)

    -- Это не потребители, не источники, не насосные и т.д.
    AND rc.id  IS NULL
    AND gc.id  IS NULL
    AND ist.id IS NULL
    AND hs.id  IS NULL
    AND c3.id  IS NULL
    AND us2.id IS NULL

    -- Условия по напору (только проблемные узлы)
    AND (
        usO.pih IS NULL 
        OR usP.pih IS NULL 
        OR usO.pih > usP.pih 
        OR usO.pih < 5
    )
    AND NOT (usP.pih IS NOT NULL AND usO.pih IS NULL)   -- исключаем "только подача без обратки"
