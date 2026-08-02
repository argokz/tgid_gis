SELECT top 2147483647 
  l.id,
  NS_OUT.id AS id2,
  eci.name as 'kod_p', ni.externalNodeName as 'uzel_p', esi.name as 'pr_p',
  ec1.name as 'kod1', n1.externalNodeName as 'uzel1', 
  case ns_out.externalSignLineID
    when 1 then ' '
    when 2 then 'Ï'
    when 3 then 'Î'
    when 4 then 'Ï'
    when 5 then 'Î'
  end as 'pr1',
  ec2.name as 'kod2', n2.externalNodeName as 'uzel2',
  case ns_out.externalSignLineID
    when 1 then ' '
    when 2 then 'Ï'
    when 3 then 'Î'
    when 4 then 'Î'
    when 5 then 'Ï'
  end as 'pr2',
  sos,
  a4, 
  a8, 
  
  sp.h_min AS a9,
  sp.q_min AS a10,
  sp.h_max AS a11,
  sp.q_max AS a12,
  
  a13, a14, a15, a16, a17, a18, 
  sp.tip_nas AS a19
  
from NS_OUT

join linesobj l on l.id = ns_out.lineID
join nodes n1 on n1.id = l.nodeID1
join nodes n2 on n2.id = l.nodeID2

join pumps p ON p.lineID=l.id
left join standardPumps sp on sp.id = p.standardPumpID


join externalCodes ec1 on ec1.id = n1.externalCodeID
join externalCodes ec2 on ec2.id = n2.externalCodeID
join externalSigns es1 on es1.id = n1.externalSignID
join externalSigns es2 on es2.id = n2.externalSignID
left join externalSignLine esl on esl.id = ns_out.externalSignLineID
left join nodes ni on ni.id = n1.internalNodeID
left join externalCodes eci on eci.id = ni.externalCodeID
left join externalSigns esi on esi.id = ni.externalSignID



WHERE n1.fileID=$fileID$ AND calculationID=$calculationID$
-- AND n1.internalNodeID IS NULL

AND l.removed=0 AND n1.removed=0 AND n2.removed=0
