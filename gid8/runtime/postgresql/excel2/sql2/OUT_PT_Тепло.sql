SELECT 
  n.id,
  IIF(n.cStateName='открыто', ' ', 'закр') AS cStateName, 
  obob AS po_pr,
  n.externalCode, n.externalNodeName, n.name_building,
  round(t1,1) AS t1,
  round(t2,1) AS t2,
  round(Qotz,3) AS Qotz, 
  round(Qotn,3) AS Qotn, 
  isnull(dop12, 0)+isnull(dop13, 0) AS dop12_13, 
  round(dop17,3) AS dop17, 
  round(dop18,3) AS dop18, 
  round(dop19,3) AS dop19, 
  round(dop20,3) AS dop20, 
  round(Qsum_z,3) AS Qsum_z, 
  round(dop18,3) AS dop18_2, 
  round(dop19,3) AS dop19_2, 
  round(Qotz_treb,3) AS Qotz_treb, 
  round(Qotn_treb,3) AS Qotn_treb, 
  round(Qvent_treb,3) AS Qvent_treb, 
  Qgvz_treb, 
  avgHLGVSopenFlow, avgHLGVSopenRet, 
  rez_q,

     isnull(Qotz_treb, 0)
    +isnull(Qotn_treb, 0)
    +isnull(Qvent_treb, 0)
    +isnull(Qgvz_treb, 0) as Qsum, 
    
    avgHLGVSopenFlow AS avgHLGVSopenFlow2, 
    avgHLGVSopenRet AS avgHLGVSopenRet2,

  hs.name
--  ,
--  n.id

FROM consumerview n

LEFT JOIN PT_OUT  ON PT_OUT.nodeID=n.id

left join externalCodes ec on ec.id = kod_ist
LEFT JOIN heatSources hs ON hs.id = ec.heatSourceID


WHERE n.fileID=$fileID$ AND calculationID=$calculationID$

