-- compat.setpressnodes обязана читаться и писаться как исходная таблица.
--
-- Три проверки: число строк совпадает с net.node_press_setting
-- (за вычетом семи узлов без пары — известный, объяснённый остаток),
-- UPDATE доходит до net, INSERT создаёт строку, видимую и оттуда,
-- и обратно.
--
--   psql -d tgid_gis -v ON_ERROR_STOP=1 -f sql/tests/test_compat_setpressnodes.sql

BEGIN;

DO $$
DECLARE
    n_view   bigint;
    n_net    bigint;
    test_id  integer;
    before_v double precision;
    after_v  double precision;
    new_id   integer;
BEGIN
    IF to_regclass('compat.setpressnodes') IS NULL THEN
        RAISE EXCEPTION 'compat.setpressnodes не существует';
    END IF;

    SELECT count(*) INTO n_view FROM compat.setpressnodes;
    SELECT count(*) INTO n_net
      FROM net.node_press_setting WHERE src_id IS NOT NULL;
    IF n_view <> n_net THEN
        RAISE EXCEPTION 'чтение расходится: compat=% net=%', n_view, n_net;
    END IF;

    -- UPDATE
    SELECT id, pressflow INTO test_id, before_v
      FROM compat.setpressnodes LIMIT 1;
    UPDATE compat.setpressnodes SET pressflow = pressflow + 1
     WHERE id = test_id;
    SELECT pressflow INTO after_v
      FROM net.node_press_setting WHERE src_id = test_id;
    IF after_v IS DISTINCT FROM before_v + 1 THEN
        RAISE EXCEPTION 'UPDATE не дошёл до net: было % стало %',
                        before_v, after_v;
    END IF;

    -- INSERT: не встречается в приложении, но проверяем на случай
    -- будущего использования (миграционные скрипты его делают).
    -- node_id уникален (node_press_setting_node_uidx), поэтому берём
    -- узел, у которого такой строки ещё нет.
    INSERT INTO compat.setpressnodes (id, nodeid, pressflow, pressret)
    SELECT -999999, n.id, 12.3, 4.5
    FROM net.node_reg n
    WHERE NOT EXISTS (SELECT 1 FROM net.node_press_setting o
                       WHERE o.node_id = n.id)
    LIMIT 1
    RETURNING id INTO new_id;

    IF NOT EXISTS (SELECT 1 FROM net.node_press_setting
                    WHERE src_id = new_id AND pressflow = 12.3) THEN
        RAISE EXCEPTION 'INSERT не дошёл до net.node_press_setting';
    END IF;

    RAISE NOTICE 'compat.setpressnodes: чтение % строк, UPDATE и INSERT '
                 'доходят до net.node_press_setting', n_view;
END $$;

ROLLBACK;
