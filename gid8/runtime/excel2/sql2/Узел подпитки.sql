SELECT top 2147483647
    n.id,
    ec.name as 'kod', n.externalNodeName as 'uzel', es.name as 'pr', 
    diameterInternal as 'diam', refillExpend as 'r_p', WDO as 'r_v', refillLoss as 'r_ut', waterVolUp as 'urov_v',
    waterVolDown as 'urov_n', waterVolUpSet as 'urov_z', potsCount as 'kol', potsSumVol as 'v_sum', potWorkingSign as 'prz_r',
    chargeExpend as 'r_z', dischargeExpend as 'r_r', setPressRet as 'napor', n.fileID
from refillNodes rn
join nodes n on n.id = rn.nodeID
join externalCodes ec on ec.id = n.externalCodeID
join externalSigns es on es.id = n.externalSignID

WHERE n.fileID=$fileID$ AND n.removed=0