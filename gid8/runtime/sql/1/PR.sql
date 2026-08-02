SELECT top 2147483647
n.id,
cst.name as 'sost',

ec.name  as 'kod',
n.externalNodeName  as  'uzel',
--es.name as 'pr',


-- rc.sectConsumerCode as 'kod_cekz_p',
rc.schemeNum as 'cxema',
rc.name as 'Наименование здания',
rc.buildHeight as 'h',
rc.accumCoeff as 'k_akkuml',

-- n.geoMarkTopTube as 'geodz',
n.geoMarkNodeArea as 'Геод.отметка подвальной прокладки,м',

se.specExpendID as 'kodur',
ct.calcTemperatureID as 'kodtr',
gvslg.gvsLoadGraphID as 'kod_gvs',
vc.kodkv as 'kodkv',

rc.calcHLdep as 'otoplz',
rc.calcHLindep as 'otopln',
rc.relLoadFacade as 'otn_fs',
rc.calcInternHD as 'otopl_tp',
rc.calcHLventil as 'ventil',
rc.expendHWpart as 'dolja_vent',
rc.avgHLcond as 'kondiz',
rc.avgHLcloseSys as 'txz',
rc.avgHLopenSysFlow as 'txop',
rc.avgHLopenSysRet as 'txoo',
rc.avgHLGVSopenFlow as 'gvop',
rc.avgHLGVSopenRet as 'gvoo',
rc.avgHLGVScloseParall as 'gvpr',
rc.avgHLGVScloseMix as 'gvsm',
rc.avgHLGVScloseConseq as 'gvps',
rc.avgHLGVSclosePreon as 'gvpw',

hms.name as 'pr_avar_TP',
rc.mixFactCoeff as 'uf',
rc.hourIrregCoeff as 'gvs_max',
rc.circHLosOpen as 'rez',
rc.tempRecircPipe as 'T_pot_rez',

rc.setLeakageFlow as 'utechp',
rc.setLeakageRet as 'utecho',
rc.volWaterHS as 'a24',
rc.volWaterVS as 'a25',

rc.hydroResCloseSys as 'gsz',
rc.hydroResCloseSysSummer as 'gszl',

slcscs.name as 'prznz',


rc.hydroResWDOFlow as 'gsop',
slcscs2.name as 'prznp',

rc.hydroResWDORet as 'gsoo',
slcscs3.name as 'przno',




rc.hydroThrustIn as 'pP',
rc.hydroThrustOut as 'pO',

rc.calcExpendDep as 'G_otoplz',
rc.calcExpendIndep as 'G_otopln',
rc.calcExpendVentil as 'G_ventil',
rc.calcExpendCond as 'G_kondiz',
rc.calcExpendHWFlow as 'G_gvop',
rc.calcExpendHWRet as 'G_gvoo',
rc.calcExpendRecircOpen as 'G_rez',
rc.calcExpendHWparall as 'G_gvpr',
rc.calcExpendHWmix as 'G_gvsm',
rc.calcExpendHWconseq as 'G_gvps',
rc.calcExpendHWpreon as 'G_gvpw',

rc.diameterThrotDiaph as 'a15',
rc.diameterElevNozzle as 'a14',

rc.calcSignRes as 'pr_sopr',
rc.calcSignHL as 'pr_teplo',
rc.parallHeatersCount1 as 'a18',
rc.parallHeatersCount2 as 'a19',

rc.parallHeatersCountIndep as 'a19_co',
rc.calcThrustLosHS as 'a7',
rc.calcThrustLosAH as 'a8',
rc.calcThrustLosAC as 'a9',
rc.calcThrustLosFlow as 'a10',

rc.calcThrustLosFlowCirc as 'a11',
rc.calcThrustInWDO as 'a12',
rc.calcThrustLosHeaters1 as 'a22',
rc.calcThrustLosHeaters2 as 'a23',

pdvil.name as 'pr_per_PD',
rc.setPDonRegulator as 'p_per_PD',
rc.calcTempHR as 'Tr_tv_co',
rc.calcTempVS as 'Tr_tv_cv',
rc.calcTempHWDO as 'Tr_gvs',

rc.contractNumber as 'contract',
rc.isolationType as 'Изоляция',
svt.name as 'Запорная арматура',

rc.meterDevWorkSign as 'Признак работы прибора учета',
rc.meterDevSafety as 'Сохранность пломбы прибора учета',

rc.meterDevStampNum as 'Номер пломбы прибора учета',

wemd.name as 'pr_uchet',

cscs2.name as 'gszpr',
cscs3.name as 'gsoppr',
cscs.name as 'gsoopr',


tss.name as 'a13',
tcs.name as 'a17',
cfds.name as 'pr_shaibi',

ad.name as 'pr_avtomat',
resp.name as 'otv'



from realConsumers rc
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

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND n.removed=0
