-- Таблицы эксплуатации должны разрешаться в search_path gid8 и принимать записи.
BEGIN;

SET LOCAL search_path = compat, public, net, ref, calc, meta, addr, doc, el, ops, org;

DO $$
DECLARE
    table_name text;
    missing text[] := ARRAY[]::text[];
    inserted_id integer;
BEGIN
    FOREACH table_name IN ARRAY ARRAY[
        'defect', 'opres', 'remont2', 'osmotr',
        'defecttube', 'shurfy', 'shurf', 'remont', 'remonttube',
        'zapornaya_armatura', 'vid_ispytani', 'responsibles'
    ]
    LOOP
        IF to_regclass(table_name) IS NULL THEN
            missing := array_append(missing, table_name);
        END IF;
    END LOOP;

    IF cardinality(missing) <> 0 THEN
        RAISE EXCEPTION 'не разрешаются таблицы эксплуатации gid8: %', missing;
    END IF;

    INSERT INTO defect DEFAULT VALUES RETURNING id INTO inserted_id;
    INSERT INTO opres DEFAULT VALUES RETURNING id INTO inserted_id;
    INSERT INTO remont2 DEFAULT VALUES RETURNING id INTO inserted_id;
    INSERT INTO osmotr DEFAULT VALUES RETURNING id INTO inserted_id;
    INSERT INTO defecttube DEFAULT VALUES RETURNING id INTO inserted_id;
    INSERT INTO shurfy DEFAULT VALUES RETURNING id INTO inserted_id;
    INSERT INTO shurf DEFAULT VALUES RETURNING id INTO inserted_id;
    INSERT INTO remont DEFAULT VALUES RETURNING id INTO inserted_id;
    INSERT INTO remonttube DEFAULT VALUES RETURNING id INTO inserted_id;
    INSERT INTO zapornaya_armatura DEFAULT VALUES RETURNING id INTO inserted_id;
    INSERT INTO vid_ispytani DEFAULT VALUES RETURNING id INTO inserted_id;
    INSERT INTO responsibles DEFAULT VALUES RETURNING id INTO inserted_id;

    RAISE NOTICE 'gid8 maintenance tables: lookup and INSERT OK';
END;
$$;

ROLLBACK;
