-- Эквивалентность старого onUtZakrAll и списка из объектных таблиц.

\set ON_ERROR_STOP on

BEGIN;

DO $$
DECLARE
    old_count integer;
    new_count integer;
BEGIN
    CREATE TEMP TABLE old_disconnected_pipe_sections ON COMMIT DROP AS
    SELECT line.id, code_from.name AS code_from,
           node_from.externalnodename AS node_from,
           code_to.name AS code_to,
           node_to.externalnodename AS node_to,
           pipe.pipesectlength, pipe.diameterinternal
      FROM public.linesobj line
      JOIN public.heatpipesections pipe ON pipe.lineid = line.id
      JOIN public.nodes node_from
        ON node_from.id = line.nodeid1 AND node_from.removed = 0
      JOIN ref.externalcodes code_from
        ON code_from.id = node_from.externalcodeid
      JOIN public.nodes node_to
        ON node_to.id = line.nodeid2 AND node_to.removed = 0
      JOIN ref.externalcodes code_to
        ON code_to.id = node_to.externalcodeid
      LEFT JOIN calc.ut_out result ON result.lineid = line.id
      LEFT JOIN (
          SELECT calculation.fileid, max(calculation.id) AS calculation_id
            FROM calc.calculation calculation
           GROUP BY calculation.fileid
      ) latest
        ON latest.fileid = node_from.fileid
       AND latest.calculation_id = result.calculationid
     WHERE line.removed = 0
       AND node_from.internalnodeid IS NULL
       AND result.id IS NULL;

    CREATE TEMP TABLE new_disconnected_pipe_sections ON COMMIT DROP AS
    SELECT pipe.src_id AS id, code_from.name AS code_from,
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
       AND NOT EXISTS (
           SELECT 1
             FROM calc.ut_out result
            WHERE result.lineid = pipe.src_id
       );

    SELECT count(*) INTO old_count FROM old_disconnected_pipe_sections;
    SELECT count(*) INTO new_count FROM new_disconnected_pipe_sections;

    IF old_count = 0 OR old_count <> new_count THEN
        RAISE EXCEPTION
            'Количество onUtZakrAll отличается: old %, new %',
            old_count, new_count;
    END IF;

    IF EXISTS (
        (SELECT * FROM old_disconnected_pipe_sections
         EXCEPT SELECT * FROM new_disconnected_pipe_sections)
        UNION ALL
        (SELECT * FROM new_disconnected_pipe_sections
         EXCEPT SELECT * FROM old_disconnected_pipe_sections)
    ) THEN
        RAISE EXCEPTION
            'Строки нового onUtZakrAll отличаются от старого запроса';
    END IF;

    RAISE NOTICE 'onUtZakrAll: % строк, точное совпадение', new_count;
END
$$;

ROLLBACK;
