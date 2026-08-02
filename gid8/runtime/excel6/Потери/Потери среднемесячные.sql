SELECT
    heatSources.name,
    months.name,

potNP,
potNO,
potPodz,
potAll,
V1,
Vall


FROM avgHeatLosesMonth ahlm
LEFT JOIN heatSources ON heatSources.id=ahlm.heatSourceID
LEFT JOIN months ON months.id=ahlm.m
LEFT JOIN heatLosesSourceMonths hlsm ON hlsm.heatSourceID=ahlm.heatSourceID AND hlsm.r=ahlm.r