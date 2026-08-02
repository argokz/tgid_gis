SELECT
  diametr,
  diametr,
  sum(IIF(y=1 AND name_typ='Н', dlina, 0)),
  sum(IIF(y=2 AND name_typ='Н', dlina, 0)),
  sum(IIF(y=3 AND name_typ='Н', dlina, 0)),
  sum(IIF(y=4 AND name_typ='Н', dlina, 0)),
  sum(IIF(name_typ='Н', dlina, 0)) AS dlinaN,
  sum(IIF(y=1 AND name_typ<>'Н', dlina, 0)),
  sum(IIF(y=2 AND name_typ<>'Н', dlina, 0)),
  sum(IIF(y=3 AND name_typ<>'Н', dlina, 0)),
  sum(IIF(y=4 AND name_typ<>'Н', dlina, 0)),
  sum(IIF(name_typ<>'Н', dlina, 0)) AS dlinaP,
  sum(dlina) AS dlinaA,
  kod_ist

FROM
(
SELECT 
  PC.kod_ist AS kod_ist,
  diametr,
  IIF(YEAR(dateend) < 1990, 1, IIF(YEAR(dateend) < 1998, 2, IIF(YEAR(dateend) <= 2003, 3, 4))) AS y,
  name_typ,
  UT.dlina*IIF(pr1=' ', 2, 1) AS dlina
FROM 
  [Участок теплопровода] UT,
  [Расчетная схема] PC
WHERE
   UT.kod1=PC.kod_rs
AND
   (UT.kod_p = '' OR UT.kod_p IS NULL)
)
GROUP BY
  diametr,
  kod_ist
