select 
l.id,
ec1.name,
n1.externalNodeName,
ec2.name,
n2.externalNodeName,
hps.pipeSectLength,
hps.diameterInternal

 from linesobj l 
join heatpipesections hps on hps.lineid=l.id
join nodes n1 on n1.id=l.nodeid1 and n1.removed=0
join externalcodes ec1 on ec1.id=n1.externalcodeID
join nodes n2 on n2.id=l.nodeid2 and n2.removed=0
join externalcodes ec2 on ec2.id=n2.externalcodeID
left join UT_OUT on UT_OUT.lineid =l.id
left JOIN (
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON calc.fileID=n1.fileID AND calc.cid=UT_OUT.calculationID

where l.removed=0
and n1.internalnodeid is null and n1.fileID in ($fragments$)
and UT_OUT.id is null

--AND_NODE
