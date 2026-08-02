SELECT TOP 2000000
  n.id,
  IIF(n.cStateName='открыто', ' ', 'закр') AS cStateName, 
  obob AS po_pr,
  n.externalCode, n.externalNodeName, n.name_building,

  a4,
  a5, 
  a6+a7 AS a6_7, 
  a11, 
  a12, 
  a13, 
  a14, 

  a15,
  a16,
  a17,
  round(a21,1) AS a21,
  round(a22,1) AS a22,
  round(a23,1) AS a23,
  round(t1,1) AS t1,
  round(t2,1) AS t2,

  round(Qotz,3) AS Qotz, round(Qotn,3) AS Qotn, 
  isnull(dop12, 0)+isnull(dop13, 0) AS dop12_13, 
  round(dop17,3) AS dop17, 
  round(dop18,3) AS dop18, 
  round(dop19,3) AS dop19, 
  round(dop20,3) AS dop20, 
  round(Qsum_z,3) AS Qsum_z, 
  
  round(dop18,3) AS dop18_2,
  round(dop19,3) AS dop19_2, 

  isnull(Qotz_treb, 0)+  
  isnull(Qotn_treb, 0)+  
  isnull(Qvent_treb, 0)+ 
  isnull(Qgvz_treb, 0) AS Qsum,
  
  iif ((dop18 IS NULL OR dop18 = 0) AND (dop19 IS NULL OR dop19 = 0), 0, Qgvop_treb*isnull(dop18, 0)/(isnull(dop18, 0)+isnull(dop19, 0))) AS dop18_1, 
  iif ((dop18 IS NULL OR dop18 = 0) AND (dop19 IS NULL OR dop19 = 0), 0, Qgvoo_treb*isnull(dop19, 0)/(isnull(dop18, 0)+isnull(dop19, 0))) AS dop19_1, 

  round(Gneob,3) AS Gneob,

  hs.name
--  ,
--  n.id

  
FROM consumerview n

LEFT JOIN PT_OUT  ON PT_OUT.nodeID=n.id
left join externalCodes ec on ec.id = kod_ist
LEFT JOIN heatSources hs ON hs.id = ec.heatSourceID

WHERE n.fileID=$fileID$ AND calculationID=$calculationID$

    
  
