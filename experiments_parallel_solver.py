"""Параллельный счёт подсетей.

Схема разбивается на слабо связные компоненты, которые решаются независимо.
Замер показал, что их 403, крупнейшая занимает 2.8 % узлов — работа
распределена ровно, значит распараллеливание не упрётся в одну компоненту.

Цена подготовки данных измерена заранее (tools/probe_parallel.py):
копирование 0.40 с, сериализация 0.09 с, разбор 0.11 с на все 403 подсети.
Это мало по сравнению с 8.6 с, которые занимает решатель.

Включается переменной окружения SETY_PARALLEL=<число процессов>.
По умолчанию выключено: последовательный путь остаётся основным, пока
параллельный не подтверждён регрессией на нескольких фрагментах.

Особенность Windows: процессы создаются через spawn, поэтому каждый
рабочий заново импортирует модули и поднимает справочники. Пул создаётся
один раз на расчёт, а не на подсеть.
"""
import os
import sys

_DB = None
_ARGV = None


def workers():
    """Сколько процессов запрашивает пользователь. 0 или 1 — выключено."""
    try:
        n = int(os.environ.get('SETY_PARALLEL', '0'))
    except ValueError:
        return 0
    if n < 0:
        return 0
    return n


def _init(argv, db, sety_dir):
    """Инициализация рабочего процесса.

    Решатель опирается на состояние уровня модулей: разобранные аргументы,
    справочники и таблицы b5. В spawn-процессе ничего этого нет, поэтому
    поднимаем заново — один раз на процесс, а не на подсеть.
    """
    global _DB
    if sety_dir not in sys.path:
        sys.path.insert(0, sety_dir)

    sys.argv = list(argv)

    from sety import config, w
    from sety.sprav import read_sprav

    config.init()
    w.open_b5()
    read_sprav(db)
    _DB = db


def _solve(payload):
    """Счёт одной подсети. Возвращает посчитанные атрибуты, а не граф.

    Обратно нужны только значения на узлах и рёбрах: сам подграф
    в родительском процессе уже есть.
    """
    import pickle

    from sety import w_calc, w_print

    GG = pickle.loads(payload)

    # node_name читает глобальный граф; в рабочем процессе его роль
    # играет подсеть. Влияет только на текст сообщений.
    w_print.setG(GG)

    param_sum = {}
    w_calc.w_calc2(GG, param_sum)

    nodes = {n: dict(d) for n, d in GG.nodes(data=True)}
    edges = {(u, v, k): dict(d) for u, v, k, d in GG.edges(keys=True, data=True)}
    return nodes, edges, param_sum


def solve_components(G, components, db, param_sum, n_workers, log=None):
    """Посчитать подсети параллельно и перенести результаты в общий граф.

    components — список наборов узлов в том же порядке, в каком их
    обрабатывает последовательный путь. Результаты применяются в этом же
    порядке, чтобы поведение не зависело от того, кто закончил первым.
    """
    import pickle
    from concurrent.futures import ProcessPoolExecutor

    sety_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

    payloads = []
    for nodes in components:
        payloads.append(
            pickle.dumps(G.subgraph(nodes).copy(),
                         protocol=pickle.HIGHEST_PROTOCOL))

    if log:
        log('параллельный счёт: %d подсетей, %d процессов'
            % (len(payloads), n_workers))

    with ProcessPoolExecutor(max_workers=n_workers,
                             initializer=_init,
                             initargs=(sys.argv, db, sety_dir)) as ex:
        results = list(ex.map(_solve, payloads))

    for nodes_attr, edges_attr, ps in results:
        for n, d in nodes_attr.items():
            if n in G:
                G.nodes[n].update(d)
        for (u, v, k), d in edges_attr.items():
            if G.has_edge(u, v, k):
                G.edges[u, v, k].update(d)
        for key, val in ps.items():
            param_sum[key] = val

    return len(results)
