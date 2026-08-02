SELECT 
    heatSources.name,
    V1,
    V1*(0.5+hls.t_percent/200) AS V1Leto,
    V2,
    V2*(0.5+hls.t_percent/200) AS V2Leto,
    Vpodv,
    Vpodv*(0.5+hls.t_percent/200) AS VpodvLeto,

    ifnull(VobM,0)+ifnull(VobR,0) AS Vob,
    (ifnull(VobM,0)+ifnull(VobR,0))*(0.5+hls.t_percent/200) AS VobLeto,

    Vot,
    0 AS VotLeto,
    Vvent,
    0 AS VventLeto,
    Vgvs,
    Vgvs AS VgvsLeto,

--    V6,
--    V1+V2+V6 AS Vsts,
--    Vot1, Vvent1,  Vgvs1,
--    Vot2, Vvent2,  Vgvs2,
--    Vot3, Vvent3,  Vgvs3,

--    (V1+V2+Vpodv+Vot+Vvent+Vgvs)*hlm.a/100 AS Podp,
--    (V1+V2+Vpodv+Vgvs)*(0.5+hls.t_percent/200)*hlm.a/100 AS PodpLeto,

    (V1+V2+Vpodv+ifnull(VobM,0)+ifnull(VobR,0)+Vot+Vvent+Vgvs) AS Vall,
    (V1+V2+Vpodv+ifnull(VobM,0)+ifnull(VobR,0)+Vgvs)*(0.5+hls.t_percent/200) AS VAllLeto,


    (SELECT Vall)*hlm.a/100 AS Podp,
    (SELECT VAllLeto)*hlm.a/100 AS PodpLeto

--    Vot+Vvent+Vgvs,
--    TR.heatSourceID

FROM 
(
  SELECT 
    _T1.heatSourceID,

    sum(IF(objectID = 1 AND typnet <> 20, VV, 0)) AS V1,
    sum(IF(objectID = 2 AND typnet <> 20, VV, 0)) AS V2,
    sum(IF(typnet = 20, VV, 0)) AS Vpodv,
--    sum(IF(typnet = 30, VV, 0)) AS V30,
   sum(IF(NOT objectID IN (1,2) AND typnet<>20 , VV, 0)) AS V6


  FROM 
  (
    SELECT 
      hps.heatSourceID,
      diameterExternal,
      diameterCondit,
      diameterInternal,
      objectID,
      wallThickness,
      typnet,

      (lenP+lenO)*diameterInternal*diameterInternal*PI()/4/1000/1000 AS VV

    FROM heatPipeSectionIst hps

  ) _T1

  GROUP BY
    _T1.heatSourceID
) TR

LEFT JOIN 

(
  SELECT 
    ec.heatSourceID,

    sum(calcHLdep+calcHLindep+calcHLparall+calcHLmix+calcHLconseq+calcHLpreON) AS gOt,
    sum(calcHLventil+calcHLcond) gVent,

    -- calcHLcloseSys+calcHLopenSysFlow+calcHLopenSysRet

    sum(calcHLGVSconseq+calcHLGVSmix+calcHLGVSparall+calcHLGVSpreON+avgHLGVSopenSysFlow+avgHLGVSopenSysRet) gGvs

  FROM generalizedConsumers gc
  JOIN nodes n ON n.id = gc.nodeID
  left join externalCodes ec on ec.id = n.externalCodeID
  WHERE NOT n.removed

  GROUP BY
    ec.heatSourceID
) PO

ON PO.heatSourceID = TR.heatSourceID

LEFT JOIN 

