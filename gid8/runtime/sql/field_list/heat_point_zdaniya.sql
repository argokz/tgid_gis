select distinct
	zd.id,
	CONCAT (
		ISNULL( st.name, 'улица не заполнена'), ' , ',
		ISNULL(zd.houseNumber, '-'), ' , ',
		ISNULL(ds.name,'-'), ' , ',
		ISNULL(tw.name,'-')
	) AS name
from realConsumers as rc
join heatPoint hp ON hp.id = rc.heatPointID
join nodes n on n.id = rc.nodeID
join buildings zd ON zd.shape.STDistance(n.shape) < 0.1
left join streets st ON st.id = zd.streetID
left join districts ds ON ds.id = zd.districtID
left join towns tw ON tw.id = zd.cityID
where hp.id = $fileID$
