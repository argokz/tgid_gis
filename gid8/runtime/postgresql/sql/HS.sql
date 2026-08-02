SELECT
n.id,
ec.name as kod, n.externalNodeName as uzel, es.name as pr,
n.geoMarkNodeArea as "Геодезическая отметка поверхности земли,м", 
n.geoMarkTopTube as "Геодезическая отметка оси трубы,м", 

--n.displaySign as podp, 
--n.calcPressFlow as pP_fact, n.calcPressRet as pO_fact, n.archiveChangeDate as date_archives, n.memo as memo, 
n.nodeName as "Наименование", 
--n.scheme as "Схема", 
--n.gpsCoords as GPS координаты, 
n.inventNumber as "Инвентарный номер", 
n.belongMagistralSite as "Принадлежность участку МС", n.belongDistSite as "Принадлежность РС", n.pipelineSign as "Признак трубопровода", 
n.belongHN as "Принадлежность тепловым сетям",
--ps.heightTubeMark as "Высотная отметка оси трубы",  
--ps.heightAreaMark as "Высотная отметка местности",  
ps.state as sost
from pumpStations ps
join nodes n on n.id = ps.nodeID
left join externalCodes ec on ec.id = n.externalCodeID
left join externalSigns es on es.id = n.externalSignID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND n.removed=0
