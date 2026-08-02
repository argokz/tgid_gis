

select 

	obj.name as 'Наименование/Адрес контура',
	IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as 'Наименование тепловой сети МС/РС',
	IIF (ms.id is not NULL, re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Район эксплуатации',
	manometr.node_name as 'Камера',
	manometr.es_name as 'Признак трубопровода',
	manometr.kol as 'Количество',
	fio_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov as 'ФИО ответственного за установку манометров и расходомеров',
	dolzhnost_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov.znachenie as 'Должность ответственного за установку манометров и расходомеров',
	podrazdelenie_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov.name as 'Подразделение  ответственного за установку манометров и расходомеров'

from opres obj
join(
	select 
	distinct
	t.objID,
	t.node_name,
	t.es_name,
	t.nodeID,
	IIF(t.ms>t.rs, t.ms, t.rs) as ms_rs_id,
	type_max = IIF(t.ms>t.rs, 'ms', 'rs'),
	count(t.nodeID) over (partition by t.objID, t.nodeID) as kol
from(
	select
		distinct
		obj.objID,
		obj.nodeID,
		es.name as es_name,
		hpss.distSite,
		hpss.magistralSite,
		IIF(hpss.distSite is not null, MIN(hpss.distSite) OVER(PARTITION BY obj.nodeID ), 0) as rs,
		IIF(hpss.magistralSite is not null, MIN(hpss.magistralSite) OVER (PARTITION BY obj.nodeID), 0) as ms,
		IIF (n.nodeName is NULL or n.nodeName = '' or n.nodeName = ' ',n.externalNodeName, n.nodeName) as node_name
		--count(obj.nodeID) over (partition by obj.objID, obj.nodeID) as kol
	from list_opres_node2 obj
	join nodes n on n.id = obj.nodeID
	left join externalSigns es on es.id = n.externalSignID
	join opresDeployed od on od.directionID = obj.objID
--	join linesobj l on l.id = od.lineID and  l.shape.STDistance(n.shape) < 0.1
    join linesobj l ON l.nodeID1 = n.id  OR l.nodeID2 = n.id  
	JOIN heatPipeSections hpss ON hpss.lineID=l.id
	where obj.objID = $fileID$
	)t
) manometr on manometr.objID = obj.id

left join uchastok_ms ms ON ms.id = manometr.ms_rs_id and manometr.type_max = 'ms'
left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka and manometr.type_max = 'ms'
left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii and manometr.type_max = 'ms'

left join uchastok_rs rs ON rs.id = manometr.ms_rs_id and manometr.type_max = 'rs'
left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka and manometr.type_max = 'rs'
left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii and manometr.type_max = 'rs'

LEFT JOIN dolzhnosti dolzhnost_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov on dolzhnost_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov.id = obj.dolzhnost_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerovID 
LEFT JOIN subdivisions podrazdelenie_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov on podrazdelenie_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov.id = obj.podrazdelenie_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerovID


where obj.id = $fileID$
