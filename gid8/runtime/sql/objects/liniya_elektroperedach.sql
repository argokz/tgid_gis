SELECT 
	le.id, 
	pe.naimenovanie_priemnika_es as 'Наименование приемника',
	tpe.znachenie as 'Тип приемника',
	ie.naimenovanie_istochnika_es as 'Наименование источника',
	te.znachenie as 'Тип источника',
	le.mestopolozhenie as 'Местоположение/Описание ЛЭП',
	le.napryazhenie__kv as 'Напряжение, кВ',
	tl.znachenie as 'Тип прокладки кабеля',
	mk.polnoe_naimenovanie_marki_tipa__kabelya as 'Марка кабеля линии',
	le.count_lep as 'Количество линий ЛЭП, шт',
	le.protyazhennost__linii_m as 'Протяженность линии, м',
	le.protyazhennost__m as 'Протяженность кабеля, м', 
	le.fidera as 'Рабочие фидера',
	le.fidera_rez as 'Резервные фидера',
	le.data_vvoda_v_ekspluatatsiyu as 'Дата ввода в эксплуатацию', 
	vld.naimenovanie as 'Владелец ЛЭП',
	le.primechanie as 'Примечание'
FROM liniya_elektroperedach le
LEFT JOIN priemnik_elektrosnabzheniya pe on pe.id = le.naimenovanie_priemnika
LEFT JOIN tipy_priemnikov_elektricheskih_setey tpe on tpe.id = pe.typID
LEFT JOIN istochnik_elektrosnabzheniya ie on ie.id = le.naimenovanie_istochnika
LEFT JOIN tipy_istochnikov_elektricheskih_setey te on te.id = ie.typID
LEFT JOIN tipy_lep tl on tl.id = le.tip_prokladki_lep
LEFT JOIN marki_kabeley_es mk on mk.id = le.marka_kabelya_linii
LEFT JOIN vladeltsy_es vld on vld.id = le.vladelets_lep