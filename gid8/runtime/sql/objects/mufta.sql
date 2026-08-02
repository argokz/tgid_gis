SELECT 
	mt.id, 
	le.mestopolozhenie as 'Местоположение/Описание ЛЭП',
	mt.tip__marka__harakteristika_ as 'Тип/Описание',
	mt.rasstoyanie_do_priemnika__m as 'Расстояние до приемника, м',	
	mt.data_ustanovki as 'Дата установки',
	resp.name as 'ФИО ответственного (исполнителя)', 
	dl.znachenie as 'Должность ответственного (исполнителя)',
	mt.primechanie as 'Примечание'
FROM mufta mt
left join liniya_elektroperedach le on le.shape.STDistance(mt.shape) < 0.1
left join responsibles_es resp on resp.id  = mt.responsibleID 
left join dolzhnosti_es dl on dl.id = resp.statusID
