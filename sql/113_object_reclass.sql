-- Безопасная смена класса сетевого объекта для Qt и прямых SQL-клиентов.

\set ON_ERROR_STOP on

BEGIN;

ALTER TABLE meta.object_change_log
    DROP CONSTRAINT IF EXISTS object_change_log_operation_check;

ALTER TABLE meta.object_change_log
    ADD CONSTRAINT object_change_log_operation_check
    CHECK (operation IN (
        'insert', 'update', 'archive', 'restore', 'reclass'));

DROP FUNCTION IF EXISTS net.reclass_object(
    text, bigint, bigint, text
);

CREATE FUNCTION net.reclass_object(
    p_source text,
    p_object_id bigint,
    p_expected_version bigint,
    p_target text
)
RETURNS TABLE (
    target_table text,
    new_version bigint,
    copied_fields integer,
    object_is_node boolean
)
LANGUAGE plpgsql
SET search_path = pg_catalog, public, net
AS $$
DECLARE
    registered_table text;
    source_kind text;
    target_kind text;
    source_version bigint;
    source_removed_at timestamptz;
    source_geom geometry;
    source_columns text;
    source_values text;
    lost_columns text[];
    nonnull_lost_columns text[];
    required_target_columns text[];
    target_editable boolean;
    inserted_version bigint;
