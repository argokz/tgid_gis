SELECT  
n.id,
ec.name as kod,
n.externalNodeName as uzel,
es.name as pr,

n.geoMarkNodeArea as "Геод.отметка поверхности земли,м",
n.geoMarkTopTube as geodz,

nt.name as name_typ


from nodes n

left join nodes ni on ni.id = n.internalNodeID
left join externalCodes eci on ni.externalCodeID = eci.id
left join externalSigns esi on ni.externalSignID = esi.id


left join externalCodes ec on ec.id = n.externalCodeID
left join externalSigns es on es.id = n.externalSignID
left join operators o on o.id = n.operatorID
left join nodeTypes nt on nt.id = n.nodeTypeID

left join organizations org on org.id = n.organizationID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL AND n.removed=0
