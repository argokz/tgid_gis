SELECT 
  UT.key_ut_p,
  UT.kod1, UT.uzel1, UT.pr1, UT.kod2, UT.uzel2, UT.pr2, 
  IIF(UT.name_typ='Н','Надземная', IIF(UT.name_typ='К','Канальная', 'Безканальная')), 
  YEAR(UT.dateend), 
  (UT.diametr+UT.tol*2)*0.001 AS diam_vn, 
  UT.dlina AS dlina_p, 
  (UT.diametr+UT.tol*2)*0.001*UT.dlina*IIF(UT.name_typ = 'Н' AND UT.pr1<>'О ', 1, 0) AS MP,
  (UT.diametr+UT.tol*2)*0.001*UT.dlina*IIF(UT.name_typ = 'Н' AND UT.pr1<>'П ', 1, 0) AS MO,
  (UT.diametr+UT.tol*2)*0.001*UT.dlina*IIF(UT.name_typ <> 'Н', 1, 0)*IIF(UT.pr1=' ', 2, 1) AS M,
  (M+MP+MO)/UT_I.M,
  dlina_p*UT.diametr*UT.diametr/4*3.1415926/1000/1000*IIF(pr1=' ', 2, 1) AS V,
  IIF(UT.kolwork, V, 0) AS Vlet,
  ORG.name,
  ORG.tip_owner,
  PC.kod_ist AS kod_ist
  
FROM 
  [Участок теплопровода] UT, 
  [Расчетная схема] PC,
  [Организация] ORG,
  ( 
    SELECT name_typ, sum((diametr+tol*2)*dlina*0.001*IIF(pr1=' ', 2, 1)) AS M
    FROM [Участок теплопровода] GROUP BY name_typ
  ) UT_I
WHERE 
  (UT.kod_p = '' OR UT.kod_p IS NULL)
  AND
    UT.name_typ=UT_I.name_typ
  AND 
    (ORG.nomer_owner=kod_owner OR ORG.nomer_owner<>1)
  AND 
    UT.kod1=PC.kod_rs
