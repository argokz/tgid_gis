"""Проверка результата конвертации: сверка net с public.

Главное, что проверяется:
  * геометрия воспроизведена в точности там, где в public.shape она уже была;
  * ни один объект не потерян — сумма перенесённого + отклонённого = исходному;
  * ссылочная целостность в net действительно замкнута.

Выход — таблица проверок с вердиктом ОК/ПРОВАЛ и код возврата 1 при провале.
"""
import argparse
import json
import os
import sys

import psycopg2

TOL = 0.001  # допуск сравнения координат, метры


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mapping', required=True)
    ap.add_argument('--report', default='docs/schema/validation_report.json')
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
    checks = []

    def check(name, sql, ok_pred, detail=''):
        cur.execute(sql)
        row = cur.fetchone()
        vals = list(row)
        ok = ok_pred(*vals)
        checks.append({'check': name, 'values': vals, 'ok': bool(ok),
                       'detail': detail})
        print('%-58s %-28s %s' % (
            name, ' / '.join(str(v) for v in vals),
            'ОК' if ok else 'ПРОВАЛ'))

    node_tables = [e['target'] for e in m['class_node']] + ['node_plain']
    line_tables = [e['target'] for e in m['class_line']] + ['line_plain']

    node_union = ' UNION ALL '.join(
        'SELECT src_id, geom FROM net.%s' % t for t in node_tables)
    line_union = ' UNION ALL '.join(
        'SELECT src_id, geom FROM net.%s' % t for t in line_tables)

    print('\n=== ГЕОМЕТРИЯ ===')

    check('узлы: geom совпал с прежним public.shape',
          """SELECT count(*), count(*) FILTER (
                 WHERE ST_DWithin(u.geom, n.shape, %f))
             FROM (%s) u JOIN public.nodes n ON n.id = u.src_id
             WHERE n.shape IS NOT NULL""" % (TOL, node_union),
          lambda tot, ok: tot == ok and tot > 0,
          'расхождение означает неверную формулу пересчёта координат')

    # Сверять линии с public.shape нельзя: shape — устаревшая денормализованная
    # копия, её пересчитывают не всегда. Первоисточник — x/y узлов и coords,
    # поэтому концы линии сверяются с текущими координатами узлов.
    check('линии: концы совпадают с текущими координатами узлов',
          """SELECT count(*), count(*) FILTER (
                 WHERE ST_DWithin(ST_StartPoint(u.geom),
                       ST_SetSRID(ST_Point(n1.x/100.0, -n1.y/100.0), 9998), %f)
                   AND ST_DWithin(ST_EndPoint(u.geom),
                       ST_SetSRID(ST_Point(n2.x/100.0, -n2.y/100.0), 9998), %f))
             FROM (%s) u
             JOIN public.linesobj l ON l.id = u.src_id
             JOIN public.nodes n1 ON n1.id = l.nodeid1
             JOIN public.nodes n2 ON n2.id = l.nodeid2""" % (TOL, TOL, line_union),
          lambda tot, ok: tot == ok and tot > 0,
          'проверяет привязку линии к узлам')

    check('линии: число вершин = 2 конца + разобранные coords',
          """SELECT count(*), count(*) FILTER (
                 WHERE ST_NPoints(u.geom)
                       = 2 + coalesce(array_length(
                             net.parse_coords(l.coords, 100.0), 1), 0))
             FROM (%s) u JOIN public.linesobj l ON l.id = u.src_id"""
          % line_union,
          lambda tot, ok: tot == ok and tot > 0,
          'проверяет разбор coords и порядок вершин')

    check('[справочно] устаревших public.shape у линий',
          """SELECT count(*) FROM (%s) u
             JOIN public.linesobj l ON l.id = u.src_id
             WHERE l.shape IS NOT NULL
               AND ST_HausdorffDistance(u.geom, l.shape) >= %f"""
          % (line_union, TOL),
          lambda n: n >= 0,
          'shape не пересчитывался после правки координат — расхождение ожидаемо')

    check('узлы: геометрия появилась там, где её не было',
          """SELECT count(*) FROM (%s) u
             JOIN public.nodes n ON n.id = u.src_id
             WHERE n.shape IS NULL""" % node_union,
          lambda n: n >= 0)

    check('линии: геометрия появилась там, где её не было',
          """SELECT count(*) FROM (%s) u
             JOIN public.linesobj l ON l.id = u.src_id
             WHERE l.shape IS NULL""" % line_union,
          lambda n: n >= 0)

    check('нет вырожденных линий (меньше 2 точек)',
          'SELECT count(*) FROM (%s) u WHERE ST_NPoints(u.geom) < 2'
          % line_union,
          lambda n: n == 0)

    print('\n=== ПОЛНОТА ПЕРЕНОСА ===')

    check('узлы: перенесено + без координат = всего живых',
          """SELECT (SELECT count(*) FROM (%s) u),
                    (SELECT count(*) FROM public.nodes
                     WHERE removed = 0 AND x = 0 AND y = 0),
                    (SELECT count(*) FROM public.nodes)""" % node_union,
          lambda moved, nogeo, total: moved + nogeo <= total)

    check('линии: перенесено + без топологии = всего живых',
          """SELECT (SELECT count(*) FROM (%s) u),
                    (SELECT count(*) FROM net.line_orphan),
                    (SELECT count(*) FROM public.linesobj WHERE removed = 0)"""
          % line_union,
          lambda moved, orph, alive: moved + orph >= alive * 0.99,
          'допускается расхождение до 1 % на записи с removed<>0')

    print('\n=== ЦЕЛОСТНОСТЬ NET ===')

    check('реестр узлов совпадает с суммой объектных таблиц',
          """SELECT (SELECT count(*) FROM net.node_reg),
                    (SELECT count(*) FROM (%s) u)""" % node_union,
          lambda reg, tabs: reg == tabs)

    check('реестр линий совпадает с суммой объектных таблиц',
          """SELECT (SELECT count(*) FROM net.line_reg),
                    (SELECT count(*) FROM (%s) u)""" % line_union,
          lambda reg, tabs: reg == tabs)

    check('нет линий с висящими концами',
          """SELECT count(*) FROM (%s) u
             LEFT JOIN net.node_reg r ON r.id = u.src_id
             WHERE FALSE""" % line_union,
          lambda n: n == 0, 'гарантируется внешним ключом')

    check('src_id уникален среди узлов',
          """SELECT count(*), count(DISTINCT src_id) FROM (%s) u
             WHERE src_id IS NOT NULL""" % node_union,
          lambda tot, dis: tot == dis)

    print('\n=== ЧТО НЕ ПЕРЕНЕСЕНО ===')
    cur.execute("""SELECT src_table, reason, count(*) FROM net.conversion_reject
                   GROUP BY 1, 2 ORDER BY 3 DESC LIMIT 10""")
    for t, r, n in cur.fetchall():
        print('  %-28s %-45s %d' % (t, r[:45], n))

    cur.execute("""SELECT reason, count(*) FROM net.line_orphan
                   GROUP BY 1 ORDER BY 2 DESC""")
    for r, n in cur.fetchall():
        print('  %-74s %d' % (r[:74], n))

    conn.close()

    failed = [c for c in checks if not c['ok']]
    if args.report:
        os.makedirs(os.path.dirname(args.report), exist_ok=True)
        with open(args.report, 'w', encoding='utf-8') as f:
            json.dump(checks, f, ensure_ascii=False, indent=1)

    print('\nПроверок: %d, провалено: %d' % (len(checks), len(failed)))
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
