"""Кардинальность связи «базовый объект -> строка подтипа».

Отвечает на вопрос, чем на самом деле является таблица-подтип:
  1:1  — класс объекта, его можно сделать отдельной таблицей;
  1:N  — таблица-связь или история, объектом быть не может.
"""
import argparse
import json
import os

import psycopg2


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mapping', required=True)
    ap.add_argument('--out')
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

    report = []
    for key, link in (('node_types', 'nodeid'), ('line_types', 'lineid')):
        for e in m[key]:
            cur.execute(
                'select count(*) as rows, count(distinct {link}) as objs, '
                'max(c) as max_per_obj from ('
                '  select {link}, count(*) c from public.{src} '
                '  where {link} is not null group by {link}) z'.format(
                    link=link, src=e['source']))
            rows, objs, mx = cur.fetchone()
            mx = mx or 0
            # rows здесь — число различных объектов; пересчитаем строки честно
            cur.execute('select count(*) from public.%s' % e['source'])
            total = cur.fetchone()[0]
            cur.execute('select count(*) from public.%s where %s is null'
                        % (e['source'], link))
            nulls = cur.fetchone()[0]

            verdict = '1:1' if mx <= 1 else '1:N'
            report.append({
                'kind': 'node' if key == 'node_types' else 'line',
                'source': e['source'],
                'target': e['target'],
                'total_rows': total,
                'distinct_objects': objs or 0,
                'max_rows_per_object': mx,
                'null_link_rows': nulls,
                'cardinality': verdict,
                'columns': len(e['columns']),
            })

    conn.close()

    report.sort(key=lambda r: (r['cardinality'], -r['total_rows']))
    print('%-28s %-22s %8s %8s %6s %6s %s' % (
        'ИСТОЧНИК', 'ЦЕЛЬ', 'СТРОК', 'ОБЪЕКТОВ', 'MAX', 'КОЛ-В', 'ВЕРДИКТ'))
    for r in report:
        print('%-28s %-22s %8d %8d %6d %6d %s' % (
            r['source'], r['target'], r['total_rows'], r['distinct_objects'],
            r['max_rows_per_object'], r['columns'], r['cardinality']))

    if args.out:
        os.makedirs(os.path.dirname(args.out), exist_ok=True)
        with open(args.out, 'w', encoding='utf-8') as f:
            json.dump(report, f, ensure_ascii=False, indent=1)
        print('\n-> %s' % args.out)


if __name__ == '__main__':
    main()
