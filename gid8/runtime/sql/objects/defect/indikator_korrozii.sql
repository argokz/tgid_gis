SELECT top 2147483647
    t.id,   
    IIF( PATINDEX('',ISNULL(t.beginNode,'')) > 0, '-', t.beginNode ) as 'Начальный узел',
    IIF( PATINDEX('',ISNULL(t.endNode,'')) > 0, '-', t.endNode ) as 'Конечный узел',
--    t.beginNode as 'Начальный узел',
--  t.endNode as 'Конечный узел',
    ISNULL(t.state, '-') as 'Состояние',        
    ISNULL( convert(varchar, t.data_planirovaniya, 104),'-') as 'Дата планирования',    
    ISNULL( convert(varchar, t.data_ustanovki, 104),'-') as 'Дата установки',   
    ISNULL( convert(varchar, t.data_izvlecheniya, 104),'-') as 'Дата извлечения',   
    ISNULL(t.externalLineStr,'-') as 'Трубопровод', 
    ISNULL(convert(varchar, t.hpsDiameterCondit), '-') as 'Диаметр трубы',
    ISNULL(t.mesto_ustanovki,'-') as 'Место установки', 
    ISNULL(t.nomer_indikatora_korrozii, '-') as 'Номер индикатора коррозии',    
    ISNULL(t.kod_rs_blizhayshey_kamery, '-') as 'Код РС ближайшей камеры',
    ISNULL(t.blizhayshaya_kamera, '-') as 'Ближайшая камера',
    ISNULL(t.rasstoyanie_do_kamery__m, '0')  as 'Расстояние до камеры, м',    
    ISNULL(t.naimenovanie_uchastka,'-') as 'Наименование участка',    
    ISNULL(t.naimenovanie_rayona, '-') as 'Наименование района',
--    '-' as 'Наименование участка',    
--    '-' as 'Наименование района',
    ISNULL(t.istochnik_tepla, '-') as 'Источник тепла', 
    ISNULL(t.fio,'-') as 'Начальник участка',
