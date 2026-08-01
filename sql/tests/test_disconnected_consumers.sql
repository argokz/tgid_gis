-- Эквивалентность старого onPotrOtkl и отчёта из net/calc/ref.

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
          FROM attic.nodes_legacy node
          JOIN externalcodes code ON code.id = node.externalcodeid
          JOIN (
              SELECT item.nodeid, item.consumerstateid, item.name
                FROM attic.generalizedconsumers_legacy item
              UNION
              SELECT item.nodeid, item.consumerstateid, item.name
                FROM attic.realconsumers_legacy item
          ) consumer ON consumer.nodeid = node.id
          LEFT JOIN (
              SELECT calculated_node.id
                FROM attic.nodes_legacy calculated_node
                JOIN calc.pt_out output
                  ON output.nodeid = calculated_node.id
                JOIN (
                    SELECT calculation.fileid,
                           max(calculation.id) AS calculation_id
                      FROM calc.calculation calculation
                     GROUP BY calculation.fileid
                ) latest
                  ON latest.fileid = calculated_node.fileid
                 AND latest.calculation_id = output.calculationid
               WHERE calculated_node.removed = 0
          ) calculated ON calculated.id = node.id
         WHERE calculated.id IS NULL AND node.removed = 0
    ),
    new_rows AS (
        SELECT node.id, code.name AS code_name,
               node.externalnodename, consumer.name
          FROM net.v_nodes node
          JOIN ref.externalcodes code ON code.id = node.externalcodeid
          JOIN (
              SELECT item.nodeid, item.consumerstateid, item.name
                FROM net.v_generalizedconsumers item
              UNION
              SELECT item.nodeid, item.consumerstateid, item.name
                FROM net.v_realconsumers item
          ) consumer ON consumer.nodeid = node.id
          LEFT JOIN (
              SELECT calculated_node.id
                FROM net.v_nodes calculated_node
                JOIN calc.pt_out output
                  ON output.nodeid = calculated_node.id
                JOIN (
                    SELECT calculation.fileid,
                           max(calculation.id) AS calculation_id
                      FROM calc.calculation calculation
                     GROUP BY calculation.fileid
                ) latest
                  ON latest.fileid = calculated_node.fileid
                 AND latest.calculation_id = output.calculationid
               WHERE calculated_node.removed = 0
          ) calculated ON calculated.id = node.id
         WHERE calculated.id IS NULL AND node.removed = 0
    )
    SELECT (SELECT count(*) FROM old_rows),
           (SELECT count(*) FROM new_rows)
      INTO old_count, new_count;

    IF old_count = 0 OR old_count <> new_count THEN
        RAISE EXCEPTION
            'Количество onPotrOtkl отличается: old %, new %',
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
                  UNION
                  SELECT item.nodeid, item.consumerstateid, item.name
                    FROM attic.realconsumers_legacy item
              ) consumer ON consumer.nodeid = node.id
              LEFT JOIN (
                  SELECT calculated_node.id
                    FROM attic.nodes_legacy calculated_node
                    JOIN calc.pt_out output
                      ON output.nodeid = calculated_node.id
                    JOIN (
                        SELECT calculation.fileid,
                               max(calculation.id) AS calculation_id
                          FROM calc.calculation calculation
                         GROUP BY calculation.fileid
                    ) latest
                      ON latest.fileid = calculated_node.fileid
                     AND latest.calculation_id = output.calculationid
                   WHERE calculated_node.removed = 0
              ) calculated ON calculated.id = node.id
             WHERE calculated.id IS NULL AND node.removed = 0
        ),
        new_rows AS (
            SELECT node.id, code.name AS code_name,
                   node.externalnodename, consumer.name
              FROM net.v_nodes node
              JOIN ref.externalcodes code ON code.id = node.externalcodeid
              JOIN (
                  SELECT item.nodeid, item.consumerstateid, item.name
                    FROM net.v_generalizedconsumers item
                  UNION
                  SELECT item.nodeid, item.consumerstateid, item.name
                    FROM net.v_realconsumers item
              ) consumer ON consumer.nodeid = node.id
              LEFT JOIN (
                  SELECT calculated_node.id
                    FROM net.v_nodes calculated_node
                    JOIN calc.pt_out output
                      ON output.nodeid = calculated_node.id
                    JOIN (
                        SELECT calculation.fileid,
                               max(calculation.id) AS calculation_id
                          FROM calc.calculation calculation
                         GROUP BY calculation.fileid
                    ) latest
                      ON latest.fileid = calculated_node.fileid
                     AND latest.calculation_id = output.calculationid
                   WHERE calculated_node.removed = 0
              ) calculated ON calculated.id = node.id
             WHERE calculated.id IS NULL AND node.removed = 0
        )
        (SELECT * FROM old_rows EXCEPT SELECT * FROM new_rows)
        UNION ALL
        (SELECT * FROM new_rows EXCEPT SELECT * FROM old_rows)
    ) THEN
        RAISE EXCEPTION 'Строки нового onPotrOtkl отличаются от старого запроса';
    END IF;

    RAISE NOTICE 'onPotrOtkl: % строк, точное совпадение', new_count;
END
$$;

ROLLBACK;
