--ALTER INDEX [PK_Individual_ShurfyID] ON [dbo].[shurfy] REBUILD PARTITION = ALL WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON)
--ALTER INDEX [shurfy_shape_idx] ON [dbo].[shurfy] REBUILD PARTITION = ALL WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON)

select top 2147483647 *
FROM (

select distinct top 2147483647
--  t.obj_id as id,
  t.shurfID as id,
    t.data_nachala_shurf as 'Дата начала шурфовки',
    t.data_okonchaniya_shurf as 'Дата окончания шурфовки',
    CONCAT(st_shurf.name,' ',t.nomer_doma_shurf) as 'Адрес шурфовки',
    n_vskr_shurf.name as 'Назначение вскрытия',
    sost_shurf.name as 'Состояние Шурфа',
    IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Наименование начального узла',
    ec1.name as 'Код расчетной схемы начального узла',
    IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Наименование конечного узла',
    ec2.name as 'Код расчетной схемы конечного узла',
    es.name as 'Признак участка трубопровода',
    pss.firstPICdateHP as 'Дата первичного ввода в эксплуатацию',
    pss.lastTransDate as 'Дата последней перекладки',
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
    pss.diameterExternal as 'Диаметр наружный, мм',
    pss.wallThickness AS 'Толщина стенки, мм',
    pss.pipeSectLength AS 'Длина участка теплопровода, м',
    im.name as 'Материал тепловой изоляции',
    isolationTypes.name as 'Тип изоляции ППУ',
    externalMaterials.name as 'Материал наружного покрытия',
    anticorrMaterials.name as 'Материал антикоррозийного покрытия',
    deft.name as 'Режим',
    FORMAT(t.data_osmotra,'dd.MM.yyyy' ) as 'Дата обнаружения нарушения',
    t.vremya_osmotra as 'Время обнаружения повреждения',
    CONCAT(st.name, ' ', t.nomer_doma) as 'Адрес нарушения',
    es.name as 'Поврежденный трубопровод',
    t.rasstoyanieDoPovrezhdeniyaNachKamery as 'Расстояние до нарушения от ближайшей камеры, м',
    tp.name as 'Поверхность в месте раскопки',
    t.defectDescription as 'Описание повреждения',
    op.name as 'Наименование контура опрессовки',
    t.nomer_akta as 'Номер акта',
    t.nomer_prikaza as 'Номер приказа',
    vn.name as 'Вид нарушения',
    rc.name as 'Категория нарушения',
    tpov.name as 'Характер повреждения',
    t.tsentrPovrezhdenia as 'Место расположения центра повреждения на трубопроводе,  часов ',
    t.vysotaPovrezhdenia as 'Высота повреждения',
    t.shirinaPovrezhdenia as 'Ширина повреждения',
    t.ploshchadPovrezhdenia as 'Площадь повреждения',
    sostT.name as 'Состояние теплоизоляции теплопроводов',
    sostNP.name as 'Состояние наружной поверхности теплопроводов',
    sostVP.name as 'Состояние внутренней поверхности поврежденной трубы',
    stateDefect.name as 'Состояние Нарушения',
    IIF (uzel_nach_otcl.nodeName is NULL or uzel_nach_otcl.nodeName = '' or uzel_nach_otcl.nodeName = ' ',uzel_nach_otcl.externalNodeName, uzel_nach_otcl.nodeName) as 'Начальная камера отключения от сети',
    IIF (uzel_konec_otcl.nodeName is NULL or uzel_konec_otcl.nodeName = '' or uzel_konec_otcl.nodeName = ' ',uzel_konec_otcl.externalNodeName, uzel_konec_otcl.nodeName) as 'Конечная камера отключения от сети',
    t.meropriyatiya as 'Способ ликвидации нарушения',
    t.data_shurfovki as 'Дата шурфовки',
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
    t.inye_socialnye_posledstviya as 'Иные социальные последствия',
        t.shirinaZaplatki as 'Ширина заплатки',
        t.vysotaZaplatki as 'Высота заплатки',
        t.len_tube_cur as 'Длина заменённой трубы, м',
        remontIzolKonstruktsii.name as 'Ремонт теплоизоляционной конструкции',
        t.len_izol_cur as 'Длина заменённой изоляции, м',
        remont_kanala.name as 'Ремонт канала',
        t.len_channel_cur as 'Длина участка ремонта канала',
        remont_kamery.name as 'Ремонт камеры'       
from (
        select
            distinct
            l.lineID,
            organizations.name as 'organization',
            d.id as obj_id,
            d.remontTypeID,
            d.data_osmotra,
            d.vremya_osmotra,
            d.nomer_doma,
            d.ulicaID,
            d.priznak_truboprovoda,
            d.rasstoyanieDoPovrezhdeniyaNachKamery,
            d.tipPoverhnostiID,
            d.dreniruemyj_truboprovodID,
            d.defectDescription,
            d.opresID,
            d.nomer_akta,
            d.nomer_prikaza,
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
            d.remont_kameryID,
            shurfy.id as shurfID,
            shurfy.naznachenie_vskrID as naznachenie_vskrID_shurf,
            shurfy.ulicaID as ulicaID_shurf,
            shurfy.nomer_doma as nomer_doma_shurf,
            shurfy.sostoyanie_shurfaID,
            shurfy.data_nachala as data_nachala_shurf,
            shurfy.data_okonchaniya as data_okonchaniya_shurf
        from defect d
        JOIN (
            select 
                k.lineID,
                k.obj_id,
                k.organizationID,
                k.shurfID
            from (
                select
                    distinct
                        l.id as lineID,
                        d.id as obj_id,
                        ds.objID as shurfID,
                        l.organizationID,
                        ROW_NUMBER() OVER (PARTITION BY d.id ORDER  BY d.shape.STPointN(1).STDistance(l.shape)) AS rn
--                        l.shape.STDistance(d.shape) as length,
--                        MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len"
                from defect d
                join defectsForShurfy ds on ds.defectID = d.id
                JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1 )
            )k
--        where k.min_len = k.length 
        where k.rn = 1
    ) l on l.obj_id = d.id 
    left join shurfy on shurfy.id = l.shurfID
    left join organizations on organizations.id = l.organizationID
  where   
  $state_cond$
  (
  (d.data_osmotra $season_condition$)
  )
)t
LEFT JOIN heatPipeSections hpss ON hpss.lineID=t.lineID $and_condition2$
--$and_condition2$
LEFT JOIN pipeSections pss ON pss.id = hpss.pipeSectionID
JOIN nodes n1 ON ( n1.id = pss.nodeID1 and n1.removed = 0 and n1.internalNodeID IS NULL)
JOIN nodes n2 ON n2.id = pss.nodeID2
left join nodes uzel_nach_otcl on uzel_nach_otcl.id = t.nodeID1 
left join nodes uzel_konec_otcl on uzel_konec_otcl.id = t.nodeID2 
left join externalCodes ec1 ON ec1.id = n1.externalCodeID
left join externalCodes ec2 ON ec2.id = n2.externalCodeID

