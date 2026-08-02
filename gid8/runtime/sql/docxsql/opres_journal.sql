select 
	obj.id,
	obj.name as 'Наименование/Адрес контура',
	obj.opisaniye_kontura as 'Описание контура',
	ist.naimenovanie as 'Источник тепла',
	ot.name as 'Вид испытания',
	sostoyanie_opres.name as 'Состояние',
	obj.data_utverzhdeniya_plana as 'Дата утверждения плана',
	'Утверждение' = IIF(obj.utverdit = 1, 'Утверждено', 'Не утверждено'),
	obj.nodeOprID1 as 'Узел Опрессовочного насоса',
	objekt_opressovochnogo_nasosa.name as 'Объект Опрессовочного насоса',
	obj.data_nachala_plan as 'Дата начала опрессовки, план',
	obj.data_okonchaniya_plan as 'Дата окончания опрессовки план',
	obj.date_opres as 'Дата проведения опрессовки',
	obj.vremya_provedeniya_opressovki as 'Время проведения опрессовки',
	obj.prodolzhitelnost_opressovki as 'Продолжительность опрессовки',
	obj.reshenie_komissii as 'Решение комиссии',
	obj.davlenie_opressovki_1_etap as 'Давление опрессовки 1 этапа, кгс/см2',
	obj.davlenie_opressovki_2_etap as 'Давление опрессовки 2 этапа, кгс/см2',
	obj.temperatura_raskholazhivaniya_kontura as 'Температура расхолаживания контура, Град.С',
	obj.kolichestvo_zvenjev_obhodchikov as 'Количество звеньев обходчиков',
	obj.data_utverzhdeniya_akta_ispytanij as 'Дата утверждения акта испытаний',
	obj.ne_preduprezhdennye_potrebiteli as 'Не предупрежденные потребители',
	obj.primechanie as 'Примечание', 
	obj.fio_rukovoditel_ispytanij as 'ФИО руководителя испытаний',
	dolzhnosti_ruk.znachenie as 'Должность руководителя испытаний',
	subd_ruk.name as 'Подразделение руководителя испытаний',
	fio_utverzhdaemogo as 'ФИО утверждающего',
	dolzhnost_utverzhdaemogoID as 'Должность утвердаещего',
	podrazdelenie_utverzhdaemogoID as 'Подразделение утверждающего',

	fio_otvetstvennyj_za_obespechenie_rezhimov as 'ФИО ответственного за обеспечения режимов',
	dolzhnost_otvetstvennyj_za_obespechenie_rezhimov.znachenie as 'Должность ответственного за обеспечения режимов' ,
	podrazdelenie_otvetstvennyj_za_obespechenie_rezhimov.name as 'Подразделение ответственного за обеспечения режимов',

	fio_otvetstvennyj_za_blank_pereklyuchenij as 'ФИО ответственного за бланк переключений',
	dolzhnost_otvetstvennyj_za_blank_pereklyuchenij.znachenie as 'Должность ответственного за бланк переключений',
	podrazdelenie_otvetstvennyj_za_blank_pereklyuchenij.name as 'Подразделение ответственного за бланк переключений',

	fio_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov as 'ФИО ответственного за установку манометров и расходомеров',
	dolzhnost_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov.znachenie as 'Должность ответственного за установку манометров и расходомеров',
	podrazdelenie_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov.name as 'Подразделение  ответственного за установку манометров и расходомеров',


	fio_otvetstvennyj_za_obespechenie_avtotransportom as 'ФИО ответственного за обеспечение  автотранспортом',
	dolzhnost_otvetstvennyj_za_obespechenie_avtotransportom.znachenie as 'Должность ответственного за обеспечение  автотранспортом' ,
	podrazdelenie_otvetstvennyj_za_obespechenie_avtotransportom.name as 'Подразделение ответственного за обеспечение  автотранспортом',

	fio_otvetstvennyj_za_priem_zayavok_ot_potrebitelej_na_uchastie_v_sovmestnoj_opressovke as 'ФИО ответсвенного за прием   заявок  от  потребителей  на  участие  в  совместной  опрессовке ',
	dolzhnost_otvetstvennyj_za_priem_zayavok_ot_potrebitelej_na_uchastie_v_sovmestnoj_opressovke.znachenie as 'Должность ответсвенного за прием   заявок  от  потребителей  на  участие  в  совместной  опрессовке',
	podrazdelenie_otvetstvennyj_za_priem_zayavok_ot_potrebitelej_na_uchastie_v_sovmestnoj_opressovke.name as 'Подразделение ответсвенного за прием   заявок  от  потребителей  на  участие  в  совместной  опрессовке',


	fio_otvetstvennyj_za_obespechenie_raboty_elektrooborudovaniya as 'ФИО ответственного за обеспечение работы электрооборудования',
	dolzhnost_otvetstvennyj_za_obespechenie_raboty_elektrooborudovaniya.znachenie as 'Должность за обеспечение работы электрооборудования',
	podrazdelenie_otvetstvennyj_za_obespechenie_raboty_elektrooborudovaniya.name as 'Подразделение за обеспечение работы электрооборудования',

	fio_otvetstvennyj_po_snip_kontura_istochnika_tepla as 'ФИО ответственного по СНиП контура источника тепла',
	dolzhnost_otvetstvennyj_po_snip_kontura_istochnika_tepla.znachenie as 'Должность ответственного по СНиП контура источника тепла',
	podrazdelenie_otvetstvennyj_po_snip_kontura_istochnika_tepla.name as 'Подразделение ответственного по СНиП контура источника тепла',


	fio_otvetstvennyj_za_opoveshchenie_naseleniya_o_ispytaniyah as 'ФИО ответственного за оповещение  населения о испытаниях',
	dolzhnost_otvetstvennyj_za_opoveshchenie_naseleniya_o_ispytaniyah.znachenie as 'Должность ответственного за оповещение  населения о испытаниях',
	podrazdelenie_otvetstvennyj_za_opoveshchenie_naseleniya_o_ispytaniyah.name as 'Подразделение ответственного за оповещение  населения о испытаниях'
