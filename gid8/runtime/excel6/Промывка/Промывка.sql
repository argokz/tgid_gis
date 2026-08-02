SELECT 
 
  hs.name,  
  months.name,

  IIF(flushing_flow<>0 AND flushing_ret<>0, 2, IIF(flushing_flow=0 AND flushing_ret=0, 0, 1)) kolv,

  flushing_temp1,
  tv.tn,
  flushing,
  lvv.V1,
  lvv.V2,
  lvv.Vpodv,
  lvv.VobM,
  lvv.VobR,
  lvv.V1+lvv.V2+lvv.Vpodv+lvv.VobM+lvv.VobR AS Vall,
  (SELECT Vall)*flushing*(SELECT kolv)/2*(flushing_temp1-flushing_temp2)/1000 AS Q



FROM heatLosesSource hls
LEFT JOIN tempView tv ON tv.hID=hls.heatSourceID AND tv.sezon=2
LEFT JOIN losesVolumesView lvv ON lvv.heatSourceID=hls.heatSourceID

LEFT JOIN heatSources hs ON hs.id=hls.heatSourceID
LEFT JOIN months ON months.id=tv.m


WHERE tv.m = month_flushing
 

ORDER BY tv.r
