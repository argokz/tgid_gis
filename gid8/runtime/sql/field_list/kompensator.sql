select id_obj as id, CONCAT('Компенсатор № ', id_obj,' ( ',ISNULL(externalLineStr,'признак трубопровада не указан'),' ), ','конструкция: ',ISNULL(CAST(constructionTypes AS nvarchar(max)), 'не указана')) name
from getPts_kompensator_kolodtsy($fileID$,'pipe', '$FragmentIds$')
where tblName = 'kompensator'
