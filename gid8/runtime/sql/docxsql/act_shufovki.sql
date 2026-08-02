--ALTER INDEX [PK_Individual_ShurfyID] ON [dbo].[shurfy] REBUILD PARTITION = ALL WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON)
--GO
--ALTER INDEX [shurfy_shape_idx] ON [dbo].[shurfy] REBUILD PARTITION = ALL WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON)
--GO

select distinct top 2147483647
    t.obj_id as id,
    t.data_utverzhdenija_akta,
    IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Наименование начального узла',
    ec1.name as 'Код расчетной схемы начального узла',
    IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Наименование конечного узла',
    ec2.name as 'Код расчетной схемы конечного узла',
    es.name as 'Признак участка трубопровода',
    hpss.firstPICdateHP as 'Дата первичного ввода в эксплуатацию',
    hpss.lastTransDate as 'Дата последней перекладки',
    --hpss.netType as 'Вид сети',
    magistrali.naimenovanie_magistrali as 'Магистраль',
    IIF (pss.magistralSite is not NULL, re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Район эксплуатации',
    IIF (pss.magistralSite is not NULL, ue_ms.nomer_uchastka, ue_rs.nomer_uchastka) as 'Участок эксплуатации',
    IIF (pss.magistralSite is not NULL, ist_ms.naimenovanie, ist_rs.naimenovanie) as 'Источник теплоснабжения',
    IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as 'Наименование участка',
    IIF (pss.magistralSite is not NULL, 'магистраль', 'распредсеть') as 'Признак сети',
    t.organization as 'Владелец участка теплопровода',
    IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка',
    tubingTypes.name as 'Тип прокладки',
    hpss.diameterExternal as 'Диаметр наружный, мм',
    hpss.wallThickness AS 'Толщина стенки, мм',
    hpss.pipeSectLength AS 'Длина участка теплопровода, м',
    IIF (im.name is NULL, '—',im.name) as 'Материал тепловой изоляции',
    isolationTypes.name as 'Тип изоляции ППУ',
    IIF (externalMaterials.name is NULL, '—',externalMaterials.name) as 'Материал наружного покрытия',
    IIF (anticorrMaterials.name is NULL, '—',anticorrMaterials.name) as 'Материал антикоррозийного покрытия',
    IIF (n_vskr.name is NULL, '—',n_vskr.name) as 'Назначение вскрытия',
    CONCAT(st.name,' ',t.nomer_doma) as 'Адрес',
    sost_shurf.name as 'Состояние',
    t.data_nachala as 'Дата начала шурфовки',
    t.data_okonchaniya as 'Дата окончания шурфовки',
    t.rasstoyanie_do_blizhajshej_kamery as 'Расстояние до ближайшей камеры, м',
    IIF (node_bizhajshej_kamery.nodeName is NULL or node_bizhajshej_kamery.nodeName = '' or node_bizhajshej_kamery.nodeName = ' ',node_bizhajshej_kamery.externalNodeName, node_bizhajshej_kamery.nodeName) as 'Наименование ближайшей камеры',
    t.dlina_osmotra as 'Длина осмотра, м',
    t.glubina_zalozheniya as 'Глубина заложения прокладки, м',
    IIF (harakter_grunta_shurf.ground is NULL, '—',harakter_grunta_shurf.ground) as 'Грунт в месте прокладки',
    IIF (peshehod.name is NULL, '—',peshehod.name) as 'Прокладка трубопроводов под пешеходными зонами',
    IIF (tubingWays.name  is NULL, '—',tubingWays.name ) as 'Прокладка трубопроводов под авто и ж/д дорогами',
    IIF (LudiProklad.name is NULL, '—',LudiProklad.name) as 'Прокладка трубопровода в местах массового скопления жителей города',
    IIF (poverhnost_nad_trassoj.name is NULL, '—',poverhnost_nad_trassoj.name) as 'Поверхность над трассой',
    IIF (TransportElekricht.name is NULL, '—',TransportElekricht.name) as 'Наличие электрифицированного транспорта на расстоянии',
    IIF (Elektrich.name is NULL, '—',Elektrich.name) as 'Наличие эл.коммуникаций, электрохимзащиты газопровода',
    IIF (floodIntensitiesGW.name is NULL, '—',floodIntensitiesGW.name) as 'Интенсивность подтопления грунтовыми водами',
    floodIntensitiesFW.name as 'Интенсивность подтопления паводковыми водами',
    floodIntensitiesTW.name as 'Интенсивность подтопления водопроводными водами',
    floodIntensitiesBW.name as 'Интенсивность подтопления фекальными водами',
    UzherbLudi.name as 'Нанесение ущерба населению от подтопления при возможном повреждении трубопроводов',
    UzherbSity.name as 'Нанесение ущерба инфраструктуре города  при возможном повреждении трубопроводов',
    'Вблизи лежащие коммуникации' = 
        STUFF(
        ( SELECT concat(',', rtt2.name) AS n
        FROM shurf r2
            LEFT JOIN nalichie_vblizi_kommunikacij_for_shurfy rt2 ON r2.id=rt2.objID
            LEFT JOIN nalichie_vblizi_kommunikacij rtt2 ON rtt2.id=rt2.activityID
        WHERE r2.id=t.obj_id
        FOR XML PATH('')
        , TYPE
        ).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
    
    t.nomer_akta as 'Номер акта',
    'Осмотрены элементы' = 
        STUFF(
        ( SELECT concat(',', rtt2.name) AS n
        FROM shurf r2
            LEFT JOIN vidy_elementov_for_shurfy rt2 ON r2.id=rt2.objID
            LEFT JOIN vidy_elementov_shurf rtt2 ON rtt2.id=rt2.activityID
        WHERE r2.id=t.obj_id
        FOR XML PATH('')
        , TYPE
        ).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
    IIF (stroitelnye_konstrukcii_kanala.name is NULL, '—',stroitelnye_konstrukcii_kanala.name) as 'Строительные конструкции Канала',
    IIF (sostoyanie_stroitelnyh_konstrukcij_kanala.name is NULL, '—',sostoyanie_stroitelnyh_konstrukcij_kanala.name) as 'Состояние строительных конструкций канала',
    IIF (vnutrennee_sostoyanie_kanala.name is NULL, '—',vnutrennee_sostoyanie_kanala.name) as 'Внутреннее состояние канала',
    IIF (yn_podtoplenie_do_truby.name is NULL, '—',yn_podtoplenie_do_truby.name) as 'Подтопление до трубы',
    zanos_kanala_gruntom.name as 'Занос канала грунтом',
    IIF (konstrukciya_drenazhnogo_ustrojstva.name is NULL, '—',konstrukciya_drenazhnogo_ustrojstva.name) as 'Конструкция дренажного устройства',
    IIF (sectWaterDumps.name is NULL, '—',sectWaterDumps.name) as 'Водоотведение на участке',
    IIF (sostoyanie_teplovoj_izolyacii_obratka.name is NULL, '—',sostoyanie_teplovoj_izolyacii_obratka.name) as 'Состояние тепловой изоляции (обратный трубопровод)',
    IIF (sostoyanie_teplovoj_izolyacii_podacha.name is NULL, '—',sostoyanie_teplovoj_izolyacii_podacha.name) as 'Состояние тепловой изоляции (подающий трубопровод)',
    IIF (sostoyanie_naruzhnogo_pokrytiya_obratka.name is NULL, '—',sostoyanie_naruzhnogo_pokrytiya_obratka.name) as 'Состояние наружного покрытия (обратный трубопровод)',
    IIF (sostoyanie_naruzhnogo_pokrytiya_podacha.name is NULL, '—',sostoyanie_naruzhnogo_pokrytiya_podacha.name) as 'Состояние наружного покрытия (подающий трубопровод)',
    IIF (sostoyanie_protivokorrozionnogo_pokrytiya_obratka.name is NULL, '—',sostoyanie_protivokorrozionnogo_pokrytiya_obratka.name) as 'Состояние противокоррозионного покрытия (обратный трубопровод)',
    IIF (sostoyanie_protivokorrozionnogo_pokrytiya_podacha.name is NULL, '—',sostoyanie_protivokorrozionnogo_pokrytiya_podacha.name) as 'Состояние противокоррозионного покрытия (подающий трубопровод)',
    IIF (nalichie_korrozii_obratka.name is NULL, '—',nalichie_korrozii_obratka.name) as 'Наличие коррозии (обратный трубопровод)',
    IIF (nalichie_korrozii_podacha.name is NULL, '—',nalichie_korrozii_podacha.name) as 'Наличие коррозии (подающий трубопровод)',
    IIF (t.predpolagaemye_prichiny_razrusheniya_izolyacii is NULL, '—',t.predpolagaemye_prichiny_razrusheniya_izolyacii) as 'Предполагаемые причины разрушения изоляции, коррозии',
    IIF (t.rezultaty_osmotra is NULL, '—',t.rezultaty_osmotra) as 'Результаты осмотра',
    IIF (t.namechennye_meropriyatiya is NULL, '—',t.namechennye_meropriyatiya) as 'Намеченные мероприятия',
    IIF (t.meropriyatiya_po_vosstanovleniyu_prokladki is NULL, '—',t.meropriyatiya_po_vosstanovleniyu_prokladki) as 'Мероприятия по восстановлению прокладки',
    t.primechanie as 'Примечание',
    t.fio_utverzhdaemogo as 'ФИО утверждающего',
    dolz.znachenie as 'Должность утверждающего',
    subd.name as 'Служба утверждающего',
    t.fio_1 as 'ФИО 1 члена',
    dolz_f1.znachenie as 'Должность 1 члена',
    t.fio_2 as 'ФИО 2 члена',
    dolz_f2.znachenie as 'Должность 2 члена',
    fio_viziruemogo_1 as 'ФИО визирующего',
    dolz_1.znachenie as 'Должность визирующего',

    f7.name as 'Степень внешней коррозии',
    f8.name as 'Степень внутренней коррозии',
    sostoyanie_metalla_truboprovoda.name AS 'Состояние металла трубопровода'


from(
    select distinct
        l.lineID,
        organizations.name as 'organization',
        l.externalSignLineID,
        d.id as obj_id,
        d.data_utverzhdenija_akta as data_utverzhdenija_akta,
        d.materialy_i_mekhanizmyID,
        d.data_utverzhdeniya_plana_shurfovok,
        d.naznachenie_vskrID,
        d.ulicaID,
        d.nomer_doma,
        d.sostoyanie_shurfaID,
        d.data_nachala_plan,
        d.data_okonchaniya_plan,
        d.data_nachala,
        d.data_okonchaniya,
        d.rasstoyanie_do_blizhajshej_kamery,
        d.nodeID_bizhajshej_kamery,
        d.dlina_osmotra,
        d.glubina_zalozheniya,
        d.nomer_akta,
        d.predpolagaemye_prichiny_razrusheniya_izolyacii,
        d.rezultaty_osmotra,
        d.namechennye_meropriyatiya,
        d.meropriyatiya_po_vosstanovleniyu_prokladki,
        d.primechanie,
        d.fio_utverzhdaemogo,
        d.dolzhnost_utverzhdaemogoID,
        d.sluzhba_utverzhdaemogoID,
        d.fio_1,
        d.dolzhnost_1,
        d.fio_2,
        d.dolzhnost_2,
        d.fio_viziruemogo_1,
        d.dolzhnost_viziruemogoID_1
    from shurfy d
        JOIN (
            select 
                k.lineID,
                k.externalSignLineID,
                k.obj_id,
                k.organizationID
            from (
                select
                    distinct
                        l.id as lineID,
                        d.id as obj_id,
                        l.externalSignLineID,
                        l.organizationID,
                        l.shape.STDistance(d.shape) as length,
                        MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len"
                from shurfy d
                JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1 )
                where d.id = $fileID$
            )k
        where k.min_len = k.length 
    ) l on l.obj_id = d.id 
    left join organizations on organizations.id = l.organizationID
)t
    left JOIN heatPipeSections hpss ON hpss.lineID=t.lineID
    left JOIN pipeSections pss ON pss.id = hpss.pipeSectionID
    left JOIN nodes n1 ON ( n1.id = pss.nodeID1 and n1.removed = 0 )
    LEFT JOIN nodes n2 ON n2.id = pss.nodeID2
    left join externalCodes ec1 ON ec1.id = n1.externalCodeID
    left join externalCodes ec2 ON ec2.id = n2.externalCodeID
    left join externalSigns  es on es.id = t.externalSignLineID
    left join nodes node_bizhajshej_kamery on node_bizhajshej_kamery.id = t.nodeID_bizhajshej_kamery

    left join isolMaterials im on im.id = hpss.isolMaterialID
    LEFT JOIN tubingTypes ON tubingTypes.id=hpss.tubingTypeID
    left join isolationTypes on isolationTypes.id = hpss.isolationTypeID
    left join externalMaterials on externalMaterials.id = hpss.externMaterialID
    left join anticorrMaterials on anticorrMaterials.id = hpss.anticorrMaterialID
    left join magistrali on magistrali.id = hpss.magistral

    left join uchastok_ms ms ON ms.id = pss.magistralSite
    left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka

    left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
    left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka

    left join uchastok_rs rs ON rs.id = pss.distSite
    left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka

    left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
    left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka

    LEFT JOIN istochniki_tepla ist_ms ON ist_ms.id=ue_ms.istochnik_tepla
    LEFT JOIN istochniki_tepla ist_rs ON ist_rs.id=ue_rs.istochnik_tepla

    left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = pss.id and faktory_riska_truboprovoda.objID = t.obj_id and faktory_riska_truboprovoda.obj_type_faktory_riskaID = 1

    LEFT JOIN faktRiska_7_vneshkorroz f7 ON f7.id=faktory_riska_truboprovoda.VnesnKorrozia
    LEFT JOIN faktRiska_8_vnutkorroz f8 ON f8.id=faktory_riska_truboprovoda.VnunrenKorrozia
    left join sostoyanie_metalla_truboprovoda on sostoyanie_metalla_truboprovoda.id = faktory_riska_truboprovoda.sostoyanie_metalla_truboprovodaID


    LEFT JOIN faktRiska_15_peshehod peshehod on peshehod.id = faktory_riska_truboprovoda.Pesehod
    LEFT JOIN tubingWays on tubingWays.id = faktory_riska_truboprovoda.ZhdDorogi
    LEFT JOIN faktRiska_17_skoplenie_zhit LudiProklad on LudiProklad.id = faktory_riska_truboprovoda.LudiProklad
    left join poverhnost_nad_trassoj on poverhnost_nad_trassoj.id = faktory_riska_truboprovoda.poverhnost_nad_trassojID
    LEFT JOIN faktRiska_5_transport TransportElekricht on TransportElekricht.id = faktory_riska_truboprovoda.TransportElekricht
    LEFT JOIN faktRiska_4_elekt Elektrich on Elektrich.id = faktory_riska_truboprovoda.Elektrich

    LEFT JOIN floodIntensitiesGW on floodIntensitiesGW.id = faktory_riska_truboprovoda.floodIntensityGWid
    LEFT JOIN floodIntensitiesFW on floodIntensitiesFW.id = faktory_riska_truboprovoda.floodIntensityFWid
    LEFT JOIN floodIntensitiesTW on floodIntensitiesTW.id = faktory_riska_truboprovoda.floodIntensityTWid
    LEFT JOIN floodIntensitiesBW on floodIntensitiesBW.id = faktory_riska_truboprovoda.floodIntensityBWid

    LEFT JOIN populDamages UzherbLudi on UzherbLudi.id = faktory_riska_truboprovoda.UzherbLudi
    LEFT JOIN infrastrDamages UzherbSity on UzherbSity.id = faktory_riska_truboprovoda.UzherbSity
    

    left join stroitelnye_konstrukcii_kanala on stroitelnye_konstrukcii_kanala.id = faktory_riska_truboprovoda.stroitelnye_konstrukcii_kanalaID
    left join sostoyanie_stroitelnyh_konstrukcij_kanala on sostoyanie_stroitelnyh_konstrukcij_kanala.id = faktory_riska_truboprovoda.sostoyanie_stroitelnyh_konstrukcij_kanalaID
    left join vnutrennee_sostoyanie_kanala on vnutrennee_sostoyanie_kanala.id = faktory_riska_truboprovoda.vnutrennee_sostoyanie_kanalaID
    left join YesOrNo yn_podtoplenie_do_truby on yn_podtoplenie_do_truby.id = faktory_riska_truboprovoda.podtoplenie_do_truby
    left join zanos_kanala_gruntom on zanos_kanala_gruntom.id = faktory_riska_truboprovoda.zanos_kanala_gruntomID
    left join konstrukciya_drenazhnogo_ustrojstva on konstrukciya_drenazhnogo_ustrojstva.id = faktory_riska_truboprovoda.konstrukciya_drenazhnogo_ustrojstvaID
    LEFT JOIN sectWaterDumps on sectWaterDumps.id = faktory_riska_truboprovoda.Vodootved
    left join sostoyanie_teplovoj_izolyacii sostoyanie_teplovoj_izolyacii_obratka on sostoyanie_teplovoj_izolyacii_obratka.id = faktory_riska_truboprovoda.sostoyanie_teplovoj_izolyacii_obratkaID
    left join sostoyanie_teplovoj_izolyacii sostoyanie_teplovoj_izolyacii_podacha on sostoyanie_teplovoj_izolyacii_podacha.id = faktory_riska_truboprovoda.sostoyanie_teplovoj_izolyacii_podachaID
    left join sostoyanie_naruzhnogo_pokrytiya sostoyanie_naruzhnogo_pokrytiya_obratka on sostoyanie_naruzhnogo_pokrytiya_obratka.id = faktory_riska_truboprovoda.sostoyanie_naruzhnogo_pokrytiya_obratkaID
    left join sostoyanie_naruzhnogo_pokrytiya sostoyanie_naruzhnogo_pokrytiya_podacha on sostoyanie_naruzhnogo_pokrytiya_podacha.id = faktory_riska_truboprovoda.sostoyanie_naruzhnogo_pokrytiya_podachaID
    left join sostoyanie_protivokorrozionnogo_pokrytiya_shurf sostoyanie_protivokorrozionnogo_pokrytiya_podacha on sostoyanie_protivokorrozionnogo_pokrytiya_podacha.id = faktory_riska_truboprovoda.sostoyanie_protivokorrozionnogo_pokrytiya_podachaID
    left join sostoyanie_protivokorrozionnogo_pokrytiya_shurf sostoyanie_protivokorrozionnogo_pokrytiya_obratka on sostoyanie_protivokorrozionnogo_pokrytiya_obratka.id = faktory_riska_truboprovoda.sostoyanie_protivokorrozionnogo_pokrytiya_obratkaID
    left join nalichie_korrozii_shurf nalichie_korrozii_podacha on nalichie_korrozii_podacha.id = faktory_riska_truboprovoda.nalichie_korrozii_podachaID
    left join nalichie_korrozii_shurf nalichie_korrozii_obratka on nalichie_korrozii_obratka.id = faktory_riska_truboprovoda.nalichie_korrozii_obratkaID

    left join ulitsy st ON st.id = t.ulicaID
    left join materialy_i_mekhanizmy m ON m.id = t.materialy_i_mekhanizmyID
    left join naznachenie_vskr n_vskr ON n_vskr.id = t.naznachenie_vskrID
    left join sostoyanie_shurfa sost_shurf on sost_shurf.id = t.sostoyanie_shurfaID
    left join sprav.dbo.[01_teploprovodnost_grunta] harakter_grunta_shurf  on harakter_grunta_shurf.id = faktory_riska_truboprovoda.harakter_gruntaID
    
    LEFT JOIN dolzhnosti dolz ON dolz.id=t.dolzhnost_utverzhdaemogoID
    LEFT JOIN dolzhnosti dolz_1 ON dolz_1.id=t.dolzhnost_viziruemogoID_1
    LEFT JOIN dolzhnosti dolz_f1 ON dolz_f1.id=t.dolzhnost_1
    LEFT JOIN dolzhnosti dolz_f2 ON dolz_f2.id=t.dolzhnost_2
    LEFT JOIN subdivisions subd ON subd.id=t.sluzhba_utverzhdaemogoID

