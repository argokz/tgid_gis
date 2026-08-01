"""Сверка результатов расчёта между двумя БД.

Одинаковых итоговых цифр в консоли мало: расчёт пишет результаты
в таблицы *_OUT, и сравнивать надо именно их — построчно и по значениям.

Итерационный решатель НЕ бит-идентичен сам себе: два прогона на одной
БД уже дают расхождения (см. docs/schema/calc_regression_baseline.json).
Поэтому по умолчанию сравниваются абсолютная и относительная погрешности,
а не строгое равенство.

    python tools/compare_calc.py --db-a almatygid --db-b tgid_gis --fragment 2
    python tools/compare_calc.py --db-a almatygid --db-b tgid_gis --fragment 2 \\
        --report docs/schema/calc_regression_report.json
"""
import argparse
import json
import os
import sys

import psycopg2

OUT_TABLES = [
    # Ключ обязан быть УНИКАЛЬНЫМ в пределах расчёта. В ut_out и подобных
    # на один объект приходится две строки — подача и обратка, — поэтому
    # ключ составной. С одним lineid строки схлопывались в словаре, и
    # сравнение выдавало тысячи фантомных расхождений.
    ('us_out', ('nodeid', 'externalsign')),
    ('ut_out', ('lineid', 'externalsignlineid')),
    ('pt_out', ('nodeid',)),
    ('dr_out', ('nodeid',)),
    ('ns_out', ('lineid', 'externalsignlineid')),
    ('rs_out', ('lineid', 'externalsignlineid')),
    ('bp_out', ('lineid', 'externalsignlineid')),
    ('ok_out', ('lineid', 'externalsignlineid')),
    ('is_out', ('nodeid',)),
    ('po_out', ('nodeid',)),
]


