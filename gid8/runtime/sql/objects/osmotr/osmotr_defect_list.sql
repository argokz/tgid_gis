select
d.id,
obj.name as 'Наименование',
obj.data_osmotra as 'Дата осмотра',
nach.fio as 'Ответственное лицо',
d.defectDescription as 'Описание дефекта',
deft.name as 'Режим',
stateDefect.name as 'Состояние нарушения',
FORMAT(d.data_osmotra,'yyyy-MM-dd' ) as 'Дата обнаружения нарушения',
d.vremya_osmotra as 'Время обнаружения повреждения',
CONCAT(st.name, ' ', d.nomer_doma) as 'Адрес нарушения',
IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',CONCAT(nt1.name, ' ', n1.externalNodeName), n1.nodeName) as 'Наименование начального узла',
ec1.name as 'Код расчетной схемы начального узла',
IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',CONCAT(nt2.name, ' ', n2.externalNodeName), n2.nodeName) as 'Наименование конечного узла',
ec2.name as 'Код расчетной схемы конечного узла',
es.name as 'Признак участка трубопровода',
pss.firstPICdateHP as 'Дата первичного ввода в эксплуатацию',
pss.lastTransDate as 'Дата последней перекладки',
magistrali.naimenovanie_magistrali as 'Магистраль',
IIF (pss.magistralSite is not NULL, re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Район эксплуатации',
IIF (pss.magistralSite is not NULL, ue_ms.nomer_uchastka, ue_rs.nomer_uchastka) as 'Участок эксплуатации',
IIF (pss.magistralSite is not NULL, ist_ms.naimenovanie, ist_rs.naimenovanie) as 'Источник теплоснабжения',
IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as 'Наименование участка',
IIF (pss.magistralSite is not NULL, 'магистраль', 'распредсеть') as 'Признак сети',
org.name as 'Владелец участка теплопровода',
IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка',
tubingTypes.name as 'Тип прокладки',
pss.pipeLength AS 'Длина участка теплопровода, м',
pss.DiamUslov as 'Диаметр условный'
from osmotr obj
left join osmotrDeployed osd ON osd.directionID = obj.id
JOIN heatPipeSections hpss ON hpss.lineID=osd.lineID
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

join defect d ON l.shape.STDistance(d.shape) < 0.1 and d.osmotrID = obj.id
LEFT JOIN defectTypes deft ON deft.id = d.remontTypeID
left join streets st ON st.id = d.ulicaID
left join stateDefect on stateDefect.id = d.stateID

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

where obj.id = $id$
