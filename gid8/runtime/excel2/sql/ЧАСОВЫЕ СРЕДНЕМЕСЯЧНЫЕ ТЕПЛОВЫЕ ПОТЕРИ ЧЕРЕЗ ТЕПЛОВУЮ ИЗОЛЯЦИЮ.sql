SELECT
  UT0.kod1, UT0.uzel1, UT0.pr1, UT0.kod2, UT0.uzel2, UT0.pr2, 
  IIF(UT0.name_typ='Н','Надземная', IIF(UT0.name_typ='К','Канальная', 'Безканальная')) AS typ, 
  UT0.year,
  IIF(UT0.kolwork, 'больше', 'меньше') AS kolwork,
  (UT0.diametr+UT0.tol*2)*0.001 AS diam_vn, 
  UT0.diametr_usl,
  UT0.dlina,

  UT0.dlina*qP*beta*UT0.kti AS qqP,
  UT0.dlina*qO*beta*UT0.kti AS qqO,
  UT0.dlina*q*beta*UT0.kti AS qq,

  UT_KTP_OUT.beta AS ktp,
  UT0.kti,
  q01,q02,q03,q04,q05,q06,q07,q08,q09,q10,q11,q12,
  UT0.dlina*q01*beta*UT0.kti AS qq01,
  UT0.dlina*q02*beta*UT0.kti AS qq02,
  UT0.dlina*q03*beta*UT0.kti AS qq03,
  UT0.dlina*q04*beta*UT0.kti AS qq04,
  UT0.dlina*q05*beta*UT0.kti AS qq05,
  UT0.dlina*q06*beta*UT0.kti AS qq06,
  UT0.dlina*q07*beta*UT0.kti AS qq07,
  UT0.dlina*q08*beta*UT0.kti AS qq08,
  UT0.dlina*q09*beta*UT0.kti AS qq09,
  UT0.dlina*q10*beta*UT0.kti AS qq10,
  UT0.dlina*q11*beta*UT0.kti AS qq11,
  UT0.dlina*q12*beta*UT0.kti AS qq12,


  IIF(UT0.pr1 = 'П', UT0.q,  0) AS qP,
  IIF(UT0.pr1 = 'О', UT0.q,  0) AS qO,

  IIF(UT0.pr1 = 'П' OR UT0.pr1 = 'О', 0, UT0.q) AS q,

  IIF(diam_vn >= 0.15, 1.15, IIF(UT0.name_typ='Б', 1.15, 1.2)) AS beta

FROM 
  VYD,
  VYD VYD2,
  UT_TEPLO_OUT UT0,
  UT_KTP_OUT
WHERE
  UT0.diametr+UT0.tol*2 = UT_KTP_OUT.diam
AND
  UT0.kod1=VYD.kod
AND
  UT0.uzel1=VYD.uzel
AND
  UT0.kod2=VYD2.kod
AND
  UT0.uzel2=VYD2.uzel
AND
  UT0.name_typ = UT_KTP_OUT.name_typ

