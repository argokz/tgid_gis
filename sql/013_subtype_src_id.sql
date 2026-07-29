-- Сохранение id исходной строки подтипа.
--
-- В объектной таблице net.* колонка src_id хранит id УЗЛА, а приложение
-- читает ещё и id строки подтипа: getNodeQ отдаёт его как id2
-- (gidview/gidr_add.cpp:22). Чтобы слой совместимости мог вернуть то же
-- самое значение, исходный id подтипа нужно хранить отдельно.
--
-- Применяется к готовой схеме net, пересборка не требуется.

DO $$
DECLARE t text;
BEGIN
    FOR t IN
        SELECT c.relname
        FROM pg_class c JOIN pg_namespace n ON n.oid = c.relnamespace
        WHERE n.nspname = 'net' AND c.relkind = 'r'
          AND EXISTS (SELECT 1 FROM pg_attribute a
                      WHERE a.attrelid = c.oid AND a.attname = 'src_id'
                        AND a.attnum > 0 AND NOT a.attisdropped)
    LOOP
        EXECUTE format(
            'ALTER TABLE net.%I ADD COLUMN IF NOT EXISTS subtype_src_id int', t);
    END LOOP;
END $$;