from opres obj
LEFT JOIN dolzhnosti  ON dolzhnosti.id=obj.dolzhnost_rukovoditel_ispytanijID
LEFT JOIN subdivisions subd ON subd.id=obj.podrazdelenie_rukovoditel_ispytanijID
LEFT JOIN istochniki_tepla ist ON ist.id=obj.istochnik_tepla
left join opres_types ot on ot.id = obj.opres_typeID
left join sostoyanie_opres on sostoyanie_opres.id = obj.sostoyanie_opresID
left join nodes n1 on n1.id = obj.nodeOprID1
LEFT JOIN dolzhnosti dolzhnosti_ruk  ON dolzhnosti_ruk.id=obj.dolzhnost_rukovoditel_ispytanijID
LEFT JOIN subdivisions subd_ruk ON subd.id=obj.podrazdelenie_rukovoditel_ispytanijID
left join objekt_opressovochnogo_nasosa on objekt_opressovochnogo_nasosa.id = obj.objekt_opressovochnogo_nasosaID


LEFT JOIN dolzhnosti dolzhnost_utverzhdaemogo on dolzhnost_utverzhdaemogo.id = obj.dolzhnost_utverzhdaemogoID
LEFT JOIN subdivisions podrazdelenie_utverzhdaemogo on podrazdelenie_utverzhdaemogo.id = obj.podrazdelenie_utverzhdaemogoID

LEFT JOIN dolzhnosti dolzhnost_otvetstvennyj_za_obespechenie_rezhimov on dolzhnost_otvetstvennyj_za_obespechenie_rezhimov. id = obj.dolzhnost_otvetstvennyj_za_obespechenie_rezhimovID
LEFT JOIN subdivisions	podrazdelenie_otvetstvennyj_za_obespechenie_rezhimov on podrazdelenie_otvetstvennyj_za_obespechenie_rezhimov.id = obj.podrazdelenie_otvetstvennyj_za_obespechenie_rezhimovID


LEFT JOIN dolzhnosti dolzhnost_otvetstvennyj_za_blank_pereklyuchenij on dolzhnost_otvetstvennyj_za_blank_pereklyuchenij.id = obj.dolzhnost_otvetstvennyj_za_blank_pereklyuchenijID
LEFT JOIN subdivisions podrazdelenie_otvetstvennyj_za_blank_pereklyuchenij on podrazdelenie_otvetstvennyj_za_blank_pereklyuchenij.id = obj.podrazdelenie_otvetstvennyj_za_blank_pereklyuchenijID


