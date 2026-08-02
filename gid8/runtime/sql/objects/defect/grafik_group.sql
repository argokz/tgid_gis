select 
	t.id,
	t.[Контур ремонта],
	t.[Дата начала ремонта плановая],
	t.[Дата окончания ремонта плановая],
	t.[Вид ремонта],
	t.[Тепловая сеть],
	t.[Проложено труб, м.п, план, м],
	t.[Проложено труб, м.п, факт ,м ],
	'Проложено труб, %' = CASE 
							when t.[Проложено труб, м.п, план, м] != 0 then t.[Проложено труб, м.п, факт ,м ]/t.[Проложено труб, м.п, план, м]*100
							else 0
							end,
	t.[Восстановление тепловой изоляции, м2, план],
	t.[Восстановление тепловой изоляции, м2, факт],
	'Восстановление тепловой изоляции, %' = CASE 
												when t.[Восстановление тепловой изоляции, м2, план] != 0 then t.[Восстановление тепловой изоляции, м2, факт]/t.[Восстановление тепловой изоляции, м2, план]*100
												else 0
											end,
	t.[Восстановление канальной прокладки, план, м],
	t.[Восстановление канальной прокладки, факт, м],
	'Восстановление канальной прокладки, %' = CASE
												when t.[Восстановление канальной прокладки, план, м] != 0 then t.[Восстановление канальной прокладки, факт, м]/t.[Восстановление канальной прокладки, план, м]*100
												else 0
											end,
	t.[Асфальтирование, план, м2],
	t.[Асфальтирование, факт, м2],
	'Асфальтирование, %' = CASE 
									when t.[Асфальтирование, план, м2] != 0 then t.[Асфальтирование, факт, м2]/t.[Асфальтирование, план, м2] * 100
									else 0
								  end,
	t.[Ответственный за ремонт],
	t.[Подразделение производившее ремонт]
from(
	select
		obj.id, 
		obj.otchet_po_defektu as 'Контур ремонта',
		obj.data_nachala_plan as 'Дата начала ремонта плановая',
		obj.data_okonchaniya_plan as 'Дата окончания ремонта плановая',
		rt.name as 'Вид ремонта',
		ts.name as 'Тепловая сеть',
		obj.len_tube_plan as 'Проложено труб, м.п, план, м',
		sum(faktory_riska_truboprovoda.len_tube) as 'Проложено труб, м.п, факт ,м ',
		obj.len_izol_plan as 'Восстановление тепловой изоляции, м2, план',
		sum(faktory_riska_truboprovoda.len_izol) as 'Восстановление тепловой изоляции, м2, факт',
		obj.len_channel_plan as 'Восстановление канальной прокладки, план, м',
		sum(faktory_riska_truboprovoda.len_channel) as 'Восстановление канальной прокладки, факт, м',
		obj.asfaltirovanie_plan as 'Асфальтирование, план, м2',
		sum(faktory_riska_truboprovoda.asfaltirovanie) as 'Асфальтирование, факт, м2',
		nu.fio as 'Ответственный за ремонт',
		sb.name as 'Подразделение производившее ремонт'
	from remont2 obj
	left join remont2Deployed d on d.directionID = obj.id
	left JOIN heatPipeSections hpss ON hpss.lineID=d.lineID $and_condition2$
	left join linesobj l on l.id = d.lineID
	JOIN pipeSections pss ON pss.id=hpss.pipeSectionID
	left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = pss.id and faktory_riska_truboprovoda.objID = obj.id and faktory_riska_truboprovoda.obj_type_faktory_riskaID = 3
	left join remontTypes rt on rt.id = obj.remontTypeID
	left join (SELECT TOP 10 1 AS id, 'Магистральная сеть' AS name UNION SELECT 2, 'Внутриквартальная сеть') ts on ts.id = obj.teplovaya_setID
	left join nachalniki_uchastkov nu on nu.id = obj.responsibleID
	left join subdivisions sb on sb.id = obj.subdivisionID

    WHERE 
        ((obj.data_nachala_plan $season_condition$) OR
        (obj.data_nachala_remonta $season_condition$)) 
        AND  obj.plan_flag = 1

	group by 
	obj.id, 
	obj.otchet_po_defektu, 
	obj.data_nachala_plan, 
	obj.data_okonchaniya_plan,
	rt.name,
	ts.name,
	obj.len_tube_plan,
	obj.len_izol_plan,
	obj.len_channel_plan,
	obj.asfaltirovanie_plan,
	nu.fio,
	sb.name
) t
