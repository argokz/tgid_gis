
select top 10000000
t.obj_id as id,
CONCAT(FORMAT(t.data_osmotra,'yyyy-MM-dd' ),' ',t.vremya_osmotra) as 'Дата обнаружения повреждения',
IIF (ps1.magistralSite is not NULL, re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Район',
IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as '№ т/м присоединения',
IIF (ps1.magistralSite is not NULL, 'магистраль', 'распредсеть') as 'Признак сети', 
IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Начальная точка',
IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Конечная точка',
t.rasstoyanieDoPovrezhdeniyaNachKamery as 'Расстояние до нарушения от ближайшей камеры, м',
tube_type.name as 'Тип прокладки',
tp.name as 'Поверхность в месте раскопки',
es.name as 'Поврежденный трубопровод',
hps.diameterExternal as 'Диаметр наружний',
hps.wallThickness as 'Толщина стенки',
deft.name as 'Режим',
vn.name as 'Вид нарушения',
es_dr.name as 'Дренируемый трубопровод',
'Поврежденный элемент' =
		STUFF(
		( SELECT concat(',', rtt2.name) AS n FROM defect r2 
		LEFT JOIN povrezhdennyiElementForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN povrezhdennyiElement rtt2 ON rtt2.id=rt2.activityID
		WHERE r2.id=t.obj_id 
		FOR XML PATH('')
		, TYPE
		).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
tpov.name as 'Тип повреждения',
t.tsentrPovrezhdenia as 'Место расположения центра повреждения на трубопроводе,  часов',
t.vysotaPovrezhdenia as 'Высота повреждения',
t.shirinaPovrezhdenia as 'Ширина повреждения',
t.ploshchadPovrezhdenia as 'Площадь повреждения',
 'Организационные причины' =
        STUFF(
        ( SELECT concat(',', rtt2.name) AS n FROM defect r2 
        LEFT JOIN prichiny_narusheniya_organizacionnye_for_defect rt2 ON r2.id=rt2.objID
        LEFT JOIN prichiny_narusheniya_organizacionnye rtt2 ON rtt2.id=rt2.activityID
        WHERE r2.id=t.obj_id 
        FOR XML PATH('')
        , TYPE
        ).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
'Причины повреждения' =
		STUFF(
		( SELECT concat(',', rtt2.name) AS n FROM defect r2 
		LEFT JOIN prichinyPovrezhdeniaForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN prichinyPovrezhdenia rtt2 ON rtt2.id=rt2.activityID
		WHERE r2.id=t.obj_id
		FOR XML PATH('')
		, TYPE
		).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
'Сопутствующие причины' =
	STUFF(
	( SELECT concat(',', rtt2.name) AS n FROM defect r2 
	LEFT JOIN soputstvuiushchiePrichinyForDefect rt2 ON r2.id=rt2.objID
	LEFT JOIN soputstvuiushchiePrichiny rtt2 ON rtt2.id=rt2.activityID
	WHERE r2.id=t.obj_id
	FOR XML PATH('')
	, TYPE
	).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),

vid_rabot.name as 'Вид ремонта',
'Ремонт трубопровода и элементов' = 
		STUFF(
		( SELECT distinct concat(IIF(rtt2.name != '', CONCAT(',', rtt2.name, ':'), ''), STUFF (
			(select concat(',',se.name) as n from defectTube dt
		LEFT JOIN spisokElementov se ON se.id=dt.elementID
		where objId = t.obj_id and activityID = rt2.activityID
		FOR XML PATH('')
		, TYPE
		).value('.', 'NVARCHAR(MAX)'), 1, 1, '')
) AS n FROM defect r2 
		LEFT JOIN defectTube rt2 ON r2.id=rt2.objID
		LEFT JOIN remontTruboprovodaSpisok rtt2 ON rtt2.id=rt2.activityID
		WHERE r2.id=t.obj_id
		FOR XML PATH('')
		, TYPE
		).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
CONCAT(t.shirinaZaplatki,' мм, ' ,t.vysotaZaplatki, ' мм') as 'Размер заплатки',
t.len_tube_cur as 'Длина заменённой трубы, м',
rik.name as 'Ремонт изоляционной конструкции',
remont_kanala.name as 'Ремонт канала',
remont_kamery.name as 'Ремонт камеры',
'Состояние конструкции канала' = 
	STUFF(
	( SELECT concat(',', rtt2.name) AS n FROM defect r2 
	LEFT JOIN sostKonstruktsiiKanalaForDefect rt2 ON r2.id=rt2.objID
	LEFT JOIN sostKonstruktsiiKanala rtt2 ON rtt2.id=rt2.activityID
	WHERE r2.id=t.obj_id 
	FOR XML PATH('')
	, TYPE
	).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),		
'Состояние конструкции камеры' = 
	STUFF(
	( SELECT concat(',', rtt2.name) AS n FROM defect r2 
	LEFT JOIN sostKonstruktsiiKameryForDefect rt2 ON r2.id=rt2.objID
	LEFT JOIN sostKonstruktsiiKamery rtt2 ON rtt2.id=rt2.activityID
	WHERE r2.id=t.obj_id
	FOR XML PATH('')
	, TYPE
	).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	sostT.name as 'Состояние теплоизоляции теплопроводов',
	sostNP.name as 'Состояние наружной поверхности теплопроводов',
	sostVP.name as 'Состояние внутренней поверхности поврежденной трубы',
	IIF (uzel_nach_otcl.nodeName is NULL or uzel_nach_otcl.nodeName = '' or uzel_nach_otcl.nodeName = ' ',uzel_nach_otcl.externalNodeName, uzel_nach_otcl.nodeName) as 'Начальная камера отключения от сети',
	IIF (uzel_konec_otcl.nodeName is NULL or uzel_konec_otcl.nodeName = '' or uzel_konec_otcl.nodeName = ' ',uzel_konec_otcl.externalNodeName, uzel_konec_otcl.nodeName) as 'Конечная камера отключения от сети',
	t.data_nachala_remonta as 'Дата начала ремонтных работ',
	t.vremiaNachalaRemonta as 'Время начала ремонтных работ',
	t.data_zaversheniya_remonta as 'Дата завершения ремонтных работ',
	t.vremiaZaversheniaRemonta as 'Время завершения  ремонтных работ',
	t.trudozatratyNaRemont as 'Трудозатраты на ремонт',
	t.stoimostRemonta as 'Стоимость ремонтных работ, тг',
	IIF (ps1.magistralSite is not NULL, dolz_nu_ms.znachenie, dolz_nu_rs.znachenie) as 'Должность',
	IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка'
from (
	select
		distinct
		d.id as 'obj_id',
		l.id as 'lineID',
		d.data_osmotra,
		d.vremya_osmotra,
		d.rasstoyanieDoPovrezhdeniyaNachKamery,
		d.tipPoverhnostiID,
		d.priznak_truboprovoda,
		d.remontTypeID,
		d.vid_rabotID,
		d.dreniruemyj_truboprovodID,
		d.tipPovrezhdeniaID,
		d.tsentrPovrezhdenia,
		d.vysotaPovrezhdenia,
		d.shirinaPovrezhdenia,
		d.ploshchadPovrezhdenia,
		d.shirinaZaplatki,
		d.vysotaZaplatki,
		d.len_tube_cur,
		d.remont_kameryID,
		d.remont_kanalaID,
		d.data_nachala_remonta, 
		d.vremiaNachalaRemonta, 
		d.data_zaversheniya_remonta, 
		d.vremiaZaversheniaRemonta, 
		d.trudozatratyNaRemont, 
		d.stoimostRemonta, 
		d.vid_narusheniyaID,
		d.nodeID1,
		d.nodeID2,
		d.remontCatID,
		d.sostTeploizolID,
		d.remontIzolKonstruktsiiID,
		d.sostNaruzhnoiPoverkhnostiID,
		d.sostVnutrenneiPoverkhnostiID
	from defect d
	JOIN linesobj l ON l.id = (select top 1
	k.lineID as lineID
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
	d.id = $fileID$
	)k
	where k.min_len = k.length 
	)
	where d.id = $fileID$
)t
LEFT JOIN heatPipeSections hps ON hps.lineID=t.lineID
left join tubingTypes tube_type ON tube_type.id = hps.tubingTypeID
LEFT JOIN pipeSections ps1 ON ps1.id = hps.pipeSectionID
JOIN nodes n1 ON ( n1.id = ps1.nodeID1 and n1.removed = 0)
LEFT JOIN nodes n2 ON n2.id = ps1.nodeID2
left join nodes uzel_nach_otcl on uzel_nach_otcl.id = t.nodeID1 
left join nodes uzel_konec_otcl on uzel_konec_otcl.id = t.nodeID2 
left join externalCodes ec1 ON ec1.id = n1.externalCodeID
left join externalCodes ec2 ON ec2.id = n2.externalCodeID
LEFT JOIN defectTypes deft ON deft.id = t.remontTypeID
left join tipPoverhnosti tp on tp.id = t.tipPoverhnostiID
left join externalSigns  es on es.id = t.priznak_truboprovoda
left join externalSigns  es_dr on es_dr.id = t.dreniruemyj_truboprovodID 
left join vid_narusheniya vn on vn.id = t.vid_narusheniyaID
left join remontCat rc ON rc.id = t.remontCatID
left join vid_rabot on vid_rabot.id = t.vid_rabotID
left join tipPovrezhdenia tpov ON  tpov.id = t.tipPovrezhdeniaID
left join remont_kamery on remont_kamery.id = t.remont_kameryID
left join remont_kanala on remont_kanala.id = t.remont_kanalaID


left join sostTeploizol sostT ON sostT.id = t.sostTeploizolID
left join remontIzolKonstruktsii rik on rik.id = t.remontIzolKonstruktsiiID
left join sostNaruzhnoiPoverkhnosti sostNP ON sostNP.id = t.sostNaruzhnoiPoverkhnostiID
left join sostVnutrenneiPoverkhnosti sostVP ON sostVP.id = t.sostVnutrenneiPoverkhnostiID



left join uchastok_ms ms ON ms.id = ps1.magistralSite
left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka

left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka

left join uchastok_rs rs ON rs.id = ps1.distSite
left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka

left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka

LEFT join dolzhnosti dolz_nu_rs on dolz_nu_rs.id = nu_rs.dolzhnost
LEFT join dolzhnosti dolz_nu_ms on dolz_nu_ms.id = nu_ms.dolzhnost

order by t.data_osmotra desc
