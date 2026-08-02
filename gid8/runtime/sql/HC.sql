SELECT top 2147483647
l.id,

 eci.name as 'kod_p',
 ni.externalNodeName as 'uzel_p',
 esi.name as 'pr_p',

ec1.name as 'kod1',
n1.externalNodeName as 'uzel1',
case l.externalSignLineID
    when 1 then ' '
    when 2 then 'П'
    when 3 then 'О'
    when 4 then 'П'
    when 5 then 'О'
end as 'pr1',
ec2.name as 'kod2', 
n2.externalNodeName as 'uzel2',
case l.externalSignLineID
    when 1 then ' '
    when 2 then 'П'
    when 3 then 'О'
    when 4 then 'О'
    when 5 then 'П'
end as 'pr2',
p.offreason as 'pr_otkl', p.pumpstationid as 'name_nas_stanzii', p.number as 'nomer_nas_agregata', p.thrust as 'h', p.parallagregcount as 'k_nas', 
p.r0, p.r1, p.r2, p.e0, p.e1, p.e2, p.k0, p.k1, p.k2, p.r0_z, 
p.r1_z, p.r2_z, p.e0_z, p.e1_z, p.e2_z, p.k0_z, p.k1_z, p.k2_z, 
p.rotorrotspeedset as 'rate_zad', p.rotordiameterset as 'd_zad', p.lastpumpreplacedate as 'dateend_nasos', p.lastemreplacedate as 'dateend_electric', 
p.lastagregreplacedate as 'dateend_pump', p.repaircountpump as 'kol_nasos', p.replacecountagreg as 'kol_pump', p.repaircountem as 'kol_electric', 
--p.opc as 'opc', 
sp.tip_nas as 'tip_nas', dt.name as 'pr_privod', rdt.name as 'pr_diametr', 
sem.name as 'tip_el', org.name as 'Владелец', 
o.name as 'Оператор', 
st.name as 'sost'
from pumps p
join linesobj l on l.id = p.lineID
left join nodes n1 on n1.id = l.nodeID1
left join nodes n2 on n2.id = l.nodeID2

left join externalCodes ec1 on n1.externalCodeID = ec1.id
left join externalSigns es1 on n1.externalSignID = es1.id
left join externalCodes ec2 on n2.externalCodeID = ec2.id
left join externalSigns es2 on n2.externalSignID = es2.id

left join nodes ni on ni.id = n1.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id

left join organizations org on org.id = l.organizationID
left join operators o on o.id = l.operatorID
left join standardEMs sem on sem.id = p.standardEMID
left join rotorDiameterTypes rdt on rdt.id = p.rotorDiameterTypeID
left join driveTypes dt on dt.id = p.driveTypeID
left join standardPumps sp on sp.id = p.standardPumpID
left join states st on st.id = p.stateID

WHERE n1.fileID=$fileID$ AND l.removed=0 AND n1.removed=0 AND n2.removed=0

--AND n1.internalNodeID IS NULL
