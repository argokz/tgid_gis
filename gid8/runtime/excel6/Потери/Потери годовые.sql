SELECT
    heatSources.name,
    months.name,

potNP*workCount*24,
potNO*workCount*24,
potPodz*workCount*24,
potAll*workCount*24,
V1*workCount*24,
Vall*workCount*24


FROM avgHeatLosesMonth ahlm
LEFT JOIN heatSources ON heatSources.id=ahlm.heatSourceID
LEFT JOIN months ON months.id=ahlm.m
LEFT JOIN heatLosesSourceMonths hlsm ON hlsm.heatSourceID=ahlm.heatSourceID AND hlsm.r=ahlm.r