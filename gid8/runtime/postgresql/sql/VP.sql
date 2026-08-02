SELECT
    n.id,
    ec.name as kod, n.externalNodeName as uzel, es.name as pr, 
   WDO, hydroRes


from WDOdevices spn
join nodes n on n.id = spn.nodeID
join externalCodes ec on ec.id = n.externalCodeID
join externalSigns es on es.id = n.externalSignID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND n.removed=0
