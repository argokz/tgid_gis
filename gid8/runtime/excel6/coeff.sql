SELECT hs.name, 
coeffMsFlowNorms1,coeffMsRetNorms1,coeffMsUndergroundNorms1,
coeffRsFlowNorms1,coeffRsRetNorms1,coeffRsUndergroundNorms1,
coeffBasementFlowNorms1,coeffBasementRetNorms1,coeffBasementUndergroundNorms1,
coeffHarnessFlowNorms1,coeffHarnessRetNorms1,coeffHarnessUndergroundNorms1,
coeffMsFlowNorms3,coeffMsRetNorms3,coeffMsUndergroundNorms3,
coeffRsFlowNorms3,coeffRsRetNorms3,coeffRsUndergroundNorms3,
coeffBasementFlowNorms3,coeffBasementRetNorms3,coeffBasementUndergroundNorms3,
coeffHarnessFlowNorms3,coeffHarnessRetNorms3,coeffHarnessUndergroundNorms3
-- , 

-- hls.id 

FROM heatLosesSource hls 
LEFT JOIN heatSources hs ON hs.id=hls.heatSourceID
