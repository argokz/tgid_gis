SELECT
  hs.name,
  tab2.mesto,
  tab2.designCapacity,
  tab2.quantity,
  tab2.height,
  tab2.diameter
FROM heatLosesMain tab1

LEFT JOIN 
(
    SELECT 
      IIF(NOT ec.objectID=2, ec.heatSourceID, ecm.heatSourceID) AS heatSourceID,
      CONCAT(ec.name, ' ', n.externalNodeName) mesto,
      
      b.designCapacity,
      b.quantity,
      b.height,
      b.diameter
      -- PI()*b.diameter/1000*(b.height+b.diameter/2)/1000*2 AS F
    
    FROM tankBatteries b
    JOIN nodes n ON b.nodeID = n.id
    JOIN externalCodes ec ON n.externalCodeID = ec.id
    left join externalCodes ecm on ec.belongMagistral = ecm.id AND ec.objectID=2
    LEFT JOIN heatLosesMain hlm ON hlm.id=1
) tab2 ON 1=1

LEFT JOIN heatSources hs ON hs.id=tab2.heatSourceID