select distinct
id,
CONCAT('Шурф № ', id,' ( ',
ISNULL([Признак участка трубопровода], 'не указано' ),' ) '
, ', дата начала: ', ISNULL(CAST([Дата начала] as nvarchar(max)) ,'не указано' ) ) name
from getPts_shurf($fileID$,'pipe', '$FragmentIds$')