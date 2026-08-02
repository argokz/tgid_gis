SELECT 
  n.id,
  ec.name as 'kod', n.externalNodeName as 'uzel', 
  
  case US_OUT.externalSign
    when 1 then 'Ï'
    when 2 then 'Î'
  end as 'pr',

  pih, t
from US_OUT
join nodes n on n.id = us_out.nodeID
join externalCodes ec on ec.id = n.externalCodeID
join externalSigns es on es.id = n.externalSignID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND calculationID=$calculationID$
AND n.removed=0 
