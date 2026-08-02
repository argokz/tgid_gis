select 
    obj.id, 
    obj.otchet_po_defektu as 'Контур ремонта',
    obj.data_nachala_plan as 'Дата начала ремонта плановая',
--    obj.data_okonchaniya_plan as 'Дата окончания ремонта плановая',
    rt.name as 'Вид ремонта',
    ts.name as 'Тепловая сеть',
    opisanie_rabot AS 'Описание работ',
    N'янв' = CASE WHEN CAST(CONCAT(DAY(obj.data_nachala_plan),'.03.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end,
    N'февр' = case when CAST(CONCAT((CASE WHEN DAY(obj.data_nachala_plan) > 28 then 28 else DAY(obj.data_nachala_plan) end) ,'.02.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end,
    N'март' = case when CAST(CONCAT(DAY(obj.data_nachala_plan),'.03.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end,
    N'апр' = case when CAST(CONCAT(DAY(obj.data_nachala_plan),'.04.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end,
    N'май' = case when CAST(CONCAT(DAY(obj.data_nachala_plan),'.05.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end,
    N'июнь' = case when CAST(CONCAT(DAY(obj.data_nachala_plan),'.06.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end,
    N'июль' = case when CAST(CONCAT(DAY(obj.data_nachala_plan),'.07.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end,
    N'авг' = case when CAST(CONCAT(DAY(obj.data_nachala_plan),'.08.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end,
    N'сент' =case when CAST(CONCAT(DAY(obj.data_nachala_plan),'.09.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end,
    N'окт' =case when CAST(CONCAT(DAY(obj.data_nachala_plan),'.10.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end,
    N'нояб' = case when CAST(CONCAT(DAY(obj.data_nachala_plan),'.11.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end,
    N'дек' = case when CAST(CONCAT(DAY(obj.data_nachala_plan),'.12.',YEAR(obj.data_nachala_plan)) AS date) BETWEEN obj.data_nachala_plan and obj.data_okonchaniya_plan then 'x' else ' ' end
from remont2 obj
left join remontTypes rt on rt.id = obj.remontTypeID
left join (SELECT TOP 10 1 AS id, 'Магистральная сеть' AS name UNION SELECT 2, 'Внутриквартальная сеть') ts on ts.id = obj.teplovaya_setID
where obj.plan_flag = 1 and obj.data_nachala_plan is not null and obj.data_okonchaniya_plan is not null
 AND ((obj.data_nachala_plan $season_condition$) OR
  (obj.data_nachala_remonta $season_condition$)) 
