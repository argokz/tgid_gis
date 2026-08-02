SELECT 
l.id,
eci.name as kod_p, ni.externalNodeName as uzel_p, esi.name as pr_p, 
ec1.name as kod1,
n1.externalNodeName as uzel1,
case l.externalSignLineID
    when 1 then ' '
    when 2 then 'П'
    when 3 then 'О'
    when 4 then 'П'
    when 5 then 'О'
end as pr1,
ec2.name as kod2, n2.externalNodeName as uzel2,
case l.externalSignLineID
    when 1 then ' '
    when 2 then 'П'
    when 3 then 'О'
    when 4 then 'О'
    when 5 then 'П'
end as pr2,

rad.name as name_ra, rad.type as typ_ra, rad.count as kol_ra, rad.totalEquivSurface as f_ra, 
l.hydroRes as sopr, l.registNum as registr, l.firstPICdate as datenew, l.lastMaintDate as dateend_to, l.displaySign as podp, 
l.archiveChangeDate as date_archives,
st.name as sost, org.name as "Владелец", o.name as operator, ni.fileID
from systemRadiators rad
join linesobj l on l.id = rad.lineID
join nodes n1 on n1.id = l.nodeID1
join nodes n2 on n2.id = l.nodeID2
join externalCodes ec1 on n1.externalCodeID = ec1.id
join externalSigns es1 on n1.externalSignID = es1.id
join externalCodes ec2 on n2.externalCodeID = ec2.id
join externalSigns es2 on n2.externalSignID = es2.id
join nodes ni on ni.id = n1.internalNodeID
join externalCodes eci on ni.externalCodeID = eci.id
join externalSigns esi on ni.externalSignID = esi.id
join states st on st.id = rad.stateID
left join operators o on o.id = l.operatorID
left join organizations org on org.id = l.organizationID

WHERE n1.fileID=$fileID$ AND n1.internalNodeID IS NULL
AND l.removed=0 AND n1.removed=0 AND n2.removed=0
