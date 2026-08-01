-- Проверка справочника externalcodes и эквивалентности запроса aZap6.

\set ON_ERROR_STOP on

BEGIN;

DO $$
DECLARE
    missing_lookup integer;
    missing_fk integer;
    old_count integer;
    new_count integer;
    source_code_count integer;
BEGIN
    IF to_regclass('public.externalcodes') IS NOT NULL THEN
        EXECUTE 'SELECT count(*) FROM public.externalcodes'
           INTO source_code_count;
        IF (SELECT count(*) FROM ref.externalcodes) <> source_code_count THEN
            RAISE EXCEPTION 'Не все externalcodes перенесены в ref';
        END IF;
    END IF;

    SELECT count(*)
      INTO missing_lookup
      FROM meta.field_catalog field
     WHERE field.table_schema = 'net'
       AND field.column_name = 'externalcodeid'
       AND (field.editor_kind <> 'lookup'
            OR field.lookup_schema <> 'ref'
            OR field.lookup_table <> 'externalcodes'
            OR field.lookup_value_column <> 'id'
            OR field.lookup_label_column <> 'name');
    IF missing_lookup <> 0 THEN
        RAISE EXCEPTION 'Не настроены lookup для % полей', missing_lookup;
    END IF;

    SELECT count(*)
      INTO missing_fk
      FROM meta.layer_catalog layer
      JOIN information_schema.columns column_info
        ON column_info.table_schema = layer.schema_name
       AND column_info.table_name = layer.table_name
       AND column_info.column_name = 'externalcodeid'
     WHERE layer.schema_name = 'net'
       AND NOT EXISTS (
           SELECT 1
             FROM pg_constraint constraint_info
            WHERE constraint_info.conrelid =
                      format('net.%I', layer.table_name)::regclass
              AND constraint_info.contype = 'f'
              AND constraint_info.confrelid =
                      'ref.externalcodes'::regclass);
    IF missing_fk <> 0 THEN
        RAISE EXCEPTION 'Нет FK externalcodeid у % классов', missing_fk;
    END IF;

    WITH old_rows AS (
        SELECT node.id, code.name AS code_name,
               node.externalnodename, consumer.name
          FROM attic.nodes_legacy node
          JOIN externalcodes code ON code.id = node.externalcodeid
          JOIN (
              SELECT nodeid, name
                FROM attic.generalizedconsumers_legacy
               WHERE consumerstateid = 2
              UNION
              SELECT nodeid, name
                FROM attic.realconsumers_legacy
               WHERE consumerstateid = 2
          ) consumer ON consumer.nodeid = node.id
         WHERE node.removed = 0
    ),
    new_rows AS (
        SELECT consumer.src_id AS id, code.name AS code_name,
               consumer.externalnodename, consumer.name
          FROM net.consumer_general consumer
          JOIN ref.externalcodes code ON code.id = consumer.externalcodeid
         WHERE consumer.removed_at IS NULL
           AND consumer.consumerstateid = 2
        UNION
        SELECT consumer.src_id, code.name,
               consumer.externalnodename, consumer.name
          FROM net.consumer_real consumer
          JOIN ref.externalcodes code ON code.id = consumer.externalcodeid
         WHERE consumer.removed_at IS NULL
           AND consumer.consumerstateid = 2
    )
    SELECT (SELECT count(*) FROM old_rows),
           (SELECT count(*) FROM new_rows)
      INTO old_count, new_count;
    IF old_count <> new_count THEN
        RAISE EXCEPTION
            'Количество aZap6 отличается: old %, new %',
            old_count, new_count;
    END IF;

    IF EXISTS (
        WITH old_rows AS (
            SELECT node.id, code.name AS code_name,
                   node.externalnodename, consumer.name
              FROM attic.nodes_legacy node
              JOIN externalcodes code
                ON code.id = node.externalcodeid
              JOIN (
                  SELECT nodeid, name
                    FROM attic.generalizedconsumers_legacy
                   WHERE consumerstateid = 2
                  UNION
                  SELECT nodeid, name
                    FROM attic.realconsumers_legacy
                   WHERE consumerstateid = 2
              ) consumer ON consumer.nodeid = node.id
             WHERE node.removed = 0
        ),
        new_rows AS (
            SELECT consumer.src_id AS id, code.name AS code_name,
                   consumer.externalnodename, consumer.name
              FROM net.consumer_general consumer
              JOIN ref.externalcodes code
                ON code.id = consumer.externalcodeid
             WHERE consumer.removed_at IS NULL
               AND consumer.consumerstateid = 2
            UNION
            SELECT consumer.src_id, code.name,
                   consumer.externalnodename, consumer.name
              FROM net.consumer_real consumer
              JOIN ref.externalcodes code
                ON code.id = consumer.externalcodeid
             WHERE consumer.removed_at IS NULL
               AND consumer.consumerstateid = 2
        )
        (SELECT * FROM old_rows EXCEPT SELECT * FROM new_rows)
        UNION ALL
        (SELECT * FROM new_rows EXCEPT SELECT * FROM old_rows)
    ) THEN
        RAISE EXCEPTION 'Строки нового aZap6 отличаются от старого запроса';
    END IF;
END
$$;

ROLLBACK;
