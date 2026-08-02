SELECT top 2147483647 
     l.id
      ,l.tip_istochnika
--      ,l.naimenovanie_istochnika
	  ,ist.naimenovanie_istochnika_es AS naimenovanie_istochnika
      ,l.tip_priemnika
--      ,l.naimenovanie_priemnika
	  ,pr.naimenovanie_priemnika_es AS naimenovanie_priemnika
      ,l.naimenovanie_lep
--      ,l.vladelets_lep
	  ,vl.naimenovanie AS vladelets_lep
      ,l.napryazhenie__kv
      ,l.fidera
--      ,l.tip_prokladki_lep
	 ,tp.znachenie AS tip_prokladki_lep
--      ,l.marka_kabelya_linii
	  ,mark.polnoe_naimenovanie_marki_tipa__kabelya AS marka_kabelya_linii
      ,l.protyazhennost__m
      ,l.data_vvoda_v_ekspluatatsiyu
      ,l.primechanie
FROM liniya_elektroperedach l
LEFT JOIN istochnik_elektrosnabzheniya ist ON ist.id=l.naimenovanie_istochnika
LEFT JOIN priemnik_elektrosnabzheniya pr ON pr.id=l.naimenovanie_priemnika
LEFT JOIN vladeltsy_es vl ON vl.id=l.vladelets_lep
LEFT JOIN marki_kabeley_es mark ON mark.id=l.marka_kabelya_linii
-- LEFT JOIN tipy_lep tp ON IIF(IsNumeric(l.tip_prokladki_lep)=1 AND tp.id=l.tip_prokladki_lep, 1, 0)=1
LEFT JOIN tipy_lep tp  ON tp.id=IIF(IsNumeric(l.tip_prokladki_lep)=1, CAST (l.tip_prokladki_lep AS int), 0)
