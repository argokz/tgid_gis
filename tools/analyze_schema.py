"""Классификация таблиц ТГИД по структуре: подтипы узлов, подтипы линий,
справочники, пустые таблицы. Входные данные — JSON от dump_schema.py.
"""
import argparse
import json
from collections import defaultdict

# Колонки, по которым таблица опознаётся как подтип базовой геометрии.
NODE_LINK = 'internalnodeid'
LINE_LINK = 'internalnodeid'


def load(path):
    with open(path, encoding='utf-8') as f:
        return json.load(f)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--schema', required=True)
    ap.add_argument('--out', required=True)
    args = ap.parse_args()

    d = load(args.schema)

    cols = defaultdict(dict)
    order = defaultdict(list)
    for c in d['columns']:
        cols[c['table_name']][c['column_name']] = c
        order[c['table_name']].append(c['column_name'])

    counts = d['exact_counts']
    geom = {g['table_name']: g for g in d['geometry_columns']}
    kinds = {t['name']: t['kind'] for t in d['tables']}
    sizes = {t['name']: t['total_bytes'] for t in d['tables']}

    groups = defaultdict(list)

    for t in d['tables']:
        name = t['name']
        if kinds[name] != 'r':
            groups['view_or_other'].append(name)
            continue
        cc = cols[name]
        n = counts.get(name, 0)

        if name in ('nodes', 'linesobj'):
            g = 'base'
        elif name in ('audit_log', 'changed_object', 'audit_group_comments',
                      'oshibki_vstavki'):
            g = 'audit'
        elif name == 'spatial_ref_sys':
            g = 'postgis'
        elif n == 0:
            g = 'empty'
        elif 'nodeid' in cc:
            g = 'node_subtype'
        elif 'lineid' in cc:
            g = 'line_subtype'
        elif name in geom:
            g = 'geo_other'
        elif n <= 500 and 'id' in cc:
            g = 'reference'
        else:
            g = 'other'
        groups[g].append(name)

    lines = []
    lines.append('# Классификация таблиц БД almatygid\n')
    lines.append('Всего таблиц: %d, колонок: %d, внешних ключей: %d\n' % (
        len(d['tables']), len(d['columns']),
        sum(1 for c in d['constraints'] if c['type'] == 'f')))

    for g in ('base', 'node_subtype', 'line_subtype', 'geo_other', 'reference',
              'other', 'audit', 'empty', 'postgis', 'view_or_other'):
        names = sorted(groups[g], key=lambda x: -counts.get(x, 0))
        lines.append('\n## %s (%d)\n' % (g, len(names)))
        for name in names:
            gt = geom.get(name, {}).get('type', '')
            srid = geom.get(name, {}).get('srid', '')
            lines.append('- %-45s rows=%-8s cols=%-4d %s %s' % (
                name, counts.get(name, '?'), len(order[name]),
                gt, ('srid=%s' % srid) if srid else ''))

    with open(args.out, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines))

    print('\n'.join('%-16s %d' % (g, len(v)) for g, v in sorted(
        groups.items(), key=lambda kv: -len(kv[1]))))
    print('-> %s' % args.out)


if __name__ == '__main__':
    main()
