SELECT
    n.id,
    ec.name as kod, n.externalNodeName as uzel, es.name as pr, 
    pressFlow as h_p, pressRet as h_o, kod_m, uzel_m, 
    fr.name AS "Фрагмент присоединения"
from setPressNodes spn
join nodes n on n.id = spn.nodeID
join externalCodes ec on ec.id = n.externalCodeID
join externalSigns es on es.id = n.externalSignID

LEFT JOIN fragments fr on fr.id = spn.fragment_resultID


WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND n.removed=0
