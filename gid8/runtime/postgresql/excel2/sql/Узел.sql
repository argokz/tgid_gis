SELECT 
  kod,
  uzel,
  sum(IIF(pr<>'Î', pih, 0)) AS pihP,
  sum(IIF(pr<>'Ï', pih, 0)) AS pihO,
  sum(IIF(pr<>'Î', t, 0)) AS tP,
  sum(IIF(pr<>'Ï', t, 0)) AS tO

FROM 
  US_OUT
WHERE
  (kod_p = '' OR kod_p IS NULL)
GROUP BY
  kod,
  uzel
