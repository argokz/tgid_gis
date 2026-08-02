SELECT 

--pss.id,
pss.id,
temp.id1 AS '№ по порядку',
-- ec1.name,
-- n1.externalNodeName,
-- ec2.name,
-- n2.externalNodeName,

IIF (n1.nodeName='' OR n1.nodeName IS NULL, CONCAT(nt1.name, ' ', n1.externalNodeName), n1.nodeName) AS 'Начальный узел',
IIF (n2.nodeName='' OR n2.nodeName IS NULL, CONCAT(nt2.name, ' ', n2.externalNodeName), n2.nodeName) AS 'Конечный узел',
hps.diameterCondit AS 'Условный диаметр, мм',
hps.diameterExternal AS 'Наружный диаметр, мм',
hps.wallThickness AS 'Толщина стенки, мм',
temp.lengthP AS 'Протяженность по подаче, м',
temp.lengthO AS 'Протяженность по обратке, м',
tt.name AS 'Вид прокладки'

--temp.lengthO AS 'Общая протяженность, м'

FROM $vyd$ temp
JOIN nodes n1 ON n1.id=temp.n1
JOIN nodes n2 ON n2.id=temp.n2
LEFT JOIN nodeTypes nt1 ON nt1.id=n1.nodeTypeID
LEFT JOIN nodeTypes nt2 ON nt2.id=n2.nodeTypeID



JOIN linesobj l ON l.id = temp.ID2

--JOIN nodes n21 ON n21.id=l.nodeID1 AND n21.fileID = temp.fileID
--JOIN nodes n22 ON n22.id=l.nodeID2 AND n22.fileID = temp.fileID

JOIN nodes n21 ON n21.id=l.nodeID1
JOIN nodes n22 ON n22.id=l.nodeID2



JOIN externalCodes ec1 ON ec1.id=n1.externalCodeID
JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID
JOIN heatPipeSections hps ON hps.lineID=l.id
LEFT JOIN pipeSections pss ON hps.pipeSectionID=pss.id
left join tubingTypes tt on tt.id = hps.tubingTypeID



