with ll AS (
SELECT l.id,l.shape FROM linesobj l 
JOIN nodes n1 ON n1.id=l.nodeID1 
JOIN nodes n2 ON n2.id=l.nodeID2 
JOIN externalCodes ec1 ON ec1.id=n1.externalCodeID
JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID
JOIN heatPipeSections hps ON hps.lineID=l.id

WHERE n1.removed=0 AND l.removed=0
AND n1.fileID < 1000
AND not (ec1.name in ('Ï1','Ï2') AND not ec2.name in ('Ï1','Ï2')) 
AND hps.magistralSite=89

),

l_obj1 AS (

SELECT 
ll.id AS l_id,
d.id AS obj_id,
ll.shape.STDistance(d.shape.STStartPoint()) AS len,
min(ll.shape.STDistance(d.shape.STStartPoint())) OVER(PARTITION BY d.id) AS minlen

FROM defect d 
JOIN  ll ON ll.shape.STDistance(d.shape.STStartPoint()) < 1
--ORDER BY l_id, obj_id
),

l_obj2 AS (
SELECT 
COUNT(*) OVER(PARTITION BY obj_id) AS cnt,
ROW_NUMBER() OVER(PARTITION BY obj_id ORDER BY l_id) AS rown,
* FROM l_obj1
WHERE len=minlen
),

l_obj AS (
SELECT l_id, obj_id FROM l_obj2
WHERE rown = 1
)

SELECT l_id, obj_id FROM l_obj
--ORDER BY obj_id, l_id


--JOIN defects d 