select
    rc.id,
    hs.name as hsName,
    ec.name as ecName, 
    n.externalNodeName,
    n.PICdate,
    rc.name as 'rcName',
    rc.contractNumber,	
    st.name as 'streetName',
    rc.houseNumber,
    meter.name as 'meter',
    rc.countFloors,	
    rc.builtYear,
    hp.name as 'hpName',
    hpt.name as 'hpType',
    rc.countUserGV,
    rc.countBusinessConsumers,
    CASE 
        WHEN  PATINDEX('1.[1-4]',schemeNum) > 0 OR PATINDEX('%.[5-8]',schemeNum) > 0 THEN  calcHLdep /*зависимая - элеваторная*/
        WHEN  PATINDEX('1.[5-6]',schemeNum) > 0 OR PATINDEX('%.9',schemeNum) > 0 OR PATINDEX('%.1[0-2]',schemeNum) > 0  THEN  calcHLindep /* независимая - неэлеваторная*/
    END as 'calcHL',
    CASE 
        WHEN PATINDEX('3.%',schemeNum) > 0 THEN avgHLGVScloseParall
        WHEN PATINDEX('4.%',schemeNum) > 0 THEN avgHLGVScloseMix
        WHEN PATINDEX('5.%',schemeNum) > 0 THEN avgHLGVScloseConseq
        WHEN PATINDEX('6.%',schemeNum) > 0 THEN avgHLGVSopenFlow
        WHEN PATINDEX('7.%',schemeNum) > 0 THEN avgHLGVSopenRet
        WHEN PATINDEX('15.%',schemeNum) > 0 THEN avgHLGVSclosePreON
    END as 'avgHLGVS',
    CASE
        WHEN automDegID = 2 and PATINDEX('3.%',schemeNum) > 0 THEN avgHLGVScloseParall * hourIrregCoeff
        WHEN automDegID = 2 and PATINDEX('4.%',schemeNum) > 0 THEN avgHLGVScloseMix * hourIrregCoeff
        WHEN automDegID = 2 and PATINDEX('5.%',schemeNum) > 0 THEN avgHLGVScloseConseq * hourIrregCoeff
        WHEN automDegID = 2 and PATINDEX('6.%',schemeNum) > 0 THEN avgHLGVSopenFlow * hourIrregCoeff
        WHEN automDegID = 2 and PATINDEX('7.%',schemeNum) > 0 THEN avgHLGVSopenRet * hourIrregCoeff
        WHEN automDegID = 2 and PATINDEX('15.%',schemeNum) > 0 THEN avgHLGVSclosePreON * hourIrregCoeff
        WHEN automDegID = 1 and PATINDEX('3.%',schemeNum) > 0 THEN avgHLGVScloseParall * hourIrregCoeff
        WHEN automDegID = 1 and PATINDEX('4.%',schemeNum) > 0 THEN avgHLGVScloseMix * hourIrregCoeff
        WHEN automDegID = 1 and PATINDEX('5.%',schemeNum) > 0 THEN avgHLGVScloseConseq * hourIrregCoeff
        WHEN automDegID = 1 and PATINDEX('6.%',schemeNum) > 0 THEN avgHLGVSopenFlow * hourIrregCoeff
        WHEN automDegID = 1 and PATINDEX('7.%',schemeNum) > 0 THEN avgHLGVSopenRet * hourIrregCoeff
        WHEN automDegID = 1 and PATINDEX('15.%',schemeNum) > 0 THEN avgHLGVSclosePreON * hourIrregCoeff
    END as 'maxGV',
    rc.calcHLventil,
    rc.area,
    rc.buildingVolume,
    rc.basementVolume,
    rc.builtInVolume,
    rc.note_1,
    rc.note_2
from realConsumers rc
join heatPoint hp on hp.id = rc.heatPointID
left join nodes n ON n.id = rc.nodeID
left join externalCodes ec on ec.id = n.externalCodeID
left join heatSources hs on hs.id = ec.heatSourceID
left join WEmeteringDevices meter on meter.id = rc.WEmeteringDeviceID
left join streets st ON st.id = rc.streetID
left join heatPointTypes hpt on hpt.id = hp.heatPointTypeID	
WHERE
    n.fileID != 122 and rc.heatPointID in myId

GROUP by rc.heatPointID, rc.id, hs.name, ec.name,n.externalNodeName, 
    n.PICdate, rc.name, rc.contractNumber,st.name,rc.houseNumber,
    meter.name,
    rc.countFloors,	
    rc.builtYear,hp.name, 
    hpt.name, 
    rc.countUserGV,
    rc.countBusinessConsumers,
    rc.schemeNum, 
    rc.calcHLdep,
    rc.avgHLGVScloseParall,
    rc.avgHLGVScloseMix,
    rc.avgHLGVScloseConseq,
    rc.avgHLGVSopenFlow,
    rc.avgHLGVSopenFlow,
    rc.avgHLGVSopenRet,
    rc.automDegID,
    rc.hourIrregCoeff,
    rc.automDegID,
    rc.hourIrregCoeff,
    rc.calcHLventil,
    rc.calcHLindep,
    rc.avgHLGVSclosePreON,
    rc.area,
    rc.buildingVolume,
    rc.basementVolume,
    rc.builtInVolume,
    rc.note_1,
    rc.note_2