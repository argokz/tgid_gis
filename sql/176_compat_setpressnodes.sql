-- Последняя настоящая таблица ТГИД вне продуктовой модели: setpressnodes.
--
-- До этого шага public.setpressnodes оставалась обычной таблицей, а не
-- представлением. Инвентаризация tools/db_consumers.py показала, что
-- её используют шесть потребителей сразу: gid8 (11 обращений),
-- SQL-запросы gid6 (13), движок расчёта sety (2), скрипты потерь potr5
-- (2) — и все они разрешаются в public, то есть работали бы даже без
-- слоя net, минуя перенесённые данные.
--
-- Класс net.node_press_setting уже существует и заполнен: 5733 строки
-- из 5740 связаны через src_id (совпадает с исходным id), node_id
-- совпадает с исходным nodeid у всех 5733 строк без исключений —
-- проверено перед переносом, не предположено. Семь без пары — те же
-- узлы МТК13-8, что описаны в docs/14: ссылаются на узлы без координат,
-- которые конвертер не переносит по правилу, а не по ошибке.
--
-- Представление простое (один SELECT из одной таблицы, без агрегатов),
-- поэтому PostgreSQL делает его автообновляемым сам — INSTEAD OF
-- триггеры не нужны. Проверено в транзакции с откатом перед этим
-- шагом: UPDATE через представление доходит до net.node_press_setting.
--
-- Единственная запись, которую делает сам gid8 (gidrSlot.cpp), —
-- UPDATE fragment_resultID. INSERT в исходниках приложения не найден,
-- только в разовом скрипте миграции full_tgid.sql.

BEGIN;

CREATE SCHEMA IF NOT EXISTS compat;

CREATE OR REPLACE VIEW compat.setpressnodes AS
SELECT
    o.src_id  AS id,
    o.node_id AS nodeid,
    o.pressflow,
    o.pressret,
    o.fragment_resultid,
    o.file_result,
    o.kod_m,
    o.uzel_m,
    o.fileid
FROM net.node_press_setting o
WHERE o.src_id IS NOT NULL;

COMMENT ON VIEW compat.setpressnodes IS
    'Узлы с заданным напором для gid8. Данные в net.node_press_setting; '
    'представление обновляемо PostgreSQL автоматически.';

COMMIT;

SELECT count(*) AS strok FROM compat.setpressnodes;
