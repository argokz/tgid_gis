-- Дубли в generalizedconsumers: две строки на один узел.
--
-- Работает и на исходной БД (almatygid), и на новой (tgid_gis) —
-- сама таблица generalizedconsumers в обеих осталась без изменений.
--
-- Конвертер выбирает строку с бОльшим числом заполненных полей
-- (net.data_score). Спорны только пары, где данные есть в ОБЕИХ строках:
-- их надо посмотреть глазами.

-- ---------------------------------------------------------------------
-- 1. СВОДКА: сколько пар решается автоматически, сколько требует разбора
-- ---------------------------------------------------------------------
WITH dup AS (
    SELECT nodeid
    FROM generalizedconsumers
    WHERE nodeid IS NOT NULL
    GROUP BY nodeid
    HAVING count(*) = 2
),
pair AS (
    SELECT g.nodeid, min(g.id) AS id_staraya, max(g.id) AS id_novaya
    FROM generalizedconsumers g
    JOIN dup USING (nodeid)
    GROUP BY g.nodeid
),
v AS (
    SELECT p.nodeid,
           (coalesce(a.calchldep,0) <> 0 OR coalesce(a.calchlindep,0) <> 0
            OR coalesce(a.calchlventil,0) <> 0) AS est_v_staroy,
           (coalesce(b.calchldep,0) <> 0 OR coalesce(b.calchlindep,0) <> 0
            OR coalesce(b.calchlventil,0) <> 0) AS est_v_novoy
    FROM pair p
    JOIN generalizedconsumers a ON a.id = p.id_staraya
    JOIN generalizedconsumers b ON b.id = p.id_novaya
)
SELECT CASE
         WHEN est_v_staroy AND NOT est_v_novoy THEN 'данные только в старой строке'
         WHEN est_v_novoy AND NOT est_v_staroy THEN 'данные только в новой строке'
         WHEN est_v_staroy AND est_v_novoy     THEN 'ДАННЫЕ В ОБЕИХ - нужен разбор'
         ELSE 'обе строки пустые'
       END AS sluchay,
       count(*) AS par
FROM v
GROUP BY 1
ORDER BY 2 DESC;


-- ---------------------------------------------------------------------
-- 2. РАЗБОР: пары, где нагрузки заданы в обеих строках
-- ---------------------------------------------------------------------
WITH dup AS (
    SELECT nodeid
    FROM generalizedconsumers
    WHERE nodeid IS NOT NULL
    GROUP BY nodeid
    HAVING count(*) = 2
),
-- Оценку заполненности считаем здесь: в запросе с несколькими JOIN
-- ссылка вида to_jsonb(alias) разбирается неоднозначно.
ball AS (
    SELECT g.id, net.data_score(to_jsonb(g)) AS zapolneno
    FROM generalizedconsumers g
    WHERE g.nodeid IN (SELECT nodeid FROM dup)
),
pair AS (
    SELECT g.nodeid, min(g.id) AS id_staraya, max(g.id) AS id_novaya
    FROM generalizedconsumers g
    JOIN dup USING (nodeid)
    GROUP BY g.nodeid
)
SELECT
    n.fileid                                   AS fragment,
    f.name                                     AS fragment_nazvanie,
    p.nodeid                                   AS uzel,
    n.externalnodename                         AS uzel_nazvanie,
    n.removed                                  AS uzel_udalen,

    p.id_staraya,
    round(a.calchldep::numeric,    4)          AS star_otopl_zavis,
    round(a.calchlindep::numeric,  4)          AS star_otopl_nezavis,
    round(a.calchlventil::numeric, 4)          AS star_ventil,
    a.maxbuildingheight                        AS star_vysota,

    p.id_novaya,
    round(b.calchldep::numeric,    4)          AS nov_otopl_zavis,
    round(b.calchlindep::numeric,  4)          AS nov_otopl_nezavis,
    round(b.calchlventil::numeric, 4)          AS nov_ventil,
    b.maxbuildingheight                        AS nov_vysota,

    -- какую строку выберет конвертер и с каким отрывом
    CASE WHEN bb.zapolneno >= ba.zapolneno
         THEN 'новую' ELSE 'старую' END        AS vyberet_konverter,
    ba.zapolneno                               AS zapolneno_v_staroy,
    bb.zapolneno                               AS zapolneno_v_novoy

