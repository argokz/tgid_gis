-- Функции реестра и смены класса (выделено из 010 для повторного применения).

CREATE OR REPLACE FUNCTION net.reg_node_sync() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO net.node_reg (id, kind, tbl)
        VALUES (NEW.id, TG_ARGV[0], TG_TABLE_NAME)
        ON CONFLICT (id) DO UPDATE
            SET kind = EXCLUDED.kind, tbl = EXCLUDED.tbl;
        RETURN NEW;
    END IF;
    DELETE FROM net.node_reg
    WHERE id = OLD.id AND tbl = TG_TABLE_NAME;
    RETURN OLD;
END $$;

CREATE OR REPLACE FUNCTION net.reg_line_sync() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO net.line_reg (id, kind, tbl)
        VALUES (NEW.id, TG_ARGV[0], TG_TABLE_NAME)
        ON CONFLICT (id) DO UPDATE
            SET kind = EXCLUDED.kind, tbl = EXCLUDED.tbl;
        RETURN NEW;
    END IF;
    DELETE FROM net.line_reg
    WHERE id = OLD.id AND tbl = TG_TABLE_NAME;
    RETURN OLD;
END $$;

-- Перенос объекта в другой класс: общие колонки копируются, остальные
-- получают значения по умолчанию. Порядок важен — сначала вставка
-- (она обновит реестр), потом удаление (реестр уже указывает на новую
-- таблицу, поэтому строка реестра уцелеет).
CREATE OR REPLACE FUNCTION net.reclass_node(p_id bigint, p_target text)
RETURNS void
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $$
DECLARE
    src  text;
    cols text;
BEGIN
    SELECT tbl INTO src FROM net.node_reg WHERE id = p_id;
    IF src IS NULL THEN
        RAISE EXCEPTION 'узел % не найден в реестре', p_id;
    END IF;
    IF src = p_target THEN
        RETURN;
    END IF;

    SELECT string_agg(quote_ident(a.column_name), ', ')
    INTO cols
    FROM information_schema.columns a
    JOIN information_schema.columns b
      ON b.table_schema = 'net' AND b.table_name = p_target
     AND b.column_name = a.column_name
    WHERE a.table_schema = 'net' AND a.table_name = src
      AND a.is_generated = 'NEVER' AND b.is_generated = 'NEVER';

    EXECUTE format('INSERT INTO net.%I (%s) SELECT %s FROM net.%I WHERE id = $1',
                   p_target, cols, cols, src) USING p_id;
    EXECUTE format('DELETE FROM net.%I WHERE id = $1', src) USING p_id;
END $$;


