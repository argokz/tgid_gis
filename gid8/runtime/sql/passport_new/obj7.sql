with ll AS (


SELECT l.id,l.shape FROM linesobj l 
JOIN nodes n1 ON n1.id=l.nodeID1 
JOIN nodes n2 ON n2.id=l.nodeID2 
JOIN externalCodes ec1 ON ec1.id=n1.externalCodeID
JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID
--JOIN heatPipeSections hps ON hps.lineID=l.id

WHERE n1.removed=0 AND l.removed=0 
--AND ISNULL(n1.internalNodeID, 0)=0
--AND COALESCE(n1.internalNodeID, 0) = 0
--AND n1.internalNodeID IS NULL 
AND n1.fileID < 1000
--AND n1.fileID = 58
AND not (ec1.name in ('П1','П2') AND not ec2.name in ('П1','П2')) 

--AND hps.magistralSite=89

),

l_obj1 AS (

SELECT 
ll.id AS l_id,
d.id AS obj_id,
ll.shape.STDistance(d.shape.STStartPoint()) AS len,
min(ll.shape.STDistance(d.shape.STStartPoint())) OVER(PARTITION BY d.id) AS minlen

FROM defect d 
JOIN  ll ON ll.shape.STDistance(d.shape.STStartPoint()) < 1
--ORDER BY l_id, obj_id
),

l_obj2 AS (
SELECT 
COUNT(*) OVER(PARTITION BY obj_id) AS cnt,
ROW_NUMBER() OVER(PARTITION BY obj_id ORDER BY l_id) AS rown,
* FROM l_obj1
WHERE len=minlen
),

l_obj AS (
SELECT l_id, obj_id FROM l_obj2
WHERE rown = 1
),

------------------------------------------------

d1 AS (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM povrezhdennyiElementForDefect el
JOIN povrezhdennyiElement el2 ON el2.id=el.activityID
GROUP BY el.objID
),

d2 AS (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM prichinyPovrezhdeniaForDefect el
JOIN prichinyPovrezhdenia el2 ON el2.id=el.activityID
GROUP BY el.objID
),

d3 AS (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM prichiny_narusheniya_organizacionnye_for_defect el
JOIN prichiny_narusheniya_organizacionnye el2 ON el2.id=el.activityID
GROUP BY el.objID
),

d4 AS (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM soputstvuiushchiePrichinyForDefect el
JOIN soputstvuiushchiePrichiny el2 ON el2.id=el.activityID
GROUP BY el.objID
),

d5 AS (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM soputstvuiushchiePrichinyForDefect el
JOIN soputstvuiushchiePrichiny el2 ON el2.id=el.activityID
GROUP BY el.objID
),

d6 AS (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM sostKonstruktsiiKanalaForDefect el
JOIN sostKonstruktsiiKanala el2 ON el2.id=el.activityID
GROUP BY el.objID
),

d7 AS (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM sostKonstruktsiiKameryForDefect el
JOIN sostKonstruktsiiKamery el2 ON el2.id=el.activityID
GROUP BY el.objID
),

d8 AS (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM prichinyPovrezhdeniaForDefect el
JOIN prichinyPovrezhdenia el2 ON el2.id=el.activityID
GROUP BY el.objID
),

d9 AS (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM prichinyPovrezhdeniaForDefect el
JOIN prichinyPovrezhdenia el2 ON el2.id=el.activityID
GROUP BY el.objID
),

d10 AS (
SELECT 
objID AS id, STRING_AGG(el2.name, ',') AS v
FROM prichinyPovrezhdeniaForDefect el
JOIN prichinyPovrezhdenia el2 ON el2.id=el.activityID
GROUP BY el.objID
),

d11 AS (

SELECT 
objID AS id,
STRING_AGG(v, ', ') AS v

FROM (
SELECT 

objID, 
CONCAT(name, IIF(v IS NULL, '',  ' ('+v+')')) AS v

FROM (

SELECT 
dt.objID,rt.name, STRING_AGG(se.name, ', ') AS v
FROM defectTube dt
LEFT JOIN spisokElementov se ON se.id=dt.elementID
LEFT JOIN remontTruboprovodaSpisok rt ON rt.id=dt.activityID
where se.id IS NOT NULL OR rt.id IS NOT NULL
GROUP BY dt.objID,rt.ord,rt.name
) _
) _
GROUP BY objID

)

