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
 aa17

FROM
(
SELECT 
  PC.kod_ist AS kod_ist,
  '' AS kod_kod,
  name_ist,

  sum(dop18) AS aa0,
  sum(a12) AS aa1,
  sum(dop19) AS aa2,
  sum(a13) AS aa3,
  sum(dop20) AS aa4,
  sum(a14) AS aa5,

  sum(Qgvop_treb) AS aa6,
  sum(G_gvop) AS aa7,
  sum(Qgvoo_treb) AS aa8,
  sum(G_gvoo) AS aa9,
  sum(Qrez_treb) AS aa10,
  sum(G_rez) AS aa11,

  sum(gvop) AS aa12,
  sum(G_gvop) AS aa13,
  sum(gvoo) AS aa14,
  sum(G_gvoo) AS aa15,
  sum(rez_q) AS aa16,
  sum(G_rez) AS aa17,

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
  sum(dop18),
  sum(a12),
  sum(dop19),
  sum(a13),
  sum(dop20),
  sum(a14),

  sum(Qgvop_treb),
  sum(G_gvop),
  sum(Qgvoo_treb),
  sum(G_gvoo),
  sum(Qrez_treb),
  sum(G_rez),

  sum(gvop),
  sum(G_gvop),
  sum(gvoo),
  sum(G_gvoo),
  sum(rez_q),
  sum(G_rez),
  1 AS num

FROM
(
#include OUT_Расчетные нагрузки2.sql
)
GROUP BY kod, PC.kod_ist

UNION

SELECT 
  'Итого', '','',
  sum(dop18),
  sum(a12),
  sum(dop19),
  sum(a13),
  sum(dop20),
  sum(a14),

  sum(Qgvop_treb),
  sum(G_gvop),
  sum(Qgvoo_treb),
  sum(G_gvoo),
  sum(Qrez_treb),
  sum(G_rez),

  sum(gvop),
  sum(G_gvop),
  sum(gvoo),
  sum(G_gvoo),
  sum(rez_q),
  sum(G_rez),
  3 AS num

FROM
(
#include OUT_Расчетные нагрузки2.sql
)
)
ORDER BY num, kod_ist, kod_kod