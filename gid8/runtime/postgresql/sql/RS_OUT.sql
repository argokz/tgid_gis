SELECT 
  l.id,
  RS_OUT.id AS id2,
  eci.name as 'kod_p', ni.externalNodeName as 'uzel_p', esi.name as 'pr_p',
  ec1.name as 'kod1', 
  n1.externalNodeName as 'uzel1', 
--   rs_out.externalSignLineID,
  case rs_out.externalSignLineID
    when 1 then ' '
    when 2 then 'Ï'
    when 3 then 'Î'
    when 4 then 'Ï'
    when 5 then 'Î'
  end as 'pr1',
  ec2.name as 'kod2', n2.externalNodeName as 'uzel2',
  case rs_out.externalSignLineID
    when 1 then ' '
    when 2 then 'Ï'
    when 3 then 'Î'
    when 4 then 'Î'
    when 5 then 'Ï'
  end as 'pr2',

--  a4,
--  a8,

  ec3.name as 'kod3', 
  n3.externalNodeName as 'uzel3',
--  pr3,
  case rd.pipelineSignID
    when 1 then 'Ï'
    when 2 then 'Î'
  end as 'pr3',


  a11,
  a12,
  a13,
  a14,
  a15,
  a16,
  a17,
  a18,
  dx,
  a19


from RS_OUT
join linesobj l on l.id = rs_out.lineID
left join pressRegulators rd ON rd.lineID=l.id


join nodes n1 on n1.id = l.nodeID1
join nodes n2 on n2.id = l.nodeID2

left join nodes n3 on n3.id = pressRegulators.nodeID
left join externalCodes ec3 on ec3.id = n3.externalCodeID


join externalCodes ec1 on ec1.id = n1.externalCodeID
join externalCodes ec2 on ec2.id = n2.externalCodeID
join externalSigns es1 on es1.id = n1.externalSignID
join externalSigns es2 on es2.id = n2.externalSignID
join externalSignLine esl on esl.id = rs_out.externalSignLineID
left join nodes ni on ni.id = n1.internalNodeID
left join externalCodes eci on eci.id = ni.externalCodeID
left join externalSigns esi on esi.id = ni.externalSignID
WHERE n1.fileID=$fileID$ AND n1.internalNodeID IS NULL
AND calculationID=$calculationID$
AND l.removed=0 AND n1.removed=0 AND n2.removed=0

