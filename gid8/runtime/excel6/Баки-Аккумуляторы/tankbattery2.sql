SELECT
  hs.name,
  months.name,
  TAB2.tn,
  TAB2.tgO,
  SUM(TAB2.q),
  TAB2.workCount,

--  TAB2.name, 
--  TAB2.externalNodeName,
--  TAB2.designCapacity,
-- TAB2.quantity,
--  TAB2.height,
--  TAB2.diameter,
--  TAB2.F,
  SUM(TAB2.q)
--  TAB2.TT
FROM 
  heatLosesMain h1

JOIN 
(
SELECT 
  IIF(ec.objectID IN (1,3,4,5), ec.heatSourceID, ecm.heatSourceID) AS heatSourceID,
  ec.name, n.externalNodeName,
tv.r,
tv.m,
tv.sezon,
tv.tn,
tv.tgO,

b.designCapacity,
b.quantity,
b.height,
b.diameter,
PI()*b.diameter/1000*(b.height+b.diameter/2)/1000*2 AS F,
PI()*b.diameter/1000*(b.height+b.diameter/2)/1000*2*hlm.tankbattery_q*(tv.tgO-tv.tx)/(tvg.tgO-tvg.tx) AS q,
(tv.tgO-tv.tx)/(tvg.tgO-tvg.tx) AS TT,
tv.workCount

FROM tankBatteries b
JOIN nodes n ON b.nodeID = n.id
JOIN externalCodes ec ON n.externalCodeID = ec.id
left join externalCodes ecm on ec.belongMagistral = ecm.id AND ec.objectID=2
LEFT JOIN tempview tv ON tv.hID=IIF(ec.objectID IN (1,3,4,5), ec.heatSourceID, ecm.heatSourceID)
LEFT JOIN tempview tvg ON tv.hID=IIF(ec.objectID IN (1,3,4,5), ec.heatSourceID, ecm.heatSourceID) AND tvg.m=15
LEFT JOIN heatLosesMain hlm ON hlm.id=1
) TAB2
ON 1=1

LEFT JOIN heatLosesSourceMonths lsm ON lsm.heatSourceID=TAB2.heatSourceID AND lsm.r=TAB2.r



LEFT JOIN heatSources hs ON hs.id=TAB2.heatSourceID
LEFT JOIN months ON months.id=TAB2.m


GROUP BY
  TAB2.heatSourceID,
  lsm.id,
  TAB2.TT,
  TAB2.workCount,
  TAB2.r,
  TAB2.m,
  TAB2.sezon,
  TAB2.tn,
  TAB2.tgO,
  months.name,
  hs.name



ORDER BY TAB2.heatSourceID, TAB2.r