"""Генерация converter/mapping.json — карты «исходная таблица → целевая таблица».

Классификация построена на измеренных фактах (tools/check_cardinality.py,
tools/check_overlap.py), а не на догадках по именам:

  A. class_node / class_line — настоящие классы объектов, связь с nodes/linesobj
     заполнена, кардинальность 1:1. Становятся отдельными таблицами с геометрией.
  B. layer                   — уже самостоятельные ГИС-слои: своя геометрия
     заполнена на 100 %, ссылка на nodes пуста на 100 %. Переносятся как есть.
  C. aspect                  — 1:1, но не класс: один и тот же узел одновременно
     является потребителем и, например, узлом задания давления. Остаются
     дочерними таблицами.
  D. child                   — честная связь 1:N (до 10 строк на объект).
     Остаются дочерними таблицами.
  E. junk                    — ни геометрии, ни ссылки. Не переносятся, идут в отчёт.
"""
import argparse
import json
import os
from collections import defaultdict

# A. Классы точечных объектов (связь nodeid заполнена)
CLASS_NODE = {
    'generalizedconsumers': 'consumer_general',
    'realconsumers': 'consumer_real',
    'heatsources': 'heat_source',
    'pumpstations': 'pump_station',
    'heatchambers': 'heat_chamber',
    'threewayvalves': 'valve_3way',
    'connectnodes': 'connect_node',
    'refillnodes': 'refill_node',
}

# A. Классы линейных объектов (связь lineid заполнена)
#
# pressregulators имеет ОБЕ ссылки — nodeid (463) и lineid (509).
# Решающий признак — как объект читает само приложение: ut.sql джойнит
# его по lineID и присваивает код типа 'RD', а us.sql не джойнит вовсе.
# Значит это линейный класс, регулятор стоит НА участке.
CLASS_LINE = {
    'pressregulators': 'regulator_press',
    'heatpipesections': 'pipe_section',
    'diaphragms': 'diaphragm',
    'dampers': 'damper',
    'elevators': 'elevator',
    'systemradiators': 'radiator',
    'pumps': 'pump',
    'heatexchangers': 'heat_exchanger',
    'airheaters': 'air_heater',
    'localhydroresistances2': 'local_resistance',
}

# B. Самостоятельные слои: геометрия своя, ссылки на nodes/linesobj нет
LAYER = {
    'ugol_povorota_truboprovoda': 'pipe_turn',
    'lyuki': 'manhole',
    'opora': 'support',
    'vvod_v_zdanie': 'building_inlet',
    'vvody_v_zdanie': 'building_inlet_line',
    'perehod_diametra': 'diameter_change',
    'kapremont_uchastki_remonta': 'repair_section',
    'opressovka_defekt': 'pressure_test_defect',
    'zdaniya_2': 'building',
    'zdaniya_potrebiteley': 'building_consumer',
    'zdaniya_tu': 'building_tu',
    'truby_rekonstruiruemye': 'pipe_reconstructed',
}

# C. Аспекты узла (1:1, но узел при этом принадлежит другому классу)
ASPECT = {
    'setpressnodes': ('node_press_setting', 'nodeid'),
}

# D. Дочерние таблицы 1:N
CHILD = {
    'wdodevices': ('node_metering_device', 'nodeid'),
    'deployeddirections': ('node_direction', 'nodeid'),
}

# E. Ни геометрии, ни ссылки — переносить нечего
JUNK = {
    'pressdropregulators': 'нет геометрии, nodeid пуст во всех 13 строках',
    'consumptregulators': 'нет геометрии, nodeid пуст во всех 7 строках',
}

BASE_DROP = {
    'nodes': {'x', 'y', 'shape', 'removed', 'idremoved', 'coords',
              'globalid', 'gistable', 'sync', 'gis', 'sync_tgid', 'id_old'},
    'linesobj': {'coords', 'shape', 'removed', 'idremoved',
                 'globalid', 'gistable', 'sync', 'gis', 'sync_tgid', 'id_old'},
}

SUB_DROP = {'id', 'nodeid', 'lineid'}
LAYER_DROP = {'id', 'shape', 'nodeid', 'lineid'}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--schema', required=True)
    ap.add_argument('--cardinality')
    ap.add_argument('--out', required=True)
    args = ap.parse_args()

    with open(args.schema, encoding='utf-8') as f:
        d = json.load(f)

    cols = defaultdict(list)
    types = defaultdict(dict)
    for c in d['columns']:
        cols[c['table_name']].append(c['column_name'])
        types[c['table_name']][c['column_name']] = c

    counts = d['exact_counts']
    geom = {g['table_name']: g for g in d['geometry_columns']}

    def col_meta(src, keep):
        return {c: {
            'data_type': types[src][c]['data_type'],
            'max_length': types[src][c]['character_maximum_length'],
            'nullable': types[src][c]['is_nullable'] == 'YES',
        } for c in keep}

    def entry(src, target, geom_kind, link, category):
        drop = LAYER_DROP if category == 'layer' else SUB_DROP
        keep = [c for c in cols[src] if c not in drop]
        return {
            'source': src,
            'target': target,
            'category': category,
            'geometry': geom_kind,
            'link_column': link,
            'rows': counts.get(src, 0),
            'columns': keep,
            'column_types': col_meta(src, keep),
        }

    mapping = {
        'srid': 9998,
        'coord_scale': 100.0,
        'flip_y': True,
        'base': {
            'nodes': {'columns': [c for c in cols['nodes']
                                  if c not in BASE_DROP['nodes']]},
            'linesobj': {'columns': [c for c in cols['linesobj']
                                     if c not in BASE_DROP['linesobj']]},
        },
        'class_node': [entry(s, t, 'Point', 'nodeid', 'class_node')
                       for s, t in sorted(CLASS_NODE.items())],
        'class_line': [entry(s, t, 'LineString', 'lineid', 'class_line')
                       for s, t in sorted(CLASS_LINE.items())],
        'layer': [entry(s, t, geom.get(s, {}).get('type', 'Geometry'),
                        None, 'layer')
                  for s, t in sorted(LAYER.items())],
        'aspect': [entry(s, t, None, link, 'aspect')
                   for s, (t, link) in sorted(ASPECT.items())],
        'child': [entry(s, t, None, link, 'child')
                  for s, (t, link) in sorted(CHILD.items())],
        'junk': [{'source': s, 'rows': counts.get(s, 0), 'reason': r}
                 for s, r in sorted(JUNK.items())],
    }

    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    with open(args.out, 'w', encoding='utf-8') as f:
        json.dump(mapping, f, ensure_ascii=False, indent=1)

    for k in ('class_node', 'class_line', 'layer', 'aspect', 'child', 'junk'):
        print('%-12s %d' % (k, len(mapping[k])))
    print('-> %s' % args.out)


if __name__ == '__main__':
    main()
