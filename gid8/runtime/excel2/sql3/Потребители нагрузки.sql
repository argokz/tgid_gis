SELECT TOP 2000000 * FROM
(


SELECT
  sost, 
  '' AS typ_pt,
  PC.kod_ist,
  PT.kod AS kod, 
  PT.uzel AS uzel, 
  name_building, 
  otoplz+otopln+otn_fs AS otopl,
  ventil+kondiz AS vent, 
  gvpr+gvsm+gvps+gvpw AS gvz, 
  otopl_tp,
  gvop, gvoo, rez,
  otopl+vent+gvz-otopl_tp AS qz,

  f_otoplz+f_otopln AS f_otopl,
  f_ventil+f_kondiz AS f_vent, 
  f_gvpr+f_gvsm+f_gvps+f_gvpw AS f_gvz, 
  f_otopl_tp,
  f_gvop, f_gvoo, 
  f_otopl+f_vent+f_gvz-f_otopl_tp AS f_qz

FROM 
  [Потребитель реальный] PT,
  [Расчетная схема] PC,
  VYD
WHERE 
  PT.kod=PC.kod_rs
AND
  PT.kod=VYD.kod
AND
  PT.uzel=VYD.uzel

ORDER BY kod_ist

UNION

SELECT 
  sost, 
  'О',
  PC.kod_ist,
  PT.kod AS kod, 
  PT.uzel AS uzel, 
  name_building, 
  otopl_zco+otopl_nco+otopl_pr+otopl_ps+otopl_pw+otopl_sm AS otopl,
  ventil_cv+kondiz AS vent,
  gv_pr+gv_sm+gv_ps+gv_pw AS gvz, 
  otopl_tp_zco+otopl_tp_nco AS otopl_tp,
  gv_op AS gvop, 
  gv_oo AS gvoo,
  rez_op AS rez,
  otopl+vent+gvz-otopl_tp AS qz,

  otopl_zco+otopl_nco AS f_otopl,
  ventil_cv+kondiz AS f_vent,
  gv_pr+gv_sm+gv_ps+gv_pw AS f_gvz, 
  otopl_tp_zco+otopl_tp_nco AS f_otopl_tp,
  gv_op AS f_gvop, 
  gv_oo AS f_gvoo,
  otopl+vent+gvz-otopl_tp AS f_qz

FROM 
  [Потребитель обобщенный] PT,
  [Расчетная схема] PC,
  VYD
WHERE 
  PT.kod=PC.kod_rs
AND
  PT.kod=VYD.kod
AND
  PT.uzel=VYD.uzel

-- ORDER BY kod_ist

) _QQQ

ORDER BY
    kod,uzel