(
SELECT 
    _TTT1.heatSourceID,

    sum(gOt_pr*volWaterHS) AS Vot,
    sum(gVent_pr*volWaterVS) AS Vvent,
    sum(gGvs_pr*volWaterOpenGvs) AS Vgvs,

    sum(gOt_pr1*volWaterHS) AS Vot1,
    sum(gVent_pr1*volWaterVS) AS Vvent1,
    sum(gGvs_pr1*volWaterOpenGvs) AS Vgvs1,

    sum(gOt_pr2*volWaterHS) AS Vot2,
    sum(gVent_pr2*volWaterVS) AS Vvent2,
    sum(gGvs_pr2*volWaterOpenGvs) AS Vgvs2,

    sum(gOt_pr3*volWaterHS) AS Vot3,
    sum(gVent_pr3*volWaterVS) AS Vvent3,
    sum(gGvs_pr3*volWaterOpenGvs) AS Vgvs3


FROM (
  SELECT 
    ec.heatSourceID,

    sum(if(buildingTypeID=1,calcHLdep+calcHLindep, 0)) AS gOt_pr1,
    sum(if(buildingTypeID=1,calcHLventil, 0)) AS gVent_pr1,
    sum(if(buildingTypeID=1,avgHLGVSopenFlow+avgHLGVSopenRet+avgHLGVScloseParall+avgHLGVScloseMix+avgHLGVScloseConseq+avgHLGVSclosePreON, 0)) AS gGvs_pr1,

    sum(if(buildingTypeID=2,calcHLdep+calcHLindep, 0)) AS gOt_pr2,
    sum(if(buildingTypeID=2,calcHLventil, 0)) AS gVent_pr2,
    sum(if(buildingTypeID=2,avgHLGVSopenFlow+avgHLGVSopenRet+avgHLGVScloseParall+avgHLGVScloseMix+avgHLGVScloseConseq+avgHLGVSclosePreON, 0)) AS gGvs_pr2,

    sum(if(buildingTypeID=3,calcHLdep+calcHLindep, 0)) AS gOt_pr3,
    sum(if(buildingTypeID=3,calcHLventil, 0)) AS gVent_pr3,
    sum(if(buildingTypeID=3,avgHLGVSopenFlow+avgHLGVSopenRet+avgHLGVScloseParall+avgHLGVScloseMix+avgHLGVScloseConseq+avgHLGVSclosePreON, 0)) AS gGvs_pr3,

    sum(calcHLdep+calcHLindep) AS gOt_pr,
    sum(calcHLventil) AS gVent_pr,

    sum(avgHLGVSopenFlow+avgHLGVSopenRet+avgHLGVScloseParall+avgHLGVScloseMix+avgHLGVScloseConseq+avgHLGVSclosePreON) AS gGvs_pr,

    IF(hlm.useTableData=0, rc.volWaterHS, hlm.volWaterHS) AS volWaterHS,
    IF(hlm.useTableData=0, rc.volWaterVS, hlm.volWaterVS) AS volWaterVS,
    hlm.volWaterOpenGvs

  FROM realConsumers rc
  JOIN nodes n ON n.id = rc.nodeID
  left join externalCodes ec on ec.id = n.externalCodeID
  join heatLosesMain hlm ON hlm.id=1
  WHERE NOT n.removed

  GROUP BY
    rc.volWaterHS,
    rc.volWaterVS,
    ec.heatSourceID
) _TTT1
GROUP BY
    _TTT1.heatSourceID,
    volWaterHS,
    volWaterVS,
    volWaterOpenGvs
) PR

ON PR.heatSourceID = TR.heatSourceID

LEFT JOIN heatSources ON heatSources.id=TR.heatSourceID

LEFT JOIN 
(
SELECT _TT1.heatSourceID, SUM(IF(belongMS=1, V, 0)) AS VObM, SUM(IF(belongMS<>1, V, 0)) AS VObR 
  FROM 
  (
    SELECT heatPipeSectionsHarness.heatSourceID, belongMS, SUM(POW(diameterExternal/1000,2)*PI()/4*pipeSectLength) AS V
      FROM heatPipeSectionsHarness
      GROUP BY
      heatPipeSectionsHarness.heatSourceID,belongMS
  ) _TT1
  GROUP BY _TT1.heatSourceID
) OBV
ON OBV.heatSourceID=TR.heatSourceID
left join heatLosesSource hls ON hls.heatSourceID=TR.heatSourceID
left join heatLosesMain hlm ON hlm.id=1


WHERE TR.heatSourceID IN (SELECT id FROM heatLosesSourceList)

ORDER BY
    IF (TR.heatSourceID IS NULL, 1, 0),
    TR.heatSourceID

