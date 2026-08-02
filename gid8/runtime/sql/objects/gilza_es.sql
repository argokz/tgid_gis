SELECT 
	gl.id, 
	le.mestopolozhenie as 'Местоположение/Описание ЛЭП',
	gl.tip__marka__harakteristika_ as 'Тип/Описание',
	gl.dlina_gilzy as 'Длина гильзы, м',
	gl.shema_gilzy_v_razreze as 'Схема гильзы в разрезе', 
	gl.data_ustanovki as 'Дата установки',
	gl.primechanie as 'Примечание'
FROM gilza_es gl
left join liniya_elektroperedach le on le.shape.STDistance(gl.shape) < 0.1
