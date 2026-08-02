select
	tn.id,
	tn.name as 'Наименование теплового пункта',
	hpt.name as 'Тип теплового пункта',
	STUFF(
			( SELECT ', '+rt2.contractNumber AS n FROM heatPoint r2 
			LEFT JOIN realConsumers rt2 ON r2.id=rt2.heatPointID
			WHERE r2.id = tn.id 
			FOR XML PATH('')
			, TYPE
			).value('.', 'NVARCHAR(MAX)'), 1, 1, '') as 'Номера договоров с теплоснабжающей организацией',
	tn.countUserGV as 'Число пользователей ГВ, чел',	
	tn.countBusinessConsumers as 'Количество потребителей-предприятий, шт',
	tn.raschetnayaNagruzkaCO as 'Расчётная нагрузка на CO, гкал/час',
	tn.srednyayaNagruzkaGV as 'Средняя нагрузка на ГВ, гкал/час',
	tn.maxNagruzkaGV as 'Максимальная нагрузка на ГВ, гкал/час',
	tn.raschetnayaNagruzkaSV as 'Расчётная нагрузка на СВ',
	tn.area as 'Отапливаемая площадь, м2',
	tn.buildingVolume as 'Объем отапливаемого здания, м3',
	tn.basementVolume as 'Объем подвала отапливаемого здания, м3',
	tn.builtInVolume as 'Объем встроенных помещений отапливаемого здания, м3',
	count(tn.id) as 'Кол-во подключенных реальных потребителей'
from heatPoint tn 
join realConsumers rc on rc.heatPointID = tn.id
left join heatPointTypes hpt on hpt.id = tn.heatPointTypeID
group by 
	tn.id,
	tn.name,
	hpt.name,
	tn.countUserGV,
	tn.countBusinessConsumers,
	tn.raschetnayaNagruzkaCO,
	tn.srednyayaNagruzkaGV,
	tn.maxNagruzkaGV,
	tn.raschetnayaNagruzkaSV,
	tn.area,
	tn.buildingVolume,
	tn.basementVolume,
	tn.builtInVolume