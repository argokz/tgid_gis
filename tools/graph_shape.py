"""Размеры подсетей расчётной схемы.

Решатель обрабатывает слабо связные компоненты графа по очереди.
Выгодно ли считать их параллельно, зависит от распределения размеров:
если время съедает одна крупная компонента, параллельность не поможет
(закон Амдала).

    H:\\venv\\sety\\Scripts\\python.exe tools/graph_shape.py --db tgid_gis --fragment 2
"""
import argparse
import os
import sys

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
        '-out_file', os.path.join(os.environ.get('TEMP', '.'), 'shape.txt'),
        '-user_gid', 'shape',
    ]
    if args.password:
        sys.argv += ['-password', args.password]

    import networkx as nx
    from sety import config, read_gid
    from sety.any import connect
    from sety.sprav import read_sprav
    from sety import w

    cfg = config.init()
    w.open_b5()
    db = dict(rdbms=cfg.rdbms, server=cfg.server, user=cfg.user,
              password=cfg.password, db=cfg.database, port=cfg.port,
              files={cfg.fileID})
    conn = connect.connect(**db)
    read_sprav(db)
    G = read_gid.read_gid(conn, {cfg.fileID}, cfg.database)

    comps = [len(c) for c in nx.weakly_connected_components(G)]
    comps = sorted((c for c in comps if c > 1), reverse=True)

    total = sum(comps)
    print('\nвсего компонент (больше одного узла): %d' % len(comps))
    print('узлов в них: %d' % total)
    print('\nсамые крупные:')
    for i, c in enumerate(comps[:10], 1):
        print('  %2d. %6d узлов  (%.1f %% всех)' % (i, c, 100.0 * c / total))

    top1 = comps[0] if comps else 0
    top4 = sum(comps[:4])
    print('\nдоля крупнейшей компоненты: %.1f %%' % (100.0 * top1 / total))
    print('доля четырёх крупнейших:    %.1f %%' % (100.0 * top4 / total))
    print('\nЕсли крупнейшая занимает больше половины, параллельный счёт')
    print('подсетей упрётся в неё и заметного ускорения не даст.')


if __name__ == '__main__':
    main()
