SELECT top 2147483647 
n.id,
eci.name as 'kod_p', ni.externalNodeName as 'uzel_p', esi.name as 'pr_p',
ec.name as 'kod',    n.externalNodeName as 'uzel'   , es.name as 'pr'

from connectNodes ps

join nodes n on n.id = ps.nodeID
left join externalCodes ec on ec.id = n.externalCodeID
left join externalSigns es on es.id = n.externalSignID

left join nodes ni on ni.id = n.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id



WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND n.removed=0
