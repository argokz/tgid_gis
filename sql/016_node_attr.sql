-- Атрибуты узла в реестре — чтобы не соединяться с представлением nodes.
--
-- Запросы расчётного ядра и отчётов соединяются с nodes ради нескольких
-- полей: фрагмента, признака удаления, внутреннего узла и внешнего кода.
-- В схеме net public.nodes — представление из 10 ветвей, и такое
-- соединение стоит дорого: чтение участков теплопровода занимало 1841 мс
-- против 252 мс при чтении напрямую из объектной таблицы.
--
-- Реестр net.node_reg уже существует, содержит по строке на узел и имеет
-- первичный ключ. Добавляем в него горячие атрибуты — соединение
-- становится обращением к обычной таблице с индексом.
--
-- Значения поддерживаются теми же триггерами, что и сам реестр,
-- плюс триггером на UPDATE (его раньше не было — реестр реагировал
-- только на вставку и удаление).

ALTER TABLE net.node_reg ADD COLUMN IF NOT EXISTS fragment_id    int;
ALTER TABLE net.node_reg ADD COLUMN IF NOT EXISTS internalnodeid int;
ALTER TABLE net.node_reg ADD COLUMN IF NOT EXISTS externalcodeid int;
ALTER TABLE net.node_reg ADD COLUMN IF NOT EXISTS externalsignid int;
ALTER TABLE net.node_reg ADD COLUMN IF NOT EXISTS externalnodename text;
ALTER TABLE net.node_reg ADD COLUMN IF NOT EXISTS removed        boolean NOT NULL DEFAULT false;

CREATE INDEX IF NOT EXISTS node_reg_frag_idx
    ON net.node_reg (fragment_id) WHERE NOT removed;

CREATE OR REPLACE FUNCTION net.reg_node_sync() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    IF TG_OP = 'DELETE' THEN
        DELETE FROM net.node_reg
        WHERE id = OLD.id AND tbl = TG_TABLE_NAME;
        RETURN OLD;
    END IF;

    -- INSERT и UPDATE: строка реестра создаётся или обновляется.
    -- ON CONFLICT нужен и для переноса объекта между классами
    -- (net.reclass_node вставляет в новую таблицу до удаления из старой).
    INSERT INTO net.node_reg (id, kind, tbl, fragment_id, internalnodeid,
                              externalcodeid, externalsignid,
                              externalnodename, removed)
    VALUES (NEW.id, TG_ARGV[0], TG_TABLE_NAME, NEW.fragment_id,
            NEW.internalnodeid, NEW.externalcodeid, NEW.externalsignid,
            NEW.externalnodename, NEW.removed_at IS NOT NULL)
    ON CONFLICT (id) DO UPDATE
        SET kind = EXCLUDED.kind, tbl = EXCLUDED.tbl,
            fragment_id = EXCLUDED.fragment_id,
            internalnodeid = EXCLUDED.internalnodeid,
            externalcodeid = EXCLUDED.externalcodeid,
            externalsignid = EXCLUDED.externalsignid,
            externalnodename = EXCLUDED.externalnodename,
            removed = EXCLUDED.removed;
    RETURN NEW;
END $$;

-- Перевесить триггеры: теперь они должны срабатывать и на UPDATE.
DO $$
DECLARE t text;
BEGIN
    FOR t IN
        SELECT c.relname
        FROM pg_class c
        JOIN pg_namespace n ON n.oid = c.relnamespace
        JOIN pg_trigger tg ON tg.tgrelid = c.oid AND NOT tg.tgisinternal
        -- Отбираем строго по текущей функции триггера. Отбор по набору
        -- колонок не годится: internalnodeid есть и в линейных таблицах
        -- (он унаследован от linesobj), и узловой триггер навешивался
        -- на них — вставка линии падала на отсутствии externalcodeid.
        WHERE n.nspname = 'net' AND tg.tgname = c.relname || '_reg'
          AND tg.tgfoid = 'net.reg_node_sync'::regproc
    LOOP
        EXECUTE format('DROP TRIGGER IF EXISTS %I ON net.%I', t || '_reg', t);
        EXECUTE format(
            'CREATE TRIGGER %I AFTER INSERT OR UPDATE OR DELETE ON net.%I '
            'FOR EACH ROW EXECUTE FUNCTION net.reg_node_sync(%L)',
            t || '_reg', t, t);
    END LOOP;
END $$;

-- Разовое заполнение для уже перенесённых данных.
DO $$
DECLARE t text;
BEGIN
    FOR t IN SELECT DISTINCT tbl FROM net.node_reg
    LOOP
        EXECUTE format(
            'UPDATE net.node_reg r SET fragment_id = o.fragment_id, '
            '  internalnodeid = o.internalnodeid, '
            '  externalcodeid = o.externalcodeid, '
            '  externalsignid = o.externalsignid, '
            '  externalnodename = o.externalnodename, '
            '  removed = (o.removed_at IS NOT NULL) '
            'FROM net.%I o WHERE o.id = r.id AND r.tbl = %L', t, t);
    END LOOP;
END $$;

ANALYZE net.node_reg;

\echo 'Атрибуты узла добавлены в реестр'
