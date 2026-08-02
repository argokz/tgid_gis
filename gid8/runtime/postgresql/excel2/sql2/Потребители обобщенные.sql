select
n.id,
IIF(cst.name='открыто', ' ', 'закр') as sost, 

'О' AS po_pr,
ec.name as kod, 
n.externalNodeName as uzel, 

'' AS name_building, 
n.geoMarkTopTube as geodz, 
se.specExpendID as kodur,ct.calcTemperatureID as kodtr, 
maxBuildingHeight as h,

gc.calcHLdep as otoplz, 
gc.calcHLindep as otopln, 
gc.calcInternHDdep+gc.calcInternHDindep+gc.internHDparall+gc.internHDmix+gc.internHDconseq+gc.internHDpreON as otopl_tp, 
gc.calcHLventil as ventil, 
gc.calcHLcond as kondiz, 

calcHLparall+calcHLmix+calcHLconseq+calcHLpreON AS q1,
gc.internHDparall+gc.internHDmix+gc.internHDconseq+gc.internHDpreON AS q2,

gc.avgHLGVSopenSysFlow as gv_op,
gc.avgHLGVSopenSysRet as gv_oo, 
gc.avgHLcompOpen as rez, 


gc.calcHLGVSparall as gvpr, 
gc.calcHLGVSmix as gvsm, 
gc.calcHLGVSconseq as gvps, 
gc.calcHLGVSpreon as gvpw, 

gc.setLeakageFlow as utechp, 
gc.setLeakageRet as utecho, 
vc.kodkv as kodkv,
hms.name as pr_avar_TP, 

gc.hydroResCloseSys as gsz, 
cscs2.name as gszpr, 


gc.hydroResWDOFlow as gsop, 
slcscs2.name as prznp, 

gc.hydroResWDORet as gsoo,
slcscs3.name as przno, 

hs.name



from generalizedConsumers gc
join nodes n on n.id = gc.nodeID
left join externalCodes ec on ec.id = n.externalCodeID
left join externalSigns es on es.id = n.externalSignID
left join consumerStates cst on cst.id = gc.consumerStateID
left join WEmeteringDevices wemd on wemd.id = gc.WEmeteringDeviceID
left join closeSysCalcSigns cscs on cscs.id = gc.calcSignResWDORetID
left join closeSysCalcSigns cscs2 on cscs2.id = gc.closeSysCalcSignID
left join closeSysCalcSigns cscs3 on cscs3.id = gc.calcSignOpenSysFlowID
left join setLoadCloseSysCalcSigns slcscs on slcscs.id = gc.calcSignSetLoadCloseSys
left join setLoadCloseSysCalcSigns slcscs2 on slcscs2.id = gc.calcSignSetLoadOpenSysFlow
left join setLoadCloseSysCalcSigns slcscs3 on slcscs3.id = gc.calcSignSetLoadOpenSysRet
left join connectionSchemes cs on cs.id = gc.connectionSchemeID
left join varCoefficients vc on vc.id = gc.varCoeffID
left join gvsLoadGraphs gvslg on gvslg.id = gc.gvsLoadGraphID
left join calcTemperatures ct on ct.id = gc.calcTemperatureID
left join specExpends se on se.id = gc.specExpendID
left join hydroModeSigns hms on hms.id = gc.hydroModeSignID
left join pdValveInstallLocs pdvil on pdvil.id = gc.PDvalveInstallLocID

left join externalCodes ecm on ec.belongMagistral = ecm.id AND ec.objectID=2
Left join heatSources hs on hs.id=IIF(ec.objectID <> 2, ec.heatSourceID, ecm.heatSourceID)

WHERE n.fileID=$fileID$ AND n.removed=0

