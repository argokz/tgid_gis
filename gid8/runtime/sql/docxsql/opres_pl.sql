SELECT DISTINCT
    t.id,
    IIF(t.[Наименование начального узла] is null or t.[Наименование начального узла] = '', 'не заполнено', t.[Наименование начального узла] ) as 'Наименование начального узла',
    IIF(t.[Наименование конечного узла] is null or t.[Наименование конечного узла] = '', 'не заполнено',  t.[Наименование конечного узла] ) as  'Наименование конечного узла',
    DiamUslov AS 'Диаметр условный, мм',
    pipeLength AS 'Протяженность, м',
    typ_pr AS 'Тип прокладки',
    t.[Наименование участка],
    t.[Участок эксплуатации],
    t.[Район эксплуатации],
    t.[Признак сети]
FROM 
(
SELECT 
DISTINCT
ps1.id,
IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',CONCAT(nt1.name, ' ', n1.externalNodeName), n1.nodeName) as 'Наименование начального узла',
IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',CONCAT(nt2.name, ' ', n2.externalNodeName), n2.nodeName) as 'Наименование конечного узла',
ps1.DiamUslov,
ps1.pipeLength,
tt.name AS typ_pr,
IIF (ps1.magistralSite is not NULL, re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Район эксплуатации',
IIF (ps1.magistralSite is not NULL, ue_ms.nomer_uchastka, ue_rs.nomer_uchastka) as 'Участок эксплуатации',
IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as 'Наименование участка',
IIF (ps1.magistralSite is not NULL, 'магистраль', 'распредсеть') as 'Признак сети'
,srt.orderID

FROM opres obj
JOIN opresDeployed d ON d.directionID=obj.id
JOIN heatPipeSections hps ON hps.lineID=d.lineID
JOIN pipeSections ps1 ON ps1.id=hps.pipeSectionID
--JOIN linesobj l ON l.id=d.lineID
LEFT JOIN sortLinesForUchastok srt ON ps1.id = srt.pipeSectionID    
LEFT JOIN tubingTypes tt ON ps1.tubingTypeID = tt.id
JOIN nodes n1 ON n1.id=ps1.nodeID1
JOIN nodes n2 ON n2.id=ps1.nodeID2
LEFT JOIN nodeTypes nt1 ON nt1.id=n1.nodeTypeID
LEFT JOIN nodeTypes nt2 ON nt2.id=n2.nodeTypeID

left join uchastok_ms ms ON ms.id = ps1.magistralSite
left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka

left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka

left join uchastok_rs rs ON rs.id = ps1.distSite
left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka

left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
WHERE obj.id=$fileID$
) t
--O1RDER BY orderID

