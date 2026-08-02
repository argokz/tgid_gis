SELECT 
	le.id, 
	pe.naimenovanie_priemnika_es,
	tpe.znachenie as 'tip_priemnika',
	ie.naimenovanie_istochnika_es,
	te.znachenie as 'tip_istochnika',
	le.mestopolozhenie,
	le.napryazhenie__kv,
	tl.znachenie as 'tip_prokladki_kabelya',
	mk.polnoe_naimenovanie_marki_tipa__kabelya as 'marka_kabelya',
	le.count_lep,
	le.protyazhennost__linii_m,
	le.protyazhennost__m, 
	le.fidera,
	le.fidera_rez, isp_s,
	le.data_vvoda_v_ekspluatatsiyu, 
	vld.naimenovanie as 'vladelets',
	le.primechanie
FROM liniya_elektroperedach le
LEFT JOIN priemnik_elektrosnabzheniya pe on pe.id = le.naimenovanie_priemnika
LEFT JOIN tipy_priemnikov_elektricheskih_setey tpe on tpe.id = pe.typID
LEFT JOIN istochnik_elektrosnabzheniya ie on ie.id = le.naimenovanie_istochnika
LEFT JOIN tipy_istochnikov_elektricheskih_setey te on te.id = ie.typID
LEFT JOIN tipy_lep tl on tl.id = le.tip_prokladki_lep
LEFT JOIN marki_kabeley_es mk on mk.id = le.marka_kabelya_linii
LEFT JOIN vladeltsy_es vld on vld.id = le.vladelets_lep
ORDER BY naimenovanie_priemnika_es, data_vvoda_v_ekspluatatsiyu desc