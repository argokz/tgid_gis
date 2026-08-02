select distinct top 2147483647
t.obj_id as id,
t.beginNode as 'Наименование начального узла',
t.[Код расчетной схемы начального узла],
t.endNode as 'Наименование конечного узла',
t.[Код расчетной схемы конечного узла],
t.[Режим],
t.[Дата осмотра],
t.[Время осмотра],
CONCAT(t.[Улица], ' ', t.[Номер дома]) as 'Адрес',
t.[Поврежденный трубопровод],
t.[Категория нарушения],
t.[Состояние],
t.[Вид работ],
t.[beginNodeOtkl] as  'Наименование начального узла отключения от сети',
t.[Код расчетной схемы начального узла отключения от сети],
t.[endNodeOtkl] as 'Наименование конечного узла отключения от сети',
t.[Код расчетной схемы конечного узла отключения от сети],
t.[Способ ликвидации нарушения],
t.[Дата шурфовки],
CONCAT(t.[Дата начала ремонтных работ], ' ', t.[Время начала ремонтных работ]) as 'Начала ремонта',
CONCAT(t.[Дата завершения ремонтных работ], ' ', t.[Время завершения  ремонтных работ])  as 'Окончание ремонта',
t.[Трудозатраты на ремонт],
t.[Стоимость ремонтных работ, тг],
t.[Количество отключенных потребителей],
t.[Количество недоотпущенной тепловой энергии],
t.[Затраты на восстановление],
t.[Иные социальные факторы],
t.[Ремонт трубопровода и элементов],
t.[Размер заплатки],
t.[Длина заменённой трубы, м],
t.[Ремонт теплоизоляционной конструкции],
t.[Длина заменённой изоляции, м],
t.[Ремонт канала],
t.[Ремонт камеры],
t.[Примечание]
from (
select
	distinct
	IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'beginNode',
	IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'endNode',	
	ec1.name as 'Код расчетной схемы начального узла',
	ec2.name as 'Код расчетной схемы конечного узла',
	deft.name as 'Режим',
---	FORMAT(d.data_osmotra,'yyyy-MM-dd' ) as 'Дата осмотра',
  FORMAT(d.data_osmotra,'dd.MM.yyyy' ) as 'Дата осмотра',
	d.vremya_osmotra as 'Время осмотра',
	st.name as 'Улица',
	d.nomer_doma as 'Номер дома',
	es.name as 'Поврежденный трубопровод',
	d.rasstoyanieDoPovrezhdeniyaNachKamery as 'Расстояние до нарушения от ближайшей камеры, м',
	tp.name as 'Поверхность в месте раскопки',
	d.defectDescription as 'Описание повреждения',
	op.name as 'Наименование контура опрессовки',
	vn.name as 'Вид нарушения',
	rc.name as 'Категория нарушения',
	vid_rabot.name as 'Вид работ',
	IIF (n_otkl_1.nodeName is NULL or n_otkl_1.nodeName = '' or n_otkl_1.nodeName = ' ',n_otkl_1.externalNodeName, n_otkl_1.nodeName) as 'beginNodeOtkl',
	IIF (n_otkl_2.nodeName is NULL or n_otkl_2.nodeName = '' or n_otkl_2.nodeName = ' ',n_otkl_2.externalNodeName, n_otkl_2.nodeName) as 'endNodeOtkl',	
	ec1_otkl.name as 'Код расчетной схемы начального узла отключения от сети',
	ec2_otkl.name as 'Код расчетной схемы конечного узла отключения от сети',
	defectStates.name as 'Состояние',
	d.data_shurfovki as 'Дата шурфовки',
	d.trudozatratyNaRemont as 'Трудозатраты на ремонт',
	d.stoimostRemonta as 'Стоимость ремонтных работ, тг',
	d.kolichestvo_otklyuchennyh_potrebitelej as 'Количество отключенных потребителей',
	d.kolichestvo_nedootpushchennoj_teplovoj_energii as 'Количество недоотпущенной тепловой энергии',
	d.zatraty_na_vosstanovlenie as 'Затраты на восстановление',
	d.inye_socialnye_posledstviya as 'Иные социальные факторы',
	CONCAT( IIF( d.shirinaZaplatki is not NULL,d.shirinaZaplatki,'0' ),' мм, ' ,IIF (d.vysotaZaplatki is not null, d.vysotaZaplatki, '0'), ' мм') as 'Размер заплатки',
	rik.name as 'Ремонт теплоизоляционной конструкции',
	remont_kanala.name as 'Ремонт канала',
	remont_kamery.name as 'Ремонт камеры',
	'Поврежденный элемент' =
		STUFF(
		( SELECT concat(',', rtt2.name) AS n FROM defect r2 
		LEFT JOIN povrezhdennyiElementForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN povrezhdennyiElement rtt2 ON rtt2.id=rt2.activityID
		WHERE r2.id=d.id 
		FOR XML PATH('')
		, TYPE
		).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	tpov.name as 'Тип повреждения',
	d.tsentrPovrezhdenia as 'Место расположения центра повреждения на трубопроводе,  часов ',
	d.vysotaPovrezhdenia as 'Высота повреждения',
	d.shirinaPovrezhdenia as 'Ширина повреждения',
	'Причины нарушения (технические)' =
		STUFF(
		( SELECT concat(',', rtt2.name) AS n FROM defect r2 
		LEFT JOIN prichinyPovrezhdeniaForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN prichinyPovrezhdenia rtt2 ON rtt2.id=rt2.activityID
		WHERE r2.id=d.id 
		FOR XML PATH('')
		, TYPE
		).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	'Причины нарушения (организационные) ' =
		STUFF(
		( SELECT concat(',', rtt2.name) AS n FROM defect r2 
		LEFT JOIN prichiny_narusheniya_organizacionnye_for_defect rt2 ON r2.id=rt2.objID
		LEFT JOIN prichiny_narusheniya_organizacionnye rtt2 ON rtt2.id=rt2.activityID
		WHERE r2.id=d.id 
		FOR XML PATH('')
		, TYPE
		).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	'Сопутствующие причины' =
		STUFF(
		( SELECT concat(',', rtt2.name) AS n FROM defect r2 
		LEFT JOIN soputstvuiushchiePrichinyForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN soputstvuiushchiePrichiny rtt2 ON rtt2.id=rt2.activityID
		WHERE r2.id=d.id 
		FOR XML PATH('')
		, TYPE
		).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	'Состояние конструкции канала' = 
		STUFF(
		( SELECT concat(',', rtt2.name) AS n FROM defect r2 
		LEFT JOIN sostKonstruktsiiKanalaForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN sostKonstruktsiiKanala rtt2 ON rtt2.id=rt2.activityID
		WHERE r2.id=d.id 
		FOR XML PATH('')
		, TYPE
		).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),		
	'Состояние конструкции камеры' = 
		STUFF(
		( SELECT concat(',', rtt2.name) AS n FROM defect r2 
		LEFT JOIN sostKonstruktsiiKameryForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN sostKonstruktsiiKamery rtt2 ON rtt2.id=rt2.activityID
		WHERE r2.id=d.id 
		FOR XML PATH('')
		, TYPE
		).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
		sostT.name as 'Состояние теплоизоляции теплопроводов',
		sostNP.name as 'Состояние наружной поверхности теплопроводов',
		sostVP.name as 'Состояние внутренней поверхности поврежденной трубы',
		d.meropriyatiya as 'Способ ликвидации нарушения',
		d.data_nachala_remonta as 'Дата начала ремонтных работ',
		d.vremiaNachalaRemonta as 'Время начала ремонтных работ',
		d.data_zaversheniya_remonta as 'Дата завершения ремонтных работ',
		d.vremiaZaversheniaRemonta as 'Время завершения  ремонтных работ',
		'Ремонт трубопровода и элементов' = 
			STUFF(
			( SELECT distinct concat(IIF(rtt2.name != '', CONCAT(',', rtt2.name, ':'), ''), STUFF (
				(select concat(',',se.name) as n from defectTube dt
			LEFT JOIN spisokElementov se ON se.id=dt.elementID
			where objId = d.id and activityID = rt2.activityID
			FOR XML PATH('')
			, TYPE
			).value('.', 'NVARCHAR(MAX)'), 1, 1, '')
) AS n FROM defect r2 
			LEFT JOIN defectTube rt2 ON r2.id=rt2.objID
			LEFT JOIN remontTruboprovodaSpisok rtt2 ON rtt2.id=rt2.activityID
			WHERE r2.id=d.id 
			FOR XML PATH('')
			, TYPE
			).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
		d.len_tube_cur as 'Длина заменённой трубы, м',
		d.len_izol_cur as 'Длина заменённой изоляции, м',
		d.len_channel_cur as 'Длина участка ремонта канала',
		d.otchet_po_defektu as 'Примечание',
		MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len",
		l.shape.STDistance(d.shape) as length,
		l.id as lineID,
		d.id as obj_id
		--,IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as naimenovanie_uchastka
		--,IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as fio
from defect d
JOIN linesobj l ON l.id = (select top 1
t.lineID as lineID
from (
select
	distinct
		l.id as lineID,
		d.id as obj_id,
		l.shape.STDistance(d.shape) as length,
		MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len"
from defect d
JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1 )
WHERE	
--d.id = $defectID$
d.id = $id$
--and n1.fileID in (58)
) t where t.length = t.min_len) and l.shape.STDistance(d.shape) < 0.1 
LEFT JOIN heatPipeSections hpss ON hpss.lineID=l.id
LEFT JOIN pipeSections pss ON pss.id = hpss.pipeSectionID
JOIN nodes n1 ON ( n1.id = pss.nodeID1 and n1.removed = 0)
LEFT JOIN nodes n2 ON n2.id = pss.nodeID2
left join externalCodes ec1 ON ec1.id = n1.externalCodeID
left join externalCodes ec2 ON ec2.id = n2.externalCodeID
LEFT JOIN defectTypes deft ON deft.id = d.remontTypeID
left join tipPoverhnosti tp on tp.id = d.tipPoverhnostiID
left join externalSigns  es on es.id = d.priznak_truboprovoda 
left join vid_narusheniya vn on vn.id = d.vid_narusheniyaID
left join remontCat rc ON rc.id = d.remontCatID
left join ulitsy st ON st.id = d.ulicaID
left join opres op ON op.id = d.opresID
left join tipPovrezhdenia tpov ON  tpov.id = d.tipPovrezhdeniaID
left join sostTeploizol sostT ON sostT.id = d.sostTeploizolID
left join sostNaruzhnoiPoverkhnosti sostNP ON sostNP.id = d.sostNaruzhnoiPoverkhnostiID
left join sostVnutrenneiPoverkhnosti sostVP ON sostVP.id = d.sostVnutrenneiPoverkhnostiID
left join vid_rabot vid_rabot ON vid_rabot.id = d.vid_rabotID
left join defectStates defectStates ON defectStates.id = d.stateID
left join remontIzolKonstruktsii rik on rik.id = d.remontIzolKonstruktsiiID
left join remont_kamery remont_kamery on remont_kamery.id = d.remont_kameryID
left join remont_kanala remont_kanala on remont_kanala.id = d.remont_kanalaID
left join nodes n_otkl_1 on n_otkl_1.id = d.nodeID1
left join nodes n_otkl_2 on n_otkl_2.id = d.nodeID2 
left join externalCodes ec1_otkl ON ec1_otkl.id = n_otkl_1.externalCodeID
left join externalCodes ec2_otkl ON ec2_otkl.id = n_otkl_2.externalCodeID
/*
LEFT JOIN nachalniki_uchastkov nach ON nach.id=obj.responsibleID
LEFT JOIN dolzhnosti d ON d.id=nach.dolzhnost
LEFT JOIN subdivisions subd ON subd.id=subdivisionID
left join uchastok_ms ms ON ms.id = srt.uchastok_ms_id
left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
--left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
left join uchastok_rs rs ON rs.id = srt.uchastok_rs_id
left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
--left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
*/

WHERE 
--and n1.fileID in (58)
( (not ec1.name in ('П1','П2') or not ec2.name in ('П1','П2')) or (ec1.name is null AND ec2.name is null) )

)t

where t.min_len = t.length

order by t.[Дата осмотра] desc


--196