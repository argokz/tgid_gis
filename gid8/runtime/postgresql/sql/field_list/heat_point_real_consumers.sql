select
	rc.id,
	CONCAT (
		ISNULL(st.name, 'улица не заполнена'), ', ',
		ISNULL(rc.houseNumber, ' номер дома не заполнен'),', ',
		ISNULL(rc.name, ' наименование не запонено '),', ',
		ISNULL(n.externalNodeName ,' наименование узла не заполнено')
	) AS name
from realConsumers rc
left join nodes n on n.id = rc.nodeID
left join streets st ON st.id = rc.streetID
where rc.heatPointID = $fileID$
