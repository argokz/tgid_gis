-- Справочник внешних кодов для чистой продуктовой БД и отчёта aZap6.

\set ON_ERROR_STOP on

BEGIN;

CREATE SCHEMA IF NOT EXISTS ref;

DO $$
DECLARE
    max_id bigint;
    layer record;
BEGIN
    IF to_regclass('public.externalcodes') IS NULL THEN
        RAISE EXCEPTION
            'Для первичного переноса требуется public.externalcodes';
    END IF;

    DROP TABLE IF EXISTS ref.externalcodes CASCADE;
    CREATE TABLE ref.externalcodes (
        LIKE public.externalcodes
        INCLUDING CONSTRAINTS
        INCLUDING INDEXES
        INCLUDING STORAGE
        INCLUDING COMMENTS
    );
    INSERT INTO ref.externalcodes
    SELECT * FROM public.externalcodes;

    DROP SEQUENCE IF EXISTS ref.externalcodes_id_seq CASCADE;
    CREATE SEQUENCE ref.externalcodes_id_seq;
    SELECT max(id) INTO max_id FROM ref.externalcodes;
    IF max_id IS NOT NULL THEN
        PERFORM setval('ref.externalcodes_id_seq', max_id, true);
    END IF;
    ALTER TABLE ref.externalcodes
        ALTER COLUMN id
        SET DEFAULT nextval('ref.externalcodes_id_seq'::regclass);
    ALTER SEQUENCE ref.externalcodes_id_seq
        OWNED BY ref.externalcodes.id;

    FOR layer IN
        SELECT DISTINCT catalog.table_name
          FROM meta.layer_catalog AS catalog
          JOIN information_schema.columns AS column_info
            ON column_info.table_schema = catalog.schema_name
           AND column_info.table_name = catalog.table_name
           AND column_info.column_name = 'externalcodeid'
         WHERE catalog.schema_name = 'net'
         ORDER BY catalog.table_name
    LOOP
        EXECUTE format(
            'ALTER TABLE net.%1$I '
            'DROP CONSTRAINT IF EXISTS %2$I',
            layer.table_name,
            layer.table_name || '_externalcode_fk');
        EXECUTE format(
            'ALTER TABLE net.%1$I '
            'ADD CONSTRAINT %2$I FOREIGN KEY (externalcodeid) '
            'REFERENCES ref.externalcodes(id) ON UPDATE RESTRICT '
            'ON DELETE RESTRICT NOT VALID',
            layer.table_name,
            layer.table_name || '_externalcode_fk');
        EXECUTE format(
            'ALTER TABLE net.%1$I VALIDATE CONSTRAINT %2$I',
            layer.table_name,
            layer.table_name || '_externalcode_fk');
    END LOOP;
END
$$;

UPDATE meta.field_catalog
   SET display_name = 'Внешний код',
       editor_kind = 'lookup',
       lookup_schema = 'ref',
       lookup_table = 'externalcodes',
       lookup_value_column = 'id',
       lookup_label_column = 'name',
       display_order = 29,
       group_name = 'Принадлежность'
 WHERE table_schema = 'net'
   AND column_name = 'externalcodeid';

INSERT INTO meta.schema_version (version, description)
VALUES (13, 'Справочник внешних кодов и отчёт закрытых потребителей')
ON CONFLICT (version) DO NOTHING;

COMMIT;
