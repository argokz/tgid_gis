UPDATE i
SET 
i.tekuschiy_nomer = NULL,
i.nomer_indikatora_korrozii = NULL,
i.truboprovod = NULL,
i.kolichestvo_plastin_v_sborke = NULL,
i.sredniy_ves_plastiny_pri_ustanovke__g = NULL,
i.radius_krugloy_plastiny__mm = NULL,
i.radius_vtulki__mm = NULL,
i.tolschina_plastiny__mm = NULL,
i.data_ustanovki = NULL,
i.data_izvlecheniya = NULL,
i.kolichestvo_dney_ispytaniy = NULL,
i.sredniy_ves_plastiny_posle_ispytaniy__g = NULL,
i.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g = NULL,
i.srednyaya_skorost_korrozii__mm_god = NULL,
i.otsenka_korrozionnogo_protsessa = NULL,
i.agressivnost_setevoy_vody = NULL,
i.vneshniy_vid_plastin = NULL,
i.data_planirovaniya = NULL,
i.primechanie = NULL,
i.stateIndID = NULL

FROM indikator_korrozii i
JOIN indikator_korrozii_po_godam i2 ON i2.id_i=i.id 
WHERE YEAR(i2.data_planirovaniya)=2017 OR YEAR(i2.data_ustanovki)=2017

GO

UPDATE i
SET 
i.tekuschiy_nomer = i2.tekuschiy_nomer,
i.nomer_indikatora_korrozii = i2.nomer_indikatora_korrozii,
i.truboprovod = i2.truboprovod,
i.kolichestvo_plastin_v_sborke = i2.kolichestvo_plastin_v_sborke,
i.sredniy_ves_plastiny_pri_ustanovke__g = i2.sredniy_ves_plastiny_pri_ustanovke__g,
i.radius_krugloy_plastiny__mm = i2.radius_krugloy_plastiny__mm,
i.radius_vtulki__mm = i2.radius_vtulki__mm,
i.tolschina_plastiny__mm = i2.tolschina_plastiny__mm,
i.data_ustanovki = i2.data_ustanovki,
i.data_izvlecheniya = i2.data_izvlecheniya,
i.kolichestvo_dney_ispytaniy = i2.kolichestvo_dney_ispytaniy,
i.sredniy_ves_plastiny_posle_ispytaniy__g = i2.sredniy_ves_plastiny_posle_ispytaniy__g,
i.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g = i2.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g,
i.srednyaya_skorost_korrozii__mm_god = i2.srednyaya_skorost_korrozii__mm_god,
i.otsenka_korrozionnogo_protsessa = i2.otsenka_korrozionnogo_protsessa,
i.agressivnost_setevoy_vody = i2.agressivnost_setevoy_vody,
i.vneshniy_vid_plastin = i2.vneshniy_vid_plastin,
i.data_planirovaniya = i2.data_planirovaniya,
i.primechanie = i2.primechanie,
i.stateIndID = i2.stateIndID

FROM indikator_korrozii i
JOIN indikator_korrozii_po_godam i2 ON i2.id_i=i.id 
WHERE YEAR(i2.data_planirovaniya)=2017 OR YEAR(i2.data_ustanovki)=2017

GO