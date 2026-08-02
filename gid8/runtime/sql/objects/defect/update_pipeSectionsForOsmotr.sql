UPDATE pss
  set
    VnesniiVid = f.VnesniiVid,
    SostOborudovania = f.SostOborudovania,
    sostoyanie_metalla_truboprovodaID = f.sostoyanie_metalla_truboprovodaID,
    SostKonstrukz = f.SostKonstrukz,
    sostoyanie_teplovoj_izolyacii_podachaID = f.sostoyanie_teplovoj_izolyacii_podachaID,
    sostoyanie_teplovoj_izolyacii_obratkaID = f.sostoyanie_teplovoj_izolyacii_obratkaID,
    sostoyanie_naruzhnogo_pokrytiya_podachaID = f.sostoyanie_naruzhnogo_pokrytiya_podachaID,
    sostoyanie_naruzhnogo_pokrytiya_obratkaID = f.sostoyanie_naruzhnogo_pokrytiya_obratkaID,
    sostoyanie_protivokorrozionnogo_pokrytiya_podachaID = f.sostoyanie_protivokorrozionnogo_pokrytiya_podachaID,
    sostoyanie_protivokorrozionnogo_pokrytiya_obratkaID = f.sostoyanie_protivokorrozionnogo_pokrytiya_obratkaID,
    zashemID = f.zashemID,
    sost_podvID = f.sost_podvID,
    sost_oporID = f.sost_oporID,
    nalich_obrKlID = f.nalich_obrKlID,
    sootv_nadpID = f.sootv_nadpID,
    nalich_tabl_regID = f.nalich_tabl_regID,
    nalich_tabl_parID = f.nalich_tabl_parID

from pipeSections pss
join faktory_riska_truboprovoda f ON f.lineID = pss.id AND f.id = $faktorID$ AND f.obj_type_faktory_riskaID = 2

