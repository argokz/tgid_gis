-- Базовые справочники продуктовой БД.
--
-- Скрипт выполняется в сборочной/переходной БД, пока исходная схема public
-- ещё доступна. В продуктовый дамп затем попадают только net/ref/meta.
-- Повторный запуск полностью пересобирает выбранные справочники.

BEGIN;

CREATE SCHEMA IF NOT EXISTS ref;

DO $$
DECLARE
    r record;
    seq_name text;
    max_id bigint;
BEGIN
    FOR r IN
        SELECT c.relname
        FROM pg_class c
        JOIN pg_namespace n ON n.oid = c.relnamespace
        WHERE n.nspname = 'public'
          AND c.relkind = 'r'
          AND c.relname ~* '^(nodetypes|tubingtypes|isolmaterial|standard|seasons|states|organization)'
        ORDER BY c.relname
    LOOP
        EXECUTE format('DROP TABLE IF EXISTS ref.%I CASCADE', r.relname);
        EXECUTE format(
            'CREATE TABLE ref.%I (LIKE public.%I INCLUDING CONSTRAINTS INCLUDING INDEXES INCLUDING STORAGE INCLUDING COMMENTS)',
            r.relname,
            r.relname
        );
        EXECUTE format(
            'INSERT INTO ref.%I SELECT * FROM public.%I',
            r.relname,
            r.relname
        );

        IF EXISTS (
            SELECT 1
            FROM information_schema.columns
            WHERE table_schema = 'ref'
              AND table_name = r.relname
              AND column_name = 'id'
              AND data_type IN ('smallint', 'integer', 'bigint')
        ) THEN
            seq_name := r.relname || '_id_seq';
            EXECUTE format('DROP SEQUENCE IF EXISTS ref.%I CASCADE', seq_name);
            EXECUTE format('CREATE SEQUENCE ref.%I', seq_name);
            EXECUTE format(
                'SELECT max(id) FROM ref.%I',
                r.relname
            ) INTO max_id;
            IF max_id IS NOT NULL THEN
                PERFORM setval(
                    format('ref.%I', seq_name)::regclass,
                    max_id,
                    true
                );
            END IF;
            EXECUTE format(
                'ALTER TABLE ref.%I ALTER COLUMN id SET DEFAULT nextval(%L::regclass)',
                r.relname,
                format('ref.%I', seq_name)
            );
            EXECUTE format(
                'ALTER SEQUENCE ref.%I OWNED BY ref.%I.id',
                seq_name,
                r.relname
            );
        END IF;
    END LOOP;
END
$$;

COMMIT;
