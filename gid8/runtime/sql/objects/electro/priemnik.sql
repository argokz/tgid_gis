SELECT 	pe.id,
		pe.naimenovanie_priemnika_es,
		tpe.znachenie as 'typ_priemnika',
		pe.maksimalno_dopustimaya_nagruzka_vneshnego_vvoda_rp__kvt,
		pe.trans_pro_kol, 
		pe.trans_sob_kol,	
		pe.edv_pod_kol, 
		pe.edv_obr_kol, 
		pe.edv_dren_kol, 
		pe.edv_opr_kol, 
		pe.diz_kol, 
		pe.chast_reg_reg,		
		vld.naimenovanie as 'vladelets', 
		pe.primechanie
FROM priemnik_elektrosnabzheniya pe
LEFT JOIN tipy_priemnikov_elektricheskih_setey tpe on tpe.id = pe.typID
LEFT JOIN vladeltsy_es vld on vld.id = pe.vladeltsy_es_ID
ORDER BY naimenovanie_priemnika_es