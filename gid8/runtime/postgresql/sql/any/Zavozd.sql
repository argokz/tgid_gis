select
--rc.buildHeight,
--US_OUT.pih,
--n.geoMarkTopTube,
--PT_OUT.id,

n.id, ec.name as kod, n.externalnodename, rc.name
from nodes n
join externalcodes ec on ec.id=n.externalcodeid 
join realconsumers rc on rc.nodeID=n.id

join PT_OUT on PT_OUT.nodeid =n.id

left JOIN (
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON calc.fileID=n.fileID AND calc.cid=PT_OUT.calculationID -- AND calc.cid=US_OUT.calculationID

join US_OUT on US_OUT.nodeid =n.id and calc.cid=US_OUT.calculationID

where n.removed=0 
and US_OUT.externalsign = 2
and US_OUT.pih < rc.buildHeight
and n.fileID in ($fragments$)

--AND_NODE
