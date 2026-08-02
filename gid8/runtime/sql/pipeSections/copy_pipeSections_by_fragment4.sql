UPDATE hps
	set pipeSectionID = t.pss_new_id
from heatPipeSections hps
join (	
	select hps.id, t.pss_new_id from Temp_Mapping t
	join heatPipeSections hps on hps.id = t.hps_id
) t on t.id = hps.id

