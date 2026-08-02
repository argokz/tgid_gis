SELECT
    n.id
--      , n.city
--      , n.mkr
--      , n.street
--      , n.house
--      , n.house2
      , n.addr AS "Наименование КСК/Адрес/Лицевой счёт"
      , n.name AS "Тип строения"
      , n.rayon AS "Эксплуатационный район"
      , n.adm_rayon AS "Административный район"
      , n.uchastok AS "Эксплуатационный участок"
      , n.ist AS "Тепловой источник"
      , n.tg AS "Температурный график объекта"
      , n.otop AS "Нагрузка отопление, ккал/ч"
      , n.gvs AS "Нагрузка ГВС, ккал/ч"
--      , n.vent
--      , n.par
--      , n.dogovor
--      , n.numb
--      , n.owner


FROM nagruzki n 
LEFT JOIN zdaniya_2 z 
ON 
    (n.mkr=z.mkr2 OR n.mkr IS NULL AND z.mkr2 IS NULL)
    AND
    (n.street=z.street2 OR n.street IS NULL AND z.street2 IS NULL) 
    AND
        LOWER(REPLACE(n.house, ' ', '')) = LOWER(REPLACE(z.house2, ' ', ''))
        
WHERE z.id IS NULL AND n.name = 'МЖД'

