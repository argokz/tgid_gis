select distinct 
	id_obj as 'id', 
	CONCAT('Освидетельствование № ', id_obj,' ( ', 
	ISNULL(externalSignLineStr, 'не указано'  ),' ) '  
	, ', дата : ', ISNULL(CAST(data_prov as nvarchar(max)) ,'не указано' ) )  name
from getPts_certification($fileID$,'pipe', '$FragmentIds$')