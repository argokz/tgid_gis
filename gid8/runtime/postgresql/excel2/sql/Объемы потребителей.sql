SELECT
sum(IIF(pr_uchet = 'У', v_otop+v_vent, 0)) AS Vprib,
sum(IIF(pr_uchet = 'У', 0, v_otop+v_vent)) AS Vbezpr,

sum(IIF(pr_uchet = 'У', 0, gvop+gvoo)) AS QObezpr,



PT.kod_ist AS kod_ist

FROM
(
#include Потребители.sql
) PT

WHERE
  sost <> '*'

GROUP BY
  PT.kod_ist
