select 
	rc.id, 
	ISNULL(contractNumber, 'не заполнен ') as name
from realConsumers rc
where rc.heatPointID = $fileID$
