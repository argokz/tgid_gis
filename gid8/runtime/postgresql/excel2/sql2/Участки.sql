SELECT l.id,
-- pssF.name as key_ut_p,
-- pssR.name as key_ut_o,

IIF(hps.pipeSectStateIDflow=1, '', 'закр')  as key_ut_p, 
IIF(hps.pipeSectStateIDret=1,  '', 'закр')     as key_ut_o, 


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
 s.name as standard,
-- hps.standardTubeLink as kod_truba,

hps.tubesCount as truba,
hps.pipeSectLength as dlina,
hps.diameterInternal as diametr,
hps.tubeRoughness as scher,
hps.localResSum as mestnoe,
hps.localLosesShare as dolja,
 vcF.kodkv as kodkvp,
 vcR.kodkv as kodkvo,
l.hydroRes,

tt.name as name_typ,
hps.wallThickness as tol,
hps.heatTestsCoeff as kti,
hs.sourceName,
org.name AS org_name



from heatPipeSections hps
join linesobj l on l.id = hps.lineID
join nodes n1 on n1.id = l.nodeID1
join nodes n2 on n2.id = l.nodeID2
left join nodes ni on ni.id = n1.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id
join externalCodes ec1 on ec1.id = n1.externalCodeID
join externalCodes ec2 on ec2.id = n2.externalCodeID
join externalSigns es1 on es1.id = n1.externalSignID
join externalSigns es2 on es2.id = n2.externalSignID
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
LEFT JOIN heatSources hs ON hs.id = ec1.heatSourceID
left join organizations org on org.id = l.organizationID


WHERE n1.fileID=$fileID$ AND n1.internalNodeID IS NULL AND l.removed=0 AND n1.removed=0 AND n2.removed=0 AND n1.fileID=n2.fileID
