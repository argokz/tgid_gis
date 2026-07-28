"""Конвертер БД ТГИД: public (надтип + подтипы) -> net (одна таблица — один объект).

Свойства:
  * идемпотентен — повторный запуск даёт тот же результат (--truncate);
  * ничего не меняет в схеме public;
  * работает целиком на сервере (INSERT ... SELECT), данные не гоняются через Python;
  * всё, что перенести нельзя, попадает в net.conversion_reject / net.line_orphan,
    а не теряется молча.

Запуск:
    python converter/convert.py --mapping converter/mapping.json --apply
"""
import argparse
import json
import os
import sys
import time

import psycopg2

# Приоритет классов при отнесении узла к таблице: узел может числиться
# в нескольких подтипах, но объектом становится ровно в одном.
NODE_PRIORITY = [
    'heat_source', 'pump_station', 'consumer_real', 'consumer_general',
    'valve_3way', 'regulator_press', 'heat_chamber', 'refill_node',
    'connect_node',
]
LINE_PRIORITY = [
    'pipe_section', 'pump', 'elevator', 'heat_exchanger', 'air_heater',
    'damper', 'diaphragm', 'radiator', 'local_resistance',
]

GEOM_NODE = 'ST_SetSRID(ST_Point(n.x / {scale}, -n.y / {scale}), {srid})'


def q(name):
    return '"%s"' % name


