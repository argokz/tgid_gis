SELECT DISTINCT
n.id,
n.NodeName AS 'Наименование узла ПТС',

ec.name AS 'Код РС', 
n.externalNodeName AS 'Наименование узла',
--gc.name  AS 'Описание',
--IIF (ms.id IS NOT NULL, ms.opisanie_uchastka_ms,rs.naimenovanie_uchastka_rs) AS 'Участок',
--o.name AS 'Владелец',
fr.name AS 'Фрагмент',
srt.orderID AS '#'

FROM nodes n
LEFT JOIN sortNodesForUchastok srt ON srt.nodeID=n.id
JOIN externalCodes ec ON ec.id=n.externalCodeID
JOIN fragments fr ON fr.id=n.fileID
LEFT JOIN generalizedConsumers gc ON gc.nodeID=n.id
JOIN linesobj l ON l.nodeID1=n.id OR l.nodeID2=n.id
JOIN opresDeployed dep ON dep.lineID=l.id
JOIN opres obj ON obj.id=dep.directionID
LEFT JOIN heatPipeSections hps ON hps.lineID=l.id
--LEFT JOIN uchastok_rs rs ON rs.id=hps.distSite
--LEFT JOIN uchastok_ms ms ON ms.id=hps.magistralSite
LEFT JOIN organizations o ON o.id=l.organizationID

WHERE n.removed=0 AND n.internalNodeID IS NULL AND n.nodeName <> ''
AND obj.id=$ID$
ORDER BY srt.orderID
