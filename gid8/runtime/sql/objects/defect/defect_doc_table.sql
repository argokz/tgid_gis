IF OBJECT_ID('tempdb..##defect_doc_table') IS NOT NULL DROP TABLE ##defect_doc_table;

select 
    *
INTO ##defect_doc_table
from (
select distinct
l.id as lineID,
d.id as obj_id,
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
l.shape.STDistance(d.shape) as length,
MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len"
    from defect d
    LEFT JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1 )
    LEFT JOIN nodes n1 ON n1.id=l.nodeID1 AND n1.removed=0  $and_n1_fileID$
    where 
        d.data_osmotra IS NOT NULL AND (d.data_osmotra $season_condition$) AND
        d.vid_narusheniyaID is not NULL or d.vid_rabotID is not NULL
)k
WHERE k.min_len = k.length;



select 
v.ps_id as id,
v.[Наименование начального узла],
v.[Наименование конечного узла],
v.Диаметр as 'Условный диаметр, мм',
v.[Длина участка теплопровода, м],
v.[Материал тепловой изоляции],
v.[Тип изоляции ППУ],
v.[Тип прокладки],
v.firstPICdateHP_2 as 'Дата ввода в эксплуатацию',
v.[Год эксплуатации],
DATEDIFF (Hh , v.firstPICdateHP_2, MIN(v.[data_osmotra])) as 'Наработка основного оборудования, ч' ,

SUM(CASE v.[Вид работ] WHEN 2 THEN 1 ELSE 0 END) as 'ТР',
SUM(CASE v.[Вид работ] WHEN 3 THEN 1 ELSE 0 END) as 'КР',
SUM(CASE v.[Вид работ] WHEN 4 THEN 1 ELSE 0 END) as 'ИР',
SUM(CASE v.[Вид работ] WHEN 6 THEN 1 ELSE 0 END) as 'О',

SUM(CASE v.[Вид нарушения] WHEN 1 THEN 1 ELSE 0 END) as 'A',
SUM(CASE v.[Вид нарушения] WHEN 2 THEN 1 ELSE 0 END) as 'ТО',
SUM(CASE v.[Вид нарушения] WHEN 3 THEN 1 ELSE 0 END) as 'ФО'


from (
    select distinct top 2147483647
        t.obj_id as id,
        pss.id as 'ps_id',
        IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Наименование начального узла',
        IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Наименование конечного узла',
        pss.DiamUslov as 'Диаметр',
        pss.pipeLength AS 'Длина участка теплопровода, м',
        im.name as 'Материал тепловой изоляции',
        isolationTypes.name as 'Тип изоляции ППУ',
        tubingTypes.name as 'Тип прокладки',
        pss.firstPICdateHP as 'Дата первичного ввода в эксплуатацию',
        firstPICdateHP_2 = CASE
            when (pss.lastTransDate is not null)  
                then pss.lastTransDate
            when pss.firstPICdateHP is not null then pss.firstPICdateHP
        END,
        MIN(t.data_osmotra) OVER(PARTITION BY pss.id, YEAR(t.data_osmotra) ) AS "min_date",
        YEAR(t.data_osmotra) as 'Год эксплуатации',
        t.data_osmotra,
        t.vid_rabotID as 'Вид работ',
        t.vid_narusheniyaID as 'Вид нарушения',
        'Наработка основного оборудования' = 
        CASE
            when (pss.lastTransDate is not null ) and t.data_osmotra is not null  
                then DATEDIFF (Hh , pss.lastTransDate, t.data_osmotra )
            when pss.firstPICdateHP is not null and t.data_osmotra is not null then DATEDIFF (Hh , pss.firstPICdateHP, t.data_osmotra )
        END,
        t.nomer_akta as 'Номер акта',
        t.data_sostavleniya_akta as 'Дата составления акта',
        t.nomer_prikaza as 'Номер приказа',
        t.data_prikaza_vvoda_v_ekspluataciyu as 'Дата приказа ввода в эксплуатацию'
        --pss.lastTransDate as 'Дата последней перекладки',
        --pss.PICdateCapital as 'Дата ввода в эксплуатацию после последнего капитального ремонта',
    from ##defect_doc_table t
    JOIN heatPipeSections hpss ON hpss.lineID=t.lineID $and_condition2$
    LEFT JOIN pipeSections pss ON pss.id = hpss.pipeSectionID
    LEFT JOIN nodes n1 ON ( n1.id = pss.nodeID1 and n1.removed = 0)
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

    WHERE
    ( (not ec1.name in ('П1','П2') or not ec2.name in ('П1','П2')) or (ec1.name is null AND ec2.name is null) ) 
     $and_n1_fileID$
)v 
group by 
v.ps_id,
v.[Наименование начального узла], 
v.[Наименование конечного узла],
v.firstPICdateHP_2,
v.Диаметр,
v.[Длина участка теплопровода, м],
v.[Тип изоляции ППУ],
v.[Материал тепловой изоляции],
v.[Тип прокладки],
v.[Год эксплуатации]

ORDER BY v.ps_id,v.[Год эксплуатации]
