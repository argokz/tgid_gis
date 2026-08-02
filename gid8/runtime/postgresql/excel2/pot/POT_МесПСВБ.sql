SELECT 
  MON,
  Ven*CT.a/100*24*length+IIF(OB5.period=0, Ven*(1.5+0.5)*lengthR/lengthRL, 0)+REG.SlivEn AS VenP,
  Vpr*CT.a/100*24*length+IIF(OB5.period=0, Vpr*(1.5+0.5)*lengthR/lengthRL, 0)+REG.SlivPr AS VprP,
  Vbp*CT.a/100*24*length+IIF(OB5.period=0, Vbp*(1.5+0.5)*lengthR/lengthRL, 0)+REG.SlivBp AS VbpP,
  Vmg*CT.a/100*24*length+IIF(OB5.period=0, Vmg*(1.5+0.5)*lengthR/lengthRL, 0)+REG.SlivMg AS VmgP,

  VprP+VbpP+VmgP AS VdrP,
  VenP+VdrP+VbpP+VmgP AS VuP,

  IIF(OB5.period=1, Vprib1*CT.a/100*24*length, Vprib1*(1.5+0.5)*lengthR/lengthRL) AS VpprP,
  IIF(OB5.period=1, Vbezpr1*CT.a/100*24*length, Vbezpr1*(1.5+0.5)*lengthR/lengthRL) AS VpbpP,
  VpprP+VpbpP AS VpP,
  VpP+VuP,
  OB5.period AS period,
  TEMP_IST_OUT.m

FROM
(
#include ../sql/Объемы5.sql
) OB5,
  (
#include ../sql/Регуляторы2.sql
  ) REG,
  [Система теплоснабжения] CT,
  TEMP_IST_OUT,
  TEMP_PROD_OUT
WHERE
  TEMP_IST_OUT.kod_ist=OB5.kod_ist
AND  
  TEMP_IST_OUT.kod_ist='$IST$'
AND 
  TEMP_PROD_OUT.kod_ist=OB.kod_ist 
AND 
  REG.m=TEMP_IST_OUT.m
AND 
  REG.kod_ist=OB5.kod_ist
AND
 OB5.period=TEMP_IST_OUT.period

ORDER BY TEMP_IST_OUT.m
