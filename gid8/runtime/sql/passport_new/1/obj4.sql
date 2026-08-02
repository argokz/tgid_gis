with ll AS (
SELECT l.id,l.shape FROM linesobj l 
JOIN nodes n1 ON n1.id=l.nodeID1 
JOIN nodes n2 ON n2.id=l.nodeID2 
JOIN externalCodes ec1 ON ec1.id=n1.externalCodeID
JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID
--JOIN heatPipeSections hps ON hps.lineID=l.id

WHERE n1.removed=0 AND l.removed=0
AND n1.fileID < 1000
AND not (ec1.name in ('Ï1','Ï2') AND not ec2.name in ('Ï1','Ï2')) 
--AND hps.magistralSite=89

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


SELECT 
ec1.name,
n1.externalNodeName,
ec2.name,
n2.externalNodeName,

d1.v AS v1, 
d2.v AS v2, 
d3.v AS v3, 
d4.v AS v4, 
d5.v AS v5, 
d6.v AS v6, 
d7.v AS v7, 
d8.v AS v8, 
d9.v AS v9, 
d10.v AS v10, 
d11.v AS v11,


d.* FROM defect d
JOIN l_obj ON l_obj.obj_id=d.id
JOIN linesobj l ON l.id=l_obj.l_id

JOIN nodes n1 ON n1.id=l.nodeID1 
JOIN nodes n2 ON n2.id=l.nodeID2 
JOIN externalCodes ec1 ON ec1.id=n1.externalCodeID
JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID


LEFT JOIN (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM povrezhdennyiElementForDefect el
JOIN povrezhdennyiElement el2 ON el2.id=el.activityID
GROUP BY el.objID
) d1 ON d1.id=d.id

LEFT JOIN (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM prichinyPovrezhdeniaForDefect el
JOIN prichinyPovrezhdenia el2 ON el2.id=el.activityID
GROUP BY el.objID
) d2 ON d2.id=d.id

LEFT JOIN (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM prichiny_narusheniya_organizacionnye_for_defect el
JOIN prichiny_narusheniya_organizacionnye el2 ON el2.id=el.activityID
GROUP BY el.objID
) d3 ON d3.id=d.id


LEFT JOIN (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM soputstvuiushchiePrichinyForDefect el
JOIN soputstvuiushchiePrichiny el2 ON el2.id=el.activityID
GROUP BY el.objID
) d4 ON d4.id=d.id

LEFT JOIN (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM soputstvuiushchiePrichinyForDefect el
JOIN soputstvuiushchiePrichiny el2 ON el2.id=el.activityID
GROUP BY el.objID
) d5 ON d5.id=d.id

LEFT JOIN (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM sostKonstruktsiiKanalaForDefect el
JOIN sostKonstruktsiiKanala el2 ON el2.id=el.activityID
GROUP BY el.objID
) d6 ON d6.id=d.id


LEFT JOIN (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM sostKonstruktsiiKameryForDefect el
JOIN sostKonstruktsiiKamery el2 ON el2.id=el.activityID
GROUP BY el.objID
) d7 ON d7.id=d.id


LEFT JOIN (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM prichinyPovrezhdeniaForDefect el
JOIN prichinyPovrezhdenia el2 ON el2.id=el.activityID
GROUP BY el.objID
) d8 ON d8.id=d.id


LEFT JOIN (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM prichinyPovrezhdeniaForDefect el
JOIN prichinyPovrezhdenia el2 ON el2.id=el.activityID
GROUP BY el.objID
) d9 ON d9.id=d.id


LEFT JOIN (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM prichinyPovrezhdeniaForDefect el
JOIN prichinyPovrezhdenia el2 ON el2.id=el.activityID
GROUP BY el.objID
) d10 ON d10.id=d.id

LEFT JOIN (

SELECT 
objID AS id,
STRING_AGG(v, ', ') AS v

FROM (
SELECT 

objID, 
CONCAT(name, IIF(v IS NULL, '',  ' ('+v+')')) AS v

FROM (

SELECT 
dt.objID,rt.name, STRING_AGG(se.name, ', ') AS v
FROM defectTube dt
LEFT JOIN spisokElementov se ON se.id=dt.elementID
LEFT JOIN remontTruboprovodaSpisok rt ON rt.id=dt.activityID
where se.id IS NOT NULL OR rt.id IS NOT NULL
GROUP BY dt.objID,rt.ord,rt.name
) _
) _
GROUP BY objID

) d11 ON d11.id=d.id



--WHERE d1.id IS NOT NULL OR d2.id IS NOT NULL OR d3.id IS NOT NULL OR d4.id IS NOT NULL OR d5.id IS NOT NULL OR d6.id IS NOT NULL OR d7.id IS NOT NULL OR d8.id IS NOT NULL OR d9.id IS NOT NULL OR d10.id IS NOT NULL OR d11.id IS NOT NULL

