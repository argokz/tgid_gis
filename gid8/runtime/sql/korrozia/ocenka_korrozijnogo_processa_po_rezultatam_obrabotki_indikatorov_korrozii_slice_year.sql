SELECT DISTINCT
    t.id,
    row_number() over(ORDER BY t.magistral_name desc ) as num,
    ISNULL(t.magistral_name, '-') as 'magistral_name',
    IIF( PATINDEX('',ISNULL(t.mesto_ustanovki,'')) > 0, '-', t.mesto_ustanovki ) as 'mesto_ustanovki',
    IIF(t.nomer_indikatora_korrozii IS NULL OR t.nomer_indikatora_korrozii = ' ', '-', t.nomer_indikatora_korrozii) as 'nomer_indikatora_korrozii',
    ISNULL(t.externalSignLine,'-') as 'externalSignLine',

    $zzz$
    /*
    COALESCE(MAX(CASE WHEN t.year_number = '2013' THEN  IIF( t.stateIndID = 2, t.stateIndName, convert(varchar, t.skk)) END), '-') as '2013',
    COALESCE(MAX(CASE WHEN t.year_number = '2014' THEN IIF( t.stateIndID = 2, t.stateIndName, convert(varchar, t.skk)) END), '-') as '2014',
    COALESCE(MAX(CASE WHEN t.year_number = '2015' THEN IIF( t.stateIndID = 2, t.stateIndName, convert(varchar, t.skk)) END), '-') as '2015',
    COALESCE(MAX(CASE WHEN t.year_number = '2016' THEN IIF( t.stateIndID = 2, t.stateIndName, convert(varchar, t.skk)) END), '-') as '2016',
    COALESCE(MAX(CASE WHEN t.year_number = '2017' THEN IIF( t.stateIndID = 2, t.stateIndName, convert(varchar, t.skk)) END), '-') as '2017',	   
    */
    sum(t.skk) as 'total',
    ISNULL(t.ex_code_1, '-') as ex_code_1,
    IIF( PATINDEX('',ISNULL(t.beginNode,'')) > 0, '-', t.beginNode ) as 'beginNode',    
    ISNULL(t.ex_code_2, '-') as ex_code_2,
	IIF( PATINDEX('',ISNULL(t.endNode,'')) > 0, '-', t.endNode ) as 'endNode'
FROM 	(
    SELECT
    distinct
    obj.id,
    IIF(magistral_ms.naimenovanie_magistrali is not NULL, magistral_ms.naimenovanie_magistrali, magistral_rs.naimenovanie_magistrali) as magistral_name,        
    IIF(obj.mesto_ustanovki IS NULL OR obj.mesto_ustanovki = ' ', '-', obj.mesto_ustanovki) as mesto_ustanovki,
    IIF(obj.nomer_indikatora_korrozii IS NULL OR obj.nomer_indikatora_korrozii = ' ', '-', obj.nomer_indikatora_korrozii) as nomer_indikatora_korrozii,
    el.name as 'externalSignLine',
    ec1.name as 'ex_code_1',
    IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'beginNode',
    ec2.name as 'ex_code_2',
    IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'endNode',	
    obj_godam.stateIndID,
    stateInds.name as stateIndName,
    obj_godam.srednyaya_skorost_korrozii__mm_god as skk,		
    CONVERT(nvarchar(5), year(obj_godam.data_ustanovki)) as year_number
FROM indikator_korrozii_po_godam obj_godam
join(
        	select 
			distinct
				k.lineID,
				k.obj_id,
				k.j_id_i,
				k.j_id,
				k.nodeID1,
				k.nodeID2,
			    max(k.lineID) OVER(PARTITION BY k.obj_id ) AS max_id
			from (
				select
					distinct
						obj_godam.id as j_id,
						obj_godam.id_i as j_id_i,
						l.id as lineID,
						l.nodeID1,
						l.nodeID2,
						obj.id as obj_id,
						l.shape.STDistance(obj.shape) as length,
						MIN(l.shape.STDistance(obj.shape)) OVER(PARTITION BY obj.id ) AS "min_len"
				from indikator_korrozii obj
				LEFT JOIN indikator_korrozii_po_godam obj_godam on obj_godam.id_i = obj.id 

                left join (
                    select l.id, l.nodeID1, l.nodeID2, l.shape from linesobj l 
            		left join nodes n on n.id = l.nodeID1 and n.fileID in ($fileID$)
			   	    where l.removed = 0 AND n.removed=0 AND n.internalNodeID IS NULL
                ) l on l.shape.STDistance(obj.shape.STPointN(1)) < 0.1

				left join nodes n on n.id = l.nodeID1
					WHERE
					n.fileID in ($fileID$)
					and
					year(obj_godam.data_ustanovki) >= $YEAR1$-1 AND year(obj_godam.data_ustanovki) <= $YEAR2$-1

					AND

					(
    				year(obj_godam.data_izvlecheniya) >= year(obj_godam.data_ustanovki) 
--                    OR	year(obj_godam.data_izvlecheniya) = year(obj_godam.data_ustanovki)+1 OR
--					year(obj_godam.data_izvlecheniya) = year(obj_godam.data_ustanovki)
					)
	
		            
			)k
	 WHERE k.min_len = k.length 
	
	) tt on tt.j_id = obj_godam.id 
JOIN indikator_korrozii obj ON obj.id = tt.obj_id
LEFT JOIN externalSignLine el ON el.id = obj.truboprovod
LEFT JOIN heatPipeSections hps ON hps.lineID=tt.lineID

LEFT JOIN stateInds ON stateInds.id = obj_godam.stateIndID     

LEFT JOIN nodes n1 ON n1.id = tt.nodeID1
LEFT JOIN nodes n2 ON n2.id = tt.nodeID2	

left join externalCodes ec1 ON ec1.id = n1.externalCodeID
left join externalCodes ec2 ON ec2.id = n2.externalCodeID		

LEFT JOIN uchastok_ms ms ON ms.id= hps.magistralSite
LEFT JOIN magistrali magistral_ms ON magistral_ms.id = ms.magistral    

LEFT JOIN uchastok_rs rs ON rs.id= hps.distSite
LEFT JOIN magistrali magistral_rs ON magistral_rs.id = rs.magistral

WHERE     
tt.lineID = tt.max_id
and not (n1.x = 0 and n1.y = 0)
and not (n2.x = 0 and n2.y = 0)
) t
GROUP BY t.id,   		
        t.magistral_name,		
        t.ex_code_1,		
        t.ex_code_2,		
        t.beginNode,
		t.endNode,
        t.nomer_indikatora_korrozii,
        t.mesto_ustanovki,
		t.externalSignLine
ORDER BY num