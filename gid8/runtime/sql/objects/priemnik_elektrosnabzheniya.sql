SELECT 	pe.id,
		pe.naimenovanie_priemnika_es as 'Ќаименование приемника Ё—',
		tpe.znachenie as '“ип приемника',
		pe.maksimalno_dopustimaya_nagruzka_vneshnego_vvoda_rp__kvt as 'ћаксимально-допустима€ нагрузка внешнего ввода, к¬т',
		pe.trans_pro_kol as '“рансформаторы производственные, кол-во', 
		pe.trans_sob_kol as '“рансформаторы, собственные нужды, кол-во',	
		pe.edv_pod_kol as 'Ёлектродвигатели по подаче, кол-во', 
		pe.edv_obr_kol as 'Ёлектродвигатели по обратке, кол-во', 
		pe.edv_dren_kol as 'Ёлектродвигатели дренажные, кол-во', 
		pe.edv_opr_kol as 'Ёлектродвигатели опрессовочные, кол-во', 
		pe.diz_kol as 'ƒизель-генераторна€ установка, кол-во', 
		pe.chast_reg_reg as '„астотно-регулируемый привод, наличие',		
		vld.naimenovanie as '¬ладелец приемника Ё—', 
		pe.primechanie as 'ѕримечание'
FROM priemnik_elektrosnabzheniya pe
LEFT JOIN tipy_priemnikov_elektricheskih_setey tpe on tpe.id = pe.typID
LEFT JOIN vladeltsy_es vld on vld.id = pe.vladeltsy_es_ID
