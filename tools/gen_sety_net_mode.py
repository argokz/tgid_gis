"""Генерация модуля sety/net_mode.py — нативное чтение из схемы net.

Ядро читает подтипы запросами вида «таблица-подтип JOIN nodes».
В схеме net это соединение с представлением из 10 ветвей и стоит дорого:
участки теплопровода читались 1841 мс против 418 мс при чтении напрямую
из объектной таблицы с соединением по реестру.

Модуль подставляет нативные запросы, если схема net присутствует,
и молчит, если её нет, — ядро продолжает работать на исходной БД.
"""
import argparse
import json
import os

HEADER = '''"""Нативное чтение схемы net — СГЕНЕРИРОВАНО tools/gen_sety_net_mode.py.

Подключается из read_gid.py. Если схемы net нет, is_net() возвращает False
и ядро работает прежними запросами к public.
"""

# Соответствие «таблица-подтип -> объектная таблица net»
NODE_CLASS = @NODE_MAP@

LINE_CLASS = @LINE_MAP@

_is_net = None


def is_net(conn):
    """Есть ли схема net. Результат кэшируется на процесс."""
    global _is_net
    if _is_net is None:
        cur = conn.cursor()
        cur.execute("SELECT count(*) FROM information_schema.tables "
                    "WHERE table_schema = 'net' AND table_name = 'node_reg'")
        _is_net = cur.fetchone()[0] > 0
        cur.close()
    return _is_net


def node_query(tn, cols, s_fileID):
    """Замена запроса read_node2.

    Объектная таблица net уже содержит колонки узла, поэтому соединение
    с nodes не нужно. Строки подтипов, не ставшие объектом (дубли и
    отнесённые к другому классу), лежат в extra_* и добавляются UNION —
    без них ядро увидит меньше объектов, чем на исходной БД.
    """
    cls = NODE_CLASS.get(tn)
    if not cls:
        return None
    own = cols.replace('o.', 'o.')
    return f"""
        SELECT o.subtype_src_id AS id, o.id AS nodeID,
               o.externalSignID, o.externalCodeID, o.externalNodeName,
               o.internalnodeid, {own}
        FROM net.{cls} o
        WHERE o.removed_at IS NULL
          AND o.fragment_id IN ({s_fileID})
          AND o.internalNodeID IS NULL
        UNION ALL
        SELECT x.id, x.obj_id,
               r.externalsignid, r.externalcodeid, r.externalnodename,
               r.internalnodeid, {cols.replace('o.', 'x.')}
        FROM net.extra_{tn} x
        JOIN net.node_reg r ON r.id = x.obj_id
        WHERE NOT r.removed
          AND r.fragment_id IN ({s_fileID})
          AND r.internalnodeid IS NULL
    """


def line_query(tn, cols, s_fileID):
    """Замена запроса read_line2.

    Соединение идёт с реестром net.node_reg — обычной таблицей с индексом,
    а не с представлением nodes из десяти ветвей.
    """
    cls = LINE_CLASS.get(tn)
    if not cls:
        return None
    return f"""
        SELECT o.subtype_src_id AS id, o.id AS lineID,
               o.node_from_src AS nodeID1, o.node_to_src AS nodeID2,
               o.externalSignLineID, r.internalNodeID, ec.heatSourceID,
               o.hydroRes, {cols}
        FROM net.{cls} o
        JOIN net.node_reg r ON r.id = o.node_from AND NOT r.removed
        JOIN externalCodes ec ON r.externalCodeID = ec.id
        LEFT JOIN net.node_reg r0 ON r0.id = r.internalNodeID AND NOT r0.removed
        LEFT JOIN net.consumer_real rc ON rc.id = r.internalNodeID
        WHERE o.removed_at IS NULL
          AND r.fragment_id IN ({s_fileID})
          AND (r0.id IS NULL OR NOT r0.removed)
          AND rc.id IS NULL
    """
'''


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mapping', required=True)
    ap.add_argument('--out', required=True)
    args = ap.parse_args()

    with open(args.mapping, encoding='utf-8') as f:
        m = json.load(f)

    node_map = {e['source']: e['target'] for e in m['class_node']}
    line_map = {e['source']: e['target'] for e in m['class_line']}

    def fmt(d):
        return '{\n' + ''.join("    '%s': '%s',\n" % kv
                               for kv in sorted(d.items())) + '}'

    # Простая подстановка, а не format: тело модуля содержит
    # f-строки с фигурными скобками, и format их ломает.
    src = (HEADER.replace('@NODE_MAP@', fmt(node_map))
                 .replace('@LINE_MAP@', fmt(line_map)))
    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    with open(args.out, 'w', encoding='utf-8') as f:
        f.write(src)
    print('-> %s (узловых классов %d, линейных %d)'
          % (args.out, len(node_map), len(line_map)))


if __name__ == '__main__':
    main()
