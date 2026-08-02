with obj AS (
SELECT id, 1 AS id2, shape FROM vozdushnik
UNION ALL
SELECT id, 2 AS id2, shape FROM drenazhnyy_kran
UNION ALL
SELECT id, 3 AS id2, shape FROM zapornaya_armatura
),

obj_names AS (
SELECT * FROM (VALUES 
    (1, 'Воздушник'),
    (2, 'Дренажный кран'),
    (3, 'Запорная арматура')
    ) obj_names(id, name) 
),


l_v AS (
SELECT 

v.shape.STStartPoint().STDistance(l.shape) AS d,
count(*) over(partition by l.id, v.id) AS cnt,
ROW_NUMBER() over(partition by l.id order by v.id2, v.id) AS numb,
--min(v.shape.STStartPoint.STDistance(l.shape)) over(partition by ps1.id, v.id),

n1.fileID,
l.id AS l_id,
v.id AS v_id,
v.id2 AS v_id2

FROM linesobj l
JOIN heatPipeSections hps ON hps.lineID=l.id
JOIN obj v ON v.shape.STDistance(l.shape) < 1
JOIN nodes n1 ON n1.id=l.nodeID1
JOIN nodes n2 ON n2.id=l.nodeID2

WHERE l.removed=0 AND n1.internalNodeID IS NULL 

AND n1.fileID IN (58)
AND hps.magistralSite=9

)

SELECT
l.id,
IIF(numb = 1, n1.externalNodeName, '') AS name1,
IIF(numb = 1, n2.externalNodeName, '') AS name2,
obj_names.name
--, numb
FROM l_v
JOIN obj_names ON obj_names.id=v_id2
JOIN linesobj l ON l.id=l_v.l_id
JOIN heatPipeSections hps ON hps.lineID=l.id
JOIN nodes n1 ON n1.id=l.nodeID1
JOIN nodes n2 ON n2.id=l.nodeID2

ORDER BY l.id, numb
--WHERE cnt > 1

--WHERE hps.magistralSite=9

--AND ps1.id IS NOT NULL


/*
SELECT 

v.shape.STDistance(l.shape),

rank() over(partition by ps1.id, v.id order by v.shape.STDistance(l.shape)),

min(v.shape.STDistance(l.shape)) over(partition by ps1.id, v.id),

ps1.id,
v.id,


* 


FROM linesobj l
JOIN heatPipeSections hps ON hps.lineID=l.id
JOIN nodes n1 ON n1.id=l.nodeID1
JOIN nodes n2 ON n2.id=l.nodeID2
LEFT JOIN pipeSections ps1 ON ps1.id=hps.pipeSectionID

JOIN vozdushnik v ON v.shape.STDistance(l.shape) < 1


WHERE l.removed=0 AND n1.internalNodeID IS NULL AND n1.fileID=58
AND 

--AND hps.magistralSite=9

--AND ps1.id IS NOT NULL
*/