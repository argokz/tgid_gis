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
prec.name as "Код РС узла с регулируемым напором", prn.externalNodeName as "Наименование узла с регулируемым напором", pres.name as przu,
pr.h as h_uzu, pr.deltaH as delta, pr.regValveRelCap as kv, pr.relLeakage as otn_kv, pr.consDrip as g_tep_poteri, pr.valveHydroResOpen as min_sm, 
--l.registNum as registr,l.firstPICdate as datenew,l.lastMaintDate as dateend_to, l.displaySign as podp, l.archiveChangeDate as date_archives,
pr.valveHydroResClose as max_sm, rs.name as sost, 
--ps.name as przu1, 
wa.name as pr_raboti, 
o.name as "Оператор", org.name as "Владелец", ni.fileID
from pressRegulators pr
join linesobj l on l.id = pr.lineID
join nodes n1 on n1.id = l.nodeID1
join nodes n2 on n2.id = l.nodeID2
join externalCodes ec1 on n1.externalCodeID = ec1.id
join externalSigns es1 on n1.externalSignID = es1.id
join externalCodes ec2 on n2.externalCodeID = ec2.id
join externalSigns es2 on n2.externalSignID = es2.id
left join nodes ni on ni.id = n1.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id
left join nodes prn on prn.id = pr.nodeID
left join externalCodes prec on prec.id = prn.externalCodeID
left join externalSigns pres on pres.id = prn.externalSignID
left join workAttributes wa on wa.id = pr.workAttrID
left join regulatorStates rs on rs.id = pr.regulatorStateID
left join pipelineSigns ps on ps.id = pr.pipelineSignID
left join operators o on o.id = l.operatorID
left join organizations org on org.id = l.organizationID

WHERE n1.fileID=$fileID$ -- AND n1.internalNodeID IS NULL
AND l.removed=0 AND n1.removed=0 AND n2.removed=0
