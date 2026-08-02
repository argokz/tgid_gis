SELECT  
 pssF.name as pipeSectionState,
 ec1.name as externalCode1,
 n1.externalNodeName as externalNodeName1,
  case l.externalSignLineID
    when 1 then ' '
    when 2 then 'Ï'
    when 3 then 'Î'
    when 4 then 'Ï'
    when 5 then 'Î'
 end as externalSignLine1,
 ec2.name as externalCode2, 
 n2.externalNodeName as externalNodeName2,
 case l.externalSignLineID
    when 1 then ' '
    when 2 then 'Ï'
    when 3 then 'Î'
    when 4 then 'Î'
    when 5 then 'Ï'
 end as externalSignLine2,
tt.name as tubingType,

YEAR(firstPICdateHP) as firstPICdateHP,
PICdateCapital as PICdateCapital,
hps.diameterExternal as diameterExternal,
hps.pipeSectLength as pipeSectLength,
IIF (hps.tubingTypeID IN (4) AND l.externalSignLineID IN (1,2,4), hps.pipeSectLength*hps.diameterInternal/1000, 0) as matCharFlow,
IIF (hps.tubingTypeID IN (4) AND l.externalSignLineID IN (1,3,4), hps.pipeSectLength*hps.diameterInternal/1000, 0) as matCharRet,
IIF (hps.tubingTypeID NOT IN (4),  hps.pipeSectLength*hps.diameterInternal/1000, 0)*IIF (l.externalSignLineID IN (1), 2, 1) as matCharUnder,

-- hps.pipeSectLength*hps.diameterInternal/1000,
'' AS MM,
hps.pipeSectLength*POWER(hps.diameterInternal/1000,2)*PI()/4*IIF (l.externalSignLineID IN (1), 2, 1) as capacityW,
hps.pipeSectLength*POWER(hps.diameterInternal/1000,2)*PI()/4*IIF (l.externalSignLineID IN (1), 2, 1) as capacityS,

-- repairDatePlanTP as repairDate,
org.name as organization

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
left join tubingTypes tt on tt.id = hps.tubingTypeID
left join organizations org ON org.id = l.organizationID
LEFT JOIN heatSources ON heatSources.id=eci.heatSourceID


WHERE n1.fileID=$fileID$ AND n1.internalNodeID IS NULL AND l.removed=0


