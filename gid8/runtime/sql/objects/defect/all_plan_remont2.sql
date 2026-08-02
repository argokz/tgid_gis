SELECT DISTINCT
r.id,
r.otchet_po_defektu AS 'Наименование/Адрес',
st.name as 'Состояние',
r.data_nachala_plan as 'Дата начала ремонта плановая',
r.data_okonchaniya_plan as 'Дата окончания ремонта плановая',
CASE
WHEN r.utverdit = 0 THEN 'Не утверждено'
ELSE 'Утверждено'
END AS 'Утверждение плана',

r.data_utverzhdeniya_plana AS 'Дата утверждения плана',
rt.name as 'Вид ремонта',
nach.fio as 'Ответственный за ремонт'
FROM remont2 r
LEFT JOIN remontTypes rt ON rt.id=r.remontTypeID
LEFT JOIN nachalniki_uchastkov nach ON nach.id=r.responsibleID
left join stateRemont2 st on st.id = r.stateID
JOIN remont2Deployed depl ON depl.directionID=r.id
JOIN linesobj l ON l.id=depl.lineID
JOIN heatPipeSections hpss ON hpss.lineID=l.id 
WHERE r.stateID = 1 OR (r.stateID = 2 AND r.data_nachala_remonta IS NULL)
ORDER BY r.id DESC