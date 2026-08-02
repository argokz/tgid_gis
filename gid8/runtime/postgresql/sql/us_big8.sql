SELECT
n.id,
eci.name as kod_p,
ni.externalNodeName as uzel_p,
esi.name as pr_p,
ec.name as kod,
n.externalNodeName as uzel,
es.name as pr,

n.nodeName as "Наименование узла ПТС",
n.pipelineSign as "Признак трубопровода.",

n.geoMarkTopTube as "Геод.отметка оси трубы, м",
n.geoMarkNodeArea as "Геод.отметка поверхности земли,м",
nt.name as "Тип узла",

n.calcPressFlow as "Измеренное давление на подающем теплопроводе, м.вод.ст",
n.calcPressRet as "Измеренное давление на обратном теплопроводе, м.вод.ст",

org.name as "Владелец",
n.registNumber as "Регистрационный номер",
n.PICdate as "Дата ввода в эксплуатацию",
n.lastRepairDate as "Дата последнего ремонта",
n.archiveChangeDate as "Дата изменения архива",
o.user_name as "Оператор архивации"

from nodes n

left join nodes ni on ni.id = n.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id


left join externalCodes ec on ec.id = n.externalCodeID
left join externalSigns es on es.id = n.externalSignID
left join passwords o on o.id = n.operatorID
left join nodeTypes nt on nt.id = n.nodeTypeID

left join organizations org on org.id = n.organizationID

WHERE n.removed=0 and n.internalnodeid is null
