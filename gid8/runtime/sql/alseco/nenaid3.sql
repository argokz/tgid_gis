SELECT DISTINCT
    z.id
--      , n.city
      , z.mkr2 AS 'Микрорайон'
      , z.street2 AS 'Улица'
--      , n.house AS ''
      , z.house2 AS 'Дом'
      , z.gvs AS 'Нагрузка ГВС, ккал/ч'
      , z.otop AS 'Нагрузка Отопление, ккал/ч'
      , z.vent AS 'Нагрузка Вентиляция, ккал/ч'
      , z.par AS 'Нагрузка Пар, ккал/ч'

FROM zdaniya_2 z 
        
WHERE z.potrebitel IS NULL AND z.otop IS NOT NULL
ORDER BY z.mkr2, z.street2, z.house2