------------------------------------------------

, 
defect_all AS (
SELECT 
d.id,
ec1.name AS kod1,
n1.externalNodeName AS uzel1,
IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as name1,

ec2.name AS kod2,
n2.externalNodeName AS uzel2,

IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as name2,

d1.v AS v1, 
d2.v AS v2, 
d3.v AS v3, 
d4.v AS v4, 
d5.v AS v5, 
d6.v AS v6, 
d7.v AS v7, 
d8.v AS v8, 
d9.v AS v9, 
d10.v AS v10, 
d11.v AS v11,

deft.name AS deft_name,
sd.name AS sd_name,
rc.name AS rc_name,
nach.fio AS nach_fio,

--d.lineID,
d.stateID,
d.name AS df_name,
d.data_osmotra,
d.vremya_osmotra,
d.otchet_po_defektu,
d.len_tube_cur,
d.len_izol_cur,
d.len_channel_cur,
d.data_nachala_remonta,
d.data_zaversheniya_remonta,
d.defectDescription,
--d.remontTypeID,
--d.remontCatID,

d.subdivisionID,
d.responsibleID,
d.primechanie,
d.opresID,

d.defectsForDiagID,
d.priznak_truboprovoda,
d.mestoPovrezhdenijaID,
d.tipPoverhnostiID,
d.povrezhdennyiElementID,
d.tipPovrezhdeniaID,
d.prichinyPovrezhdeniaID,

d.soputstvuiushchiePrichinyID,
d.sostKonstruktsiiKanalaID,
d.sostKonstruktsiiKameryID,
d.sostTeploizolID,
d.sostNaruzhnoiPoverkhnostiID,
d.sostVnutrenneiPoverkhnostiID,
d.tsentrPovrezhdenia,
d.vysotaPovrezhdenia,

d.shirinaPovrezhdenia,
d.ploshchadPovrezhdenia,
d.stoimostRemonta,
d.shirinaZaplatki,
d.vysotaZaplatki,
d.remontIzolKonstruktsiiID,
d.rasstoyanieDoPovrezhdeniyaNachKamery,
d.brigadesID,
d.vremiaNachalaRemonta,

d.vremiaZaversheniaRemonta,
d.meropriyatiya,
d.trudozatratyNaRemont,
d.osmotrID,
d.ulicaID,
d.nomer_doma,
d.vid_narusheniyaID,
d.prichiny_narusheniya_organizacionnyeID,
d.sostoyanie_teploizolyacii_teploprovodaID,

d.kolichestvo_otklyuchennyh_potrebitelej,
d.zatraty_na_vosstanovlenie,
d.inye_socialnye_posledstviya,
d.nodeID1,
d.nodeID2,
d.nomer_akta,
d.nomer_prikaza,
d.vid_rabotID,
d.dreniruemyj_truboprovodID,
d.kamery_otkrytiya_drenazhejID,

d.komissiya_rassledovaniyaID,
d.data_shurfovki,
d.data_sostavleniya_akta,
d.prikaz_vvoda_v_ekspluataciyu,
d.kolichestvo_nedootpushchennoj_teplovoj_energii,
d.predsedatel,
d.dolzhnost_predsedatelya,
d.chlen_tn_1,
d.dolzhnost_tn_1,

d.chlen_tn_2,
d.dolzhnost_tn_2,
d.chlen_tn_3,
d.dolzhnost_tn_3,
d.remont_kameryID,
d.remont_kanalaID,
d.materialy_i_mekhanizmyID,
d.show_map,
d.data_prikaza_vvoda_v_ekspluataciyu,
d.nodeID_bizhajshej_kamery,


--ist_ms.naimenovanie AS ist_ms_naimenovanie,
--ist_rs.naimenovanie AS ist_rs_naimenovanie,
ms.opisanie_uchastka_ms AS ms_opisanie_uchastka_ms,
hps.magistralSite AS pss_magistralSite,
re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla AS re_rs_naimenovanie_rayona_ekspluatatsii_istochnika_tepla,
rs.naimenovanie_uchastka_rs AS rs_naimenovanie_uchastka_rs,
ue_ms.nomer_uchastka AS ue_ms_nomer_uchastka,
ue_rs.nomer_uchastka AS ue_rs_nomer_uchastka,

--nu_ms.fio AS nu_ms_fio,
--nu_rs.fio AS nu_rs_fio,

--node_bizhajshej_kamery.nodeName AS node_bizhajshej_kamery_nodeName,
--node_bizhajshej_kamery.externalNodeName AS node_bizhajshej_kamery_externalNodeName,

--uzel_nach_otcl.nodeName AS uzel_nach_otcl_nodeName,
--uzel_nach_otcl.externalNodeName AS uzel_nach_otcl_externalNodeName,
--uzel_konec_otcl.nodeName AS uzel_konec_otcl_nodeName,
--uzel_konec_otcl.externalNodeName AS uzel_konec_otcl_externalNodeName,




--im.name AS im_name,
--isolationTypes.name AS isolationTypes_name,
--externalMaterials.name AS externalMaterials_name,
--anticorrMaterials.name AS anticorrMaterials_name,
remontIzolKonstruktsii.name AS remontIzolKonstruktsii_name,

remont_kanala.name AS remont_kanala_name,
remont_kamery.name AS remont_kamery_name,

--tubingTypes.name AS tubingTypes_name,

magistrali.naimenovanie_magistrali AS magistrali_naimenovanie_magistrali,
re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla AS re_ms_naimenovanie_rayona_ekspluatatsii_istochnika_tepla,

--st.name AS st_name,
--tp.name AS tp_name,
--es.name AS es_name,
--op.name AS op_name,
vn.name AS vn_name,
vn.code AS vn_code,
vid_rabot.name AS vid_rabot_name,
sostT.name AS sostT_name,
sostNP.name AS sostNP_name,
sostVP.name AS sostVP_name,
tpov.name AS tpov_name,

--organizations.name AS organization,

hps.firstPICdateHP,
hps.lastTransDate,
hps.diameterExternal,
hps.wallThickness,
hps.pipeSectLength


---- d.transfer_flag,
---- d.remontID,
---- d.remontNodeID


FROM defect d
JOIN l_obj ON l_obj.obj_id=d.id
JOIN linesobj l ON l.id=l_obj.l_id
JOIN heatPipeSections hps ON hps.lineID=l.id
LEFT JOIN pipeSections pss ON pss.id = hps.pipeSectionID

JOIN nodes n1 ON n1.id=l.nodeID1 
JOIN nodes n2 ON n2.id=l.nodeID2 
JOIN externalCodes ec1 ON ec1.id=n1.externalCodeID
JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID


LEFT JOIN d1 ON d1.id=d.id
LEFT JOIN d2 ON d2.id=d.id
LEFT JOIN d3 ON d3.id=d.id
LEFT JOIN d4 ON d4.id=d.id
LEFT JOIN d5 ON d5.id=d.id
LEFT JOIN d6 ON d6.id=d.id
LEFT JOIN d7 ON d7.id=d.id
LEFT JOIN d8 ON d8.id=d.id
LEFT JOIN d9 ON d9.id=d.id
LEFT JOIN d10 ON d10.id=d.id
LEFT JOIN d11 ON d11.id=d.id


LEFT JOIN defectTypes deft ON deft.id = d.remontTypeID
LEFT JOIN stateDefect sd ON sd.id = d.stateID
LEFT JOIN remontCat rc ON rc.id = d.remontCatID 
LEFT JOIN nachalniki_uchastkov nach ON nach.id=d.responsibleID

LEFT JOIN dolzhnosti dolzh ON dolzh.id=nach.dolzhnost
LEFT JOIN subdivisions subd ON subd.id=subdivisionID

left join magistrali on magistrali.id = hps.magistral
left join uchastok_ms ms ON ms.id = hps.magistralSite
left join uchastok_rs rs ON rs.id = hps.distSite

left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka

left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii

--left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
--left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka


--LEFT JOIN istochniki_tepla ist_ms ON ist_ms.id=ue_ms.istochnik_tepla
--LEFT JOIN istochniki_tepla ist_rs ON ist_rs.id=ue_rs.istochnik_tepla

--left join organizations on organizations.id = l.organizationID 

--LEFT JOIN tubingTypes ON tubingTypes.id=hps.tubingTypeID
--left join isolMaterials im on im.id = hps.isolMaterialID
--left join isolationTypes on isolationTypes.id = hps.isolationTypeID
--left join externalMaterials on externalMaterials.id = hps.externMaterialID
--left join anticorrMaterials on anticorrMaterials.id = hps.anticorrMaterialID

--left join ulitsy st ON st.id = d.ulicaID

left join vid_narusheniya vn on vn.id = d.vid_narusheniyaID
left join vid_rabot on vid_rabot.id = d.vid_rabotID
left join tipPovrezhdenia tpov ON tpov.id = d.tipPovrezhdeniaID
left join sostTeploizol sostT ON sostT.id = d.sostTeploizolID
left join sostNaruzhnoiPoverkhnosti sostNP ON sostNP.id = d.sostNaruzhnoiPoverkhnostiID
left join sostVnutrenneiPoverkhnosti sostVP ON sostVP.id = d.sostVnutrenneiPoverkhnostiID

--left join tipPoverhnosti tp on tp.id = d.tipPoverhnostiID
--left join externalSigns es on es.id = d.priznak_truboprovoda 
--left join opres op ON op.id = d.opresID

--left join nodes node_bizhajshej_kamery on node_bizhajshej_kamery.id = d.nodeID_bizhajshej_kamery
--left join nodes uzel_nach_otcl on uzel_nach_otcl.id = d.nodeID1 
--left join nodes uzel_konec_otcl on uzel_konec_otcl.id = d.nodeID2 

left join remontIzolKonstruktsii on remontIzolKonstruktsii.id = d.remontIzolKonstruktsiiID
left join remont_kanala on remont_kanala.id = d.remont_kanalaID
left join remont_kamery on remont_kamery.id = d.remont_kameryID



--left join nodes uzel_nach_otcl on uzel_nach_otcl.id = t.nodeID1 
--left join nodes uzel_konec_otcl on uzel_konec_otcl.id = t.nodeID2 


--left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
--left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
--left join uchastok_rs rs ON rs.id = srt.uchastok_rs_id
--left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
--left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka


--WHERE d1.id IS NOT NULL OR d2.id IS NOT NULL OR d3.id IS NOT NULL OR d4.id IS NOT NULL OR d5.id IS NOT NULL OR d6.id IS NOT NULL OR d7.id IS NOT NULL OR d8.id IS NOT NULL OR d9.id IS NOT NULL OR d10.id IS NOT NULL OR d11.id IS NOT NULL

)

