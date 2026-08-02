IF OBJECT_ID('tempdb..##defect_doc_table') IS NOT NULL DROP TABLE ##defect_doc_table;

select 
    *
INTO ##defect_doc_table
from (
select distinct
l.id as lineID,
d.id as obj_id,
d.remontTypeID,
d.data_osmotra,
d.vremya_osmotra,
d.vid_rabotID,
d.vid_narusheniyaID,
d.nomer_doma,
d.ulicaID,
d.priznak_truboprovoda,
d.rasstoyanieDoPovrezhdeniyaNachKamery,
d.tipPoverhnostiID,
d.dreniruemyj_truboprovodID,
d.defectDescription,
d.opresID,
d.data_sostavleniya_akta,
d.nomer_akta,
d.nomer_prikaza,
d.data_prikaza_vvoda_v_ekspluataciyu,
d.prikaz_vvoda_v_ekspluataciyu,
d.remontCatID,
d.stateID,
d.sostTeploizolID,
d.sostKonstruktsiiKameryID,
d.sostKonstruktsiiKanalaID,
d.sostNaruzhnoiPoverkhnostiID,
d.sostVnutrenneiPoverkhnostiID,
d.nodeID1,
d.nodeID2,
d.tipPovrezhdeniaID,
d.tsentrPovrezhdenia,
d.vysotaPovrezhdenia,
d.shirinaPovrezhdenia,
d.ploshchadPovrezhdenia,
d.meropriyatiya,
d.data_shurfovki,
d.data_nachala_remonta,
d.vremiaNachalaRemonta,
d.data_zaversheniya_remonta,
d.vremiaZaversheniaRemonta,
d.trudozatratyNaRemont,
d.stoimostRemonta,
d.otchet_po_defektu,
d.kolichestvo_otklyuchennyh_potrebitelej,
d.kolichestvo_nedootpushchennoj_teplovoj_energii,
d.zatraty_na_vosstanovlenie,
d.inye_socialnye_posledstviya,
d.shirinaZaplatki,
d.vysotaZaplatki,
d.len_tube_cur,
d.remontIzolKonstruktsiiID,
d.len_izol_cur,
d.remont_kanalaID,
d.len_channel_cur,
d.remont_kameryID,
l.shape.STDistance(d.shape) as length,
MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS "min_len"
    from defect d
    LEFT JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1 )
    LEFT JOIN nodes n1 ON n1.id=l.nodeID1 AND n1.removed=0  $and_n1_fileID$
    where 
        d.data_osmotra IS NOT NULL AND (d.data_osmotra $season_condition$) AND
        d.vid_narusheniyaID is not NULL or d.vid_rabotID is not NULL
)k
WHERE k.min_len = k.length;

