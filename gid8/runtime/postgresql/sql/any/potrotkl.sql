select

n.id, ec.name, n.externalnodename, pt.name
from nodes n
join externalcodes ec on ec.id=n.externalcodeid 
join (
select gc.nodeID, gc.consumerStateID, gc.name from generalizedconsumers gc
union
select rc.nodeID, rc.consumerStateID, rc.name from realconsumers rc
) pt on pt.nodeID=n.id

left join (

select 
n.id
from nodes n

join PT_OUT on PT_OUT.nodeid =n.id

JOIN (
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON calc.fileID=n.fileID AND calc.cid=PT_OUT.calculationID
where 
n.removed=0 and n.fileID in ($fragments$)

) pt_v on pt_v.id=n.id

where pt_v.id is Null

and n.removed=0 and n.fileID in ($fragments$)

--AND_NODE
