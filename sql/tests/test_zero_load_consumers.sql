-- Эквивалентность старого onPotNagr0 и отчёта из единой схемы net.

\set ON_ERROR_STOP on

BEGIN;
    -- Сверка идёт с замороженным снимком дореформенной модели в attic.
    -- Объекты, заведённые в программе уже ПОСЛЕ переноса, в снимке
    -- отсутствовать обязаны, поэтому из новой стороны их исключаем:
    -- признак прямой: узел присутствует в самом снимке. Столбец
    -- id_old для этого не годится — он нулевой у 91 491 узла из
    -- 91 532, то есть перенесённые от новых по нему не отличить.
    -- Без этого тест начинает падать при первой же реальной работе
    -- пользователя: так и вышло 3 августа, когда через Tgid-07 завели
    -- потребителя 605612 (журнал meta.object_change_log, 20:44).


DO $$
DECLARE
    old_count integer;
    new_count integer;
BEGIN
    WITH old_rows AS (
        SELECT node.id, code.name AS code_name,
               node.externalnodename, consumer.name
          FROM attic.nodes_legacy node
          JOIN externalcodes code ON code.id = node.externalcodeid
          JOIN (
              SELECT item.nodeid, item.consumerstateid, item.name
                FROM attic.generalizedconsumers_legacy item
               WHERE coalesce(item.calchldep, 0) = 0
                 AND coalesce(item.calchlindep, 0) = 0
                 AND coalesce(item.calchlparall, 0) = 0
                 AND coalesce(item.calchlmix, 0) = 0
                 AND coalesce(item.calchlconseq, 0) = 0
                 AND coalesce(item.calchlpreon, 0) = 0
              UNION
              SELECT item.nodeid, item.consumerstateid, item.name
                FROM attic.realconsumers_legacy item
               WHERE coalesce(item.calchldep, 0) = 0
                 AND coalesce(item.calchlindep, 0) = 0
          ) consumer ON consumer.nodeid = node.id
         WHERE node.removed = 0
    ),
    new_rows AS (
        SELECT node.id, code.name AS code_name,
               node.externalnodename, consumer.name
          FROM net.v_nodes node
          JOIN ref.externalcodes code ON code.id = node.externalcodeid
          JOIN (
              SELECT item.nodeid, item.consumerstateid, item.name
                FROM net.v_generalizedconsumers item
               WHERE coalesce(item.calchldep, 0) = 0
                 AND coalesce(item.calchlindep, 0) = 0
                 AND coalesce(item.calchlparall, 0) = 0
                 AND coalesce(item.calchlmix, 0) = 0
                 AND coalesce(item.calchlconseq, 0) = 0
                 AND coalesce(item.calchlpreon, 0) = 0
              UNION
              SELECT item.nodeid, item.consumerstateid, item.name
                FROM net.v_realconsumers item
               WHERE coalesce(item.calchldep, 0) = 0
                 AND coalesce(item.calchlindep, 0) = 0
          ) consumer ON consumer.nodeid = node.id
         WHERE node.removed = 0
               AND EXISTS (SELECT 1 FROM attic.nodes_legacy o WHERE o.id = node.id)
    )
    SELECT (SELECT count(*) FROM old_rows),
           (SELECT count(*) FROM new_rows)
      INTO old_count, new_count;

    IF old_count = 0 OR old_count <> new_count THEN
        RAISE EXCEPTION
            'Количество onPotNagr0 отличается: old %, new %',
            old_count, new_count;
    END IF;

    IF EXISTS (
        WITH old_rows AS (
            SELECT node.id, code.name AS code_name,
                   node.externalnodename, consumer.name
              FROM attic.nodes_legacy node
              JOIN externalcodes code ON code.id = node.externalcodeid
              JOIN (
                  SELECT item.nodeid, item.consumerstateid, item.name
                    FROM attic.generalizedconsumers_legacy item
                   WHERE coalesce(item.calchldep, 0) = 0
                     AND coalesce(item.calchlindep, 0) = 0
                     AND coalesce(item.calchlparall, 0) = 0
                     AND coalesce(item.calchlmix, 0) = 0
                     AND coalesce(item.calchlconseq, 0) = 0
                     AND coalesce(item.calchlpreon, 0) = 0
                  UNION
                  SELECT item.nodeid, item.consumerstateid, item.name
                    FROM attic.realconsumers_legacy item
                   WHERE coalesce(item.calchldep, 0) = 0
                     AND coalesce(item.calchlindep, 0) = 0
              ) consumer ON consumer.nodeid = node.id
             WHERE node.removed = 0
        ),
        new_rows AS (
            SELECT node.id, code.name AS code_name,
                   node.externalnodename, consumer.name
              FROM net.v_nodes node
              JOIN ref.externalcodes code ON code.id = node.externalcodeid
              JOIN (
                  SELECT item.nodeid, item.consumerstateid, item.name
                    FROM net.v_generalizedconsumers item
                   WHERE coalesce(item.calchldep, 0) = 0
                     AND coalesce(item.calchlindep, 0) = 0
                     AND coalesce(item.calchlparall, 0) = 0
                     AND coalesce(item.calchlmix, 0) = 0
                     AND coalesce(item.calchlconseq, 0) = 0
                     AND coalesce(item.calchlpreon, 0) = 0
                  UNION
                  SELECT item.nodeid, item.consumerstateid, item.name
                    FROM net.v_realconsumers item
                   WHERE coalesce(item.calchldep, 0) = 0
                     AND coalesce(item.calchlindep, 0) = 0
              ) consumer ON consumer.nodeid = node.id
             WHERE node.removed = 0
               AND EXISTS (SELECT 1 FROM attic.nodes_legacy o WHERE o.id = node.id)
        )
        (SELECT * FROM old_rows EXCEPT SELECT * FROM new_rows)
        UNION ALL
        (SELECT * FROM new_rows EXCEPT SELECT * FROM old_rows)
    ) THEN
        RAISE EXCEPTION 'Строки нового onPotNagr0 отличаются от старого запроса';
    END IF;

    RAISE NOTICE 'onPotNagr0: % строк, точное совпадение', new_count;
END
$$;

ROLLBACK;
