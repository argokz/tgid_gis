SELECT
l.id,

n1.fileID,

l.nodeID1,
l.nodeID2,
l.externalSignLineID,
l.displaySign,
l.organizationID,

--hps.pipeSectStateIDflow,
--hps.pipeSectStateIDret,


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
    WHEN NOT hps                 .id IS NULL THEN hps.pipeSectStateIDflow
    WHEN NOT pumps               .id IS NULL THEN pumps.stateID
    WHEN NOT pressRegulators     .id IS NULL THEN pressRegulators.regulatorStateID
    WHEN NOT consumptRegulators  .id IS NULL THEN consumptRegulators.regulatorStateID
    WHEN NOT dampers             .id IS NULL THEN dampers.damperArmatureStateID
    WHEN NOT regulArmatures      .id IS NULL THEN regulArmatures.damperArmatureStateID
    WHEN NOT bypass              .id IS NULL THEN bypass.regulatorStateID
    WHEN NOT diaphragms          .id IS NULL THEN diaphragms.stateID
    WHEN NOT elevators           .id IS NULL THEN elevators.stateID
    WHEN NOT systemRadiators     .id IS NULL THEN systemRadiators.stateID
    WHEN NOT heatExchangers      .id IS NULL THEN heatExchangers.stateID
    WHEN NOT airHeaters          .id IS NULL THEN NULL -- airHeaters.s
    WHEN NOT pressDropRegulators .id IS NULL THEN pressDropRegulators.regulatorStateID
    WHEN NOT reverseValves       .id IS NULL THEN reverseValves.stateID
    ELSE NULL
END AS pipeSectStateIDflow,

CASE
    WHEN NOT hps                 .id IS NULL THEN hps.pipeSectStateIDret
    WHEN NOT pumps               .id IS NULL THEN pumps.stateID
    WHEN NOT pressRegulators     .id IS NULL THEN pressRegulators.regulatorStateID
    WHEN NOT consumptRegulators  .id IS NULL THEN consumptRegulators.regulatorStateID
    WHEN NOT dampers             .id IS NULL THEN dampers.damperArmatureStateID
    WHEN NOT regulArmatures      .id IS NULL THEN regulArmatures.damperArmatureStateID
    WHEN NOT bypass              .id IS NULL THEN bypass.regulatorStateID
    WHEN NOT diaphragms          .id IS NULL THEN diaphragms.stateID
    WHEN NOT elevators           .id IS NULL THEN elevators.stateID
    WHEN NOT systemRadiators     .id IS NULL THEN systemRadiators.stateID
    WHEN NOT heatExchangers      .id IS NULL THEN heatExchangers.stateID
    WHEN NOT airHeaters          .id IS NULL THEN NULL -- airHeaters.s
    WHEN NOT pressDropRegulators .id IS NULL THEN pressDropRegulators.regulatorStateID
    WHEN NOT reverseValves       .id IS NULL THEN reverseValves.stateID
    ELSE NULL
END AS pipeSectStateIDret,


CASE
    WHEN NOT hps                 .id IS NULL THEN hps.id
    WHEN NOT pumps               .id IS NULL THEN pumps.id
    WHEN NOT pressRegulators     .id IS NULL THEN pressRegulators.id
    WHEN NOT consumptRegulators  .id IS NULL THEN consumptRegulators.id
    WHEN NOT dampers             .id IS NULL THEN dampers.id
    WHEN NOT regulArmatures      .id IS NULL THEN regulArmatures.id
    WHEN NOT bypass              .id IS NULL THEN bypass.id
    WHEN NOT diaphragms          .id IS NULL THEN diaphragms.id
    WHEN NOT elevators           .id IS NULL THEN elevators.id
    WHEN NOT systemRadiators     .id IS NULL THEN systemRadiators.id
    WHEN NOT heatExchangers      .id IS NULL THEN heatExchangers.id
    WHEN NOT airHeaters          .id IS NULL THEN airHeaters.id
    WHEN NOT pressDropRegulators .id IS NULL THEN pressDropRegulators.id
    WHEN NOT reverseValves       .id IS NULL THEN reverseValves.id
    ELSE NULL
END 
AS id2,

