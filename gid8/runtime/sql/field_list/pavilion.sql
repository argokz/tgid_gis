select objID as 'id', CONCAT('Павильон № ', objID,',',' площадь: ',ISNULL(CAST(s AS nvarchar(max)), 'не указано'))  name
from getPts_pavilion($fileID$,'pipe', '$FragmentIds$')