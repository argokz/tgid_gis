SELECT
  heatSources.sourceName,

  dbo.getMon(m) AS m1,

  case sezon
    when 1 then 'î'
    when 2 then 'ë'
    else ''
  end as sezon1,
  ROUND(tn, 1) AS tn,
  ROUND(tpod, 1) AS tpod,
  ROUND(tgr, 1) AS tgr,
  ROUND(tgP, 1) AS tgP,
  ROUND(tgO, 1) AS tgO,
  ROUND(tx, 1) AS tx,
  workCount

FROM (


  SELECT
    heatSourceID,
    IIF(sezon=1, 14, 15) AS r,
    IIF(sezon=1, 13, 14) AS m,
    sezon, 
    sum(tn*workCount)/sum(workCount) AS tn,
    sum(tpod*workCount)/sum(workCount) AS tpod,
    sum(tgr*workCount)/sum(workCount) AS tgr,
    sum(tgP*workCount)/sum(workCount) AS tgP,
    sum(tgO*workCount)/sum(workCount) AS tgO,
    sum(tx*workCount)/sum(workCount) AS tx,
    sum(workCount) AS workCount
  FROM
  (
    SELECT 
      heatSourceID,
      r, 
      m, 
      sezon, 
      tn, 
      tpod, 
      tgr,
      tgP, 
      tgO, 
      tx, 
      workCount 
    FROM heatLosesSourceMonths
    WHERE heatSourceID IN (SELECT id FROM heatLosesSourceList)
  ) _T1

  GROUP BY
    heatSourceID,
    sezon

  UNION

    SELECT 
      hls.heatSourceID,
      hls.r, 
      hls.m, 
      hls.sezon, 
      hls.tn, 
      hls.tpod, 
      hls.tgr,
      hls.tgP, 
      hls.tgO, 
      hls.tx, 
      hls.workCount 
    FROM heatLosesSourceMonths hls
    WHERE hls.heatSourceID IN (SELECT id FROM heatLosesSourceList)

  UNION

  SELECT
    heatSourceID,
    16 AS r,
    15 AS m,
    3, 
    sum(tn*workCount)/sum(workCount) AS tn,
    sum(tpod*workCount)/sum(workCount) AS tpod,
    sum(tgr*workCount)/sum(workCount) AS tgr,
    sum(tgP*workCount)/sum(workCount) AS tgP,
    sum(tgO*workCount)/sum(workCount) AS tgO,
    sum(tx*workCount)/sum(workCount) AS tx,
    sum(workCount) AS workCount
  FROM
  (
    SELECT 
      heatSourceID,
      r, 
      m, 
      sezon, 
      tn, 
      tpod, 
      tgr,
      tgP, 
      tgO, 
      tx, 
      workCount 
    FROM heatLosesSourceMonths hls
    WHERE heatSourceID IN (SELECT id FROM heatLosesSourceList)
  ) _T2

  GROUP BY
    heatSourceID
) _T3

LEFT JOIN heatSources ON heatSources.id=_T3.heatSourceID


ORDER BY 
  heatSourceID, r