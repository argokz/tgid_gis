SELECT
  IIF(UT0.pr1 = 'О', 0, (UT.diametr+UT.tol*2)) AS diam_vnP,
  IIF(UT0.pr1 = 'П', 0, (UT.diametr+UT.tol*2)) AS diam_vnO,
  IIF(UT0.pr1 = 'О', 0, UT.diametr_usl),
  IIF(UT0.pr1 = 'П', 0, UT.diametr_usl),
  name_typ,

  sum(IIF(UT.kolwork AND UT.y=1, UT.dlina, 0)),
  sum(IIF(UT.kolwork AND UT.y=2, UT.dlina, 0)),
  sum(IIF(UT.kolwork AND UT.y=3, UT.dlina, 0)),
  sum(IIF(UT.kolwork AND UT.y=4, UT.dlina, 0)),

  max(IIF(UT.kolwork AND UT.y=1, IIF(UT0.pr1 = ' ', UT.q, IIF(UT0.pr1 = 'П', UT.qP, UT.qO)), 0)),
  max(IIF(UT.kolwork AND UT.y=2, IIF(UT0.pr1 = ' ', UT.q, IIF(UT0.pr1 = 'П', UT.qP, UT.qO)), 0)),
  max(IIF(UT.kolwork AND UT.y=3, IIF(UT0.pr1 = ' ', UT.q, IIF(UT0.pr1 = 'П', UT.qP, UT.qO)), 0)),
  max(IIF(UT.kolwork AND UT.y=4, IIF(UT0.pr1 = ' ', UT.q, IIF(UT0.pr1 = 'П', UT.qP, UT.qO)), 0)),

  sum(IIF(NOT UT.kolwork AND UT.y=1, UT.dlina, 0)),
  sum(IIF(NOT UT.kolwork AND UT.y=2, UT.dlina, 0)),
  sum(IIF(NOT UT.kolwork AND UT.y=3, UT.dlina, 0)),
  sum(IIF(NOT UT.kolwork AND UT.y=4, UT.dlina, 0)),

  max(IIF(NOT UT.kolwork AND UT.y=1, IIF(UT0.pr1 = ' ', UT.q, IIF(UT0.pr1 = 'П', UT.qP, UT.qO)), 0)),
  max(IIF(NOT UT.kolwork AND UT.y=2, IIF(UT0.pr1 = ' ', UT.q, IIF(UT0.pr1 = 'П', UT.qP, UT.qO)), 0)),
  max(IIF(NOT UT.kolwork AND UT.y=3, IIF(UT0.pr1 = ' ', UT.q, IIF(UT0.pr1 = 'П', UT.qP, UT.qO)), 0)),
  max(IIF(NOT UT.kolwork AND UT.y=4, IIF(UT0.pr1 = ' ', UT.q, IIF(UT0.pr1 = 'П', UT.qP, UT.qO)), 0)),

  kod_ist

FROM
  (
#include ../sql/Участок2.sql
  ) UT

WHERE 
  UT.name_typ<>'Н'
  AND UT.kod_ist='$IST$'


GROUP BY
  UT.tol,
  UT.diametr_usl,
  UT.diametr,
  name_typ,
  UT0.pr1,
  kod_ist

ORDER BY 
  name_typ,
  UT.diametr_usl

