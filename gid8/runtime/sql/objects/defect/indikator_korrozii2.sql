
SELECT top 2147483647
	t.id,	
    IIF( PATINDEX('',ISNULL(t.beginNode,'')) > 0, '-', t.beginNode ) as 'Начальный узел',
	IIF( PATINDEX('',ISNULL(t.endNode,'')) > 0, '-', t.endNode ) as 'Конечный узел',
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
    ISNULL(t.istochnik_tepla, '-') as 'Источник тепла',	
    ISNULL(t.fio,'-') as 'Начальник участка',
    t.coolantTypeName as 'Теплоноситель',	
    ISNULL(t.responsibleName, '-') as 'Ответственный',
    ISNULL(t.kolichestvo_plastin_v_sborke, 0) as 'Количество пластин в сборке',
    ISNULL(t.sredniy_ves_plastiny_pri_ustanovke__g, 0) as 'Средний вес пластины при установке, г',	
    ISNULL(t.radius_krugloy_plastiny__mm, 0) as 'Радиус круглой пластины, мм',    
	ISNULL(t.radius_vtulki__mm, 0) as 'Радиус втулки, мм',
    ISNULL(t.tolschina_plastiny__mm, 0) as 'Толщина пластины, мм',
    ISNULL(stateInds.name, '-') as 'Состояние стержня',



    ISNULL(t.sredniy_ves_plastiny_posle_ispytaniy__g, 0) as 'Средний вес пластины после испытаний, г',
    ISNULL(t.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g,0) as 'Потеря массы средняя при кислотной обрабоке, г',
    ISNULL(t.srednyaya_skorost_korrozii__mm_god,0) as 'Средняя скорость коррозии, мм/год',

    ISNULL(corrosionMark.name,'-') as 'Оценка коррозионного процесса',	
    ISNULL( agressivnost.name, '-') as 'Агрессивность сетевой воды',





    t.primechanie as 'Примечание'
    --t.vneshniy_vid_plastin as 'Внешний вид пластин',
	--t.diametr_truby_podayuschiy__uslovn__mm as 'Диаметр трубы подающий, условн, мм',
    --t.diametr_truby_obratnyy__uslovn__mm as 'Диаметр трубы обратный, условн, мм',
	--t.ulitsa as 'Улица',
    --t.nomer_doma as 'Номер дома'
    --t.district_site,
	FROM (
		SELECT
			distinct 
			IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'beginNode',
			IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'endNode',	
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, 'Архивный' ,sost.name) as 'state',			
            obj.nomer_indikatora_korrozii,
            obj.istochnik_tepla,
            obj.blizhayshaya_kamera,
            obj.kod_rs_blizhayshey_kamery,          
            obj.rasstoyanie_do_kamery__m ,
            obj.mesto_ustanovki,
            obj.ulitsa,
            obj.nomer_doma,
            coolantType.name as 'coolantTypeName',
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.kolichestvo_plastin_v_sborke, obj.kolichestvo_plastin_v_sborke) as kolichestvo_plastin_v_sborke,
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.sredniy_ves_plastiny_pri_ustanovke__g, obj.sredniy_ves_plastiny_pri_ustanovke__g) as sredniy_ves_plastiny_pri_ustanovke__g,
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.radius_krugloy_plastiny__mm, obj.radius_krugloy_plastiny__mm) as  radius_krugloy_plastiny__mm,
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.radius_vtulki__mm, obj.radius_vtulki__mm) as radius_vtulki__mm,
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.tolschina_plastiny__mm, obj.tolschina_plastiny__mm) as tolschina_plastiny__mm,
            --obj.diametr_truby_podayuschiy__uslovn__mm,
            --obj.diametr_truby_obratnyy__uslovn__mm,
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.data_planirovaniya,obj.data_planirovaniya) as data_planirovaniya,
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.data_ustanovki,obj.data_ustanovki) as data_ustanovki,
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.data_izvlecheniya,obj.data_izvlecheniya) as data_izvlecheniya,          
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.sredniy_ves_plastiny_posle_ispytaniy__g ,obj.sredniy_ves_plastiny_posle_ispytaniy__g) as sredniy_ves_plastiny_posle_ispytaniy__g ,
			IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g, obj.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g) as poterya_massy_srednyaya_pri_kislotnoy_obraboke__g,
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.srednyaya_skorost_korrozii__mm_god, obj.srednyaya_skorost_korrozii__mm_god) as srednyaya_skorost_korrozii__mm_god ,		   

            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1,  obj_godam.otsenka_korrozionnogo_protsessa, obj.otsenka_korrozionnogo_protsessa) as  otsenka_korrozionnogo_protsessa,
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1,  obj_godam.agressivnost_setevoy_vody,  obj.agressivnost_setevoy_vody) as agressivnost_setevoy_vody,
          
            responsible.name as responsibleName,			
            IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1,   obj_godam.stateIndID,   obj.stateIndID) as stateIndID, 
         
            hps.diameterCondit as 'hpsDiameterCondit',
			IIF(year(obj_godam.data_izvlecheniya) = $YEAR1$+1, obj_godam.primechanie, obj.primechanie) as primechanie,
        
			-- obj.vneshniy_vid_plastin,
            IIF(ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as naimenovanie_uchastka,
            IIF(re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla is not NULL,
	            re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as naimenovanie_rayona,
            IIF(ue_ms.nomer_uchastka is not NULL, ue_ms.nomer_uchastka, ue_rs.nomer_uchastka) as district_site,
            IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as fio,
			obj.id,
			el.name as 'externalLineStr'
		FROM indikator_korrozii obj		
		LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape.STPointN(1)) < 0.1
		LEFT JOIN externalSignLine el ON el.id = obj.truboprovod
		LEFT JOIN heatPipeSections hps ON hps.lineID=l.id
        LEFT JOIN indikator_korrozii_po_godam obj_godam on obj_godam.id_i = obj.id 
        LEFT JOIN stateIndicator sost ON sost.id = obj.sostoyanie
	    LEFT JOIN tubingTypes tubingType ON tubingType.id = obj.vid_prokladki
        LEFT JOIN coolantTypes coolantType ON coolantType.id = obj.teplonositel
        LEFT JOIN responsibles_korrozia responsible ON responsible.id = obj.responsibleID 
    	LEFT JOIN nodes n1 ON n1.id = l.nodeID1
		LEFT JOIN nodes n2 ON n2.id = l.nodeID2		
        left join externalCodes ec1 ON ec1.id = n1.externalCodeID
		left join externalCodes ec2 ON ec2.id = n2.externalCodeID		

        LEFT JOIN uchastok_ms ms ON ms.id= hps.magistralSite
        left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
        left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
        left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
        
		LEFT JOIN uchastok_rs rs ON rs.id= hps.distSite
        left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
        left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
        left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
--$j$
		WHERE (
		obj.sostoyanie=1 AND YEAR(obj.data_planirovaniya)=$YEAR1$
			OR
		obj.sostoyanie=2 AND YEAR(obj.data_ustanovki)=$YEAR1$
			OR
		obj.sostoyanie=3 AND YEAR(obj.data_izvlecheniya)=$YEAR1$+1
			OR YEAR(obj_godam.data_izvlecheniya)=$YEAR1$+1
		)

        AND

        l.removed=0 AND
        n1.removed=0 AND 

		n1.fileID in ($fileID$)	
--        AND ((sostoyanie<>4) OR YEAR(obj_godam.data_izvlecheniya) = YEAR(GETDATE()))
		and l.externalSignLineID is not NULL
        $and_condition$
	) as t

    LEFT JOIN netWaterAggressivenesses agressivnost ON agressivnost.id = t.agressivnost_setevoy_vody		
    LEFT JOIN corrosionProcessMarks corrosionMark ON corrosionMark.id = t.otsenka_korrozionnogo_protsessa		
    LEFT JOIN stateInds ON stateInds.id = t.stateIndID		



order by t.state,  data_planirovaniya


