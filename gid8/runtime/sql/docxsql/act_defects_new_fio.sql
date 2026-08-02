select top 10000000
	t.obj_id as id,
	IIF (pss.magistralSite is not NULL, re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Район эксплуатации',
	IIF (pss.magistralSite is not NULL, ue_ms.nomer_uchastka, ue_rs.nomer_uchastka) as 'Участок эксплуатации',
	magistrali.naimenovanie_magistrali as 'Магистраль',
	IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as 'Наименование участка',
	IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка',
	IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'Наименование начального узла',
	ec1.name as 'Код расчетной схемы начального узла',
	IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'Наименование конечного узла',
	ec2.name as 'Код расчетной схемы конечного узла',
	FORMAT(t.data_osmotra,'yyyy-MM-dd' ) as 'Дата осмотра',
	t.vremya_osmotra as 'Время осмотра',
	CONCAT(st.name, ' ', t.nomer_doma) as 'Адрес',
	es.name as 'Поврежденный трубопровод',
	t.rasstoyanieDoPovrezhdeniyaNachKamery as 'Расстояние до ближайшей камеры, м', 
	IIF (node_bizhajshej_kamery.nodeName is NULL or node_bizhajshej_kamery.nodeName = '' or node_bizhajshej_kamery.nodeName = ' ',node_bizhajshej_kamery.externalNodeName, node_bizhajshej_kamery.nodeName) as 'Наименование ближайшей камеры',
	vn.code as 'Вид нарушения',
	rc.name as 'Категория нарушения',
	'Поврежденный элемент' =
	STUFF(
	( SELECT concat(',', rtt2.name) AS n
	FROM defect r2
		LEFT JOIN povrezhdennyiElementForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN povrezhdennyiElement rtt2 ON rtt2.id=rt2.activityID
	WHERE r2.id=t.obj_id
	FOR XML PATH('')
	, TYPE
	).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	tpov.name as 'Характер повреждения',
	t.tsentrPovrezhdenia as 'Место расположения центра повреждения на трубопроводе,  часов ',
	CONCAT ( t.vysotaPovrezhdenia,', мм | ', t.shirinaPovrezhdenia,', мм | ',t.ploshchadPovrezhdenia, ', мм') as 'Размер повреждения',
	'Причины нарушения (технические)' =
	STUFF(
	( SELECT concat(',', rtt2.name) AS n
	FROM defect r2
		LEFT JOIN prichinyPovrezhdeniaForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN prichinyPovrezhdenia rtt2 ON rtt2.id=rt2.activityID
	WHERE r2.id=t.obj_id
	FOR XML PATH('')
	, TYPE
	).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	'Причины нарушения (организационные)' =
	STUFF(
	( SELECT concat(',', rtt2.name) AS n
	FROM defect r2
		LEFT JOIN prichiny_narusheniya_organizacionnye_for_defect rt2 ON r2.id=rt2.objID
		LEFT JOIN prichiny_narusheniya_organizacionnye rtt2 ON rtt2.id=rt2.activityID
	WHERE r2.id=t.obj_id
	FOR XML PATH('')
	, TYPE
	).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	'Сопутствующие причины' =
	STUFF(
	( SELECT concat(',', rtt2.name) AS n
	FROM defect r2
		LEFT JOIN soputstvuiushchiePrichinyForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN soputstvuiushchiePrichiny rtt2 ON rtt2.id=rt2.activityID
	WHERE r2.id=t.obj_id
	FOR XML PATH('')
	, TYPE
	).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	'Состояние конструкции канала' = 
	STUFF(
	( SELECT concat(',', rtt2.name) AS n
	FROM defect r2
		LEFT JOIN sostKonstruktsiiKanalaForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN sostKonstruktsiiKanala rtt2 ON rtt2.id=rt2.activityID
	WHERE r2.id=t.obj_id
	FOR XML PATH('')
	, TYPE
	).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	'Состояние конструкции камеры' = 
	STUFF(
	( SELECT concat(',', rtt2.name) AS n
	FROM defect r2
		LEFT JOIN sostKonstruktsiiKameryForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN sostKonstruktsiiKamery rtt2 ON rtt2.id=rt2.activityID
	WHERE r2.id=t.obj_id
	FOR XML PATH('')
	, TYPE
	).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
	sostT.name as 'Состояние теплоизоляции теплопроводов',
	sostNP.name as 'Состояние наружной поверхности теплопроводов',
	sostVP.name as 'Состояние внутренней поверхности поврежденной трубы',
	t.meropriyatiya as 'Способ ликвидации нарушения',
	t.kolichestvo_otklyuchennyh_potrebitelej as 'Количество отключенных потребителей',
	CONCAT(t.data_nachala_remonta, ' ', t.vremiaNachalaRemonta) as 'Начала ремонта',
	CONCAT(t.data_zaversheniya_remonta, ' ', t.vremiaZaversheniaRemonta)  as 'Окончание ремонта',
	t.kolichestvo_nedootpushchennoj_teplovoj_energii as 'Количество недоотпущенной тепловой энергии, Гкал',
	t.zatraty_na_vosstanovlenie as 'Затраты на восстановление, тг',
	t.inye_socialnye_posledstviya as 'Иные социальные последствия',
	dolz_p.znachenie as 'Должность председателя',
	t.predsedatel as 'ФИО председателя',
	dolz_1.znachenie as 'Должность члена 1',
	t.chlen_tn_1 as 'ФИО 1',
	dolz_2.znachenie as 'Должность члена 2',
	t.chlen_tn_2 as 'ФИО 2',
	dolz_3.znachenie as 'Должность члена 3',
	t.chlen_tn_3 as 'ФИО 3'
from (
select
		distinct
		d.id as obj_id,
		l.id as lineID,
		d.data_osmotra,
		d.vremya_osmotra,
		d.ulicaID,
		d.nomer_doma,
		d.remontTypeID,
		d.priznak_truboprovoda,
		d.rasstoyanieDoPovrezhdeniyaNachKamery,
        d.nodeID_bizhajshej_kamery,
		d.tipPoverhnostiID,
		d.defectDescription,
		d.vid_narusheniyaID,
		d.remontCatID,
		d.tipPovrezhdeniaID,
		d.tsentrPovrezhdenia,
		d.vysotaPovrezhdenia,
		d.shirinaPovrezhdenia,
		d.ploshchadPovrezhdenia,
		d.shirinaZaplatki,
		d.vysotaZaplatki,
		d.remontIzolKonstruktsiiID,
		d.sostTeploizolID,
		d.sostNaruzhnoiPoverkhnostiID,
		d.sostVnutrenneiPoverkhnostiID,
		d.meropriyatiya,
		d.data_nachala_remonta,
		d.vremiaNachalaRemonta,
		d.data_zaversheniya_remonta,
		d.vremiaZaversheniaRemonta,
		d.kolichestvo_otklyuchennyh_potrebitelej,
		d.kolichestvo_nedootpushchennoj_teplovoj_energii,
		d.zatraty_na_vosstanovlenie,
		d.inye_socialnye_posledstviya,
		d.dolzhnost_predsedatelya,
		d.predsedatel,
		d.dolzhnost_tn_1,
		d.dolzhnost_tn_2,
		d.dolzhnost_tn_3,
		d.chlen_tn_1,
		d.chlen_tn_2,
		d.chlen_tn_3
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

	LEFT JOIN heatPipeSections hpss ON hpss.lineID=t.lineID
	left join tubingTypes tube_type ON tube_type.id = hpss.tubingTypeID
	LEFT JOIN pipeSections pss ON pss.id = hpss.pipeSectionID
	JOIN nodes n1 ON ( n1.id = pss.nodeID1 and n1.removed = 0)
	LEFT JOIN nodes n2 ON n2.id = pss.nodeID2
	left join externalCodes ec1 ON ec1.id = n1.externalCodeID
	left join externalCodes ec2 ON ec2.id = n2.externalCodeID
	left join nodes node_bizhajshej_kamery on node_bizhajshej_kamery.id = t.nodeID_bizhajshej_kamery
	LEFT JOIN defectTypes deft ON deft.id = t.remontTypeID
	left join tipPoverhnosti tp on tp.id = t.tipPoverhnostiID
	left join externalSigns  es on es.id = t.priznak_truboprovoda
	left join vid_narusheniya vn on vn.id = t.vid_narusheniyaID
	left join remontCat rc ON rc.id = t.remontCatID
	left join ulitsy st ON st.id = t.ulicaID
	left join tipPovrezhdenia tpov ON  tpov.id = t.tipPovrezhdeniaID
	left join sostTeploizol sostT ON sostT.id = t.sostTeploizolID
	left join remontIzolKonstruktsii rik on rik.id = t.remontIzolKonstruktsiiID
	left join sostNaruzhnoiPoverkhnosti sostNP ON sostNP.id = t.sostNaruzhnoiPoverkhnostiID
	left join sostVnutrenneiPoverkhnosti sostVP ON sostVP.id = t.sostVnutrenneiPoverkhnostiID

	left join magistrali on magistrali.id = hpss.magistral

	left join uchastok_ms ms ON ms.id = pss.magistralSite
	left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka

	left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
	left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka

	left join uchastok_rs rs ON rs.id = pss.distSite
	left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka

	left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
	left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka



	left join dolzhnosti dolz_p on dolz_p.id = t.dolzhnost_predsedatelya
	left join dolzhnosti dolz_1 on dolz_1.id = t.dolzhnost_tn_1
	left join dolzhnosti dolz_2 on dolz_2.id = t.dolzhnost_tn_2
	left join dolzhnosti dolz_3 on dolz_3.id = t.dolzhnost_tn_3

--WHERE 
--and n1.fileID in (58)
--( (not ec1.name in ('П1','П2') or not ec2.name in ('П1','П2')) or (ec1.name is null AND ec2.name is null) )

order by t.data_osmotra desc