select TOP 2000000

n.id,
IIF(cst.name='открыто', ' ', 'закр') as 'sost', 
' ' AS po_pr,
ec.name  as 'kod',    n.externalNodeName  as  'uzel', 
rc.name AS name_building, 
n.geoMarkTopTube as 'geodz', 
se.specExpendID as 'kodur', ct.calcTemperatureID as 'kodtr', 
buildHeight AS h,
rc.calcHLdep as 'otoplz', 
rc.calcHLindep as 'otopln', 
rc.relLoadFacade as 'otn_fs', 
rc.calcInternHD as 'otopl_tp', 
rc.calcHLventil as 'ventil', 
rc.avgHLcond as 'kondiz', 
rc.avgHLGVSopenFlow as 'gvop', 
rc.avgHLGVSopenRet as 'gvoo', 
rc.circHLosOpen as 'rez', 
rc.avgHLGVScloseParall as 'gvpr', 
rc.avgHLGVScloseMix as 'gvsm', 
rc.avgHLGVScloseConseq as 'gvps', 
rc.avgHLGVSclosePreon as 'gvpw', 
rc.setLeakageFlow as 'utechp', 
rc.setLeakageRet as 'utecho', 
vc.kodkv as 'kodkv',
hms.name as 'pr_avar_TP', 
cscs2.name as 'gszpr', 
rc.hydroResCloseSys as 'gsz', 
slcscs2.name as 'prznp', 
rc.hydroResWDOFlow as 'gsop', 
slcscs3.name as 'przno', 
rc.hydroResWDORet as 'gsoo',
hs.name,
org.name as 'kod_owner'


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

WHERE n.fileID=$fileID$ AND n.removed=0
