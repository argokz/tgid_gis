SELECT 
  hs.name,  
  months.name,

-- lsm.heatSourceID, lsm.r,lsm.m,lsm.sezon,
  psv.fillingG,
  psv.avgGpressingG,
  psv.avgGflushingG,
  0 AS i1,  -- 'Испытание1'
  0 AS i2,  -- 'Испытание2'
  psv.avgGsarzG,
  psv.normG,
  psv.fillingG+psv.avgGpressingG+psv.avgGflushingG+psv.avgGsarzG+psv.normG AS Gall


FROM psvView psv


LEFT JOIN heatSources hs ON hs.id=psv.heatSourceID
LEFT JOIN months ON months.id=psv.m


ORDER BY
  psv.heatSourceID, psv.r
