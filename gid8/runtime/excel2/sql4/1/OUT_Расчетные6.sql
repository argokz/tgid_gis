SELECT 
 kod_ist,
 kod_kod,
 name_ist,
 Qsum_z_,
 a15_,
 dop18_,
 a12_,
 dop19_,
 a13_,
 Qz_treb_,
 G_gz_,
 Qgvop_treb_,
 G_gvop_,
 Qgvoo_treb_,
 G_gvoo_,
 gz_r,
 G_gz_r,
 gvop_r,
 G_gvop_r,
 gvoo_r,
 G_gvoo_r

FROM
(
SELECT 
  PC.kod_ist AS kod_ist,
  '' AS kod_kod,
  name_ist,

  sum(Qsum_z) AS Qsum_z_,
  sum(a15) AS a15_,
  sum(dop18) AS dop18_,
  sum(a12) AS a12_,
  sum(dop19) AS dop19_,
  sum(a13) AS a13_,

  sum(Qz_treb) AS Qz_treb_,
  sum(G_gz) AS G_gz_,
  sum(Qgvop_treb) AS Qgvop_treb_,
  sum(G_gvop) AS G_gvop_,
  sum(Qgvoo_treb) AS Qgvoo_treb_,
  sum(G_gvoo) AS G_gvoo_,

  sum(Qras_gv_z+otopln+otoplz+venkond) AS gz_r,
  sum(G_gz) AS G_gz_r,
  sum(gvop) AS gvop_r,
  sum(G_gvop) AS G_gvop_r,
  sum(gvoo) AS gvoo_r,
  sum(G_gvoo) AS G_gvoo_r,

  2 AS num

FROM
(
#include OUT_Расчетные нагрузки2.sql
) PT,
[Расчетная схема] PC,
[Источник тепла] IST
WHERE 
  PC.kod_rs=PT.kod
AND 
  PC.kod_ist=IST.kod_ist

GROUP BY PC.kod_ist, name_ist


UNION

SELECT 
  PC.kod_ist,
  kod, 
  '',

  sum(Qsum_z),
  sum(a15),
  sum(dop18),
  sum(a12),
  sum(dop19),
  sum(a13),

  sum(Qz_treb),
  sum(G_gz),
  sum(Qgvop_treb),
  sum(G_gvop),
  sum(Qgvoo_treb),
  sum(G_gvoo),

  sum(Qras_gv_z+otopln+otoplz+venkond),
  sum(G_gz),
  sum(gvop),
  sum(G_gvop),
  sum(gvoo),
  sum(G_gvoo),

  1 AS num

FROM
(
#include OUT_Расчетные нагрузки2.sql
)
GROUP BY kod, PC.kod_ist

UNION

SELECT 
  'Итого', '','',

  sum(Qsum_z),
  sum(a15),
  sum(dop18),
  sum(a12),
  sum(dop19),
  sum(a13),

  sum(Qz_treb),
  sum(G_gz),
  sum(Qgvop_treb),
  sum(G_gvop),
  sum(Qgvoo_treb),
  sum(G_gvoo),

  sum(Qras_gv_z+otopln+otoplz+venkond),
  sum(G_gz),
  sum(gvop),
  sum(G_gvop),
  sum(gvoo),
  sum(G_gvoo),

  3 AS num

FROM
(
#include OUT_Расчетные нагрузки2.sql
)
)
ORDER BY num, kod_ist, kod_kod