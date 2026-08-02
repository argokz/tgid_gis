SELECT 
--  heatSourceID,
--  typnet, 
  heatSources.sourceName AS ist,

  dbo.getTypnet(typnet) as typnet1,

  diameterExternal,
--  diameterCondit,
--  diameterInternal,
--  wallThickness,

  lenP_N,
  lenO_N,
  
--  (lenP_N+lenO_N)/2 AS lenN,

  lenP_K+lenP_B lenPodzP,
  lenO_K+lenO_B lenPodzO,
  
  lenP_N+lenO_N+lenP_K+lenO_K+lenP_B+lenO_B lenAll,

  0 AS len_tr,

  MN_P,
  MN_O,
--  (lenP_K+lenO_K+lenP_B+lenO_B)*diameterExternal*0.001 AS MP,
  MP_P,
  MP_O,
  
  M,
  VV

FROM
(
  SELECT 
    heatSourceID,
    typnet,
    diameterExternal,
--    diameterCondit,
--    diameterInternal,
--    wallThickness,
    sum(lenP_K) AS lenP_K,
    sum(lenO_K) AS lenO_K,
    sum(lenP_B) AS lenP_B,
    sum(lenO_B) AS lenO_B,
    sum(lenP_N) AS lenP_N,
    sum(lenO_N) AS lenO_N,
    sum(MN_P) AS MN_P,
    sum(MN_O) AS MN_O,
    sum(MP_P) AS MP_P,
    sum(MP_O) AS MP_O,
    sum(M) AS M,
    sum(VV) AS VV
FROM 
(
  
  SELECT 
    heatSourceID,
    typnet,
    diameterExternal,
--    diameterCondit,
--    diameterInternal,
--    wallThickness,
    sum(lenP_K) AS lenP_K,
    sum(lenO_K) AS lenO_K,
    sum(lenP_B) AS lenP_B,
    sum(lenO_B) AS lenO_B,
    sum(lenP_N) AS lenP_N,
    sum(lenO_N) AS lenO_N,
    sum(lenP_N)*diameterExternal*0.001 AS MN_P,
    sum(lenO_N)*diameterExternal*0.001 AS MN_O,
    sum((lenP_K+lenP_B)*diameterExternal*0.001) AS MP_P,
    sum((lenO_K+lenO_B)*diameterExternal*0.001) AS MP_O,
    sum((lenP_N+lenP_N+lenP_K+lenO_K+lenP_B+lenO_B)*diameterExternal)*0.001 AS M,
    
    sum(VV) AS VV

  FROM 

  (
    SELECT 
      heatSourceID,
      diameterExternal,
      diameterCondit,
--      diameterInternal,
--      wallThickness,
      typnet,
      IIF(tubingTypeID = 1, lenP, 0) lenP_K,
      IIF(tubingTypeID = 1, lenO, 0) lenO_K,

      IIF(tubingTypeID = 2, lenP, 0) lenP_B,
      IIF(tubingTypeID = 2, lenO, 0) lenO_B,
      
      
      IIF(tubingTypeID = 4 OR tubingTypeID = 5 OR tubingTypeID = 3, lenP, 0) lenP_N,
      IIF(tubingTypeID = 4 OR tubingTypeID = 5 OR tubingTypeID = 3, lenO, 0) lenO_N,

      (lenP+lenO)*diameterInternal*diameterInternal*PI()/4/1000/1000 AS VV,

      IIF(lenP > 0 AND lenO > 0, 2, 1) mult 

    FROM heatPipeSectionIst hps

--    AND hps.heatSourceID IN ($ist_list$)

  ) _T1
  GROUP BY
    heatSourceID,
    typnet,
    diameterExternal,
    diameterCondit
--    diameterInternal
--    wallThickness
) _T3
  GROUP BY
    heatSourceID,
    typnet,
--    diameterCondit
    diameterExternal


) _T2
LEFT JOIN heatSources ON heatSources.id=heatSourceID

WHERE heatSourceID IN (SELECT id FROM heatLosesSourceList)

ORDER BY
    heatSourceID,
    typnet,
--    diameterCondit
    diameterExternal
--    diameterInternal
  