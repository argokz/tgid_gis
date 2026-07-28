-- ПЕРЕКЛЮЧЕНИЕ приложения на схему net. Обратимо (см. 041_rollback.sql).
--
-- Применять только после того, как прошли:
--     python converter/validate.py   — 19 проверок
--     python tools/check_compat.py   — 0 неожиданных расхождений
--
-- Что делает: прячет исходные таблицы под именами *_legacy и ставит на их
-- место представления из схемы net. Данные не удаляются и не копируются —
-- только переименование, поэтому шаг быстрый и обратимый.
--
-- ВНИМАНИЕ: после этого шага public.nodes и public.linesobj становятся
-- ТОЛЬКО ДЛЯ ЧТЕНИЯ. Запись из приложения работать не будет, пока не
-- сделан шаг с INSTEAD OF триггерами (см. docs/04-compat-layer.md).
-- Для режима «только просмотр и расчёт» этого достаточно.

\set ON_ERROR_STOP on

BEGIN;

-- Страховка: не переключаться, если конвертация не выполнялась.
DO $$
DECLARE n bigint;
BEGIN
    SELECT count(*) INTO n FROM net.node_reg;
    IF n = 0 THEN
        RAISE EXCEPTION 'схема net пуста — сначала выполните конвертер';
    END IF;
END $$;

ALTER TABLE public.nodes    RENAME TO nodes_legacy;
ALTER TABLE public.linesobj RENAME TO linesobj_legacy;

CREATE VIEW public.nodes    AS SELECT * FROM net.v_nodes;
CREATE VIEW public.linesobj AS SELECT * FROM net.v_linesobj;

COMMIT;

\echo 'Приложение переключено на net. Откат: sql/041_rollback_to_public.sql'
