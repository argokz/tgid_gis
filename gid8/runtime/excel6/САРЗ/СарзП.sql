SELECT 
-- hlsm.heatSourceID, hlsm.m,

  hs.name,  
  months.name,

hlsm.netWaterExpFlow,hlsm.workCountFlow,
hlsm.workCountFlow*hlsm.netWaterExpFlow*hlsm.regCountFlow AS avgGsarzGFlow,
  
hlsm.regCountFlow,
  
hlsm.workCountFlow*hlsm.netWaterExpFlow*hlsm.regCountNodeFlow AS avgGsarzNodeGFlow,


(SELECT avgGsarzGFlow)+(SELECT avgGsarzNodeGFlow) AS avgGsarzGFlowAll,

hlsm.regCountNodeFlow,
tv.tgP, tv.tn,

(SELECT avgGsarzGFlowAll)*(tv.tgP-tv.tn) AS Qsarz
  
FROM heatLosesSourceMonths hlsm
LEFT JOIN tempView tv ON tv.hID=hlsm.heatSourceID AND tv.r=hlsm.r

LEFT JOIN heatSources hs ON hs.id=hlsm.heatSourceID
LEFT JOIN months ON months.id=tv.m

WHERE hlsm.heatSourceID IN (SELECT id FROM heatLosesSourceList)
