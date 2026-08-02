SELECT
  UT0.kod1, UT0.uzel1, UT0.pr1, UT0.kod2, UT0.uzel2, UT0.pr2, 
  UT0.name_typ,
  name_typ_f, 
  year_n, 
  y,
  UT0.kolwork,
  UT0.kti AS kti,
  diam_vn, 
  UT0.diametr, 
  UT0.diametr_usl, 
  dlina_p, 
  MP,
  MO,
  M,
  dM,
  TEPLO_OUT0.q_P AS q_P,
  TEPLO_OUT0.q_O AS q_O

FROM 
  (
#include Участок.sql
  ) UT1,
  (
#include TEPLO_OUT.sql
  ) TEPLO_OUT0
WHERE 
  UT1.kod1=TEPLO_OUT0.kod
AND 
  UT1.y=TEPLO_OUT0.y
AND 
  UT1.name_typ=TEPLO_OUT0.name_typ
AND 
  UT1.kolwork=TEPLO_OUT0.kolwork
AND 
  UT1.diam_vn=TEPLO_OUT0.diametr AND UT1.diametr_usl=TEPLO_OUT0.diametr_usl