def load_env(path='.env'):
    if not os.path.isfile(path):
        return
    with open(path, encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#') or '=' not in line:
                continue
            k, v = line.split('=', 1)
            os.environ.setdefault(k.strip(), v.strip().strip('"').strip("'"))


def connect(db, a):
    return psycopg2.connect(host=a.host, port=a.port, dbname=db,
                            user=a.user, password=a.password)


def last_calc(cur, fragment):
    cur.execute('SELECT max(id) FROM public.calculation WHERE fileid = %s',
                (fragment,))
    return cur.fetchone()[0]


def numeric_cols(cur, table):
    cur.execute("""
        SELECT column_name, data_type FROM information_schema.columns
        WHERE table_schema = 'public' AND table_name = %s
        ORDER BY ordinal_position""", (table,))
    return cur.fetchall()


def differs(va, vb, atol, rtol):
    if va is None and vb is None:
        return False, 0.0
    if va is None or vb is None:
        return True, None
    a, b = float(va), float(vb)
    d = abs(a - b)
    scale = max(abs(a), abs(b), 1e-12)
    return d > atol and d / scale > rtol, d


def is_virtual(k):
    """Ключ виртуального узла: движок нумерует их отрицательными id."""
    return bool(k) and isinstance(k[0], int) and k[0] < 0


def compare_table(qa, qb, table, key, ida, idb, atol, rtol):
    qa.execute('SELECT count(*) FROM public.%s WHERE calculationid = %%s'
               % table, (ida,))
    na = qa.fetchone()[0]
    qb.execute('SELECT count(*) FROM public.%s WHERE calculationid = %%s'
               % table, (idb,))
    nb = qb.fetchone()[0]

    info = {
        'table': table, 'key': key,
        'rows_a': na, 'rows_b': nb,
        'status': None, 'diff_cells': 0, 'max_abs': 0.0, 'worst_col': None,
        'numeric_cols': 0, 'virtual_rows': 0, 'virtual_unmatched': 0,
    }
    if na != nb:
        info['status'] = 'row_count_mismatch'
        return info
    if na == 0:
        info['status'] = 'empty'
        return info

    cols = [c for c, t in numeric_cols(qa, table)
            if t in ('double precision', 'real', 'numeric', 'integer',
                     'bigint', 'smallint')
            and c not in ('id', 'calculationid') and c not in key]
    info['numeric_cols'] = len(cols)

    diffs = 0
    maxd = 0.0
    worst = None
    virt_rows = virt_unmatched = 0
    first = True
    for c in cols:
        qa.execute(
            'SELECT %s, %s FROM public.%s WHERE calculationid = %%s' % (', '.join(key), c, table), (ida,))
        rows_a = {r[:-1]: r[-1] for r in qa.fetchall()}
        qb.execute(
            'SELECT %s, %s FROM public.%s WHERE calculationid = %%s' % (', '.join(key), c, table), (idb,))
        rows_b = {r[:-1]: r[-1] for r in qb.fetchall()}
        keys = set(rows_a) | set(rows_b)
        for k in keys:
            # Отрицательный id — виртуальный узел, созданный движком по ходу
            # расчёта, а не объект БД. Его номер зависит от порядка создания
            # и между прогонами не совпадает, поэтому такие строки в вердикт
            # не идут: иначе сверка вечно показывает ~135 «расхождений»,
            # за которыми нет ни одного реального объекта.
            if is_virtual(k):
                if first:
                    virt_rows += 1
                    if k not in rows_a or k not in rows_b:
                        virt_unmatched += 1
                continue
            if k not in rows_a or k not in rows_b:
                diffs += 1
                continue
            bad, d = differs(rows_a[k], rows_b[k], atol, rtol)
            if bad:
                diffs += 1
                if d is not None and d > maxd:
                    maxd, worst = d, c
        first = False

    info['virtual_rows'] = virt_rows
    info['virtual_unmatched'] = virt_unmatched

    info['diff_cells'] = diffs
    info['max_abs'] = maxd
    info['worst_col'] = worst
    info['status'] = 'mismatch' if diffs else 'ok'
    return info


def main():
    load_env()
    ap = argparse.ArgumentParser()
    ap.add_argument('--db-a', required=True)
    ap.add_argument('--db-b', required=True)
    ap.add_argument('--fragment', type=int, required=True)
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int,
                    default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--atol', type=float, default=1e-6,
                    help='абсолютная погрешность')
    ap.add_argument('--rtol', type=float, default=1e-6,
                    help='относительная погрешность (доля от max(|a|,|b|))')
    ap.add_argument('--report', default='',
                    help='путь к JSON-отчёту')
    args = ap.parse_args()

    ca, cb = connect(args.db_a, args), connect(args.db_b, args)
    qa, qb = ca.cursor(), cb.cursor()

    ida, idb = last_calc(qa, args.fragment), last_calc(qb, args.fragment)
    print('расчёт в %s: id=%s' % (args.db_a, ida))
    print('расчёт в %s: id=%s' % (args.db_b, idb))
    report = {
        'db_a': args.db_a, 'db_b': args.db_b,
        'fragment': args.fragment,
        'calc_a': ida, 'calc_b': idb,
        'atol': args.atol, 'rtol': args.rtol,
        'tables': [],
    }
    if ida is None or idb is None:
        print('ПРОВАЛ: расчёт не найден в одной из БД')
        report['error'] = 'missing_calculation'
        if args.report:
            with open(args.report, 'w', encoding='utf-8') as f:
                json.dump(report, f, ensure_ascii=False, indent=2)
        return 1

    bad = 0
    print('\n%-10s %10s %10s  %s' % ('таблица', args.db_a[:10], args.db_b[:10],
                                     'вердикт'))
    for table, key in OUT_TABLES:
        try:
            info = compare_table(qa, qb, table, key, ida, idb,
                                 args.atol, args.rtol)
        except Exception:
            ca.rollback()
            cb.rollback()
            info = {'table': table, 'status': 'missing',
                    'rows_a': None, 'rows_b': None}
            print('%-10s %10s %10s  таблицы нет' % (table, '-', '-'))
            report['tables'].append(info)
            continue

        report['tables'].append(info)
        if info['status'] == 'row_count_mismatch':
            print('%-10s %10d %10d  РАСХОЖДЕНИЕ В ЧИСЛЕ СТРОК'
                  % (table, info['rows_a'], info['rows_b']))
            bad += 1
        elif info['status'] == 'empty':
            print('%-10s %10d %10d  пусто'
                  % (table, info['rows_a'], info['rows_b']))
        elif info['status'] == 'mismatch':
            print('%-10s %10d %10d  РАСХОЖДЕНИЙ %d, макс %.6g по %s'
                  % (table, info['rows_a'], info['rows_b'],
                     info['diff_cells'], info['max_abs'], info['worst_col']))
            bad += 1
        else:
            print('%-10s %10d %10d  совпадает по всем %d числовым полям'
                  % (table, info['rows_a'], info['rows_b'],
                     info['numeric_cols']))

        v = info.get('virtual_rows') or 0
        if v:
            print('%-10s %10s %10s  + %d виртуальных узлов вне сверки'
                  ' (без пары %d)'
                  % ('', '', '', v, info.get('virtual_unmatched') or 0))

    ca.close()
    cb.close()
    report['tables_with_diffs'] = bad
    print('\nтаблиц с расхождениями: %d' % bad)
    if args.report:
        os.makedirs(os.path.dirname(args.report) or '.', exist_ok=True)
        with open(args.report, 'w', encoding='utf-8') as f:
            json.dump(report, f, ensure_ascii=False, indent=2)
        print('отчёт:', args.report)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
