SELECT TOP 1000
n.id,
ec.name,
n.externalNodeName,
n.nodeName,
ecn.name AS ec_name,
nn.externalNodeName AS nn_name,


CASE
WHEN NOT rc.id IS NULL THEN 'Потребитель реальный'
WHEN NOT gc.id IS NULL THEN 'Потребитель обобщенный'
WHEN NOT pumpStations.id IS NULL THEN 'Насосная станция'
WHEN NOT hs.id IS NULL THEN 'Источник'
ELSE 'Узел'
END AS name_table,

fr.name AS fileID

--* 

FROM nodes n
LEFT JOIN externalCodes ec ON ec.id=n.externalCodeID
LEFT JOIN nodes nn ON nn.id=n.internalNodeID
LEFT JOIN externalCodes ecn ON ecn.id=nn.externalCodeID
LEFT JOIN fragments fr ON fr.id=n.fileID
LEFT JOIN realConsumers rc ON rc.nodeID=n.id
LEFT JOIN generalizedConsumers gc ON gc.nodeID =n.id
LEFT JOIN pumpStations ON pumpStations.nodeID=n.id
LEFT JOIN heatSources hs ON hs.nodeID = n.id
LEFT JOIN realConsumers rc2 ON rc2.nodeID = nn.id

WHERE /* n.fileID IN ($fileID$) AND */ rc2.id IS NULL AND n.removed=0


