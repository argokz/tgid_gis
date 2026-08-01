"""Потолок выигрыша от замены networkx на массивы.

Переписывание ядра — это 374 места в 34 файлах. Прежде чем за это браться,
надо знать верхнюю границу ускорения. Здесь одни и те же действия
выполняются двумя способами на РЕАЛЬНОМ графе:

  A. как сейчас — обход networkx и чтение атрибутов из словарей;
  B. как было бы — те же данные в массивах, доступ по индексу.

Измеряется именно то, что заменяется: обход рёбер и чтение атрибутов.
Числовая часть (сборка матрицы, решение) в обоих случаях одинакова
и в замер не входит — иначе потолок получился бы завышенным.

    H:\\venv\\sety\\Scripts\\python.exe tools/probe_arrays.py --db tgid_gis --fragment 2
"""
import argparse
import os
import sys
import time

SETY_DIR = r'H:\projects\tgid-app-new\gid8\python\sety'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', required=True)
    ap.add_argument('--fragment', type=int, default=2)
    ap.add_argument('--tn', default='-25')
    ap.add_argument('--repeat', type=int, default=5,
                    help='сколько раз повторить каждый вариант')
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
        '-out_file', os.path.join(os.environ.get('TEMP', '.'), 'arr.txt'),
        '-user_gid', 'probe',
    ]
    if args.password:
        sys.argv += ['-password', args.password]

    import numpy as np
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
    subs = [G.subgraph(c) for c in comps]
    print('подсетей: %d, узлов: %d, рёбер: %d'
          % (len(subs), G.number_of_nodes(), G.number_of_edges()))

    # ---------- A: как сейчас ----------
    def pass_networkx():
        total = 0.0
        for GG in subs:
            for n1, n2, key, orient in nx.edge_dfs(GG, orientation="ignore"):
                e = GG.edges[n1, n2, key]
                a = GG.nodes[n1]
                b = GG.nodes[n2]
                total += (e.get('S', 0.0) or 0.0)
                total += (a.get('geoMarkTopTube', 0.0) or 0.0)
                total += (b.get('geoMarkTopTube', 0.0) or 0.0)
        return total

    # ---------- подготовка массивов ----------
    t0 = time.time()
    packs = []
    for GG in subs:
        idx = {n: i for i, n in enumerate(GG.nodes)}
        order = list(nx.edge_dfs(GG, orientation="ignore"))
        u = np.fromiter((idx[a] for a, b, k, o in order), dtype=np.int32,
                        count=len(order))
        v = np.fromiter((idx[b] for a, b, k, o in order), dtype=np.int32,
                        count=len(order))
        s = np.fromiter(((GG.edges[a, b, k].get('S', 0.0) or 0.0)
                         for a, b, k, o in order),
                        dtype=np.float64, count=len(order))
        geo = np.fromiter(((GG.nodes[n].get('geoMarkTopTube', 0.0) or 0.0)
                           for n in GG.nodes), dtype=np.float64,
                          count=GG.number_of_nodes())
        packs.append((u, v, s, geo))
    t_prep = time.time() - t0

    # ---------- B: как было бы ----------
    def pass_arrays():
        total = 0.0
        for u, v, s, geo in packs:
            total += float(s.sum())
            total += float(geo[u].sum())
            total += float(geo[v].sum())
        return total

    def best(fn, n):
        b = 9e9
        for _ in range(n):
            t = time.time()
            fn()
            b = min(b, time.time() - t)
        return b

    ta = best(pass_networkx, args.repeat)
    tb = best(pass_arrays, args.repeat)

    print('\nодин полный проход по всем подсетям:')
    print('  A. networkx, словари: %8.3f с' % ta)
    print('  B. массивы, индексы:  %8.3f с' % tb)
    print('  отношение:            %8.1fx' % (ta / tb if tb else 0))
    print('\nподготовка массивов (разовая, из того же networkx): %.3f с' % t_prep)
    print('\nКак читать: отношение — это ПОТОЛОК для той части кода,')
    print('которая занимается обходом и чтением атрибутов. Общий выигрыш')
    print('будет меньше: числовая часть и работа с БД не ускорятся.')


if __name__ == '__main__':
    main()
