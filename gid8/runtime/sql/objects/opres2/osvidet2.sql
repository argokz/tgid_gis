SELECT 

id,
u1 AS 'Начальный узел',
u2 AS 'Конечный узел',

DiamUslov AS 'Диаметр условный, мм',
pipeLength AS 'Протяженность, м',
typ_pr AS 'Тип прокладки'


 
FROM 
(

SELECT 
DISTINCT
ps1.id,
IIF (n1.nodeName='' OR n1.nodeName IS NULL, CONCAT(nt1.name, ' ', n1.externalNodeName), n1.nodeName) AS u1,
IIF (n2.nodeName='' OR n2.nodeName IS NULL, CONCAT(nt2.name, ' ', n2.externalNodeName), n2.nodeName) AS u2,

ps1.DiamUslov,
ps1.pipeLength,
tt.name AS typ_pr


,srt.orderID

FROM $opres$ obj
JOIN $opres$Deployed d ON d.directionID=obj.id
JOIN heatPipeSections hps ON hps.lineID=d.lineID
JOIN pipeSections ps1 ON ps1.id=hps.pipeSectionID
--JOIN linesobj l ON l.id=d.lineID


LEFT JOIN sortLinesForUchastok srt ON ps1.id = srt.pipeSectionID	

LEFT JOIN tubingTypes tt ON ps1.tubingTypeID = tt.id

JOIN nodes n1 ON n1.id=ps1.nodeID1
JOIN nodes n2 ON n2.id=ps1.nodeID2
LEFT JOIN nodeTypes nt1 ON nt1.id=n1.nodeTypeID
LEFT JOIN nodeTypes nt2 ON nt2.id=n2.nodeTypeID


WHERE obj.id=$m_opr_id$

) _QQ


ORDER BY orderID
