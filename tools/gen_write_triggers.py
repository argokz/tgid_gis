"""Генерация INSTEAD OF триггеров для net.v_nodes и net.v_linesobj.

Без них представления доступны только для чтения, и приложение не может
ни переместить объект, ни удалить его.

Маршрутизация: реестр net.node_reg / net.line_reg хранит имя физической
таблицы объекта, поэтому UPDATE и DELETE адресуются динамически.

INSERT кладёт объект в net.node_plain / net.line_plain: в момент вставки
тип ещё не известен — прежняя модель сначала создавала строку в nodes,
и только потом строку в таблице-подтипе.
"""
import argparse
import json
import os

import psycopg2

# Колонки, которые нельзя писать напрямую: они производные либо служебные
SKIP = {'id', 'x', 'y', 'shape', 'coords', 'removed', 'idremoved',
        'globalid', 'gistable', 'sync', 'gis', 'sync_tgid', 'id_old',
        'fileid', 'nodeid1', 'nodeid2'}

HEADER = """-- СГЕНЕРИРОВАНО tools/gen_write_triggers.py
--
-- INSTEAD OF триггеры делают слой совместимости доступным для записи.
-- Приложение продолжает выполнять привычные UPDATE nodes SET x=..., y=...
-- и UPDATE nodes SET removed=1, а строки уходят в объектные таблицы net.
--
-- Геометрия пересчитывается из x/y при каждой записи, поэтому она остаётся
-- единственным источником истины: рассинхронизации, как между прежними
-- coords и shape, возникнуть не может.
"""


def gen(cur, view, reg, plain, base_table, is_line):
    cur.execute("""SELECT column_name FROM information_schema.columns
                   WHERE table_schema = 'public' AND table_name = %s
                   ORDER BY ordinal_position""", (base_table,))
    cols = [r[0] for r in cur.fetchall() if r[0] not in SKIP]

    # $1 — id, $2 — removed_at, $3 — геометрия, дальше идут колонки
    sets = ',\n                '.join(
        "%s = $%d" % (c, i + 4) for i, c in enumerate(cols))
    using = ',\n            '.join('NEW.%s' % c for c in cols)

    geom_new = ("ST_SetSRID(ST_MakeLine("
                "ARRAY[ST_SetSRID(ST_Point(0,0),9998)]), 9998)"
                if is_line else
                "ST_SetSRID(ST_Point(NEW.x / 100.0, -NEW.y / 100.0), 9998)")

    upd_geom = ('geom = net.rebuild_line_geom(v.id, NEW.coords)'
                if is_line else 'geom = %s' % geom_new)

    parts = []

    # ---- UPDATE ----
    parts.append("""
CREATE OR REPLACE FUNCTION net.{view}_update() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
DECLARE
    t text;
    g geometry;
BEGIN
    SELECT tbl INTO t FROM net.{reg} WHERE id = OLD.id;
    IF t IS NULL THEN
        RAISE EXCEPTION 'объект % не найден в реестре {reg}', OLD.id;
    END IF;

    -- Геометрию считаем ЗДЕСЬ и передаём параметром: внутри динамического
    -- запроса запись NEW недоступна, там видны только $1, $2, ...
    g := {geomexpr};

    EXECUTE format(
        'UPDATE net.%I SET
                geom = $3,
                removed_at = $2,
                {sets}
         WHERE id = $1', t)
    USING OLD.id,
          CASE WHEN NEW.removed <> 0 THEN now() ELSE NULL END,
          g,
          {using};

    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS {view}_upd ON net.{view};
CREATE TRIGGER {view}_upd INSTEAD OF UPDATE ON net.{view}
    FOR EACH ROW EXECUTE FUNCTION net.{view}_update();
""".format(view=view, reg=reg, sets=sets, using=using,
           geomexpr=('net.rebuild_line_geom(OLD.id, NEW.coords)' if is_line
                     else geom_new)))

    # ---- DELETE ----
    parts.append("""
CREATE OR REPLACE FUNCTION net.{view}_delete() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
DECLARE
    t text;
BEGIN
    SELECT tbl INTO t FROM net.{reg} WHERE id = OLD.id;
    IF t IS NULL THEN
        RETURN OLD;
    END IF;
    EXECUTE format('DELETE FROM net.%I WHERE id = $1', t) USING OLD.id;
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS {view}_del ON net.{view};
CREATE TRIGGER {view}_del INSTEAD OF DELETE ON net.{view}
    FOR EACH ROW EXECUTE FUNCTION net.{view}_delete();
""".format(view=view, reg=reg))

    # ---- INSERT ----
    ins_cols = ', '.join(cols)
    ins_vals = ', '.join('NEW.%s' % c for c in cols)
    if is_line:
        extra_cols = ', node_from, node_to, node_from_src, node_to_src, fileid_src'
        extra_vals = (', NEW.nodeid1, NEW.nodeid2, NEW.nodeid1, NEW.nodeid2,'
                      ' NEW.fileid')
        geom_ins = 'net.build_line_geom(NEW.nodeid1, NEW.nodeid2, NEW.coords)'
        frag = ('(SELECT fragment_id FROM net.node_plain WHERE id = NEW.nodeid1 '
                'UNION ALL SELECT fragment_id FROM net.consumer_real '
                'WHERE id = NEW.nodeid1 LIMIT 1)')
    else:
        extra_cols = ''
        extra_vals = ''
        geom_ins = geom_new
        frag = 'NEW.fileid'

    parts.append("""
CREATE OR REPLACE FUNCTION net.{view}_insert() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Тип объекта в момент вставки ещё не известен: прежняя модель
    -- создавала строку в базовой таблице, а тип задавала следующим
    -- запросом в таблицу-подтип. Поэтому объект попадает в *_plain,
    -- а смену класса выполняет net.reclass_node / net.reclass_line.
    IF NEW.id IS NULL THEN
        NEW.id := nextval('net.obj_id_seq');
    END IF;

    INSERT INTO net.{plain} (id, fragment_id, geom, removed_at{extra_cols}, {ins_cols})
    VALUES (NEW.id, {frag}, {geom_ins},
            CASE WHEN NEW.removed <> 0 THEN now() ELSE NULL END{extra_vals},
            {ins_vals});

    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS {view}_ins ON net.{view};
CREATE TRIGGER {view}_ins INSTEAD OF INSERT ON net.{view}
    FOR EACH ROW EXECUTE FUNCTION net.{view}_insert();
""".format(view=view, plain=plain, ins_cols=ins_cols, ins_vals=ins_vals,
           extra_cols=extra_cols, extra_vals=extra_vals,
           geom_ins=geom_ins, frag=frag))

    return '\n'.join(parts)


