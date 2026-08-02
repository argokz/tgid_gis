SELECT
  UT0.kod1, UT0.uzel1, UT0.pr1, UT0.kod2, UT0.uzel2, UT0.pr2, 
  TEPLO_OUT.y AS y,
  UT0.name_typ AS name_typ, 
  (UT0.diametr+UT0.tol*2)*0.001 AS diam_vn, 
  UT0.tol,
  UT0.diametr*0.001, 
  UT0.dlina AS dlina_p,
  UT0.dlina*IIF(UT0.pr1=' ', 2, 1) AS dlina2,

  IIF(UT0.name_typ='Н' AND UT0.pr1 <> 'О', TEPLO_OUT.q_N_P, 0) AS qP,
  IIF(UT0.name_typ='Н' AND UT0.pr1 <> 'П', TEPLO_OUT.q_N_O, 0) AS qO,

  IIF(UT0.name_typ<>'Н', 
    IIF(UT0.pr1 <> 'О', IIF(UT0.name_typ='Б', TEPLO_OUT.q_B_P, TEPLO_OUT.q_K_P), 0)+
    IIF(UT0.pr1 <> 'П', IIF(UT0.name_typ='Б', TEPLO_OUT.q_B_O, TEPLO_OUT.q_K_O), 0), 0) AS q,
  IIF(diam_vn >= 0.15, 1.15, IIF(UT0.name_typ='Б', 1.15, 1.2)) AS beta,
  UT0.kti,

  UT0.diametr, 
  UT0.diametr_usl, 
  UT0.dlina,
  UT0.kolwork,
  PC.kod_ist AS kod_ist,
  
  qP,
  qO,
  q,
  UT0.dlina*qP*beta*UT0.kti AS qqP,
  UT0.dlina*qO*beta*UT0.kti AS qqO,
  UT0.dlina*q*beta*UT0.kti AS qq
     
FROM 
  [Участок теплопровода] UT0, 
  TEPLO_OUT,
  ( 
    SELECT name_typ, sum((diametr+tol*2)*dlina*0.001*IIF(pr1=' ', 2, 1)) AS M
    FROM [Участок теплопровода] GROUP BY name_typ
  ) UT_I,
  VYD,
  VYD VYD2,
  [Расчетная схема] PC

WHERE 
    (UT0.kod_p = '' OR UT0.kod_p IS NULL)
  AND
    UT0.name_typ=UT_I.name_typ
  AND
    UT0.kod1 = TEPLO_OUT.kod 
  AND 
    ((UT0.diametr+UT0.tol*2)=TEPLO_OUT.diametr AND UT0.diametr_usl=TEPLO_OUT.diametr_usl)
  AND 
    UT0.kolwork=TEPLO_OUT.kolwork
  AND 
    IIF(YEAR(dateend) < 1990, 1, IIF(YEAR(dateend) < 1998, 2, IIF(YEAR(dateend) <= 2003, 3, 4))) = TEPLO_OUT.y
  AND
    UT0.kod1=VYD.kod
  AND
    UT0.uzel1=VYD.uzel
  AND
    UT0.kod2=VYD2.kod
  AND
    UT0.uzel2=VYD2.uzel
  AND 
   UT0.kod1=PC.kod_rs
