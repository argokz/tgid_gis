SELECT 
  num,
  name,
  q,
  q_treb,
  g,
  g_ur,
  col,
  0 AS t1,
  0 AS t1_zad,
  0 AS t2,
  0 AS dlina,
  0 AS otm,
  kod_ist

FROM
(
SELECT 
TB.num AS num,
TB.name AS name,
sum(
IIF(TB.num=1, Qotz+Qotn,
  IIF(TB.num=2, dop18+dop19+dop20,
  IIF(TB.num=3, dop17,
  IIF(TB.num=4, dop12+dop13,
  IIF(TB.num=7, Qotz+Qotn+dop18+dop19+dop20+dop17+dop12+dop13,
  0)))))
) AS q,


sum(
IIF(TB.num=1, Qotz_treb+Qotn_treb,
  IIF(TB.num=2, Qgvop_treb+Qgvoo_treb+Qrez_treb,
  IIF(TB.num=3, Qgvz_treb,
  IIF(TB.num=4, Qvent_treb,
  IIF(TB.num=7, Qtreb,
  0)))))
) AS q_treb,


sum(
IIF(TB.num=1, PT.otoplz*UR.otoplz+PT.otopln*UR.otopln,
  IIF(TB.num=2, (PT.gvop+PT.gvoo)*UR.gvo,
  IIF(TB.num=3, PT.gvpr*UR.gvpr+PT.gvsm*UR.gvsm+PT.gvps*UR.gvps+PT.gvpw*UR.gvpw,
  IIF(TB.num=4, PT.ventil*UR.ventil,
  IIF(TB.num=7, PT.otoplz*UR.otoplz+PT.otopln*UR.otopln+(PT.gvop+PT.gvoo)*UR.gvo+PT.gvpr*UR.gvpr+PT.gvsm*UR.gvsm+PT.gvps*UR.gvps+PT.gvpw*UR.gvpw+PT.ventil*UR.ventil,
  0)))))
) AS g_ur,


sum(
IIF(TB.num=1, a4+a5,
  IIF(TB.num=2, a12+a13+a14,
  IIF(TB.num=3, a11,
  IIF(TB.num=4, a6,
  IIF(TB.num=7, a4+a5+a12+a13+a14+a11+a6,
  0)))))
) AS g,

sum(
IIF(TB.num=1, IIF(a4+a5 = 0 , 0, 1),
  IIF(TB.num=2, IIF(a12+a13+a14 = 0 , 0, 1),
  IIF(TB.num=3, IIF(a11 = 0 , 0, 1),
  IIF(TB.num=4, IIF(a6 = 0 , 0, 1),
  1
  ))))
) AS col,


TB.kod_ist AS kod_ist

FROM 
(
#include Потребители2.sql
) PT,
MAIN_OUT,
[Система теплоснабжения] CT3,
[Удельные расходы] UR,
(
          SELECT IST.kod_ist AS kod_ist, 1 AS num, 'Отопление' AS name  FROM [Источник тепла] IST
UNION ALL SELECT IST.kod_ist, 2, 'ГВС, открытая' FROM [Источник тепла] IST
UNION ALL SELECT IST.kod_ist, 3, 'ГВС, закрытая' FROM [Источник тепла] IST
UNION ALL SELECT IST.kod_ist, 4, 'Вентиляция' FROM [Источник тепла] IST
UNION ALL SELECT IST.kod_ist, 7, 'Итого' FROM [Источник тепла] IST


) TB
WHERE 
  PT.kod_ist=TB.kod_ist
  AND PT.sost<>'*'
  AND PT.kodur=UR.kodur


GROUP BY
  TB.num,
  TB.name,
  TB.kod_ist

ORDER BY
  TB.kod_ist
)
