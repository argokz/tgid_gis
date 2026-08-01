-- Эквивалентность старого onPotNagr0 и отчёта из единой схемы net.

\set ON_ERROR_STOP on

BEGIN;

DO $$
DECLARE
    old_count integer;
    new_count integer;
BEGIN
    WITH old_rows AS (
        SELECT node.id, code.name AS code_name,
               node.externalnodename, consumer.name
          FROM public.nodes node
          JOIN externalcodes code ON code.id = node.externalcodeid
          JOIN (
              SELECT item.nodeid, item.consumerstateid, item.name
                FROM public.generalizedconsumers item
               WHERE coalesce(item.calchldep, 0) = 0
                 AND coalesce(item.calchlindep, 0) = 0
                 AND coalesce(item.calchlparall, 0) = 0
                 AND coalesce(item.calchlmix, 0) = 0
                 AND coalesce(item.calchlconseq, 0) = 0
                 AND coalesce(item.calchlpreon, 0) = 0
              UNION
              SELECT item.nodeid, item.consumerstateid, item.name
                FROM public.realconsumers item
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
              FROM public.nodes node
              JOIN externalcodes code ON code.id = node.externalcodeid
              JOIN (
                  SELECT item.nodeid, item.consumerstateid, item.name
                    FROM public.generalizedconsumers item
                   WHERE coalesce(item.calchldep, 0) = 0
                     AND coalesce(item.calchlindep, 0) = 0
                     AND coalesce(item.calchlparall, 0) = 0
                     AND coalesce(item.calchlmix, 0) = 0
                     AND coalesce(item.calchlconseq, 0) = 0
                     AND coalesce(item.calchlpreon, 0) = 0
                  UNION
                  SELECT item.nodeid, item.consumerstateid, item.name
                    FROM public.realconsumers item
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
