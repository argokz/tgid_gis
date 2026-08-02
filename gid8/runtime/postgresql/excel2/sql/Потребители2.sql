SELECT 
  kod_ist,
  sost,
  obob,
  kodur,
  PT0.kod, 
  PT0.uzel, 
  name_building,
  iif(PT0.otoplz IS NULL, 0, PT0.otoplz) AS otoplz, 
  iif(PT0.otopln IS NULL, 0, PT0.otopln) AS otopln, 
  iif(PT0.tp IS NULL, 0, PT0.tp) AS tp, 
  iif(PT0.ventil IS NULL, 0, PT0.ventil) AS ventil, 
  iif(PT0.kondiz IS NULL, 0, PT0.kondiz) AS kondiz, 
  iif(PT0.gvop IS NULL, 0, PT0.gvop) AS gvop, 
  iif(PT0.gvoo IS NULL, 0, PT0.gvoo) AS gvoo, 
  iif(PT0.rez_q IS NULL, 0, PT0.rez_q) AS rez_q, 
  iif(PT0.gvpr IS NULL, 0, PT0.gvpr) AS gvpr, 
  iif(PT0.gvsm IS NULL, 0, PT0.gvsm) AS gvsm, 
  iif(PT0.gvps IS NULL, 0, PT0.gvps) AS gvps, 
  iif(PT0.gvpw IS NULL, 0, PT0.gvpw) AS gvpw, 
  iif(PT0.q IS NULL, 0, PT0.q) AS q, 
  iif(PT0.Tr_tv_co_zco IS NULL, 0, PT0.Tr_tv_co_zco) AS Tr_tv_co_zco, 
  iif(PT0.Tr_tv_co_nco IS NULL, 0, PT0.Tr_tv_co_nco) AS Tr_tv_co_nco, 
  iif(PT0.Tr_tv_co_pr IS NULL, 0, PT0.Tr_tv_co_pr) AS Tr_tv_co_pr,  
  iif(PT0.Tr_tv_co_sm IS NULL, 0, PT0.Tr_tv_co_sm) AS Tr_tv_co_sm,  
  iif(PT0.Tr_tv_co_ps IS NULL, 0, PT0.Tr_tv_co_ps) AS Tr_tv_co_ps,  
  iif(PT0.Tr_tv_co_pw IS NULL, 0, PT0.Tr_tv_co_pw) AS Tr_tv_co_pw, 
  iif(PT_OUT.a4 IS NULL, 0, PT_OUT.a4) AS a4, 
  iif(PT_OUT.a5 IS NULL, 0, PT_OUT.a5) AS a5,  
  iif(PT_OUT.a6 IS NULL, 0, PT_OUT.a6) AS a6,  
  iif(PT_OUT.a7 IS NULL, 0, PT_OUT.a7) AS a7,  
  iif(PT_OUT.a8 IS NULL, 0, PT_OUT.a8) AS a8,  
  iif(PT_OUT.a9 IS NULL, 0, PT_OUT.a9) AS a9,  
  iif(PT_OUT.a10 IS NULL, 0, PT_OUT.a10) AS a10, 
  iif(PT_OUT.a11 IS NULL, 0, PT_OUT.a11) AS a11, 
  iif(PT_OUT.a12 IS NULL, 0, PT_OUT.a12) AS a12, 
  iif(PT_OUT.a13 IS NULL, 0, PT_OUT.a13) AS a13, 
  iif(PT_OUT.a14 IS NULL, 0, PT_OUT.a14) AS a14, 
  iif(PT_OUT.a15 IS NULL, 0, PT_OUT.a15) AS a15, 
  iif(PT_OUT.a16 IS NULL, 0, PT_OUT.a16) AS a16, 
  iif(PT_OUT.a17 IS NULL, 0, PT_OUT.a17) AS a17,
  iif(PT_OUT.a23 IS NULL, 0, PT_OUT.a23) AS a23,

  iif(PT_OUT.Qotz_treb IS NULL, 0, PT_OUT.Qotz_treb) AS Qotz_treb,
  iif(PT_OUT.Qotn_treb IS NULL, 0, PT_OUT.Qotn_treb) AS Qotn_treb,
  iif(PT_OUT.Qvent_treb IS NULL, 0, PT_OUT.Qvent_treb) AS Qvent_treb,
  iif(PT_OUT.Qgvz_treb IS NULL, 0, PT_OUT.Qgvz_treb) AS Qgvz_treb,
  iif(PT_OUT.Qgvop_treb IS NULL, 0, PT_OUT.Qgvop_treb) AS Qgvop_treb,
  iif(PT_OUT.Qgvoo_treb IS NULL, 0, PT_OUT.Qgvoo_treb) AS Qgvoo_treb,
  iif(PT_OUT.Qrez_treb IS NULL, 0, PT_OUT.Qrez_treb) AS Qrez_treb,
  iif(PT_OUT.Qtreb IS NULL, 0, PT_OUT.Qtreb) AS Qtreb,

  iif(PT_OUT.Qotn IS NULL, 0, PT_OUT.Qotn) AS Qotn,
  iif(PT_OUT.Qotz IS NULL, 0, PT_OUT.Qotz) AS Qotz,
  iif(PT_OUT.Qsum_z IS NULL, 0, PT_OUT.Qsum_z) AS Qsum_z,
  iif(PT_OUT.dop12 IS NULL, 0, PT_OUT.dop12) AS dop12,
  iif(PT_OUT.dop13 IS NULL, 0, PT_OUT.dop13) AS dop13,
  iif(PT_OUT.dop17 IS NULL, 0, PT_OUT.dop17) AS dop17,
  iif(PT_OUT.dop18 IS NULL, 0, PT_OUT.dop18) AS dop18,
  iif(PT_OUT.dop19 IS NULL, 0, PT_OUT.dop19) AS dop19,
  iif(PT_OUT.dop20 IS NULL, 0, PT_OUT.dop20) AS dop20
  
