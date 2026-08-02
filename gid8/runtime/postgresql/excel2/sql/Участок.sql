SELECT 
  UT0.kod1, UT0.uzel1, UT0.pr1, UT0.kod2, UT0.uzel2, UT0.pr2, 
  UT0.name_typ,
  IIF(UT0.name_typ='Н','Надземная', IIF(UT0.name_typ='К','Канальная', 'Безканальная')) AS name_typ_f, 
  YEAR(UT0.dateend) AS year_n, 
  IIF(YEAR(dateend) < 1990, 1, IIF(YEAR(dateend) < 1998, 2, IIF(YEAR(dateend) <= 2003, 3, 4))) AS y,
  UT0.kolwork,
  UT0.kti,
  UT0.diametr+UT0.tol*2 AS diam_vn, 
  UT0.diametr, 
  UT0.diametr_usl, 
  UT0.tol AS tol, 
  UT0.dlina AS dlina_p, 
  (UT0.diametr+UT0.tol*2)*0.001*UT0.dlina*IIF(UT0.name_typ = 'Н' AND UT0.pr1<>'О ', 1, 0) AS MP,
  (UT0.diametr+UT0.tol*2)*0.001*UT0.dlina*IIF(UT0.name_typ = 'Н' AND UT0.pr1<>'П ', 1, 0) AS MO,
  (UT0.diametr+UT0.tol*2)*0.001*UT0.dlina*IIF(UT0.name_typ <> 'Н', 1, 0)*IIF(UT0.pr1=' ', 2, 1) AS M,
  (M+MP+MO)/UT_I.M AS dM
  
FROM 
  [Участок теплопровода] UT0,
  VYD,
  VYD VYD2,
  ( 
    SELECT name_typ, sum((diametr+tol*2)*dlina*0.001*IIF(pr1=' ', 2, 1)) AS M
    FROM [Участок теплопровода] GROUP BY name_typ
  ) UT_I
WHERE 
  (UT0.kod_p = '' OR UT0.kod_p IS NULL)
  AND
    UT0.kod1=VYD.kod
  AND
    UT0.uzel1=VYD.uzel
  AND
    UT0.kod2=VYD2.kod
  AND
    UT0.uzel2=VYD2.uzel
  AND
    UT0.name_typ=UT_I.name_typ
