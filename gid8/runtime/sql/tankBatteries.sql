SELECT top 2147483647 
tb.id,
tb.designCapacity,
tb.quantity,
tb.height,
tb.diameter,
eci.name AS 'Код РС',
n.externalNodeName AS 'Имя узла'

FROM tankBatteries tb
LEFT JOIN nodes n ON tb.nodeID=n.id
left join externalCodes eci on n.externalCodeID = eci.id
