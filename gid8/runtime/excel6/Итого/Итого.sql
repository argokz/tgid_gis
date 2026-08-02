SELECT
    heatSources.name,
    months.name,

-- potNP*workCount*24,
-- potNO*workCount*24,
-- potPodz*workCount*24,
ahlm.potAll*lsm.workCount*24 AS isolQ,
tb.qTB*lsm.workCount*24 AS qTB,

psv.normQ,
(IFNULL(psv.fillingQ,0)+IFNULL(psv.avgGpressingQ,0)+IFNULL(psv.avgGflushingQ,0)+IFNULL(psv.avgGsarzQ,0)) AS reglQ,

psv.normG,
IFNULL(psv.fillingG,0)+IFNULL(psv.avgGpressingG,0)+IFNULL(psv.avgGflushingG,0)+IFNULL(psv.avgGsarzG,0) AS reglG,

(psv.normG+(SELECT reglG)) AS gAll,
((SELECT isolQ)+(SELECT qTB)+psv.normQ+(SELECT reglQ)) AS allQ



FROM avgHeatLosesMonth ahlm
JOIN heatLosesMain hlm ON hlm.id=1
LEFT JOIN heatLosesSourceMonths lsm ON lsm.heatSourceID=ahlm.heatSourceID AND lsm.r=ahlm.r
LEFT JOIN tankbatteryView tb ON tb.heatSourceID AND tb.r=ahlm.r
LEFT JOIN losesvolumesview lv ON lv.heatSourceID=lsm.heatSourceID
LEFT JOIN psvView psv ON psv.r=ahlm.r AND psv.heatSourceID=ahlm.heatSourceID
LEFT JOIN heatSources ON heatSources.id=ahlm.heatSourceID
LEFT JOIN months ON months.id=ahlm.m
