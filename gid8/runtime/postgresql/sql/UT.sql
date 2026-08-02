SELECT l.id,
-- mestn,
 pssF.name as key_ut_p,
 pssR.name as key_ut_o,
 eci.name as kod_p,
 ni.externalNodeName as uzel_p,
 esi.name as pr_p,
 ec1.name as kod1,
 n1.externalNodeName as uzel1,
 case l.externalSignLineID
    when 1 then ' ' 
    when 2 then 'П'
    when 3 then 'О'
    when 4 then 'П'
    when 5 then 'О'
 end as pr1,
 ec2.name as kod2, 
 n2.externalNodeName as uzel2,
 case l.externalSignLineID
    when 1 then ' ' 
    when 2 then 'П'
    when 3 then 'О'
    when 4 then 'О'
    when 5 then 'П'
 end as pr2,

org.name as "Владелец",
 
 s.name as standard,
-- hps.standardTubeLink as kod_truba,
 vcF.kodkv as kodkvp,
 vcR.kodkv as kodkvo,



hps.tubesCount as truba,
hps.diameterInternal as diametr,
hps.wallThickness as tol,
hps.diameterCondit as diametr_usl,
--hps.pipeSectLength as dlina,

IIF(l.externalSignLineID IN (1, 2, 4), hps.pipeSectLength, 0) as "Протяженность по подаче, м",
IIF(l.externalSignLineID IN (1, 3, 5), hps.pipeSectLength, 0) as "Протяженность по обратке, м",

hps.tubeRoughness as scher,
hps.localResSum as mestnoe,
hps.localLosesShare as dolja,
--hps.channelID as kog_canal,
hps.heatTestsCoeff as kti,
hps.signNumWork as kolwork,
hps.isolThickness as thickizol,
hps.isolMaterialHCcoeff as k_izol,
hps.pipelineLayingDepth as depth,
hps.isolHTcoeffAbove as izoloutair,
hps.isolHTcoeffUnder as izol_air,
hps.airGroundHTcoeffUnder as air_ground,
hps.groundHCcoeff as tpground,
hps.pipelineAxesDist as distance,
hps.opc as opc,

 chls.name as name_calctpr,
 tt.name as name_typ,
 im.name as kod_izol,
--  pdl.name as f_elektropotenz,
--  cdo.name as f_nar_korroz,
--  cdi.name as f_vnu_korroz,
--  figw.name as f_grund_water,
--  fifw.name as f_pavod_water,
--  fitw.name as f_vod_water,
--  fibw.name as f_fek_water,
--  swd.name as f_water_otved,
--  bc.name as f_otkl,
--  bcv.name as f_okl_pot,
--  erwd.name as f_slog_arbeit,
--  tp.name as f_tratuar,
--  tw.name as f_doroga,
--  pd.name as f_ucherb_narod,
--  idam.name as f_ucherb_gorod,
 tc.name as "Характеристика трубы",
 ttypes.name as "Тип трубы",
 fm.name as "Завод изготовитель",
 em.name as "Материал наружного покрытия",
 am.name as "Материал антикоррозийного покрытия",


hps.damageNum as kol_pereklad,
hps.lastTransDate as dateend,
hps.lastIsolDate as dateend_izol,
hps.repairDateCapital as datenew_kapital,
hps.PICdateCapital as dateend_kapital,
hps.repairDateMaint as datenew_repair,
hps.PICdateMaint as dateend_repair,
-- hps.sectExploitPeriod as f_kol_let,
-- hps.specDamageCoeff as f_koef_povr,
-- hps.specDamageCoeff2 as f_koef_povr_2goda,
-- hps.powCabInstCount10 as f_10_elektro,
-- hps.powCabInstCount5 as f_5_elektro,
-- hps.powCabInstCount3 as f_3_elektro,
-- hps.powCabInstCount1 as f_1_elektro,
-- hps.powCabInstCount0 as f_0_elektro,
-- hps.powCabIntersCount as f_peres_elektro,
-- hps.gasECPcount10 as f_10_gas,
-- hps.gasECPcount5 as f_5_gas,
-- hps.gasECPcount3 as f_3_gas,
-- hps.gasECPcount1 as f_1_gas,
-- hps.gasECPcount0 as f_0_gas,
-- hps.gasECPIntersCount as f_peres_gas,
-- hps.elTranspCount10 as f_10_transp,
-- hps.elTranspCount5 as f_5_transp,
-- hps.elTranspCount3 as f_3_transp,
-- hps.elTranspCount1 as f_1_transp,
-- hps.elTranspCount0 as f_0_transp,
-- hps.elTranspIntersCount as f_peres_transp,
-- hps.jointsTightnessInfr as f_st_stik,
-- hps.chanConstrFract as f_st_islom,
-- hps.projSolDeviation as f_st_otkl,
-- hps.constrBearReduce as f_st_nes,
-- hps.fixedSuppDestr as f_st_opor,
hps.hydraTestsDate as date_isp_gid,
hps.sectExpend as G_isp_gid,
hps.sectThrustLoses as dP_isp_gid,
hps.heatTestsDate as date_isp_tep,
hps.temperOutAir as tn_isp_tep,
hps.temperGround as tg_isp_tep,
hps.expendDWflow as G1_isp_tep,
hps.expendDWret as G2_isp_tep,
hps.temperCoolFlow as dt1_isp_tep,
hps.temperCoolRet as dt2_isp_tep,
hps.temperDWflow as t1_isp_tep,
hps.temperDWret as t2_isp_tep,
hps.diameterExternal as "Диаметр внешний",
--hps.tubeCharactID as "Характеристика трубы",
-- hps.tubeTypeID as "Тип трубы",
--hps.tubeMaterial as "Материал трубы",
hps.temperMax as "Максимально допустимая температура",
--hps.factoryManufID as "Завод изготовитель",
--hps.externMaterialID as "Материал наружного покрытия",
--hps.isolationTypeID as "Тип изоляции",
hps.externCoverThick as "Толщина наружного покрытия, мм",
--hps.anticorrMaterialID as "Материал антикоррозийного покрытия",

