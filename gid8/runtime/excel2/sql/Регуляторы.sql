SELECT
  kod_ist,
  uzel1, kod1, pr1, uzel2, kod2, pr2,
  IIF(RD.pr_raboti IS NULL, '', RD.pr_raboti) AS pr_raboti,
  g_tep_poteri,
  kod_owner
FROM
  [Регулятор давления] RD,
  [Расчетная схема] PC
WHERE 
  RD.kod1=PC.kod_rs

UNION

SELECT
  kod_ist,
  uzel1, kod1, pr1, uzel2, kod2, pr2,
  IIF(RD.pr_raboti IS NULL, '', RD.pr_raboti) AS pr_raboti,
  g_tep_poteri,
  kod_owner
FROM
  [Регулятор расхода] RD,
  [Расчетная схема] PC
WHERE 
  RD.kod1=PC.kod_rs

UNION

SELECT
  kod_ist,
  uzel1, kod1, pr1, uzel2, kod2, pr2,
  IIF(RD.pr_raboti IS NULL, '', RD.pr_raboti) AS pr_raboti,
  g_tep_poteri,
  kod_owner
FROM
  [Регулятор перепада давления] RD,
  [Расчетная схема] PC
WHERE 
  RD.kod1=PC.kod_rs


UNION

SELECT
  kod_ist,
  '', '', '', '', '', '',
  'Летний',
  0,
  0
FROM
  [Расчетная схема] PC

UNION

SELECT
  kod_ist,
  '', '', '', '', '', '',
  'Зимний',
  0,
  0
FROM
  [Расчетная схема] PC

