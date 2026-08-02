IF OBJECT_ID('tempdb..##defect_doc') IS NOT NULL DROP TABLE ##defect_doc;

SELECT 
    *
INTO ##defect_doc
FROM (
    SELECT DISTINCT
        l.id AS lineID,
        d.id AS obj_id,
            d.remontTypeID,
d.data_osmotra,
d.vremya_osmotra,
d.vid_rabotID,
d.vid_narusheniyaID,
d.nomer_doma,
d.ulicaID,
d.priznak_truboprovoda,
d.rasstoyanieDoPovrezhdeniyaNachKamery,
d.tipPoverhnostiID,
d.dreniruemyj_truboprovodID,
d.defectDescription,
d.opresID,
d.data_sostavleniya_akta,
d.nomer_akta,
d.nomer_prikaza,
d.data_prikaza_vvoda_v_ekspluataciyu,
d.prikaz_vvoda_v_ekspluataciyu,
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
d.remont_kameryID,
        l.shape.STDistance(d.shape) AS length,
        MIN(l.shape.STDistance(d.shape)) OVER (PARTITION BY d.id) AS min_len

    FROM defect d
    left JOIN linesobj l ON l.removed = 0 AND l.shape.STDistance(d.shape) < 0.1
    left JOIN nodes n1 ON n1.id = l.nodeID1 AND n1.removed = 0 $and_n1_fileID$
    WHERE d.vid_narusheniyaID IS NOT NULL OR d.vid_rabotID IS NOT NULL
    AND (
     (d.data_osmotra $season_condition$)
    )
) k
WHERE k.min_len = k.length;

SELECT top 2147483647 * 
FROM (
select DISTINCT top 2147483647
        t.obj_id as id,
        pss.id as pss_id,
        IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Наименование начального узла',
        IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Наименование конечного узла',
        pss.DiamUslov as 'Диаметр',
        pss.pipeLength AS 'Длина участка теплопровода, м',
        im.name as 'Материал тепловой изоляции',
        isolationTypes.name as 'Тип изоляции ППУ',
        tubingTypes.name as 'Тип прокладки',
        pss.firstPICdateHP as 'Дата первичного ввода в эксплуатацию',
        YEAR(t.data_osmotra) as 'Год эксплуатации',
        --t.data_osmotra,
        t.vid_rabotID as 'Вид работ',
        t.vid_narusheniyaID as 'Вид нарушения',
        'Наработка основного оборудования' = 
        CASE
            when (pss.lastTransDate is not null) and t.data_osmotra is not null  
                then DATEDIFF (Hh , pss.lastTransDate, MIN(t.data_osmotra) OVER(PARTITION BY pss.id, YEAR(t.data_osmotra) ) )

            when pss.firstPICdateHP is not null and t.data_osmotra is not null then DATEDIFF (Hh , pss.firstPICdateHP, MIN(t.data_osmotra) OVER(PARTITION BY pss.id, YEAR(t.data_osmotra) ) )
        END,
        t.nomer_akta as 'Номер акта',
        t.data_sostavleniya_akta as 'Дата составления акта',
        t.nomer_prikaza as 'Номер приказа',
        t.data_prikaza_vvoda_v_ekspluataciyu as 'Дата приказа ввода в эксплуатацию',
        IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as 'Наименование участка',
        IIF (pss.magistralSite is not NULL, 'магистраль', 'распредсеть') as 'Признак сети',
        IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка'
    from ##defect_doc t
    LEFT JOIN heatPipeSections hpss ON hpss.lineID=t.lineID  $and_condition2$
    LEFT JOIN pipeSections pss ON pss.id = hpss.pipeSectionID
    JOIN nodes n1 ON ( n1.id = pss.nodeID1 and n1.removed = 0)
    LEFT JOIN nodes n2 ON n2.id = pss.nodeID2
    left join externalCodes ec1 ON ec1.id = n1.externalCodeID
    left join externalCodes ec2 ON ec2.id = n2.externalCodeID

    LEFT JOIN defectTypes deft ON deft.id = t.remontTypeID
    left join stateDefect on stateDefect.id = t.stateID
    left join externalSigns  es on es.id = t.priznak_truboprovoda 
    left join vid_narusheniya vn on vn.id = t.vid_narusheniyaID
    left join vid_rabot on vid_rabot.id = t.vid_rabotID

    left join isolMaterials im on im.id = pss.isolMaterialID
    LEFT JOIN tubingTypes ON tubingTypes.id=pss.tubingTypeID
    left join isolationTypes on isolationTypes.id = pss.isolationTypeID
    left join externalMaterials on externalMaterials.id = pss.externMaterialID
    left join anticorrMaterials on anticorrMaterials.id = pss.anticorrMaterialID

    left join uchastok_ms ms ON ms.id = pss.magistralSite
    left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka

    left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii


    left join uchastok_rs rs ON rs.id = pss.distSite
    left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka

    left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii

    left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
    left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka

    WHERE 
--        n1.fileID in (58) and
        ( (not ec1.name in ('П1','П2') or not ec2.name in ('П1','П2')) or (ec1.name is null AND ec2.name is null) ) 
        $and_n1_fileID$
 
) __ 
-- WHERE  [Наработка основного оборудования] >= 0

order by pss_id, [Год эксплуатации]
