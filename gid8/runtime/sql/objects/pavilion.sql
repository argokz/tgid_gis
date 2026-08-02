select distinct 
	t.id,
	t.[Павильон],
	t.Месторасположение,
	t.[Конструкция стен],
	t.[Внутренняя высота, мм],
	t.[Внутренняя длина, мм],
	t.[Внутренняя ширина, мм],
	t.Площадь,
	t.Оборудование,
	t.[Средства пожаратушения],
	t.Сигнализация,
	t.[Наличие освещения],
	t.[Наличие схем],
	t.[Балансовая принадлежность],
	t.[Год ввода в эксплуатацию],
	t.Примечание,
	t.[Начальник участка],
	t.[Наименование участка],
	t.[Фрагмент сети]
from (
select 
  obj.id,
  lt.name as  "Месторасположение",
  ct.name as "Конструкция стен",
  vnutr_vysota_kamery as "Внутренняя высота, мм",
  vnutr_dlina_kamery as "Внутренняя длина, мм",
  vnutr_shirina_kamery as "Внутренняя ширина, мм",
 (vnutr_shirina_kamery * vnutr_dlina_kamery) * 1e-6 as "Площадь",
  oborudovanie_pavilona as "Оборудование",
  sredstva_pozharotushenija as "Средства пожаратушения",
  signalizacija as "Сигнализация",
  nalichie_osveshhenija as "Наличие освещения",
  nalichie_shem_truboprovodov as "Наличие схем",
  org.name as "Балансовая принадлежность",
  obj.primechanie as "Примечание",
  god_poslednego_vvoda_v_jekspluataciju as "Год ввода в эксплуатацию",
  l.shape.STDistance(obj.shape) as length,        
  MIN(l.shape.STDistance(obj.shape)) OVER(PARTITION BY obj.id ) AS "min_len",
  IIF(obj.shape.STIntersects(n1.shape) = 1, IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName),IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName)) AS "Павильон",
  IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as "Наименование участка",
  IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as "Начальник участка",
  fr.name as 'Фрагмент сети'

from pavilion obj

LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape) < 0.1

$JOIN_GEO$


LEFT JOIN heatPipeSections hps ON hps.lineID=l.id
LEFT JOIN pipeSections pss ON pss.id = hps.pipeSectionID

LEFT JOIN nodes n1 ON n1.id = pss.nodeID1
LEFT JOIN nodes n2 ON n2.id = pss.nodeID2
left join externalCodes ec1 ON ec1.id = n1.externalCodeID
left join externalCodes ec2 ON ec2.id = n2.externalCodeID

left join fragments fr on fr.id = n1.fileID
LEFT JOIN locationTypes lt ON lt.id = obj.locationTypesID
LEFT JOIN constructionTypes ct on ct.id = obj.constructionTypesID
left join organizations org on org.id = obj.organizationID

left join uchastok_ms ms ON ms.id = pss.magistralSite
left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka

left join uchastok_rs rs ON rs.id = pss.distSite
left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka

WHERE NOT l.shape.STDistance(obj.shape) IS NULL 
$AND_N1FILEID$
AND ( (not ec1.name in ('П1','П2') or not ec2.name in ('П1','П2')) or (ec1.name is null AND ec2.name is null) )	
)t 
where t.length = t.min_len 