HELPERS = """
-- Геометрия линии по её концам и промежуточным точкам.
CREATE OR REPLACE FUNCTION net.build_line_geom(n_from bigint, n_to bigint,
                                               coords text)
RETURNS geometry
LANGUAGE sql STABLE SET search_path = pg_catalog, public, net AS $$
    SELECT ST_SetSRID(ST_MakeLine(
        ARRAY[(SELECT geom FROM net.v_nodes_geom WHERE id = n_from)]
        || net.parse_coords(coords, 100.0)
        || ARRAY[(SELECT geom FROM net.v_nodes_geom WHERE id = n_to)]), 9998);
$$;

-- Пересчёт геометрии существующей линии при смене промежуточных точек.
CREATE OR REPLACE FUNCTION net.rebuild_line_geom(line_id bigint, coords text)
RETURNS geometry
LANGUAGE sql STABLE SET search_path = pg_catalog, public, net AS $$
    SELECT net.build_line_geom(l.node_from, l.node_to, coords)
    FROM (SELECT node_from, node_to FROM net.v_linesobj_topo
          WHERE id = line_id) l;
$$;
"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mapping', required=True)
    ap.add_argument('--out', default='sql/050_write_triggers.sql')
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int, default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'almatygid'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    args = ap.parse_args()

    with open(args.mapping, encoding='utf-8') as f:
        m = json.load(f)

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()

    node_tables = [e['target'] for e in m['class_node']] + ['node_plain']
    line_tables = [e['target'] for e in m['class_line']] + ['line_plain']

    aux = ('CREATE OR REPLACE VIEW net.v_nodes_geom AS\n%s;\n\n'
           'CREATE OR REPLACE VIEW net.v_linesobj_topo AS\n%s;\n'
           % (' UNION ALL '.join('SELECT id, geom FROM net.%s' % t
                                 for t in node_tables),
              ' UNION ALL '.join(
                  'SELECT id, node_from, node_to FROM net.%s' % t
                  for t in line_tables)))

    sql = HEADER + '\n' + aux + HELPERS
    sql += gen(cur, 'v_nodes', 'node_reg', 'node_plain', 'nodes', False)
    sql += gen(cur, 'v_linesobj', 'line_reg', 'line_plain', 'linesobj', True)

    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    with open(args.out, 'w', encoding='utf-8') as f:
        f.write(sql)
    print('-> %s' % args.out)
    conn.close()


if __name__ == '__main__':
    main()
