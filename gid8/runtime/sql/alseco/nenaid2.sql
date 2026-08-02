SELECT DISTINCT
    n.id
--      , n.city
--      , n.mkr
--      , n.street
--      , n.house
--      , n.house2
      , n.owner AS 'Наименование контрагента'
      , n.dogovor AS '№ договора'
      , n.numb AS '№ объекта'
      , n.name AS 'Наименование объекта'
      , n.addr AS 'Адрес объекта'
      , n.adm_rayon AS 'Административный район по объекту'
      , n.rayon AS 'Эксплуатационный район по объекту'
      , n.uchastok AS 'Тепловой участок по объекту'
      , n.gvs AS 'Нагрузка ГВС, ккал/ч'
      , n.otop AS 'Нагрузка Отопление, ккал/ч'
      , n.vent AS 'Нагрузка Вентиляция, ккал/ч'
      , n.par AS 'Нагрузка Пар, ккал/ч'
      , n.ist AS 'Тепловой источник'
      , n.tg AS 'Температурный график объекта'

FROM nagruzki n 
LEFT JOIN zdaniya_2 z 
ON 
    (n.mkr=z.mkr2 OR n.mkr IS NULL AND z.mkr2 IS NULL)
    AND
    (n.street=z.street2 OR n.street IS NULL AND z.street2 IS NULL) 
    AND
        LOWER(REPLACE(n.house, ' ', '')) = LOWER(REPLACE(z.house2, ' ', ''))

        
WHERE z.id IS NULL AND n.name <> 'МЖД'
