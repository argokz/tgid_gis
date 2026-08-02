IF OBJECT_ID('tempdb..##defect_doc') IS NOT NULL DROP TABLE ##defect_doc;

SELECT 
    *
INTO ##defect_doc
FROM (
    SELECT DISTINCT
        l.id AS lineID,
        d.id AS obj_id,
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
        l.shape.STDistance(d.shape) AS length,
        MIN(l.shape.STDistance(d.shape)) OVER (PARTITION BY d.id) AS min_len

    FROM defect d
    left JOIN linesobj l ON l.removed = 0 AND l.shape.STDistance(d.shape) < 0.1
    left JOIN nodes n1 ON n1.id = l.nodeID1 AND n1.removed = 0 $and_n1_fileID$
    WHERE d.vid_narusheniyaID IS NOT NULL OR d.vid_rabotID IS NOT NULL
    AND (
     (d.data_osmotra $season_condition$)
    )
) k
WHERE k.min_len = k.length

