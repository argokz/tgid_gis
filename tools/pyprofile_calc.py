"""Профиль расчёта на уровне Python: где именно уходит время.

Профилирование по фазам (profile_calc.py) показало только «чтение» против
«решателя». Чтобы ускорять, нужно знать конкретные функции.

    H:\\venv\\sety\\Scripts\\python.exe tools/pyprofile_calc.py --db tgid_gis --fragment 2

Запускать интерпретатором из окружения sety: модуль импортируется в процесс.
"""
import argparse
import cProfile
import io
import os
import pstats
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
    ap.add_argument('--top', type=int, default=30)
    ap.add_argument('--sety', default=SETY_DIR)
    ap.add_argument('--out', default='')
    args = ap.parse_args()

    sys.path.insert(0, args.sety)
    sys.path.insert(0, os.path.join(args.sety, 'sety'))

    # Аргументы ядру передаются через argv — так же, как из приложения.
    sys.argv = [
        'ww.py',
        '-type_of_net', '1',
        '-server', args.host, '-database', args.db,
        '-user', args.user, '-port', str(args.port),
        '-fileID', str(args.fragment), '-Tn', args.tn,
        '-GWS', '1', '-GWS2', '1',
        '-sopr', '0', '-roP', '0.975', '-roO', '0.975', '-ro_temp',
        '-rdbms', 'postgreSQL',
        '-out_file', os.path.join(os.environ.get('TEMP', '.'), 'pyprof.txt'),
        '-user_gid', 'pyprofile',
    ]
    if args.password:
        sys.argv += ['-password', args.password]

    from sety import config, w

    cfg = config.init()
    pr = cProfile.Profile()
    pr.enable()
    try:
        w.run(rdbms=cfg.rdbms, server=cfg.server, user=cfg.user,
              password=cfg.password, db=cfg.database, port=cfg.port,
              files={cfg.fileID})
    except SystemExit:
        pass
    finally:
        pr.disable()

    s = io.StringIO()
    ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
    ps.print_stats(args.top)
    out = s.getvalue()

    # Печатаем только содержательную часть, без длинной шапки.
    lines = out.splitlines()
    start = next((i for i, l in enumerate(lines)
                  if l.strip().startswith('ncalls')), 0)
    print('\n'.join(lines[max(0, start - 2):start + args.top + 2]))

    print('\n--- по собственному времени (tottime) ---')
    s2 = io.StringIO()
    pstats.Stats(pr, stream=s2).sort_stats('tottime').print_stats(20)
    l2 = s2.getvalue().splitlines()
    st2 = next((i for i, l in enumerate(l2)
                if l.strip().startswith('ncalls')), 0)
    print('\n'.join(l2[st2:st2 + 22]))

    if args.out:
        pr.dump_stats(args.out)
        print('\nпрофиль сохранён:', args.out)


if __name__ == '__main__':
    main()
