-- Уставки давления должны иметь ровно один источник.
--
-- До sql/182 их было два: public.setpressnodes (5740 строк) и
-- net.node_press_setting (5733). Кто какой увидит, зависело от
-- search_path: gid8 читал net через compat, а sety, potr5 и инструменты
-- читали public, потому что compat в их пути нет. Уставка, сохранённая
-- в программе, до расчёта не доходила, и никакая ошибка при этом не
-- возникала — расчёт просто шёл на устаревших данных.
--
-- Тест сторожит именно это: не «представление работает», а
-- «второго источника не появилось снова». Соблазн вернуть таблицу в
-- public ради простого запроса будет возникать и дальше.
--
--   psql -d tgid_gis -v ON_ERROR_STOP=1 -f sql/tests/test_setpressnodes_edinstvennyy.sql

BEGIN;

DO $$
DECLARE
    n_view  bigint;
    n_net   bigint;
    n_free  bigint;
    new_id  integer;
    v       double precision;
BEGIN
    -- 1. Единственность источника.
    IF to_regclass('public.setpressnodes') IS NOT NULL THEN
        RAISE EXCEPTION
            'в public снова есть setpressnodes — движок расчёта будет '
            'читать её вместо net.node_press_setting';
    END IF;

    IF to_regclass('net.v_setpressnodes') IS NULL THEN
        RAISE EXCEPTION 'net.v_setpressnodes не существует';
    END IF;

    -- 2. compat остаётся тонкой обёрткой над тем же представлением,
    --    а не вторым определением, которое может разойтись.
    SELECT count(*) INTO n_view FROM net.v_setpressnodes;
    SELECT count(*) INTO n_net  FROM compat.setpressnodes;
    IF n_view <> n_net THEN
        RAISE EXCEPTION 'net.v_setpressnodes=% и compat.setpressnodes=% '
                        'разошлись', n_view, n_net;
    END IF;

    -- 3. Запись доходит до целевой таблицы через оба имени.
    --    node_id уникален, поэтому берём узел без уставки.
    SELECT count(*) INTO n_free
      FROM net.v_map_nodes n
      LEFT JOIN net.node_press_setting s ON s.node_id = n.id
     WHERE s.node_id IS NULL;
    IF n_free = 0 THEN
        RAISE NOTICE 'свободных узлов нет, проверка записи пропущена';
    ELSE
        INSERT INTO compat.setpressnodes (id, nodeid, pressflow, pressret)
        SELECT -999998, n.id, 12.3, 4.5
          FROM net.v_map_nodes n
          LEFT JOIN net.node_press_setting s ON s.node_id = n.id
         WHERE s.node_id IS NULL
         LIMIT 1
        RETURNING id INTO new_id;

        IF NOT EXISTS (SELECT 1 FROM net.node_press_setting
                        WHERE src_id = new_id AND pressflow = 12.3) THEN
            RAISE EXCEPTION 'INSERT через compat не дошёл до net';
        END IF;

        UPDATE net.v_setpressnodes SET pressflow = 13.7 WHERE id = new_id;
        SELECT pressflow INTO v
          FROM net.node_press_setting WHERE src_id = new_id;
        IF v IS DISTINCT FROM 13.7 THEN
            RAISE EXCEPTION 'UPDATE через net.v_setpressnodes не дошёл: %', v;
        END IF;

        DELETE FROM net.v_setpressnodes WHERE id = new_id;
        IF EXISTS (SELECT 1 FROM net.node_press_setting
                    WHERE src_id = new_id) THEN
            RAISE EXCEPTION 'DELETE через net.v_setpressnodes не дошёл';
        END IF;
    END IF;

    -- 4. Изъятый источник сохранён, а не уничтожен.
    IF to_regclass('attic.setpressnodes') IS NULL THEN
        RAISE EXCEPTION 'attic.setpressnodes отсутствует — исходник потерян';
    END IF;

    RAISE NOTICE 'setpressnodes: один источник (% строк), запись доходит '
                 'через compat и net, исходник на чердаке', n_view;
END $$;

ROLLBACK;
