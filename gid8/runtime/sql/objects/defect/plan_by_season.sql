select 
	obj.id, 
	obj.otchet_po_defektu as 'Контур ремонта',
	obj.data_nachala_plan as 'Дата начала ремонта плановая',
	obj.data_okonchaniya_plan as 'Дата окончания ремонта плановая',
	rt.name as 'Вид ремонта',
	ts.name as 'Тепловая сеть',
	obj.opisanie_rabot as 'Описание работ',
	obj.vydelennye_sredstva_plan as 'Объем выделенных средств, тыс. тг с НДС',
	obj.remontnyj_personal_plan as 'Ремонтный персонал, планиру-емый, чел.',
	nu.fio as 'Ответственный за ремонт',
	sb.name as 'Подразделение производившее ремонт'
from remont2 obj
left join remontTypes rt on rt.id = obj.remontTypeID
left join (SELECT TOP 10 1 AS id, 'Магистральная сеть' AS name UNION SELECT 2, 'Внутриквартальная сеть') ts on ts.id = obj.teplovaya_setID
left join nachalniki_uchastkov nu on nu.id = obj.responsibleID
left join subdivisions sb on sb.id = obj.subdivisionID
where obj.plan_flag = 1
AND
        ((obj.data_nachala_plan $season_condition$) OR
        (obj.data_nachala_remonta $season_condition$)) AND  obj.plan_flag = 1




