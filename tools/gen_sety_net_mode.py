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
    """Обслуживается ли БД схемой net. Результат кэшируется на процесс.

    Проверять существование схемы net недостаточно. В исходной БД после
    пробной конвертации схема net остаётся рядом с настоящими таблицами
    public, и данные в ней устаревают. Чтение из неё в таком состоянии
    даёт неполный результат БЕЗ ошибки: расчёт отрабатывает, пишет
    меньше строк и сообщает «во фрагменте нет источников тепла».

    Признак настоящего перевода — public.nodes стал представлением
    (применён 040_switch_to_net.sql). Пока это базовая таблица, данные
    живут в public, и читать надо оттуда.
    """
    global _is_net
    if _is_net is None:
        cur = conn.cursor()
        cur.execute("SELECT count(*) FROM information_schema.tables "
                    "WHERE table_schema = 'net' AND table_name = 'node_reg'")
        has_net = cur.fetchone()[0] > 0
        cur.execute("SELECT count(*) FROM information_schema.views "
                    "WHERE table_schema = 'public' AND table_name = 'nodes'")
        switched = cur.fetchone()[0] > 0
        cur.close()
        _is_net = has_net and switched
    return _is_net


def fragment_table(conn):
    """Имя таблицы фрагментов для подстановки в запрос.

    В net объект называется fragment (единственное число, как остальные
    классы), в старой схеме — fragments. Имена не совпадают, поэтому
    search_path тут не помогает и подстановка нужна явная.
    """
    return 'net.fragment' if is_net(conn) else 'fragments'


# Надтипы и подтипы, которые в net представлены готовыми view.
# Они живут В СХЕМЕ net и включают строки extra_* — то есть отдают ровно
# то же, что представления совместимости в public, ради которых их и
# создавали. Подстановка имени семантику не меняет.
COMPAT_VIEW = {
    'nodes': 'net.v_nodes',
    'linesobj': 'net.v_linesobj',
    'heatsources': 'net.v_heatsources',
    'realconsumers': 'net.v_realconsumers',
    'generalizedconsumers': 'net.v_generalizedconsumers',
    'connectnodes': 'net.v_connectnodes',
}


def tbl(conn, name):
    """Имя таблицы для подстановки в запрос.

    На переведённой БД возвращает объект из net, на старой — исходное
    имя. Нужна там, где запрос слишком переплетён, чтобы переписывать
    его целиком, но зависимость от public убрать надо.
    """
    if not is_net(conn):
        return name
    return COMPAT_VIEW.get(name.lower(), name)


def tbl_cached(name):
    """Как tbl, но без соединения — по уже определённому режиму.

    Нужна там, где запрос собирается функцией, у которой соединения нет:
    g2.write_po пишет текст UPDATE в файл, а выполняется он позже.

    Режим к этому моменту уже определён: is_net вызывается при чтении
    схемы, задолго до записи. Если всё же не определён, возвращается
    старое имя — оно разрешится в представление совместимости, пока то
    существует, а когда его удалят, запрос упадёт заметно. Это лучше
    молчаливой записи не туда.
    """
    if _is_net is None:
        return name
    return COMPAT_VIEW.get(name.lower(), name) if _is_net else name


def node_query(tn, cols, s_fileID):
    """Замена запроса read_node2.

    Объектная таблица net уже содержит колонки узла, поэтому соединение
    с nodes не нужно. Строки подтипов, не ставшие объектом (дубли и
    отнесённые к другому классу), лежат в extra_* и добавляются UNION —
    без них ядро увидит меньше объектов, чем на исходной БД.
    """
    # Ключи отображения в нижнем регистре, а ядро передаёт имя таблицы
    # как в исходнике — 'realConsumers', 'heatChambers'. Без приведения
    # регистра .get не находил класс, возвращал None, и чтение молча
    # уходило на старый запрос к представлениям совместимости.
    cls = NODE_CLASS.get(tn.lower())
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
    cls = LINE_CLASS.get(tn.lower())  # см. примечание в node_query
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


def pt_node_query(tn, cols, s_fileID):
    """Замена запросов read_pt_line и read_pt_vnutr.

    Тот же шаблон, что у node_query, но без условия
    internalNodeID IS NULL: пьезометрия читает и внутренние узлы.
    Полей пять, без internalnodeid — порядок обязан совпадать
    с распаковкой строки в вызывающем коде.
    """
    cls = NODE_CLASS.get(tn.lower())
    if not cls:
        return None
    return f"""
        SELECT o.subtype_src_id AS id, o.id AS nodeID,
               o.externalSignID, o.externalCodeID, o.externalNodeName,
               {cols}
        FROM net.{cls} o
        WHERE o.removed_at IS NULL
          AND o.fragment_id IN ({s_fileID})
        UNION ALL
        SELECT x.id, x.obj_id,
               r.externalsignid, r.externalcodeid, r.externalnodename,
               {cols.replace('o.', 'x.')}
        FROM net.extra_{tn} x
        JOIN net.node_reg r ON r.id = x.obj_id
        WHERE NOT r.removed
          AND r.fragment_id IN ({s_fileID})
    """