FROM pair p
JOIN generalizedconsumers a ON a.id = p.id_staraya
JOIN generalizedconsumers b ON b.id = p.id_novaya
JOIN ball ba                ON ba.id = p.id_staraya
JOIN ball bb                ON bb.id = p.id_novaya
JOIN nodes n                ON n.id = p.nodeid
LEFT JOIN fragments f       ON f.id = n.fileid
WHERE (coalesce(a.calchldep,0) <> 0 OR coalesce(a.calchlindep,0) <> 0
       OR coalesce(a.calchlventil,0) <> 0)
  AND (coalesce(b.calchldep,0) <> 0 OR coalesce(b.calchlindep,0) <> 0
       OR coalesce(b.calchlventil,0) <> 0)
ORDER BY n.fileid, p.nodeid;


-- ---------------------------------------------------------------------
-- 3. СКОЛЬКО НА САМОМ ДЕЛЕ РАЗНЫХ СЛУЧАЕВ
--
-- Фрагменты 76/78/80/82/84/86 и 91/93/95/97 — копии одной сети
-- (одинаковое число узлов и совпадающие границы координат), поэтому
-- один и тот же потребитель встречается в них многократно, каждый раз
-- отдельным узлом со своим externalcodeid. Спорных строк 23, но
-- РАЗНЫХ потребителей среди них всего несколько.
-- ---------------------------------------------------------------------
WITH dup AS (
    SELECT nodeid FROM generalizedconsumers
    WHERE nodeid IS NOT NULL GROUP BY nodeid HAVING count(*) = 2
),
pair AS (
    SELECT g.nodeid, min(g.id) AS id_lo, max(g.id) AS id_hi
    FROM generalizedconsumers g JOIN dup USING (nodeid) GROUP BY g.nodeid
)
SELECT n.externalnodename                       AS uzel_nazvanie,
       count(*)                                 AS v_skolkih_fragmentah,
       count(DISTINCT round(a.calchldep::numeric, 4)::text || ' / ' ||
                      round(b.calchldep::numeric, 4)::text) AS raznyh_variantov,
       string_agg(DISTINCT round(a.calchldep::numeric, 4)::text || ' / ' ||
                           round(b.calchldep::numeric, 4)::text, ' ; ')
                                                AS varianty_nagruzki
FROM pair p
JOIN generalizedconsumers a ON a.id = p.id_lo
JOIN generalizedconsumers b ON b.id = p.id_hi
JOIN nodes n ON n.id = p.nodeid
WHERE (coalesce(a.calchldep,0) <> 0 OR coalesce(a.calchlindep,0) <> 0
       OR coalesce(a.calchlventil,0) <> 0)
  AND (coalesce(b.calchldep,0) <> 0 OR coalesce(b.calchlindep,0) <> 0
       OR coalesce(b.calchlventil,0) <> 0)
GROUP BY n.externalnodename
ORDER BY 2 DESC;


-- ---------------------------------------------------------------------
-- 4. ФРАГМЕНТЫ-КОПИИ: одинаковое число узлов и границы координат
-- ---------------------------------------------------------------------
SELECT fileid AS fragment, count(*) AS uzlov,
       count(DISTINCT externalnodename) AS raznyh_imen,
       min(x) AS x_min, max(x) AS x_max
FROM nodes WHERE removed = 0
GROUP BY fileid
ORDER BY count(*) DESC, fileid;


-- ---------------------------------------------------------------------
-- 5. ВСЕ РАЗЛИЧИЯ по конкретной паре — подставьте нужный nodeid
-- ---------------------------------------------------------------------
-- \set uzel 12345
--
-- WITH p AS (
--     SELECT min(id) AS id_staraya, max(id) AS id_novaya
--     FROM generalizedconsumers WHERE nodeid = :uzel
-- )
-- SELECT e.key AS pole,
--        to_jsonb(a) ->> e.key AS v_staroy,
--        to_jsonb(b) ->> e.key AS v_novoy
-- FROM p
-- JOIN generalizedconsumers a ON a.id = p.id_staraya
-- JOIN generalizedconsumers b ON b.id = p.id_novaya
-- CROSS JOIN LATERAL jsonb_object_keys(to_jsonb(a)) AS e(key)
-- WHERE e.key <> 'id'
--   AND (to_jsonb(a) ->> e.key) IS DISTINCT FROM (to_jsonb(b) ->> e.key)
-- ORDER BY 1;
