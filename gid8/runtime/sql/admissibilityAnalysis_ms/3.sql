SELECT 
    n.id,
    n.externalNodeName,
    usP.pih AS [Получ. напор на подаче, м.вд.ст.],
    usO.pih AS [Получ. напор на обратке, м.вд.ст.],
    PT_OUT.Gneob AS [Необходимый расп. напор, м.вд.ст.],
    (usP.pih - usO.pih) AS [Полученный расп. напор, м.вд.ст.],
    CASE 
        WHEN (usP.pih - usO.pih) >= ISNULL(PT_OUT.Gneob, 0) 
             AND (usP.pih - usO.pih) >= 0                                 THEN N'Режим допустим'
        WHEN usP.pih IS NULL AND usO.pih IS NULL                            THEN N'Потребитель не подключен'
        WHEN usP.pih IS NOT NULL AND usO.pih IS NOT NULL AND usP.pih < usO.pih THEN N'Отрицательные перепады'
        WHEN (usP.pih - usO.pih) < ISNULL(PT_OUT.Gneob, 0)                   THEN N'Ниже допустимого'
        ELSE N'Нет данных по напору'
    END AS [Режим]
FROM nodes n
    INNER JOIN realConsumers rc ON rc.nodeID = n.id  -- только реальные потребители

    LEFT JOIN generalizedConsumers gc ON gc.nodeID = n.id
    LEFT JOIN heatSources         ist ON ist.nodeID = n.id
    LEFT JOIN pumpStations        hs  ON hs.nodeID  = n.id
    LEFT JOIN threeWayValves      c3  ON c3.nodeID  = n.id
    LEFT JOIN connectNodes       us2  ON us2.nodeID = n.id

    -- Последний расчёт по fileID (быстрее и надёжнее, чем GROUP BY)
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
    AND n.fileID = $fileID$                  -- или $fileID$ в вашем фреймворке
    AND n.internalNodeID IS NULL
    AND n.externalSignID = 1
    AND rc.id IS NOT NULL                   -- только реальные потребители

    -- Показываем только проблемные случаи:
    AND (
        usP.pih IS NULL OR usO.pih IS NULL                                          -- нет данных хотя бы по одной трубе
        OR usP.pih < usO.pih                                                             -- обратка выше подачи
        OR (usP.pih - usO.pih) < ISNULL(PT_OUT.Gneob, 0)                             -- недостаточный перепад
    )
