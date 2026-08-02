SELECT 
objID AS id,
STRING_AGG(v, ', ') AS v

FROM (
SELECT 

objID, 
CONCAT(name, IIF(v IS NULL, '',  ' ('+v+')')) AS v

FROM (

SELECT 

dt.objID,
rt.name, 
STRING_AGG(se.name, ', ') AS v
FROM defectTube dt

LEFT JOIN spisokElementov se ON se.id=dt.elementID
LEFT JOIN remontTruboprovodaSpisok rt ON rt.id=dt.activityID

where se.id IS NOT NULL OR rt.id IS NOT NULL
GROUP BY 
dt.objID,
rt.ord,
rt.name

) _
) _
GROUP BY objID

