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
  sum(Qotz) AS aa0,
  sum(a4) AS aa1,
  sum(Qotn) AS aa2,
  sum(a5) AS aa3,
  sum(dop12+dop13) AS aa4,
  sum(a6+a7) AS aa5,
  sum(dop17) AS aa6,
  sum(a11) AS aa7,
  sum(dop18) AS aa8,
  sum(a12) AS aa9,
  sum(dop19) AS aa10,
  sum(a13) AS aa11,
  sum(dop20) AS aa12,
  sum(a14) AS aa13,
  sum(Qotz+Qotn+dop12+dop13+dop17) AS aa14,
  sum(a15) AS aa15,
  sum(dop18) AS aa16,
  sum(a12) AS aa17,
  sum(dop19) AS aa18,
  sum(a13) AS aa19,
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
  sum(Qotz) AS aa0,
  sum(a4) AS aa1,
  sum(Qotn) AS aa2,
  sum(a5) AS aa3,
  sum(dop12+dop13) AS aa4,
  sum(a6+a7) AS aa5,
  sum(dop17) AS aa6,
  sum(a11) AS aa7,
  sum(dop18) AS aa8,
  sum(a12) AS aa9,
  sum(dop19) AS aa10,
  sum(a13) AS aa11,
  sum(dop20) AS aa12,
  sum(a14) AS aa13,
  sum(Qotz+Qotn+dop12+dop13+dop17) AS aa14,
  sum(a15) AS aa15,
  sum(dop18) AS aa16,
  sum(a12) AS aa17,
  sum(dop19) AS aa18,
  sum(a13) AS aa19,
  1 AS num

FROM
(
#include OUT_Расчетные нагрузки2.sql
)
GROUP BY kod, PC.kod_ist

UNION

SELECT 
  'Итого', '','',
  sum(Qotz) AS aa0,
  sum(a4) AS aa1,
  sum(Qotn) AS aa2,
  sum(a5) AS aa3,
  sum(dop12+dop13) AS aa4,
  sum(a6+a7) AS aa5,
  sum(dop17) AS aa6,
  sum(a11) AS aa7,
  sum(dop18) AS aa8,
  sum(a12) AS aa9,
  sum(dop19) AS aa10,
  sum(a13) AS aa11,
  sum(dop20) AS aa12,
  sum(a14) AS aa13,
  sum(Qotz+Qotn+dop12+dop13+dop17) AS aa14,
  sum(a15) AS aa15,
  sum(dop18) AS aa16,
  sum(a12) AS aa17,
  sum(dop19) AS aa18,
  sum(a13) AS aa19,
  3 AS num

FROM
(
#include OUT_Расчетные нагрузки2.sql
)
)
ORDER BY num, kod_ist, kod_kod