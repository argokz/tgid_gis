select distinct objID as 'id', CONCAT('Тепловая камера № ', objID,', размеры (Д x Ш x В): '
,ISNULL(CAST(lenKamera AS nvarchar(max)), 'не указано'),' x '
,ISNULL(CAST(width AS nvarchar(max)), 'не указано'),' x '
,ISNULL(CAST(height AS nvarchar(max)), 'не указано') )  name
from getPts_tkamera($fileID$,'pipe', '$FragmentIds$')