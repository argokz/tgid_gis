select distinct
  obj.id as id,
	obj.otchet_po_defektu as 'Наименование/Адрес',
	obj.teplovaya_setID as 'Тепловая сеть',
	obj.harakteristika_uchastkov_remontiruemoj_teplovoj_seti as 'Характеристика участков ремонтируемой тепловой сети',
	rt.name as 'Вид ремонта',
	st.name as 'Состояние',
	obj.data_nachala_plan as 'Дата начала ремонта плановая',
	obj.data_okonchaniya_plan as 'Дата окончания ремонта плановая',
	obj.len_tube_cur as 'Длина заменённых труб, план, м',
	obj.diametr_trub_plan as 'Диаметр условный заменённых труб, план, мм',
	obj.len_izol_plan as 'Восстановление тепловой изоляции поверхности труб, план, м2',
	obj.len_channel_plan as 'Восстановление канальной прокладки, план, м',
	obj.asfaltirovanie_plan as 'Асфальтирование, план, м2',
	obj.vydelennye_sredstva_plan as 'Объем выделенных средств и затрат, тыс.тг. с НДС',
	obj.remontnyj_personal_plan as 'Ремонтный персонал, план, тыс.чел.'


from remont2 obj
	join remont2Deployed d on d.directionID = obj.id
	JOIN heatPipeSections hpss ON hpss.lineID=d.lineID $and_condition2$
	JOIN pipeSections pss ON pss.id=hpss.pipeSectionID
	join linesobj l on l.id = d.lineID
	JOIN nodes n1 ON n1.id=pss.nodeID1
	JOIN nodes n2 ON n2.id=pss.nodeID2
	LEFT JOIN nodeTypes nt1 ON nt1.id=n1.nodeTypeID
	LEFT JOIN nodeTypes nt2 ON nt2.id=n2.nodeTypeID

	left join remontTypes rt on rt.id = obj.remontTypeID
	left join stateRemont2 st on st.id = obj.stateID
	left join subdivisions sb on sb.id = obj.subdivisionID 
--	left join responsibles rs on rs.id = obj.responsibleID
	left join nachalniki_uchastkov rs on rs.id = obj.responsibleID
  


	left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = pss.id and faktory_riska_truboprovoda.objID = obj.id and faktory_riska_truboprovoda.obj_type_faktory_riskaID = 3
	LEFT JOIN faktRiska_4_elekt Elektrich on Elektrich.id = faktory_riska_truboprovoda.Elektrich
	LEFT JOIN faktRiska_5_transport TransportElekricht on TransportElekricht.id = faktory_riska_truboprovoda.TransportElekricht
	LEFT JOIN faktRiska_15_peshehod peshehod on peshehod.id = faktory_riska_truboprovoda.Pesehod
	LEFT JOIN tubingWays on tubingWays.id = faktory_riska_truboprovoda.ZhdDorogi
	LEFT JOIN faktRiska_17_skoplenie_zhit LudiProklad on LudiProklad.id = faktory_riska_truboprovoda.LudiProklad
	left join poverhnost_nad_trassoj on poverhnost_nad_trassoj.id = faktory_riska_truboprovoda.poverhnost_nad_trassojID
	left join sprav.dbo.[01_teploprovodnost_grunta] harakter_grunta_shurf  on harakter_grunta_shurf.id = faktory_riska_truboprovoda.harakter_gruntaID
	left join YesOrNo yn_podtoplenie_do_truby on yn_podtoplenie_do_truby.id = faktory_riska_truboprovoda.podtoplenie_do_truby
	left join sost_konstr on sost_konstr.id=faktory_riska_truboprovoda.SostKonstrukz
	left join sostoyanie_stroitelnyh_konstrukcij_kanala on sostoyanie_stroitelnyh_konstrukcij_kanala.id = faktory_riska_truboprovoda.sostoyanie_stroitelnyh_konstrukcij_kanalaID
	left join vnutrennee_sostoyanie_kanala on vnutrennee_sostoyanie_kanala.id = faktory_riska_truboprovoda.vnutrennee_sostoyanie_kanalaID
	left join konstrukciya_drenazhnogo_ustrojstva on konstrukciya_drenazhnogo_ustrojstva.id = faktory_riska_truboprovoda.konstrukciya_drenazhnogo_ustrojstvaID
	LEFT JOIN sectWaterDumps on sectWaterDumps.id = faktory_riska_truboprovoda.Vodootved
	left join vneshny_vid on vneshny_vid.id=faktory_riska_truboprovoda.VnesniiVid
	left join sost_oborud on sost_oborud.id=faktory_riska_truboprovoda.SostOborudovania
	left join nalichie_korrozii_shurf nalichie_korrozii_podacha on nalichie_korrozii_podacha.id = faktory_riska_truboprovoda.nalichie_korrozii_podachaID
	left join nalichie_korrozii_shurf nalichie_korrozii_obratka on nalichie_korrozii_obratka.id = faktory_riska_truboprovoda.nalichie_korrozii_obratkaID
	left join sostoyanie_metalla_truboprovoda on sostoyanie_metalla_truboprovoda.id = faktory_riska_truboprovoda.sostoyanie_metalla_truboprovodaID
	left join sostoyanie_teplovoj_izolyacii sostoyanie_teplovoj_izolyacii_obratka on sostoyanie_teplovoj_izolyacii_obratka.id = faktory_riska_truboprovoda.sostoyanie_teplovoj_izolyacii_obratkaID
	left join sostoyanie_teplovoj_izolyacii sostoyanie_teplovoj_izolyacii_podacha on sostoyanie_teplovoj_izolyacii_podacha.id = faktory_riska_truboprovoda.sostoyanie_teplovoj_izolyacii_podachaID
	left join sostoyanie_naruzhnogo_pokrytiya sostoyanie_naruzhnogo_pokrytiya_obratka on sostoyanie_naruzhnogo_pokrytiya_obratka.id = faktory_riska_truboprovoda.sostoyanie_naruzhnogo_pokrytiya_obratkaID
	left join sostoyanie_naruzhnogo_pokrytiya sostoyanie_naruzhnogo_pokrytiya_podacha on sostoyanie_naruzhnogo_pokrytiya_podacha.id = faktory_riska_truboprovoda.sostoyanie_naruzhnogo_pokrytiya_podachaID
	left join sostoyanie_protivokorrozionnogo_pokrytiya_shurf sostoyanie_protivokorrozionnogo_pokrytiya_podacha on sostoyanie_protivokorrozionnogo_pokrytiya_podacha.id = faktory_riska_truboprovoda.sostoyanie_protivokorrozionnogo_pokrytiya_podachaID
	left join sostoyanie_protivokorrozionnogo_pokrytiya_shurf sostoyanie_protivokorrozionnogo_pokrytiya_obratka on sostoyanie_protivokorrozionnogo_pokrytiya_obratka.id = faktory_riska_truboprovoda.sostoyanie_protivokorrozionnogo_pokrytiya_obratkaID
	left join isolMaterials im on im.id = faktory_riska_truboprovoda.isolMaterialID
	left join isolationTypes on isolationTypes.id = faktory_riska_truboprovoda.isolationTypeID
	left join externalMaterials on externalMaterials.id = faktory_riska_truboprovoda.externMaterialID
	left join anticorrMaterials on anticorrMaterials.id = faktory_riska_truboprovoda.anticorrMaterialID

WHERE
  ((obj.data_nachala_plan $season_condition$) OR
  (obj.data_nachala_remonta $season_condition$)) 
  AND 
    $condition$



