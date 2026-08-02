SELECT top 2147483647
l.id,
--eci.name as 'kod_p', ni.externalNodeName as 'uzel_p', esi.name as 'pr_p', 
rs.name as 'sost', 

ec1.name as 'kod1', n1.externalNodeName as 'uzel1', 
  case externalSignLineID    when 1 then ' '    when 2 then 'Ï'    when 3 then 'Î'    when 4 then 'Ï'    when 5 then 'Î'  end as 'pr1',
  ec2.name as 'kod2', n2.externalNodeName as 'uzel2',
  case externalSignLineID    when 1 then ' '    when 2 then 'Ï'    when 3 then 'Î'    when 4 then 'Î'    when 5 then 'Ï'  end as 'pr2',


  ec3.name as 'kod3', n3.externalNodeName as 'uzel3',
  case b.pipelineSignID
    when 1 then 'Ï'
    when 2 then 'Î'
  end as 'pr3',


--  bec.name as 'uzu_k', 
--  bn.externalNodeName as 'uzu', 
--  bes.name as 'przu',

b.q as 'q', 
b.deltaQ as 'delta_q', 

--b.standardTubeLink as 'kod_truba', 
b.length as 'dln', 
b.diameterInternal as 'diam', 
b.tubeRoughness as 'scher', 
b.resCoeffsSum as 'sum_m_s', 

l.hydroRes as 'sopr',
b.locInstall as 'ustanovka',
hs.sourceName



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

left join nodes n3 on n3.id = b.nodeID
left join externalCodes ec3 on ec3.id = n3.externalCodeID
left join externalSigns es3 on es3.id = n3.externalSignID

LEFT JOIN heatSources hs ON hs.id = ec1.heatSourceID


WHERE n1.fileID=$fileID$ AND l.removed=0 AND n1.removed=0 AND n2.removed=0 AND n1.fileID=n2.fileID
