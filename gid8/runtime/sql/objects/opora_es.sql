SELECT 
	op.id, 
	le.mestopolozhenie as 'Местоположение/Описание ЛЭП',
	op.tip__marka__harakteristika_ as 'Тип/Описание', 
	op.primechanie as 'Примечание'
FROM opora_es op
left join liniya_elektroperedach le on le.shape.STDistance(op.shape) < 0.1
