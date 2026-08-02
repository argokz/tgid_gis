SELECT top 2147483647 
l.id,
das.name as 'sost', 
d.dispatcherSwitch as 'name_zd', 
eci.name as 'kod_p', ni.externalNodeName as 'uzel_p', esi.name as 'pr_p', 
ec1.name as 'kod1',
n1.externalnodename as 'uzel1',
case l.externalSignLineID    when 1 then ' '    when 2 then 'Ï'    when 3 then 'Î'    when 4 then 'Ï'    when 5 then 'Î' end as 'pr1',
ec2.name as 'kod2', 
n2.externalNodeName as 'uzel2',
case l.externalSignLineID    when 1 then ' '    when 2 then 'Ï'    when 3 then 'Î'    when 4 then 'Î'    when 5 then 'Ï' end as 'pr2',

d.diameterCondit as 'diametr', 
d.relatLeakage as 'otn_kv', 
d.partDempOpen as 'proz_kv', 
l.hydroRes as 'sopr',
'' AS aa1,
'' AS aa2,
hs.sourceName



from dampers d
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
join damperArmatureStates das on das.id = d.damperArmatureStateID
left join operators o on o.id = l.operatorID
left join organizations org on org.id = l.organizationID
LEFT JOIN heatSources hs ON hs.id = ec1.heatSourceID


WHERE n1.fileID=$fileID$ AND l.removed=0 AND n1.removed=0 AND n2.removed=0 AND n1.fileID=n2.fileID
