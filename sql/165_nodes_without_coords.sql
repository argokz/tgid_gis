-- Задним числом записываем узлы, отброшенные из-за нулевых координат.
--
-- Конвертер фильтровал узлы условием (n.x <> 0 OR n.y <> 0): точка
-- (0, 0) — это отсутствие координат, а не место на карте. Отбрасывать
-- такие узлы правильно, но делалось это молча, без записи в
-- net.conversion_reject.
--
-- Из 86 624 живых узлов источника так исчез 41. Сверка результатов
-- такую потерю не ловит: объекта нет ни с одной стороны, расхождению
-- взяться неоткуда. Всплыли они случайно — через setpressnodes, где
-- на семь из них ссылались.
--
-- В конвертере причина устранена шагом nodes_without_coords
-- (converter/convert.py). Этот скрипт приводит уже собранные базы
-- к тому же состоянию.
--
-- Источник — attic.nodes_legacy: исходная таблица nodes, переименованная
-- при установке слоя совместимости и убранная на чердак. В продуктовой
-- БД её нет, и скрипт молча ничего не делает.

BEGIN;

DO $$
DECLARE
    n int;
BEGIN
    IF to_regclass('attic.nodes_legacy') IS NULL THEN
        RAISE NOTICE 'attic.nodes_legacy нет — нечего дописывать';
        RETURN;
    END IF;

    INSERT INTO net.conversion_reject (src_table, src_id, reason, detail)
    SELECT 'nodes', n.id, 'у узла нет координат',
           jsonb_build_object(
               'fileid', n.fileid,
               'externalnodename', n.externalnodename,
               'externalsignid', n.externalsignid)
    FROM attic.nodes_legacy n
    WHERE n.removed = 0
      AND n.x = 0 AND n.y = 0
      AND NOT EXISTS (SELECT 1 FROM net.conversion_reject r
                      WHERE r.src_table = 'nodes' AND r.src_id = n.id);

    GET DIAGNOSTICS n = ROW_COUNT;
    RAISE NOTICE 'записано узлов без координат: %', n;
END $$;

COMMIT;

SELECT reason, count(*) AS skolko
FROM net.conversion_reject
WHERE src_table = 'nodes'
GROUP BY 1 ORDER BY 2 DESC;
