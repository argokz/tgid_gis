SELECT 
  MON,
  IIF (period = 1, "Отопит",  IIF (period = 0,  "Летний", "Годовой")) AS zimaleto,
  sum(IIF(name_typ<>'Н', qq*(TAB2.t1+TAB2.t2-2*TAB2.tgr)/(t1Z+t2Z-2*tgrZ), 0))/1000000 AS ud,
  sum(IIF(name_typ='Н' AND pr1<>'О', qqP*(TAB2.t1-TAB2.tn)/(t1Z-tnZ), 0))/1000000 AS udP,
  sum(IIF(name_typ='Н' AND pr1<>'П', qqO*(TAB2.t2-TAB2.tn)/(t2Z-tnZ), 0))/1000000 AS udO,

  TAB2.length AS length,
  ud*length*24 AS udlen,
  (udP+udO)*length*24 AS udPOlen,
  (ud+udP+udO)*length*24 AS QQQ,
  OTPUSK.qh AS qh,
  OTPUSK.q AS q,
  IIF (OTPUSK.q=0, 0, QQQ/OTPUSK.q*100) AS qpq,
  period

FROM 
  (
#include ../sql/Участок3.sql
  ) SRGOD,

  (
  SELECT kod_ist, M, MON, 
  IIF(TAB.period IS NULL, 0, TAB.period) AS period, 
  IIF(TAB.tn_long IS NULL, 0, TAB.tn_long) AS tn, 
  IIF(TAB.tgr_long IS NULL, 0, TAB.tgr_long) AS tgr, 
  IIF(TAB.length IS NULL, 0, TAB.length) AS length, 
  IIF(TAB.t1 IS NULL, 0, TAB.t1) AS t1, 
  IIF(TAB.t2 IS NULL, 0, TAB.t2) AS t2
  FROM 
    (
#include ../sql/Температуры Полные.sql
    ) TAB
  ) TAB2,
  (
#include ../sql/Отпуск тепла.sql
  ) OTPUSK,
  TEMP_PROD_OUT

WHERE 
  OTPUSK.kod_ist=TAB2.kod_ist
AND 
  OTPUSK.M=TAB2.M
AND
  TEMP_PROD_OUT.kod_ist=TAB2.kod_ist
AND
  TEMP_PROD_OUT.kod_ist='$IST$'


GROUP BY
  TAB2.M,
  TAB2.MON,
  period,
  OTPUSK.q,
  OTPUSK.qh,
  TAB2.t1,
  TAB2.t2,
  TAB2.tn,
  TAB2.tgr,
  TAB2.length,
  t1A,
  t2A,
  tnA,
  tgrA
