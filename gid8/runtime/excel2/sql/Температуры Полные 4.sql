SELECT
kod_ist,
period,
ro,
lengthA,
IIF(lengthA > 0, t1Al/lengthA, 0) AS t1A,
IIF(lengthA > 0, t2Al/lengthA, 0) AS t2A,
IIF(lengthA > 0, tAl/lengthA, 0) AS tA,
IIF(lengthA > 0, tnAl/lengthA, 0) AS tnA,
IIF(lengthA > 0, tgrAl/lengthA, 0) AS tgrA

FROM 
(

SELECT
kod_ist,
period,
0.99 AS ro,
sum(length) AS lengthA,
sum(length*t1) AS t1Al,
sum(length*t2) AS t2Al,
sum(length*t) AS tAl,
sum(length*tn_long) AS tnAl,
sum(length*tgr_long) AS tgrAl
FROM TEMP_IST_OUT
GROUP BY 
  kod_ist,
  period

UNION ALL

SELECT
kod_ist,
2,
0.99 AS ro,
sum(length) AS lengthA,
sum(length*t1) AS t1Al,
sum(length*t2) AS t2Al,
sum(length*t) AS tAl,
sum(length*tn_long) AS tnAl,
sum(length*tgr_long) AS tgrAl
FROM TEMP_IST_OUT
GROUP BY 
  kod_ist
)