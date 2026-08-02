SELECT *
FROM (
    SELECT
        n.id,
        n.externalNodeName,
        150 AS [Верхний доп. пьез.напор, м.вод.ст.],
        
        CASE 
    WHEN ROUND(0.535 * usP.t - 49.2, 2) < 0 THEN 0
    ELSE ROUND(0.535 * usP.t - 49.2, 2)
END AS [Нижний доп. пьез.напор, м.вод.ст.],
        
        usP.pih AS [Полученный напор, м.вод.ст.],
        usP.t AS [Температура сет. воды, Град.C],
        
        CASE
            WHEN usP.pih <= 150
                 AND usP.pih >= CASE 
                                    WHEN ROUND(0.535 * usP.t - 49.2, 2) < 0 
                                        THEN 0 
                                    ELSE ROUND(0.535 * usP.t - 49.2, 2) 
                                END
                THEN 'Режим допустим'
                
            WHEN usP.pih > 150
                THEN 'Превышает допустимый'
                
            WHEN usP.pih < CASE 
                               WHEN ROUND(0.535 * usP.t - 49.2, 2) < 0 
                                   THEN 0 
                               ELSE ROUND(0.535 * usP.t - 49.2, 2) 
                           END
                THEN 'Ниже допустимого'
                
            WHEN n.id IN (
                SELECT internalnodeid
                FROM nodes n2
                WHERE n2.removed = 0
                  AND n2.internalnodeid IS NOT NULL
            )
                THEN 'Узел с внутренней схемой'
                
            WHEN usP.pih IS NULL AND usO.pih IS NULL
                THEN 'Узел не подключен'
                
            ELSE 'Нет данных'
        END AS [Режим]

    FROM nodes n
    LEFT JOIN realConsumers rc          ON rc.nodeID = n.id
    LEFT JOIN generalizedConsumers gc   ON gc.nodeID = n.id
    LEFT JOIN heatSources ist           ON ist.nodeID = n.id
    LEFT JOIN pumpStations hs           ON hs.nodeID = n.id
    LEFT JOIN threeWayValves c3         ON c3.nodeID = n.id
    LEFT JOIN connectNodes us2          ON us2.nodeID = n.id
    
    LEFT JOIN (
        SELECT 
            c.fileID,
            MAX(c.id) AS cid
        FROM CALCULATION c
        LEFT JOIN fragments fr ON fr.id = c.fileID
        GROUP BY c.fileID
    ) calc ON calc.fileID = n.fileID
    
    LEFT JOIN US_OUT usP 
           ON usP.nodeID = n.id 
          AND usP.externalSign = 1 
          AND usP.calculationID = calc.cid
          
    LEFT JOIN US_OUT usO 
           ON usO.nodeID = n.id 
          AND usO.externalSign = 2 
          AND usO.calculationID = calc.cid
          
    LEFT JOIN PT_OUT 
           ON PT_OUT.nodeID = n.id 
          AND PT_OUT.calculationID = calc.cid
          
    LEFT JOIN DR_OUT 
           ON DR_OUT.nodeID = n.id 
          AND DR_OUT.calculationID = calc.cid
          
    WHERE n.removed = 0
      AND n.fileID = $fileID$                  -- или ? если используете параметр в ADO/SSMS
      AND n.internalNodeID IS NULL
      AND (n.externalSignID = 1 OR n.externalSignID = 2)
      AND rc.id IS NULL
      AND gc.id IS NULL
      AND ist.id IS NULL
      AND hs.id IS NULL
      AND c3.id IS NULL
      AND us2.id IS NULL
      AND (
            usP.t IS NULL 
         OR usP.pih IS NULL 
         OR usP.pih > 150 
         OR usP.pih < CASE 
                          WHEN ROUND(0.535 * usP.t - 49.2, 2) < 0 
                              THEN 0 
                          ELSE ROUND(0.535 * usP.t - 49.2, 2)
                      END
      )
      AND NOT (usP.pih IS NULL AND usO.pih IS NOT NULL)
) AS src