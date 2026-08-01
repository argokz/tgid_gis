-- Инварианты GIS после sql/170_gis_geometry_cleanup.sql и обновления каталога.
\set ON_ERROR_STOP on

DO $$
DECLARE
    problem_count integer;
    missing_count integer;
    layer record;
BEGIN
    SELECT count(*) INTO problem_count
    FROM geometry_columns g
    JOIN pg_class c
      ON c.oid = format('%I.%I', g.f_table_schema, g.f_table_name)::regclass
    WHERE g.f_table_schema = 'net'
      AND c.relkind IN ('r', 'p')
      AND upper(g.type) = 'GEOMETRY';
    IF problem_count <> 0 THEN
        RAISE EXCEPTION 'Остались generic geometry-слои: %', problem_count;
    END IF;

    SELECT count(*) INTO problem_count
    FROM geometry_columns g
    JOIN pg_class c
      ON c.oid = format('%I.%I', g.f_table_schema, g.f_table_name)::regclass
    WHERE g.f_table_schema = 'net'
      AND c.relkind IN ('r', 'p')
      AND NOT EXISTS (
          SELECT 1
          FROM pg_index i
          JOIN pg_class ic ON ic.oid = i.indexrelid
          JOIN pg_am am ON am.oid = ic.relam
          JOIN pg_attribute a
            ON a.attrelid = i.indrelid
           AND a.attnum = ANY(i.indkey)
          WHERE i.indrelid = c.oid
            AND am.amname = 'gist'
            AND a.attname = g.f_geometry_column);
    IF problem_count <> 0 THEN
        RAISE EXCEPTION 'Слои без GiST: %', problem_count;
    END IF;

    SELECT count(*) INTO missing_count
    FROM (VALUES
        ('building_consumer_point'), ('building_consumer_line'),
        ('building_consumer_area'), ('building_tu_line'),
        ('building_tu_area'), ('pipe_reconstructed_point'),
        ('pipe_reconstructed_line')) AS required(table_name)
    WHERE to_regclass(format('net.%I', table_name)) IS NULL;
    IF missing_count <> 0 THEN
        RAISE EXCEPTION 'Не созданы раздельные GIS-слои: %', missing_count;
    END IF;

    IF to_regclass('net.building_consumer') IS NOT NULL
       OR to_regclass('net.building_tu') IS NOT NULL
       OR to_regclass('net.pipe_reconstructed') IS NOT NULL THEN
        RAISE EXCEPTION 'Смешанные переходные таблицы остались в net';
    END IF;

    FOR layer IN
        SELECT g.f_table_name, g.f_geometry_column
        FROM geometry_columns g
        JOIN pg_class c ON c.oid =
            format('%I.%I', g.f_table_schema, g.f_table_name)::regclass
        WHERE g.f_table_schema = 'net'
          AND c.relkind IN ('r', 'p')
    LOOP
        EXECUTE format(
            'SELECT count(*) FROM net.%I WHERE NOT ST_IsValid(%I)',
            layer.f_table_name, layer.f_geometry_column)
        INTO problem_count;
        IF problem_count <> 0 THEN
            RAISE EXCEPTION 'Невалидная геометрия в net.%: %',
                layer.f_table_name, problem_count;
        END IF;
    END LOOP;

    IF EXISTS (
           SELECT 1
           FROM geometry_columns g
           JOIN pg_class c ON c.oid =
               format('%I.%I', g.f_table_schema, g.f_table_name)::regclass
           WHERE g.f_table_schema = 'net'
             AND c.relkind IN ('r', 'p')
             AND EXISTS (
                 SELECT 1
                 FROM pg_catalog.pg_attribute a
                 WHERE a.attrelid = c.oid
                   AND a.attname = g.f_geometry_column
                   AND NOT a.attnotnull)) THEN
        RAISE EXCEPTION 'GIS-инвариант NOT NULL нарушен';
    END IF;

    SELECT count(*) INTO missing_count
    FROM geometry_columns g
    JOIN pg_class c
      ON c.oid = format('%I.%I', g.f_table_schema, g.f_table_name)::regclass
    LEFT JOIN meta.layer_catalog l
      ON l.schema_name = g.f_table_schema
     AND l.table_name = g.f_table_name
    WHERE g.f_table_schema = 'net'
      AND c.relkind IN ('r', 'p')
      AND l.id IS NULL;
    IF missing_count <> 0 THEN
        RAISE EXCEPTION 'GIS-слои отсутствуют в meta.layer_catalog: %', missing_count;
    END IF;
END
$$;

SELECT 'test_gis_geometry_cleanup: ok' AS result;
