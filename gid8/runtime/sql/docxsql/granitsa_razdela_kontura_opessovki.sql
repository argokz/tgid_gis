SELECT DISTINCT
zd.id,
IIF (n.nodeName is NULL or n.nodeName = '' or n.nodeName = ' ',n.externalNodeName, n.nodeName) as 'Узел',
es.name AS 'Признак трубопровода',
st.name AS 'Состояние',
zd.nomer2 AS 'Номер',
zd.diametr AS 'Диаметр условный, мм',
zd.primechanie AS 'Примечание'

FROM zapornaya_armatura zd
JOIN nodes n ON n.id=zd.nodeID_pts

JOIN linesobj l ON n.id IN (l.nodeID1, l.nodeID2)
JOIN opresDeployed dep ON dep.lineID=l.id 
LEFT JOIN damperArmatureStates st ON st.id=zd.stateID
LEFT JOIN externalSigns es ON es.id=zd.priznak_truboprovoda
WHERE dep.directionID=$fileID$ AND zd.stateID=2

