select distinct 
    id_obj as 'id', 
    CONCAT('Дефект № ', id_obj,' ( ', 
    ISNULL(CAST([Дата обнаружения нарушения] as nvarchar(max)) , '-'  ),' ) '  
    , ', Категория нарушений: ', ISNULL([Категория нарушения],'не указано' ) )  name
from  getPts_defect($fileID$,'pipe', '$FragmentIds$')