select
n.id,
IIF(n.cStateName='открыто', ' ', 'закр') as sost, 

obob AS po_pr,

n.externalCode, n.externalNodeName, n.name_building,

otoplz, 
otopln, 
0 as otopl_tp, 
ventil, 
kondiz, 
n.avgHLGVSopenFlow as gvop1, 
n.avgHLGVSopenRet as gvoo1, 
rez_q, 
n.avgHLGVScloseParall as gvpr, 
n.avgHLGVScloseMix as gvsm, 
n.avgHLGVScloseConseq as gvps, 
n.avgHLGVSclosePreon as gvpw, 

 qz,
 n.avgHLGVSopenFlow as gvop, 
 n.avgHLGVSopenRet as gvoo, 
 (n.otoplz+n.otopln)*n.volWaterHS AS v_otop,
 (n.ventil+n.kondiz)*n.volWaterVS AS v_vent,

 orgName
--, n.id

FROM 


--consumerview 

(
select n.id AS id,
 n.fileID, cStates.name AS cStateName,
 ' ' AS obob,
 ec.id AS kod_ist,
 ec.name AS externalCode,
 n.externalNodeName AS externalNodeName,
 addr.name_building AS name_building,
 rc2.otoplz AS otoplz,
 rc2.otopln AS otopln,
 rc2.tp AS tp,
 rc2.ventil AS ventil,
 rc2.kondiz AS kondiz,
 rc2.f_otoplz AS f_otoplz,
 rc2.f_otopln AS f_otopln,
 rc2.f_tp AS f_tp,
 rc2.f_ventil AS f_ventil,
 rc2.f_kondiz AS f_kondiz,
 rc.avgHLGVSopenFlow AS avgHLGVSopenFlow,
 rc.avgHLGVSopenRet AS avgHLGVSopenRet,
 rc.contAvgHLGVSopenFlow AS contAvgHLGVSopenFlow,
 rc.contAvgHLGVSopenRet AS contAvgHLGVSopenRet,
 (((rc.contAvgHLGVSopenFlow + rc.contAvgHLGVSopenRet) * rc.circHLosOpen) / 100) AS rez_q,
 rc.avgHLGVScloseParall AS avgHLGVScloseParall,
 rc.avgHLGVScloseMix AS avgHLGVScloseMix,
 rc.avgHLGVScloseConseq AS avgHLGVScloseConseq,
 rc.avgHLGVSclosePreON AS avgHLGVSclosePreON,
 rc.contAvgHLGVScloseParall AS contAvgHLGVScloseParall,
 rc.contAvgHLGVScloseMix AS contAvgHLGVScloseMix,
 rc.contAvgHLGVScloseConseq AS contAvgHLGVScloseConseq,
 rc.contAvgHLGVSclosePreON AS contAvgHLGVSclosePreON,
 rc.volWaterHS, rc.volWaterVS, 

 IIF((cStates.name = 'закрыто'), 0, 
 (
  ((rc.calcHLdep + rc.calcHLindep - rc.calcInternHD + rc.calcHLventil + rc.avgHLcond) + 
      ((rc.avgHLGVSopenFlow + rc.avgHLGVSopenRet) * (1 + (rc.circHLosOpen / 100)))) + 
        rc.avgHLGVScloseParall + rc.avgHLGVScloseMix + rc.avgHLGVScloseConseq + rc.avgHLGVSclosePreON)) AS q,


 IIF((cStates.name = 'закрыто'), 0, 
 (
  ((rc.calcHLdep + rc.calcHLindep - rc.calcInternHD + rc.calcHLventil + rc.avgHLcond) + 
      ((rc.avgHLGVSopenFlow + rc.avgHLGVSopenRet) * (0 + (rc.circHLosOpen / 100)))) + 
        rc.avgHLGVScloseParall + rc.avgHLGVScloseMix + rc.avgHLGVScloseConseq + rc.avgHLGVSclosePreON)) AS qz,


 isnull(rc.calcTempHR,0) AS Tr_tv_co_zco,
 isnull(rc.calcTempHR,0) AS Tr_tv_co_nco,
 isnull(rc.calcTempHR,0) AS Tr_tv_co_pr,
 isnull(rc.calcTempHR,0) AS Tr_tv_co_sm,
 isnull(rc.calcTempHR,0) AS Tr_tv_co_ps,
 isnull(rc.calcTempHR,0) AS Tr_tv_co_pw,
 rc.calcTempVS AS Tr_tv_cv,
(((rc.calcHLdep + rc.calcHLindep) + rc2.tp) * isnull(rc.volWaterHS,0)) AS v_otop,
 ((rc.calcHLventil + rc.avgHLcond) * isnull(rc.volWaterVS,0)) AS v_vent,
 weDevices.name AS weDeviceName,
 CAST(org.name AS nvarchar) AS orgName,
 org.sign AS tip_owner
FROM (((((((realconsumers rc 
LEFT JOIN nodes n on((n.id = rc.nodeID))) 
LEFT JOIN consumerstates cStates on((cStates.id = rc.consumerStateID))) 
LEFT JOIN wemeteringdevices weDevices on((weDevices.id = rc.WEmeteringDeviceID))) 
LEFT JOIN organizations org on((org.id = n.organizationID))) 
LEFT JOIN (select rcc.id AS id,
 isnull(rcc.calcHLdep,0) AS otoplz,
 isnull(rcc.calcHLindep,0) AS otopln,
 -isnull(rcc.calcInternHD,0) AS tp,
 isnull(rcc.calcHLventil,0) AS ventil,
 isnull(rcc.avgHLcond,0) AS kondiz,
 isnull(rcc.contCalcHLdep,0) AS f_otoplz,
 isnull(rcc.contcalcHLindep,0) AS f_otopln,
 -isnull(rcc.contInternHD,0) AS f_tp,
 isnull(rcc.contCalcHLventil,0) AS f_ventil,
 isnull(rcc.contAvgHLcond,0) AS f_kondiz
FROM realconsumers rcc) rc2 on((rc2.id = rc.id))) 
LEFT JOIN externalcodes ec on((ec.id = n.externalCodeID))) 
LEFT JOIN addresses addr on((addr.id = n.addressID)))
WHERE n.removed=0 

union

select n.id AS id,
 n.fileID, cStates.name AS name,
 'О' AS О, ec.id AS kod_ist,
 ec.name AS name,
 n.externalNodeName AS externalNodeName,
 addr.name_building AS name_building,
 ISNULL(gc.calcHLdep,0) + IIF(gc.schemeParallID = 1, 0, ISNULL(gc.calcHLparall,0)) + IIF(gc.schemeConseqID = 1,0, ISNULL(gc.calcHLconseq,0)) + IIF(gc.schemePreONID = 1, 0, ISNULL(gc.calcHLpreON,0)) + IIF(gc.schemeMixID = 1, 0, ISNULL(gc.calcHLmix,0)) AS otoplz,
 ((((ISNULL(gc.calcHLindep,0) + IIF((gc.schemeParallID = 1),ISNULL(gc.calcHLparall,0), 0)) + IIF((gc.schemeConseqID = 1), ISNULL(gc.calcHLconseq, 0), 0)) + IIF((gc.schemePreONID = 1), ISNULL(gc.calcHLpreON, 0), 0)) + IIF((gc.schemeMixID = 1), ISNULL(gc.calcHLmix, 0), 0)) AS otopln,
 -((((((ISNULL(gc.calcInternHDdep, 0) + ISNULL(gc.calcInternHDindep, 0)) + gc.internHDparall) + gc.internHDconseq) + gc.internHDpreON) + gc.internHDmix)) AS tp,
 ISNULL(gc.calcHLventil,0) AS ventil_cv,
 ISNULL(gc.calcHLcond,0) AS kondiz,
 ((((gc.adjCalcHLdep + IIF((gc.schemeParallID = 1), 0, gc.adjCalcHLparall)) + IIF((gc.schemeConseqID = 1), 0, gc.adjCalcHLconseq)) + IIF((gc.schemePreONID = 1), 0, gc.adjCalcHLpreON)) + IIF((gc.schemeMixID = 1), 0, gc.adjCalcHLmix)) AS f_otoplz,
 ((((gc.adjCalcHLindep + IIF((gc.schemeParallID = 1), gc.adjCalcHLparall, 0)) + IIF((gc.schemeConseqID = 1), gc.adjCalcHLconseq, 0)) + IIF((gc.schemePreONID = 1), gc.adjCalcHLpreON, 0)) + IIF((gc.schemeMixID = 1), gc.adjCalcHLmix, 0)) AS f_otopln,
 -((((((gc.adjCalcHDdep + gc.adjCalcInternHDindep) + gc.adjInternHDparall) + gc.adjInternHDconseq) + gc.adjInternHDpreON) + gc.adjInternHDmix)) AS f_tp,
 isnull(gc.adjCalcHLventil,0) AS f_ventil_cv,
 isnull(gc.adjCalcHLcond,0) AS f_kondiz,
 gc.avgHLGVSopenSysFlow AS avgHLGVSopenSysFlow,
 gc.avgHLGVSopenSysRet AS avgHLGVSopenSysRet,
 gc.adjAvgHLGVSopenSysFlow AS adjAvgHLGVSopenSysFlow,
 gc.adjAvgHLGVSopenSysRet AS adjAvgHLGVSopenSysRet,
-- (((gc.adjAvgHLGVSopenSysFlow + gc.adjAvgHLGVSopenSysRet) * gc.avgHLcompOpen) / 100) AS rez_q,
 (((gc.avgHLGVSopenSysFlow+gc.avgHLGVSopenSysRet) * gc.avgHLcompOpen) / 100) AS rez_q,



 gc.calcHLGVSparall AS calcHLGVSparall,
 gc.calcHLGVSmix AS calcHLGVSmix,
 gc.calcHLGVSconseq AS calcHLGVSconseq,
 gc.calcHLGVSpreON AS calcHLGVSpreON,
 gc.adjCalcHLGVSparall AS adjCalcHLGVSparall,
 gc.adjCalcHLGVSmix AS adjCalcHLGVSmix,
 gc.adjCalcHLGVSconseq AS adjCalcHLGVSconseq,
 gc.adjCalcHLGVSpreON AS adjCalcHLGVSpreON,
 gc.volWaterHS, gc.volWaterVS, 
 IIF((cStates.name = 'закрыто'),0, (((((((((gc.calcHLdep + gc.calcHLindep + gc.calcHLparall + gc.calcHLconseq + gc.calcHLpreON + gc.calcHLmix) - (gc.calcInternHDdep + gc.calcInternHDdep + gc.internHDparall + gc.internHDconseq + gc.internHDpreON + gc.internHDmix)) + gc.calcHLventil) + isnull(gc.calcHLcond,0)) + ((gc.avgHLGVSopenSysFlow + gc.avgHLGVSopenSysRet) * (1 + (gc.avgHLcompOpen / 100)))) + gc.calcHLGVSparall) + gc.calcHLGVSmix) + gc.calcHLGVSconseq) + gc.calcHLGVSpreON)) AS q,
 IIF((cStates.name = 'закрыто'),0, (((((((((gc.calcHLdep + gc.calcHLindep + gc.calcHLparall + gc.calcHLconseq + gc.calcHLpreON + gc.calcHLmix) - (gc.calcInternHDdep + gc.calcInternHDdep + gc.internHDparall + gc.internHDconseq + gc.internHDpreON + gc.internHDmix)) + gc.calcHLventil) + isnull(gc.calcHLcond,0)) + ((gc.avgHLGVSopenSysFlow + gc.avgHLGVSopenSysRet) * ((gc.avgHLcompOpen / 100)))) + gc.calcHLGVSparall) + gc.calcHLGVSmix) + gc.calcHLGVSconseq) + gc.calcHLGVSpreON)) AS qz,

 
 gc.calcTempDep AS calcTempDep,
 gc.calcTempIndep AS calcTempIndep,
 gc.calcTempHRparall AS calcTempHRparall,
 gc.calcTempHRmix AS calcTempHRmix,
 gc.calcTempHRconseq AS calcTempHRconseq,
 gc.calcTempHRpreON AS calcTempHRpreON,
 gc.calcTempHA AS calcTempHA,
 ((((((((((((gc.calcHLdep + gc.calcHLindep) + gc.calcHLparall) + gc.calcHLconseq) + gc.calcHLpreON) + gc.calcHLmix) + gc.calcInternHDdep) + gc.calcInternHDindep) + gc.internHDparall) + gc.internHDconseq) + gc.internHDpreON) + gc.internHDmix) * isnull(gc.volWaterHS,0)) AS v_otopl,
 ((gc.calcHLventil + gc.calcHLcond) * isnull(gc.volWaterVS,0)) AS v_ventil,
 weDevices.name AS weDeviceName,
 CAST(org.name AS nvarchar) AS orgName,
 org.sign AS tip_owner 
FROM generalizedconsumers gc 
LEFT JOIN consumerstates cStates on cStates.id = gc.consumerStateID
LEFT JOIN nodes n on n.id = gc.nodeID
LEFT JOIN addresses addr on addr.id = n.addressID
LEFT JOIN wemeteringdevices weDevices on weDevices.id = gc.WEmeteringDeviceID
LEFT JOIN organizations org on org.id = n.organizationID
LEFT JOIN externalcodes ec on ec.id = n.externalCodeID



)



n

WHERE n.fileID=$fileID$ AND 1=1

