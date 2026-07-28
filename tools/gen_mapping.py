"""Генерация converter/mapping.json — карты «исходная таблица → целевая таблица».

Имена целевых таблиц заданы вручную (TARGET_NAMES), состав колонок берётся
из выгрузки схемы, поэтому карта не расходится с реальной БД.
"""
import argparse
import json
import os
from collections import defaultdict

# Точечные объекты: исходная таблица-подтип -> целевое имя
NODE_TYPES = {
    'generalizedconsumers': 'consumer_general',
    'realconsumers': 'consumer_real',
    'ugol_povorota_truboprovoda': 'pipe_turn',
    'lyuki': 'manhole',
    'setpressnodes': 'press_set_node',
    'vvod_v_zdanie': 'building_inlet',
    'opora': 'support',
    'wdodevices': 'metering_device',
    'deployeddirections': 'direction_deployed',
    'connectnodes': 'connect_node',
    'pressregulators': 'regulator_press',
    'pumpstations': 'pump_station',
    'heatsources': 'heat_source',
    'heatchambers': 'heat_chamber',
    'threewayvalves': 'valve_3way',
    'pressdropregulators': 'regulator_pressdrop',
    'consumptregulators': 'regulator_flow',
    'kapremont_uchastki_remonta': 'repair_section',
    'perehod_diametra': 'diameter_change',
    'opressovka_defekt': 'pressure_test_defect',
    'refillnodes': 'refill_node',
}

# Линейные объекты
LINE_TYPES = {
    'heatpipesections': 'pipe_section',
    'diaphragms': 'diaphragm',
    'dampers': 'damper',
    'vvody_v_zdanie': 'building_inlet_line',
    'elevators': 'elevator',
    'systemradiators': 'radiator',
    'pumps': 'pump',
    'heatexchangers': 'heat_exchanger',
    'airheaters': 'air_heater',
    'localhydroresistances2': 'local_resistance',
}

# Полигональные слои — переносятся как есть, геометрия уже в shape
POLY_TYPES = {
    'zdaniya_2': 'building',
    'zdaniya_potrebiteley': 'building_consumer',
    'zdaniya_tu': 'building_tu',
    'truby_rekonstruiruemye': 'pipe_reconstructed',
}

# Колонки базовых таблиц, которые не переносятся: заменены геометрией,
# реестром или сменой механизма (removed -> removed_at).
BASE_DROP = {
    'nodes': {'x', 'y', 'shape', 'removed', 'idremoved', 'coords',
              'globalid', 'gistable', 'sync', 'gis', 'sync_tgid', 'id_old'},
    'linesobj': {'coords', 'shape', 'removed', 'idremoved',
                 'globalid', 'gistable', 'sync', 'gis', 'sync_tgid', 'id_old'},
}

# Колонки подтипов, которые не переносятся: ссылка ушла в id/реестр.
SUB_DROP = {'id', 'nodeid', 'lineid'}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--schema', required=True)
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

    def entry(src, target, geom_kind, link):
        keep = [c for c in cols[src] if c not in SUB_DROP]
        return {
            'source': src,
            'target': target,
            'geometry': geom_kind,
            'link_column': link,
            'rows': counts.get(src, 0),
            'columns': keep,
            'column_types': {c: {
                'data_type': types[src][c]['data_type'],
                'max_length': types[src][c]['character_maximum_length'],
                'nullable': types[src][c]['is_nullable'] == 'YES',
            } for c in keep},
        }

    mapping = {
        'srid': 9998,
        'coord_scale': 100.0,
        'flip_y': True,
        'base': {
            'nodes': {
                'columns': [c for c in cols['nodes']
                            if c not in BASE_DROP['nodes']],
            },
            'linesobj': {
                'columns': [c for c in cols['linesobj']
                            if c not in BASE_DROP['linesobj']],
            },
        },
        'node_types': [entry(s, t, 'Point', 'nodeid')
                       for s, t in sorted(NODE_TYPES.items())],
        'line_types': [entry(s, t, 'LineString', 'lineid')
                       for s, t in sorted(LINE_TYPES.items())],
        'polygon_types': [entry(s, t, 'Geometry', None)
                          for s, t in sorted(POLY_TYPES.items())],
    }

    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    with open(args.out, 'w', encoding='utf-8') as f:
        json.dump(mapping, f, ensure_ascii=False, indent=1)

    print('node_types=%d line_types=%d polygon_types=%d -> %s' % (
        len(mapping['node_types']), len(mapping['line_types']),
        len(mapping['polygon_types']), args.out))


if __name__ == '__main__':
    main()
