SELECT top 2147483647 
n.id,
cst.name as 'sost', 
ec.name as 'kod', 
n.externalNodeName as 'uzel', 
es.name as 'pr',
se.specExpendID as 'kodur',
ct.calcTemperatureID as 'kodtr', 
gvslg.gvsLoadGraphID as 'kod_gvs', 
vc.kodkv as 'kodkv', 
gc.quarter as 'kvartal',gc.accumCoeff as 'k_akkuml', hms.name as 'pr_avar_TP',gc.normHLosFlow as 'tpotepi__pod',gc.normHLosRet as 'tpotepi__obr',
gc.calcHLdep as 'otopl_zco',gc.calcInternHDdep as 'otopl_tp_zco',gc.adjCalcHLdep as 'f_otopl_zco',gc.adjCalcHDdep as 'f_otopl_tp_zco',
gc.perspCalcHLdep as 'p_otopl_zco',gc.perspCalcInternHDdep as 'p_otopl_tp_zco',gc.calcTempDep as 'Tr_tv_co_zco',gc.mixFactCoeffDep as 'uf_zco',
gc.calcHLindep as 'otopl_nco',gc.calcInternHDindep as 'otopl_tp_nco',gc.adjCalcHLindep as 'f_otopl_nco',gc.adjCalcInternHDindep as 'f_otopl_tp_nco',
gc.perspCalcHLindep as 'p_otopl_nco',gc.perspCalcInternHDindep as 'p_otopl_tp_nco',gc.calcTempIndep as 'Tr_tv_co_nco',gc.mixFactCoeffIndep as 'uf_nco',
gc.calcHLventil as 'ventil_cv',gc.adjCalcHLventil as 'f_ventil_cv',gc.perspCalcHLventil as 'p_ventil_cv',gc.calcHLcond as 'kondiz',
gc.adjCalcHLcond as 'f_kondiz',gc.perspCalcHLcond as 'p_kondiz',gc.calcHLcloseSys as 'txz',gc.calcHLopenSysFlow as 'txop',gc.calcHLopenSysRet as 'txoo',
gc.adjCalcHLcloseSys as 'f_txz',gc.adjCalcHLopenSysFlow as 'f_txop',gc.adjCalcHLopenSysRet as 'f_txoo',gc.perspCalcHLcloseSys as 'p_txz',
gc.perspCalcHLopenSysFlow as 'p_txop',gc.perspCalcHLopenSysRet as 'p_txoo',gc.expendHWpart as 'dolja_vent_cv',gc.calcTempHA as 'Tr_tv_cv',
gc.calcHLparall as 'otopl_pr',gc.internHDparall as 'otopl_tp_pr',gc.adjCalcHLparall as 'f_otopl_pr',gc.adjInternHDparall as 'f_otopl_tp_pr',
gc.perspCalcHLparall as 'p_otopl_pr',gc.perspInternHDparall as 'p_otopl_tp_pr',
gc.calcHLGVSparall as 'gv_pr',gc.adjCalcHLGVSparall as 'f_gv_pr',gc.perspCalcHLGVSparall as 'p_gv_pr',gc.hourIrregCoeffParall as 'gvs_max_pr',
gc.avgHLcompParall as 'rez_pr',gc.tempRecircPipeParall as 'T_pot_rez_pr',gc.calcTempHRparall as 'Tr_tv_co_pr',gc.calcTempHWDOparall as 'Tr_gvs_pr',
gc.AMRdepParall as 'uf_zco_pr',gc.calcHLmix as 'otopl_sm',gc.internHDmix as 'otopl_tp_sm',gc.adjCalcHLmix as 'f_otopl_sm',gc.adjInternHDmix as 'f_otopl_tp_sm',
gc.perspCalcHLmix as 'p_otopl_sm',gc.perspInternHDmix as 'p_otopl_tp_sm',gc.calcHLGVSmix as 'gv_sm',gc.adjCalcHLGVSmix as 'f_gv_sm',
gc.perspCalcHLGVSmix as 'p_gv_sm',gc.hourIrregCoeffMix as 'gvs_max_sm',gc.avgHLcompMix as 'rez_sm',gc.tempRecircPipeMix as 'T_pot_rez_sm',
gc.calcTempHRmix as 'Tr_tv_co_sm',gc.calcTempHWDOmix as 'Tr_gvs_sm',gc.AMRdepMix as 'uf_zco_sm',gc.calcHLconseq as 'otopl_ps',
gc.internHDconseq as 'otopl_tp_ps',gc.adjCalcHLconseq as 'f_otopl_ps',gc.adjInternHDconseq as 'f_otopl_tp_ps',gc.perspCalcHLconseq as 'p_otopl_ps',
gc.perspInternHDconseq as 'p_otopl_tp_ps',gc.schemeConseqID as 'cx_co_ps',gc.calcHLGVSconseq as 'gv_ps',gc.adjCalcHLGVSconseq as 'f_gv_ps',
gc.perspCalcHLGVSconseq as 'p_gv_ps',gc.hourIrregCoeffConseq as 'gvs_max_ps',gc.avgHLcompconseq as 'rez_ps',gc.tempRecircPipeConseq as 'T_pot_rez_ps',
gc.calcTempHRconseq as 'Tr_tv_co_ps',gc.calcTempHWDOconseq as 'Tr_gvs_ps',gc.AMRdepConseq as 'uf_zco_ps',gc.calcHLpreon as 'otopl_pw',
gc.internHDpreon as 'otopl_tp_pw',gc.adjCalcHLpreon as 'f_otopl_pw',gc.adjInternHDpreon as 'f_otopl_tp_pw',gc.perspCalcHLpreon as 'p_otopl_pw',
gc.perspInternHDpreon as 'p_otopl_tp_pw',gc.schemePreonID as 'cx_co_pw',gc.calcHLGVSpreon as 'gv_pw',gc.adjCalcHLGVSpreon as 'f_gv_pw',
gc.perspCalcHLGVSpreon as 'p_gv_pw',gc.hourIrregCoeffPreon as 'gvs_max_pw',gc.avgHLcompPreon as 'rez_pw',gc.tempRecircPipePreon as 'T_pot_rez_pw',
gc.calcTempHRpreon as 'Tr_tv_co_pw',gc.calcTempHWDOpreon as 'Tr_gvs_pw',gc.AMRdepPreon as 'uf_zco_pw',gc.avgHLGVSopenSysFlow as 'gv_op',
gc.avgHLGVSopenSysRet as 'gv_oo',gc.adjAvgHLGVSopenSysFlow as 'f_gv_op',gc.adjAvgHLGVSopenSysRet as 'f_gv_oo',gc.perspAvgHLGVSopenSysFlow as 'p_gv_op',
gc.perspAvgHLGVSopenSysRet as 'p_gv_oo',gc.hourIrregCoeffOpen as 'gvs_max_o',gc.avgHLcompOpen as 'rez_op',gc.tempRecircPipeOpen as 'T_pot_rez_op',
gc.calcTempHWDOopen as 'Tr_gvs_oo_op', pdvil.name as 'pr_per_PD',gc.setPDonRegulator as 'p_per_PD',gc.setLeakageFlow as 'utechp',
gc.setLeakageRet as 'utecho',gc.volWaterHS as 'a24',gc.volWaterVS as 'a25',gc.minThrustDS as 'p_min',gc.hydroResCloseSys as 'gsz',
gc.hydroResCloseSysSummer as 'gszl',gc.hydroResWDOFlow as 'gsop',gc.hydroResWDORet as 'gsoo',gc.hydroThrustIn as 'pP',gc.hydroThrustOut as 'pO',
gc.calcExpendDep as 'G_otoplz',gc.calcExpendIndep as 'G_otopln',gc.calcExpendVentil as 'G_ventil',gc.calcExpendCond as 'G_kondiz',gc.calcExpendHWFlow as 'G_gvop',
gc.calcExpendHWRet as 'G_gvoo',gc.calcExpendRecircOpen as 'G_rez',gc.calcExpendHWparall as 'G_gvpr',gc.calcExpendHWmix as 'G_gvsm',
gc.calcExpendHWconseq as 'G_gvps',gc.calcExpendHWpreon as 'G_gvpw', gc.calcThrustInWSD as 'a12', 

cs.name as 'cx_zco',
gc.schemeParallID as 'cx_co_pr',gc.schemeMixID as 'cx_co_sm', wemd.name as 'pr_uchet', cscs2.name as 'gszpr',
cscs3.name as 'gsoppr', cscs.name as 'gsoopr', slcscs.name as 'prznz', slcscs2.name as 'prznp',
slcscs3.name as 'przno',

n.geoMarkTopTube as 'geodz', n.calcPressFlow as 'pP_fact', n.calcPressRet as 'pO_fact', 
n.PICdate as 'datenew', n.memo as 'memo', 
-- n.x as 'x', n.y as 'y', 
n.nodeName as 'Наименование', n.scheme as 'Схема', n.gpsCoords as 'GPS координаты', n.inventNumber as 'Инвентарный номер', n.belongMagistralSite as 'Принадлежность участку МС', n.belongDistSite as 'Принадлежность РС', n.magistralSite as 'Участок МС', n.distSite as 'Участок РС', n.belongHN as 'Принадлежность тепловым сетям', n.passport as 'Паспорт', n.isLoaded as 'name_typ', n.fileID, n.externalCodeID as 'operator'
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

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND n.removed=0
