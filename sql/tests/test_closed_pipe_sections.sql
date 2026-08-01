-- Эквивалентность старого onUtZakr и списка из net.pipe_section.

\set ON_ERROR_STOP on

BEGIN;

DO $$
DECLARE
    old_count integer;
    new_count integer;
BEGIN
    WITH old_rows AS (
        SELECT line.id, code_from.name AS code_from,
               node_from.externalnodename AS node_from,
               code_to.name AS code_to,
               node_to.externalnodename AS node_to,
               pipe.pipesectlength, pipe.diameterinternal
          FROM attic.linesobj_legacy line
          JOIN attic.heatpipesections_legacy pipe ON pipe.lineid = line.id
          JOIN attic.nodes_legacy node_from
            ON node_from.id = line.nodeid1 AND node_from.removed = 0
          JOIN externalcodes code_from
            ON code_from.id = node_from.externalcodeid
          JOIN attic.nodes_legacy node_to
            ON node_to.id = line.nodeid2 AND node_to.removed = 0
          JOIN externalcodes code_to
            ON code_to.id = node_to.externalcodeid
         WHERE line.removed = 0
           AND node_from.internalnodeid IS NULL
           AND pipe.pipesectstateidflow = 2
           AND pipe.pipesectstateidret = 2
    ),
    new_rows AS (
        SELECT pipe.src_id, code_from.name AS code_from,
               node_from.externalnodename AS node_from,
               code_to.name AS code_to,
               node_to.externalnodename AS node_to,
               pipe.pipesectlength, pipe.diameterinternal
          FROM net.pipe_section pipe
          JOIN net.v_nodes node_from
            ON node_from.id = pipe.node_from AND node_from.removed = 0
          JOIN ref.externalcodes code_from
            ON code_from.id = node_from.externalcodeid
          JOIN net.v_nodes node_to
            ON node_to.id = pipe.node_to AND node_to.removed = 0
          JOIN ref.externalcodes code_to
            ON code_to.id = node_to.externalcodeid
         WHERE pipe.removed_at IS NULL
           AND node_from.internalnodeid IS NULL
           AND pipe.pipesectstateidflow = 2
           AND pipe.pipesectstateidret = 2
    )
    SELECT (SELECT count(*) FROM old_rows),
           (SELECT count(*) FROM new_rows)
      INTO old_count, new_count;

    IF old_count = 0 OR old_count <> new_count THEN
        RAISE EXCEPTION
            'Количество onUtZakr отличается: old %, new %',
            old_count, new_count;
    END IF;

    IF EXISTS (
        WITH old_rows AS (
            SELECT line.id, code_from.name AS code_from,
                   node_from.externalnodename AS node_from,
                   code_to.name AS code_to,
                   node_to.externalnodename AS node_to,
                   pipe.pipesectlength, pipe.diameterinternal
              FROM attic.linesobj_legacy line
              JOIN attic.heatpipesections_legacy pipe ON pipe.lineid = line.id
              JOIN attic.nodes_legacy node_from
                ON node_from.id = line.nodeid1 AND node_from.removed = 0
              JOIN externalcodes code_from
                ON code_from.id = node_from.externalcodeid
              JOIN attic.nodes_legacy node_to
                ON node_to.id = line.nodeid2 AND node_to.removed = 0
              JOIN externalcodes code_to
                ON code_to.id = node_to.externalcodeid
             WHERE line.removed = 0
               AND node_from.internalnodeid IS NULL
               AND pipe.pipesectstateidflow = 2
               AND pipe.pipesectstateidret = 2
        ),
        new_rows AS (
            SELECT pipe.src_id, code_from.name AS code_from,
                   node_from.externalnodename AS node_from,
                   code_to.name AS code_to,
                   node_to.externalnodename AS node_to,
                   pipe.pipesectlength, pipe.diameterinternal
              FROM net.pipe_section pipe
              JOIN net.v_nodes node_from
                ON node_from.id = pipe.node_from AND node_from.removed = 0
              JOIN ref.externalcodes code_from
                ON code_from.id = node_from.externalcodeid
              JOIN net.v_nodes node_to
                ON node_to.id = pipe.node_to AND node_to.removed = 0
              JOIN ref.externalcodes code_to
                ON code_to.id = node_to.externalcodeid
             WHERE pipe.removed_at IS NULL
               AND node_from.internalnodeid IS NULL
               AND pipe.pipesectstateidflow = 2
               AND pipe.pipesectstateidret = 2
        )
        (SELECT * FROM old_rows EXCEPT SELECT * FROM new_rows)
        UNION ALL
        (SELECT * FROM new_rows EXCEPT SELECT * FROM old_rows)
    ) THEN
        RAISE EXCEPTION 'Строки нового onUtZakr отличаются от старого запроса';
    END IF;

    RAISE NOTICE 'onUtZakr: % строк, точное совпадение', new_count;
END
$$;

ROLLBACK;
