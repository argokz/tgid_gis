SELECT 
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
pdrec.name as uzu_k, pdrn.externalNodeName as uzu, pdres.name as przu, 

pdr.deltaH as delta, pdr.regValveRelCap as kv, pdr.maxLeakageCloseValve as otn_kv, pdr.regValveHydroRes as sm, 
pdr.consThroughRegValve as g, pdr.thrustDropMean as h_fakt, pdr.consDrip as g_tep_poteri, 
rs.name as sost, pdr.workAttrID as pr_raboti,
l.registNum as registr, l.firstPICdate as datenew, l.lastMaintDate as dateend_to, l.displaySign as podp, 
l.archiveChangeDate as date_archives,
wa.name as pr_raboti, 
o.name as operator, org.name as kod_owner, ni.fileID
from pressDropRegulators pdr
join linesobj l on l.id = pdr.lineID
join nodes n1 on n1.id = l.nodeID1
join nodes n2 on n2.id = l.nodeID2
join externalCodes ec1 on n1.externalCodeID = ec1.id
join externalSigns es1 on n1.externalSignID = es1.id
join externalCodes ec2 on n2.externalCodeID = ec2.id
join externalSigns es2 on n2.externalSignID = es2.id
join nodes ni on ni.id = n1.internalNodeID
join externalCodes eci on ni.externalCodeID = eci.id
join externalSigns esi on ni.externalSignID = esi.id
join nodes pdrn on pdrn.id = pdr.nodeID
join externalCodes pdrec on pdrec.id = pdrn.externalCodeID
join externalSigns pdres on pdres.id = pdrn.externalSignID
left join workAttributes wa on wa.id = pdr.workAttrID
left join regulatorStates rs on rs.id = pdr.regulatorStateID
left join operators o on o.id = l.operatorID
left join organizations org on org.id = l.organizationID

WHERE n1.fileID=$fileID$ AND l.removed=0 AND n1.removed=0 AND n2.removed=0 AND n1.fileID=n2.fileID
