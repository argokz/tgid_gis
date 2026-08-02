select distinct 
	id_obj as 'id', 
	CONCAT('Ремонт № ', id_obj,' ( ', 
	ISNULL(CAST(data_zaversheniya_remonta as nvarchar(max)) , '-'  ),' ) '  
	, ', Вид работ: ', ISNULL(remontType,'не указано' ) )  name
from getPts_remont($fileID$,'pipe', '$FragmentIds$')