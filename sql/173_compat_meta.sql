-- Журнал изменений для gid8: audit_log и changed_object.
--
-- Обе таблицы переехали в схему meta (145_legacy_audit.sql), а meta не
-- входила ни в один порядок поиска. Приложение падало на первом же
-- запросе после подключения:
--
--   ОШИБКА: отношение "audit_log" не существует
--   select max(log_id) as id from audit_log
--
-- Схема meta добавлена в search_path базы. Но gid8 задаёт свой порядок
-- поиска при подключении, и до пересборки приложения meta он не увидит.
-- Поэтому здесь ещё и представления в compat: они разрешают имя сразу,
-- без пересборки.
--
-- Представление над одной таблицей PostgreSQL обновляет автоматически,
-- поэтому запись в журнал из gid8 продолжает работать.
--
-- Когда приложение пересоберут с meta в списке схем, представления
-- станут лишними, но вреда не принесут: они указывают на те же таблицы.

BEGIN;

ALTER DATABASE tgid_gis SET search_path =
    public, net, ref, calc, meta, addr, doc, el, ops, org;

CREATE SCHEMA IF NOT EXISTS compat;

CREATE OR REPLACE VIEW compat.audit_log AS
    SELECT * FROM meta.audit_log;

CREATE OR REPLACE VIEW compat.changed_object AS
    SELECT * FROM meta.changed_object;

COMMENT ON VIEW compat.audit_log IS
    'Журнал изменений для gid8. Данные в meta.audit_log; представление '
    'обновляемо, запись из приложения работает.';

COMMENT ON VIEW compat.changed_object IS
    'Изменённые объекты для gid8. Данные в meta.changed_object.';

COMMIT;

SELECT (SELECT count(*) FROM compat.audit_log)      AS audit_log,
       (SELECT count(*) FROM compat.changed_object) AS changed_object;
