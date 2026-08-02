SELECT top 2147483647
n.id,
ec.name as 'kod', n.externalNodeName as 'uzel', es.name as 'pr',
hs.name as 'Наименование источника', 
--hs.sourceName as 'Код источника', 
hs.hSourcePower as 'n_rasp', hs.hSourcePowerInst as 'n_ustan', hs.hSeasonBeginDate as 'begin_year', 
hs.hSeasonEndDate as 'end_year', hs.temperDWflowSummer as 't1_leto', hs.temperDWretSummer as 't2_leto', hs.specVolHS as 'a24', hs.specVolVent as 'a25', hs.expendDWnorm1 as 'G1n_ist_1', 
hs.expendDWnorm2 as 'G1n_ist_2', hs.expendDWnorm3 as 'G1n_ist_3', hs.expendDWnorm4 as 'G1n_ist_4', hs.expendDWnorm5 as 'G1n_ist_5', 
hs.managerPhone as 'phone_manager', hs.controllerPhone as 'phone_controller', hs.heightTubeMark as 'Высотная отметка оси трубы', 
hs.heightAreaMark as 'Высотная отметка местности', cs.name as 'sost', 
o.name as 'Оператор', 
hst.name as 'Тип источника', 
org.name as 'Владелец'
--, 
--nt.name as 'name_typ', n.fileID
from heatSources hs
join nodes n on n.id = hs.nodeID
left join externalCodes ec on ec.id = n.externalCodeID
left join externalSigns es on es.id = n.externalSignID
left join operators o on o.id = n.operatorID
left join nodeTypes nt on nt.id = n.nodeTypeID
left join consumerStates cs on cs.id = hs.stateID
left join heatSourceTypes hst on hst.id = hs.hSourceTypeID
left join organizations org on org.id = n.organizationID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND n.removed=0
