SELECT 
  ist.naimenovanie AS [Наименование],
  _nagr.nagr AS [Наагрузка],
  prirost AS [Прирост]

      ,[god] AS [Год]
      ,[raspolagaemaya_moschnost_ov] AS [Располагаемая мощность ОВ]
      ,[raspolagaemaya_moschnost_gvs_srednyaya] AS [Располагаемая мощность ГВС]
      ,[raspolagaemaya_moschnost_summarnaya] AS [Располагаемая мощность суммарная]
      ,[normativnye_teplovye_poteri] AS [Нормативные тепловые потери]
      ,[prisoedinennaya_moschnost_otoplenie_1] AS [Присоединенная мощность отопление]
      ,[prisoedinennaya_moschnost_ventilyatsiya_1] AS [Присоединенная мощность вентиляция]
      ,[prisoedinennaya_moschnost_gvs_maksimalnaya_1] AS [Присоединенная мощность ГВС максимальная]
      ,[prisoedinennaya_moschnost_par_1] AS [Присоединенная мощность пар]
      ,[prisoedinennaya_moschnost_otoplenie] AS [Присоединенная мощность отопление]
      ,[prisoedinennaya_moschnost_ventilyatsiya] AS [Присоединенная мощность вентиляция]
      ,[prisoedinennaya_moschnost_gvs_maksimalnaya] AS [Присоединенная мощность ГВС максимальная]
      ,[prisoedinennaya_moschnost_par] AS [Присоединенная мощность пар]



 FROM 
(
    SELECT 
      istochnik_tepla,
      sum(nagr) AS nagr

    FROM (
       SELECT 
       istochnik_tepla,
       nagruzka_gvs+nagruzka_otoplenie AS nagr
       FROM zhile

    UNION
       SELECT 
       istochnik_tepla,
       nagruzka__otoplenie_+nagruzka__par_+nagruzka__ventilyatsiya_+nagruzka__gvs_ AS nagr

        FROM organizatsii
    ) _T
    GROUP BY istochnik_tepla
) _nagr 

RIGHT JOIN istochniki_tepla ist ON _nagr.istochnik_tepla=ist.naimenovanie OR (_nagr.istochnik_tepla IS NULL AND ist.naimenovanie='АО "АлЭС" (ТЭЦ-1, ТЭЦ-2, ЗТК)')

LEFT JOIN 
(
    SELECT 
    istochnik,
    sum(prirost_nagruzki) AS prirost
    FROM tehnicheskie_usloviya
    GROUP BY istochnik
) tu ON tu.istochnik = ist.naimenovanie

LEFT JOIN prisoedinennaya_nagruzka_istochnikov pr ON pr.id2=ist.id

WHERE god=2018

ORDER BY pr.id2