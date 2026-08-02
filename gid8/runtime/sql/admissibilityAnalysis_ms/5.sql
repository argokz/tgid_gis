WITH Base AS (
    SELECT
        n.id                    AS node_id,
        n.externalNodeName,
        rc.id                   AS consumer_id,
        ptOut.QFact,
        ptOut.QTreb,
        ptOut.Q_obesp_otopl_min
    FROM nodes n
        INNER JOIN realConsumers rc ON rc.nodeID = n.id
        OUTER APPLY (
            SELECT TOP 1 c.id AS last_calc_id
            FROM CALCULATION c
            WHERE c.fileID = n.fileID
            ORDER BY c.id DESC
        ) calc
        LEFT JOIN PT_OUT ptOut 
               ON ptOut.nodeID = n.id 
              AND ptOut.calculationID = calc.last_calc_id
    WHERE n.removed = 0
      AND n.fileID = $fileID$
      AND n.externalSignID = 1
      AND n.internalNodeID IS NULL
),
Aggregated AS (
    SELECT
        node_id,
        externalNodeName,
        consumer_id,

        SUM(QFact)  OVER (PARTITION BY consumer_id) AS q1_cons,
        SUM(QTreb)  OVER (PARTITION BY consumer_id) AS q2_cons,

        SUM(QFact)  OVER () AS q1_total,
        SUM(QTreb)  OVER () AS q2_total,

        -- тут убираем ptOut.
        Q_obesp_otopl_min AS pct_cons_raw
    FROM Base
)
SELECT
    node_id                  AS [id],
    externalNodeName         AS [Наименование узла],

    q1_cons                  AS [Получ. тепл. нагрузка ТП, Гкал/ч.],
    q2_cons                  AS [Треб. тепл. нагрузка ТП, Гкал/ч.],

    pct_cons_raw             AS [Теплообесп.СО, %],

    CASE 
        WHEN pct_cons_raw IS NULL                  THEN N'Нет данных'
        WHEN pct_cons_raw < 70                     THEN N'Дефицит тепла'
        WHEN pct_cons_raw > 100                    THEN N'Перерасход тепла'
        ELSE                                              N'Норма'
    END AS [Режим (отд. потреб.)],

    q1_total                 AS [Сумм. получ. тепл. нагрузка, Гкал/ч.],
    q2_total                 AS [Сумм. треб. тепл. нагрузка, Гкал/ч.],

    ROUND(
        100.0 * q1_total / NULLIF(q2_total, 0), 2
    ) AS [Теплообесп. совокуп. потреб., %],

    CASE 
        WHEN q1_total IS NULL OR q2_total IS NULL OR q2_total = 0 THEN N'Нет данных'
        WHEN 100.0 * q1_total / q2_total < 70                      THEN N'Дефицит тепла'
        WHEN 100.0 * q1_total / q2_total > 100                     THEN N'Перерасход тепла'
        ELSE                                                               N'Норма'
    END AS [Режим (совокупно)]

FROM Aggregated
WHERE pct_cons_raw IS NULL
   OR pct_cons_raw < 70
   OR pct_cons_raw > 100
ORDER BY 
    [Режим (отд. потреб.)] DESC,
    pct_cons_raw ASC,
    externalNodeName;