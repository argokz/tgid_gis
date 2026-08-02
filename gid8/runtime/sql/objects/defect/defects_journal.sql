IF OBJECT_ID('tempdb..##InitialJoin') IS NOT NULL DROP TABLE ##InitialJoin;

SELECT
    DISTINCT
    l.lineID,
--    organizations.name as 'organization',
    d.id as obj_id,
    d.remontTypeID,
    d.data_osmotra,
    d.vremya_osmotra,
    d.nomer_doma,
    d.ulicaID,
    d.priznak_truboprovoda,
    d.nodeID_bizhajshej_kamery,
    d.rasstoyanieDoPovrezhdeniyaNachKamery,
    d.tipPoverhnostiID,
    d.dreniruemyj_truboprovodID,
    d.defectDescription,
    d.opresID,
    d.data_sostavleniya_akta,
    d.nomer_akta,
    d.nomer_prikaza,
    d.data_prikaza_vvoda_v_ekspluataciyu,
    d.vid_narusheniyaID,
    d.vid_rabotID,
    d.remontCatID,
    d.stateID,
    d.sostTeploizolID,
    d.sostKonstruktsiiKameryID,
    d.sostKonstruktsiiKanalaID,
    d.sostNaruzhnoiPoverkhnostiID,
    d.sostVnutrenneiPoverkhnostiID,
    d.nodeID1,
    d.nodeID2,
    d.tipPovrezhdeniaID,
    d.tsentrPovrezhdenia,
    d.vysotaPovrezhdenia,
    d.shirinaPovrezhdenia,
    d.ploshchadPovrezhdenia,
    d.meropriyatiya,
    d.data_shurfovki,
    d.data_nachala_remonta,
    d.vremiaNachalaRemonta,
    d.data_zaversheniya_remonta,
    d.vremiaZaversheniaRemonta,
    d.trudozatratyNaRemont,
    d.stoimostRemonta,
    d.otchet_po_defektu,
    d.kolichestvo_otklyuchennyh_potrebitelej,
    d.kolichestvo_nedootpushchennoj_teplovoj_energii,
    d.zatraty_na_vosstanovlenie,
    d.inye_socialnye_posledstviya,
    d.shirinaZaplatki,
    d.vysotaZaplatki,
    d.len_tube_cur,
    d.remontIzolKonstruktsiiID,
    d.len_izol_cur,
    d.remont_kanalaID,
    d.len_channel_cur,
    d.remont_kameryID
INTO ##InitialJoin
FROM defect d
JOIN (
    SELECT 
        k.lineID,
        k.obj_id
---        k.organizationID
    FROM (
        SELECT
            DISTINCT
                l.id as lineID,
                d.id as obj_id,
                d.stateID,
                d.data_osmotra,
--                l.organizationID,
                l.shape.STDistance(d.shape) as length,
                MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len"
        FROM defect d
        LEFT JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1 )
        LEFT JOIN nodes n1 ON n1.id=l.nodeID1 AND n1.removed=0 $and_n1_fileID$
       where   
       $state_cond$
       (
       (d.data_osmotra $season_condition$)
       )
    ) k
    WHERE k.min_len = k.length
) l on l.obj_id = d.id;



