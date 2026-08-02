SELECT 
  
  hs.name,  
  months.name,

IIF(tv.m=pressingMonth1, 'весенняя',IIF(tv.m=pressingMonth2, 'осенняя', '')) AS opr,


IIF(tv.m=pressingMonth1, netTempPressing1,IIF(tv.m=pressingMonth2, netTempPressing2, 0)) AS tset,

tv.tn,

IIF(tv.m=pressingMonth1, spring_pressing,IIF(tv.m=pressingMonth2, autumn_pressing, 0)) AS percent1,

lvv.V1,
lvv.V2,
lvv.Vpodv,
lvv.VobM,
lvv.VobR,
lvv.V1+lvv.V2+lvv.Vpodv+lvv.VobM+lvv.VobR AS Vall,

(SELECT Vall)*(hls.netTempPressing1-hls.coldTempPressing1)/1000*(SELECT percent1)/100 AS avgQpressing


FROM heatLosesSource hls
LEFT JOIN tempView tv ON tv.hID=hls.heatSourceID AND tv.sezon=2
LEFT JOIN losesVolumesView lvv ON lvv.heatSourceID=hls.heatSourceID

LEFT JOIN heatSources hs ON hs.id=hls.heatSourceID
LEFT JOIN months ON months.id=tv.m


WHERE tv.m = pressingMonth1 OR tv.m = pressingMonth2 AND hls.heatSourceID IN (SELECT id FROM heatLosesSourceList)
  

ORDER BY tv.r
