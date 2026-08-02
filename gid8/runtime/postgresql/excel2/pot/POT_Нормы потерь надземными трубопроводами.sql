SELECT
  (UT.diametr+UT.tol*2) AS diam_vn,
  UT.diametr_usl,
  'П' AS po,
  sum(IIF(UT.kolwork AND UT.y=1, UT.dlina, 0)) AS dlina1,
  sum(IIF(UT.kolwork AND UT.y=2, UT.dlina, 0)) AS dlina2,
  sum(IIF(UT.kolwork AND UT.y=3, UT.dlina, 0)) AS dlina3,
  sum(IIF(UT.kolwork AND UT.y=4, UT.dlina, 0)) AS dlina4,

  max(IIF(UT.kolwork AND UT.y=1, UT.qP, 0)) AS qP1,
  max(IIF(UT.kolwork AND UT.y=2, UT.qP, 0)) AS qP2,
  max(IIF(UT.kolwork AND UT.y=3, UT.qP, 0)) AS qP3,
  max(IIF(UT.kolwork AND UT.y=4, UT.qP, 0)) AS qP4,

  sum(IIF(NOT UT.kolwork AND UT.y=1, UT.dlina, 0)) AS dlina5,
  sum(IIF(NOT UT.kolwork AND UT.y=2, UT.dlina, 0)) AS dlina6,
  sum(IIF(NOT UT.kolwork AND UT.y=3, UT.dlina, 0)) AS dlina7,
  sum(IIF(NOT UT.kolwork AND UT.y=4, UT.dlina, 0)) AS dlina8,

  max(IIF(NOT UT.kolwork AND UT.y=1, UT.qP, 0)) AS qP5,
  max(IIF(NOT UT.kolwork AND UT.y=2, UT.qP, 0)) AS qP6,
  max(IIF(NOT UT.kolwork AND UT.y=3, UT.qP, 0)) AS qP7,
  max(IIF(NOT UT.kolwork AND UT.y=4, UT.qP, 0)) AS qP8

FROM
  (
#include ../sql/Участок2.sql
  ) UT

WHERE 
  UT.name_typ='Н'
  AND
    UT.pr1 <> 'О'
  AND UT.kod_ist='$IST$'


GROUP BY
  UT.tol,
  UT.diametr,
  UT.diametr_usl

UNION

SELECT
  (UT.diametr+UT.tol*2) AS diam_vn,
  UT.diametr_usl,
  'О' AS po,
  sum(IIF(UT.kolwork AND UT.y=1, UT.dlina, 0)),
  sum(IIF(UT.kolwork AND UT.y=2, UT.dlina, 0)),
  sum(IIF(UT.kolwork AND UT.y=3, UT.dlina, 0)),
  sum(IIF(UT.kolwork AND UT.y=4, UT.dlina, 0)),

  max(IIF(UT.kolwork AND UT.y=1, UT.qO, 0)),
  max(IIF(UT.kolwork AND UT.y=2, UT.qO, 0)),
  max(IIF(UT.kolwork AND UT.y=3, UT.qO, 0)),
  max(IIF(UT.kolwork AND UT.y=4, UT.qO, 0)),

  sum(IIF(NOT UT.kolwork AND UT.y=1, UT.dlina, 0)),
  sum(IIF(NOT UT.kolwork AND UT.y=2, UT.dlina, 0)),
  sum(IIF(NOT UT.kolwork AND UT.y=3, UT.dlina, 0)),
  sum(IIF(NOT UT.kolwork AND UT.y=4, UT.dlina, 0)),

  max(IIF(NOT UT.kolwork AND UT.y=1, UT.qO, 0)),
  max(IIF(NOT UT.kolwork AND UT.y=2, UT.qO, 0)),
  max(IIF(NOT UT.kolwork AND UT.y=3, UT.qO, 0)),
  max(IIF(NOT UT.kolwork AND UT.y=4, UT.qO, 0))

FROM
  (
#include ../sql/Участок2.sql
  ) UT

WHERE 
    UT.name_typ='Н'
  AND
    UT.pr1 <> 'П'
  AND UT.kod_ist='$IST$'

GROUP BY
  UT.tol,
  UT.diametr,
  UT.diametr_usl

ORDER BY
  po DESC;