------------------------------------------------


SELECT 
/*
id,

CONCAT ( d.vysotaPovrezhdenia,', мм | ', d.shirinaPovrezhdenia,', мм | ',d.ploshchadPovrezhdenia, ', мм') as 'Размер повреждения',

    predsedatel,
    dolzhnost_predsedatelya,
    chlen_tn_1,
    dolzhnost_tn_1,
    chlen_tn_2,
    dolzhnost_tn_2,
    chlen_tn_3,
    dolzhnost_tn_3,
* 

*/

    id as id,
	name1 as 'Наименование начального узла',
	kod1 as 'Код расчетной схемы начального узла',
	name2 as 'Наименование конечного узла',
	kod2 as 'Код расчетной схемы конечного узла',

--	es.name as 'Признак участка трубопровода',
--	firstPICdateHP as 'Дата первичного ввода в эксплуатацию',
--	lastTransDate as 'Дата последней перекладки',

--	es.name as 'Признак участка трубопровода',
	firstPICdateHP as 'Дата первичного ввода в эксплуатацию',
	lastTransDate as 'Дата последней перекладки',
	--hpss.netType as 'Вид сети',
	magistrali_naimenovanie_magistrali as 'Магистраль',
	IIF (pss_magistralSite is not NULL, re_ms_naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs_naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Район эксплуатации',
	IIF (pss_magistralSite is not NULL, ue_ms_nomer_uchastka, ue_rs_nomer_uchastka) as 'Участок эксплуатации',
--	IIF (pss_magistralSite is not NULL, ist_ms_naimenovanie, ist_rs_naimenovanie) as 'Источник теплоснабжения',
	IIF (ms_opisanie_uchastka_ms is not NULL, ms_opisanie_uchastka_ms, rs_naimenovanie_uchastka_rs) as 'Наименование участка',
	IIF (pss_magistralSite is not NULL, 'магистраль', 'распредсеть') as 'Признак сети',
--	t.organization as 'Владелец участка теплопровода',
--	IIF (nu_ms_fio is not NULL, nu_ms_fio, nu_rs_fio) as 'Начальник участка',
--	tubingTypes_name as 'Тип прокладки',

	diameterExternal as 'Диаметр наружный, мм',
	wallThickness AS 'Толщина стенки, мм',
	pipeSectLength AS 'Длина участка теплопровода, м',

--	im_name as 'Материал тепловой изоляции',
--	isolationTypes_name as 'Тип изоляции ППУ',
--	externalMaterials_name as 'Материал наружного покрытия',
--	anticorrMaterials_name as 'Материал антикоррозийного покрытия',
    deft_name as 'Режим',
---    FORMAT(t.data_osmotra,'yyyy-MM-dd' ) as 'Дата обнаружения нарушения',
    FORMAT(t.data_osmotra,'dd.MM.yyyy' ) as 'Дата обнаружения нарушения',
    t.vremya_osmotra as 'Время обнаружения повреждения',
--    CONCAT(st_name, '', t.nomer_doma) as 'Адрес',
--    es_name as 'Поврежденный трубопровод',
    t.rasstoyanieDoPovrezhdeniyaNachKamery as 'Расстояние до нарушения от ближайшей камеры, м',
--	IIF (node_bizhajshej_kamery_nodeName is NULL or node_bizhajshej_kamery_nodeName = '' or node_bizhajshej_kamery_nodeName = ' ',node_bizhajshej_kamery_externalNodeName, node_bizhajshej_kamery_nodeName) as 'Наименование ближайшей камеры',
--    tp_name as 'Поверхность в месте раскопки',
    t.defectDescription as 'Описание повреждения',
--    op_name as 'Наименование контура опрессовки',
	t.nomer_akta as 'Номер акта',
	t.data_sostavleniya_akta as 'Дата составления акта',
	t.nomer_prikaza as 'Номер приказа',
	t.data_prikaza_vvoda_v_ekspluataciyu as 'Дата приказа ввода в эксплуатацию',
    vn_code as 'Код нарушения',
    vn_name as 'Вид нарушения',
    vid_rabot_name as 'Вид работ',
    rc_name as 'Категория нарушения',
--    tpov_name as 'Характер повреждения',
    t.tsentrPovrezhdenia as 'Место расположения центра повреждения на трубопроводе,  часов ',
    t.vysotaPovrezhdenia as 'Высота повреждения',
    t.shirinaPovrezhdenia as 'Ширина повреждения',
	t.ploshchadPovrezhdenia as 'Площадь повреждения',
    sostT_name as 'Состояние теплоизоляции теплопроводов',
    sostNP_name as 'Состояние наружной поверхности теплопроводов',
    sostVP_name as 'Состояние внутренней поверхности поврежденной трубы',
	sd_name as 'Состояние',
--	IIF (uzel_nach_otcl_nodeName is NULL or uzel_nach_otcl_nodeName = '' or uzel_nach_otcl_nodeName = ' ',uzel_nach_otcl_externalNodeName, uzel_nach_otcl_nodeName) as 'Начальная камера отключения от сети',
--	IIF (uzel_konec_otcl_nodeName is NULL or uzel_konec_otcl_nodeName = '' or uzel_konec_otcl_nodeName = ' ',uzel_konec_otcl_externalNodeName, uzel_konec_otcl_nodeName) as 'Конечная камера отключения от сети',
    t.meropriyatiya as 'Способ ликвидации нарушения',
--	t.data_shurfovki as 'Дата шурфовки',
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
		remontIzolKonstruktsii_name as 'Ремонт теплоизоляционной конструкции',
		t.len_izol_cur as 'Длина заменённой изоляции, м',
		remont_kanala_name as 'Ремонт канала',
		t.len_channel_cur as 'Длина участка ремонта канала',
		remont_kamery_name as 'Ремонт камеры'		




FROM defect_all t
--WHERE stateID=1
