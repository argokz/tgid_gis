"""Сравнение двух расчётов по их id — в одной БД или в разных.

В старой БД результаты лежат в ``public``, в новой — в ``calc``. Схема
определяется по факту тем же кодом, что использует compare_calc.py.

    python tools/compare_calc_ids.py --db-a tgid_gis --calc-a 1 \
        --db-b tgid_gis --calc-b 2
    python tools/compare_calc_ids.py ... \
        --report docs/schema/calc_regression_baseline.json
"""
import argparse
import json
import os
import sys

from compare_calc import (OUT_TABLES, compare_table, connect, load_env,
                          qname, schema_of)


def calculation(cur, calculation_id):
    cur.execute('SELECT fileid FROM %s WHERE id = %%s'
                % qname(cur, 'calculation'), (calculation_id,))
    return cur.fetchone()


def relation_exists(cur, table):
    try:
        schema_of(cur, table)
        return True
    except KeyError:
        return False


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

    ca, cb = connect(args.db_a, args), connect(args.db_b, args)
    qa, qb = ca.cursor(), cb.cursor()

    print('A: %s расчёт %d' % (args.db_a, args.calc_a))
    print('B: %s расчёт %d\n' % (args.db_b, args.calc_b))

    calc_a = calculation(qa, args.calc_a)
    calc_b = calculation(qb, args.calc_b)
    if calc_a is None or calc_b is None:
        if calc_a is None:
            print('ПРОВАЛ: расчёт %d не найден в %s'
                  % (args.calc_a, args.db_a))
        if calc_b is None:
            print('ПРОВАЛ: расчёт %d не найден в %s'
                  % (args.calc_b, args.db_b))
        ca.close()
        cb.close()
        return 2

    report = {
        'db_a': args.db_a, 'calc_a': args.calc_a,
        'db_b': args.db_b, 'calc_b': args.calc_b,
        'fragment_a': calc_a[0], 'fragment_b': calc_b[0],
        'atol': args.atol, 'rtol': args.rtol,
        'tables': [],
    }

    if calc_a[0] != calc_b[0]:
        print('ПРОВАЛ: расчёты относятся к разным фрагментам: %s и %s'
              % (calc_a[0], calc_b[0]))
        ca.close()
        cb.close()
        return 2

    print('фрагмент: %s' % calc_a[0])
    print('%-10s %10s %10s  %s'
          % ('таблица', 'строк A', 'строк B', 'вердикт'))

    bad = 0
    compared = 0
    for table, key in OUT_TABLES:
        exists_a = relation_exists(qa, table)
        exists_b = relation_exists(qb, table)
        if not exists_a and not exists_b:
            print('%-10s %10s %10s  нет в обеих БД'
                  % (table, '-', '-'))
            report['tables'].append({
                'table': table, 'status': 'missing_both',
                'rows_a': None, 'rows_b': None,
            })
            continue
        if exists_a != exists_b:
            print('%-10s %10s %10s  ТАБЛИЦА ЕСТЬ ТОЛЬКО В ОДНОЙ БД'
                  % (table, '-' if not exists_a else 'есть',
                     '-' if not exists_b else 'есть'))
            report['tables'].append({
                'table': table, 'status': 'missing_one',
                'rows_a': None, 'rows_b': None,
            })
            bad += 1
            continue

        info = compare_table(qa, qb, table, key, args.calc_a, args.calc_b,
                             args.atol, args.rtol)
        report['tables'].append(info)
        compared += 1

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
                     info['diff_cells'], info['max_abs'],
                     info['worst_col']))
            bad += 1
        else:
            print('%-10s %10d %10d  совпадает по всем %d числовым полям'
                  % (table, info['rows_a'], info['rows_b'],
                     info['numeric_cols']))

        virtual_rows = info.get('virtual_rows') or 0
        if virtual_rows:
            print('%-10s %10s %10s  + %d виртуальных узлов вне сверки'
                  ' (без пары %d)'
                  % ('', '', '', virtual_rows,
                     info.get('virtual_unmatched') or 0))

    ca.close()
    cb.close()

    if compared == 0:
        print('\nПРОВАЛ: ни одна таблица результатов не была проверена')
        report['error'] = 'no_result_tables_checked'
        bad += 1

    report['tables_checked'] = compared
    report['tables_with_diffs'] = bad
    print('\nпроверено таблиц: %d' % compared)
    print('таблиц с расхождениями: %d' % bad)
    if args.report:
        os.makedirs(os.path.dirname(args.report) or '.', exist_ok=True)
        with open(args.report, 'w', encoding='utf-8') as f:
            json.dump(report, f, ensure_ascii=False, indent=2)
        print('отчёт:', args.report)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