BEGIN
    IF p_source IS NULL OR p_source !~ '^[a-z_][a-z0-9_]*$'
       OR p_target IS NULL OR p_target !~ '^[a-z_][a-z0-9_]*$'
       OR p_object_id <= 0 OR p_expected_version <= 0 THEN
        RAISE EXCEPTION 'Некорректные параметры смены класса';
    END IF;
    IF p_source = p_target THEN
        RAISE EXCEPTION 'Исходный и целевой классы совпадают';
    END IF;

    SELECT CASE
               WHEN tg.tgfoid = 'net.reg_node_sync'::regproc THEN 'node'
               WHEN tg.tgfoid = 'net.reg_line_sync'::regproc THEN 'line'
           END
      INTO source_kind
      FROM pg_trigger tg
      JOIN pg_class c ON c.oid = tg.tgrelid
      JOIN pg_namespace n ON n.oid = c.relnamespace
     WHERE n.nspname = 'net'
       AND c.relname = p_source
       AND NOT tg.tgisinternal
       AND tg.tgfoid IN (
           'net.reg_node_sync'::regproc,
           'net.reg_line_sync'::regproc)
     LIMIT 1;
    IF source_kind IS NULL THEN
        RAISE EXCEPTION 'Класс net.% не относится к сетевым объектам', p_source;
    END IF;

    IF source_kind = 'node' THEN
        SELECT tbl
          INTO registered_table
          FROM net.node_reg
         WHERE id = p_object_id
         FOR UPDATE;
    ELSE
        SELECT tbl
          INTO registered_table
          FROM net.line_reg
         WHERE id = p_object_id
         FOR UPDATE;
    END IF;
    IF registered_table IS NULL THEN
        RAISE EXCEPTION 'Объект % отсутствует в сетевых реестрах', p_object_id;
    END IF;
    IF registered_table IS DISTINCT FROM p_source THEN
        RAISE EXCEPTION
            'Объект % относится к net.%, а не к net.%',
            p_object_id, registered_table, p_source;
    END IF;

    SELECT CASE
               WHEN tg.tgfoid = 'net.reg_node_sync'::regproc THEN 'node'
               WHEN tg.tgfoid = 'net.reg_line_sync'::regproc THEN 'line'
           END
      INTO target_kind
      FROM pg_trigger tg
      JOIN pg_class c ON c.oid = tg.tgrelid
      JOIN pg_namespace n ON n.oid = c.relnamespace
     WHERE n.nspname = 'net'
       AND c.relname = p_target
       AND NOT tg.tgisinternal
       AND tg.tgfoid IN (
           'net.reg_node_sync'::regproc,
           'net.reg_line_sync'::regproc)
     LIMIT 1;
    IF target_kind IS NULL OR target_kind IS DISTINCT FROM source_kind THEN
        RAISE EXCEPTION
            'Класс net.% не относится к сетевым объектам типа %',
            p_target, source_kind;
    END IF;

    SELECT is_editable
      INTO target_editable
      FROM meta.layer_catalog
     WHERE schema_name = 'net' AND table_name = p_target;
    IF NOT coalesce(target_editable, false) THEN
        RAISE EXCEPTION 'Целевой класс net.% недоступен для записи', p_target;
    END IF;

    EXECUTE format(
        'SELECT row_version, removed_at, geom
           FROM net.%I
          WHERE id = $1
          FOR UPDATE',
        p_source)
    INTO source_version, source_removed_at, source_geom
    USING p_object_id;
    IF source_geom IS NULL THEN
        RAISE EXCEPTION 'Объект % не найден или не имеет геометрии', p_object_id;
    END IF;
    IF source_removed_at IS NOT NULL THEN
        RAISE EXCEPTION 'Архивный объект % нельзя переклассифицировать', p_object_id;
    END IF;
    IF source_version IS DISTINCT FROM p_expected_version THEN
        RAISE EXCEPTION
            'CONFLICT: версия объекта % изменилась: ожидалась %, текущая %',
            p_object_id, p_expected_version, source_version;
    END IF;

    SELECT array_agg(a.attname ORDER BY a.attnum)
      INTO lost_columns
      FROM pg_attribute a
     WHERE a.attrelid = format('net.%I', p_source)::regclass
       AND a.attnum > 0 AND NOT a.attisdropped
       AND a.attname NOT IN (
           'id', 'geom', 'removed_at', 'row_version',
           'updated_at', 'updated_by', 'src_id', 'subtype_src_id')
       AND NOT EXISTS (
           SELECT 1 FROM pg_attribute b
            WHERE b.attrelid = format('net.%I', p_target)::regclass
              AND b.attname = a.attname
              AND b.attnum > 0 AND NOT b.attisdropped);
    IF lost_columns IS NOT NULL THEN
        EXECUTE format(
            'SELECT array_agg(e.key ORDER BY e.key)
               FROM net.%I AS source_row,
                    LATERAL jsonb_each(to_jsonb(source_row)) AS e(key, value)
              WHERE source_row.id = $1
                AND e.key = ANY($2)
                AND e.value <> ''null''::jsonb',
            p_source)
        INTO nonnull_lost_columns
        USING p_object_id, lost_columns;
    END IF;
    IF nonnull_lost_columns IS NOT NULL THEN
        RAISE EXCEPTION
            'Смена класса приведёт к потере заполненных полей: %',
            array_to_string(nonnull_lost_columns, ', ');
    END IF;

    SELECT array_agg(a.attname ORDER BY a.attnum)
      INTO required_target_columns
      FROM pg_attribute a
     WHERE a.attrelid = format('net.%I', p_target)::regclass
       AND a.attnum > 0 AND NOT a.attisdropped
       AND a.attnotnull
       AND a.atthasdef = false
       AND a.attgenerated = ''
       AND a.attidentity = ''
       AND NOT EXISTS (
           SELECT 1 FROM pg_attribute b
            WHERE b.attrelid = format('net.%I', p_source)::regclass
              AND b.attname = a.attname
              AND b.attnum > 0 AND NOT b.attisdropped);
    IF required_target_columns IS NOT NULL THEN
        RAISE EXCEPTION
            'В целевом классе обязательны отсутствующие поля: %',
            array_to_string(required_target_columns, ', ');
    END IF;

    SELECT
        string_agg(format('%I', source_col.attname), ', '
                   ORDER BY source_col.attnum),
        string_agg(
            CASE source_col.attname
                WHEN 'row_version' THEN '$2 + 1'
                WHEN 'updated_at' THEN 'clock_timestamp()'
                WHEN 'updated_by' THEN 'session_user'
                ELSE format('source_row.%I', source_col.attname)
            END,
            ', ' ORDER BY source_col.attnum),
        count(*) FILTER (
            WHERE source_col.attname NOT IN (
                'id', 'geom', 'removed_at', 'row_version',
                'updated_at', 'updated_by'))::integer
      INTO source_columns, source_values, copied_fields
      FROM pg_attribute source_col
      JOIN pg_attribute target_col
        ON target_col.attrelid = format('net.%I', p_target)::regclass
       AND target_col.attname = source_col.attname
       AND target_col.attnum > 0 AND NOT target_col.attisdropped
       AND target_col.attgenerated = ''
       AND target_col.attidentity = ''
     WHERE source_col.attrelid = format('net.%I', p_source)::regclass
       AND source_col.attnum > 0 AND NOT source_col.attisdropped
       AND source_col.attgenerated = ''
       AND source_col.attidentity = '';
    IF source_columns IS NULL THEN
        RAISE EXCEPTION 'У классов нет общих переносимых полей';
    END IF;

    EXECUTE format(
        'INSERT INTO net.%1$I (%2$s)
         SELECT %3$s FROM net.%4$I AS source_row WHERE source_row.id = $1
         RETURNING row_version',
        p_target, source_columns, source_values, p_source)
    INTO inserted_version
    USING p_object_id, p_expected_version;
    IF inserted_version IS DISTINCT FROM p_expected_version + 1 THEN
        RAISE EXCEPTION 'Целевой объект получил некорректную версию';
    END IF;

    EXECUTE format('DELETE FROM net.%I WHERE id = $1', p_source)
    USING p_object_id;

    UPDATE meta.object_change_log
       SET operation = 'reclass',
           changed_fields = jsonb_build_object(
               'class', jsonb_build_object(
                   'old', p_source, 'new', p_target),
               'copied_fields', copied_fields)
     WHERE id = (
         SELECT id
           FROM meta.object_change_log
          WHERE schema_name = 'net'
            AND table_name = p_target
            AND object_id = p_object_id
            AND row_version = inserted_version
            AND operation = 'insert'
          ORDER BY id DESC
          LIMIT 1);
    IF NOT FOUND THEN
        RAISE EXCEPTION 'Не удалось записать смену класса в журнал изменений';
    END IF;

    target_table := p_target;
    new_version := inserted_version;
    object_is_node := source_kind = 'node';
    RETURN NEXT;
END
$$;

COMMENT ON FUNCTION net.reclass_object(text, bigint, bigint, text) IS
    'Атомарно меняет класс узла или линии без потери заполненных полей';

INSERT INTO meta.schema_version (version, description)
VALUES (12, 'Безопасная смена класса сетевых объектов')
ON CONFLICT (version) DO NOTHING;

COMMIT;
