select top 2147483647
    t.obj_id as id,
    IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Наименование начального узла',
    ec1.name as 'Код расчетной схемы начального узла',
    IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Наименование конечного узла',
    ec2.name as 'Код расчетной схемы конечного узла',
    es.name as 'Признак участка трубопровода',
    n_vskr.name as 'Назначение вскрытия',
    t.data_nachala_plan as 'Дата начала шурфовки (План)',
    t.data_okonchaniya_plan as 'Дата окончания шурфовки (План)',
    t.data_nachala as 'Дата начала шурфовки',
    t.data_okonchaniya as 'Дата окончания шурфовки',
    CONCAT(st.name,' ',t.nomer_doma) as 'Адрес',
    sost_shurf.name as 'Состояние',
    t.primechanie as 'Примечание',
    --hpss.netType as 'Вид сети',
    magistrali.naimenovanie_magistrali as 'Магистраль',
    IIF (pss.magistralSite is not NULL, re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Район эксплуатации',
    IIF (pss.magistralSite is not NULL, ue_ms.nomer_uchastka, ue_rs.nomer_uchastka) as 'Участок эксплуатации',
    IIF (pss.magistralSite is not NULL, ist_ms.naimenovanie, ist_rs.naimenovanie) as 'Источник теплоснабжения',
    IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as 'Наименование участка',
    IIF (pss.magistralSite is not NULL, 'магистраль', 'распредсеть') as 'Признак сети',
    IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка',
    pss.firstPICdateHP as 'Дата первичного ввода в эксплуатацию',
    pss.lastTransDate as 'Дата последней перекладки',
    tubingTypes.name as 'Тип прокладки',
    pss.diameterExternal as 'Диаметр наружный, мм',
    pss.wallThickness AS 'Толщина стенки, мм',
    pss.pipeSectLength AS 'Длина участка теплопровода, м',
    im.name as 'Материал тепловой изоляции',
    isolationTypes.name as 'Тип изоляции ППУ',
    externalMaterials.name as 'Материал наружного покрытия',
    anticorrMaterials.name as 'Материал антикоррозийного покрытия',
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
        l.id as lineID,
        l.externalSignLineID,
        d.id as obj_id,
        d.materialy_i_mekhanizmyID,
        d.data_utverzhdeniya_plana_shurfovok,
        d.naznachenie_vskrID,
        d.sostoyanie_shurfaID,
        d.ulicaID,
        d.nomer_doma,
        d.data_nachala,
        d.data_okonchaniya,
        d.data_nachala_plan,
        d.data_okonchaniya_plan,
        d.primechanie,
        d.fio_utverzhdaemogo,
        d.dolzhnost_utverzhdaemogoID,
        d.sluzhba_utverzhdaemogoID,
        d.fio_1,
        d.dolzhnost_1,
        d.fio_2,
        d.dolzhnost_2,
        d.fio_viziruemogo_1,
        d.dolzhnost_viziruemogoID_1,
        d.utverdit
    from shurfy d
        JOIN linesobj l ON l.id = (select top 1
    k.lineID as lineID
    from (
    select
        distinct
            l.id as lineID,
            d.id as obj_id,
            l.shape.STDistance(d.shape) as length,
            MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len"
    from shurfy d
    JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1 )
    WHERE   
    (d.data_nachala_plan $season_condition$) $and_naznachenie_vskrID$
    d.id = $id$
    )k
    where k.min_len = k.length 
    ) and l.shape.STDistance(d.shape) < 0.1 
)t
--  LEFT 
    JOIN heatPipeSections hpss ON hpss.lineID=t.lineID $and_condition2$

    LEFT JOIN pipeSections pss ON pss.id = hpss.pipeSectionID
    JOIN nodes n1 ON ( n1.id = pss.nodeID1 and n1.removed = 0 )
    JOIN nodes n2 ON n2.id = pss.nodeID2
    left join externalCodes ec1 ON ec1.id = n1.externalCodeID
    left join externalCodes ec2 ON ec2.id = n2.externalCodeID
    left join externalSigns  es on es.id = t.externalSignLineID

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

    left join ulitsy st ON st.id = t.ulicaID
    
    left join materialy_i_mekhanizmy m ON m.id = t.materialy_i_mekhanizmyID
    left join naznachenie_vskr n_vskr ON n_vskr.id = t.naznachenie_vskrID
    left join sostoyanie_shurfa sost_shurf on sost_shurf.id = t.sostoyanie_shurfaID

    LEFT JOIN dolzhnosti dolz ON dolz.id=t.dolzhnost_utverzhdaemogoID
    LEFT JOIN dolzhnosti dolz_1 ON dolz_1.id=t.dolzhnost_viziruemogoID_1
    LEFT JOIN dolzhnosti dolz_f1 ON dolz_f1.id=t.dolzhnost_1
    LEFT JOIN dolzhnosti dolz_f2 ON dolz_f2.id=t.dolzhnost_2
    LEFT JOIN subdivisions subd ON subd.id=t.sluzhba_utverzhdaemogoID

order by t.data_nachala_plan desc