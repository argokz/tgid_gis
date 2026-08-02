
select 
	case 
		when t.count_line = count_complete then 1
		else 0
	end as 'result'
from (
	SELECT
		distinct
		count(id) over (partition by obj_id) as count_line,
		sum(statusOsmotr) over (partition by obj_id) as count_complete
	FROM
		(
	SELECT
			DISTINCT
			ps1.id,
			IIF(faktory_riska_truboprovoda.id IS NULL, 0, 1) as statusOsmotr,
			obj.id as obj_id
	, srt.orderID
		FROM remont2 obj
		JOIN remont2Deployed d ON d.directionID=obj.id
			JOIN heatPipeSections hps ON hps.lineID=d.lineID
			JOIN pipeSections ps1 ON ps1.id=hps.pipeSectionID
			--JOIN linesobj l ON l.id=d.lineID
			left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = ps1.id and faktory_riska_truboprovoda.objID = $ID$ and faktory_riska_truboprovoda.obj_type_faktory_riskaID = 3

			LEFT JOIN sortLinesForUchastok srt ON ps1.id = srt.pipeSectionID

			LEFT JOIN tubingTypes tt ON ps1.tubingTypeID = tt.id

			JOIN nodes n1 ON n1.id=ps1.nodeID1
			JOIN nodes n2 ON n2.id=ps1.nodeID2
			LEFT JOIN nodeTypes nt1 ON nt1.id=n1.nodeTypeID
			LEFT JOIN nodeTypes nt2 ON nt2.id=n2.nodeTypeID
	WHERE obj.id=$ID$

	) _QQ
)t




