select 
n.id, 
ec.name, 
n.externalnodename, 
nt.name 
from nodes n
join externalcodes ec on ec.id=n.externalcodeid 
left join nodetypes nt on nt.id=n.nodetypeid
-- ++ --
where n.removed=0 and n.internalnodeid is null

