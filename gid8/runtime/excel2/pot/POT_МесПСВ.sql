SELECT
  MON,
  round(IIF(period=1, VenZ, VenL), 3)*CT.a/100*24*length AS Ven,

  round(IIF(period=0, IIF(period=1, VenZ, VenL), 0), 3)*1.5*lengthR/lengthRL AS Vpusk,
  round(IIF(period=0, IIF(period=1, VenZ, VenL), 0), 3)*0.5*lengthR/lengthRL AS Visp,
  REG.SlivEn AS Vsliv,
  Ven+Vpusk+Visp+Vsliv AS VenItog,

  round(IIF(period=1, VprZ+VbpZ+VmgZ, VprL+VbpL+VmgL), 3)*CT.a/100*24*length AS Vdr,
  round(IIF(period=0, IIF(period=1, VprZ+VbpZ+VmgZ, VprL+VbpL+VmgL), 0), 3)*1.5*lengthR/lengthRL AS VpuskDr,
  round(IIF(period=0, IIF(period=1, VprZ+VbpZ+VmgZ, VprL+VbpL+VmgL), 0), 3)*0.5*lengthR/lengthRL AS VispDr,
  
  REG.SlivDr AS VslivDr,
  Vdr+VpuskDr+VispDr+VslivDr AS VdrItog,

  VenItog+VdrItog AS Vut,


  round(IIF(period=1, Vprib+Vbezpr, 0), 3)*CT.a/100*24*length AS Vpot,
  round(Vprib+Vbezpr, 3)*1.5*lengthR/lengthRL AS VpuskPot,
  round(Vprib+Vbezpr, 3)*0.5*lengthR/lengthRL AS VispPot,
  
  Vpot+VpuskPot+VispPot AS VpotItog,
  VpotItog+Vut AS VItog,
  period,
  OB.kod_ist AS kodist


FROM
  (
#include ../sql/Объемы.sql
  ) OB,
  (
#include ../sql/Регуляторы2.sql
  ) REG,
  [Система теплоснабжения] CT,
  TEMP_PROD_OUT,
  TEMP_IST_OUT

WHERE
  OB.kod_ist='$IST$'
AND
  TEMP_IST_OUT.kod_ist=OB.kod_ist
AND 
  REG.m=TEMP_IST_OUT.m
AND 
  REG.kod_ist=OB.kod_ist
AND 
  TEMP_PROD_OUT.kod_ist=OB.kod_ist 
