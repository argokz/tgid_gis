select * FROM (
select 

STUFF (
(
select concat(', ', name)  AS name 
from prichinyPovrezhdeniaForDefect el1
JOIN prichinyPovrezhdenia el2 ON el2.id = el1.activityID
WHERE el1.objID=d.id FOR XML PATH(''), TYPE
).value('.', 'NVARCHAR(MAX)')

, 1, 1, '') AS d1

FROM defect d

) __

where d1 IS NOT NULL
