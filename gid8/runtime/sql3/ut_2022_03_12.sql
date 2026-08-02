
SELECT
l.id,
n1.fileID,
l.nodeID1,
l.nodeID2,
l.externalSignLineID,
l.displaySign,
l.organizationID,

diaphragms.diameterInternal AS dru_home,

hps.magistral,
hps.distSite,
hps.magistralSite, 
hps.tubingTypeID, 
hps.diameterInternal, 
hps.diameterExternal, 
hps.diameterCondit, 
hps.pipeSectLength, 
hps.wallThickness, 
hps.crimpingQueSite, 
hps.pipeSectionID,

CASE
    WHEN hps.id IS NOT NULL THEN hps.pipeSectStateIDflow
    WHEN pumps.id IS NOT NULL THEN pumps.stateID
    WHEN pressRegulators.id IS NOT NULL THEN pressRegulators.regulatorStateID
    WHEN consumptRegulators.id IS NOT NULL THEN consumptRegulators.regulatorStateID
    WHEN dampers.id IS NOT NULL THEN dampers.damperArmatureStateID
    WHEN regulArmatures.id IS NOT NULL THEN regulArmatures.damperArmatureStateID
    WHEN bypass.id IS NOT NULL THEN bypass.regulatorStateID
    WHEN diaphragms.id IS NOT NULL THEN diaphragms.stateID
    WHEN elevators.id IS NOT NULL THEN elevators.stateID
    WHEN systemRadiators.id IS NOT NULL THEN systemRadiators.stateID
    WHEN heatExchangers.id IS NOT NULL THEN heatExchangers.stateID
    WHEN airHeaters.id IS NOT NULL THEN NULL
    WHEN pressDropRegulators.id IS NOT NULL THEN pressDropRegulators.regulatorStateID
    WHEN reverseValves.id IS NOT NULL THEN reverseValves.stateID
    ELSE NULL
END AS pipeSectStateIDflow,

CASE
    WHEN hps.id IS NOT NULL THEN hps.pipeSectStateIDret
    WHEN pumps.id IS NOT NULL THEN pumps.stateID
    WHEN pressRegulators.id IS NOT NULL THEN pressRegulators.regulatorStateID
    WHEN consumptRegulators.id IS NOT NULL THEN consumptRegulators.regulatorStateID
    WHEN dampers.id IS NOT NULL THEN dampers.damperArmatureStateID
    WHEN regulArmatures.id IS NOT NULL THEN regulArmatures.damperArmatureStateID
    WHEN bypass.id IS NOT NULL THEN bypass.regulatorStateID
    WHEN diaphragms.id IS NOT NULL THEN diaphragms.stateID
    WHEN elevators.id IS NOT NULL THEN elevators.stateID
    WHEN systemRadiators.id IS NOT NULL THEN systemRadiators.stateID
    WHEN heatExchangers.id IS NOT NULL THEN heatExchangers.stateID
    WHEN airHeaters.id IS NOT NULL THEN NULL
    WHEN pressDropRegulators.id IS NOT NULL THEN pressDropRegulators.regulatorStateID
    WHEN reverseValves.id IS NOT NULL THEN reverseValves.stateID
    ELSE NULL
END AS pipeSectStateIDret,

CASE
    WHEN hps.id IS NOT NULL THEN hps.id
    WHEN pumps.id IS NOT NULL THEN pumps.id
    WHEN pressRegulators.id IS NOT NULL THEN pressRegulators.id
    WHEN consumptRegulators.id IS NOT NULL THEN consumptRegulators.id
    WHEN dampers.id IS NOT NULL THEN dampers.id
    WHEN regulArmatures.id IS NOT NULL THEN regulArmatures.id
    WHEN bypass.id IS NOT NULL THEN bypass.id
    WHEN diaphragms.id IS NOT NULL THEN diaphragms.id
    WHEN elevators.id IS NOT NULL THEN elevators.id
    WHEN systemRadiators.id IS NOT NULL THEN systemRadiators.id
    WHEN heatExchangers.id IS NOT NULL THEN heatExchangers.id
    WHEN airHeaters.id IS NOT NULL THEN airHeaters.id
    WHEN pressDropRegulators.id IS NOT NULL THEN pressDropRegulators.id
    WHEN reverseValves.id IS NOT NULL THEN reverseValves.id
    ELSE NULL
END AS id2,

