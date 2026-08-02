select
	tn.id,
	tn.name as 'name',
	hpt.name as 'type',
	STUFF(
			( SELECT ', '+rt2.contractNumber AS n FROM heatPoint r2 
			LEFT JOIN realConsumers rt2 ON r2.id=rt2.heatPointID
			WHERE r2.id = tn.id 
			FOR XML PATH('')
			, TYPE
			).value('.', 'NVARCHAR(MAX)'), 1, 1, '') as 'Номера договоров с теплоснабжающей организацией',
	tn.countUserGV as 'countUser',	
	tn.countBusinessConsumers as 'countCompany',
	tn.raschetnayaNagruzkaCO as 'r1',
	tn.srednyayaNagruzkaGV as 'r2',
	tn.maxNagruzkaGV as 'r3',
	tn.raschetnayaNagruzkaSV as 'r4',
	tn.area as 's1',
	tn.buildingVolume as 'v1',
	tn.basementVolume as 'v2',
	tn.builtInVolume as 'v3',
	count(tn.id) as 'Кол-во подключенных реальных потребителей'
from heatPoint tn 
join (select distinct hp.id as hpID  
		from myTableName tn
		join linesobj l on l.id = tn.id
		join heatPoint hp on l.shape.STDistance(hp.shape) < 0.1
		) thp on thp.hpID = tn.id
left join realConsumers rc on rc.heatPointID = tn.id
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

