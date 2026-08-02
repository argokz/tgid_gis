select distinct top 2147483647
    t.obj_id as id,
    deft.name as 'Режим',
    FORMAT(t.data_osmotra,'dd.MM.yyyy' ) as 'Дата обнаружения нарушения',
    t.vremya_osmotra as 'Время обнаружения повреждения',
    CONCAT(st.name, ' ', t.nomer_doma) as 'Адрес',
    es.name as 'Поврежденный трубопровод',
    t.rasstoyanieDoPovrezhdeniyaNachKamery as 'Расстояние до нарушения от ближайшей камеры, м',
    tp.name as 'Поверхность в месте раскопки',
    t.defectDescription as 'Описание повреждения',
    op.name as 'Наименование контура опрессовки',
	t.nomer_akta as 'Номер акта',
	t.nomer_prikaza as 'Номер приказа',
    vn.name as 'Вид нарушения',
    rc.name as 'Категория нарушения',
    'Поврежденный элемент' =
		STUFF(
		( SELECT concat(',', rtt2.name) AS n FROM defect r2 
		LEFT JOIN povrezhdennyiElementForDefect rt2 ON r2.id=rt2.objID
		LEFT JOIN povrezhdennyiElement rtt2 ON rtt2.id=rt2.activityID
		WHERE r2.id=t.obj_id 
		FOR XML PATH('')
		, TYPE
		).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
    tpov.name as 'Характер повреждения',
    t.tsentrPovrezhdenia as 'Место расположения центра повреждения на трубопроводе,  часов ',
    t.vysotaPovrezhdenia as 'Высота повреждения',
    t.shirinaPovrezhdenia as 'Ширина повреждения',
	t.ploshchadPovrezhdenia as 'Площадь повреждения',
    'Причины нарушения (технические)' =
        STUFF(
        ( SELECT concat(',', rtt2.name) AS n FROM defect r2 
        LEFT JOIN prichinyPovrezhdeniaForDefect rt2 ON r2.id=rt2.objID
        LEFT JOIN prichinyPovrezhdenia rtt2 ON rtt2.id=rt2.activityID
        WHERE r2.id=t.obj_id 
        FOR XML PATH('')
        , TYPE
        ).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
    'Причины нарушения (организационные) ' =
        STUFF(
        ( SELECT concat(',', rtt2.name) AS n FROM defect r2 
        LEFT JOIN prichiny_narusheniya_organizacionnye_for_defect rt2 ON r2.id=rt2.objID
        LEFT JOIN prichiny_narusheniya_organizacionnye rtt2 ON rtt2.id=rt2.activityID
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
	stateDefect.name as 'Состояние',
	IIF (uzel_nach_otcl.nodeName is NULL or uzel_nach_otcl.nodeName = '' or uzel_nach_otcl.nodeName = ' ',uzel_nach_otcl.externalNodeName, uzel_nach_otcl.nodeName) as 'Начальная камера отключения от сети',
	IIF (uzel_konec_otcl.nodeName is NULL or uzel_konec_otcl.nodeName = '' or uzel_konec_otcl.nodeName = ' ',uzel_konec_otcl.externalNodeName, uzel_konec_otcl.nodeName) as 'Конечная камера отключения от сети',
    t.meropriyatiya as 'Способ ликвидации нарушения',
	t.data_shurfovki as 'Дата шурфовки',
    t.data_nachala_remonta as 'Дата начала ремонтных работ',
    t.vremiaNachalaRemonta as 'Время начала ремонтных работ',
    t.data_zaversheniya_remonta as 'Дата завершения ремонтных работ',
    t.vremiaZaversheniaRemonta as 'Время завершения  ремонтных работ',
	t.trudozatratyNaRemont as 'Трудозатраты на ремонт',
	t.stoimostRemonta as 'Стоимость ремонтных работ, тг',
	t.otchet_po_defektu as 'Примечание',
	t.kolichestvo_otklyuchennyh_potrebitelej as 'Количество отключенных потребителей',
	t.kolichestvo_nedootpushchennoj_teplovoj_energii as 'Количество недоотпущенной тепловой энергии, Гкал',
	t.zatraty_na_vosstanovlenie as 'Затраты на восстановление, тг',
	t.inye_socialnye_posledstviya as 'Иные социальные последствия',
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
        , TYPE ).value('.', 'NVARCHAR(MAX)'), 1, 1, ''),
		t.shirinaZaplatki as 'Ширина заплатки',
		t.vysotaZaplatki as 'Высота заплатки',
		t.len_tube_cur as 'Длина заменённой трубы, м',
		remontIzolKonstruktsii.name as 'Ремонт теплоизоляционной конструкции',
		t.len_izol_cur as 'Длина заменённой изоляции, м',
		remont_kanala.name as 'Ремонт канала',
		t.len_channel_cur as 'Длина участка ремонта канала',
		remont_kamery.name as 'Ремонт камеры'		
from (
		select
			distinct
            hps.pipeSectionID,
			l.id as 'lineID',
			d.id as obj_id,
			d.remontTypeID,
			d.data_osmotra,
			d.vremya_osmotra,
			d.nomer_doma,
			d.ulicaID,
			d.priznak_truboprovoda,
			d.rasstoyanieDoPovrezhdeniyaNachKamery,
			d.tipPoverhnostiID,
			d.dreniruemyj_truboprovodID,
			d.defectDescription,
			d.opresID,
			d.nomer_akta,
			d.nomer_prikaza,
			d.vid_narusheniyaID,
			d.remontCatID,
			d.stateID,
			d.sostTeploizolID,
			d.sostKonstruktsiiKameryID,
			d.sostKonstruktsiiKanalaID,
			d.sostNaruzhnoiPoverkhnostiID,
			d.sostVnutrenneiPoverkhnostiID,
			d.vid_rabotID,
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
			d.remont_kameryID		
		from defect d
		JOIN heatPipeSections hps ON hps.pipeSectionID = (select top 1
	k.psID as psID
	from (
        select
            distinct
                hps.pipeSectionID as psID,
                l.id as lineID,
                d.id as obj_id,
                l.shape.STDistance(d.shape) as length,
                MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len"
        from defect d
        JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1 )
        left join heatPipeSections hps on hps.lineID = l.id 
        WHERE d.id = $id$
	) k
	where k.min_len = k.length 
	)
    JOIN linesobj l on l.id = hps.lineID AND l.shape.STDistance(d.shape) < 0.1
)t
--$and_condition2$
/*
LEFT JOIN heatPipeSections hps ON hps.lineID=t.lineID
--left join tubingTypes tube_type ON tube_type.id = hps.tubingTypeID
LEFT JOIN pipeSections ps1 ON ps1.id = hps.pipeSectionID
JOIN nodes n1 ON ( n1.id = ps1.nodeID1 and n1.removed = 0)
LEFT JOIN nodes n2 ON n2.id = ps1.nodeID2
left join externalCodes ec1 ON ec1.id = n1.externalCodeID
left join externalCodes ec2 ON ec2.id = n2.externalCodeID
*/

left join nodes uzel_nach_otcl on uzel_nach_otcl.id = t.nodeID1 
left join nodes uzel_konec_otcl on uzel_konec_otcl.id = t.nodeID2 
LEFT JOIN defectTypes deft ON deft.id = t.remontTypeID
left join stateDefect on stateDefect.id = t.stateID
left join tipPoverhnosti tp on tp.id = t.tipPoverhnostiID
left join externalSigns  es on es.id = t.priznak_truboprovoda 
left join vid_narusheniya vn on vn.id = t.vid_narusheniyaID
left join remontCat rc ON rc.id = t.remontCatID
left join remontIzolKonstruktsii on remontIzolKonstruktsii.id = t.remontIzolKonstruktsiiID
left join ulitsy st ON st.id = t.ulicaID
left join opres op ON op.id = t.opresID
left join tipPovrezhdenia tpov ON  tpov.id = t.tipPovrezhdeniaID
left join sostTeploizol sostT ON sostT.id = t.sostTeploizolID
left join sostNaruzhnoiPoverkhnosti sostNP ON sostNP.id = t.sostNaruzhnoiPoverkhnostiID
left join sostVnutrenneiPoverkhnosti sostVP ON sostVP.id = t.sostVnutrenneiPoverkhnostiID
left join remont_kanala on remont_kanala.id = t.remont_kanalaID
left join remont_kamery on remont_kamery.id = t.remont_kameryID


--WHERE 
   --n1.fileID in (58) and
    --( (not ec1.name in ('П1','П2') or not ec2.name in ('П1','П2')) or (ec1.name is null AND ec2.name is null) ) 
	--$and_condition$

--ord22er2 by t.data_osmotra desc
