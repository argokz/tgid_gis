SELECT

sum(IIF(tip_owner = "ЭН", V, 0)) AS VenZ,
sum(IIF(tip_owner = "ЭН", Vlet, 0)) AS VenL,

sum(IIF(tip_owner = "ПР", V, 0)) AS VprZ,
sum(IIF(tip_owner = "ПР", Vlet, 0)) AS VprL,

sum(IIF(tip_owner = "БП", V, 0)) AS VbpZ,
sum(IIF(tip_owner = "БП", Vlet, 0)) AS VbpL,

sum(IIF(tip_owner = "МГ", V, 0)) AS VmgZ,
sum(IIF(tip_owner = "МГ", Vlet, 0)) AS VmgL,

MH.kod_ist AS kod_ist

FROM
(
#include Материальная характеристика.sql
) MH

GROUP BY
  MH.kod_ist
