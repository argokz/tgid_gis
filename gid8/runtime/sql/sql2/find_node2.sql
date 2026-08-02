SELECT TOP 1000
n.id, 

ec.name AS 'Код', 
n.externalNodeName AS 'Наименование', 
ISNULL (rc.name, n.nodeName) AS 'Наименование ПТС',

IIF (ps.id IS NULL,
IIF (hs.id IS NULL,
IIF (rc.id IS NULL,
IIF (gc.id IS NULL,
'Узел'
,'Обобщенный потребитель')
,'Реальный потребитель')
,'Источник')
,'Насосная станция')

 AS 'Тип',

ec2.name AS 'Код 2', 
n2.externalNodeName AS 'Наименование 2',
fr.name AS 'Фрагмент'

FROM nodes n
LEFT JOIN externalCodes ec ON ec.id=n.externalCodeID
LEFT JOIN nodes n2 ON n2.id=n.internalNodeID
LEFT JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID

LEFT JOIN realConsumers rc ON rc.nodeID = n.id
LEFT JOIN generalizedConsumers gc ON gc.nodeID = n.id
LEFT JOIN heatSources hs ON hs.nodeID = n.id
LEFT JOIN pumpStations ps ON ps.nodeID = n.id


LEFT JOIN realConsumers rc2 ON rc2.nodeID = n2.id


LEFT JOIN fragments fr ON fr.id=n.fileID
WHERE n.fileID IN ($fileID$) AND rc2.id IS NULL AND n.removed=0


