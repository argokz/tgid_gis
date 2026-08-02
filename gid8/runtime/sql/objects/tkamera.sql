select distinct
t.id,
t.Павильон,
t.[Внутренняя высота, мм],
t.[Внутренняя длина, мм],
t.[Внутренняя ширина, мм],
t.[Конструкция перекрытия],
t.[Конструкция камеры],
t.[Размеры перекрытия, мм],
t.[Марка плиты перекрытия],
t.[Дно камеры, м],
t.[Возвышение верха плиты перекрытия, мм],
t.[Заглубление от верха люка до низа, мм],
t.[Фактическое количество люков, шт],
t.[Действующее количество люков, шт],
t.[Наличие решеток],
t.[Количество чугунных люков, шт],
t.[Количество стальных люков, шт],
t.[Количество железобетонных люков, шт],
t.[Количество полимерных люков, шт],
t.[Количество запирающих люков, шт],
t.[Количество дренажных линий, шт],
t.[Характеристика дренажных линий],
t.[Количество дренажных колодцев, шт],
t.[Количество точек замера, шт],
t.[Характеристика точек замера],
t.[Балансовая принадлежность],
t.[Год ввода в эксплуатацию],
t.Примечание,
t.[Начальник участка],
t.[Наименование участка],
t.[Фрагмент сети]
from(
select 
	obj.id,
	vnutr_vysota_kamery as "Внутренняя высота, мм",
	vnutr_dlina_kamery as "Внутренняя длина, мм",
	vnutr_shirina_kamery as "Внутренняя ширина, мм",
	cov.name as "Конструкция перекрытия",
	ct.name as "Конструкция камеры",
	razmery_perekrytiya as "Размеры перекрытия, мм",
	ov.name as "Марка плиты перекрытия",
	dno_kamery as "Дно камеры, м",
	vozvyshenie_verha_plity_perekry as "Возвышение верха плиты перекрытия, мм",
	zaglublenie_ot_verha_lyuka_do_n as "Заглубление от верха люка до низа, мм",
	fakticheskoe_kolichestvo_lyukov as "Фактическое количество люков, шт",
	kolichestvo_deystvuyuschiy_lyukov as "Действующее количество люков, шт",
	nalichie_reshetok as "Наличие решеток", 
	kolichestvo_chugunnyh_lyukov as "Количество чугунных люков, шт",
	kolichestvo_stalnyh_lyukov as  "Количество стальных люков, шт",
	kolichestvo_zhelezobet_lyukov as "Количество железобетонных люков, шт",
	kolichestvo_polymer_lyukov as "Количество полимерных люков, шт",
	kolichestvo_zapirayuschih_lyukov as "Количество запирающих люков, шт",
	kolichestvo_dren_liniy as "Количество дренажных линий, шт",
	harakteristika_dren_liniy as "Характеристика дренажных линий",
	kolichestvo_dren_kolodtsev as "Количество дренажных колодцев, шт",
	kolichestvo_tochek_zamera as "Количество точек замера, шт",
	harakteristika_tochek_zamera as "Характеристика точек замера",
	org.name as "Балансовая принадлежность",
	god_vvoda_v_jekspluataciju as "Год ввода в эксплуатацию",
	obj.primechanie as "Примечание",
	l.shape.STDistance(obj.shape) as length,        
	MIN(l.shape.STDistance(obj.shape)) OVER(PARTITION BY obj.id ) AS "min_len",
	IIF(obj.shape.STIntersects(n1.shape) = 1, IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName),IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName)) AS "Павильон",
	IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as "Наименование участка",
	IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as "Начальник участка",
  fr.name as 'Фрагмент сети'
	from tkamera obj
LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape) < 0.1
$JOIN_GEO$
LEFT JOIN heatPipeSections hps ON hps.lineID=l.id
LEFT JOIN pipeSections pss ON pss.id = hps.pipeSectionID
LEFT JOIN nodes n1 ON n1.id = pss.nodeID1
LEFT JOIN nodes n2 ON n2.id = pss.nodeID2
left join externalCodes ec1 ON ec1.id = n1.externalCodeID
left join externalCodes ec2 ON ec2.id = n2.externalCodeID 

left join fragments fr on fr.id = n1.fileID

LEFT JOIN overlapTypes ov on ov.id = obj.overlapTypesID
LEFT JOIN constructionOverlapTypes cov ON cov.id = obj.constructionOverlapTypesID
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

) t  where t.length = t.min_len 





