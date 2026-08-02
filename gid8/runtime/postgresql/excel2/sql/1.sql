SELECT
  RG.kod_ist,
  m,
  sum(tep_poteri*length) AS SlivMg

FROM 
  TEMP_IST_OUT,
(
SELECT ALL
  kod_ist,
  kod_owner,
  sum(g_tep_poteri) AS tep_poteri
FROM
(
#include Регуляторы.sql
)
WHERE 
   pr_raboti <> "Летний"

GROUP BY
  kod_ist,
  kod_owner
) RG


WHERE 
  RG.kod_ist = TEMP_IST_OUT.kod_ist
GROUP BY
  RG.kod_ist,
  m
  
