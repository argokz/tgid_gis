SELECT 

-- hlsm.heatSourceID, hlsm.m,
  hs.name,  
  months.name,


hlsm.netWaterExpRet,hlsm.workCountRet,
hlsm.workCountRet*hlsm.netWaterExpRet*hlsm.regCountRet AS avgGsarzGRet,
  
hlsm.regCountRet,
  
hlsm.workCountRet*hlsm.netWaterExpRet*hlsm.regCountNodeRet AS avgGsarzNodeGRet,


(SELECT avgGsarzGRet)+(SELECT avgGsarzNodeGRet) AS avgGsarzGRetAll,

hlsm.regCountNodeRet,
tv.tgP, tv.tn,

(SELECT avgGsarzGRetAll)*(tv.tgP-tv.tn) AS Qsarz
  
FROM heatLosesSourceMonths hlsm
LEFT JOIN tempView tv ON tv.hID=hlsm.heatSourceID AND tv.r=hlsm.r


LEFT JOIN heatSources hs ON hs.id=hlsm.heatSourceID
LEFT JOIN months ON months.id=tv.m

WHERE hlsm.heatSourceID IN (SELECT id FROM heatLosesSourceList)
