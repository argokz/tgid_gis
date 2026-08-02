SELECT 
  num,
  name,
  name2,
  name3,
  name4,
  name5,
  IIF(num = 1, lengthZ, IIF(num = 2, lengthL, lengthA)) AS len,
  IIF(num=1, VenZ, IIF(num=2, VenL, IIF(num=3, (VenZ*lengthZ+VenL*lengthL)/lengthA, 0))) AS Ven,
  IIF(num=1, VprZ, IIF(num=2, VprL, IIF(num=3, (VprZ*lengthZ+VprL*lengthL)/lengthA, 0))) AS Vpr,
  IIF(num=1, VbpZ, IIF(num=2, VbpL, IIF(num=3, (VbpZ*lengthZ+VbpL*lengthL)/lengthA, 0))) AS Vbp,
  IIF(num=1, VmgZ, IIF(num=2, VmgL, IIF(num=3, (VmgZ*lengthZ+VmgL*lengthL)/lengthA, 0))) AS Vmg,
  Vpr+Vbp+Vmg AS Vdrug,
  Ven+Vpr+Vbp+Vmg AS Vuch,

  IIF(num=1, Vprib, IIF(num=2, 0, IIF(num=3, Vprib*lengthZ/lengthA, 0))) AS Vprib1,
  IIF(num=1, Vbezpr, IIF(num=2, 0, IIF(num=3, Vbezpr*lengthZ/lengthA, 0))) AS Vbezpr1,
  Vprib1+Vbezpr1 AS Vpot,
  Vuch+Vpot AS Vall,


  TAB.kod_ist AS kod_ist
FROM 
(
#include Объемы.sql
) OB,
TEMP_PROD_OUT,
  (
SELECT IST1.kod_ist AS kod_ist, 1 AS num, 
          'Объем, отопительный сезон, м3' AS name, 
          'Норма утечки, отопительный, м3/ч' AS name2,
          'ПСВ с норм.утечкой, отопительный, м3' AS name3,
          'Нормы тепл.потерь, отопительный, Гкал/ч' AS name4,
          'Норм.тепловые потери, отопительный, Гкал' AS name5
          FROM [Источник тепла] IST1
UNION ALL SELECT IST1.kod_ist, 2,'Объем, летний сезон, м3', 
                                 'Норма утечки, летний сезон, м3/ч', 
                                 'ПСВ с норм.утечкой, летний сезон, м3',
                                 'Нормы тепл.потерь, летний сезон, Гкал/ч',
                                 'Норм.тепловые потери, летний сезон, Гкал'
          FROM [Источник тепла] IST1
UNION ALL SELECT IST1.kod_ist, 3,'         среднесезонный, м3', 
                                 '         среднесезонный, м3/ч', 
                                 '         сезонные, м3',
                                 '         сезонные, Гкал/ч',
                                 '         сезонные, Гкал'
          FROM [Источник тепла] IST1
) TAB

WHERE 
  TAB.kod_ist= OB.kod_ist
AND
  TAB.kod_ist= TEMP_PROD_OUT.kod_ist