CASE
    WHEN NOT hps                 .id IS NULL THEN 'UT'
    WHEN NOT pumps               .id IS NULL THEN 'HC'
    WHEN NOT pressRegulators     .id IS NULL THEN 'RD'
    WHEN NOT consumptRegulators  .id IS NULL THEN 'RR'
    WHEN NOT dampers             .id IS NULL THEN 'ZD'
    WHEN NOT regulArmatures      .id IS NULL THEN 'ZD2'
    WHEN NOT bypass              .id IS NULL THEN 'BP'
    WHEN NOT diaphragms          .id IS NULL THEN 'DR'
    WHEN NOT elevators           .id IS NULL THEN 'EL'
    WHEN NOT systemRadiators     .id IS NULL THEN 'RO'
    WHEN NOT heatExchangers      .id IS NULL THEN 'TO'
    WHEN NOT airHeaters          .id IS NULL THEN 'KU'
    WHEN NOT pressDropRegulators .id IS NULL THEN 'RP'
    WHEN NOT reverseValves       .id IS NULL THEN 'OK'
    ELSE NULL
END AS type_txt,

CASE
    WHEN NOT utP   .id IS NULL THEN utP.id
    WHEN NOT NS_OUT_P   .id IS NULL THEN NS_OUT_P.id
    WHEN NOT RS_OUT_P   .id IS NULL THEN RS_OUT_P.id
    WHEN NOT BP_OUT_P   .id IS NULL THEN BP_OUT_P.id
    WHEN NOT ZD_OUT_P   .id IS NULL THEN ZD_OUT_P.id
    WHEN NOT ZD2_OUT_P  .id IS NULL THEN ZD2_OUT_P.id
    WHEN NOT DRO_OUT_P  .id IS NULL THEN DRO_OUT_P.id
    WHEN NOT ANY_OUT_P  .id IS NULL THEN ANY_OUT_P.id
    WHEN NOT OK_OUT_P   .id IS NULL THEN OK_OUT_P.id
    ELSE -1
END AS nomgP,

    

CASE
    WHEN NOT utO   .id IS NULL THEN utO.id
    WHEN NOT NS_OUT_O   .id IS NULL THEN NS_OUT_O.id
    WHEN NOT RS_OUT_O   .id IS NULL THEN RS_OUT_O.id
    WHEN NOT BP_OUT_O   .id IS NULL THEN BP_OUT_O.id
    WHEN NOT ZD_OUT_O   .id IS NULL THEN ZD_OUT_O.id
    WHEN NOT ZD2_OUT_O  .id IS NULL THEN ZD2_OUT_O.id
    WHEN NOT DRO_OUT_O  .id IS NULL THEN DRO_OUT_O.id
    WHEN NOT ANY_OUT_O  .id IS NULL THEN ANY_OUT_O.id
    WHEN NOT OK_OUT_O   .id IS NULL THEN OK_OUT_O.id
    ELSE -1
END AS nomgO,
    

--    utP.a13 AS pod_q,        --  Расход сетевой воды на участке
    utP.a14 AS pod_poter,
    utP.a10 AS pod_w,        --  Скорость потока сетевой воды
    utP.a11 AS pod_time1,    --  Время прохождения потока воды
    utP.a14 AS pod_a14,      --  Удельные линейные потери напора на участке
    utP.a15 AS pod_a15,      --  Линейные потери напора на участке         
    utP.a16 AS pod_a16,      --  Местные потери напора на участке          
    utP.a17 AS pod_a17,      --  Общие потери напора на участке            
    utP.tzam AS pod_tzam,      --  Время остывания воды при остановке движения
    utP.tpot AS pod_tpot,    --  Тепловые потери  теплопроводом

--    utO.a13 AS obr_q,        --  Расход сетевой воды на участке
    utO.a14 AS obr_poter,
    utO.a10 AS obr_w,        --  Скорость потока сетевой воды
    utO.a11 AS obr_time1,    --  Время прохождения потока воды
    utO.a14 AS obr_a14,      --  Удельные линейные потери напора на участке
    utO.a15 AS obr_a15,      --  Линейные потери напора на участке         
    utO.a16 AS obr_a16,      --  Местные потери напора на участке          
    utO.a17 AS obr_a17,      --  Общие потери напора на участке            
    utO.tzam AS obr_tzam,      --  Время остывания воды при остановке движения
    utO.tpot AS obr_tpot,    --  Тепловые потери  теплопроводом
