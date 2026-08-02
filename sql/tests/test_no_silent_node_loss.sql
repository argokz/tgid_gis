-- Ни один живой узел источника не должен исчезнуть молча.
--
-- Конвертер имеет право не переносить строку — но обязан записать
-- причину в net.conversion_reject. Узел, которого нет ни в net, ни в
-- журнале отказов, потерян незаметно: при сверке результатов он не
-- всплывёт, потому что его нет ни с одной стороны.
--
-- Так и нашлись 7 узлов МТК13-8: они всплыли только через
-- setpressnodes, где на них ссылались.
--
-- Причина была в insert_class: узлы фильтровались условием
-- (n.x <> 0 OR n.y <> 0), и точка (0, 0) — то есть отсутствие
-- координат — отбрасывала узел без записи. Исправлено шагом
-- nodes_without_coords в converter/convert.py. Порог 0: любая новая
-- тихая потеря — провал.
--
--   psql -d tgid_gis -v ON_ERROR_STOP=1 -f sql/tests/test_no_silent_node_loss.sql

DO $$
DECLARE
    baseline CONSTANT int := 0;    -- причина устранена, docs/14
    lost     int;
BEGIN
    IF to_regclass('attic.nodes_legacy') IS NULL THEN
        RAISE NOTICE 'attic.nodes_legacy нет — проверка пропущена '
                     '(продуктовая БД без исходных таблиц)';
        RETURN;
    END IF;

    WITH src AS (
        SELECT id FROM attic.nodes_legacy WHERE removed = 0
    ), moved AS (
        SELECT src_id AS id FROM net.node_src_map
        UNION
        SELECT p.src_id FROM net.node_plain p
    ), rejected AS (
        SELECT src_id AS id FROM net.conversion_reject WHERE src_id IS NOT NULL
    )
    SELECT count(*) INTO lost
    FROM src s
    WHERE NOT EXISTS (SELECT 1 FROM moved m WHERE m.id = s.id)
      AND NOT EXISTS (SELECT 1 FROM rejected r WHERE r.id = s.id);

    IF lost > baseline THEN
        RAISE EXCEPTION 'узлов потеряно без записи: % (было %) — '
                        'конвертер начал терять больше', lost, baseline;
    END IF;


    RAISE NOTICE 'узлов потеряно без записи: 0 — каждый неперенесённый '
                 'узел объяснён в net.conversion_reject';
END $$;
