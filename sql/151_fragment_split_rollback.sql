-- Откат 150_fragment_split.sql.
--
-- Дочерние таблицы удаляются целиком. Добавленные скалярные поля
-- net.fragment НЕ удаляются: их значения перенесены из
-- public.fragments и там же остались, а DROP COLUMN необратим —
-- если после отката окажется, что поля кому-то нужны, восстановить
-- их будет неоткуда. Пустая колонка вреда не приносит.

BEGIN;

DROP TABLE IF EXISTS net.fragment_system;
DROP TABLE IF EXISTS net.fragment_month;

COMMIT;
