-- Метаданные универсальных карточек Qt.
--
-- Каталог не дублирует значения объектов. Он задаёт подписи, единицы
-- измерения, порядок и безопасные ссылки на справочники ref.

BEGIN;

CREATE SCHEMA IF NOT EXISTS meta;

CREATE TABLE IF NOT EXISTS meta.field_catalog (
    table_schema        text NOT NULL DEFAULT 'net',
    table_name          text NOT NULL,
    column_name         text NOT NULL,
    display_name        text NOT NULL,
    unit                text,
    editor_kind         text NOT NULL DEFAULT 'text'
                        CHECK (editor_kind IN (
                            'text', 'multiline', 'boolean', 'lookup'
                        )),
    lookup_schema       text,
    lookup_table        text,
    lookup_value_column text,
    lookup_label_column text,
    display_order       integer NOT NULL DEFAULT 1000,
    group_name          text,
    is_visible          boolean NOT NULL DEFAULT true,
    is_editable         boolean,
    PRIMARY KEY (table_schema, table_name, column_name),
    CHECK (
        editor_kind <> 'lookup'
        OR (
            lookup_schema IS NOT NULL
            AND lookup_table IS NOT NULL
            AND lookup_value_column IS NOT NULL
            AND lookup_label_column IS NOT NULL
        )
    )
);

COMMENT ON TABLE meta.field_catalog IS
    'Подписи, порядок и редакторы полей объектных карточек Qt/QGIS';

CREATE INDEX IF NOT EXISTS field_catalog_order_idx
    ON meta.field_catalog (table_schema, table_name, display_order, column_name);

WITH specs (
    column_name,
    display_name,
    unit,
    editor_kind,
    lookup_schema,
    lookup_table,
    lookup_value_column,
    lookup_label_column,
    display_order,
    group_name
) AS (
    VALUES
        ('name',               'Наименование',              NULL, 'text',   NULL,  NULL,              NULL, NULL,       10, 'Основные'),
        ('nodename',           'Наименование узла',         NULL, 'text',   NULL,  NULL,              NULL, NULL,       11, 'Основные'),
        ('externalnodename',   'Внешнее имя узла',          NULL, 'text',   NULL,  NULL,              NULL, NULL,       12, 'Основные'),
        ('sourcename',         'Наименование источника',     NULL, 'text',   NULL,  NULL,              NULL, NULL,       13, 'Основные'),
        ('location',           'Местоположение',             NULL, 'text',   NULL,  NULL,              NULL, NULL,       20, 'Основные'),
        ('registnum',          'Регистрационный номер',      NULL, 'text',   NULL,  NULL,              NULL, NULL,       21, 'Основные'),
        ('registnumber',       'Регистрационный номер',      NULL, 'text',   NULL,  NULL,              NULL, NULL,       21, 'Основные'),
        ('inventnumber',       'Инвентарный номер',         NULL, 'text',   NULL,  NULL,              NULL, NULL,       22, 'Основные'),
        ('organizationid',     'Организация',                NULL, 'lookup', 'ref', 'organizations',   'id', 'name',     30, 'Принадлежность'),
        ('nodetypeid',         'Тип узла',                   NULL, 'lookup', 'ref', 'nodetypes',       'id', 'name',     31, 'Классификация'),
        ('standardid',         'Стандарт трубы',             NULL, 'lookup', 'ref', 'standards',       'id', 'name',     32, 'Трубопровод'),
        ('tubingtypeid',       'Тип прокладки',              NULL, 'lookup', 'ref', 'tubingtypes',     'id', 'name',     33, 'Трубопровод'),
        ('isolmaterialid',     'Материал изоляции',          NULL, 'lookup', 'ref', 'isolmaterials',   'id', 'name',     34, 'Трубопровод'),
        ('standardpumpid',     'Стандартный насос',          NULL, 'lookup', 'ref', 'standardpumps',   'id', 'name',     35, 'Оборудование'),
        ('standarddamplink',   'Стандартная задвижка',       NULL, 'lookup', 'ref', 'standarddampers', 'id', 'name',     36, 'Оборудование'),
        ('diametercondit',     'Условный диаметр',           'мм', 'text',   NULL,  NULL,              NULL, NULL,       40, 'Трубопровод'),
        ('diameterinternal',   'Внутренний диаметр',        'мм', 'text',   NULL,  NULL,              NULL, NULL,       41, 'Трубопровод'),
        ('diameterexternal',   'Наружный диаметр',          'мм', 'text',   NULL,  NULL,              NULL, NULL,       42, 'Трубопровод'),
        ('wallthickness',      'Толщина стенки',            'мм', 'text',   NULL,  NULL,              NULL, NULL,       43, 'Трубопровод'),
        ('isolthickness',      'Толщина изоляции',          'мм', 'text',   NULL,  NULL,              NULL, NULL,       44, 'Трубопровод'),
        ('pipesectlength',     'Длина участка',               'м', 'text',   NULL,  NULL,              NULL, NULL,       45, 'Трубопровод'),
        ('firstpicdate',       'Дата ввода в эксплуатацию', NULL, 'text',   NULL,  NULL,              NULL, NULL,       60, 'Эксплуатация'),
        ('lastmaintdate',      'Дата последнего ремонта',   NULL, 'text',   NULL,  NULL,              NULL, NULL,       61, 'Эксплуатация'),
        ('memo',               'Примечание',                NULL, 'multiline', NULL, NULL,            NULL, NULL,      900, 'Дополнительно'),
        ('note_1',             'Примечание 1',              NULL, 'multiline', NULL, NULL,            NULL, NULL,      901, 'Дополнительно'),
        ('note_2',             'Примечание 2',              NULL, 'multiline', NULL, NULL,            NULL, NULL,      902, 'Дополнительно')
)
INSERT INTO meta.field_catalog (
    table_schema,
    table_name,
    column_name,
    display_name,
    unit,
    editor_kind,
    lookup_schema,
    lookup_table,
    lookup_value_column,
    lookup_label_column,
    display_order,
    group_name
)
SELECT
    'net',
    layer.table_name,
    specs.column_name,
    specs.display_name,
    specs.unit,
    specs.editor_kind,
    specs.lookup_schema,
    specs.lookup_table,
    specs.lookup_value_column,
    specs.lookup_label_column,
    specs.display_order,
    specs.group_name
FROM specs
JOIN meta.layer_catalog layer
  ON layer.schema_name = 'net'
JOIN information_schema.columns column_info
  ON column_info.table_schema = layer.schema_name
 AND column_info.table_name = layer.table_name
 AND column_info.column_name = specs.column_name
ON CONFLICT (table_schema, table_name, column_name) DO NOTHING;

INSERT INTO meta.schema_version (version, description)
VALUES (7, 'Метаданные карточек и справочные редакторы Qt')
ON CONFLICT (version) DO NOTHING;

COMMIT;
