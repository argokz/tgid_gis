SELECT TOP 10000000 * FROM (

select distinct
    pss.id as id,
    IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',CONCAT(nt1.name, ' ', n1.externalNodeName), n1.nodeName) as 'Наименование начального узла',
    IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',CONCAT(nt2.name, ' ', n2.externalNodeName), n2.nodeName) as 'Наименование конечного узла',
    obj.otchet_po_defektu as 'Наименование/Адрес',
    ts.name as 'Тепловая сеть',
    obj.harakteristika_uchastkov_remontiruemoj_teplovoj_seti as 'Характеристика участков ремонтируемой тепловой сети',
    rt.name as 'Вид ремонта',
    st.name as 'Состояние',
    obj.data_nachala_plan as 'Дата начала ремонта плановая',
    obj.data_okonchaniya_plan as 'Дата окончания ремонта плановая',
    obj.data_utverzhdeniya_plana as 'Дата утверждения плана',
    obj.len_tube_cur as 'Длина заменённых труб, план, м',
    obj.diametr_trub_plan as 'Диаметр условный заменённых труб, план, мм',
    obj.len_izol_plan as 'Восстановление тепловой изоляции поверхности труб, план, м2',
    obj.len_channel_plan as 'Восстановление канальной прокладки, план, м',
    obj.asfaltirovanie_plan as 'Асфальтирование, план, м2',
    obj.vydelennye_sredstva_plan as 'Объем выделенных средств и затрат, тыс.тг. с НДС',
    obj.remontnyj_personal_plan as 'Ремонтный персонал, план, тыс.чел.',
    obj.data_nachala_remonta as 'Дата начала ремонтных работ',
    obj.data_zaversheniya_remonta as 'Дата завершения ремонтных работ',
    obj.harakteristika_rabot as 'Характеристика работ (отчёт)',
    obj.len_tube_cur as 'Длина заменённых труб, ремонт, факт, м',
    obj.len_izol_cur as 'Восстановление тепловой изоляции поверхности труб, ремонт, факт, м2',
    obj.len_channel_cur as 'Восстановление канальной прокладки, факт, м',
    obj.asfaltirovanie as 'Асфальтирование, ремонт, факт, м2',
    obj.vydelennye_sredstva as 'Объем освоенных средств и затрат, тыс.тг. с НДС',
    obj.remontnyj_personal as 'Ремонтный персонал, использовано, чел',
    obj.rezultaty_remonta as 'Результаты ремонта (отчёт)',
    obj.kolichestvo_otklyuchennyh_potrebitelej as 'Количество отключенных потребителей',
    obj.kolichestvo_nedootpushchennoj_teplovoj_energii as 'Количество недоотпущенной тепловой энергии, ГКал',
    obj.nomer_prikaza as 'Номер приказа на ввод в эксплуатацию',
    format(obj.data_prikaza_vvoda_v_ekspluataciyu,'dd.MM.yyyy') as 'Дата приказа ввода в эксплуацию',
    sb.name as 'Подразделение производившее ремонт',
    rs.fio as 'Ответственный за ремонт',
    --  rs.name as 'Ответственный за ремонт',
    Elektrich.name as 'Наличие эл.коммуникаций, электрохимзащиты газопровода',

    'Вблизи лежащие коммуникации' = 
    STUFF(
    ( SELECT concat(',', rtt2.name) AS n
    FROM faktory_riska_truboprovoda r2
        LEFT JOIN nalichie_vblizi_kommunikacij_for_shurfy rt2 ON r2.id=rt2.objID
        LEFT JOIN nalichie_vblizi_kommunikacij rtt2 ON rtt2.id=rt2.activityID
    WHERE r2.id=faktory_riska_truboprovoda.id
    FOR XML PATH('')
    , TYPE
    ).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),

    TransportElekricht.name as 'Наличие электрифицированного транспорта на расстоянии',
    peshehod.name as 'Прокладка трубопроводов под пешеходными зонами',
    LudiProklad.name as 'Прокладка трубопровода в местах массового скопления жителей города',
    tubingWays.name as 'Прокладка трубопроводов под авто и ж/д дорогами',
    harakter_grunta_shurf.ground as 'Грунт в месте прокладки',
    poverhnost_nad_trassoj.name as 'Поверхность над трассой',
    yn_podtoplenie_do_truby.name as 'Подтопление до трубы',
    sost_konstr.name as 'Состояние строительных конструкций',
    sostoyanie_stroitelnyh_konstrukcij_kanala.name as 'Состояние строительных конструкций канала',
    vnutrennee_sostoyanie_kanala.name as 'Внутреннее состояние канала',
    konstrukciya_drenazhnogo_ustrojstva.name as 'Конструкция дренажного устройства',
    sectWaterDumps.name as 'Водоотведение на участке',
    vneshny_vid.name as 'Внешний вид',
    sost_oborud.name as 'Состояние оборудования',
    nalichie_korrozii_obratka.name as 'Наличие коррозии (обратный трубопровод)',
    nalichie_korrozii_podacha.name as 'Наличие коррозии (подающий трубопровод)',
    sostoyanie_metalla_truboprovoda.name as 'Состояние металла трубопровода',
    sostoyanie_teplovoj_izolyacii_obratka.name as 'Состояние тепловой изоляции (обратный трубопровод)',
    sostoyanie_teplovoj_izolyacii_podacha.name as 'Состояние тепловой изоляции (подающий трубопровод)',
    sostoyanie_naruzhnogo_pokrytiya_obratka.name as 'Состояние наружного покрытия (обратный трубопровод)',
    sostoyanie_naruzhnogo_pokrytiya_podacha.name as 'Состояние наружного покрытия (подающий трубопровод)',
    sostoyanie_protivokorrozionnogo_pokrytiya_obratka.name as 'Состояние противокоррозионного покрытия (обратный трубопровод)',
    sostoyanie_protivokorrozionnogo_pokrytiya_podacha.name as 'Состояние противокоррозионного покрытия (подающий трубопровод)',
    tubingTypes.name AS 'Тип прокладки',
    faktory_riska_truboprovoda.len_tube as 'Длина заменённой трубы, м',
    faktory_riska_truboprovoda.len_channel as 'Восстановление канальной прокладки, м',
    faktory_riska_truboprovoda.diameterCondit as 'Диаметр условный, заменённой трубы, м',
    faktory_riska_truboprovoda.diameterInternal as 'Диаметр внутренний, заменённой трубы, м',
    faktory_riska_truboprovoda.diameterExternal as 'Диаметр наружный, заменённой трубы, м',
    faktory_riska_truboprovoda.wallThickness as 'Толщина стенки, , заменённой трубы, мм',
    faktory_riska_truboprovoda.len_izol as 'Восстановление тепловой изоляции поверхности трубы, м2',
    im.name as 'Материал тепловой изоляции',
    isolationTypes.name as 'Тип изоляции ППУ',
    faktory_riska_truboprovoda.isolThickness as 'Толщина изоляции, мм',
    externalMaterials.name as 'Материал наружного покрытия',
    faktory_riska_truboprovoda.externCoverThick as 'Толщина наружного покрытия, мм',
    anticorrMaterials.name as 'Материал антикоррозийного покрытия',
    faktory_riska_truboprovoda.asfaltirovanie as 'Асфальтирование, ремонт, м2',

    faktory_riska_truboprovoda.zamena_kanala_procent AS 'Замена канала, %',
    faktory_riska_truboprovoda.zamena_kompensatorov AS 'Замена компенсаторов, шт',
    CASE 
        WHEN faktory_riska_truboprovoda.rekonstrukciya_kamery_nachalnogo_uzla=1 THEN 'Да'
        ELSE 'Нет'
    END AS 'Реконструкция камеры начального узла',
    CASE 
        WHEN faktory_riska_truboprovoda.rekonstrukciya_kamery_konechnogo_uzla=1 THEN 'Да'
        ELSE 'Нет'
    END AS 'Реконструкция камеры конечного узла',
    faktory_riska_truboprovoda.ustanovka_i_zamena_zadvizhek AS 'Установка и замена задвижек, шт',
    'Перечень работ (трубопровода)' = 
    STUFF(
    ( SELECT concat(',', rtt2.name) AS n
    FROM faktory_riska_truboprovoda r2
        LEFT JOIN remontCapitalTube rt2 ON r2.id=rt2.objID
        LEFT JOIN remontCapitalTubeTypes rtt2 ON rtt2.id=rt2.activityID
    WHERE r2.id=obj.id
    FOR XML PATH('')
    , TYPE
    ).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
    'Перечень работ (канал)' = 
    STUFF(
    ( SELECT concat(',', rtt2.name) AS n
    FROM faktory_riska_truboprovoda r2
        LEFT JOIN remontChannel rt2 ON r2.id=rt2.objID
        LEFT JOIN remontChannelTypes rtt2 ON rtt2.id=rt2.activityID
    WHERE r2.id=obj.id
    FOR XML PATH('')
    , TYPE
    ).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
    'Перечень работ (камеры)' = 
    STUFF(
    ( SELECT concat(',', rtt2.name) AS n
    FROM faktory_riska_truboprovoda r2
        LEFT JOIN remontKamera rt2 ON r2.id=rt2.objID
        LEFT JOIN remontChannelTypes rtt2 ON rtt2.id=rt2.activityID
    WHERE r2.id=obj.id
    FOR XML PATH('')
    , TYPE
    ).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
    obj.id AS 'Номер контура'

from remont2 obj
    join remont2Deployed d on d.directionID = obj.id
    JOIN heatPipeSections hpss ON hpss.lineID=d.lineID $and_condition2$
    JOIN pipeSections pss ON pss.id=hpss.pipeSectionID
    join linesobj l on l.id = d.lineID
    JOIN nodes n1 ON n1.id=pss.nodeID1
    JOIN nodes n2 ON n2.id=pss.nodeID2
    left join externalCodes ec1 ON ec1.id = n1.externalCodeID
    left join externalCodes ec2 ON ec2.id = n2.externalCodeID
    LEFT JOIN nodeTypes nt1 ON nt1.id=n1.nodeTypeID
    LEFT JOIN nodeTypes nt2 ON nt2.id=n2.nodeTypeID

    left join remontTypes rt on rt.id = obj.remontTypeID
    left join stateRemont2 st on st.id = obj.stateID
    left join subdivisions sb on sb.id = obj.subdivisionID
    --  left join responsibles rs on rs.id = obj.responsibleID
    left join nachalniki_uchastkov rs on rs.id = obj.responsibleID
    left join(                                  SELECT TOP 10
            1 AS id, 'Магистральная сеть' AS name
    UNION
        SELECT 2, 'Внутриквартальная сеть') ts on ts.id = obj.teplovaya_setID

    left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = pss.id and faktory_riska_truboprovoda.objID = obj.id and faktory_riska_truboprovoda.obj_type_faktory_riskaID = 3
    left join tubingTypes on tubingTypes.id=faktory_riska_truboprovoda.tubingTypeID
    LEFT JOIN faktRiska_4_elekt Elektrich on Elektrich.id = faktory_riska_truboprovoda.Elektrich
    LEFT JOIN faktRiska_5_transport TransportElekricht on TransportElekricht.id = faktory_riska_truboprovoda.TransportElekricht
    LEFT JOIN faktRiska_15_peshehod peshehod on peshehod.id = faktory_riska_truboprovoda.Pesehod
    LEFT JOIN tubingWays on tubingWays.id = faktory_riska_truboprovoda.ZhdDorogi
    LEFT JOIN faktRiska_17_skoplenie_zhit LudiProklad on LudiProklad.id = faktory_riska_truboprovoda.LudiProklad
    left join poverhnost_nad_trassoj on poverhnost_nad_trassoj.id = faktory_riska_truboprovoda.poverhnost_nad_trassojID
    left join sprav.dbo.[01_teploprovodnost_grunta] harakter_grunta_shurf on harakter_grunta_shurf.id = faktory_riska_truboprovoda.harakter_gruntaID
    left join YesOrNo yn_podtoplenie_do_truby on yn_podtoplenie_do_truby.id = faktory_riska_truboprovoda.podtoplenie_do_truby
    left join sost_konstr on sost_konstr.id=faktory_riska_truboprovoda.SostKonstrukz
    left join sostoyanie_stroitelnyh_konstrukcij_kanala on sostoyanie_stroitelnyh_konstrukcij_kanala.id = faktory_riska_truboprovoda.sostoyanie_stroitelnyh_konstrukcij_kanalaID
    left join vnutrennee_sostoyanie_kanala on vnutrennee_sostoyanie_kanala.id = faktory_riska_truboprovoda.vnutrennee_sostoyanie_kanalaID
    left join konstrukciya_drenazhnogo_ustrojstva on konstrukciya_drenazhnogo_ustrojstva.id = faktory_riska_truboprovoda.konstrukciya_drenazhnogo_ustrojstvaID
    LEFT JOIN sectWaterDumps on sectWaterDumps.id = faktory_riska_truboprovoda.Vodootved
    left join vneshny_vid on vneshny_vid.id=faktory_riska_truboprovoda.VnesniiVid
    left join sost_oborud on sost_oborud.id=faktory_riska_truboprovoda.SostOborudovania
    left join nalichie_korrozii_shurf nalichie_korrozii_podacha on nalichie_korrozii_podacha.id = faktory_riska_truboprovoda.nalichie_korrozii_podachaID
    left join nalichie_korrozii_shurf nalichie_korrozii_obratka on nalichie_korrozii_obratka.id = faktory_riska_truboprovoda.nalichie_korrozii_obratkaID
    left join sostoyanie_metalla_truboprovoda on sostoyanie_metalla_truboprovoda.id = faktory_riska_truboprovoda.sostoyanie_metalla_truboprovodaID
    left join sostoyanie_teplovoj_izolyacii sostoyanie_teplovoj_izolyacii_obratka on sostoyanie_teplovoj_izolyacii_obratka.id = faktory_riska_truboprovoda.sostoyanie_teplovoj_izolyacii_obratkaID
    left join sostoyanie_teplovoj_izolyacii sostoyanie_teplovoj_izolyacii_podacha on sostoyanie_teplovoj_izolyacii_podacha.id = faktory_riska_truboprovoda.sostoyanie_teplovoj_izolyacii_podachaID
    left join sostoyanie_naruzhnogo_pokrytiya sostoyanie_naruzhnogo_pokrytiya_obratka on sostoyanie_naruzhnogo_pokrytiya_obratka.id = faktory_riska_truboprovoda.sostoyanie_naruzhnogo_pokrytiya_obratkaID
    left join sostoyanie_naruzhnogo_pokrytiya sostoyanie_naruzhnogo_pokrytiya_podacha on sostoyanie_naruzhnogo_pokrytiya_podacha.id = faktory_riska_truboprovoda.sostoyanie_naruzhnogo_pokrytiya_podachaID
    left join sostoyanie_protivokorrozionnogo_pokrytiya_shurf sostoyanie_protivokorrozionnogo_pokrytiya_podacha on sostoyanie_protivokorrozionnogo_pokrytiya_podacha.id = faktory_riska_truboprovoda.sostoyanie_protivokorrozionnogo_pokrytiya_podachaID
    left join sostoyanie_protivokorrozionnogo_pokrytiya_shurf sostoyanie_protivokorrozionnogo_pokrytiya_obratka on sostoyanie_protivokorrozionnogo_pokrytiya_obratka.id = faktory_riska_truboprovoda.sostoyanie_protivokorrozionnogo_pokrytiya_obratkaID
    left join isolMaterials im on im.id = faktory_riska_truboprovoda.isolMaterialID
    left join isolationTypes on isolationTypes.id = faktory_riska_truboprovoda.isolationTypeID
    left join externalMaterials on externalMaterials.id = faktory_riska_truboprovoda.externMaterialID
    left join anticorrMaterials on anticorrMaterials.id = faktory_riska_truboprovoda.anticorrMaterialID

WHERE
  ((obj.data_nachala_plan
$season_condition$) OR
(obj.data_nachala_remonta $season_condition$)) 
  AND 
    $condition$

--  AND obj.stateID $stateID$
--  AND obj.remontTypeID $remontTypeID$

) _____

order by [Номер контура]
