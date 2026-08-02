SELECT 
n.id, n.fileID, n.internalNodeID, n.externalCodeID, n.externalSignID, n.displaySign, n.x, n.y, nodeTypeID, externalNodeName, nodeName,geoMarkTopTube,geoMarkNodeArea,
n.calcPressFlow,
n.calcPressRet,

CASE
    WHEN rc.id IS NOT NULL THEN rc.consumerStateID
    WHEN gc.id IS NOT NULL THEN gc.consumerStateID
    ELSE 1
END AS consumerStateID,

CASE
    WHEN rc.id IS NOT NULL THEN rc.buildHeight
    WHEN gc.id IS NOT NULL THEN gc.maxBuildingHeight
    ELSE NULL
END AS hz,

PT_OUT.id AS nomPT,

us2.connectID,

rc.automDegID,

CASE
    WHEN rc.id IS NOT NULL THEN rc.calcHLdep
    WHEN gc.id IS NOT NULL THEN gc.calcHLdep
    ELSE NULL
END AS calcHLdep,

CASE
    WHEN rc.id IS NOT NULL THEN rc.calcHLindep
    WHEN gc.id IS NOT NULL THEN gc.calcHLindep
    ELSE NULL
END AS calcHLindep,

CASE
    WHEN rc.id IS NOT NULL THEN rc.calcHLventil
    WHEN gc.id IS NOT NULL THEN gc.calcHLventil
    ELSE NULL
END AS calcHLventil,




--gc.calcHLdep,
--gc.calcHLindep,
--gc.calcHLventil,
gc.calcHLparall,
gc.calcHLGVSparall,
gc.avgHLcompParall,
gc.calcHLmix,
gc.calcHLGVSmix,
gc.avgHLcompMix,
gc.calcHLconseq,
gc.calcHLGVSconseq,
gc.avgHLcompconseq,
gc.calcHLpreON,
gc.calcHLGVSpreON,
gc.avgHLcompPreON,
gc.avgHLGVSopenSysFlow,
gc.avgHLGVSopenSysRet,
gc.avgHLcompOpen,



--rc.calcHLdep,
--rc.calcHLindep,
--rc.relLoadFacade,
rc.calcInternHD,
--rc.calcHLventil,
rc.expendHWpart,
rc.avgHLcond,
rc.avgHLcloseSys,
rc.avgHLopenSysFlow,
rc.avgHLopenSysRet,
rc.avgHLGVSopenFlow,
rc.avgHLGVSopenRet,
rc.avgHLGVScloseParall,
rc.avgHLGVScloseMix,
rc.avgHLGVScloseConseq,
rc.avgHLGVSclosePreON,



rc.WEmeteringDeviceID,


usP.id AS usPid,
usO.id AS usOid,

usP.calculationID AS calculationIDP,
usO.calculationID AS calculationIDO,
usP.pih AS pihP,
usO.pih AS pihO,
usP.t AS tP1,
usO.t AS tO1,

PT_OUT.a15,
PT_OUT.a16,
PT_OUT.a17,
IIF(PT_OUT.a3='Закрыто', 1, 0) AS isZakr,
DR_OUT.id AS nomDR,

rc.schemeNum,
CASE
    WHEN rc.id IS NOT NULL THEN rc.id
    WHEN gc.id IS NOT NULL THEN gc.id
    WHEN ist.id IS NOT NULL THEN ist.id
    WHEN hs.id IS NOT NULL THEN hs.id
    WHEN c3.id IS NOT NULL THEN c3.id
    WHEN us2.id IS NOT NULL THEN us2.id
    ELSE 0
END AS id2,


CASE
    WHEN rc.id IS NOT NULL THEN 'PR'
    WHEN gc.id IS NOT NULL THEN 'PO'
    WHEN ist.id IS NOT NULL THEN 'IS'
    WHEN hs.id IS NOT NULL THEN 'HS'
    WHEN c3.id IS NOT NULL THEN 'C3'
    WHEN us2.id IS NOT NULL THEN 'US2'
    ELSE 'US'
END AS type_txt


FROM nodes n
LEFT JOIN realConsumers rc ON rc.nodeID = n.id
LEFT JOIN generalizedConsumers gc ON gc.nodeID = n.id

LEFT JOIN heatSources        ist ON ist.nodeID = n.id
LEFT JOIN pumpStations       hs ON hs.nodeID = n.id
LEFT JOIN threeWayValves     c3 ON c3.nodeID = n.id
LEFT JOIN connectNodes       us2 ON us2.nodeID = n.id


OUTER APPLY (
    SELECT TOP 1 c.id AS cid
    FROM CALCULATION c
    WHERE c.fileID = n.fileID
    ORDER BY c.id DESC
) calc


LEFT JOIN US_OUT usP ON usP.calculationID = calc.cid AND usP.nodeID = n.id AND usP.externalSign = 1
LEFT JOIN US_OUT usO ON usO.calculationID = calc.cid AND usO.nodeID = n.id AND usO.externalSign = 2
LEFT JOIN PT_OUT ON PT_OUT.calculationID = calc.cid AND PT_OUT.nodeID = n.id
LEFT JOIN DR_OUT ON DR_OUT.calculationID = calc.cid AND DR_OUT.nodeID = n.id

WHERE n.removed = 0
