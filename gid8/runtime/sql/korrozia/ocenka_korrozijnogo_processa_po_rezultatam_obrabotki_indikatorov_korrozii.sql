SELECT
    t.id,
    row_number() over(ORDER BY t.responsibleName desc, t.magistral_name desc ) as num,
    ISNULL(t.magistral_name, '-') as 'magistral_name',
    IIF( PATINDEX('',ISNULL(t.mesto_ustanovki,'')) > 0, '-', t.mesto_ustanovki ) as 'mesto_ustanovki',
    IIF(t.nomer_indikatora_korrozii IS NULL OR t.nomer_indikatora_korrozii = ' ', '-', t.nomer_indikatora_korrozii) as 'nomer_indikatora_korrozii',
    ISNULL(t.externalSignLine,'-') as 'externalSignLine' ,
    t.data_ustanovki as 'data_ustanovki',
    t.data_izvlecheniya as 'data_izvlecheniya',
    ISNULL(t.count_day,0) as 'count_day',
    ISNULL(t.srednyaya_skorost_korrozii__mm_god,0) as 'srednyaya_skorost_korrozii__mm_god',
    ISNULL(t.corrosionMarkName,'-') as 'corrosionMarkName', 
    ISNULL( t.agressivnostName, '-') as 'agressivnostName',
    ISNULL(t.ex_code_1, '-') as ex_code_1,
    IIF( PATINDEX('',ISNULL(t.beginNode,'')) > 0, '-', t.beginNode ) as 'beginNode',    
    ISNULL(t.ex_code_2, '-') as ex_code_2,
    IIF( PATINDEX('',ISNULL(t.endNode,'')) > 0, '-', t.endNode ) as 'endNode',
    ISNULL(t.responsibleName,'-') as 'responsibleName',
    ISNULL(t.stateIndName,'-') as 'stateIndName'
    FROM (




 SELECT distinct
        obj.id,
        IIF(magistral_ms.naimenovanie_magistrali is not NULL, magistral_ms.naimenovanie_magistrali, magistral_rs.naimenovanie_magistrali) as magistral_name,        
        IIF(obj.mesto_ustanovki IS NULL OR obj.mesto_ustanovki = ' ', '-', obj.mesto_ustanovki) as mesto_ustanovki,
        IIF(obj.nomer_indikatora_korrozii IS NULL OR obj.nomer_indikatora_korrozii = ' ', '-', obj.nomer_indikatora_korrozii) as nomer_indikatora_korrozii,
        el.name as 'externalSignLine',
        obj_godam.data_ustanovki,
        obj_godam.data_izvlecheniya,    
        DATEDIFF ( day , obj_godam.data_ustanovki  , obj_godam.data_izvlecheniya) as 'count_day',
        obj_godam.srednyaya_skorost_korrozii__mm_god,
        ISNULL(agressivnost.name, '-') as agressivnostName, 
        ISNULL(corrosionMark.name, '-') as corrosionMarkName,
        ec1.name as 'ex_code_1',
        IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'beginNode',
        ec2.name as 'ex_code_2',
        IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'endNode',   
        responsible.name as responsibleName,
        stateInds.name as stateIndName,
        obj_godam.primechanie

        FROM indikator_korrozii_po_godam obj_godam
        JOIN (
            
            select 
            distinct
                k.lineID,
                k.obj_id,
                k.j_id_i,
                k.j_id,
    			k.nodeID1,
				k.nodeID2,
                k.length,
                k.min_len,
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
                    (       
                        obj_godam.sostoyanie=2 AND YEAR(obj_godam.data_ustanovki)=$YEAR1$
                    OR
                        obj_godam.sostoyanie=3 AND YEAR(obj_godam.data_ustanovki)=$YEAR1$
          -- year (obj_godam.data_ustanovki) = $YEAR1$  and  (year(obj_godam.data_izvlecheniya) = $YEAR1$ OR year(obj_godam.data_izvlecheniya) = $YEAR1$+1)
        ) 
            )k
     WHERE k.min_len = k.length 
     --and k.lineID = k.max_id
    ) tt on tt.j_id = obj_godam.id 
    JOIN indikator_korrozii obj ON obj.id =  tt.obj_id      

--    JOIN linesobj l ON l.id = tt.lineID and l.id = tt.max_id
      
       LEFT JOIN externalSignLine el ON el.id = obj.truboprovod
        LEFT JOIN heatPipeSections hps ON hps.lineID=tt.lineID

        LEFT JOIN responsibles_korrozia responsible ON responsible.id = obj.responsibleID 
        
        LEFT JOIN stateInds ON stateInds.id = obj_godam.stateIndID     

        LEFT JOIN nodes n1 ON n1.id = tt.nodeID1
        LEFT JOIN nodes n2 ON n2.id = tt.nodeID2 

        left join externalCodes ec1 ON ec1.id = n1.externalCodeID
        left join externalCodes ec2 ON ec2.id = n2.externalCodeID       

        LEFT JOIN uchastok_ms ms ON ms.id= hps.magistralSite
        LEFT JOIN magistrali magistral_ms ON magistral_ms.id = ms.magistral    

        LEFT JOIN uchastok_rs rs ON rs.id= hps.distSite
        LEFT JOIN magistrali magistral_rs ON magistral_rs.id = rs.magistral 

        LEFT JOIN netWaterAggressivenesses agressivnost ON agressivnost.id = obj_godam.agressivnost_setevoy_vody
        LEFT JOIN corrosionProcessMarks corrosionMark ON corrosionMark.id = obj_godam.otsenka_korrozionnogo_protsessa
     where tt.lineID = tt.max_id
        --obj.id in (43)
        
            --l.externalSignLineID is not NULL
            --and l.removed=0
            /*
            year (obj_godam.data_ustanovki) = $YEAR1$  and  (year(obj_godam.data_izvlecheniya) = $YEAR1$ OR year(obj_godam.data_izvlecheniya) = $YEAR1$+1)



            and n1.fileID  IN (2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,48,49,50,51,52,54,55,801,806,3151,3152)
            and l.externalSignLineID is not NULL
            and l.removed=0
            and not (n1.x = 0 and n1.y = 0)
            and not (n2.x = 0 and n2.y = 0)
            */






    )t
    order by num