SELECT 
  kod_ist,
  sost,' ' AS obob,
  kod, uzel, name_building,
  0 AS otoplz,
  0 AS otopln,
  0 AS tp,
  0 AS ventil,
  0 AS kondiz,
  gvop,
  gvoo,
  (gvop+gvoo)*rez/100 AS rez_q,
  gvpr,
  gvsm,
  gvps,
  gvpw,
  IIF(sost='*', 0, (gvop+gvoo)*(1+rez/100)+gvpr+gvsm+gvps+gvpw) AS q,
  IIF(gvpr+gvsm+gvps+gvpw=0 AND otoplz > 0, Tr_tv_co, 0) AS Tr_tv_co_zco,
  IIF(gvpr+gvsm+gvps+gvpw=0 AND otopln > 0, Tr_tv_co, 0) AS Tr_tv_co_nco,
  IIF(gvpr>0, Tr_tv_co, 0)                               AS Tr_tv_co_pr, 
  IIF(gvsm>0, Tr_tv_co, 0)                               AS Tr_tv_co_sm, 
  IIF(gvps>0, Tr_tv_co, 0)                               AS Tr_tv_co_ps, 
  IIF(gvpw>0, Tr_tv_co, 0)                               AS Tr_tv_co_pw, 
  Tr_tv_cv,
  0 AS v_otop,
  0 AS v_vent,
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
  [Потребитель реальный] PR,
  [Расчетная схема] PC
WHERE 
  PR.kod=PC.kod_rs
AND 
  (ORG.nomer_owner=kod_owner OR ((kod_owner IS NULL OR kod_owner = 0) AND ORG.nomer_owner=0))

UNION ALL

SELECT 
  kod_ist,
  sost,'О',
  kod, uzel, name_building,
  0,
  0,
  0,
  0,
  0,
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
  Tr_tv_cv,
  0,
  0,
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
  [Потребитель обобщенный] PO,
  [Расчетная схема] PC
WHERE 
  PO.kod=PC.kod_rs
AND 
  (ORG.nomer_owner=kod_owner OR ((kod_owner IS NULL OR kod_owner = 0) AND ORG.nomer_owner=0))
