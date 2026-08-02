SELECT hs.name, 
coeffMsFlowNorms1_r,coeffMsRetNorms1_r,coeffMsUndergroundNorms1_r,
coeffRsFlowNorms1_r,coeffRsRetNorms1_r,coeffRsUndergroundNorms1_r,
coeffBasementFlowNorms1_r,coeffBasementRetNorms1_r,coeffBasementUndergroundNorms1_r,
coeffHarnessFlowNorms1_r,coeffHarnessRetNorms1_r,coeffHarnessUndergroundNorms1_r,
coeffMsFlowNorms3_r,coeffMsRetNorms3_r,coeffMsUndergroundNorms3_r,
coeffRsFlowNorms3_r,coeffRsRetNorms3_r,coeffRsUndergroundNorms3_r,
coeffBasementFlowNorms3_r,coeffBasementRetNorms3_r,coeffBasementUndergroundNorms3_r,
coeffHarnessFlowNorms3_r,coeffHarnessRetNorms3_r,coeffHarnessUndergroundNorms3_r
-- , 

-- hls.id 

FROM heatLosesSource hls 
LEFT JOIN heatSources hs ON hs.id=hls.heatSourceID
