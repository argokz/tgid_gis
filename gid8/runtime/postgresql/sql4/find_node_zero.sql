SELECT
n.id, 

ec.name AS "Код РС", 
n.externalNodeName AS "Наименование узла", 
ISNULL (rc.name, ISNULL (gc.name, n.nodeName)) AS "Описание",
--n.NodeName AS "Наименование ПТС", 

IIF (ps.id IS NULL,
IIF (hs.id IS NULL,
IIF (rc.id IS NULL,
IIF (gc.id IS NULL,
N'Узел'
,N'Обобщенный потребитель')
,N'Реальный потребитель')
,N'Источник')
,N'Насосная станция')

 AS "Тип",

fr.name AS "Фрагмент"

FROM nodes n
LEFT JOIN externalCodes ec ON ec.id=n.externalCodeID

LEFT JOIN realConsumers rc ON rc.nodeID = n.id
LEFT JOIN generalizedConsumers gc ON gc.nodeID = n.id
LEFT JOIN heatSources hs ON hs.nodeID = n.id
LEFT JOIN pumpStations ps ON ps.nodeID = n.id

LEFT JOIN fragments fr ON fr.id=n.fileID
WHERE n.fileID IN ($fileID$) AND n.removed=0 AND n.internalNodeID IS NULL
AND n.geoMarkTopTube = 0
--ORDER BY 
--    ec.name,
--    n.externalNodeName