class Converter:
    def __init__(self, conn, mapping, log):
        self.conn = conn
        self.m = mapping
        self.log = log
        self.srid = mapping['srid']
        self.scale = mapping['coord_scale']
        self.stats = []

    def run(self, cur, sql, label=None):
        t0 = time.time()
        cur.execute(sql)
        n = cur.rowcount
        if label:
            dt = time.time() - t0
            self.log('  %-28s %8d строк  %6.1f с' % (label, n, dt))
            self.stats.append({'step': label, 'rows': n, 'seconds': round(dt, 2)})
        return n

    # ---------- справочные шаги ----------

    def truncate(self, cur):
        self.log('Очистка схемы net')
        cur.execute("""
            select tablename from pg_tables where schemaname = 'net'
        """)
        tables = [r[0] for r in cur.fetchall()]
        if tables:
            cur.execute('TRUNCATE %s RESTART IDENTITY CASCADE'
                        % ', '.join('net.%s' % q(t) for t in tables))
        cur.execute("SELECT setval('net.obj_id_seq', 1, false)")

    def fragments(self, cur):
        self.log('Фрагменты')
        self.run(cur, """
            INSERT INTO net.fragment (id, name, name_sys, nasel_point, sezon,
                                      "year", removed_at)
            SELECT id, coalesce(name, 'без имени'), name_sys, nasel_point,
                   sezon, "year",
                   CASE WHEN removed <> 0 THEN now() END
            FROM public.fragments
            ON CONFLICT (id) DO NOTHING
        """, 'fragment')

        # В public.fragments всего 14 строк, а nodes.fileid принимает 25
        # различных значений: 73 % узлов ссылаются на несуществующий фрагмент.
        # Без этого шага внешний ключ обнулил бы им привязку, и приложение
        # перестало бы находить объекты по " AND n.fileID IN (...)".
        self.run(cur, """
            INSERT INTO net.fragment (id, name)
            SELECT DISTINCT f.fileid,
                   'фрагмент ' || f.fileid || ' (восстановлен конвертером)'
            FROM (
                SELECT fileid FROM public.nodes WHERE fileid IS NOT NULL
                UNION
                SELECT fileid FROM public.linesobj WHERE fileid IS NOT NULL
            ) f
            LEFT JOIN net.fragment nf ON nf.id = f.fileid
            WHERE nf.id IS NULL
            ON CONFLICT (id) DO NOTHING
        """, 'fragment (восстановленные)')

    # ---------- отнесение объектов к классам ----------

    def assign(self, cur, kind):
        """Таблица «объект -> целевой класс» с учётом приоритета."""
        entries = self.m['class_node' if kind == 'node' else 'class_line']
        prio = NODE_PRIORITY if kind == 'node' else LINE_PRIORITY
        link = 'nodeid' if kind == 'node' else 'lineid'
        by_target = {e['target']: e for e in entries}

        parts = []
        for rank, target in enumerate(prio):
            e = by_target.get(target)
            if not e:
                continue
            parts.append(
                "SELECT {link} AS obj_id, '{t}' AS target, {rank} AS rank, "
                "max(id) AS src_row "
                "FROM public.{src} WHERE {link} IS NOT NULL "
                "GROUP BY {link}".format(
                    link=link, t=target, rank=rank, src=e['source']))

        cur.execute('DROP TABLE IF EXISTS _assign_%s' % kind)
        self.run(cur, """
            CREATE TEMP TABLE _assign_{k} AS
            SELECT DISTINCT ON (obj_id) obj_id, target, src_row
            FROM ({parts}) u
            ORDER BY obj_id, rank
        """.format(k=kind, parts=' UNION ALL '.join(parts)),
            'assign_%s' % kind)
        cur.execute('CREATE UNIQUE INDEX ON _assign_%s (obj_id)' % kind)
        cur.execute('ANALYZE _assign_%s' % kind)

        # Строки подтипов, проигравшие приоритет или дубли, — в отчёт.
        for e in entries:
            self.run(cur, """
                INSERT INTO net.conversion_reject (src_table, src_id, reason, detail)
                SELECT '{src}', s.id, 'узел отнесён к другому классу или дубль',
                       jsonb_build_object('obj_id', s.{link})
                FROM public.{src} s
                LEFT JOIN _assign_{k} a
                       ON a.obj_id = s.{link} AND a.src_row = s.id
                                             AND a.target = '{t}'
                WHERE s.{link} IS NOT NULL AND a.obj_id IS NULL
            """.format(src=e['source'], link=link, k=kind, t=e['target']))

    # ---------- перенос классов ----------

    def class_tables(self, cur, kind):
        entries = self.m['class_node' if kind == 'node' else 'class_line']
        is_line = kind == 'line'
        base = self.m['base']['linesobj' if is_line else 'nodes']['columns']
        base_alias = 'l' if is_line else 'n'
        link = 'lineid' if is_line else 'nodeid'
        geom = self.geom_expr(is_line)

        for e in entries:
            self.insert_class(cur, e, base, base_alias, link, geom, is_line)

        # Объекты без класса
        plain = {'target': 'line_plain' if is_line else 'node_plain',
                 'source': 'linesobj' if is_line else 'nodes',
                 'columns': [], 'column_types': {}}
        self.insert_class(cur, plain, base, base_alias, link, geom, is_line,
                          plain=True)

    def build_src_map(self, cur):
        """Карта «старый nodes.id -> новый id». Нужна линиям и дочерним таблицам."""
        targets = [e['target'] for e in self.m['class_node']] + ['node_plain']
        parts = ['SELECT src_id, id FROM net.%s WHERE src_id IS NOT NULL' % t
                 for t in targets]
        self.run(cur, 'INSERT INTO net.node_src_map (src_id, id)\n%s\n'
                      'ON CONFLICT (src_id) DO NOTHING'
                      % '\nUNION ALL\n'.join(parts), 'node_src_map')
        cur.execute('ANALYZE net.node_src_map')

    def geom_expr(self, is_line):
        if not is_line:
            return GEOM_NODE.format(scale=self.scale, srid=self.srid)
        # Линия: узел1 + промежуточные точки из coords + узел2
        return ("ST_SetSRID(ST_MakeLine("
                "ARRAY[ST_Point(n1.x / {s}, -n1.y / {s})] "
                "|| net.parse_coords(l.coords, {s}) "
                "|| ARRAY[ST_Point(n2.x / {s}, -n2.y / {s})]), {srid})"
                .format(s=self.scale, srid=self.srid))

    def insert_class(self, cur, e, base, alias, link, geom, is_line, plain=False):
        target = e['target']
        seen = set()
        cols, vals = [], []

        for c in base:
            if c in ('id', 'fileid', 'nodeid1', 'nodeid2') or c in seen:
                continue
            seen.add(c)
            cols.append(q(c))
            vals.append('%s.%s' % (alias, q(c)))
        for c in e['columns']:
            if c in seen or c in ('id', 'fileid'):
                continue
            seen.add(c)
            cols.append(q(c))
            vals.append('s.%s' % q(c))

        # id СОХРАНЯЕТСЯ из public. На объекты ссылаются 139 колонок
        # в схеме public (nodeid, lineid, nodeid1, nodeid2), и перенумерация
        # порвала бы все эти связи. Реестры узлов и линий раздельные,
        # поэтому совпадение id узла и id линии допустимо.
        head = ['id', 'fragment_id', 'geom', 'removed_at', 'src_id']
        body = ['%s.id' % alias,
                'f.id',
                geom,
                'CASE WHEN %s.removed <> 0 THEN now() END' % alias,
                '%s.id' % alias]
        if is_line:
            head[1:1] = ['node_from', 'node_to', 'node_from_src',
                         'node_to_src', 'fileid_src']
            body[1:1] = ['nr1.id', 'nr2.id', 'l.nodeid1', 'l.nodeid2',
                         'l.fileid']

        src_tbl = 'linesobj' if is_line else 'nodes'
        if plain:
            join_sub = ''
            where_cls = ('a.obj_id IS NULL'
                         if not plain else 'a.obj_id IS NULL')
        else:
            join_sub = ('JOIN public.{src} s ON s.id = a.src_row\n'
                        .format(src=e['source']))
            where_cls = "a.target = '%s'" % target

        if is_line:
            frm = """
            FROM public.linesobj l
            JOIN public.nodes n1 ON n1.id = l.nodeid1
            JOIN public.nodes n2 ON n2.id = l.nodeid2
            JOIN net.node_src_map nr1 ON nr1.src_id = l.nodeid1
            JOIN net.node_src_map nr2 ON nr2.src_id = l.nodeid2
            LEFT JOIN _assign_line a ON a.obj_id = l.id
            {join_sub}
            LEFT JOIN net.fragment f ON f.id = n1.fileid
            WHERE {where}
              AND (n1.x <> 0 OR n1.y <> 0) AND (n2.x <> 0 OR n2.y <> 0)
            """.format(join_sub=join_sub, where=where_cls)
        else:
            frm = """
            FROM public.nodes n
            LEFT JOIN _assign_node a ON a.obj_id = n.id
            {join_sub}
            LEFT JOIN net.fragment f ON f.id = n.fileid
            WHERE {where}
              AND (n.x <> 0 OR n.y <> 0)
            """.format(join_sub=join_sub, where=where_cls)

        sql = ('INSERT INTO net.{t} ({cols})\nSELECT {vals}\n{frm}'
               .format(t=target,
                       cols=', '.join(head + cols),
                       vals=', '.join(body + vals),
                       frm=frm))
        self.run(cur, sql, target)

    # ---------- слои, аспекты, дочерние ----------

    def layers(self, cur):
        self.log('Самостоятельные слои')
        for e in self.m['layer']:
            cols = [q(c) for c in e['columns']]
            vals = ['s.%s' % q(c) for c in e['columns']]
            self.run(cur, """
                INSERT INTO net.{t} (geom, src_id{extra})
                SELECT ST_SetSRID(s.shape, {srid}), s.id{vals}
                FROM public.{src} s
                WHERE s.shape IS NOT NULL
            """.format(t=e['target'], src=e['source'], srid=self.srid,
                       extra=(', ' + ', '.join(cols)) if cols else '',
                       vals=(', ' + ', '.join(vals)) if vals else ''),
                e['target'])

    def children(self, cur):
        self.log('Аспекты и дочерние таблицы')
        for e in self.m['aspect'] + self.m['child']:
            link = e['link_column']
            cols = [q(c) for c in e['columns']]
            vals = ['s.%s' % q(c) for c in e['columns']]
            dedup = ('SELECT DISTINCT ON ({link}) * FROM public.{src} '
                     'WHERE {link} IS NOT NULL ORDER BY {link}, id DESC'
                     .format(link=link, src=e['source'])
                     if e['category'] == 'aspect'
                     else 'SELECT * FROM public.{src} WHERE {link} IS NOT NULL'
                     .format(src=e['source'], link=link))
            self.run(cur, """
                INSERT INTO net.{t} (node_id, src_id{extra})
                SELECT r.id, s.id{vals}
                FROM ({dedup}) s
                JOIN net.node_src_map r ON r.src_id = s.{link}
            """.format(t=e['target'], dedup=dedup, link=link,
                       extra=(', ' + ', '.join(cols)) if cols else '',
                       vals=(', ' + ', '.join(vals)) if vals else ''),
                e['target'])

            # Ничего не теряем молча: строки с пустой или битой ссылкой
            # на узел попадают в отчёт с указанием причины.
            self.run(cur, """
                INSERT INTO net.conversion_reject (src_table, src_id, reason, detail)
                SELECT '{src}', s.id,
                       CASE WHEN s.{link} IS NULL THEN 'ссылка на узел пуста'
                            ELSE 'узел не найден среди перенесённых' END,
                       jsonb_build_object('{link}', s.{link})
                FROM public.{src} s
                LEFT JOIN net.node_src_map r ON r.src_id = s.{link}
                WHERE s.{link} IS NULL OR r.src_id IS NULL
            """.format(src=e['source'], link=link))

    def bump_sequence(self, cur):
        """Сдвигаем последовательность выше всех перенесённых id, иначе
        новые объекты столкнутся с сохранёнными идентификаторами."""
        cur.execute("SELECT tablename FROM pg_tables WHERE schemaname = 'net'")
        tables = [t for (t,) in cur.fetchall()
                  if t not in ('fragment', 'node_reg', 'line_reg',
                               'line_orphan', 'conversion_reject',
                               'node_src_map')]
        parts = ['SELECT max(id) AS m FROM net.%s' % q(t) for t in tables]
        cur.execute('SELECT coalesce(max(m), 0) + 1 FROM (%s) s'
                    % ' UNION ALL '.join(parts))
        nxt = cur.fetchone()[0]
        cur.execute('SELECT setval(%s, %s, false)', ('net.obj_id_seq', nxt))
        self.log('  следующий id для новых объектов: %d' % nxt)

    def analyze(self, cur):
        """Без свежей статистики планировщик не берёт частичные индексы
        и уходит в Seq Scan — замер показал разницу в 3 раза."""
        self.log('Сбор статистики')
        cur.execute("SELECT tablename FROM pg_tables WHERE schemaname = 'net'")
        for (t,) in cur.fetchall():
            cur.execute('ANALYZE net.%s' % q(t))

    def orphans(self, cur):
        self.log('Линии без разрешимых концов')
        self.run(cur, """
            INSERT INTO net.line_orphan (id, src_table, nodeid1, nodeid2,
                                         reason, payload)
            SELECT l.id, 'linesobj', l.nodeid1, l.nodeid2,
                   CASE WHEN n1.id IS NULL AND n2.id IS NULL
                            THEN 'оба конца не найдены в nodes'
                        WHEN n1.id IS NULL THEN 'не найден nodeid1'
                        WHEN n2.id IS NULL THEN 'не найден nodeid2'
                        ELSE 'у конца нет координат' END,
                   to_jsonb(l) - 'shape'
            FROM public.linesobj l
            LEFT JOIN public.nodes n1 ON n1.id = l.nodeid1
            LEFT JOIN public.nodes n2 ON n2.id = l.nodeid2
            WHERE l.removed = 0
              AND (n1.id IS NULL OR n2.id IS NULL
                   OR (n1.x = 0 AND n1.y = 0) OR (n2.x = 0 AND n2.y = 0))
            ON CONFLICT (id) DO NOTHING
        """, 'line_orphan')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mapping', required=True)
    ap.add_argument('--apply', action='store_true',
                    help='без него делается rollback (пробный прогон)')
    ap.add_argument('--truncate', action='store_true', default=True)
    ap.add_argument('--report', default='docs/schema/conversion_report.json')
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int, default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'almatygid'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    args = ap.parse_args()

    with open(args.mapping, encoding='utf-8') as f:
        mapping = json.load(f)

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    conn.autocommit = False
    cur = conn.cursor()

    def log(msg):
        print(msg, flush=True)

    c = Converter(conn, mapping, log)
    t0 = time.time()
    try:
        if args.truncate:
            c.truncate(cur)
        c.fragments(cur)
        c.assign(cur, 'node')
        log('Точечные классы')
        c.class_tables(cur, 'node')
        c.build_src_map(cur)
        c.assign(cur, 'line')
        log('Линейные классы')
        c.class_tables(cur, 'line')
        c.layers(cur)
        c.children(cur)
        c.orphans(cur)
        c.bump_sequence(cur)
        c.analyze(cur)

        if args.apply:
            conn.commit()
            log('\nЗАФИКСИРОВАНО (COMMIT)')
        else:
            conn.rollback()
            log('\nПРОБНЫЙ ПРОГОН — откат. Для записи добавьте --apply')
    except Exception:
        conn.rollback()
        log('\nОШИБКА — откат')
        raise
    finally:
        conn.close()

    log('Всего %.1f с' % (time.time() - t0))
    if args.report:
        os.makedirs(os.path.dirname(args.report), exist_ok=True)
        with open(args.report, 'w', encoding='utf-8') as f:
            json.dump(c.stats, f, ensure_ascii=False, indent=1)


if __name__ == '__main__':
    sys.exit(main())
