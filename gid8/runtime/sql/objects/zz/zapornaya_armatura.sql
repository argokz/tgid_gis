SELECT  distinct
        obj.id,
        IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Начальный узел',
        IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Конечный узел',     
        el.name as 'Признак трубопровода',
        obj.prinadlezhnost as 'Принадлежность',
        IIF(MIN(obj.shape.STPointN(1).STDistance(n1.shape)) OVER(PARTITION BY obj.id ) < MIN(obj.shape.STPointN(1).STDistance(n2.shape)) OVER(PARTITION BY obj.id ), IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName), IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName)) as 'Наименование узла/павильона',
        loc.name as 'Месторасположение',
        arm_t.name as 'Тип арматуры',
        des_t.name as 'Исполнение',
        mat_t.name as 'Материал',
        con_t.name as 'Конструкция',
        pur_t.name as 'Назначение',
        obj.diametr as 'Диаметр',
        obj.primechanie as 'Примечание',
            IIF (
        pss.magistralSite is not NULL,
        re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla,
        re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla
    ) as 'Район эксплуатации',
    IIF (
        pss.magistralSite is not NULL,
        ue_ms.nomer_uchastka,
        ue_rs.nomer_uchastka
    ) as 'Участок эксплуатации',
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

    IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка',
        pss.id as ps_id,
        l_min.lineID as l_id
    FROM zapornaya_armatura obj        
    join (      select
        distinct
        t.id,
        t.lineID,
        t.nodeID1,
        t.nodeID2,
        t.externalSignLineID
    from (
    SELECT distinct
        l.shape.STDistance(obj.shape.STPointN(1)) as length,
        l.id as lineID,
        l.nodeID1,
        l.nodeID2,
        l.externalSignLineID,
        MIN(l.shape.STDistance(obj.shape.STPointN(1))) OVER(PARTITION BY obj.id ) AS "min_len",
        obj.id
    FROM zapornaya_armatura obj     
    LEFT JOIN linesobj l ON l.removed = 0 and l.shape.STDistance(obj.shape.STPointN(1)) < 0.1
    ) t
    where t.min_len = t.length) l_min on l_min.id = obj.id 

    LEFT JOIN externalSignLine el ON el.id = obj.priznak_truboprovoda
    LEFT JOIN Locations loc ON loc.id = obj.mestoraspolozhenie

    LEFT JOIN heatPipeSections hps ON hps.lineID=l_min.lineID
    LEFT JOIN pipeSections pss ON pss.id = hps.pipeSectionID
    LEFT JOIN sortLinesForUchastok srt ON hps.pipeSectionID = srt.pipeSectionID
    LEFT JOIN purposeTypes pur_t ON pur_t.id = obj.purposeTypesID 
    LEFT JOIN armatureTypes arm_t ON arm_t.id = obj.armatureTypesID
    LEFT JOIN designTypes des_t ON des_t.id = obj.designTypesID
    LEFT JOIN constructionTypes con_t ON con_t.id = obj.constructionTypesID
    LEFT JOIN materialTypes mat_t ON mat_t.id = obj.materialTypesID

    LEFT JOIN nodes n1 ON n1.id = l_min.nodeID1
    LEFT JOIN nodes n2 ON n2.id = l_min.nodeID2
    left join externalCodes ec1 ON ec1.id = n1.externalCodeID
    left join externalCodes ec2 ON ec2.id = n2.externalCodeID   
    left join uchastok_ms ms ON ms.id = pss.magistralSite
    left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
    left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
    left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
    left join uchastok_rs rs ON rs.id = pss.distSite
    left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
    left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
    left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
    WHERE 
    pss.id is not null 
--    and n1.fileID in (58)
    AND ( (not ec1.name in ('П1','П2') or not ec2.name in ('П1','П2')) or (ec1.name is null AND ec2.name is null) )
