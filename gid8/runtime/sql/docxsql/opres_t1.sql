
SELECT 
	t.ms_id,
	t.name as 'Наименование/Адрес контура',
	t.[Наименование участка] as 'Магистральная сеть(МС)',
	t.[Район эксплуатации],
	t.[Участок эксплуатации],
	t.[Начальник участка],
	ROUND(SUM(t.v),2) as 'Объём трубопроводов по участку, м3',
	ROUND(t.res,2) as 'Объём трубопроводов контура, м3',
	t.diameterCondit as 'Диаметр условный, мм',
	sum(t.length) as 'Протяженность, м'
from (
select
	obj.id,
	ms.id as 'ms_id',
	obj.name,
	magistrali.naimenovanie_magistrali as 'Магистраль',
	ms.opisanie_uchastka_ms as 'Наименование участка',
    re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla as 'Район эксплуатации',
	ue_ms.nomer_uchastka as 'Участок эксплуатации',
	nu_ms.fio as 'Начальник участка',
	v = ROUND(PI() * POWER(hpss.diameterInternal,2)/4 * hpss.pipeSectLength /1000000,2),
	SUM(ROUND(PI() * POWER(hpss.diameterInternal,2)/4 * hpss.pipeSectLength /1000000,2)) OVER (partition by obj.id) as res,
	hpss.pipeSectLength as length,
	j.diameterCondit
from opres obj
		join opresDeployed d on d.directionID = obj.id
		JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
		join uchastok_ms ms ON ms.id = hpss.magistralSite
		left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
		left join magistrali on magistrali.id = hpss.magistral
		left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
		left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
		join (
			select
				distinct
				d.id,
				d.diameterCondit
				from (select  
					ms.id,
					hpss.pipeSectLength,
					hpss.diameterCondit,
					max(hpss.pipeSectLength) over (partition by ms.id) as m_len
				from opres obj
				join opresDeployed d on d.directionID = obj.id
				JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
				join uchastok_ms ms ON ms.id = hpss.magistralSite
				) d 
				where d.m_len = d.pipeSectLength
			) j on j.id = ms.id
)t 

WHERE t.id = $fileID$

group by 
t.ms_id, 
t.name, 
t.[Наименование участка],
t.[Район эксплуатации],
t.[Участок эксплуатации],
t.[Начальник участка],
t.res,
t.diameterCondit