LEFT JOIN defectTypes deft ON deft.id = t.remontTypeID
left join stateDefect on stateDefect.id = t.stateID
left join tipPoverhnosti tp on tp.id = t.tipPoverhnostiID
left join externalSigns  es on es.id = t.priznak_truboprovoda 
left join vid_narusheniya vn on vn.id = t.vid_narusheniyaID
left join remontCat rc ON rc.id = t.remontCatID
left join remontIzolKonstruktsii on remontIzolKonstruktsii.id = t.remontIzolKonstruktsiiID
left join ulitsy st ON st.id = t.ulicaID
left join opres op ON op.id = t.opresID
left join tipPovrezhdenia tpov ON  tpov.id = t.tipPovrezhdeniaID
left join sostTeploizol sostT ON sostT.id = t.sostTeploizolID
left join sostNaruzhnoiPoverkhnosti sostNP ON sostNP.id = t.sostNaruzhnoiPoverkhnostiID
left join sostVnutrenneiPoverkhnosti sostVP ON sostVP.id = t.sostVnutrenneiPoverkhnostiID
left join remont_kanala on remont_kanala.id = t.remont_kanalaID
left join remont_kamery on remont_kamery.id = t.remont_kameryID

left join isolMaterials im on im.id = pss.isolMaterialID
LEFT JOIN tubingTypes ON tubingTypes.id=pss.tubingTypeID
left join isolationTypes on isolationTypes.id = pss.isolationTypeID
left join externalMaterials on externalMaterials.id = pss.externMaterialID
left join anticorrMaterials on anticorrMaterials.id = pss.anticorrMaterialID
left join magistrali on magistrali.id = pss.magistral

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


left join ulitsy st_shurf ON st_shurf.id = t.ulicaID_shurf
left join naznachenie_vskr n_vskr_shurf ON n_vskr_shurf.id = t.naznachenie_vskrID_shurf
left join sostoyanie_shurfa sost_shurf on sost_shurf.id = t.sostoyanie_shurfaID


WHERE 
   -- n1.fileID in (58) and
    ( (not ec1.name in ('П1','П2') or not ec2.name in ('П1','П2')) or (ec1.name is null AND ec2.name is null) ) 
--   $and_n1_fileID$
    --$and_condition$

) __

order by 'Дата обнаружения нарушения' desc
