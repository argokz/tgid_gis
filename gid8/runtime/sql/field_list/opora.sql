select id_obj as 'id', CONCAT('Опора № ', id_obj,' ( ',ISNULL(externalLineStr,'признак трубопровада не указан'),' ), ','назначение: ',ISNULL(CAST(oporaType AS nvarchar(max)), 'не указано')) name
from getPts_opora($fileID$,'pipe', '$FragmentIds$')
