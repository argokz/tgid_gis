-- Полный каталог полей опубликованных объектных таблиц.
--
-- 105_ui_field_catalog.sql задаёт бизнес-подписи и справочники для известных
-- полей. Этот шаг добавляет безопасные метаданные для всех остальных колонок,
-- чтобы Qt не зависел от неописанного fallback-поведения.

BEGIN;

ALTER TABLE meta.field_catalog
    DROP CONSTRAINT IF EXISTS field_catalog_editor_kind_check;

ALTER TABLE meta.field_catalog
    ADD CONSTRAINT field_catalog_editor_kind_check
    CHECK (editor_kind IN (
        'text', 'multiline', 'boolean', 'lookup',
        'integer', 'decimal', 'date', 'datetime'
    ));

WITH discovered AS (
    SELECT
        n.nspname AS table_schema,
        c.relname AS table_name,
        a.attname AS column_name,
        coalesce(
            nullif(col_description(c.oid, a.attnum), ''),
            CASE a.attname
                WHEN 'id' THEN 'Идентификатор'
                WHEN 'fragment_id' THEN 'Фрагмент'
                WHEN 'row_version' THEN 'Версия строки'
                WHEN 'updated_at' THEN 'Изменено'
                WHEN 'updated_by' THEN 'Кем изменено'
                WHEN 'src_id' THEN 'Идентификатор источника'
                WHEN 'subtype_src_id' THEN 'Идентификатор подтипа источника'
                WHEN 'fileid_src' THEN 'Файл источника'
                WHEN 'node_from' THEN 'Начальный узел'
                WHEN 'node_to' THEN 'Конечный узел'
                WHEN 'node_from_src' THEN 'Начальный узел источника'
                WHEN 'node_to_src' THEN 'Конечный узел источника'
                WHEN 'removed_at' THEN 'Дата архивирования'
                WHEN 'coords_legacy' THEN 'Исходные координаты'
                ELSE replace(a.attname, '_', ' ')
            END
        ) AS display_name,
        CASE t.typname
            WHEN 'bool' THEN 'boolean'
            WHEN 'int2' THEN 'integer'
            WHEN 'int4' THEN 'integer'
            WHEN 'int8' THEN 'integer'
            WHEN 'float4' THEN 'decimal'
            WHEN 'float8' THEN 'decimal'
            WHEN 'numeric' THEN 'decimal'
            WHEN 'date' THEN 'date'
            WHEN 'timestamp' THEN 'datetime'
            WHEN 'timestamptz' THEN 'datetime'
            WHEN 'json' THEN 'multiline'
            WHEN 'jsonb' THEN 'multiline'
            ELSE 'text'
        END AS editor_kind,
        CASE
            WHEN a.attname IN (
                'node_from', 'node_to', 'node_from_src', 'node_to_src'
            ) THEN 8000 + a.attnum
            WHEN a.attname IN (
                'id', 'fragment_id', 'row_version', 'updated_at',
                'updated_by', 'removed_at'
            ) THEN 9000 + a.attnum
            WHEN a.attname IN (
                'src_id', 'subtype_src_id', 'fileid_src', 'coords_legacy'
            ) THEN 9500 + a.attnum
            ELSE 5000 + a.attnum
        END AS display_order,
        CASE
            WHEN a.attname IN (
                'node_from', 'node_to', 'node_from_src', 'node_to_src'
            ) THEN 'Топология'
            WHEN a.attname IN (
                'id', 'fragment_id', 'row_version', 'updated_at',
                'updated_by', 'removed_at'
            ) THEN 'Системные'
            WHEN a.attname IN (
                'src_id', 'subtype_src_id', 'fileid_src', 'coords_legacy'
            ) THEN 'Наследие'
            ELSE 'Параметры'
        END AS group_name,
        a.attname NOT IN (
            'id', 'fragment_id', 'node_from', 'node_to',
            'node_from_src', 'node_to_src', 'fileid_src', 'src_id',
            'removed_at', 'row_version', 'updated_at', 'updated_by',
            'coords_legacy'
        ) AS is_editable
    FROM meta.layer_catalog layer
    JOIN pg_namespace n
      ON n.nspname = layer.schema_name
    JOIN pg_class c
      ON c.relnamespace = n.oid
     AND c.relname = layer.table_name
    JOIN pg_attribute a
      ON a.attrelid = c.oid
    JOIN pg_type t
      ON t.oid = a.atttypid
    WHERE layer.schema_name = 'net'
      AND a.attnum > 0
      AND NOT a.attisdropped
      AND a.attname <> 'geom'
)
INSERT INTO meta.field_catalog (
    table_schema,
    table_name,
    column_name,
    display_name,
    editor_kind,
    display_order,
    group_name,
    is_visible,
    is_editable
)
SELECT
    table_schema,
    table_name,
    column_name,
    display_name,
    editor_kind,
    display_order,
    group_name,
    true,
    is_editable
FROM discovered
ON CONFLICT (table_schema, table_name, column_name) DO UPDATE
SET display_name = EXCLUDED.display_name,
    editor_kind = EXCLUDED.editor_kind,
    display_order = EXCLUDED.display_order,
    group_name = EXCLUDED.group_name,
    is_visible = EXCLUDED.is_visible,
    is_editable = EXCLUDED.is_editable
WHERE meta.field_catalog.display_order >= 5000;

INSERT INTO meta.schema_version (version, description)
VALUES (8, 'Полный каталог полей и типизированные редакторы Qt')
ON CONFLICT (version) DO NOTHING;

COMMIT;
