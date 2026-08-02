SELECT top 2147483647 
  n.id,
  ec.name as 'kod', n.externalNodeName as 'uzel',
  a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, 
  a19, a20, a21, a22, a23, Gsm, Gm, Gx, Ggws, 
  
  CALCULATION.Tn AS 'Температура наружного воздуха, °С', 
  
  t1, t2, t01, t02, t03, t2co, t2gws, tv, tg, 
  dop8, dop9, Qotz, Qotn, dop12, dop13, dop14, dop15, dop16, dop17, dop18, dop19, dop20, Qsum_z, 
  dop22, dop23, Gneob, Qras_gv_z, Qotz_treb, Qotn_treb, Qvent_treb, Qgvz_treb, Qgvop_treb, Qgvoo_treb, 
  Qrez_treb, Qtreb, Qfact, Q_obesp_min, Q_obesp_otopl_min, Q_obesp_gvs_min, tzam
from PT_OUT
join nodes n on n.id = pt_out.nodeID
join externalCodes ec on ec.id = n.externalCodeID
join externalSigns es on es.id = n.externalSignID
LEFT JOIN CALCULATION ON CALCULATION.id=calculationID

WHERE n.fileID=$fileID$ AND n.internalNodeID IS NULL
AND calculationID=$calculationID$
AND n.removed=0 

