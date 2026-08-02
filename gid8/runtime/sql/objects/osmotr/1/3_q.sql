select
top 2147483647 * FROM (

select
DISTINCT top 2147483647
osd.id AS id,
--hpss.pipeSectionID AS id,
--obj.id,
obj.name as 'Наименование',
obj.data_osmotra as 'Дата осмотра',
nach.fio as 'Ответственное лицо',
IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',CONCAT(nt1.name, ' ', n1.externalNodeName), n1.nodeName) as 'Наименование начального узла',
IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',CONCAT(nt2.name, ' ', n2.externalNodeName), n2.nodeName) as 'Наименование конечного узла',
es.name as 'Признак участка трубопровода',
IIF(faktory_riska_truboprovoda.id IS NULL,'не осмотрено', 'осмотрено') as "Состояние осмотра",
IIF(d.id is not null, 'Нарушение', '-') as 'Обнаруженные нарушения',
org.name as 'Владелец участка теплопровода',
IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка',
tubingTypes.name as 'Тип прокладки',
pss.pipeLength AS 'Длина участка теплопровода, м',
pss.DiamUslov as 'Диаметр условный',
obj.id AS obj_id

from osmotr obj
left join osmotrDeployed osd ON osd.directionID = obj.id
JOIN heatPipeSections hpss ON hpss.lineID=osd.lineID $and_condition2$
JOIN pipeSections pss ON pss.id=hpss.pipeSectionID
LEFT JOIN sortLinesForUchastok srt ON pss.id = srt.pipeSectionID
LEFT JOIN linesobj l on l.id = osd.lineID
left join organizations org on org.id = pss.organizationID
left join nachalniki_uchastkov nach ON nach.id=obj.otvetstvennoe_lico_ID

LEFT JOIN tubingTypes  ON pss.tubingTypeID = tubingTypes.id

JOIN nodes n1 ON n1.id=pss.nodeID1
JOIN nodes n2 ON n2.id=pss.nodeID2
LEFT JOIN nodeTypes nt1 ON nt1.id=n1.nodeTypeID
LEFT JOIN nodeTypes nt2 ON nt2.id=n2.nodeTypeID
left join externalCodes ec1 ON ec1.id = n1.externalCodeID
left join externalCodes ec2 ON ec2.id = n2.externalCodeID
left join externalSigns  es on es.id = l.externalSignLineID

left join defect d ON l.shape.STDistance(d.shape) < 0.1 and d.osmotrID = obj.id

--left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = ps1.id and faktory_riska_truboprovoda.objID = $m_opr_id$ and faktory_riska_truboprovoda.obj_type_faktory_riskaID = 2
left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = pss.id and faktory_riska_truboprovoda.objID = obj.id and faktory_riska_truboprovoda.obj_type_faktory_riskaID = 2
--"Обнаруженные нарушения" нарушения
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

where faktory_riska_truboprovoda.id is null

AND   (  obj.data_osmotra $season_condition$  )

--or2der by obj.data_osmotra desc, obj.id 

) __

order by 
'Дата осмотра' desc, 
obj_id



