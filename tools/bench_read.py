"""Сравнение старого и нового запросов чтения схемы: совместимость и скорость.

Проверяет, что новый запрос:
  * возвращает тот же набор колонок;
  * возвращает те же объекты (сверка по id);
  * допускает дописывание фильтра " AND n.fileID IN (...)", как это
    делает приложение;
и измеряет время выполнения на реальных данных.
"""
import argparse
import os
import statistics
import time

import psycopg2


def read(path):
    # Старые sql3/*.sql лежат в CP1251, новые — в UTF-8.
    for enc in ('utf-8', 'cp1251'):
        try:
            with open(path, encoding=enc) as f:
                return f.read()
        except UnicodeDecodeError:
            continue
    raise SystemExit('не удалось определить кодировку %s' % path)


def timed(cur, sql, runs):
    times = []
    rows = None
    for _ in range(runs):
        t0 = time.time()
        cur.execute(sql)
        got = cur.fetchall()
        times.append(time.time() - t0)
        rows = len(got)
    return rows, min(times), statistics.median(times)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--old', required=True)
    ap.add_argument('--new', required=True)
    ap.add_argument('--alias', default='n')
    ap.add_argument('--fragments', default='',
                    help='список fileID через запятую; пусто = вся база')
    ap.add_argument('--runs', type=int, default=3)
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int, default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'almatygid'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    args = ap.parse_args()

    old, new = read(args.old), read(args.new)
    if args.fragments:
        # ровно так дописывает приложение
        suffix = ' AND %s.fileID IN (%s)' % (args.alias, args.fragments)
        old += suffix
        new += suffix

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()

    print('фрагменты: %s' % (args.fragments or 'вся база'))

    cur.execute('SELECT * FROM (%s) z LIMIT 0' % old)
    old_cols = [d[0] for d in cur.description]
    cur.execute('SELECT * FROM (%s) z LIMIT 0' % new)
    new_cols = [d[0] for d in cur.description]

    missing = [c for c in old_cols if c not in new_cols]
    extra = [c for c in new_cols if c not in old_cols]
    print('\nколонок старых: %d, новых: %d' % (len(old_cols), len(new_cols)))
    print('нет в новом : %s' % (', '.join(missing) or 'нет'))
    print('лишних      : %s' % (', '.join(extra) or 'нет'))

    cur.execute("""
        SELECT (SELECT count(*) FROM (%s) a),
               (SELECT count(*) FROM (%s) b),
               (SELECT count(*) FROM (SELECT id FROM (%s) a1
                                      EXCEPT SELECT id FROM (%s) b1) d1),
               (SELECT count(*) FROM (SELECT id FROM (%s) b2
                                      EXCEPT SELECT id FROM (%s) a2) d2)
    """ % (old, new, old, new, new, old))
    o, n, only_old, only_new = cur.fetchone()
    print('\nстрок старый: %d, новый: %d' % (o, n))
    print('только в старом: %d, только в новом: %d' % (only_old, only_new))

    print('\nзамер (%d прогонов, берём минимум):' % args.runs)
    ro, mn_o, md_o = timed(cur, old, args.runs)
    rn, mn_n, md_n = timed(cur, new, args.runs)
    print('  старый: %8.0f мс (медиана %.0f), строк %d' % (mn_o * 1000, md_o * 1000, ro))
    print('  новый : %8.0f мс (медиана %.0f), строк %d' % (mn_n * 1000, md_n * 1000, rn))
    if mn_n > 0:
        print('  ускорение: %.1fx' % (mn_o / mn_n))

    conn.close()


if __name__ == '__main__':
    main()
