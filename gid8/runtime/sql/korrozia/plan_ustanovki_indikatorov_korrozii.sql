SELECT
    t.id,
    row_number() over(ORDER BY t.responsibleName desc, t.magistral_name desc ) as num,
    ISNULL(t.magistral_name, '-') as 'magistral_name',
	IIF (t.mesto_ustanovki is NULL or t.mesto_ustanovki = '','-', t.mesto_ustanovki) as 'mesto_ustanovki',
    IIF(t.nomer_indikatora_korrozii IS NULL OR t.nomer_indikatora_korrozii = ' ', '-', t.nomer_indikatora_korrozii) as 'nomer_indikatora_korrozii',
    ISNULL(t.externalSignLine,'-') as 'externalSignLine' ,
	ISNULL( convert(varchar, t.data_planirovaniya, 104),'-') as 'data_planirovaniya',
--    t.data_planirovaniya as 'data_planirovaniya',
    ISNULL(t.ex_code_1, '-') as ex_code_1,
	ISNULL(t.beginNode, '-') as beginNode,
    ISNULL(t.ex_code_2, '-') as ex_code_2,
	ISNULL(t.endNode, '-') as endNode,
    ISNULL(t.responsibleName,'-') as 'responsibleName',
    t.primechanie
    FROM (
        SELECT DISTINCT
            obj.id,
            IIF(magistral_ms.naimenovanie_magistrali is not NULL, magistral_ms.naimenovanie_magistrali, magistral_rs.naimenovanie_magistrali) as magistral_name,        
            obj.nomer_indikatora_korrozii as nomer_indikatora_korrozii,
            obj.mesto_ustanovki,    
            el.name as 'externalSignLine',
            obj_godam.data_planirovaniya,
            ec1.name as 'ex_code_1',
            IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'beginNode',
            ec2.name as 'ex_code_2',
            IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'endNode',	
            responsible.name as responsibleName,
            obj_godam.primechanie
        FROM indikator_korrozii_po_godam obj_godam		
        JOIN indikator_korrozii obj ON obj.id =  obj_godam.id_i	
        LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape.STPointN(1)) < 0.1
        LEFT JOIN externalSignLine el ON el.id = obj.truboprovod
        LEFT JOIN heatPipeSections hps ON hps.lineID=l.id

        LEFT JOIN responsibles_korrozia responsible ON responsible.id = obj.responsibleID 

        LEFT JOIN nodes n1 ON n1.id = l.nodeID1
        LEFT JOIN nodes n2 ON n2.id = l.nodeID2	

        left join externalCodes ec1 ON ec1.id = n1.externalCodeID
        left join externalCodes ec2 ON ec2.id = n2.externalCodeID		

        LEFT JOIN uchastok_ms ms ON ms.id= hps.magistralSite
        LEFT JOIN magistrali magistral_ms ON magistral_ms.id = ms.magistral    

        LEFT JOIN uchastok_rs rs ON rs.id= hps.distSite
        LEFT JOIN magistrali magistral_rs ON magistral_rs.id = rs.magistral    

        WHERE  
            --year(obj.data_planirovaniya) >= $YEAR1$ AND year(obj.data_planirovaniya) <= $YEAR2$
            year(obj_godam.data_planirovaniya) = $YEAR1$
            and n1.fileID in ($fileID$)
            and l.externalSignLineID is not NULL
            and l.removed=0
            and not (n1.x = 0 and n1.y = 0)
            and not (n2.x = 0 and n2.y = 0)
    )t

	order by num