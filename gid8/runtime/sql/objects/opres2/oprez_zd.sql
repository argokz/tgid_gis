SELECT DISTINCT 
zd_id,
n_id
FROM 
(
SELECT DISTINCT
zd.id AS zd_id,
PT.id AS n_id

FROM zapornaya_armatura zd
JOIN 
(

SELECT
n.id,
n.shape
FROM nodes n
JOIN fragments fr ON fr.id=n.fileID
LEFT JOIN generalizedConsumers gc ON gc.nodeID=n.id
JOIN linesobj l ON l.nodeID1=n.id OR l.nodeID2=n.id
JOIN opresDeployed dep ON dep.lineID=l.id
JOIN opres obj ON obj.id=dep.directionID

WHERE n.removed=0 AND n.internalNodeID IS NULL
AND obj.id=$id$
AND gc.id IS NOT NULL
AND n.fileID=58
) PT ON PT.shape.STDistance(zd.shape) < 5 AND PT.shape.STDistance(zd.shape) > 0.1

UNION
SELECT zd_id, n_id
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

    WHERE dep.directionID=$id$ AND l.removed=0
) _
) __
