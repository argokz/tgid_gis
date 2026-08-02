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
 aa19,


 aa20,
 aa21,
 aa22,
 aa23,
 aa24,
 aa25,
 aa26,
 aa27,
 aa28,
 aa29


FROM
(
SELECT 
  PC.kod_ist AS kod_ist,
  '' AS kod_kod,
  name_ist,

  sum(Qotz) AS aa0,
  sum(a4) AS aa1,
  sum(Qotn) AS aa2,
  sum(a5) AS aa3,
  sum(dop12+dop13) AS aa4,
  sum(a6+a7) AS aa5,
  sum(dop17) AS aa6,
  sum(a11) AS aa7,
  sum(Qsum_z) AS aa8,
  sum(a15) AS aa9,

  sum(Qotz_treb) AS aa10,
  sum(G_otoplz) AS aa11,
  sum(Qotn_treb) AS aa12,
  sum(G_otopln) AS aa13,
  sum(Qvent_treb) AS aa14,
  sum(G_venkond) AS aa15,
  sum(Qgvz_treb) AS aa16,
  sum(G_gvz) AS aa17,
  sum(Qz_treb) AS aa18,
  sum(G_gz) AS aa19,


  sum(otoplz) AS aa20,
  sum(G_otoplz) AS aa21,
  sum(otopln) AS aa22,
  sum(G_otopln) AS aa23,
  sum(venkond) AS aa24,
  sum(G_venkond) AS aa25,
  sum(Qras_gv_z) AS aa26,
  sum(G_gvz) AS aa27,
  sum(Qras_gv_z+otopln+otoplz+venkond) AS aa28,
  sum(G_gz) AS aa29,


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


  sum(Qotz),
  sum(a4),
  sum(Qotn),
  sum(a5),
  sum(dop12+dop13),
  sum(a6+a7),
  sum(dop17),
  sum(a11),
  sum(Qsum_z),
  sum(a15),

  sum(Qotz_treb),
  sum(G_otoplz),
  sum(Qotn_treb),
  sum(G_otopln),
  sum(Qvent_treb),
  sum(G_venkond),
  sum(Qgvz_treb),
  sum(G_gvz),
  sum(Qz_treb),
  sum(G_gz),


  sum(otoplz),
  sum(G_otoplz),
  sum(otopln),
  sum(G_otopln),
  sum(venkond),
  sum(G_venkond),
  sum(Qras_gv_z),
  sum(G_gvz),
  sum(Qras_gv_z+otopln+otoplz+venkond),
  sum(G_gz),


  1 AS num

FROM
(
#include OUT_Расчетные нагрузки2.sql
)
GROUP BY kod, PC.kod_ist

UNION

SELECT 
  'Итого', '','',

  sum(Qotz),
  sum(a4),
  sum(Qotn),
  sum(a5),
  sum(dop12+dop13),
  sum(a6+a7),
  sum(dop17),
  sum(a11),
  sum(Qsum_z),
  sum(a15),

  sum(Qotz_treb),
  sum(G_otoplz),
  sum(Qotn_treb),
  sum(G_otopln),
  sum(Qvent_treb),
  sum(G_venkond),
  sum(Qgvz_treb),
  sum(G_gvz),
  sum(Qz_treb),
  sum(G_gz),


  sum(otoplz),
  sum(G_otoplz),
  sum(otopln),
  sum(G_otopln),
  sum(venkond),
  sum(G_venkond),
  sum(Qras_gv_z),
  sum(G_gvz),
  sum(Qras_gv_z+otopln+otoplz+venkond),
  sum(G_gz),


  3 AS num

FROM
(
#include OUT_Расчетные нагрузки2.sql
)
)
ORDER BY num, kod_ist, kod_kod