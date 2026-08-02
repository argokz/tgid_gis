select l.id, 
ec1.name as "Код РС начальный", 
n1.externalNodeName as "Начальный узел", 
ec2.name as "Код РС конечный", 
n2.externalNodeName as "Конечный узел", 
hps.diameterCondit as "Условный диаметр, мм",
hps.diameterInternal as "Внутренний диаметр, мм",
hps.wallThickness as "Толщина стенки, мм",
hps.pipeSectLength as "Протяженность, мм",
tt.name as "Вид прокладки"
from linesobj l
join heatpipesections hps on hps.lineid =l.id
join nodes n1 on n1.id=l.nodeid1 and n1.removed=0
join externalcodes ec1 on ec1.id=n1.externalcodeid
join nodes n2 on n2.id=l.nodeid2 and n2.removed=0
join externalcodes ec2 on ec2.id=n2.externalcodeid
left join tubingTypes tt on tt.id = hps.tubingTypeID
-- ++ --
--join (values %1) mark_lines(ord, id) on mark_lines.id=l.id
where l.removed=0 and n1.internalnodeid is null
--order by mark_lines.ord
