SELECT
    res.*
from (
        SELECT
            t.ist_name as 'Источник тепла ',
            t.id,
            t.data_nachala_plan,
            t.data_okonchaniya_plan,
            t.date_opres,
            t.name as 'Наименование/Адрес контура',
            t.fio_rukovoditel_ispytanij as 'ФИО',
            t.dolzhnost_rukovoditel_ispytanij as 'Должность',
            t.podrazdelenie_rukovoditel_ispytanij as 'Подразделение',
            t.opres_types as 'Вид испытания',
            t.obj_len as 'Протяженность контура, м',
            t.uch_id,
            'МС' as 'Признак сети',
            t.[Наименование участка],
            t.uzel_nach as 'Начальная камера',
            t.uzel_konch as 'Конечная камера',
            t.diameterCondit as 'Диаметр условный, мм',
            sum(t.length) as 'Протяженность, м'
        from (
    select
                obj.id,
                ms.id as 'uch_id',
                obj.name,
                ist.naimenovanie as 'ist_name',
                ms.uzel1 as 'uzel_nach',
                ms.uzel2 as 'uzel_konch',
                ot.name as 'opres_types',
                obj.data_nachala_plan,
                obj.data_okonchaniya_plan,
                obj.date_opres,
                obj.fio_rukovoditel_ispytanij,
                dolzhnosti.znachenie as 'dolzhnost_rukovoditel_ispytanij',
                subd.name as 'podrazdelenie_rukovoditel_ispytanij',
                ms.opisanie_uchastka_ms as 'Наименование участка',
                --re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla as 'Район эксплуатации',
                --ue_ms.nomer_uchastka as 'Участок эксплуатации',
                --nu_ms.fio as 'Начальник участка',
                hpss.pipeSectLength as length,
                sum(hpss.pipeSectLength) over (partition by obj.id) as obj_len,
                j.diameterCondit
            from opres obj
                join opresDeployed d on d.directionID = obj.id
                JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
                join uchastok_ms ms ON ms.id = hpss.magistralSite
                left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka
                left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
                left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka
                LEFT JOIN dolzhnosti ON dolzhnosti.id=obj.dolzhnost_rukovoditel_ispytanijID
                LEFT JOIN subdivisions subd ON subd.id=obj.podrazdelenie_rukovoditel_ispytanijID
                LEFT JOIN istochniki_tepla ist ON ist.id=obj.istochnik_tepla
                left join opres_types ot on ot.id = obj.opres_typeID
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

    )t
        group by 
    t.ist_name,
    t.id,
    t.data_nachala_plan,
    t.data_okonchaniya_plan,
    t.date_opres,
    t.name,
    t.fio_rukovoditel_ispytanij,
    t.dolzhnost_rukovoditel_ispytanij,
    t.podrazdelenie_rukovoditel_ispytanij,
    t.opres_types,
    t.obj_len,
    t.uch_id,
    t.[Наименование участка],
    t.uzel_nach,
    t.uzel_konch,
    t.diameterCondit

    union


        SELECT
            t.ist_name as 'Источник тепла ',
            t.id,
            t.data_nachala_plan,
            t.data_okonchaniya_plan,
            t.date_opres,
            t.name as 'Наименование/Адрес контура',
            t.fio_rukovoditel_ispytanij as 'ФИО',
            t.dolzhnost_rukovoditel_ispytanij as 'Должность',
            t.podrazdelenie_rukovoditel_ispytanij as 'Подразделение',
            t.opres_types as 'Вид испытания',
            t.obj_len as 'Протяженность контура, м',
            t.uch_id,
            'РС' as 'Признак сети',
            t.[Наименование участка],
            t.uzel_nach as 'Начальная камера',
            t.uzel_konch as 'Конечная камера',
            t.diameterCondit as 'Диаметр условный, мм',
            sum(t.length) as 'Протяженность, м'
        from (
    select
                obj.id,
                rs.id as 'uch_id',
                obj.name,
                ist.naimenovanie as 'ist_name',
                rs.uzel1 as 'uzel_nach',
                rs.uzel2 as 'uzel_konch',
                ot.name as 'opres_types',
                obj.data_nachala_plan,
                obj.data_okonchaniya_plan,
                obj.date_opres,
                obj.fio_rukovoditel_ispytanij,
                dolzhnosti.znachenie as 'dolzhnost_rukovoditel_ispytanij',
                subd.name as 'podrazdelenie_rukovoditel_ispytanij',
                rs.naimenovanie_uchastka_rs as 'Наименование участка',
                --re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla as 'Район эксплуатации',
                --ue_rs.nomer_uchastka as 'Участок эксплуатации',
                --nu_rs.fio as 'Начальник участка',
                hpss.pipeSectLength as length,
                sum(hpss.pipeSectLength) over (partition by obj.id) as obj_len,
                j.diameterCondit
            from opres obj
                join opresDeployed d on d.directionID = obj.id
                JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
                join uchastok_rs rs ON rs.id = hpss.distSite
                left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka
                left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
                left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka
                LEFT JOIN dolzhnosti ON dolzhnosti.id=obj.dolzhnost_rukovoditel_ispytanijID
                LEFT JOIN subdivisions subd ON subd.id=obj.podrazdelenie_rukovoditel_ispytanijID
                LEFT JOIN istochniki_tepla ist ON ist.id=obj.istochnik_tepla
                left join opres_types ot on ot.id = obj.opres_typeID
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
    )t
        group by 
    t.ist_name,
    t.id,
    t.data_nachala_plan,
    t.data_okonchaniya_plan,
    t.date_opres,
    t.name,
    t.fio_rukovoditel_ispytanij,
    t.dolzhnost_rukovoditel_ispytanij,
    t.podrazdelenie_rukovoditel_ispytanij,
    t.opres_types,
    t.obj_len,
    t.uch_id,
    t.[Наименование участка],
    t.uzel_nach,
    t.uzel_konch,
    t.diameterCondit
) res
WHERE   (res.date_opres  $season_condition$ )



order by res.[Источник тепла ] DESC, res.id, res.[Признак сети]
