select distinct top 2147483647
  t.obj_id as id,
    deft.name as 'Режим',
    FORMAT(t.data_osmotra,'dd.MM.yyyy' ) as 'Дата обнаружения нарушения',
    t.vremya_osmotra as 'Время обнаружения повреждения',
    CONCAT(st.name, ' ', t.nomer_doma) as 'Адрес нарушения',
    es.name as 'Поврежденный трубопровод',
    t.rasstoyanieDoPovrezhdeniyaNachKamery as 'Расстояние до нарушения от ближайшей камеры, м',
    tp.name as 'Поверхность в месте раскопки',
    t.defectDescription as 'Описание повреждения',
    t.nomer_akta as 'Номер акта',
	t.nomer_prikaza as 'Номер приказа',
    vn.name as 'Вид нарушения',
    rc.name as 'Категория нарушения',
    tpov.name as 'Характер повреждения',
    t.tsentrPovrezhdenia as 'Место расположения центра повреждения на трубопроводе,  часов ',
    t.vysotaPovrezhdenia as 'Высота повреждения',
    t.shirinaPovrezhdenia as 'Ширина повреждения',
	t.ploshchadPovrezhdenia as 'Площадь повреждения',
    sostT.name as 'Состояние теплоизоляции теплопроводов',
    sostNP.name as 'Состояние наружной поверхности теплопроводов',
    sostVP.name as 'Состояние внутренней поверхности поврежденной трубы',
	stateDefect.name as 'Состояние Нарушения',
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
			l.lineID,
			organizations.name as 'organization',
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
			d.remont_kameryID,
			shurfy.id as shurfID,
			shurfy.naznachenie_vskrID as naznachenie_vskrID_shurf,
			shurfy.ulicaID as ulicaID_shurf,
			shurfy.nomer_doma as nomer_doma_shurf,
			shurfy.sostoyanie_shurfaID,
			shurfy.data_nachala as data_nachala_shurf,
			shurfy.data_okonchaniya as data_okonchaniya_shurf
		from defect d
		JOIN (
			select 
				k.lineID,
				k.obj_id,
				k.organizationID,
				k.shurfID
			from (
				select
					distinct
						l.id as lineID,
						d.id as obj_id,
						ds.objID as shurfID,
						l.organizationID,
						l.shape.STDistance(d.shape) as length,
						MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len"
				from defect d
				join defectsForShurfy ds on ds.defectID = d.id
				JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1 )
			)k
		where k.min_len = k.length 
	) l on l.obj_id = d.id 
	left join shurfy on shurfy.id = l.shurfID
	left join organizations on organizations.id = l.organizationID
)t

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
left join tipPovrezhdenia tpov ON  tpov.id = t.tipPovrezhdeniaID
left join sostTeploizol sostT ON sostT.id = t.sostTeploizolID
left join sostNaruzhnoiPoverkhnosti sostNP ON sostNP.id = t.sostNaruzhnoiPoverkhnostiID
left join sostVnutrenneiPoverkhnosti sostVP ON sostVP.id = t.sostVnutrenneiPoverkhnostiID
left join remont_kanala on remont_kanala.id = t.remont_kanalaID
left join remont_kamery on remont_kamery.id = t.remont_kameryID




join defectsForShurfy dfd ON dfd.defectID = t.obj_id


WHERE 
t.shurfID=$ID$
