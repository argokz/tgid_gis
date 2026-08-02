SELECT
n.id,
--ec.name AS "Код РС", 
--n.externalNodeName AS "Наименование узла", 
--ISNULL (rc.name, ISNULL (gc.name, n.nodeName)) AS "Описание",
n.NodeName AS "Наименование ПТС", 
nt.name,

fr.name AS "Фрагмент"

FROM nodes n
LEFT JOIN externalCodes ec ON ec.id=n.externalCodeID
LEFT JOIN fragments fr ON fr.id=n.fileID
JOIN nodeTypes nt ON nt.id=n.nodeTypeID

WHERE n.fileID IN ($fileID$) AND n.removed=0
AND n.internalNodeID IS NULL
--AND_PS