FROM 
PT_OUT,
(

SELECT 
  kod_ist,
  sost,' ' AS obob,
  kodur,
  kod, uzel, name_building,
  otoplz,
  otopln,
  -otopl_tp AS tp,
  ventil,
  kondiz,
  gvop,
  gvoo,
  (gvop+gvoo)*rez/100 AS rez_q,
  gvpr,
  gvsm,
  gvps,
  gvpw,
  IIF(sost='*', 0, otoplz+otopln-otopl_tp+ventil+kondiz+(gvop+gvoo)*(1+rez/100)+gvpr+gvsm+gvps+gvpw) AS q,
  Tr_tv_co AS Tr_tv_co_zco,
  Tr_tv_co AS Tr_tv_co_nco,
  Tr_tv_co AS Tr_tv_co_pr, 
  Tr_tv_co AS Tr_tv_co_sm, 
  Tr_tv_co AS Tr_tv_co_ps, 
  Tr_tv_co AS Tr_tv_co_pw, 
  Tr_tv_cv

FROM
  [Потребитель реальный] PR,
  [Расчетная схема] PC
WHERE 
  PR.kod=PC.kod_rs

UNION ALL

SELECT 
  kod_ist,
  sost,'О',
  kodur,
  kod, uzel, name_building,
  otopl_zco+IIF(cx_co_pr='Н', 0, otopl_pr)+IIF(cx_co_ps='Н', 0, otopl_ps)+IIF(cx_co_pw='Н', 0, otopl_pw)+IIF(cx_co_sm='Н', 0, otopl_sm),
  otopl_nco+IIF(cx_co_pr='Н', otopl_pr, 0)+IIF(cx_co_ps='Н', otopl_ps, 0)+IIF(cx_co_pw='Н', otopl_pw, 0)+IIF(cx_co_sm='Н', otopl_sm, 0),
  -(otopl_tp_zco+otopl_tp_nco+otopl_tp_pr+otopl_tp_ps+otopl_tp_pw+otopl_tp_sm),
  ventil_cv,
  kondiz,
  gv_op,
  gv_oo,
  (gv_op+gv_oo)*rez_op/100,
  gv_pr,
  gv_sm,
  gv_ps,
  gv_pw,

  IIF(sost='*', 0, otopl_zco+otopl_nco+otopl_pr+otopl_ps+otopl_pw+otopl_sm
  -(otopl_tp_zco+otopl_tp_nco+otopl_tp_pr+otopl_tp_ps+otopl_tp_pw+otopl_tp_sm)
  +ventil_cv+IIF(kondiz IS NULL, 0, kondiz)+(gv_op+gv_oo)*(1+rez_op/100)+gv_pr+gv_sm+gv_ps+gv_pw),

  Tr_tv_co_zco,
  Tr_tv_co_nco,
  Tr_tv_co_pr,
  Tr_tv_co_sm,
  Tr_tv_co_ps,
  Tr_tv_co_pw,
  Tr_tv_cv

FROM
  [Потребитель обобщенный] PO,
  [Расчетная схема] PC
WHERE 
  PO.kod=PC.kod_rs
) PT0

WHERE 
  PT_OUT.kod=PT0.kod AND PT_OUT.uzel=PT0.uzel