-- Основной запрос с использованием временной таблицы:
    SELECT TOP 2147483647 *
    FROM (
        SELECT DISTINCT TOP 2147483647
            t.obj_id as id,
            IIF(n1.nodeName IS NULL OR n1.nodeName = '' OR n1.nodeName = ' ', n1.externalNodeName, n1.nodeName) as 'Наименование начального узла',
            ec1.name as 'Код расчетной схемы начального узла',
            IIF(n2.nodeName IS NULL OR n2.nodeName = '' OR n2.nodeName = ' ', n2.externalNodeName, n2.nodeName) as 'Наименование конечного узла',
            ec2.name as 'Код расчетной схемы конечного узла',
            es.name as 'Признак участка трубопровода',
            pss.firstPICdateHP as 'Дата первичного ввода в эксплуатацию',
            pss.lastTransDate as 'Дата последней перекладки',
            magistrali.naimenovanie_magistrali as 'Магистраль',
            IIF(pss.magistralSite IS NOT NULL, re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Район эксплуатации',
            IIF(pss.magistralSite IS NOT NULL, ue_ms.nomer_uchastka, ue_rs.nomer_uchastka) as 'Участок эксплуатации',
            IIF(pss.magistralSite IS NOT NULL, ist_ms.naimenovanie, ist_rs.naimenovanie) as 'Источник теплоснабжения',
            IIF(ms.opisanie_uchastka_ms IS NOT NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as 'Наименование участка',
            IIF(pss.magistralSite IS NOT NULL, 'магистраль', 'распредсеть') as 'Признак сети',
--            t.organization as 'Владелец участка теплопровода',
            IIF(nu_ms.fio IS NOT NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка',
            tubingTypes.name as 'Тип прокладки',
--            hpss.diameterExternal as 'Диаметр наружный, мм',
            pss.DiamUslov as 'Диаметр условный, мм',
--            hpss.wallThickness as 'Толщина стенки, мм',
--            hpss.pipeSectLength as 'Длина участка теплопровода, м',
            pss.pipeLength as 'Длина участка теплопровода, м',
            im.name as 'Материал тепловой изоляции',
            isolationTypes.name as 'Тип изоляции ППУ',
            externalMaterials.name as 'Материал наружного покрытия',
            anticorrMaterials.name as 'Материал антикоррозийного покрытия',
            deft.name as 'Режим',
            FORMAT(t.data_osmotra,'dd.MM.yyyy') as 'Дата обнаружения нарушения',
            t.vremya_osmotra as 'Время обнаружения повреждения',
            CONCAT(st.name, '', t.nomer_doma) as 'Адрес',
            es.name as 'Поврежденный трубопровод',
            t.rasstoyanieDoPovrezhdeniyaNachKamery as 'Расстояние до нарушения от ближайшей камеры, м',
            IIF(node_bizhajshej_kamery.nodeName IS NULL OR node_bizhajshej_kamery.nodeName = '' OR node_bizhajshej_kamery.nodeName = ' ', node_bizhajshej_kamery.externalNodeName, node_bizhajshej_kamery.nodeName) as 'Наименование ближайшей камеры',
            tp.name as 'Поверхность в месте раскопки',
            t.defectDescription as 'Описание повреждения',
            op.name as 'Наименование контура опрессовки',
            t.nomer_akta as 'Номер акта',
            t.data_sostavleniya_akta as 'Дата составления акта',
            t.nomer_prikaza as 'Номер приказа',
            t.data_prikaza_vvoda_v_ekspluataciyu as 'Дата приказа ввода в эксплуатацию',
            vn.code as 'Код нарушения',
            vn.name as 'Вид нарушения',
            vid_rabot.name as 'Вид работ',
            rc.name as 'Категория нарушения',
            tpov.name as 'Характер повреждения',
            t.tsentrPovrezhdenia as 'Место расположения центра повреждения на трубопроводе, часов',
            t.vysotaPovrezhdenia as 'Высота повреждения',
            t.shirinaPovrezhdenia as 'Ширина повреждения',
            t.ploshchadPovrezhdenia as 'Площадь повреждения',
            sostT.name as 'Состояние теплоизоляции теплопроводов',
            sostNP.name as 'Состояние наружной поверхности теплопроводов',
            sostVP.name as 'Состояние внутренней поверхности поврежденной трубы',
            stateDefect.name as 'Состояние',
            IIF(uzel_nach_otcl.nodeName IS NULL OR uzel_nach_otcl.nodeName = '' OR uzel_nach_otcl.nodeName = ' ', uzel_nach_otcl.externalNodeName, uzel_nach_otcl.nodeName) as 'Начальная камера отключения от сети',
            IIF(uzel_konec_otcl.nodeName IS NULL OR uzel_konec_otcl.nodeName = '' OR uzel_konec_otcl.nodeName = ' ', uzel_konec_otcl.externalNodeName, uzel_konec_otcl.nodeName) as 'Конечная камера отключения от сети',
            t.meropriyatiya as 'Способ ликвидации нарушения',
            t.data_nachala_remonta as 'Дата начала ремонтных работ',
            t.vremiaNachalaRemonta as 'Время начала ремонтных работ',
            t.data_zaversheniya_remonta as 'Дата завершения ремонтных работ',
            t.vremiaZaversheniaRemonta as 'Время завершения ремонтных работ',
            t.trudozatratyNaRemont as 'Трудозатраты на ремонт',
            t.stoimostRemonta as 'Стоимость ремонтных работ, тг',
            t.otchet_po_defektu as 'Примечание',
            t.kolichestvo_otklyuchennyh_potrebitelej as 'Количество отключенных потребителей',
            t.kolichestvo_nedootpushchennoj_teplovoj_energii as 'Количество недоотпущенной тепловой энергии, Гкал',
            t.zatraty_na_vosstanovlenie as 'Затраты на восстановление, тг',
            t.inye_socialnye_posledstviya as 'Иные социальные последствия',




            t.shirinaZaplatki as 'Ширина заплатки',
            t.vysotaZaplatki as 'Высота заплатки',
            t.len_tube_cur as 'Длина заменённой трубы, м',
            remontIzolKonstruktsii.name as 'Ремонт теплоизоляционной конструкции',
            t.len_izol_cur as 'Длина заменённой изоляции, м',
            remont_kanala.name as 'Ремонт канала',
            t.len_channel_cur as 'Длина участка ремонта канала',
            remont_kamery.name as 'Ремонт камеры'
        FROM ##InitialJoin t
        JOIN heatPipeSections hpss ON hpss.lineID=t.lineID $and_condition2$ 
        LEFT JOIN pipeSections pss ON pss.id = hpss.pipeSectionID
        LEFT JOIN nodes n1 ON (n1.id = pss.nodeID1 and n1.removed = 0 and n1.internalNodeID IS NULL)
        LEFT JOIN nodes n2 ON n2.id = pss.nodeID2
        LEFT JOIN nodes uzel_nach_otcl on uzel_nach_otcl.id = t.nodeID1 
        LEFT JOIN nodes uzel_konec_otcl on uzel_konec_otcl.id = t.nodeID2 
        LEFT JOIN externalCodes ec1 ON ec1.id = n1.externalCodeID
        LEFT JOIN externalCodes ec2 ON ec2.id = n2.externalCodeID
        LEFT JOIN nodes node_bizhajshej_kamery on node_bizhajshej_kamery.id = t.nodeID_bizhajshej_kamery
        LEFT JOIN defectTypes deft ON deft.id = t.remontTypeID
        LEFT JOIN stateDefect on stateDefect.id = t.stateID
        LEFT JOIN tipPoverhnosti tp on tp.id = t.tipPoverhnostiID
        LEFT JOIN externalSigns es on es.id = t.priznak_truboprovoda 
        LEFT JOIN vid_narusheniya vn on vn.id = t.vid_narusheniyaID
        LEFT JOIN vid_rabot on vid_rabot.id = t.vid_rabotID
        LEFT JOIN remontCat rc ON rc.id = t.remontCatID
        LEFT JOIN remontIzolKonstruktsii on remontIzolKonstruktsii.id = t.remontIzolKonstruktsiiID
        LEFT JOIN ulitsy st ON st.id = t.ulicaID
        LEFT JOIN opres op ON op.id = t.opresID
        LEFT JOIN tipPovrezhdenia tpov ON tpov.id = t.tipPovrezhdeniaID
        LEFT JOIN sostTeploizol sostT ON sostT.id = t.sostTeploizolID
        LEFT JOIN sostNaruzhnoiPoverkhnosti sostNP ON sostNP.id = t.sostNaruzhnoiPoverkhnostiID
        LEFT JOIN sostVnutrenneiPoverkhnosti sostVP ON sostVP.id = t.sostVnutrenneiPoverkhnostiID
        LEFT JOIN remont_kanala on remont_kanala.id = t.remont_kanalaID
        LEFT JOIN remont_kamery on remont_kamery.id = t.remont_kameryID
        LEFT JOIN isolMaterials im on im.id = hpss.isolMaterialID
        LEFT JOIN tubingTypes ON tubingTypes.id=hpss.tubingTypeID
        LEFT JOIN isolationTypes on isolationTypes.id = hpss.isolationTypeID
        LEFT JOIN externalMaterials on externalMaterials.id = hpss.externMaterialID
        LEFT JOIN anticorrMaterials on anticorrMaterials.id = hpss.anticorrMaterialID
        LEFT JOIN magistrali on magistrali.id = hpss.magistral
        LEFT JOIN uchastok_ms ms ON ms.id = pss.magistralSite
        LEFT JOIN uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
        LEFT JOIN rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
        LEFT JOIN nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
        LEFT JOIN uchastok_rs rs ON rs.id = pss.distSite
        LEFT JOIN uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
        LEFT JOIN rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
        LEFT JOIN nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
        LEFT JOIN istochniki_tepla ist_ms ON ist_ms.id=ue_ms.istochnik_tepla
        LEFT JOIN istochniki_tepla ist_rs ON ist_rs.id=ue_rs.istochnik_tepla
        WHERE 
            ((NOT ec1.name IN ('П1', 'П2') OR NOT ec2.name IN ('П1', 'П2')) OR (ec1.name IS NULL AND ec2.name IS NULL)) 
--            AND n1.fileID in (58)
            $and_n1_fileID$
    ) _
    ORDER BY 
    [Начальник участка],
    [Наименование участка],
    [Дата обнаружения нарушения] DESC
