SELECT top 2147483647
	t.id,	
    IIF( PATINDEX('',ISNULL(t.beginNode,'')) > 0, '-', t.beginNode ) as 'Начальный узел',
	IIF( PATINDEX('',ISNULL(t.endNode,'')) > 0, '-', t.endNode ) as 'Конечный узел',
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
    ISNULL(t.stateIndName, '-') as 'Состояние стержня',
    ISNULL(t.sredniy_ves_plastiny_posle_ispytaniy__g, 0) as 'Средний вес пластины после испытаний, г',
    ISNULL(t.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g,0) as 'Потеря массы средняя при кислотной обрабоке, г',
    ISNULL(t.srednyaya_skorost_korrozii__mm_god,0) as 'Средняя скорость коррозии, мм/год',
    ISNULL(t.corrosionMarkName,'-') as 'Оценка коррозионного процесса',	
    ISNULL( t.agressivnostName, '-') as 'Агрессивность сетевой воды',
    t.primechanie as 'Примечание'
	FROM (
		SELECT
			distinct 
			IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'beginNode',
			IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'endNode',	
--            sost.name as 'state',
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
            obj.diametr_truby_podayuschiy__uslovn__mm,
            obj.diametr_truby_obratnyy__uslovn__mm,
            obj_godam.data_planirovaniya,
            obj_godam.data_ustanovki,
            obj_godam.data_izvlecheniya,             
            obj_godam.sredniy_ves_plastiny_posle_ispytaniy__g,
            obj_godam.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g,
            obj_godam.srednyaya_skorost_korrozii__mm_god,
            obj_godam.otsenka_korrozionnogo_protsessa,
            obj_godam.primechanie,
            hps.diameterCondit as 'hpsDiameterCondit',
            corrosionMark.name as corrosionMarkName,
            agressivnost.name as agressivnostName,
            responsible.name as responsibleName,
            stateInds.name as stateIndName,
            IIF(ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as naimenovanie_uchastka,
            IIF(re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla is not NULL,
	            re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as naimenovanie_rayona,
            IIF(ue_ms.nomer_uchastka is not NULL, ue_ms.nomer_uchastka, ue_rs.nomer_uchastka) as district_site,
            IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as fio,
			obj.id,
			el.name as 'externalLineStr'	
			-- obj_godam.vneshniy_vid_plastin		
		FROM indikator_korrozii obj		
        JOIN indikator_korrozii_po_godam obj_godam on obj_godam.id_i = obj.id 
        
		LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape.STPointN(1)) < 0.1
		LEFT JOIN externalSignLine el ON el.id = obj.truboprovod
		LEFT JOIN heatPipeSections hps ON hps.lineID=l.id

        LEFT JOIN responsibles_korrozia responsible ON responsible.id = obj.responsibleID 
        LEFT JOIN stateInds ON stateInds.id = obj_godam.stateIndID         

        LEFT JOIN stateIndicator sost ON sost.id = obj.sostoyanie
	    LEFT JOIN tubingTypes tubingType ON tubingType.id = obj.vid_prokladki
        LEFT JOIN netWaterAggressivenesses agressivnost ON agressivnost.id = obj_godam.agressivnost_setevoy_vody
        LEFT JOIN corrosionProcessMarks corrosionMark ON corrosionMark.id = obj_godam.otsenka_korrozionnogo_protsessa
        LEFT JOIN coolantTypes coolantType ON coolantType.id = obj.teplonositel

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

		WHERE

        l.removed=0 AND
        n1.removed=0 AND 

        n1.fileID in ($fileID$)		
		and l.externalSignLineID is not NULL
        AND YEAR(obj_godam.data_ustanovki)=$YEAR$
        ) as t

order by data_planirovaniya
