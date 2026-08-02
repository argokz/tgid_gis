SELECT 
  hs.name,  
  months.name,

-- lsm.heatSourceID, lsm.r,lsm.m,lsm.sezon,
  psv.fillingQ,
  psv.avgGpressingQ,
  psv.avgGflushingQ,
  0 AS i1, -- 'Испытание1'
  0 AS i2, -- 'Испытание2'
  psv.avgGsarzQ,
  psv.normQ,
  psv.fillingQ+psv.avgGpressingQ+psv.avgGflushingQ+psv.avgGsarzQ+psv.normQ AS Qall


FROM psvView psv


LEFT JOIN heatSources hs ON hs.id=psv.heatSourceID
LEFT JOIN months ON months.id=psv.m


ORDER BY
  psv.heatSourceID, psv.r