-- BEGIN if UT_OUT b101

    utP.b101  AS pod_b101,    --  Расчетная тепловая нагрузка, Гкал/ч               
    utP.b102  AS pod_b102,    --  Расчетная тепловая нагрузка на отопление, Гкал/ч  
    utP.b103  AS pod_b103,    --  Расчетная тепловая нагрузка на вентиляцию, Гкал/ч 
    utP.b104  AS pod_b104,    --  Расчетная тепловая нагрузка на ГВС, Гкал/ч        

    utO.b101  AS obr_b101,    --  Расчетная тепловая нагрузка, Гкал/ч               
    utO.b102  AS obr_b102,    --  Расчетная тепловая нагрузка на отопление, Гкал/ч  
    utO.b103  AS obr_b103,    --  Расчетная тепловая нагрузка на вентиляцию, Гкал/ч 
    utO.b104  AS obr_b104,    --  Расчетная тепловая нагрузка на ГВС, Гкал/ч        
-- END if UT_OUT b101



CASE
    WHEN NOT utP   .id IS NULL THEN utP.a13
    WHEN NOT NS_OUT_P   .id IS NULL THEN NS_OUT_P.a14
    WHEN NOT RS_OUT_P   .id IS NULL THEN RS_OUT_P.a11
    WHEN NOT BP_OUT_P   .id IS NULL THEN BP_OUT_P.a13
    WHEN NOT ZD_OUT_P   .id IS NULL THEN ZD_OUT_P.a9
    WHEN NOT ZD2_OUT_P  .id IS NULL THEN ZD2_OUT_P.a9
    WHEN NOT DRO_OUT_P  .id IS NULL THEN DRO_OUT_P.ras
    WHEN NOT ANY_OUT_P  .id IS NULL THEN ANY_OUT_P.ras
    WHEN NOT OK_OUT_P   .id IS NULL THEN OK_OUT_P.ras
    ELSE NULL
END AS pod_q,

CASE
    WHEN NOT utO   .id IS NULL THEN utO.a13
    WHEN NOT NS_OUT_O   .id IS NULL THEN NS_OUT_O.a14
    WHEN NOT RS_OUT_O   .id IS NULL THEN RS_OUT_O.a11
    WHEN NOT BP_OUT_O   .id IS NULL THEN BP_OUT_O.a13
    WHEN NOT ZD_OUT_O   .id IS NULL THEN ZD_OUT_O.a9
    WHEN NOT ZD2_OUT_O  .id IS NULL THEN ZD2_OUT_O.a9
    WHEN NOT DRO_OUT_O  .id IS NULL THEN DRO_OUT_O.ras
    WHEN NOT ANY_OUT_O  .id IS NULL THEN ANY_OUT_O.ras
    WHEN NOT OK_OUT_O   .id IS NULL THEN OK_OUT_O.ras
    ELSE NULL
END AS obr_q,




l.coords



FROM linesobj l
JOIN nodes n1 ON n1.id=l.nodeID1
JOIN nodes n2 ON n2.id=l.nodeID2

LEFT JOIN heatPipeSections  hps ON hps.lineID=l.id
LEFT JOIN pumps               ON pumps              .lineID=l.id
LEFT JOIN pressRegulators     ON pressRegulators    .lineID=l.id
LEFT JOIN consumptRegulators  ON consumptRegulators .lineID=l.id
LEFT JOIN dampers             ON dampers            .lineID=l.id
LEFT JOIN regulArmatures      ON regulArmatures     .lineID=l.id
LEFT JOIN bypass              ON bypass             .lineID=l.id
LEFT JOIN diaphragms          ON diaphragms         .lineID=l.id
LEFT JOIN elevators           ON elevators          .lineID=l.id
LEFT JOIN systemRadiators     ON systemRadiators    .lineID=l.id
LEFT JOIN heatExchangers      ON heatExchangers     .lineID=l.id
LEFT JOIN airHeaters          ON airHeaters         .lineID=l.id
LEFT JOIN pressDropRegulators ON pressDropRegulators.lineID=l.id
LEFT JOIN reverseValves       ON reverseValves      .lineID=l.id

