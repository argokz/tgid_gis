select 
distinct 
CONCAT('План на ', format(d.data_utverzhdeniya_plana_shurfovok,'dd/MM/yy')) as name, 
d.data_utverzhdeniya_plana_shurfovok 
from shurfy d
where (d.data_nachala_plan $season_condition$)  AND d.naznachenie_vskrID = 1 
order by d.data_utverzhdeniya_plana_shurfovok desc
