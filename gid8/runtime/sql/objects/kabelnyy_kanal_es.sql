SELECT 
	kk.id, 
	le.mestopolozhenie as 'Местоположение/Описание ЛЭП',
	kk.tip__marka__harakteristika_ as 'Тип/Описание',
	kk.dlina_kanala as 'Длина канала, м',
	kk.shema_kanala_v_razreze as 'Схема канала в разрезе', 
	kk.data_ustanovki as 'Дата установки',
	kk.primechanie as 'Примечание'
FROM kabelnyy_kanal_es kk
left join liniya_elektroperedach le on le.shape.STDistance(kk.shape) < 0.1
