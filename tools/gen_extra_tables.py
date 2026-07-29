"""Таблицы для строк подтипов, не ставших объектом.

Такие строки бывают двух видов: дубли на один узел и «проигравшие чужому
классу» (узел числится сразу в realconsumers и generalizedconsumers).
Слой совместимости обязан их возвращать — иначе расчётное ядро видит
меньше объектов, чем на исходной БД.

Сначала они хранились в net.object_variant одним полем jsonb, а
представление доставало колонки через payload ->> 'имя'. Это оказалось
катастрофически медленным: выборка 152 колонок шла 211 секунд против
0.4 секунды на исходной БД, и чтение потребителей в расчёте занимало
145 секунд вместо 0.7.

Поэтому строки дублируются в обычную таблицу с настоящими колонками:
представление становится UNION ALL двух обычных таблиц, у планировщика
появляется статистика, jsonb из горячего пути уходит. В object_variant
данные остаются — там они нужны для разбора спорных случаев.
"""
import argparse
import json
import os

TYPE_MAP = {
    'integer': 'int', 'bigint': 'bigint', 'smallint': 'smallint',
    'double precision': 'double precision', 'real': 'real',
    'numeric': 'numeric', 'boolean': 'boolean', 'date': 'date',
    'text': 'text', 'character varying': 'text', 'character': 'text',
    'timestamp without time zone': 'timestamp',
    'timestamp with time zone': 'timestamptz',
    'time without time zone': 'time', 'uuid': 'uuid',
    'jsonb': 'jsonb', 'json': 'jsonb', 'bytea': 'bytea',
    'ARRAY': 'text[]', 'USER-DEFINED': 'text',
}

RESERVED = {'year', 'name', 'end', 'order', 'group', 'user', 'default',
            'check', 'column', 'table', 'all', 'and', 'or', 'not', 'null',
            'offset', 'limit', 'references', 'primary', 'foreign', 'from',
            'select', 'where', 'desc', 'asc', 'union', 'do', 'to', 'in', 'is'}


def ident(n):
    if n in RESERVED or not n.replace('_', '').isalnum() or n[0].isdigit():
        return '"%s"' % n
    return n


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mapping', required=True)
    ap.add_argument('--out', default='sql/014_extra_tables.sql')
    args = ap.parse_args()

    with open(args.mapping, encoding='utf-8') as f:
        m = json.load(f)

    out = ['-- СГЕНЕРИРОВАНО tools/gen_extra_tables.py',
           '--',
           '-- Строки подтипов, не ставшие объектом: дубли и проигравшие',
           '-- чужому классу. Хранятся с настоящими колонками, а не в jsonb —',
           '-- см. заголовок генератора о разнице в 211 секунд.',
           '']

    for e in m['class_node'] + m['class_line']:
        src = e['source']
        link = e['link_column']
        out.append('\n-- лишние строки %s' % src)
        out.append('CREATE TABLE IF NOT EXISTS net.extra_%s (' % src)
        out.append('    id      int PRIMARY KEY')
        out.append('  , obj_id  bigint NOT NULL')
        for c in e['columns']:
            t = TYPE_MAP.get(e['column_types'][c]['data_type'], 'text')
            out.append('  , %-32s %s' % (ident(c), t))
        out.append(');')
        out.append('CREATE INDEX IF NOT EXISTS extra_%s_obj_idx '
                   'ON net.extra_%s (obj_id);' % (src, src))

    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    with open(args.out, 'w', encoding='utf-8') as f:
        f.write('\n'.join(out) + '\n')
    print('-> %s (%d таблиц)' % (args.out,
                                 len(m['class_node']) + len(m['class_line'])))


if __name__ == '__main__':
    main()
