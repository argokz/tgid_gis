SELECT
  MON,
  IIF(period=1, VenZ, VenL) AS VVen,
  IIF(period=1, VprZ+VbpZ+VmgZ, VprL+VbpL+VmgL) AS VVdr,
  24*length_fakt AS l_f24,
  24*length AS l_24,

  VenItog*((t1+t2)/2-t)*ro/1000 AS Qen,
  VdrItog*((t1+t2)/2-t)*ro/1000 AS Qdr,
  VpotItog*((t1+t2)/2-t)*ro/1000 AS Qpot,


  VfaktEn*((t1_fakt+t2_fakt)/2-t_fakt)*ro/1000 AS QenF,
  VfaktDr*((t1_fakt+t2_fakt)/2-t_fakt)*ro/1000 AS QdrF,
  VfaktPt*((t1_fakt+t2_fakt)/2-t_fakt)*ro/1000 AS QpotF,


  round(VVen, 3*CT.a/100*l_24) AS Ven,

  round(IIF(period=0, VVen, 0), 3)*1.5*lengthR/lengthRL AS Vpusk,
  round(IIF(period=0, VVen, 0), 3)*0.5*lengthR/lengthRL AS Visp,
  REG.SlivEn AS Vsliv,
  Ven+Vpusk+Visp+Vsliv AS VenItog,

  round(VVdr, 3)*CT.a/100*l_24 AS Vdr,
  round(IIF(period=0, VVdr, 0), 3)*1.5*lengthR/lengthRL AS VpuskDr,
  round(IIF(period=0, VVdr, 0), 3)*0.5*lengthR/lengthRL AS VispDr,
  
  REG.SlivDr AS VslivDr,
  Vdr+VpuskDr+VispDr+VslivDr AS VdrItog,

  VenItog+VdrItog AS Vut,

  round(IIF(period=1, Vprib+Vbezpr, 0), 3)*CT.a/100*l_24 AS Vpot,
  round(Vprib+Vbezpr, 3)*1.5*lengthR/lengthRL AS VpuskPot,
  round(Vprib+Vbezpr, 3)*0.5*lengthR/lengthRL AS VispPot,
 
  Vpot+VpuskPot+VispPot AS VpotItog,
  VpotItog+Vut AS VItog,

  TEMP_IST_OUT.Ggvs_pribor_fakt*l_f24 AS Ggvs_pribor_fakt,
  OB.QObezpr*1000/(60-t_fakt)*l_f24 AS QObezpr,
  TEMP_IST_OUT.Gakt_tex_fakt*l_f24 AS Gakt_tex_fakt,
  REG.SlivEn+REG.SlivDr AS Sliv,

  Gakt_tex_fakt+Sliv+Gakt_avarija_fakt AS Gg,
  TEMP_IST_OUT.Gakt_avarija_fakt*l_f24 AS Gakt_avarija_fakt,

  (Gpod_fakt-Gg)*VVen/(VVen+VVdr+Vprib+Vbezpr) AS VfaktEn,
  (Gpod_fakt-Gg)*VVdr/(VVen+VVdr+Vprib+Vbezpr) AS VfaktDr,
  (Gpod_fakt-Gg)*(Vprib+Vbezpr)/(VVen+VVdr+Vprib+Vbezpr) AS VfaktPt,

  TEMP_IST_OUT.Gpod_fakt*l_f24-(Ggvs_pribor_fakt+QObezpr) AS Gpod_fakt,

  period

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