CASE
    WHEN hps.id IS NOT NULL THEN 'UT'
    WHEN pumps.id IS NOT NULL THEN 'HC'
    WHEN pressRegulators.id IS NOT NULL THEN 'RD'
    WHEN consumptRegulators.id IS NOT NULL THEN 'RR'
    WHEN dampers.id IS NOT NULL THEN 'ZD'
    WHEN regulArmatures.id IS NOT NULL THEN 'ZD2'
    WHEN bypass.id IS NOT NULL THEN 'BP'
    WHEN diaphragms.id IS NOT NULL THEN 'DR'
    WHEN elevators.id IS NOT NULL THEN 'EL'
    WHEN systemRadiators.id IS NOT NULL THEN 'RO'
    WHEN heatExchangers.id IS NOT NULL THEN 'TO'
    WHEN airHeaters.id IS NOT NULL THEN 'KU'
    WHEN pressDropRegulators.id IS NOT NULL THEN 'RP'
    WHEN reverseValves.id IS NOT NULL THEN 'OK'
    ELSE NULL
END AS type_txt,

COALESCE(utP.id, NS_OUT_P.id, RS_OUT_P.id, BP_OUT_P.id, ZD_OUT_P.id, ZD2_OUT_P.id, DRO_OUT_P.id, ANY_OUT_P.id, OK_OUT_P.id, -1) AS nomgP,
COALESCE(utO.id, NS_OUT_O.id, RS_OUT_O.id, BP_OUT_O.id, ZD_OUT_O.id, ZD2_OUT_O.id, DRO_OUT_O.id, ANY_OUT_O.id, OK_OUT_O.id, -1) AS nomgO,

    utP.a14 AS pod_poter,
    utP.a10 AS pod_w,
    utP.a11 AS pod_time1,
    utP.a14 AS pod_a14,
    utP.a15 AS pod_a15,
    utP.a16 AS pod_a16,
    utP.a17 AS pod_a17,
    utP.tzam AS pod_tzam,
    utP.tpot AS pod_tpot,

    utO.a14 AS obr_poter,
    utO.a10 AS obr_w,
    utO.a11 AS obr_time1,
    utO.a14 AS obr_a14,
    utO.a15 AS obr_a15,
    utO.a16 AS obr_a16,
    utO.a17 AS obr_a17,
    utO.tzam AS obr_tzam,
    utO.tpot AS obr_tpot,

    utP.b101 AS pod_b101,
    utP.b102 AS pod_b102,
    utP.b103 AS pod_b103,
    utP.b104 AS pod_b104,

    utO.b101 AS obr_b101,
    utO.b102 AS obr_b102,
    utO.b103 AS obr_b103,
    utO.b104 AS obr_b104,

COALESCE(utP.a13, NS_OUT_P.a14, RS_OUT_P.a11, BP_OUT_P.a13, ZD_OUT_P.a9, ZD2_OUT_P.a9, DRO_OUT_P.ras, ANY_OUT_P.ras, OK_OUT_P.ras) AS pod_q,
COALESCE(utO.a13, NS_OUT_O.a14, RS_OUT_O.a11, BP_OUT_O.a13, ZD_OUT_O.a9, ZD2_OUT_O.a9, DRO_OUT_O.ras, ANY_OUT_O.ras, OK_OUT_O.ras) AS obr_q,

l.coords

FROM linesobj l
JOIN nodes n1 ON n1.id = l.nodeID1
JOIN nodes n2 ON n2.id = l.nodeID2

LEFT JOIN heatPipeSections hps ON hps.lineID = l.id
LEFT JOIN pumps ON pumps.lineID = l.id
LEFT JOIN pressRegulators ON pressRegulators.lineID = l.id
LEFT JOIN consumptRegulators ON consumptRegulators.lineID = l.id
LEFT JOIN dampers ON dampers.lineID = l.id
LEFT JOIN regulArmatures ON regulArmatures.lineID = l.id
LEFT JOIN bypass ON bypass.lineID = l.id
LEFT JOIN diaphragms ON diaphragms.lineID = l.id
LEFT JOIN elevators ON elevators.lineID = l.id
LEFT JOIN systemRadiators ON systemRadiators.lineID = l.id
LEFT JOIN heatExchangers ON heatExchangers.lineID = l.id
LEFT JOIN airHeaters ON airHeaters.lineID = l.id
LEFT JOIN pressDropRegulators ON pressDropRegulators.lineID = l.id
LEFT JOIN reverseValves ON reverseValves.lineID = l.id

