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
-- Известный остаток — 41 узел. Это дубли по имени внутри фрагмента
-- («11» и «44» по 12 раз, МТК13-8 семь, пары во фрагменте 5). Для
-- дублей потребителей конвертер запись делает, для простых узлов нет.
-- Пока причина не устранена, тест сторожит, чтобы их не стало больше.
--
--   psql -d tgid_gis -v ON_ERROR_STOP=1 -f sql/tests/test_no_silent_node_loss.sql

DO $$
DECLARE
    baseline CONSTANT int := 41;   -- известный остаток, docs/14
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

    IF lost < baseline THEN
        RAISE NOTICE 'потеряно без записи: % — меньше известных %; '
                     'уменьшите baseline в тесте', lost, baseline;
        RETURN;
    END IF;

    RAISE WARNING 'узлов потеряно без записи: % — известный остаток, '
                  'причина не устранена (docs/14)', lost;
END $$;