def zn0_query(fileID):
    """Замена запроса read_zn0 — источник для узлов с заданным напором.

    Второй по стоимости оператор чтения: два соединения с представлением
    nodes, одно из них внутри подзапроса. Оба переведены на реестр
    net.node_reg.

    Фильтр по removed НЕ добавлен намеренно: в исходном запросе его нет,
    и добавить его — значит изменить результат, а не ускорить запрос.

    LEFT JOIN fragments в подзапросе выбора последнего расчёта убран:
    колонки fr в его выборке не участвуют.
    """
    return f"""
select distinct
ec.id, max(usP.ist)

FROM net.node_press_setting zn

JOIN net.node_reg n ON n.id = zn.node_id
JOIN externalCodes ec ON ec.id = n.externalcodeid

JOIN (
    SELECT n0.id, ec0.name AS kod, n0.externalnodename AS name,
           n0.fragment_id AS fileID
    FROM net.node_reg n0
    JOIN externalCodes ec0 ON ec0.id = n0.externalcodeid
) n0 ON n0.kod = ec.name AND n0.name = n.externalnodename

LEFT JOIN (
    SELECT c.fileID, max(c.id) AS cid
    FROM CALCULATION c
    GROUP BY c.fileID
) calc ON calc.fileID = n0.fileid
JOIN US_OUT usP ON usP.nodeID = n0.id AND usP.externalSign = 1
                AND usP.calculationid = cid
WHERE n.fragment_id = {fileID}
GROUP BY ec.id
"""


def zn_query(s_fileID):
    """Замена запроса read_zn — узлы с заданным напором.

    Исходный запрос дважды соединяется с представлением nodes: один раз
    для самого узла, второй — внутри подзапроса результатов предыдущего
    расчёта. На переведённой БД nodes — представление над net из десяти
    ветвей, и оба соединения стоят дорого: 857 мс из 3.8 с фазы чтения.

    Реестр net.node_reg содержит те же поля (fragment_id вместо fileID)
    обычной таблицей с индексами.
    """
    return f"""
SELECT
n.id,
sp.pressflow, sp.pressret,
us2.pihP, us2.pihO,
us2.t1P, us2.t1O,
us2.t2P, us2.t2O,
ec.name AS kod, n.externalnodename AS name,
us2.id AS id_m,
sp.kod_m, sp.uzel_m,
fr.name AS fr_name,
us2.ist,
sp.fragment_resultid

FROM net.node_press_setting sp
JOIN net.node_reg n ON n.id = sp.node_id AND NOT n.removed
JOIN externalCodes ec ON ec.id = n.externalcodeid

LEFT JOIN
(
    SELECT
    n2.id,
    n2.fragment_id AS fileID,
    ec2.name AS kod, n2.externalnodename AS name,
    usP.pih AS pihP, usP.t AS t1P, usP.t2 AS t2P,
    usO.pih AS pihO, usO.t AS t1O, usO.t2 AS t2O,
    usP.ist

    FROM net.node_reg n2
    JOIN externalCodes ec2 ON ec2.id = n2.externalcodeid
    JOIN US_OUT usP ON usP.nodeID = n2.id AND usP.externalSign = 1
    JOIN US_OUT usO ON usO.nodeID = n2.id AND usO.externalSign = 2

    JOIN
    (
        -- LEFT JOIN fragments здесь был и ничего не давал: колонки
        -- fr в выборке подзапроса не участвуют. Убран заодно с
        -- переводом на net — иначе тянул бы public.fragments.
        SELECT c.fileID, max(c.id) AS cid
        FROM CALCULATION c
        GROUP BY c.fileID
    ) calc ON n2.fragment_id = calc.fileID
          AND usP.calculationID = calc.cid AND usO.calculationID = calc.cid

) us2 ON us2.kod = sp.kod_m AND us2.name = sp.uzel_m
     AND us2.fileID = sp.fragment_resultid
LEFT JOIN net.fragment fr ON fr.id = sp.fragment_resultid

WHERE n.fragment_id IN ({s_fileID})
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
