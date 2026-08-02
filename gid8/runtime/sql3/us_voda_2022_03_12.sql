SELECT 
n.id, n.fileID, n.internalNodeID, n.externalCodeID, n.externalSignID, n.displaySign, n.x, n.y, nodeTypeID, externalNodeName, nodeName,geoMarkTopTube,geoMarkNodeArea,
n.calcPressFlow,
n.calcPressRet,

CASE
    WHEN NOT rc.id IS NULL THEN rc.consumerStateID
    WHEN NOT gc.id IS NULL THEN gc.consumerStateID
    ELSE 1
END AS consumerStateID,

CASE
    WHEN NOT rc.id IS NULL THEN rc.buildHeight
    WHEN NOT gc.id IS NULL THEN gc.maxBuildingHeight
    ELSE NULL
END AS hz,

PT_OUT.id AS nomPT,

us2.connectID,

rc.automDegID,

CASE
    WHEN NOT rc.id IS NULL THEN rc.calcHLdep
    WHEN NOT gc.id IS NULL THEN gc.calcHLdep
    ELSE NULL
END AS calcHLdep,

CASE
    WHEN NOT rc.id IS NULL THEN rc.calcHLindep
    WHEN NOT gc.id IS NULL THEN gc.calcHLindep
    ELSE NULL
END AS calcHLindep,

CASE
    WHEN NOT rc.id IS NULL THEN rc.calcHLventil
    WHEN NOT gc.id IS NULL THEN gc.calcHLventil
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
IIF(PT_OUT.a3='отключен', 1, 0) AS isZakr,
DR_OUT.id AS nomDR,

rc.schemeNum,
CASE
    WHEN NOT rc.id IS NULL THEN rc.id
    WHEN NOT gc.id IS NULL THEN gc.id

    WHEN NOT ist.id IS NULL THEN ist.id
    WHEN NOT hs.id IS NULL THEN hs.id
    WHEN NOT c3.id IS NULL THEN c3.id
    WHEN NOT us2.id IS NULL THEN us2.id
    ELSE 0
END AS id2,


CASE
    WHEN NOT rc.id IS NULL THEN 'PR'
    WHEN NOT gc.id IS NULL THEN 'PO'

    WHEN NOT ist.id IS NULL THEN 'IS'
    WHEN NOT hs.id IS NULL THEN 'HS'
    WHEN NOT c3.id IS NULL THEN 'C3'
    WHEN NOT us2.id IS NULL THEN 'US2'
    WHEN NOT voda_POTREBIT.id IS NULL THEN 'V_PR'
    WHEN NOT voda_ISTOC.id IS NULL THEN 'V_IS'

    ELSE 'US'
END AS type_txt


FROM nodes n
LEFT JOIN realConsumers rc ON rc.nodeID=n.id
LEFT JOIN generalizedConsumers gc ON gc.nodeID=n.id

LEFT JOIN heatSources        ist ON ist.nodeID=n.id
LEFT JOIN pumpStations       hs ON hs.nodeID=n.id
LEFT JOIN threeWayValves     c3 ON c3.nodeID=n.id
LEFT JOIN connectNodes       us2 ON us2.nodeID=n.id
LEFT JOIN voda_POTREBIT          ON voda_POTREBIT.nodeID=n.id
LEFT JOIN voda_ISTOC             ON voda_ISTOC.nodeID=n.id



LEFT JOIN 
(
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
WHERE user_gid='$user_gid$'
GROUP BY c.fileID
) calc ON calc.fileID=n.fileID

LEFT JOIN US_OUT usP ON usP.nodeID=n.id AND usP.externalSign=1 AND usP.calculationID=calc.cid
LEFT JOIN US_OUT usO ON usO.nodeID=n.id AND usO.externalSign=2 AND usO.calculationID=calc.cid
LEFT JOIN PT_OUT ON PT_OUT.nodeID=n.id AND PT_OUT.calculationID=calc.cid
LEFT JOIN DR_OUT ON DR_OUT.nodeID=n.id AND DR_OUT.calculationID=calc.cid

WHERE n.removed=0
