SELECT
    n.id
--      , n.city
      , n.mkr AS 'Микрорайон'
      , n.street AS 'Улица'
      , n.house AS 'Дом'
--      , n.house2
      , n.addr AS 'Наименование КСК/Адрес/Лицевой счёт'
      , n.name AS 'Тип строения'
      , n.rayon AS 'Эксплуатационный район'
      , n.adm_rayon AS 'Административный район'
      , n.uchastok AS 'Эксплуатационный участок'
      , n.ist AS 'Тепловой источник'
      , n.tg AS 'Температурный график объекта'
      , n.otop AS 'Нагрузка отопление, ккал/ч'
      , n.gvs AS 'Нагрузка ГВС, ккал/ч'
--      , n.vent
--      , n.par
--      , n.dogovor
--      , n.numb
--      , n.owner


FROM nagruzki n 

