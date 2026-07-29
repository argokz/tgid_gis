"""Сравнение двух расчётов по их id — в одной БД или в разных.

Нужен как контрольный опыт: прежде чем считать расхождение следствием
миграции, надо знать, воспроизводит ли расчёт сам себя на одних и тех же
данных. Итерационный решатель может давать расхождение и без миграции.

    python tools/compare_calc_ids.py --db-a tgid_gis --calc-a 1 --db-b tgid_gis --calc-b 2
    python tools/compare_calc_ids.py ... --report docs/schema/calc_regression_baseline.json
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


def differs(va, vb, atol, rtol):
    if va is None and vb is None:
        return False, 0.0
    if va is None or vb is None:
        return True, None
    a, b = float(va), float(vb)
    d = abs(a - b)
    scale = max(abs(a), abs(b), 1e-12)
    return d > atol and d / scale > rtol, d


def main():
    load_env()
    ap = argparse.ArgumentParser()
    ap.add_argument('--db-a', required=True)
    ap.add_argument('--calc-a', type=int, required=True)
    ap.add_argument('--db-b', required=True)
    ap.add_argument('--calc-b', type=int, required=True)
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int,
                    default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--atol', type=float, default=1e-6)
    ap.add_argument('--rtol', type=float, default=1e-6)
    ap.add_argument('--report', default='')
    args = ap.parse_args()

    def conn(db):
        return psycopg2.connect(host=args.host, port=args.port, dbname=db,
                                user=args.user, password=args.password)

    ca, cb = conn(args.db_a), conn(args.db_b)
    qa, qb = ca.cursor(), cb.cursor()

    print('A: %s расчёт %d' % (args.db_a, args.calc_a))
    print('B: %s расчёт %d\n' % (args.db_b, args.calc_b))
    print('%-9s %8s %8s  %s' % ('таблица', 'строк A', 'строк B', 'вердикт'))

    report = {
        'db_a': args.db_a, 'calc_a': args.calc_a,
        'db_b': args.db_b, 'calc_b': args.calc_b,
        'atol': args.atol, 'rtol': args.rtol,
        'note': ('Базовый шум решателя: два прогона на одной БД. '
                 'Сравнивать миграцию имеет смысл только относительно этого.'),
        'tables': [],
    }

    bad = 0
    for table, key in OUT_TABLES:
        try:
            qa.execute('SELECT count(*) FROM public.%s WHERE calculationid=%%s'
                       % table, (args.calc_a,))
            na = qa.fetchone()[0]
            qb.execute('SELECT count(*) FROM public.%s WHERE calculationid=%%s'
                       % table, (args.calc_b,))
            nb = qb.fetchone()[0]
        except Exception:
            ca.rollback()
            cb.rollback()
            continue

        info = {'table': table, 'rows_a': na, 'rows_b': nb}
        if na == 0 and nb == 0:
            print('%-9s %8d %8d  пусто' % (table, na, nb))
            info['status'] = 'empty'
            report['tables'].append(info)
            continue
        if na != nb:
            print('%-9s %8d %8d  РАЗНОЕ ЧИСЛО СТРОК' % (table, na, nb))
            info['status'] = 'row_count_mismatch'
            report['tables'].append(info)
            bad += 1
            continue

        qa.execute("""SELECT column_name FROM information_schema.columns
                      WHERE table_schema='public' AND table_name=%s
                        AND data_type IN ('double precision','real','numeric',
                                          'integer','bigint','smallint')
                        AND column_name NOT IN ('id','calculationid')
                      ORDER BY ordinal_position""", (table,))
        cols = [r[0] for r in qa.fetchall()]

        diffs = 0
        maxd = 0.0
        worst = None
        for c in cols:
            qa.execute('SELECT %s,%s FROM public.%s WHERE calculationid=%%s' % (', '.join(key), c, table), (args.calc_a,))
            ra = {r[:-1]: r[-1] for r in qa.fetchall()}
            qb.execute('SELECT %s,%s FROM public.%s WHERE calculationid=%%s' % (', '.join(key), c, table), (args.calc_b,))
            rb = {r[:-1]: r[-1] for r in qb.fetchall()}
            for k in set(ra) | set(rb):
                if k not in ra or k not in rb:
                    diffs += 1
                    continue
                bad_cell, d = differs(ra[k], rb[k], args.atol, args.rtol)
                if bad_cell:
                    diffs += 1
                    if d is not None and d > maxd:
                        maxd, worst = d, c

        info['diff_cells'] = diffs
        info['max_abs'] = maxd
        info['worst_col'] = worst
        info['numeric_cols'] = len(cols)
        if diffs:
            print('%-9s %8d %8d  РАСХОЖДЕНИЙ %d, макс %.6g по %s'
                  % (table, na, nb, diffs, maxd, worst))
            info['status'] = 'mismatch'
            bad += 1
        else:
            print('%-9s %8d %8d  совпадает (%d полей)'
                  % (table, na, nb, len(cols)))
            info['status'] = 'ok'
        report['tables'].append(info)

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
