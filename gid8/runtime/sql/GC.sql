SELECT top 2147483647 
n.id,
cs.name as 'sost', n.externalNodeName as 'kod', ec.name as 'uzel', es.name as 'pr',
ni.externalNodeName as 'kod_p', eci.name as 'uzel_p', esi.name as 'pr_p',
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
gc.perspInternHDconseq as 'p_otopl_tp_ps',gc.schemeConseq as 'cx_co_ps',gc.calcHLGVSconseq as 'gv_ps',gc.adjCalcHLGVSconseq as 'f_gv_ps',
gc.perspCalcHLGVSconseq as 'p_gv_ps',gc.hourIrregCoeffConseq as 'gvs_max_ps',gc.avgHLcompconseq as 'rez_ps',gc.tempRecircPipeConseq as 'T_pot_rez_ps',
gc.calcTempHRconseq as 'Tr_tv_co_ps',gc.calcTempHWDOconseq as 'Tr_gvs_ps',gc.AMRdepConseq as 'uf_zco_ps',gc.calcHLpreon as 'otopl_pw',
gc.internHDpreon as 'otopl_tp_pw',gc.adjCalcHLpreon as 'f_otopl_pw',gc.adjInternHDpreon as 'f_otopl_tp_pw',gc.perspCalcHLpreon as 'p_otopl_pw',
gc.perspInternHDpreon as 'p_otopl_tp_pw',gc.schemePreon as 'cx_co_pw',gc.calcHLGVSpreon as 'gv_pw',gc.adjCalcHLGVSpreon as 'f_gv_pw',
gc.perspCalcHLGVSpreon as 'p_gv_pw',gc.hourIrregCoeffPreon as 'gvs_max_pw',gc.avgHLcompPreon as 'rez_pw',gc.tempRecircPipePreon as 'T_pot_rez_pw',
gc.calcTempHRpreon as 'Tr_tv_co_pw',gc.calcTempHWDOpreon as 'Tr_gvs_pw',gc.AMRdepPreon as 'uf_zco_pw',gc.avgHLGVSopenSysFlow as 'gv_op',
gc.avgHLGVSopenSysRet as 'gv_oo',gc.adjAvgHLGVSopenSysFlow as 'f_gv_op',gc.adjAvgHLGVSopenSysRet as 'f_gv_oo',gc.perspAvgHLGVSopenSysFlow as 'p_gv_op',
gc.perspAvgHLGVSopenSysRet as 'p_gv_oo',gc.hourIrregCoeffOpen as 'gvs_max_o',gc.avgHLcompOpen as 'rez_op',gc.tempRecircPipeOpen as 'T_pot_rez_op',
gc.calcTempHWDOopen as 'Tr_gvs_oo_op', pdvil.name as 'pr_per_PD',gc.setPDonRegulator as 'p_per_PD',gc.setLeakageFlow as 'utechp',
gc.setLeakageRet as 'utecho',gc.volWaterHS as 'a24',gc.volWaterVS as 'a25',gc.minThrustDS as 'p_min',gc.hydroResCloseSys as 'gsz',
gc.hydroResCloseSysSummer as 'gszl',gc.hydroResWDOFlow as 'gsop',gc.hydroResWDORet as 'gsoo',gc.hydroThrustIn as 'pP',gc.hydroThrustOut as 'pO',
gc.calcExpendDep as 'G_otoplz',gc.calcExpendIndep as 'G_otopln',gc.calcExpendVentil as 'G_ventil',gc.calcExpendCond as 'G_kondiz',gc.calcExpendHWFlow as 'G_gvop',
gc.calcExpendHWRet as 'G_gvoo',gc.calcExpendRecircOpen as 'G_rez',gc.calcExpendHWparall as 'G_gvpr',gc.calcExpendHWmix as 'G_gvsm',
gc.calcExpendHWconseq as 'G_gvps',gc.calcExpendHWpreon as 'G_gvpw', gc.calcThrustInWSD as 'a12', se.name as 'kodur',
ct.name as 'kodtr', gvslg.name as 'kod_gvs', vc.name as 'kodkv', cs.name as 'cx_zco',
gc.schemeParall as 'cx_co_pr',gc.schemeMix as 'cx_co_sm', wemd.name as 'pr_uchet', cscs2.name as 'gszpr',
cscs3.name as 'gsoppr', cscs.name as 'gsoopr', slcscs.name as 'prznz', slcscs2.name as 'prznp',
slcscs3.name as 'przno',

ni.geoMarkTopTube as 'geodz', ni.geoMarkNodeArea as 'geod_z', n1.calcPressFlow as 'pP_fact', n1.calcPressRet as 'pO_fact', l.displaySign as 'podp', 
l.archiveChangeDate as 'date_archives', n1.PICdate as 'datenew', n1.memo as 'memo', n1.x as 'x', n1.y as 'y', n1.nodeName as 'Наименование', n.scheme as 'Схема', n.gpsCoords as 'GPS координаты', n.inventNumber as 'Инвентарный номер', n.belongMagistralSite as 'Принадлежность участку МС', n.belongDistSite as 'Принадлежность РС', n.magistralSite as 'Участок МС', n.distSite as 'Участок РС', n.belongHN as 'Принадлежность тепловым сетям', n.passport as 'Паспорт', n.isLoaded as 'name_typ', 
n.fileID, 
n.externalCodeID as 'operator'
from generalizedConsumers gc
join linesobj l on l.id = hps.lineID
join nodes n on n.id = l.nodeID1
join externalCodes ec on ec.id = n.externalCodeID
join externalSigns es on es.id = n.externalSignID
join nodes ni on ni.id = n.internalNodeID
join externalCodes eci on ni.externalCodeID = eci.id
join externalSigns esi on ni.externalSignID = esi.id
join consumerStates cs on cs.id = gc.consumerStateID
join WEmeteringDevices wemd on wemd.id = gc.WEmeteringDeviceID
join closeSysCalcSigns cscs on cscs.id = gc.calcSignResWDORetID
join closeSysCalcSigns cscs2 on cscs2.id = gc.closeSysCalcSignID
join closeSysCalcSigns cscs3 on cscs3.id = gc.calcSignOpenSysFlowID
join setLoadCloseSysCalcSigns slcscs on slcscs.id = gc.calcSignSetLoadCloseSys
join setLoadCloseSysCalcSigns slcscs2 on slcscs2.id = gc.calcSignSetLoadOpenSysFlow
join setLoadCloseSysCalcSigns slcscs3 on slcscs3.id = gc.calcSignSetLoadOpenSysRet
join connectionSchemes cs on cs.id = gc.connectionSchemeID
join varCoefficients vc on vc.id = gc.varCoeffID
join gvsLoadGraphs gvslg on gvslg.id = gc.gvsLoadGraphID
join calcTemperatures ct on ct.id = gc.calcTemperatureID
join specExpends se on se.id = gc.specExpendID
join hydroModeSigns hms on hms.id = gc.hydroModeSignID
join pdValveInstallLocs pdvil on pdvil.id = gc.PDvalveInstallLocID


WHERE n.fileID=$fileID$