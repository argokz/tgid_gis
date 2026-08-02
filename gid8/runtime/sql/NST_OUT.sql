SELECT top 2147483647 
  n.id,
  ec.name as 'kod1', n.externalNodeName as 'uzel1', es.name as 'pr1', 
  pih1, pih2, ras, dP
from NST_OUT
join nodes n on n.id = nst_out.nodeID
join externalCodes ec on ec.id = n.externalCodeID
join externalSigns es on es.id = n.externalSignID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND calculationID=$calculationID$
AND n.removed=0 

