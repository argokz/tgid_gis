SELECT l.id,
 IIF(n1.nodeName IS NULL, n1.externalNodeName, n1.nodeName) AS name1,
 IIF(n2.nodeName IS NULL, n2.externalNodeName, n2.nodeName) AS name2,

 im.name AS [Теплоизоляционный материал], 
 isolThickness AS [Толщина тепловой изоляции, мм], 
 em.name AS [Материал наружного покрытия], 
 externCoverThick AS [Толщина наружного покрытия], 
 am.name AS [Материал антикоррозионного покрытия] 


from heatPipeSections hps
join linesobj l on l.id = hps.lineID
join nodes n1 on n1.id = l.nodeID1
join nodes n2 on n2.id = l.nodeID2
join externalCodes ec1 on ec1.id = n1.externalCodeID
join externalCodes ec2 on ec2.id = n2.externalCodeID
join externalSigns es1 on es1.id = n1.externalSignID
join externalSigns es2 on es2.id = n2.externalSignID

left join isolMaterials im on im.id = hps.isolMaterialID
left join externalMaterials em on em.id = hps.externMaterialID
left join anticorrMaterials am on am.id = hps.anticorrMaterialID

WHERE n1.internalNodeID IS NULL
AND l.removed=0 AND l.id IN ($tuple$)
