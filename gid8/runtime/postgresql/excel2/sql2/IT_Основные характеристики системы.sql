SELECT
  name,
  q_treb,
  q,
  g,
  col,
  t1,
  t1_zad,
  t2,
  dlina,
  otm

FROM
(

SELECT  
  0 AS num,
  'Источник тепла: ' + sourceName AS name,
  NULL AS q,
  NULL AS q_treb,
  NULL AS g,
  NULL AS g_ur,
  NULL AS col,
  NULL AS t1,
  NULL AS t2,
  NULL AS t1_zad,
  NULL AS dlina,
  NULL AS otm,
  sourceName AS kod_ist
   
FROM heatSources hs
JOIN nodes n ON n.id=hs.nodeID
WHERE fileID=1


UNION ALL


SELECT  
  num,
  name,
  IIF (Z.q = 0, NULL, Z.q) AS q,
  IIF (Z.q_treb = 0, NULL, Z.q_treb) AS q_treb,
  IIF (Z.g = 0, NULL, Z.g) AS g,
  IIF (Z.g_ur = 0, NULL, Z.g_ur) AS g_ur,
  IIF (Z.col = 0, NULL, Z.col) AS col,
  IIF (Z.t1 = 0, NULL, Z.t1) AS t1,
  IIF (Z.t2 = 0, NULL, Z.t2) AS t2,
  IIF (Z.t1_zad = 0, NULL, Z.t1_zad) AS t1_zad,
  IIF (Z.dlina = 0, NULL, Z.dlina) AS dlina,
  IIF (Z.otm = 0, NULL, Z.otm) AS otm,
  
  kod_ist

FROM
(

#include ../sql/Основные характеристики 1.sql

UNION ALL

#include ../sql/Источники.sql

UNION ALL

 SELECT  
  31 AS num,
  'Напор, миним. в подающем трубопроводе' AS name,
  0, 0, 0, 0, 0,
  min(IIF(pr='П', pih, 100000)),
  0,0,0,0,
  PC.kod_ist

 FROM 
   US_OUT,
  [Расчетная схема] PC
 WHERE
   (US_OUT.kod_p = '' OR US_OUT.kod_p IS NULL)
   AND US_OUT.kod=PC.kod_rs
 GROUP BY PC.kod_ist


UNION ALL

 SELECT  
  31 AS num,
  'Напор, максим. в обратном трубопроводе' AS name,
  0, 0, 0, 0, 0,
  max(IIF(pr='О', pih, -100000)),
  0,0,0,0,
  PC.kod_ist

 FROM 
   US_OUT,
  [Расчетная схема] PC
 WHERE
   (US_OUT.kod_p = '' OR US_OUT.kod_p IS NULL)
   AND US_OUT.kod=PC.kod_rs
 GROUP BY PC.kod_ist

UNION ALL

 SELECT  
  32 AS num,
  'Напор, наименьший располаг. у потребителей' AS name,
  0, 0, 0, 0, 0,
  min(PT_OUT.a23) AS min_rasp,
  0,0,0,0,
  PC.kod_ist
 FROM 
  PT_OUT,
  [Расчетная схема] PC
 WHERE
   PT_OUT.kod=PC.kod_rs
 GROUP BY 
  PC.kod_ist

UNION ALL

 SELECT  
  33 AS num,
  'Длина, суммарная подающего трубопр.' AS name,
  0, 0, 0, 0, 0,
  0,
  0,0,
  sum(UT0.dlina),
  0,
  PC.kod_ist
FROM 
  [Участок теплопровода] UT0,
  [Расчетная схема] PC
 WHERE
   UT0.kod1=PC.kod_rs
 AND UT0.kod1 <> 'О'
 AND (UT0.kod_p = '' OR UT0.kod_p IS NULL)
GROUP BY 
  PC.kod_ist

UNION ALL

 SELECT  
  34 AS num,
  'Максимальная разность отметки местн.' AS name,
  0, 0, 0, 0, 0,
  0,
  0,
  0,0,
  max(US.h)-min(US.h),
  PC.kod_ist
FROM 
  (
    SELECT  kod, uzel, h
    FROM [Узел] US0
    WHERE (US0.kod_p = '' OR US0.kod_p IS NULL)
    UNION ALL
    SELECT  kod, uzel, geodz
    FROM [Потребитель реальный]
    UNION ALL
    SELECT  kod, uzel, geodz
    FROM [Потребитель обобщенный]
  ) US,
  [Расчетная схема] PC
 WHERE
   US.kod=PC.kod_rs
GROUP BY 
  PC.kod_ist



UNION ALL

SELECT  
  99 AS num,
  '' AS name,
  NULL AS q,
  NULL AS q_treb,
  NULL AS g,
  NULL AS g_ur,
  NULL AS col,
  NULL AS t1,
  NULL AS t2,
  NULL AS t1_zad,
  NULL AS dlina,
  NULL AS otm,
  IST8.kod_ist

FROM
  [Источник тепла] IST8


) Z

ORDER BY
  kod_ist, num
)
