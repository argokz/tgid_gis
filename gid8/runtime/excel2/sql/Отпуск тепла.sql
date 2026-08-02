SELECT 
  PT.kod_ist,
  TEMP1.M, 

sum(
IIF(period<>0, otoplz, 0)*(Tr_tv_co_zco-TEMP1.tn_long)/(Tr_tv_co_zco-CT1.t_or)+
IIF(period<>0, otopln, 0)*(Tr_tv_co_nco-TEMP1.tn_long)/(Tr_tv_co_nco-CT1.t_or)+
IIF(period<>0, tp, 0)+
IIF(period<>0, ventil, 0)*(Tr_tv_cv-TEMP1.tn_long)/(Tr_tv_cv-CT1.t_vr)+
IIF(period<>0, IIF(kondiz IS NULL, 0, kondiz), 0)*(Tr_tv_cv-TEMP1.tn_long)/(Tr_tv_cv-CT1.t_vr)+
gvop+gvoo+rez_q+gvpr+gvsm+gvps+gvpw) AS qh,
qh*length*24 AS q,
sum(
IIF(period<>0, otoplz, 0)*(Tr_tv_co_zco-TEMP1.tn_fakt)/(Tr_tv_co_zco-CT1.t_or)+
IIF(period<>0, otopln, 0)*(Tr_tv_co_nco-TEMP1.tn_fakt)/(Tr_tv_co_nco-CT1.t_or)+
IIF(period<>0, tp, 0)+
IIF(period<>0, ventil, 0)*(Tr_tv_cv-TEMP1.tn_fakt)/(Tr_tv_cv-CT1.t_vr)+
IIF(period<>0, IIF(kondiz IS NULL, 0, kondiz), 0)*(Tr_tv_cv-TEMP1.tn_fakt)/(Tr_tv_cv-CT1.t_vr)+
gvop+gvoo+rez_q+gvpr+gvsm+gvps+gvpw) AS qh_fakt,
qh*length_fakt*24 AS q_fakt


FROM 
  (
#include Потребители.sql
  ) PT,
  (
#include Температуры Полные.sql
  ) TEMP1,
  [Система теплоснабжения] CT1

WHERE
  TEMP1.kod_ist=PT.kod_ist

GROUP BY
  PT.kod_ist,
  TEMP1.M, 
  TEMP1.MON, 
  period,
  length,
  length_fakt
