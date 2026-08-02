SELECT 
  PT.kod_ist,
  TEMP.M, 
sum(
IIF(period=1, otoplz, 0)*(Tr_tv_co_zco-TEMP.tn_long)/(Tr_tv_co_zco-CT.t_or)+
IIF(period=1, otopln, 0)*(Tr_tv_co_nco-TEMP.tn_long)/(Tr_tv_co_nco-CT.t_or)+
IIF(period=1, tp, 0)+
IIF(period=1, ventil, 0)*(Tr_tv_cv-TEMP.tn_long)/(Tr_tv_cv-CT.t_vr)+
IIF(period=1, IIF(kondiz IS NULL, 0, kondiz),  0)*(Tr_tv_cv-TEMP.tn_long)/(Tr_tv_cv-CT.t_vr)+
gvop+gvoo+rez_q+gvpr+gvsm+gvps+gvpw)*length*24 AS q

FROM 
  (
#include Потребители.sql
  ) PT,
  (
#include Температуры.sql
  ) TEMP,
  [Система теплоснабжения] CT

WHERE
  TEMP.kod_ist=PT.kod_ist

GROUP BY
  PT.kod_ist,
  TEMP.M, 
  TEMP.MON, 
  period,
  length
