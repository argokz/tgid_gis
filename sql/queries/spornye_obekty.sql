-- Спорные объекты: обе версии сохранены, решение не принято.
--
-- Конвертер выбрал одну строку по числу заполненных полей, но это
-- ПРЕДПОЛОЖЕНИЕ. Вторая версия лежит рядом целиком, объект помечен
-- needs_review, и выбор можно переиграть без пересборки схемы.

-- ---------------------------------------------------------------------
-- 1. Сколько спорных объектов и по каким таблицам
-- ---------------------------------------------------------------------
SELECT src_table, klass, count(*) AS obektov
FROM net.v_needs_review
GROUP BY src_table, klass
ORDER BY 3 DESC;


-- ---------------------------------------------------------------------
-- 2. Сколько РАЗНЫХ случаев по сути: одни и те же потребители
--    размножены по копиям фрагментов
-- ---------------------------------------------------------------------
-- Внимание: показывать только calchldep НЕЛЬЗЯ — нагрузка может лежать
-- в calchlindep (независимое присоединение) или в вентиляции, и строка
-- с calchldep = 0 при этом вовсе не пустая. Поэтому рядом выводится
-- load_score — сколько полей нагрузок заполнено в каждой версии.
SELECT n.externalnodename                    AS uzel_nazvanie,
       count(*)                              AS kopiy,
       string_agg(DISTINCT
           coalesce(v.versii -> 0 -> 'payload' ->> 'calchldep', '-') || '/' ||
           coalesce(v.versii -> 0 -> 'payload' ->> 'calchlindep', '-') ||
           '  против  ' ||
           coalesce(v.versii -> 1 -> 'payload' ->> 'calchldep', '-') || '/' ||
           coalesce(v.versii -> 1 -> 'payload' ->> 'calchlindep', '-'), ' ; ')
                                             AS zavis_nezavis__vybr_protiv_otv,
       min(net.load_score(v.versii -> 0 -> 'payload')) AS nagruzok_v_vybrannoy,
       max(net.load_score(v.versii -> 1 -> 'payload')) AS nagruzok_v_otvergnutoy
FROM net.v_needs_review v
JOIN public.nodes_legacy n ON n.id = v.obj_id
GROUP BY n.externalnodename
ORDER BY 2 DESC;


-- ---------------------------------------------------------------------
-- 3. Подробно по каждому спорному объекту
-- ---------------------------------------------------------------------
SELECT v.obj_id                              AS uzel,
       n.fileid                              AS fragment,
       n.externalnodename                    AS nazvanie,
       v.ball_vybrannogo,
       v.ball_otvergnutogo,
       (v.versii -> 0 ->> 'src_id')::bigint  AS id_vybrannoy,
       (v.versii -> 0 -> 'payload' ->> 'calchldep')   AS vybr_otopl_zavis,
       (v.versii -> 0 -> 'payload' ->> 'calchlindep') AS vybr_otopl_nezavis,
       (v.versii -> 0 -> 'payload' ->> 'maxbuildingheight') AS vybr_vysota,
       (v.versii -> 1 ->> 'src_id')::bigint  AS id_otvergnutoy,
       (v.versii -> 1 -> 'payload' ->> 'calchldep')   AS otv_otopl_zavis,
       (v.versii -> 1 -> 'payload' ->> 'calchlindep') AS otv_otopl_nezavis,
       (v.versii -> 1 -> 'payload' ->> 'maxbuildingheight') AS otv_vysota
FROM net.v_needs_review v
JOIN public.nodes_legacy n ON n.id = v.obj_id
ORDER BY n.externalnodename, n.fileid;


-- ---------------------------------------------------------------------
-- 4. ПРИНЯТЬ РЕШЕНИЕ по объекту: сделать выбранной другую версию.
--    Поля объекта перезаписываются из сохранённой версии,
--    пометка needs_review снимается. Геометрия и id не меняются.
-- ---------------------------------------------------------------------
-- SELECT net.apply_variant(15373, 'generalizedconsumers');
--
-- Применить ко ВСЕМ копиям одного потребителя разом:
--
-- SELECT net.apply_variant((v.versii -> 1 ->> 'src_id')::bigint,
--                          v.src_table)
-- FROM net.v_needs_review v
-- JOIN public.nodes_legacy n ON n.id = v.obj_id
-- WHERE n.externalnodename = '6-5';
