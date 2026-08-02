SELECT
  RG.kod_ist,
  m,
  sum(IIF(ORG.tip_owner='ЭН', tep_poteri*length*24,0)) AS SlivEn,
  sum(IIF(ORG.tip_owner='ЭН', 0, tep_poteri*length*24)) AS SlivDr,

  sum(IIF(ORG.tip_owner='ПР', tep_poteri*length*24, 0)) AS SlivPr,
  sum(IIF(ORG.tip_owner='БП', tep_poteri*length*24, 0)) AS SlivBp,
  sum(IIF(ORG.tip_owner='МГ', tep_poteri*length*24, 0)) AS SlivMg

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
) RG,
(
  SELECT nomer_owner, name, tip_owner FROM [Организация]
  UNION
   SELECT 
     0,
     "Не задано",
     "ЭН"
   FROM [Система теплоснабжения]
) ORG


WHERE 
  RG.kod_ist = TEMP_IST_OUT.kod_ist
AND 
  (ORG.nomer_owner=kod_owner OR ((kod_owner IS NULL OR kod_owner = 0) AND ORG.nomer_owner=0))
GROUP BY
  RG.kod_ist,
  m
  
