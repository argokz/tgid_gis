
SELECT 
	t.rs_id,
	t.name as 'Наименование/Адрес контура',
	t.[Наименование участка] as 'Распределительная сеть (РС)',
	t.[Район эксплуатации],
	t.[Участок эксплуатации],
	t.[Начальник участка],
	ROUND(SUM(t.v),2) as 'Объём трубопроводов по участку, м3',
	ROUND(t.res,2) as 'Объём трубопроводов контура, м3',
	t.diameterCondit as 'Диаметр условный, мм',
	sum(t.length) as 'Протяженность, м'
from (
select
	rs.id as 'rs_id',
	obj.id,
	obj.name,
	rs.naimenovanie_uchastka_rs as 'Наименование участка',
    re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla as 'Район эксплуатации',
	ue_rs.nomer_uchastka as 'Участок эксплуатации',
	nu_rs.fio as 'Начальник участка',
	v = ROUND(PI() * POWER(hpss.diameterInternal,2)/4 * hpss.pipeSectLength /1000000,2),
	SUM(ROUND(PI() * POWER(hpss.diameterInternal,2)/4 * hpss.pipeSectLength /1000000,2)) OVER (partition by obj.id) as res,
	hpss.pipeSectLength as length,
	j.diameterCondit
from opres obj
		join opresDeployed d on d.directionID = obj.id
		JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
		join uchastok_rs rs ON rs.id = hpss.distSite
		left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
		left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
		left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
		join (
			select
				distinct
				d.id,
				d.diameterCondit
				from (select  
					rs.id,
					hpss.pipeSectLength,
					hpss.diameterCondit,
					max(hpss.pipeSectLength) over (partition by rs.id) as m_len
				from opres obj
				join opresDeployed d on d.directionID = obj.id
				JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
				join uchastok_rs rs ON rs.id = hpss.distSite 
				) d 
				where d.m_len = d.pipeSectLength
			) j on j.id = rs.id
)t 
WHERE t.id = $fileID$
group by 
t.rs_id,
t.name, 
t.[Наименование участка],
t.[Район эксплуатации],
t.[Участок эксплуатации],
t.[Начальник участка],
t.res,
t.diameterCondit