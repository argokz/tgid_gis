UPDATE pss
  set
    Elektrich = f.Elektrich,
    TransportElekricht = f.TransportElekricht,
    floodIntensityBWid = f.floodIntensityBWid,
    floodIntensityFWid = f.floodIntensityFWid,
    floodIntensityGWid = f.floodIntensityGWid,
    floodIntensityTWid = f.floodIntensityTWid,
    Vodootved = f.Vodootved,
    SlozhOkluzh = f.SlozhOkluzh,
    AvariiVipoln = f.AvariiVipoln,
    Pesehod = f.Pesehod,
    ZhdDorogi = f.ZhdDorogi,
    LudiProklad = f.LudiProklad,
    OtkluzhGkal = f.OtkluzhGkal,
    UzherbLudi = f.UzherbLudi,
    UzherbSity = f.UzherbSity,
    harakter_gruntaID = f.harakter_gruntaID,
    poverhnost_nad_trassojID = f.poverhnost_nad_trassojID,
    podtoplenie_do_truby = f.podtoplenie_do_truby,
    zanos_kanala_gruntomID = f.zanos_kanala_gruntomID,
    stroitelnye_konstrukcii_kanalaID = f.stroitelnye_konstrukcii_kanalaID,
    sostoyanie_stroitelnyh_konstrukcij_kanalaID = f.sostoyanie_stroitelnyh_konstrukcij_kanalaID,
    konstrukciya_drenazhnogo_ustrojstvaID = f.konstrukciya_drenazhnogo_ustrojstvaID,
    nalichie_korrozii_podachaID = f.nalichie_korrozii_podachaID,
    nalichie_korrozii_obratkaID = f.nalichie_korrozii_obratkaID,
    vnutrennee_sostoyanie_kanalaID = f.vnutrennee_sostoyanie_kanalaID,
    sostoyanie_teplovoj_izolyacii_podachaID = f.sostoyanie_teplovoj_izolyacii_podachaID,
    sostoyanie_teplovoj_izolyacii_obratkaID = f.sostoyanie_teplovoj_izolyacii_obratkaID,
    sostoyanie_naruzhnogo_pokrytiya_podachaID = f.sostoyanie_naruzhnogo_pokrytiya_podachaID,
    sostoyanie_naruzhnogo_pokrytiya_obratkaID = f.sostoyanie_naruzhnogo_pokrytiya_obratkaID,
    sostoyanie_protivokorrozionnogo_pokrytiya_podachaID = f.sostoyanie_protivokorrozionnogo_pokrytiya_podachaID,
    sostoyanie_protivokorrozionnogo_pokrytiya_obratkaID = f.sostoyanie_protivokorrozionnogo_pokrytiya_obratkaID,
    document_analiz_vlazhnost = f.document_analiz_vlazhnost,
    document_analiz_korrozia = f.document_analiz_korrozia,
    document_potenzial = f.document_potenzial,
    document_analiz_vytyazhka = f.document_analiz_vytyazhka

from pipeSections pss
join faktory_riska_truboprovoda f ON f.lineID = pss.id AND f.objID = $faktorID$ AND f.obj_type_faktory_riskaID = 1

-- f.objID = $faktorID$ - это так надо, это не ошибка

