--row_number() over(partition by n0.id order by n0.externalNodeName, n.externalNodeName,n.externalSignID) as n1,
--row_number() over(partition by n.externalNodeName order by n0.externalNodeName, n.externalNodeName,n.externalSignID) as n2,

select (

kod1,
name1,
kod2, 
name2,
po,

pih,
r

from (


select 

row_number() over(partition by n0.id order by n0.externalNodeName, n.externalNodeName,n.externalSignID) as n1,
row_number() over(partition by n.externalNodeName order by n0.externalNodeName, n.externalNodeName,n.externalSignID) as n2,

ec0.name as kod1,
n0.externalNodeName as name1,
ec.name as kod2, 
n.externalNodeName as name2,

case n.externalSignID
when 2 then 'Ï'
when 3 then 'Î'
end as po,

US_OUT.pih,
UT_OUT.a13*
IIF (l.nodeID1=cn.connectID AND l.nodeID2 = n0.id, -1, 1) AS r



from connectNodes cn
join nodes n ON n.id=cn.nodeID AND n.removed=0
join externalCodes ec ON ec.id=n.externalCodeID

JOIN
(
	SELECT 
	c.fileID,
	max(c.id) AS cid
	FROM CALCULATION c
	LEFT JOIN fragments fr ON fr.id=c.fileID
	GROUP BY c.fileID
) calc ON n.fileID=calc.fileID 


join nodes n0 ON n.internalNodeID=n0.id AND n0.removed=0
join externalCodes ec0 ON ec0.id=n0.externalCodeID
join US_OUT on US_OUT.nodeID=n.id AND US_OUT.calculationID=calc.cid
--8715 --calc.cid

join linesobj l on l.removed=0 AND (l.nodeID1=cn.connectID AND l.nodeID2 = n0.id) OR (l.nodeID2=cn.connectID AND l.nodeID1 = n0.id) 
AND (l.externalSignLineID = n.externalSignID OR l.externalSignLineID=1) AND l.removed=0

join UT_OUT on UT_OUT.lineID=l.id AND UT_OUT.externalSignLineID=n.externalSignID AND UT_OUT.calculationID=calc.cid
--=8715   --calc.cid

WHERE n0.fileID=74

) __

ORDER BY
   kod1,
   name1,
   kod2, 
   name2,
   po,
