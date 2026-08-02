SELECT 
  kod_ist,
  sost,' ' AS obob,
  PR.kod, PR.uzel, name_building,
  IIF(PR.otoplz IS NULL, 0, PR.otoplz) AS otoplz,
  IIF(PR.otopln IS NULL, 0, PR.otopln) AS otopln,
  IIF(PR.otopl_tp IS NULL, 0, -PR.otopl_tp) AS tp,
  IIF(PR.ventil IS NULL, 0, PR.ventil) AS ventil,
  IIF(PR.kondiz IS NULL, 0, PR.kondiz) AS kondiz,

  IIF(PR.f_otoplz IS NULL, 0, PR.f_otoplz) AS f_otoplz,
  IIF(PR.f_otopln IS NULL, 0, PR.f_otopln) AS f_otopln,
  IIF(PR.f_otopl_tp IS NULL, 0, -PR.f_otopl_tp) AS f_tp,
  IIF(PR.f_ventil IS NULL, 0, PR.f_ventil) AS f_ventil,
  IIF(PR.f_kondiz IS NULL, 0, PR.f_kondiz) AS f_kondiz,

  
  gvop,
  gvoo,
  f_gvop,
  f_gvoo,
  (gvop+gvoo)*rez/100 AS rez_q,
  gvpr,
  gvsm,
  gvps,
  gvpw,
  f_gvpr,
  f_gvsm,
  f_gvps,
  f_gvpw,
  IIF(sost='*', 0, otoplz+otopln-otopl_tp+ventil+kondiz+(gvop+gvoo)*(1+rez/100)+gvpr+gvsm+gvps+gvpw) AS q,
  IIF(Tr_tv_co IS NULL, 0, Tr_tv_co) AS Tr_tv_co_zco,
  IIF(Tr_tv_co IS NULL, 0, Tr_tv_co) AS Tr_tv_co_nco,
  IIF(Tr_tv_co IS NULL, 0, Tr_tv_co) AS Tr_tv_co_pr, 
  IIF(Tr_tv_co IS NULL, 0, Tr_tv_co) AS Tr_tv_co_sm, 
  IIF(Tr_tv_co IS NULL, 0, Tr_tv_co) AS Tr_tv_co_ps, 
  IIF(Tr_tv_co IS NULL, 0, Tr_tv_co) AS Tr_tv_co_pw, 
  Tr_tv_cv,
  (otoplz+otopln+tp)*IIF(a24 IS NULL, 0, a24) AS v_otop,
  (ventil+kondiz)*IIF(a25 IS NULL, 0, a25) AS v_vent,
  pr_uchet,
  ORG.name,
  ORG.tip_owner AS tip_owner

FROM
  (
    SELECT nomer_owner, name, tip_owner FROM [Организация]
    UNION
     SELECT 
       0,
       "Не задано",
       "ЭН"
     FROM [Система теплоснабжения]
  ) ORG,
  VYD,
  [Потребитель реальный] PR,
  [Расчетная схема] PC
WHERE 
  PR.kod=PC.kod_rs
AND 
  (ORG.nomer_owner=kod_owner OR ((kod_owner IS NULL OR kod_owner = 0) AND ORG.nomer_owner=0))
AND
  PR.kod=VYD.kod
AND
  PR.uzel=VYD.uzel

UNION ALL

SELECT 
  kod_ist,
  sost,'О',
  PO.kod, PO.uzel, name_building,
  otopl_zco+IIF(cx_co_pr='Н', 0, otopl_pr)+IIF(cx_co_ps='Н', 0, otopl_ps)+IIF(cx_co_pw='Н', 0, otopl_pw)+IIF(cx_co_sm='Н', 0, otopl_sm),
  otopl_nco+IIF(cx_co_pr='Н', otopl_pr, 0)+IIF(cx_co_ps='Н', otopl_ps, 0)+IIF(cx_co_pw='Н', otopl_pw, 0)+IIF(cx_co_sm='Н', otopl_sm, 0),
  -(otopl_tp_zco+otopl_tp_nco+otopl_tp_pr+otopl_tp_ps+otopl_tp_pw+otopl_tp_sm),
  IIF(PO.ventil_cv IS NULL, 0, PO.ventil_cv) AS ventil_cv,
  IIF(PO.kondiz IS NULL, 0, PO.kondiz) AS kondiz,

  f_otopl_zco+IIF(cx_co_pr='Н', 0, f_otopl_pr)+IIF(cx_co_ps='Н', 0, f_otopl_ps)+IIF(cx_co_pw='Н', 0, f_otopl_pw)+IIF(cx_co_sm='Н', 0, f_otopl_sm),
  f_otopl_nco+IIF(cx_co_pr='Н', f_otopl_pr, 0)+IIF(cx_co_ps='Н', f_otopl_ps, 0)+IIF(cx_co_pw='Н', f_otopl_pw, 0)+IIF(cx_co_sm='Н', f_otopl_sm, 0),
  -(f_otopl_tp_zco+f_otopl_tp_nco+f_otopl_tp_pr+f_otopl_tp_ps+f_otopl_tp_pw+f_otopl_tp_sm),
  IIF(PO.f_ventil_cv IS NULL, 0, PO.f_ventil_cv) AS f_ventil_cv,
  IIF(PO.f_kondiz IS NULL, 0, PO.f_kondiz) AS f_kondiz,
  
  gv_op,
  gv_oo,
  f_gv_op,
  f_gv_oo,
  (gv_op+gv_oo)*rez_op/100,
  gv_pr,
  gv_sm,
  gv_ps,
  gv_pw,
  f_gv_pr,
  f_gv_sm,
  f_gv_ps,
  f_gv_pw,

  IIF(sost='*', 0, otopl_zco+otopl_nco+otopl_pr+otopl_ps+otopl_pw+otopl_sm
  -(otopl_tp_zco+otopl_tp_nco+otopl_tp_pr+otopl_tp_ps+otopl_tp_pw+otopl_tp_sm)
  +ventil_cv+IIF(kondiz IS NULL, 0, kondiz)+(gv_op+gv_oo)*(1+rez_op/100)+gv_pr+gv_sm+gv_ps+gv_pw),

  Tr_tv_co_zco,
  Tr_tv_co_nco,
  Tr_tv_co_pr,
  Tr_tv_co_sm,
  Tr_tv_co_ps,
  Tr_tv_co_pw,
  Tr_tv_cv,
  (otopl_zco+otopl_nco+otopl_pr+otopl_ps+otopl_pw+otopl_sm
  +otopl_tp_zco+otopl_tp_nco+otopl_tp_pr+otopl_tp_ps+otopl_tp_pw+otopl_tp_sm
  )*IIF(a24 IS NULL, 0, a24),
  (ventil_cv+kondiz)*IIF(a25 IS NULL, 0, a25),
  pr_uchet,
  ORG.name,
  ORG.tip_owner AS tip_owner

FROM
  (
    SELECT nomer_owner, name, tip_owner FROM [Организация]
    UNION
     SELECT 
       0,
       "Не задано",
       "ЭН"
     FROM [Система теплоснабжения]
  ) ORG,
  VYD,
  [Потребитель обобщенный] PO,
  [Расчетная схема] PC
WHERE 
  PO.kod=PC.kod_rs
AND 
  (ORG.nomer_owner=kod_owner OR ((kod_owner IS NULL OR kod_owner = 0) AND ORG.nomer_owner=0))
AND
  PO.kod=VYD.kod
AND
  PO.uzel=VYD.uzel
