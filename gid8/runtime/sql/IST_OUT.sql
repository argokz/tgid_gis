SELECT top 2147483647 
  n.id,
  ec1.name as 'kod1', n1.externalNodeName as 'uzel1', 
  case ist_out.externalSignLineID
    when 1 then ' '
    when 2 then 'Ï'
    when 3 then 'Î'
    when 4 then 'Ï'
    when 5 then 'Î'
  end as 'pr1',
  ec2.name as 'kod2', n2.externalNodeName as 'uzel2',
  case ist_out.externalSignLineID
    when 1 then ' '
    when 2 then 'Ï'
    when 3 then 'Î'
    when 4 then 'Î'
    when 5 then 'Ï'
  end as 'pr2',
  ec.name as 'kod', n.externalNodeName as 'uzel', es.name as 'pr', 
  esl.name, calc.name,
  kod_ist, t1, t2
from IST_OUT
join nodes n on
n.id = ist_out.nodeID
join externalCodes ec on
ec.id = n.externalCodeID
join externalSigns es on
es.id = n.externalSignID
join externalSignLine esl on
esl.id = ist_out.externalSignLineID
join CALCULATION calc on
calc.id = ist_out.calculationID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND calculationID=$calculationID$
AND n.removed=0 
