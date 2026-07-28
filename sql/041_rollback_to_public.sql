-- ОТКАТ переключения: возвращает приложение на исходные таблицы public.
--
-- Данные в public всё это время оставались нетронутыми, поэтому откат
-- сводится к обратному переименованию и выполняется мгновенно.
--
-- Изменения, внесённые в net после переключения, в public НЕ попадут —
-- это осознанное свойство: откат возвращает состояние до переключения.

\set ON_ERROR_STOP on

BEGIN;

DROP VIEW IF EXISTS public.nodes;
DROP VIEW IF EXISTS public.linesobj;

ALTER TABLE public.nodes_legacy    RENAME TO nodes;
ALTER TABLE public.linesobj_legacy RENAME TO linesobj;

COMMIT;

\echo 'Откат выполнен: приложение снова работает с public.nodes / public.linesobj'
