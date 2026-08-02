SELECT 
--n.id,
-- cst.name as sost, 
'ÄÐ' AS dr, 
ec.name  as kod,    n.externalNodeName  as  uzel,  
rc.name AS name_building, 
rc.schemeNum as cxema, 
rc.parallHeatersCountIndep as a19_co, 
rc.mixFactCoeff as uf,
rc.calcThrustLosHS as a7, 
rc.calcThrustLosAH as a8, 
rc.calcThrustLosAC as a9, 
rc.calcThrustLosFlow as a10, 
rc.calcThrustLosFlowCirc as a11, 
rc.calcThrustInWDO as a12, 
tss.name as a13, 
rc.diameterElevNozzle as a14,
rc.diameterThrotDiaph as a15, 
tcs.name as a17,
rc.parallHeatersCount1 as a18, rc.parallHeatersCount2 as a19, 
rc.calcThrustLosHeaters1 as a22, rc.calcThrustLosHeaters2 as a23, 
pdvil.name as pr_per_PD, rc.setPDonRegulator as p_per_PD

from realConsumers rc
join nodes n on n.id = rc.nodeID
left join externalCodes ec on ec.id = n.externalCodeID
left join externalSigns es on es.id = n.externalSignID
left join nodes ni on ni.id = n.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id
left join specExpends se on se.id = rc.specExpendID
left join pdValveInstallLocs pdvil on pdvil.id = rc.PDvalveInstallLocID

left join temperatureChartSigns tcs on tcs.id = rc.temperChartSignID
left join throtStageSigns tss on tss.id = rc.throtStageSignID

WHERE n.fileID=$fileID$ AND n.removed=0
