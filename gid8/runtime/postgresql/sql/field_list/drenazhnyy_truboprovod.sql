select id_obj as 'id', CONCAT('Дренажный трубопровод № ', id_obj,' ( ',ISNULL(externalLineStr,'признак трубопровада не указан'),' ), ','диаметр: ',ISNULL(CAST(diametr AS nvarchar(max)), 'не указано'))  name
from getPts($fileID$,'pipe', '$FragmentIds$')
where tblName = 'drenazhnyy_truboprovod'
