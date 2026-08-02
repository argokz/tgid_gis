select distinct
id_obj as 'id',
CONCAT('Вырезка № ', id_obj,' ( ',
ISNULL(externalSignLineStr, 'не указано' ),' ) '
, ', дата вырезки: ', ISNULL(CAST(data_prov as nvarchar(max)) ,'не указано' ) ) name
from getPts_cut_out($fileID$,'pipe', '$FragmentIds$')