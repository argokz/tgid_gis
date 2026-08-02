SELECT TOP 1000 
zd_id AS id,
--n_id,
Узел,
[Признак трубопровода],
[Состояние],
[Номер],
[Диаметр условный, мм]
--, st.orderID

FROM (
SELECT DISTINCT
zd2_id AS zd_id,
n_id,
--IIF( qq=1, 'Боковой', 'На контуре') AS 'Положение',
uzel AS [Узел],
es_name AS 'Признак трубопровода',
states_name AS 'Состояние',
nomer AS 'Номер',
--primechanie  AS 'Примечание',
diametr AS 'Диаметр условный, мм'

FROM
(
SELECT
zd2.id AS zd2_id,
n_id,
qq, 
MIN(qq) OVER(PARTITION BY zd_id ) AS qq_min,

es.name AS es_name, 
zd2.primechanie,
zd2.nomer,
zd2.diametr,
IIF (n.nodeName is NULL or n.nodeName = '' or n.nodeName = ' ',n.externalNodeName, n.nodeName) as uzel,
states.name AS states_name

FROM 
(

SELECT DISTINCT
1 AS qq,  -- Боковой
zd_id,
n_id

 
FROM (
SELECT DISTINCT
zd.id AS zd_id,
PT.id AS n_id,
PT.shape.STDistance(zd.shape) AS length,
MIN(PT.shape.STDistance(zd.shape)) OVER(PARTITION BY zd.id ) AS "min_len"

FROM zapornaya_armatura zd
JOIN 
(

SELECT
n.id,
n.shape
FROM nodes n
JOIN fragments fr ON fr.id=n.fileID
LEFT JOIN generalizedConsumers gc ON gc.nodeID=n.id
JOIN linesobj l ON (l.nodeID1=n.id OR l.nodeID2=n.id) AND l.removed=0
JOIN opresDeployed dep ON dep.lineID=l.id

WHERE n.removed=0 AND n.internalNodeID IS NULL
AND dep.directionID=$ID$
--AND gc.id IS NOT NULL
AND n.fileID IN ($fileID$)
AND (gc.id IS NOT NULL OR (n.nodeName != '' OR n.nodeTypeID <> 0 AND n.nodeTypeID IS NOT NULL))

) PT ON PT.shape.STDistance(zd.shape) < 5
) PT
WHERE length=min_len

UNION
SELECT 2, zd_id, n_id  -- На контуре
FROM 
(
    SELECT DISTINCT
        zd.id AS zd_id,
        IIF(zd.shape.STDistance(n1.shape) < zd.shape.STDistance(n2.shape), n1.id, n2.id) AS n_id,
        l.id AS l_id,
        l.shape.STDistance(zd.shape) AS len,
        MIN(l.shape.STDistance(zd.shape)) OVER(PARTITION BY zd.id ) AS min_len
    FROM linesobj l
    JOIN zapornaya_armatura zd ON l.shape.STDistance(zd.shape) < 0.1
    JOIN opresDeployed dep ON dep.lineID=l.id
    JOIN nodes n1 ON n1.id=l.nodeID1
    JOIN nodes n2 ON n2.id=l.nodeID2

    WHERE dep.directionID=$ID$ AND l.removed=0
) _
) zd_n
JOIN zapornaya_armatura zd2 ON zd2.id=zd_n.zd_id
LEFT JOIN damperArmatureStates states ON states.id=zd2.stateID
LEFT JOIN nodes n ON n.id=zd_n.n_id
LEFT JOIN externalSigns es ON es.id=zd2.priznak_truboprovoda

) _



WHERE qq =qq_min

) _


ORDER BY zd_id

-- LEFT JOIN sortNodesForUchastok st ON st.nodeID=n_id
-- ORDE0R BY orderID
