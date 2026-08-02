SELECT top 2147483647 
n.id,
eci.name as 'kod_p',
ni.externalNodeName as 'uzel_p',
esi.name as 'pr_p',
ec.name as 'kod',
n.externalNodeName as 'uzel',
es.name as 'pr',

n.nodeName as 'Наименование узла ПТС',
n.pipelineSign as 'Признак трубопровода.',

n.geoMarkTopTube as 'geodz',
n.geoMarkNodeArea as 'Геод.отметка поверхности земли,м',
nt.name as 'name_typ',

n.calcPressFlow as 'pP_fact',
n.calcPressRet as 'pO_fact',

org.name as 'Владелец',
--org.name as 'kod_owner',
-- n.organizationID as 'kod_owner',
n.registNumber,
n.PICdate as 'datenew',
n.lastRepairDate as 'date_rem',
n.archiveChangeDate as 'date_archives',
o.user_name as 'operator'

-- n.displaySign as 'podp',
--n.scheme as 'Схема',

--n.gpsCoords as 'GPS координаты',
--n.inventNumber as 'Инвентарный номер',
--n.belongMagistralSite as 'Принадлежность участку МС',

--n.belongDistSite as 'Принадлежность РС',
--n.magistralSite as 'Участок МС',
--n.distSite as 'Участок РС',

--n.belongHN as 'Принадлежность тепловым сетям',
--n.passport as 'Паспорт'

from nodes n

left join nodes ni on ni.id = n.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id


left join externalCodes ec on ec.id = n.externalCodeID
left join externalSigns es on es.id = n.externalSignID
left join passwords o on o.id = n.operatorID
left join nodeTypes nt on nt.id = n.nodeTypeID

left join organizations org on org.id = n.organizationID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND n.removed=0
