SELECT 
n.id,
ec.name  as kod,    n.externalNodeName  as  uzel, 
rc.name AS name_building, 
rc.calcHLdep as otoplz, 
rc.calcHLindep as otopln, 
rc.calcHLventil+rc.avgHLcond as ventil, 

rc.avgHLGVScloseParall+rc.avgHLGVScloseMix+rc.avgHLGVScloseConseq+rc.avgHLGVSclosePreon as gvz, 

rc.avgHLGVSopenFlow as gvop, 
rc.avgHLGVSopenRet as gvoo, 
rc.circHLosOpen as rez, 

rc.calcHLdep+rc.calcHLindep+rc.calcHLventil+rc.avgHLcond+
rc.avgHLGVScloseParall+rc.avgHLGVScloseMix+rc.avgHLGVScloseConseq+rc.avgHLGVSclosePreon as z, 
rc.avgHLGVSopenFlow as op, 
rc.avgHLGVSopenRet as oo, 

hs.name,
org.name as kod_owner

from
realConsumers rc
join nodes n on n.id = rc.nodeID
left join externalCodes ec on ec.id = n.externalCodeID
left join externalSigns es on es.id = n.externalSignID
left join nodes ni on ni.id = n.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id
left join consumerStates cst on cst.id = rc.consumerStateID
left join WEmeteringDevices wemd on wemd.id = rc.WEmeteringDeviceID
left join closeSysCalcSigns cscs on cscs.id = rc.calcSignResWDORetID
left join closeSysCalcSigns cscs2 on cscs2.id = rc.closeSysCalcSignID
left join closeSysCalcSigns cscs3 on cscs3.id = rc.calcSignOpenSysFlowID
left join setLoadCloseSysCalcSigns slcscs on slcscs.id = rc.calcSignSetLoadCloseSys
left join setLoadCloseSysCalcSigns slcscs2 on slcscs2.id = rc.calcSignSetLoadOpenSysFlow
left join setLoadCloseSysCalcSigns slcscs3 on slcscs3.id = rc.calcSignSetLoadOpenSysRet
left join varCoefficients vc on vc.id = rc.varCoeffID
left join gvsLoadGraphs gvslg on gvslg.id = rc.gvsLoadGraphID
left join calcTemperatures ct on ct.id = rc.calcTemperatureID
left join specExpends se on se.id = rc.specExpendID
left join hydroModeSigns hms on hms.id = rc.hydroModeSignID
left join pdValveInstallLocs pdvil on pdvil.id = rc.PDvalveInstallLocID

left join responsibles resp on resp.id = rc.responsibleID
left join automDegs ad on ad.id = rc.automDegID
left join calcFerDiameterSIgns cfds on cfds.id = rc.calcFerDiameterSignID
left join temperatureChartSigns tcs on tcs.id = rc.temperChartSignID
left join throtStageSigns tss on tss.id = rc.throtStageSignID
left join stopValveTypes svt on svt.id = rc.stopValveTypeID
left join organizations org on org.id = n.organizationID
left join externalCodes ecm on ec.belongMagistral = ecm.id AND ec.objectID=2
Left join heatSources hs on hs.id=IIF(ec.objectID <> 2, ec.heatSourceID, ecm.heatSourceID)

JOIN
(
SELECT  
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON n.fileID=calc.fileID



WHERE n.fileID=$fileID$
AND n.removed=0

AND 

(
NOT n.id
IN (


SELECT  n.id

FROM realConsumers rc
JOIN nodes n ON n.id=rc.nodeID

JOIN
(
SELECT  
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON n.fileID=calc.fileID
JOIN PT_OUT ON PT_OUT.calculationID=cid AND PT_OUT.nodeID=n.id

)
OR rc.consumerStateID=2
) 

