select distinct top 2147483647  
    t.obj_id as id,
    CONCAT(st.name,' ',t.nomer_doma) as 'Адрес',
    t.data_nachala_plan as 'Дата начала шурфовки плановая',
    t.data_okonchaniya_plan as 'Дата окончания шурфовки плановая',
    --m.name as 'Материалы и механизмы',
    IIF(t.utverdit = 0, 'Не утверждено', 'Утверждено') as 'Состояние утверждения',
    t.data_utverzhdeniya_plana_shurfovok as 'Дата утверждения плана шурфовок',
    t.primechanie as 'Примечание',
    t.fio_utverzhdaemogo as 'ФИО утверждающего',
    dolz.znachenie as 'Должность утверждающего',
    subd.name as 'Служба утверждающего',
    fr.name as 'Фрагмент сети'
from(
    select distinct
        l.lineID,
        l.fileID,
        organizations.name as 'organization',
        l.externalSignLineID,
        d.id as obj_id,
        d.materialy_i_mekhanizmyID,
        d.data_utverzhdeniya_plana_shurfovok,
        d.ulicaID,
        d.nomer_doma,
        d.data_nachala_plan,
        d.data_okonchaniya_plan,
        d.primechanie,
        d.fio_utverzhdaemogo,
        d.dolzhnost_utverzhdaemogoID,
        d.sluzhba_utverzhdaemogoID,
        d.fio_1,
        d.dolzhnost_1,
        d.fio_2,
        d.dolzhnost_2,
        d.fio_viziruemogo_1,
        d.dolzhnost_viziruemogoID_1,
        d.utverdit
    from shurfy d
        JOIN (
            select 
            distinct
                k.lineID,
                k.externalSignLineID,
                k.obj_id,
                k.organizationID,
                k.fileID
            from (
                select
                    distinct
                        l.id as lineID,
                        d.id as obj_id,
                        l.externalSignLineID,
                        l.organizationID,
                        ROW_NUMBER() OVER (PARTITION BY d.id ORDER  BY d.shape.STPointN(1).STDistance(l.shape)) AS rn,
                        n1.fileID
                from shurfy d
                left JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape.STPointN(1)) < 0.1 )
                left join nodes n1 on n1.id = l.nodeID1 and n1.removed = 0 
             where 
                d.naznachenie_vskrID = 1 $and_n1_fileID$
            )k
        where k.rn = 1
    ) l on l.obj_id = d.id
    left join organizations on organizations.id = l.organizationID
    --AND  (d.data_nachala_plan  BETWEEN '20150415' AND '20160415' )
)t
--  LEFT 
    JOIN heatPipeSections hpss ON hpss.lineID=t.lineID 

    LEFT JOIN pipeSections pss ON pss.id = hpss.pipeSectionID
    left JOIN nodes n1 ON ( n1.id = pss.nodeID1 and n1.removed = 0 )
    left JOIN nodes n2 ON n2.id = pss.nodeID2
    --left join externalCodes ec1 ON ec1.id = n1.externalCodeID
    --left join externalCodes ec2 ON ec2.id = n2.externalCodeID
    left join externalSigns  es on es.id = t.externalSignLineID

    left join isolMaterials im on im.id = pss.isolMaterialID
    LEFT JOIN tubingTypes ON tubingTypes.id=pss.tubingTypeID
    left join isolationTypes on isolationTypes.id = pss.isolationTypeID
    left join externalMaterials on externalMaterials.id = pss.externMaterialID
    left join anticorrMaterials on anticorrMaterials.id = pss.anticorrMaterialID
    --left join magistrali on magistrali.id = hpss.magistral

    left join uchastok_ms ms ON ms.id = pss.magistralSite
    left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka

    left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
    left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka

    left join uchastok_rs rs ON rs.id = pss.distSite
    left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka

    left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
    left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka

    --LEFT JOIN istochniki_tepla ist_ms ON ist_ms.id=ue_ms.istochnik_tepla
    --LEFT JOIN istochniki_tepla ist_rs ON ist_rs.id=ue_rs.istochnik_tepla

    left join ulitsy st ON st.id = t.ulicaID
    
    --left join materialy_i_mekhanizmy m ON m.id = t.materialy_i_mekhanizmyID

    LEFT JOIN dolzhnosti dolz ON dolz.id=t.dolzhnost_utverzhdaemogoID
    --LEFT JOIN dolzhnosti dolz_1 ON dolz_1.id=t.dolzhnost_viziruemogoID_1
    --LEFT JOIN dolzhnosti dolz_f1 ON dolz_f1.id=t.dolzhnost_1
    --LEFT JOIN dolzhnosti dolz_f2 ON dolz_f2.id=t.dolzhnost_2
    LEFT JOIN subdivisions subd ON subd.id=t.sluzhba_utverzhdaemogoID

    left join fragments fr ON fr.id = t.fileID
    where 1=1 $and_n1_fileID$
order by t.data_nachala_plan desc