--    '-' as 'Начальник участка',
    t.coolantTypeName as 'Теплоноситель',   
    ISNULL(t.responsibleName, '-') as 'Ответственный',
    ISNULL(t.kolichestvo_plastin_v_sborke, 0) as 'Количество пластин в сборке',
    ISNULL(t.sredniy_ves_plastiny_pri_ustanovke__g, 0) as 'Средний вес пластины при установке, г',  
    ISNULL(t.radius_krugloy_plastiny__mm, 0) as 'Радиус круглой пластины, мм',    
    ISNULL(t.radius_vtulki__mm, 0) as 'Радиус втулки, мм',
    ISNULL(t.tolschina_plastiny__mm, 0) as 'Толщина пластины, мм',
    ISNULL(t.stateIndName, '-') as 'Состояние стержня',

    ISNULL(t.sredniy_ves_plastiny_posle_ispytaniy__g, 0) as 'Средний вес пластины после испытаний, г',
    ISNULL(t.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g,0) as 'Потеря массы средняя при кислотной обрабоке, г',
    ISNULL(t.srednyaya_skorost_korrozii__mm_god,0) as 'Средняя скорость коррозии, мм/год',

    ISNULL(t.corrosionMarkName,'-') as 'Оценка коррозионного процесса', 
    ISNULL(t.agressivnostName, '-') as 'Агрессивность сетевой воды',

    t.primechanie as 'Примечание'
    --t.vneshniy_vid_plastin as 'Внешний вид пластин',
    --t.diametr_truby_podayuschiy__uslovn__mm as 'Диаметр трубы подающий, условн, мм',
    --t.diametr_truby_obratnyy__uslovn__mm as 'Диаметр трубы обратный, условн, мм',
    --t.ulitsa as 'Улица',
    --t.nomer_doma as 'Номер дома'
    --t.district_site,
    FROM (
SELECT
            IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'beginNode',
            IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'endNode',   
            sost.name as 'state',           
            obj.nomer_indikatora_korrozii,
            obj.istochnik_tepla,
            obj.blizhayshaya_kamera,
            obj.kod_rs_blizhayshey_kamery,          
            obj.rasstoyanie_do_kamery__m ,
            obj.mesto_ustanovki,
            obj.ulitsa,
            obj.nomer_doma,
            coolantType.name as 'coolantTypeName',
            obj_godam.kolichestvo_plastin_v_sborke,
            obj_godam.sredniy_ves_plastiny_pri_ustanovke__g, 
            obj_godam.radius_krugloy_plastiny__mm,
            obj_godam.radius_vtulki__mm, 
            obj_godam.tolschina_plastiny__mm,

            obj_godam.data_planirovaniya,
            obj_godam.data_ustanovki,
            obj_godam.data_izvlecheniya,         

            obj_godam.sredniy_ves_plastiny_posle_ispytaniy__g,
            obj_godam.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g,
            obj_godam.srednyaya_skorost_korrozii__mm_god,

            corrosionMark.name as corrosionMarkName,
            agressivnost.name as agressivnostName,
            stateInds.name as stateIndName,
            responsible.name as responsibleName,            
            hps.diameterCondit as 'hpsDiameterCondit',
            obj_godam.primechanie,
        
            -- obj.vneshniy_vid_plastin,

            IIF(ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as naimenovanie_uchastka,
            IIF(re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla is not NULL,
                  re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as naimenovanie_rayona,
            IIF(ue_ms.nomer_uchastka is not NULL, ue_ms.nomer_uchastka, ue_rs.nomer_uchastka) as district_site,
            IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as fio,

            obj.id,
            el.name as 'externalLineStr'
        FROM indikator_korrozii obj     
        JOIN (
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

--                left JOIN linesobj l ON ( l.removed = 0 
--                and l.shape.STDistance(obj.shape.STPointN(1)) < 0.1 )
--                left join nodes n on n.id = l.nodeID1 AND n.internalNodeID IS NULL
$j$
                WHERE 
--                    n.fileID in ($fileID$) 
                    (
        obj_godam.sostoyanie=1 AND YEAR(obj_godam.data_planirovaniya)=$YEAR1$
            OR
        obj_godam.sostoyanie=2 AND YEAR(obj_godam.data_ustanovki)=$YEAR1$
            OR
        obj_godam.sostoyanie=3 AND YEAR(obj_godam.data_ustanovki)=$YEAR1$
        ) 
    $and_condition$


            )k
     WHERE k.min_len = k.length 
    ) tt on tt.obj_id = obj.id 
        LEFT JOIN indikator_korrozii_po_godam obj_godam on obj_godam.id = tt.j_id
        LEFT JOIN externalSignLine el ON el.id = obj.truboprovod
--        LEFT JOIN linesobj l ON l.id = tt.lineID
        LEFT JOIN heatPipeSections hps ON hps.lineID=tt.lineID
        LEFT JOIN nodes n1 ON n1.id = tt.nodeID1     
        LEFT JOIN nodes n2 ON n2.id = tt.nodeID2 

        LEFT JOIN stateIndicator sost ON sost.id = obj_godam.sostoyanie
        LEFT JOIN coolantTypes coolantType ON coolantType.id = obj.teplonositel
        LEFT JOIN responsibles_korrozia responsible ON responsible.id = obj.responsibleID 
        
        LEFT JOIN netWaterAggressivenesses agressivnost ON agressivnost.id = obj_godam.agressivnost_setevoy_vody        
        LEFT JOIN corrosionProcessMarks corrosionMark ON corrosionMark.id = obj_godam.otsenka_korrozionnogo_protsessa       
        LEFT JOIN stateInds ON stateInds.id = obj_godam.stateIndID      

        LEFT JOIN uchastok_ms ms ON ms.id= hps.magistralSite
        left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
        left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
        left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
        
        LEFT JOIN uchastok_rs rs ON rs.id= hps.distSite
        left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
        left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
        left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka

        where tt.lineID = tt.max_id
) as t

ORDER BY t.state,  data_planirovaniya 
