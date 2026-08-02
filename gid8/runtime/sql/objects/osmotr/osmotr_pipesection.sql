-- Добавить поля
-- •    Состояние тепловой изоляции (подача)    
-- •    Состояние тепловой изоляции (обратка)   
-- •    Состояние наружного покрытия (подача)   
-- •    Состояние наружного покрытия (обратка)  
-- •    Состоянние противокоррозионного покрытия (подача)   
-- •    Состоянние противокоррозионного покрытия (обратка)  
-- Вставить обнаруженные нарушения -/нарушение
SELECT distinct * from (
select


t.[id],
t.[Статус],
t.[Дата осмотра],
t.[Наименование начального узла],
t.[Код расчетной схемы начального узла],
t.[Наименование конечного узла],
t.[Код расчетной схемы конечного узла],
t.[Признак участка трубопровода],
t.[Внешний вид],
t.[Состояние оборудования],
t.[Состояние строительных конструкций],
t.[Состояние металла трубопровода],
t.[Состояние тепловой изоляции (подающий трубопровод)],
t.[Состояние тепловой изоляции (обратный трубопровод)],
t.[Состояние наружного покрытия (подающий трубопровод)],
t.[Состояние наружного покрытия (обратный трубопровод)],
t.[Состояние противокоррозионного покрытия (подающий трубопровод)],
t.[Состояние противокоррозионного покрытия (обратный трубопровод)],
t.[Защемления трубопроводов],
t.[Состояние подвесок], 
t.[Состояние опор],
t.[Наличие обратных клапанов],
t.[Соответствие надписей на арматуре],
t.[Наличие табличек с регистрационными номерами],
t.[Наличие таблички с разрешенными параметрами работы],
-- t.[Первоначальная толщина металла труб],
-- t.[Фактическая толщина стенки трубопровода],
-- t.[Глубина коррозионных повреждений, мм],
-- t.[Интенсивности процесса внутренней коррозии],
-- t.[Размеры зон коррозионных повреждений, мм],
-- t.[Предполагаемые причины разрушения изоляции, коррозии],
-- t.[Результаты осмотра],
-- t.[Намеченные мероприятия],
-- t.[Мероприятия по восстановлению прокладки],
-- t.[Примечание],
t.[Дата первичного ввода в эксплуатацию],
t.[Дата последней перекладки],
t.[Магистраль],
t.[Район эксплуатации],
t.[Участок эксплуатации],
t.[Источник теплоснабжения],
t.[Наименование участка],
t.[Признак сети],
t.[Владелец участка теплопровода],
t.[Начальник участка],
t.[Тип прокладки],
t.[Длина участка теплопровода, м],
t.[Диаметр условный]
from(
SELECT 
    DISTINCT
            pss.id as 'id',
            IIF(faktory_riska_truboprovoda.id IS NULL,'не осмотрено', 'осмотрено') as 'Статус',
            obj.data_osmotra as 'Дата осмотра',
            obj.name as 'Наименование осмотра',
            vneshny_vid.name as 'Внешний вид',
            sost_oborud.name as 'Состояние оборудования',
            sost_konstr.name as 'Состояние строительных конструкций',
            sostoyanie_metalla_truboprovoda.name as 'Состояние металла трубопровода',
            zashem.name as 'Защемления трубопроводов',
            sost_podv.name as 'Состояние подвесок', 
            sost_opor.name as 'Состояние опор',
            nalich_obrKl.name as 'Наличие обратных клапанов',
            sootv_nadp.name as 'Соответствие надписей на арматуре',
            nalich_tabl_reg.name as 'Наличие табличек с регистрационными номерами',
            nalich_tabl_par.name as 'Наличие таблички с разрешенными параметрами работы',
            faktory_riska_truboprovoda.tol1 as 'Первоначальная толщина металла труб',
            faktory_riska_truboprovoda.tol2 as 'Фактическая толщина стенки трубопровода',
            faktory_riska_truboprovoda.glubina_kor as 'Глубина коррозионных повреждений, мм',
            int_kor.name as 'Интенсивности процесса внутренней коррозии',
            faktory_riska_truboprovoda.razmery_kor as 'Размеры зон коррозионных повреждений, мм',
            sostoyanie_teplovoj_izolyacii_obratka.name as 'Состояние тепловой изоляции (обратный трубопровод)',
            sostoyanie_teplovoj_izolyacii_podacha.name as 'Состояние тепловой изоляции (подающий трубопровод)',
            sostoyanie_naruzhnogo_pokrytiya_obratka.name as 'Состояние наружного покрытия (обратный трубопровод)',
            sostoyanie_naruzhnogo_pokrytiya_podacha.name as 'Состояние наружного покрытия (подающий трубопровод)',
            sostoyanie_protivokorrozionnogo_pokrytiya_obratka.name as 'Состояние противокоррозионного покрытия (обратный трубопровод)',
            sostoyanie_protivokorrozionnogo_pokrytiya_podacha.name as 'Состояние противокоррозионного покрытия (подающий трубопровод)',
            obj.predpolagaemye_prichiny_razrusheniya_izolyacii_korrozii as 'Предполагаемые причины разрушения изоляции, коррозии',
            obj.rezultaty_osmotra as 'Результаты осмотра',
            obj.namechennye_meropriyatiya as 'Намеченные мероприятия',
            obj.meropriyatiya_po_vosstanovleniyu_prokladki as 'Мероприятия по восстановлению прокладки',
            obj.primechanie as 'Примечание',
            IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',CONCAT(nt1.name, ' ', n1.externalNodeName), n1.nodeName) as 'Наименование начального узла',
            ec1.name as 'Код расчетной схемы начального узла',
            IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',CONCAT(nt2.name, ' ', n2.externalNodeName), n2.nodeName) as 'Наименование конечного узла',
            ec2.name as 'Код расчетной схемы конечного узла',
            es.name as 'Признак участка трубопровода',
            pss.firstPICdateHP as 'Дата первичного ввода в эксплуатацию',
            pss.lastTransDate as 'Дата последней перекладки',
            magistrali.naimenovanie_magistrali as 'Магистраль',
            IIF (pss.magistralSite is not NULL, re_ms.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re_rs.naimenovanie_rayona_ekspluatatsii_istochnika_tepla) as 'Район эксплуатации',
            IIF (pss.magistralSite is not NULL, ue_ms.nomer_uchastka, ue_rs.nomer_uchastka) as 'Участок эксплуатации',
            IIF (pss.magistralSite is not NULL, ist_ms.naimenovanie, ist_rs.naimenovanie) as 'Источник теплоснабжения',
            IIF (ms.opisanie_uchastka_ms is not NULL, ms.opisanie_uchastka_ms, rs.naimenovanie_uchastka_rs) as 'Наименование участка',
            IIF (pss.magistralSite is not NULL, 'магистраль', 'распредсеть') as 'Признак сети',
            organizations.name as 'Владелец участка теплопровода',
            IIF (nu_ms.fio is not NULL, nu_ms.fio, nu_rs.fio) as 'Начальник участка',
            tubingTypes.name as 'Тип прокладки',
            pss.pipeLength AS 'Длина участка теплопровода, м',
            pss.DiamUslov as 'Диаметр условный',
            srt.orderID,
            obj.id as 'objID'
        --FROM $opres$ obj
        --JOIN $opres$Deployed d ON d.directionID=obj.id
        from osmotr obj
        join osmotrDeployed d on d.directionID = obj.id
        JOIN heatPipeSections hpss ON hpss.lineID=d.lineID
        JOIN pipeSections pss ON pss.id=hpss.pipeSectionID
        LEFT JOIN sortLinesForUchastok srt ON pss.id = srt.pipeSectionID
--      LEFT join linesobj l on l.id = srt.lineID
        LEFT join linesobj l on l.id = d.lineID

        left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = pss.id and faktory_riska_truboprovoda.objID = obj.id and faktory_riska_truboprovoda.obj_type_faktory_riskaID = 2
        LEFT JOIN tubingTypes tt ON pss.tubingTypeID = tt.id
        left join sostoyanie_teplovoj_izolyacii sostoyanie_teplovoj_izolyacii_obratka on sostoyanie_teplovoj_izolyacii_obratka.id = faktory_riska_truboprovoda.sostoyanie_teplovoj_izolyacii_obratkaID
        left join sostoyanie_teplovoj_izolyacii sostoyanie_teplovoj_izolyacii_podacha on sostoyanie_teplovoj_izolyacii_podacha.id = faktory_riska_truboprovoda.sostoyanie_teplovoj_izolyacii_podachaID
        left join sostoyanie_naruzhnogo_pokrytiya sostoyanie_naruzhnogo_pokrytiya_obratka on sostoyanie_naruzhnogo_pokrytiya_obratka.id = faktory_riska_truboprovoda.sostoyanie_naruzhnogo_pokrytiya_obratkaID
        left join sostoyanie_naruzhnogo_pokrytiya sostoyanie_naruzhnogo_pokrytiya_podacha on sostoyanie_naruzhnogo_pokrytiya_podacha.id = faktory_riska_truboprovoda.sostoyanie_naruzhnogo_pokrytiya_podachaID
        left join sostoyanie_protivokorrozionnogo_pokrytiya_shurf sostoyanie_protivokorrozionnogo_pokrytiya_podacha on sostoyanie_protivokorrozionnogo_pokrytiya_podacha.id = faktory_riska_truboprovoda.sostoyanie_protivokorrozionnogo_pokrytiya_podachaID
        left join sostoyanie_protivokorrozionnogo_pokrytiya_shurf sostoyanie_protivokorrozionnogo_pokrytiya_obratka on sostoyanie_protivokorrozionnogo_pokrytiya_obratka.id = faktory_riska_truboprovoda.sostoyanie_protivokorrozionnogo_pokrytiya_obratkaID
        JOIN nodes n1 ON n1.id=pss.nodeID1
        JOIN nodes n2 ON n2.id=pss.nodeID2
        LEFT JOIN nodeTypes nt1 ON nt1.id=n1.nodeTypeID
        LEFT JOIN nodeTypes nt2 ON nt2.id=n2.nodeTypeID
        left join externalCodes ec1 ON ec1.id = n1.externalCodeID
        left join externalCodes ec2 ON ec2.id = n2.externalCodeID
        left join externalSigns  es on es.id = l.externalSignLineID

        LEFT JOIN tubingTypes ON tubingTypes.id=pss.tubingTypeID
        left join vneshny_vid on vneshny_vid.id=faktory_riska_truboprovoda.VnesniiVid
        left join sost_oborud on sost_oborud.id=faktory_riska_truboprovoda.SostOborudovania
        left join sost_konstr on sost_konstr.id=faktory_riska_truboprovoda.SostKonstrukz
        left join sostoyanie_metalla_truboprovoda on sostoyanie_metalla_truboprovoda.id = faktory_riska_truboprovoda.sostoyanie_metalla_truboprovodaID
        left join pr_ots_lookup zashem on zashem.id = faktory_riska_truboprovoda.zashemID
        left join sost_lookup sost_podv on sost_podv.id = faktory_riska_truboprovoda.sost_podvID
        left join sost_lookup sost_opor on sost_opor.id = faktory_riska_truboprovoda.sost_oporID
        left join pr_ots_lookup nalich_obrKl on nalich_obrKl.id = faktory_riska_truboprovoda.nalich_obrKlID
        left join sootv_lookup sootv_nadp on sootv_nadp.id = faktory_riska_truboprovoda.sootv_nadpID
        left join pr_ots_lookup nalich_tabl_reg on nalich_tabl_reg.id = faktory_riska_truboprovoda.nalich_tabl_regID
        left join pr_ots_lookup nalich_tabl_par on nalich_tabl_par.id = faktory_riska_truboprovoda.nalich_tabl_parID
        left join int_kor on int_kor.id = faktory_riska_truboprovoda.int_korID

        left join organizations on organizations.id = pss.organizationID
        left join magistrali on magistrali.id = pss.magistral

        left join uchastok_ms ms ON ms.id = pss.magistralSite
        left join uchastki_ekspluatatsii ue_ms ON ue_ms.id = ms.nomer_uchastka

        left join rayon_ekspluatatsii re_ms ON re_ms.id = ue_ms.rayon_ekspluatatsii
        left join nachalniki_uchastkov nu_ms ON nu_ms.id = ue_ms.nachalnik_uchastka

        left join uchastok_rs rs ON rs.id = pss.distSite
        left join uchastki_ekspluatatsii ue_rs ON ue_rs.id = rs.nomer_uchastka

        left join rayon_ekspluatatsii re_rs ON re_rs.id = ue_rs.rayon_ekspluatatsii
        left join nachalniki_uchastkov nu_rs ON nu_rs.id = ue_rs.nachalnik_uchastka

        LEFT JOIN istochniki_tepla ist_ms ON ist_ms.id=ue_ms.istochnik_tepla
        LEFT JOIN istochniki_tepla ist_rs ON ist_rs.id=ue_rs.istochnik_tepla
        where obj.id = $id$
) t

) __

order by 
[Дата осмотра] desc
--[Наименование осмотра]
