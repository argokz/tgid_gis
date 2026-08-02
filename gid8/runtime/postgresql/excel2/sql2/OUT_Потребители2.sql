select 
  n.id,
  IIF(ISNULL(gc.consumerStateID, rc.consumerStateID)=1, ' ', 'закр') AS StateID,
  IIF(NOT gc.consumerStateID IS NULL, 'О', ' ') AS prizn,
  ec.name as kod, n.externalNodeName as uzel, 
--  es.name as pr, 
  rc.name AS name_building, 
  a4, 
  a5, 
  a6, 
--  a8, 
  a11, 
  a12, a13, a14, 
  a15, a16, a17, 
  a21, a22, 
  a23, 
  Gneob,
  hs.name AS kod_ist
--  ,
--  n.id
from PT_OUT
join nodes n on n.id = pt_out.nodeID
join externalCodes ec on ec.id = n.externalCodeID
join externalSigns es on es.id = n.externalSignID
left join generalizedConsumers gc ON gc.nodeID=n.id
left join realConsumers rc ON rc.nodeID=n.id

LEFT JOIN heatSources hs ON hs.id = ec.heatSourceID

WHERE n.fileID=$fileID$ AND calculationID=$calculationID$ AND n.removed=0