LEFT JOIN dolzhnosti dolzhnost_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov on dolzhnost_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov.id = obj.dolzhnost_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerovID 
LEFT JOIN subdivisions podrazdelenie_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov on podrazdelenie_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerov.id = obj.podrazdelenie_otvetstvennyj_za_ustanovku_manometrov_i_raskhodomerovID



LEFT JOIN dolzhnosti dolzhnost_otvetstvennyj_za_obespechenie_avtotransportom on dolzhnost_otvetstvennyj_za_obespechenie_avtotransportom.id = obj.dolzhnost_otvetstvennyj_za_obespechenie_avtotransportomID
LEFT JOIN subdivisions podrazdelenie_otvetstvennyj_za_obespechenie_avtotransportom on podrazdelenie_otvetstvennyj_za_obespechenie_avtotransportom.id = obj.podrazdelenie_otvetstvennyj_za_obespechenie_avtotransportomID

LEFT JOIN dolzhnosti dolzhnost_otvetstvennyj_za_priem_zayavok_ot_potrebitelej_na_uchastie_v_sovmestnoj_opressovke on dolzhnost_otvetstvennyj_za_priem_zayavok_ot_potrebitelej_na_uchastie_v_sovmestnoj_opressovke.id = obj.dolzhnost_otvetstvennyj_za_priem_zayavok_ot_potrebitelej_na_uchastie_v_sovmestnoj_opressovkeID
LEFT JOIN subdivisions podrazdelenie_otvetstvennyj_za_priem_zayavok_ot_potrebitelej_na_uchastie_v_sovmestnoj_opressovke on podrazdelenie_otvetstvennyj_za_priem_zayavok_ot_potrebitelej_na_uchastie_v_sovmestnoj_opressovke.id = obj.podrazdelenie_otvetstvennyj_za_priem_zayavok_ot_potrebitelej_na_uchastie_v_sovmestnoj_opressovkeID


LEFT JOIN dolzhnosti dolzhnost_otvetstvennyj_za_obespechenie_raboty_elektrooborudovaniya on dolzhnost_otvetstvennyj_za_obespechenie_raboty_elektrooborudovaniya.id  = obj.dolzhnost_otvetstvennyj_za_obespechenie_raboty_elektrooborudovaniyaID
LEFT JOIN subdivisions podrazdelenie_otvetstvennyj_za_obespechenie_raboty_elektrooborudovaniya on podrazdelenie_otvetstvennyj_za_obespechenie_raboty_elektrooborudovaniya.id = obj.podrazdelenie_otvetstvennyj_za_obespechenie_raboty_elektrooborudovaniyaID

	
LEFT JOIN dolzhnosti dolzhnost_otvetstvennyj_po_snip_kontura_istochnika_tepla on dolzhnost_otvetstvennyj_po_snip_kontura_istochnika_tepla.id = obj.dolzhnost_otvetstvennyj_po_snip_kontura_istochnika_teplaID
LEFT JOIN subdivisions podrazdelenie_otvetstvennyj_po_snip_kontura_istochnika_tepla on podrazdelenie_otvetstvennyj_po_snip_kontura_istochnika_tepla.id = obj.podrazdelenie_otvetstvennyj_po_snip_kontura_istochnika_teplaID


LEFT JOIN dolzhnosti dolzhnost_otvetstvennyj_za_opoveshchenie_naseleniya_o_ispytaniyah on dolzhnost_otvetstvennyj_za_opoveshchenie_naseleniya_o_ispytaniyah.id = dolzhnost_otvetstvennyj_za_opoveshchenie_naseleniya_o_ispytaniyahID
LEFT JOIN subdivisions podrazdelenie_otvetstvennyj_za_opoveshchenie_naseleniya_o_ispytaniyah on podrazdelenie_otvetstvennyj_za_opoveshchenie_naseleniya_o_ispytaniyah.id = obj.podrazdelenie_otvetstvennyj_za_opoveshchenie_naseleniya_o_ispytaniyahID


WHERE obj.id = $fileID$