LEFT JOIN 
(
SELECT 
c.fileID,
max(c.id) AS cid
FROM CALCULATION c
LEFT JOIN fragments fr ON fr.id=c.fileID
GROUP BY c.fileID
) calc ON calc.fileID=n1.fileID



LEFT JOIN UT_OUT utP ON utP.lineID=l.id AND utP.externalSignLineID IN (2,4) AND utP.calculationID=calc.cid
LEFT JOIN UT_OUT utO ON utO.lineID=l.id AND utO.externalSignLineID IN (3,5) AND utO.calculationID=calc.cid

LEFT JOIN NS_OUT   NS_OUT_P     ON NS_OUT_P .lineID=l.id AND NS_OUT_P .externalSignLineID IN (2,4) AND NS_OUT_P .calculationID=calc.cid
LEFT JOIN NS_OUT   NS_OUT_O     ON NS_OUT_O .lineID=l.id AND NS_OUT_O .externalSignLineID IN (3,5) AND NS_OUT_O .calculationID=calc.cid
LEFT JOIN RS_OUT   RS_OUT_P     ON RS_OUT_P .lineID=l.id AND RS_OUT_P .externalSignLineID IN (2,4) AND RS_OUT_P .calculationID=calc.cid
LEFT JOIN RS_OUT   RS_OUT_O     ON RS_OUT_O .lineID=l.id AND RS_OUT_O .externalSignLineID IN (3,5) AND RS_OUT_O .calculationID=calc.cid
LEFT JOIN BP_OUT   BP_OUT_P     ON BP_OUT_P .lineID=l.id AND BP_OUT_P .externalSignLineID IN (2,4) AND BP_OUT_P .calculationID=calc.cid
LEFT JOIN BP_OUT   BP_OUT_O     ON BP_OUT_O .lineID=l.id AND BP_OUT_O .externalSignLineID IN (3,5) AND BP_OUT_O .calculationID=calc.cid
LEFT JOIN ZD_OUT   ZD_OUT_P     ON ZD_OUT_P .lineID=l.id AND ZD_OUT_P .externalSignLineID IN (2,4) AND ZD_OUT_P .calculationID=calc.cid
LEFT JOIN ZD_OUT   ZD_OUT_O     ON ZD_OUT_O .lineID=l.id AND ZD_OUT_O .externalSignLineID IN (3,5) AND ZD_OUT_O .calculationID=calc.cid
LEFT JOIN ZD2_OUT  ZD2_OUT_P    ON ZD2_OUT_P.lineID=l.id AND ZD2_OUT_P.externalSignLineID IN (2,4) AND ZD2_OUT_P.calculationID=calc.cid
LEFT JOIN ZD2_OUT  ZD2_OUT_O    ON ZD2_OUT_O.lineID=l.id AND ZD2_OUT_O.externalSignLineID IN (3,5) AND ZD2_OUT_O.calculationID=calc.cid
LEFT JOIN DRO_OUT  DRO_OUT_P    ON DRO_OUT_P.lineID=l.id AND DRO_OUT_P.externalSignLineID IN (2,4) AND DRO_OUT_P.calculationID=calc.cid
LEFT JOIN DRO_OUT  DRO_OUT_O    ON DRO_OUT_O.lineID=l.id AND DRO_OUT_O.externalSignLineID IN (3,5) AND DRO_OUT_O.calculationID=calc.cid
LEFT JOIN ANY_OUT  ANY_OUT_P    ON ANY_OUT_P.lineID=l.id AND ANY_OUT_P.externalSignLineID IN (2,4) AND ANY_OUT_P.calculationID=calc.cid
LEFT JOIN ANY_OUT  ANY_OUT_O    ON ANY_OUT_O.lineID=l.id AND ANY_OUT_O.externalSignLineID IN (3,5) AND ANY_OUT_O.calculationID=calc.cid
LEFT JOIN OK_OUT   OK_OUT_P     ON OK_OUT_P .lineID=l.id AND OK_OUT_P .externalSignLineID IN (2,4) AND OK_OUT_P .calculationID=calc.cid
LEFT JOIN OK_OUT   OK_OUT_O     ON OK_OUT_O .lineID=l.id AND OK_OUT_O .externalSignLineID IN (3,5) AND OK_OUT_O .calculationID=calc.cid


WHERE l.removed=0 AND n1.fileID=n2.fileID
