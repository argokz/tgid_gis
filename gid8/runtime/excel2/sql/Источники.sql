SELECT 
  TBI.num, 
  TBI.name,
  0 AS q,
  0 AS q_treb,
  0 AS g,
  0 AS g_ur,
  0 AS col,
  IIF(TBI.num=23, pihP-pihO, IIF(TBI.num=24, pihO, 0)) AS t1,
  IIF(TBI.num=21, tP, IIF(TBI.num=22, tO, 0)) AS t2,
  IIF(num=21, IST1.t1, IIF(num=22, IST1.t2, 0)) AS t1_zad,
  0 AS dlina,
  0 AS otm,
  TBI.kod_ist AS kod_ist
FROM
(
#include Источник.sql
) IST1,
(
SELECT *
FROM
(
          SELECT IST.kod_ist AS kod_ist, 21 AS num, 'Темп. подающий трубопровод' AS name  FROM [Источник тепла] IST
UNION ALL SELECT IST.kod_ist, 22, 'Темп. обратный трубопровод' FROM [Источник тепла] IST
UNION ALL SELECT IST.kod_ist, 23, 'Напор располаг. на источнике тепла' FROM [Источник тепла] IST
UNION ALL SELECT IST.kod_ist, 24, 'Напор подпиточных насосов' FROM [Источник тепла] IST

)
)
TBI

WHERE 
  TBI.kod_ist=IST1.kod_ist
