SELECT 
  kod1,
  uzel1,
  kod2,
  uzel2,
  sum(IIF(pr1<>'Î', a13, 0)) AS a13P,
  sum(IIF(pr1<>'Ï', a13, 0)) AS a13O

FROM 
  UT_OUT
WHERE
  (kod_p = '' OR kod_p IS NULL)
GROUP BY
  kod1,
  uzel1,
  kod2,
  uzel2
