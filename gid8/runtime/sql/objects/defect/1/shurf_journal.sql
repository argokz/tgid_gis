select distinct top 2147483647
    t.obj_id as id,
    IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Наименование начального узла',
    ec1.name as 'Код расчетной схемы начального узла',
    IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Наименование конечного узла',
    ec2.name as 'Код расчетной схемы конечного узла',
    es.name as 'Признак участка трубопровода',
    pss.firstPICdateHP as 'Дата первичного ввода в эксплуатацию',
    IIF (pss.magistralSite is not NULL, re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Участок/район эксплуатации',
    IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as 'Наименование участка',
    IIF (pss.magistralSite is not NULL, 'магистраль', 'распредсеть') as 'Признак сети',
    IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка',
    IIF (pss.magistralSite is not NULL, ist_ms.naimenovanie, ist_rs.naimenovanie) as 'Источник теплоснабжения',
    t.organization as 'Владелец участка теплопровода',
    tubingTypes.name as 'Тип прокладки',
    pss.diameterExternal as 'Диаметр наружный, мм',
    pss.wallThickness AS 'Толщина стенки, мм',
    pss.pipeSectLength AS 'Длина участка теплопровода, м',
    im.name as 'Материал изоляции',

    t.data_utverzhdeniya_plana_shurfovok as 'Дата утверждения плана шурфовок',
    n_vskr.name as 'Назначение вскрытия',
    CONCAT(st.name,' ',t.nomer_doma) as 'Адрес',
    sost_shurf.name as 'Состояние',
    t.data_nachala_plan as 'Дата начала шурфовки (План)',
    t.data_okonchaniya_plan as 'Дата окончания шурфовки (План)',
    m.name as 'Материалы и механизмы',
    t.data_nachala as 'Дата начала шурфовки',   
    t.data_okonchaniya as 'Дата окончания шурфовки',
    t.rasstoyanie_do_blizhajshej_kamery as 'Расстояние до ближайшей камеры, м',
    harakter_grunta_shurf.name as 'Грунт',
    yn_podtoplenie_do_truby.name as 'Подтопление до трубы',
    ustrojstva_vodootvedeniya.name as 'Устройства водоотведения',
    poverhnost_nad_trassoj.name as 'Поверхность над трассой',
    yn_nalichie_vblizi_elektrificirovannogo_transporta.name as 'Наличие вблизи электрифицированного транспорта',
    t.rasstoyanie_do_relsov as 'Расстояние до рельсов, м',
    nalichie_vblizi_kommunikacij.name as 'Наличие вблизи коммуникаций',
    da_net_informacii_elektrozashchitnyh_ustanovok.name as 'Наличие вблизи работающих электрозащитных установок',
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

    gidroizolyacionnaya_konstrukciya_podacha.name as 'Гидроизоляционная конструкция (подающий трубопровод)',
    gidroizolyacionnaya_konstrukciya_obratka.name as 'Гидроизоляционная конструкция (обратный трубопровод)',

    sostoyanie_protivokorrozionnogo_pokrytiya_podacha.name as 'Состояние противокоррозионного покрытия (подающий трубопровод)',
    sostoyanie_protivokorrozionnogo_pokrytiya_obratka.name as 'Состояние противокоррозионного покрытия (обратный трубопровод)',

    nalichie_korrozii_podacha.name as 'Наличие коррозии (подающий трубопровод)',
    nalichie_korrozii_obratka.name as 'Наличие коррозии (обратный трубопровод)',

    t.mesto_kontrolnoj_vyrezki_truboprovoda as 'Место контрольной вырезки трубопровода',
    t.rezultaty_vyrezki as 'Результаты вырезки',
    t.rezultaty_osmotra as 'Результаты осмотра (шурф)',
    t.namechennye_meropriyatiya as 'Намеченные мероприятия',
    t.meropriyatiya_po_vosstanovleniyu_prokladki as 'Мероприятия по восстановлению прокладки',
    t.primechanie as 'Примечание',

    t.fio_utverzhdaemogo as 'ФИО утверждающего',
    dolz.znachenie as 'Должность утверждающего',
    subd.name as 'Служба утверждающего',
    t.fio_1 as 'ФИО 1 члена',
    dolz_f1.znachenie as 'Должность 1 члена',
    t.fio_2 as 'ФИО 2 члена',
    dolz_f2.znachenie as 'Должность 2 члена',
    fio_viziruemogo_1 as 'ФИО визирующего',
    dolz_1.znachenie as 'Должность визирующего'
from(
    select distinct
--        MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len",
--        l.shape.STDistance(d.shape) as length,
        ROW_NUMBER() OVER (PARTITION BY d.id ORDER  BY d.shape.STPointN(1).STDistance(l.shape)) AS rn,
        org.name as 'organization',
        l.id as lineID,
        l.externalSignLineID,
        d.id as obj_id,
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
        d.dlina_osmotra,
        d.glubina_zalozheniya,
        d.harakter_gruntaID,
        d.podtoplenie_do_truby,
        d.ustrojstva_vodootvedeniyaID,
        d.poverhnost_nad_trassojID,
        d.nalichie_vblizi_elektrificirovannogo_transporta,
        d.rasstoyanie_do_relsov,
        d.nalichie_vblizi_kommunikacijID,
        d.nalichie_vblizi_rabotayushchih_elektrozashchitnyh_ustanovokID,
        d.nomer_akta,
        d.osmotreny_elementyID,
        d.gidroizolyacionnaya_konstrukciya_podachaID,
        d.gidroizolyacionnaya_konstrukciya_obratkaID,
        d.sostoyanie_protivokorrozionnogo_pokrytiya_podachaID,
        d.sostoyanie_protivokorrozionnogo_pokrytiya_obratkaID,
        d.nalichie_korrozii_podachaID,
        d.nalichie_korrozii_obratkaID,
        d.mesto_kontrolnoj_vyrezki_truboprovoda,
        d.rezultaty_vyrezki,
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
        left JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape.STPointN(1)) < 0.1)
        LEFT JOIN organizations org ON org.id=l.organizationID
        left join nodes n1 ON n1.id = l.nodeID1 
    where   
  (
  (d.data_nachala_plan $season_condition$) OR
  (d.data_nachala $season_condition$) 
  )
   $and_n1_fileID$

  
  
  
  $and_naznachenie_vskrID$ $and_utverdit$
)t
    LEFT JOIN heatPipeSections hpss ON hpss.lineID=t.lineID $and_condition2$

    LEFT JOIN pipeSections pss ON pss.id = hpss.pipeSectionID
    left join isolMaterials im on im.id = pss.isolMaterialID
    LEFT JOIN tubingTypes ON tubingTypes.id=pss.tubingTypeID
    left JOIN nodes n1 ON ( n1.id = pss.nodeID1 and n1.removed = 0 )
    left JOIN nodes n2 ON n2.id = pss.nodeID2
    left join externalCodes ec1 ON ec1.id = n1.externalCodeID
    left join externalCodes ec2 ON ec2.id = n2.externalCodeID
    left join externalSigns  es on es.id = t.externalSignLineID

    left join uchastok_ms ms ON ms.id = pss.magistralSite
    left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
    LEFT JOIN istochniki_tepla ist_ms ON ist_ms.id=ue_ms.istochnik_tepla
    left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
    left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka

    left join uchastok_rs rs ON rs.id = pss.distSite
    left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
    LEFT JOIN istochniki_tepla ist_rs ON ist_rs.id=ue_rs.istochnik_tepla
    left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
    left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka

    left join ulitsy st ON st.id = t.ulicaID
    left join materialy_i_mekhanizmy m ON m.id = t.materialy_i_mekhanizmyID
    left join naznachenie_vskr n_vskr ON n_vskr.id = t.naznachenie_vskrID
    left join sostoyanie_shurfa sost_shurf on sost_shurf.id = t.sostoyanie_shurfaID
    left join harakter_grunta_shurf  on harakter_grunta_shurf.id = t.harakter_gruntaID
    left join YesOrNo yn_podtoplenie_do_truby on yn_podtoplenie_do_truby.id = t.podtoplenie_do_truby
    left join YesOrNo yn_nalichie_vblizi_elektrificirovannogo_transporta on yn_nalichie_vblizi_elektrificirovannogo_transporta.id = t.nalichie_vblizi_elektrificirovannogo_transporta
    left join ustrojstva_vodootvedeniya  on ustrojstva_vodootvedeniya.id = t.ustrojstva_vodootvedeniyaID
    left join poverhnost_nad_trassoj on poverhnost_nad_trassoj.id = t.poverhnost_nad_trassojID
    left join nalichie_vblizi_kommunikacij on nalichie_vblizi_kommunikacij.id = t.nalichie_vblizi_kommunikacijID
    left join da_net_informacii da_net_informacii_elektrozashchitnyh_ustanovok on da_net_informacii_elektrozashchitnyh_ustanovok.id = t.nalichie_vblizi_rabotayushchih_elektrozashchitnyh_ustanovokID
    
    left join gidroizolyacionnaya_konstrukciya gidroizolyacionnaya_konstrukciya_podacha on gidroizolyacionnaya_konstrukciya_podacha.id = t.gidroizolyacionnaya_konstrukciya_podachaID
    left join gidroizolyacionnaya_konstrukciya gidroizolyacionnaya_konstrukciya_obratka on gidroizolyacionnaya_konstrukciya_obratka.id = t.gidroizolyacionnaya_konstrukciya_obratkaID
    
    left join sostoyanie_protivokorrozionnogo_pokrytiya_shurf sostoyanie_protivokorrozionnogo_pokrytiya_podacha on sostoyanie_protivokorrozionnogo_pokrytiya_podacha.id = t.sostoyanie_protivokorrozionnogo_pokrytiya_podachaID
    left join sostoyanie_protivokorrozionnogo_pokrytiya_shurf sostoyanie_protivokorrozionnogo_pokrytiya_obratka on sostoyanie_protivokorrozionnogo_pokrytiya_obratka.id = t.sostoyanie_protivokorrozionnogo_pokrytiya_obratkaID

    left join nalichie_korrozii_shurf nalichie_korrozii_podacha on nalichie_korrozii_podacha.id = t.nalichie_korrozii_podachaID
    left join nalichie_korrozii_shurf nalichie_korrozii_obratka on nalichie_korrozii_obratka.id = t.nalichie_korrozii_obratkaID


    LEFT JOIN dolzhnosti dolz ON dolz.id=t.dolzhnost_utverzhdaemogoID
    LEFT JOIN dolzhnosti dolz_1 ON dolz.id=t.dolzhnost_viziruemogoID_1
    LEFT JOIN dolzhnosti dolz_f1 ON dolz.id=t.dolzhnost_1
    LEFT JOIN dolzhnosti dolz_f2 ON dolz.id=t.dolzhnost_2
    LEFT JOIN subdivisions subd ON subd.id=t.sluzhba_utverzhdaemogoID

where t.rn = 1
   $and_n1_fileID$

order by t.data_nachala_plan desc
