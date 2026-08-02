UPDATE d
set d.len_tube_cur = t.len_tube_fakt,
	d.len_channel_cur = t.len_channel_fakt,
	d.len_izol_cur = t.len_izol_fakt,
	d.asfaltirovanie = t.asfaltirovanie_fakt
from remont2 d
join (
	select
		obj.id, 
		sum(faktory_riska_truboprovoda.len_tube) as len_tube_fakt,
		sum(faktory_riska_truboprovoda.len_izol) as len_izol_fakt,
		sum(faktory_riska_truboprovoda.len_channel) as len_channel_fakt,
		sum(faktory_riska_truboprovoda.asfaltirovanie) as asfaltirovanie_fakt
	from remont2 obj
	left join remont2Deployed d on d.directionID = obj.id
	left JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
	left join linesobj l on l.id = d.lineID
	JOIN pipeSections pss ON pss.id=hpss.pipeSectionID
	left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = pss.id and faktory_riska_truboprovoda.objID = obj.id and faktory_riska_truboprovoda.obj_type_faktory_riskaID = 3
	group by 
	obj.id, 
	obj.len_tube_plan,
	obj.len_izol_plan,
	obj.len_channel_plan,
	obj.asfaltirovanie_plan
	) t on t.id = d.id
where d.id = $ID$