ot.name AS "Вид сети",

mag.naimenovanie_magistrali as "Магистраль",
ms.opisanie_uchastka_ms as "Участок МС",
rs.naimenovanie_uchastka_rs as "Участок РС",
IIF(NOT npts1.id IS NULL AND NOT npts2.id IS NULL, CONCAT(ISNULL(npts1.nodeName, npts1.externalNodeName), ' - ', ISNULL(npts2.nodeName, npts2.externalNodeName)), '') as "Участок ПТС"

--hps.exploitReg as "Район эксплуатации",
--hps.net as "Сеть",
--hps.exploitSite as "Участок эксплуатации",
--hps.netType as "Вид сети",
--hps.crimpingQueSite as "Участок очереди опрессовки"

from heatPipeSections hps
join linesobj l on l.id = hps.lineID
join nodes n1 on n1.id = l.nodeID1
join nodes n2 on n2.id = l.nodeID2
left join nodes ni on ni.id = n1.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id
left join externalCodes ec1 on ec1.id = n1.externalCodeID
left join externalCodes ec2 on ec2.id = n2.externalCodeID
left join externalSigns es1 on es1.id = n1.externalSignID
left join externalSigns es2 on es2.id = n2.externalSignID
left join pipeSectionsStates pssF on pssF.id = hps.pipeSectStateIDflow
left join pipeSectionsStates pssR on pssR.id = hps.pipeSectStateIDret
left join standards s on s.id = hps.standardID
left join varCoefficients vcF on vcF.id = hps.varCoeffIDflow
left join varCoefficients vcR on vcR.id = hps.varCoeffIDret
left join calcHeatLosesSigns chls on chls.id = hps.calcHeatLosSignID
left join tubingTypes tt on tt.id = hps.tubingTypeID
left join isolMaterials im on im.id = hps.isolMaterialID
left join potentialDiffLevels pdl on pdl.id = hps.potentialDiffLevelID
left join corrosionDegreesOut cdo on cdo.id = hps.corrosionDegOutID
left join corrosionDegreesIn cdi on cdi.id = hps.corrosionDegInID
left join floodIntensitiesGW figw on figw.id = hps.floodIntensityGWid
left join floodIntensitiesFW fifw on fifw.id = hps.floodIntensityFWid
left join floodIntensitiesTW fitw on fitw.id = hps.floodIntensityTWid
left join floodIntensitiesBW fibw on fibw.id = hps.floodIntensityBWid
left join sectWaterDumps swd on swd.id = hps.sectWaterDumpID
left join breakComplexities bc on bc.id = hps.breakComplexityID
left join breakConsumVols bcv on bcv.id = hps.breakConsumVolID
left join ERWdifficulties erwd on erwd.id = hps.ERWdifficultyID
left join tubingPedestrians tp on tp.id = hps.tubingPedestrianID
left join tubingWays tw on tw.id = hps.tubingWayID
left join populDamages pd on pd.id = hps.populDamageID
left join infrastrDamages idam on idam.id = hps.infrastrDamageID
left join tubeCharacteristics tc on tc.id = hps.tubeCharactID
left join tubeTypes ttypes on ttypes.id = hps.tubeTypeID
left join factoryManufacturers fm on fm.id = hps.factoryManufID
left join externalMaterials em on em.id = hps.externMaterialID
left join anticorrMaterials am on am.id = hps.anticorrMaterialID
left join organizations org on org.id = l.organizationID

left join uchastok_ms ms ON ms.id=hps.magistralSite
left join uchastok_rs rs ON rs.id=hps.distSite
left join magistrali mag ON mag.id=hps.magistral
left join pipeSections pss ON pss.id=hps.pipeSectionID
left join nodes npts1 on npts1.id = pss.nodeID1
left join nodes npts2 on npts2.id = pss.nodeID2
left join objectTypes ot ON ot.id=hps.netType


WHERE n1.fileID=$fileID$ AND n1.internalNodeID IS NULL
AND l.removed=0 AND n1.removed=0 AND n2.removed=0 AND n1.fileID=n2.fileID
