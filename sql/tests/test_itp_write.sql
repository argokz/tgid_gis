-- Запись через ИТП-представления и net.reclass_line (sql/180).
-- Проверяет и пять новых триггеров, и ранее мёртвый путь
-- v_pressregulators, который падал без reclass_line.
\set ON_ERROR_STOP on
BEGIN;
SET search_path TO compat, public, net, ref, calc;

DO $$
DECLARE
    v_line bigint;
    v_node bigint;
    v_tbl  text;
BEGIN
    SELECT r.id INTO v_line FROM net.line_reg r
    WHERE r.tbl = 'line_plain' LIMIT 1;
    SELECT l.node_from INTO v_node FROM net.line_plain l WHERE l.id = v_line;

    -- 1. reclass_line сам по себе: line_plain -> bypass -> line_plain
    PERFORM net.reclass_line(v_line, 'bypass');
    SELECT tbl INTO v_tbl FROM net.line_reg WHERE id = v_line;
    ASSERT v_tbl = 'bypass', format('ожидали bypass, стало %s', v_tbl);
    PERFORM net.reclass_line(v_line, 'line_plain');
    SELECT tbl INTO v_tbl FROM net.line_reg WHERE id = v_line;
    ASSERT v_tbl = 'line_plain', format('не вернули line_plain: %s', v_tbl);

    -- 2. INSERT через v_bypass: reclass + атрибуты + subtype_src_id
    INSERT INTO net.v_bypass (id, lineid, nodeid, length, diameterinternal)
    VALUES (900001, v_line, v_node, 12, 100);
    SELECT tbl INTO v_tbl FROM net.line_reg WHERE id = v_line;
    ASSERT v_tbl = 'bypass', format('INSERT v_bypass: класс %s', v_tbl);
    PERFORM 1 FROM net.bypass
    WHERE id = v_line AND subtype_src_id = 900001 AND length = 12;
    ASSERT FOUND, 'INSERT v_bypass: атрибуты не записались';

    -- 3. UPDATE через v_bypass
    UPDATE net.v_bypass SET length = 15 WHERE lineid = v_line;
    PERFORM 1 FROM net.bypass WHERE id = v_line AND length = 15;
    ASSERT FOUND, 'UPDATE v_bypass не сработал';

    -- 4. DELETE через v_bypass: снятие типа, объект остаётся
    DELETE FROM net.v_bypass WHERE lineid = v_line;
    SELECT tbl INTO v_tbl FROM net.line_reg WHERE id = v_line;
    ASSERT v_tbl = 'line_plain',
        format('DELETE v_bypass: класс %s', v_tbl);
    PERFORM 1 FROM net.line_reg WHERE id = v_line;
    ASSERT FOUND, 'DELETE v_bypass удалил сам объект';

    -- 5. ранее мёртвый путь: INSERT через v_pressregulators
    INSERT INTO net.v_pressregulators (id, lineid, nodeid, valvehydroresopen)
    VALUES (900002, v_line, v_node, 3.3);
    SELECT tbl INTO v_tbl FROM net.line_reg WHERE id = v_line;
    ASSERT v_tbl = 'regulator_press',
        format('INSERT v_pressregulators: класс %s', v_tbl);
    DELETE FROM net.v_pressregulators WHERE lineid = v_line;
    SELECT tbl INTO v_tbl FROM net.line_reg WHERE id = v_line;
    ASSERT v_tbl = 'line_plain',
        format('DELETE v_pressregulators: класс %s', v_tbl);

    -- 6. consumptregulators: reclass в regulator_consumption
    INSERT INTO net.v_consumptregulators (id, lineid, nodeid, regconsmean)
    VALUES (900003, v_line, v_node, 7.7);
    SELECT tbl INTO v_tbl FROM net.line_reg WHERE id = v_line;
    ASSERT v_tbl = 'regulator_consumption',
        format('INSERT v_consumptregulators: класс %s', v_tbl);
    DELETE FROM net.v_consumptregulators WHERE lineid = v_line;

    -- 7. reversevalves
    INSERT INTO net.v_reversevalves (id, lineid, hydroresopen)
    VALUES (900004, v_line, 1.1);
    SELECT tbl INTO v_tbl FROM net.line_reg WHERE id = v_line;
    ASSERT v_tbl = 'valve_reverse',
        format('INSERT v_reversevalves: класс %s', v_tbl);
    DELETE FROM net.v_reversevalves WHERE lineid = v_line;

    SELECT tbl INTO v_tbl FROM net.line_reg WHERE id = v_line;
    ASSERT v_tbl = 'line_plain',
        format('финальный класс %s вместо line_plain', v_tbl);
END $$;

ROLLBACK;
SELECT 'test_itp_write: OK' AS result;