OUTER APPLY (
    SELECT TOP 1 c.id AS cid
    FROM CALCULATION c
    WHERE c.fileID = n1.fileID
    ORDER BY c.id DESC
) calc

LEFT JOIN UT_OUT utP ON utP.calculationID = calc.cid AND utP.lineID = l.id AND utP.externalSignLineID IN (2,4)
LEFT JOIN UT_OUT utO ON utO.calculationID = calc.cid AND utO.lineID = l.id AND utO.externalSignLineID IN (3,5)

LEFT JOIN NS_OUT NS_OUT_P ON NS_OUT_P.calculationID = calc.cid AND NS_OUT_P.lineID = l.id AND NS_OUT_P.externalSignLineID IN (2,4)
LEFT JOIN NS_OUT NS_OUT_O ON NS_OUT_O.calculationID = calc.cid AND NS_OUT_O.lineID = l.id AND NS_OUT_O.externalSignLineID IN (3,5)
LEFT JOIN RS_OUT RS_OUT_P ON RS_OUT_P.calculationID = calc.cid AND RS_OUT_P.lineID = l.id AND RS_OUT_P.externalSignLineID IN (2,4)
LEFT JOIN RS_OUT RS_OUT_O ON RS_OUT_O.calculationID = calc.cid AND RS_OUT_O.lineID = l.id AND RS_OUT_O.externalSignLineID IN (3,5)
LEFT JOIN BP_OUT BP_OUT_P ON BP_OUT_P.calculationID = calc.cid AND BP_OUT_P.lineID = l.id AND BP_OUT_P.externalSignLineID IN (2,4)
LEFT JOIN BP_OUT BP_OUT_O ON BP_OUT_O.calculationID = calc.cid AND BP_OUT_O.lineID = l.id AND BP_OUT_O.externalSignLineID IN (3,5)
LEFT JOIN ZD_OUT ZD_OUT_P ON ZD_OUT_P.calculationID = calc.cid AND ZD_OUT_P.lineID = l.id AND ZD_OUT_P.externalSignLineID IN (2,4)
LEFT JOIN ZD_OUT ZD_OUT_O ON ZD_OUT_O.calculationID = calc.cid AND ZD_OUT_O.lineID = l.id AND ZD_OUT_O.externalSignLineID IN (3,5)
LEFT JOIN ZD2_OUT ZD2_OUT_P ON ZD2_OUT_P.calculationID = calc.cid AND ZD2_OUT_P.lineID = l.id AND ZD2_OUT_P.externalSignLineID IN (2,4)
LEFT JOIN ZD2_OUT ZD2_OUT_O ON ZD2_OUT_O.calculationID = calc.cid AND ZD2_OUT_O.lineID = l.id AND ZD2_OUT_O.externalSignLineID IN (3,5)
LEFT JOIN DRO_OUT DRO_OUT_P ON DRO_OUT_P.calculationID = calc.cid AND DRO_OUT_P.lineID = l.id AND DRO_OUT_P.externalSignLineID IN (2,4)
LEFT JOIN DRO_OUT DRO_OUT_O ON DRO_OUT_O.calculationID = calc.cid AND DRO_OUT_O.lineID = l.id AND DRO_OUT_O.externalSignLineID IN (3,5)
LEFT JOIN ANY_OUT ANY_OUT_P ON ANY_OUT_P.calculationID = calc.cid AND ANY_OUT_P.lineID = l.id AND ANY_OUT_P.externalSignLineID IN (2,4)
LEFT JOIN ANY_OUT ANY_OUT_O ON ANY_OUT_O.calculationID = calc.cid AND ANY_OUT_O.lineID = l.id AND ANY_OUT_O.externalSignLineID IN (3,5)
LEFT JOIN OK_OUT OK_OUT_P ON OK_OUT_P.calculationID = calc.cid AND OK_OUT_P.lineID = l.id AND OK_OUT_P.externalSignLineID IN (2,4)
LEFT JOIN OK_OUT OK_OUT_O ON OK_OUT_O.calculationID = calc.cid AND OK_OUT_O.lineID = l.id AND OK_OUT_O.externalSignLineID IN (3,5)

WHERE l.removed = 0 
  AND n1.fileID = n2.fileID
--  AND n1.fileID IN (2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,48,49,50,51,52,54,55,58,801,806,2149,3150,3151,3152,3153,3154,3155,3156,3157,3158,3165,3177,3178,3181)

--OPTION (RECOMPILE);