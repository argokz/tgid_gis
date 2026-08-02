SELECT
l.id,
st.name as sost, 
eci.name as kod_p, ni.externalNodeName as uzel_p, esi.name as pr_p, 
ec1.name as kod1,
n1.externalnodename as uzel1,
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
d.throtDiaphLoc as name_mesto, d.diameterInternal as dru_home, d.consInstDiaphCount as kol_home, d.entryMark as name_home, 
l.registNum as "Регистрационный номер", 
l.firstPICdate as "Дата первичного ввода в эксплуатацию", 
l.lastMaintDate as "Дата последнего ремонта", 
--l.displaySign as podp, 
--l.archiveChangeDate as "Дата изменения архива", 
org.name as "Владелец", 
o.name as operator, ni.fileID

from diaphragms d
join linesobj l on l.id = d.lineID
join nodes n1 on n1.id = l.nodeID1
join nodes n2 on n2.id = l.nodeID2
join externalCodes ec1 on n1.externalCodeID = ec1.id
join externalSigns es1 on n1.externalSignID = es1.id
join externalCodes ec2 on n2.externalCodeID = ec2.id
join externalSigns es2 on n2.externalSignID = es2.id
left join nodes ni on ni.id = n1.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id
left join states st on st.id = d.stateID
left join operators o on o.id = l.operatorID
left join organizations org on org.id = l.organizationID

WHERE n1.fileID=$fileID$ -- AND n1.internalNodeID IS NULL
AND l.removed=0 AND n1.removed=0 AND n2.removed=0

-- AND n1.internalNodeID IS NULL
