SELECT 
  hs.name,  
  months.name,

  flushinghs_temp1,
  tv.tn,
  flushinghs,

  lvv.Vot1,
  lvv.Vot2,
  lvv.Vot1+lvv.Vot2 AS Vall,

  (SELECT Vall)*flushinghs*(flushinghs_temp1-flushinghs_temp2)/1000 AS Q



FROM heatLosesSource hls
LEFT JOIN tempView tv ON tv.hID=hls.heatSourceID AND tv.sezon=2
LEFT JOIN losesVolumesView lvv ON lvv.heatSourceID=hls.heatSourceID

LEFT JOIN heatSources hs ON hs.id=hls.heatSourceID
LEFT JOIN months ON months.id=tv.m


WHERE tv.m = month_flushinghs
  

ORDER BY tv.r
