SELECT
l.id,
eci.name as kod_p, ni.externalNodeName as uzel_p, esi.name as pr_p, 
ec1.name as kod1, n1.externalNodeName as uzel1, 
  case externalSignLineID
    when 1 then ' '
    when 2 then 'П'
    when 3 then 'О'
    when 4 then 'П'
    when 5 then 'О'
  end as pr1,
  ec2.name as kod2, n2.externalNodeName as uzel2,
  case externalSignLineID
    when 1 then ' '
    when 2 then 'П'
    when 3 then 'О'
    when 4 then 'О'
    when 5 then 'П'
  end as pr2,
--bec.name as uzu_k, bn.externalNodeName as uzu, bes.name as przu,
bec.name as "Код РС узла с регулируемым напором", bn.externalNodeName as "Наименование узла с регулируемым напором", bes.name as przu,



b.q as q, b.deltaQ as delta_q, b.standardTubeLink as kod_truba, b.length as dln, b.diameterInternal as diam, 
b.tubeRoughness as scher, b.resCoeffsSum as sum_m_s, b.locInstall as ustanovka,

--l.hydroRes as sopr,l.registNum as registr,l.firstPICdate as datenew,l.lastMaintDate as dateend_to, l.displaySign as podp, l.archiveChangeDate as date_archives, 
rs.name as sost, st.name as standard, org.name as "Владелец"
--, ni.fileID
from bypass b
left join linesobj l on l.id = b.lineID
left join nodes n1 on n1.id = l.nodeID1
left join nodes n2 on n2.id = l.nodeID2
left join externalCodes ec1 on n1.externalCodeID = ec1.id
left join externalSigns es1 on n1.externalSignID = es1.id
left join externalCodes ec2 on n2.externalCodeID = ec2.id
left join externalSigns es2 on n2.externalSignID = es2.id
left join nodes ni on ni.id = n1.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id
left join nodes bn on bn.id = b.nodeID
left join externalCodes bec on bec.id = bn.externalCodeID
left join externalSigns bes on bes.id = bn.externalSignID
left join regulatorStates rs on rs.id = b.regulatorStateID
left join operators o on o.id = l.operatorID
left join organizations org on org.id = l.organizationID
left join standards st on st.id = b.standardID

WHERE n1.fileID=$fileID$ AND n1.internalNodeID IS NULL
AND l.removed=0 AND n1.removed=0 AND n2.removed=0
