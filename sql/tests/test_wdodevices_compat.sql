-- compat.wdodevices поверх net.node_metering_device (sql/179).
-- Чтение отдаёт только живые строки, запись уходит в целевую таблицу.
\set ON_ERROR_STOP on
BEGIN;
SET search_path TO compat, public, net, ref, calc;

DO $$
DECLARE
    n_view int;
    n_net  int;
    new_id int;
    v_node bigint;
    n_attic_only int;
BEGIN
    SELECT count(*) INTO n_view FROM compat.wdodevices;
    SELECT count(*) INTO n_net  FROM net.node_metering_device;
    ASSERT n_view = n_net, format('view/net расходятся: %s vs %s', n_view, n_net);

    -- ни одной строки представления не должно быть "мёртвой" из attic
    SELECT count(*) INTO n_attic_only
    FROM compat.wdodevices w
    WHERE NOT EXISTS (SELECT 1 FROM net.node_reg n WHERE n.id = w.nodeid);
    ASSERT n_attic_only = 0,
        format('в представлении %s строк без живого узла', n_attic_only);

    -- запись через представление
    SELECT n.id INTO v_node FROM net.node_reg n
    WHERE NOT n.removed LIMIT 1;

    INSERT INTO compat.wdodevices (nodeid, externalsignid, wdo, hydrores)
    VALUES (v_node, 2, 1.5, 0.7)
    RETURNING id INTO new_id;
    ASSERT new_id IS NOT NULL AND new_id > 0, 'RETURNING id пустой';

    PERFORM 1 FROM net.node_metering_device
    WHERE id = new_id AND node_id = v_node AND wdo = 1.5;
    ASSERT FOUND, 'строка не долетела до net.node_metering_device';

    UPDATE compat.wdodevices SET wdo = 2.5 WHERE id = new_id;
    PERFORM 1 FROM net.node_metering_device
    WHERE id = new_id AND wdo = 2.5;
    ASSERT FOUND, 'UPDATE через представление не сработал';

    DELETE FROM compat.wdodevices WHERE id = new_id;
    PERFORM 1 FROM net.node_metering_device WHERE id = new_id;
    ASSERT NOT FOUND, 'DELETE через представление не сработал';
END $$;

ROLLBACK;
SELECT 'test_wdodevices_compat: OK' AS result;
