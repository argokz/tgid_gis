"""Парный замер двух вариантов расчёта: A и B чередуются.

Машина шумит: одни и те же неизменённые файлы давали от 15.2 до 26.7
секунды в зависимости от посторонней нагрузки. Замеры, разнесённые
во времени, несравнимы — вариант, измеренный в тихую минуту, выигрывает
у самого себя, измеренного в шумную.

Чередование A-B-A-B выравнивает влияние дрейфа: оба варианта видят
примерно одинаковые условия. Сравниваем по минимуму и по медиане
попарных разностей.

    python tools/ab_calc.py --db tgid_gis --fragment 2 --pairs 4 \\
        --var A SETY_SUBGRAPH_COPY=0 --var B SETY_SUBGRAPH_COPY=1
"""
import argparse
import os
import re
import statistics
import subprocess
import sys
import time

VENV = r'H:\venv\sety\Scripts\python.exe'
SETY = r'H:\projects\tgid-app-new\gid8\python\sety\sety\ww.py'
RE_TIME = re.compile(r'Время расчета\s+([\d.]+)')


def run_once(args, env_extra):
    cmd = [args.venv, args.sety,
           '-type_of_net', '1',
           '-server', args.host, '-database', args.db,
           '-user', args.user, '-port', str(args.port),
           '-fileID', str(args.fragment), '-Tn', args.tn,
           '-GWS', '1', '-GWS2', '1',
           '-sopr', '0', '-roP', '0.975', '-roO', '0.975', '-ro_temp',
           '-rdbms', 'postgreSQL',
           '-out_file', os.path.join(os.environ.get('TEMP', '.'), 'ab.txt'),
           '-user_gid', 'ab']
    if args.password:
        cmd += ['-password', args.password]

    env = dict(os.environ)
    env.update(env_extra)

    t0 = time.time()
    p = subprocess.run(cmd, capture_output=True, text=True,
                       encoding='utf-8', errors='replace', env=env)
    wall = time.time() - t0
    if p.returncode != 0:
        print('ПРОВАЛ, код %d' % p.returncode)
        print('\n'.join((p.stderr or p.stdout or '').splitlines()[-5:]))
        return None
    m = RE_TIME.search(p.stdout or '')
    return wall, (float(m.group(1)) if m else None)


def parse_var(spec):
    name, _, rest = spec.partition(' ')
    env = {}
    for kv in rest.split():
        k, _, v = kv.partition('=')
        env[k] = v
    return name, env


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', required=True)
    ap.add_argument('--fragment', type=int, default=2)
    ap.add_argument('--tn', default='-25')
    ap.add_argument('--pairs', type=int, default=4)
    ap.add_argument('--var', action='append', required=True,
                    help='"ИМЯ КЛЮЧ=ЗНАЧЕНИЕ ..." — вариант окружения')
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', default=os.environ.get('TGID_PORT', '5440'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--venv', default=VENV)
    ap.add_argument('--sety', default=SETY)
    args = ap.parse_args()

    variants = [parse_var(v) for v in args.var]
    times = {name: [] for name, _ in variants}
    pairs = []

    for i in range(args.pairs):
        row = {}
        for name, env in variants:
            r = run_once(args, env)
            if r is None:
                return 1
            wall, inner = r
            times[name].append(wall)
            row[name] = wall
            print('  пара %d, вариант %s: %.1f с' % (i + 1, name, wall))
        pairs.append(row)

    print('\nфрагмент %d, пар %d' % (args.fragment, args.pairs))
    print('%-6s %8s %8s %8s' % ('вариант', 'мин', 'медиана', 'макс'))
    for name, _ in variants:
        t = times[name]
        print('%-6s %8.1f %8.1f %8.1f'
              % (name, min(t), statistics.median(t), max(t)))

    if len(variants) == 2:
        a, b = variants[0][0], variants[1][0]
        diffs = [row[a] - row[b] for row in pairs]
        med = statistics.median(diffs)
        base = statistics.median(times[a])
        print('\nпопарная разность %s − %s по каждой паре: %s'
              % (a, b, ', '.join('%.1f' % d for d in diffs)))
        print('медиана разности: %+.1f с (%+.0f %% от %s)'
              % (med, 100.0 * med / base, a))
        if all(d > 0 for d in diffs):
            print('вариант %s быстрее во ВСЕХ парах' % b)
        elif all(d < 0 for d in diffs):
            print('вариант %s быстрее во ВСЕХ парах' % a)
        else:
            print('знак разности непостоянен — эффект не отличим от шума')
    return 0


if __name__ == '__main__':
    sys.exit(main())
