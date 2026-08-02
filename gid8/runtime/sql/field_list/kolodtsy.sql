select id_obj as id, CONCAT('Колодец № ', id_obj,' ( ',ISNULL(externalLineStr,'признак трубопровада не указан'),' ), ','назначение: ',ISNULL(CAST(purposeTypes AS nvarchar(max)), 'не указано'))  name
from getPts_kompensator_kolodtsy($fileID$,'pipe', '$FragmentIds$')
where tblName = 'kolodtsy'