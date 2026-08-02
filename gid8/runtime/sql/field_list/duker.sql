select obj_id as 'id', CONCAT('Дюкер № ', obj_id,',',' длина: ',ISNULL(CAST(length AS nvarchar(max)), 'не указано')) name
from getPts_duker_shield_bridge($fileID$,'pipe', '$FragmentIds$')
where description = 'Дюкер'