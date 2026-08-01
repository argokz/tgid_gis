"""Проверка выполнимости параллельного счёта подсетей.

Параллельность требует отправить каждую подсеть в рабочий процесс:
скопировать подграф и сериализовать его. Если только это стоит заметную
долю времени расчёта, выигрыш съедается ещё до старта — ровно так вышло
с материализацией подсетей.

Замеряем цену подготовки данных ДО того, как писать параллельный код.

    H:\\venv\\sety\\Scripts\\python.exe tools/probe_parallel.py --db tgid_gis --fragment 2
"""
import argparse
import os
import pickle
import sys
import time

SETY_DIR = r'H:\projects\tgid-app-new\gid8\python\sety'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', required=True)
    ap.add_argument('--fragment', type=int, default=2)
    ap.add_argument('--tn', default='-25')
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', default=os.environ.get('TGID_PORT', '5440'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--sety', default=SETY_DIR)
    args = ap.parse_args()

    sys.path.insert(0, args.sety)
    sys.path.insert(0, os.path.join(args.sety, 'sety'))
    sys.argv = [
        'ww.py', '-type_of_net', '1',
        '-server', args.host, '-database', args.db,
        '-user', args.user, '-port', str(args.port),
        '-fileID', str(args.fragment), '-Tn', args.tn,
        '-GWS', '1', '-GWS2', '1',
        '-sopr', '0', '-roP', '0.975', '-roO', '0.975', '-ro_temp',
        '-rdbms', 'postgreSQL',
        '-out_file', os.path.join(os.environ.get('TEMP', '.'), 'probe.txt'),
        '-user_gid', 'probe',
    ]
    if args.password:
        sys.argv += ['-password', args.password]

    import networkx as nx
    from sety import config, read_gid, w
    from sety.any import connect
    from sety.sprav import read_sprav

    cfg = config.init()
    w.open_b5()
    db = dict(rdbms=cfg.rdbms, server=cfg.server, user=cfg.user,
              password=cfg.password, db=cfg.database, port=cfg.port,
              files={cfg.fileID})
    conn = connect.connect(**db)
    read_sprav(db)
    G = read_gid.read_gid(conn, {cfg.fileID}, cfg.database)

    comps = [c for c in nx.weakly_connected_components(G) if len(c) > 1]
    print('подсетей: %d' % len(comps))

    t0 = time.time()
    views = [G.subgraph(c) for c in comps]
    t_view = time.time() - t0

    t0 = time.time()
    copies = [v.copy() for v in views]
    t_copy = time.time() - t0

    t0 = time.time()
    blobs = [pickle.dumps(c, protocol=pickle.HIGHEST_PROTOCOL) for c in copies]
    t_pickle = time.time() - t0
    size = sum(len(b) for b in blobs)

    t0 = time.time()
    for b in blobs:
        pickle.loads(b)
    t_unpickle = time.time() - t0

    print('\nцена подготовки данных для рабочих процессов:')
    print('  создание видов:      %6.2f с' % t_view)
    print('  копирование:         %6.2f с' % t_copy)
    print('  сериализация:        %6.2f с  (%.1f МБ)'
          % (t_pickle, size / 1048576.0))
    print('  десериализация:      %6.2f с' % t_unpickle)
    print('  ИТОГО накладных:     %6.2f с' % (t_copy + t_pickle + t_unpickle))
    print('\nСравните с временем расчёта: если накладные расходы близки')
    print('к доле решателя, делённой на число ядер, — выигрыша не будет.')
    print('Ядер в системе: %d' % (os.cpu_count() or 1))


if __name__ == '__main__':
    main()
