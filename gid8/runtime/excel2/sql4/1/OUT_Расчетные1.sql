SELECT 
 kod_ist,
 kod_kod,
  name_ist,
 aa0,
 aa1,
 aa2,
 aa3,
 aa4,
 aa5,
 aa6,
 aa7,
 aa8,
 aa9,
 aa10,
 aa11,
 aa12,
 aa13,
 aa14,
 aa15,
 aa16,
 aa17,
 aa18,
 aa19



FROM
(
SELECT 
  PC.kod_ist AS kod_ist,
  '' AS kod_kod,
  name_ist,
  sum(otoplz) AS aa0,
  sum(G_otoplz) AS aa1,
  sum(otopln) AS aa2,
  sum(G_otopln) AS aa3,
  sum(venkond) AS aa4,
  sum(G_venkond) AS aa5,
  sum(Qras_gv_z) AS aa6,
  sum(G_gvz) AS aa7,
  sum(gvop) AS aa8,
  sum(G_gvop) AS aa9,
  sum(gvoo) AS aa10,
  sum(G_gvoo) AS aa11,
  sum(rez_q) AS aa12,
  sum(G_rez) AS aa13,
  sum(Qras_gv_z+otopln+otoplz+venkond) AS aa14,
  sum(G_gz) AS aa15,
  sum(gvop) AS aa16,
  sum(G_gvop) AS aa17,
  sum(gvoo) AS aa18,
  sum(G_gvoo) AS aa19,
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
  sum(otoplz),
  sum(G_otoplz),
  sum(otopln),
  sum(G_otopln),
  sum(venkond),
  sum(G_venkond),
  sum(Qras_gv_z),
  sum(G_gvz),
  sum(gvop),
  sum(G_gvop),
  sum(gvoo),
  sum(G_gvoo),
  sum(rez_q),
  sum(G_rez),
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
  sum(otoplz),
  sum(G_otoplz),
  sum(otopln),
  sum(G_otopln),
  sum(venkond),
  sum(G_venkond),
  sum(Qras_gv_z),
  sum(G_gvz),
  sum(gvop),
  sum(G_gvop),
  sum(gvoo),
  sum(G_gvoo),
  sum(rez_q),
  sum(G_rez),
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