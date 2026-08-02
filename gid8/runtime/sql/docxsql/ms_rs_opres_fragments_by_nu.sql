select
    distinct
    t.[Начальник участка],
    t.[Район эксплуатации],
    t.[Наименование участка],
    t.[Признак сети]
from (
    select
        obj.id,
        ms.opisanie_uchastka_ms as 'Наименование участка',
        nu_ms.fio as 'Начальник участка',
        re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla as 'Район эксплуатации',
        ue_ms.nomer_uchastka as 'Участок эксплуатации',
        IIF (hpss.magistralSite is not NULL, 'магистраль', 'распредсеть') as 'Признак сети'
    from opres obj
            join opresDeployed d on d.directionID = obj.id
            JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
            join uchastok_ms ms ON ms.id = hpss.magistralSite
            left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
            left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
            left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
            join (
                select
                    distinct
                    d.id,
                    d.diameterCondit
                    from (select  
                        ms.id,
                        hpss.pipeSectLength,
                        hpss.diameterCondit,
                        max(hpss.pipeSectLength) over (partition by ms.id) as m_len
                    from opres obj
                    join opresDeployed d on d.directionID = obj.id
                    JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
                    join uchastok_ms ms ON ms.id = hpss.magistralSite
                    ) d 
                    where d.m_len = d.pipeSectLength
                ) j on j.id = ms.id
    WHERE obj.id = $fileID$

    union

    select
        obj.id,
        rs.naimenovanie_uchastka_rs as 'Наименование участка',
        nu_rs.fio as 'Начальник участка',
        re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla as 'Район эксплуатации',
        ue_rs.nomer_uchastka as 'Участок эксплуатации',
        IIF (hpss.distSite is not NULL, 'распредсеть', 'магистраль') as 'Признак сети'
    from opres obj
            join opresDeployed d on d.directionID = obj.id
            JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
            join uchastok_rs rs ON rs.id = hpss.distSite
            left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
            left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
            left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
            join (
                select
                    distinct
                    d.id,
                    d.diameterCondit
                    from (select  
                        rs.id,
                        hpss.pipeSectLength,
                        hpss.diameterCondit,
                        max(hpss.pipeSectLength) over (partition by rs.id) as m_len
                    from opres obj
                    join opresDeployed d on d.directionID = obj.id
                    JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
                    join uchastok_rs rs ON rs.id = hpss.distSite 
                    ) d 
                    where d.m_len = d.pipeSectLength
                ) j on j.id = rs.id
    WHERE obj.id = $fileID$
) t

order by t.[Начальник участка], t.[Признак сети]
