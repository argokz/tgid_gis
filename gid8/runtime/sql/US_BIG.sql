SELECT top 2147483647 
n.id,
temp.id1 AS '№ по порядку',
ec.name AS 'Код РС',
n.externalNodeName AS 'Наименование узла службы режимов',
n.nodeName AS 'Наименование узла ПТС',
nt.name AS 'Конструкция узла'

FROM $vyd$ temp
JOIN nodes n ON n.id=temp.ID2 
-- AND n.fileID = temp.fileID
JOIN externalCodes ec ON ec.id=n.externalCodeID
LEFT JOIN nodeTypes nt ON nt.id=n.nodeTypeID

ORDER BY temp.id1
