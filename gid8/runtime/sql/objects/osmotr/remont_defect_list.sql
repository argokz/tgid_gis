select
    top 2147483647 t.obj_id as id,
    IIF (
        n1.nodeName is NULL
        or n1.nodeName = ''
        or n1.nodeName = ' ',
        n1.externalNodeName,
        n1.nodeName
    ) as 'Наименование начального узла',
    
    IIF (
        n2.nodeName is NULL
        or n2.nodeName = ''
        or n2.nodeName = ' ',
        n2.externalNodeName,
        n2.nodeName
    ) as 'Наименование конечного узла',

    es.name as 'Признак участка трубопровода',
    IIF (
        ms.opisanie_uchastka_ms is not NULL,
        ms.opisanie_uchastka_ms,
        rs.naimenovanie_uchastka_rs
    ) as 'Наименование участка',
    IIF (
        pss.magistralSite is not NULL,
        'магистраль',
        'распредсеть'
    ) as 'Признак сети',
    t.organization as 'Владелец участка теплопровода',
    IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка',
    tubingTypes.name as 'Тип прокладки',
    hpss.diameterExternal as 'Диаметр наружный, мм',
    deft.name as 'Режим',
    FORMAT(t.data_osmotra, 'dd.MM.yyyy') as 'Дата обнаружения нарушения',
    t.vremya_osmotra as 'Время обнаружения повреждения',
    CONCAT(st.name, '', t.nomer_doma) as 'Адрес',
    t.defectDescription as 'Описание повреждения',
    t.nomer_akta as 'Номер акта',
    t.data_sostavleniya_akta as 'Дата составления акта',
    t.nomer_prikaza as 'Номер приказа',
    t.data_prikaza_vvoda_v_ekspluataciyu as 'Дата приказа ввода в эксплуатацию',
    vn.name as 'Вид нарушения',
    rc.name as 'Категория нарушения',
    tpov.name as 'Характер повреждения',
    t.meropriyatiya as 'Способ ликвидации нарушения',
    t.data_nachala_remonta as 'Дата начала ремонтных работ',
    t.vremiaNachalaRemonta as 'Время начала ремонтных работ',
    t.data_zaversheniya_remonta as 'Дата завершения ремонтных работ',
    t.vremiaZaversheniaRemonta as 'Время завершения  ремонтных работ',
    t.trudozatratyNaRemont as 'Трудозатраты на ремонт',
    t.stoimostRemonta as 'Стоимость ремонтных работ, тг',
    t.otchet_po_defektu as 'Примечание',
    t.kolichestvo_otklyuchennyh_potrebitelej as 'Количество отключенных потребителей',
    t.kolichestvo_nedootpushchennoj_teplovoj_energii as 'Количество недоотпущенной тепловой энергии, Гкал',
    t.zatraty_na_vosstanovlenie as 'Затраты на восстановление, тг',
    t.inye_socialnye_posledstviya as 'Иные социальные последствия'
from
    (
        select
            distinct l.lineID,
            organizations.name as 'organization',
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
            d.remontCatID,
            d.stateID,
            d.sostTeploizolID,
            d.sostKonstruktsiiKameryID,
            d.sostKonstruktsiiKanalaID,
            d.sostNaruzhnoiPoverkhnostiID,
            d.sostVnutrenneiPoverkhnostiID,
            d.vid_rabotID,
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
        from
            defect d
            JOIN (
                select distinct
                    k.lineID,
                    k.obj_id,
                    k.organizationID
                from
                    (
                        select
                            distinct l.id as lineID,
                            d.id as obj_id,
                            d.stateID,
                            l.organizationID,
                            l.shape.STDistance(d.shape) as length,
                            MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id) AS "min_len"
                        from
                            defect d
                            JOIN linesobj l ON (
                                l.removed = 0
                                and l.shape.STDistance(d.shape) < 0.1
                            )
                            where d.remontID = $id$
                    ) k
                where
                    k.min_len = k.length
            ) l on l.obj_id = d.id
            left join organizations on organizations.id = l.organizationID
    ) t
    LEFT JOIN heatPipeSections hpss ON hpss.lineID = t.lineID
    LEFT JOIN pipeSections pss ON pss.id = hpss.pipeSectionID
    JOIN nodes n1 ON (
        n1.id = pss.nodeID1
        and n1.removed = 0
    )
    LEFT JOIN nodes n2 ON n2.id = pss.nodeID2
    left join nodes uzel_nach_otcl on uzel_nach_otcl.id = t.nodeID1
    left join nodes uzel_konec_otcl on uzel_konec_otcl.id = t.nodeID2
    left join externalCodes ec1 ON ec1.id = n1.externalCodeID
    left join externalCodes ec2 ON ec2.id = n2.externalCodeID
    left join nodes node_bizhajshej_kamery on node_bizhajshej_kamery.id = t.nodeID_bizhajshej_kamery
    LEFT JOIN defectTypes deft ON deft.id = t.remontTypeID
    left join stateDefect on stateDefect.id = t.stateID
    left join tipPoverhnosti tp on tp.id = t.tipPoverhnostiID
    left join externalSigns es on es.id = t.priznak_truboprovoda
    left join vid_narusheniya vn on vn.id = t.vid_narusheniyaID
    left join remontCat rc ON rc.id = t.remontCatID
    left join remontIzolKonstruktsii on remontIzolKonstruktsii.id = t.remontIzolKonstruktsiiID
    left join ulitsy st ON st.id = t.ulicaID
    left join opres op ON op.id = t.opresID
    left join tipPovrezhdenia tpov ON tpov.id = t.tipPovrezhdeniaID
    left join sostTeploizol sostT ON sostT.id = t.sostTeploizolID
    left join sostNaruzhnoiPoverkhnosti sostNP ON sostNP.id = t.sostNaruzhnoiPoverkhnostiID
    left join sostVnutrenneiPoverkhnosti sostVP ON sostVP.id = t.sostVnutrenneiPoverkhnostiID
    left join remont_kanala on remont_kanala.id = t.remont_kanalaID
    left join remont_kamery on remont_kamery.id = t.remont_kameryID
    left join isolMaterials im on im.id = hpss.isolMaterialID
    LEFT JOIN tubingTypes ON tubingTypes.id = hpss.tubingTypeID
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
    LEFT JOIN istochniki_tepla ist_ms ON ist_ms.id = ue_ms.istochnik_tepla
    LEFT JOIN istochniki_tepla ist_rs ON ist_rs.id = ue_rs.istochnik_tepla
WHERE
    -- n1.fileID in (58) and
    (
        (
            not ec1.name in ('П1', 'П2')
            or not ec2.name in ('П1', 'П2')
        )
        or (
            ec1.name is null
            AND ec2.name is null
        )
    )
order by
    'Начальник участка',
    'Наименование участка',
    t.data_osmotra desc
