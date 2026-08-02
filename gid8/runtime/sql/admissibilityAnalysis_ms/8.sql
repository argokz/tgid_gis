WITH Data AS (
    SELECT
        n.id,
        n.externalNodeName,

        usP.pih                                          AS [Получ. напор на подаче, м],
        usO.pih                                          AS [Получ. напор на обратке, м],

        -- Расчётная тепловая нагрузка Qo = отопление + ГВС + вентиляция (dop22/dop23)
        COALESCE(ptOut.qsum_z, 0) + COALESCE(ptOut.dop22, 0) + COALESCE(ptOut.dop23, 0) 
                                                         AS [Расч. тепл. нагрузка, Гкал/ч],

        -- Полученный располагаемый напор
        (usP.pih - usO.pih)                              AS [Полученный расп. напор, м]

    FROM nodes n
        INNER JOIN generalizedConsumers gc ON gc.nodeID = n.id   -- только обобщённые потребители

        -- Последний расчёт по fileID (самый быстрый способ)
        OUTER APPLY (
            SELECT TOP 1 c.id AS last_calc_id
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

        LEFT JOIN PT_OUT ptOut 
               ON ptOut.nodeID = n.id 
              AND ptOut.calculationID = calc.last_calc_id

    WHERE n.removed = 0
      AND n.fileID = $fileID$
      AND n.externalSignID = 1
      AND n.internalNodeID IS NULL
)
SELECT
    id,
    externalNodeName,
    [Получ. напор на подаче, м],
    [Получ. напор на обратке, м],
    [Расч. тепл. нагрузка, Гкал/ч],
    [Полученный расп. напор, м],

    -- Определяем требуемый диапазон и проверяем нарушение
    CASE
        WHEN [Расч. тепл. нагрузка, Гкал/ч] <= 10 
             AND [Полученный расп. напор, м] NOT BETWEEN 20 AND 25 
             THEN 'При Qo<=10, диапазон [20–25]'

        WHEN [Расч. тепл. нагрузка, Гкал/ч] > 10 AND [Расч. тепл. нагрузка, Гкал/ч] <= 20
             AND [Полученный расп. напор, м] NOT BETWEEN 30 AND 35 
             THEN 'При 10<Qo<=20, диапазон [30–35]'

        WHEN [Расч. тепл. нагрузка, Гкал/ч] > 20 AND [Расч. тепл. нагрузка, Гкал/ч] <= 30
             AND [Полученный расп. напор, м] NOT BETWEEN 40 AND 45 
             THEN 'При 20<Qo<=30, диапазон [40–45]'

        WHEN [Расч. тепл. нагрузка, Гкал/ч] > 30
             AND [Полученный расп. напор, м] NOT BETWEEN 45 AND 50 
             THEN 'При Qo>30, диапазон [45–50]'

        ELSE N'Допустимо'
    END AS [Необх. распол. напор, м (анализ)]

FROM Data

-- Фильтруем только те, где действительно есть нарушение
WHERE
    ([Расч. тепл. нагрузка, Гкал/ч] <= 10  AND [Полученный расп. напор, м] NOT BETWEEN 20 AND 25) OR
    ([Расч. тепл. нагрузка, Гкал/ч] > 10  AND [Расч. тепл. нагрузка, Гкал/ч] <= 20  AND [Полученный расп. напор, м] NOT BETWEEN 30 AND 35) OR
    ([Расч. тепл. нагрузка, Гкал/ч] > 20  AND [Расч. тепл. нагрузка, Гкал/ч] <= 30  AND [Полученный расп. напор, м] NOT BETWEEN 40 AND 45) OR
    ([Расч. тепл. нагрузка, Гкал/ч] > 30  AND [Полученный расп. напор, м] NOT BETWEEN 45 AND 50)

ORDER BY 
    [Расч. тепл. нагрузка, Гкал/ч] DESC,
    ABS([Полученный расп. напор, м] - 
        CASE 
            WHEN [Расч. тепл. нагрузка, Гкал/ч] <= 10 THEN 22.5
            WHEN [Расч. тепл. нагрузка, Гкал/ч] <= 20 THEN 32.5
            WHEN [Расч. тепл. нагрузка, Гкал/ч] <= 30 THEN 42.5
            ELSE 47.5
        END) DESC  -- сначала самые "далекие" от нормы