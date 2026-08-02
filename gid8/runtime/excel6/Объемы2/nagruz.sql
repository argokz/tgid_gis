SELECT 
    hs.name,

    sum(calcHLdep+calcHLindep) AS gOt_pr,
    sum(calcHLventil) AS gVent_pr,
    sum(avgHLGVSopenFlow+avgHLGVSopenRet+avgHLGVScloseParall+avgHLGVScloseMix+avgHLGVScloseConseq+avgHLGVSclosePreON) AS gGvs_pr


FROM realConsumers rc
JOIN nodes n ON n.id = rc.nodeID
left join externalCodes ec on ec.id = n.externalCodeID
join heatLosesMain hlm ON hlm.id=1

LEFT JOIN heatSources hs ON hs.id=ec.heatSourceID

WHERE n.removed=0 AND ec.heatSourceID IN (SELECT id FROM heatLosesSourceList)

GROUP BY
    ec.heatSourceID,
    hs.name
