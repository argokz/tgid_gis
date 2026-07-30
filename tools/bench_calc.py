"""Повторяемый замер расчёта: несколько прогонов, минимум и медиана.

Одиночный замер на рабочей машине непригоден для оптимизации: один и тот
же неизменённый код давал 16.65, 18.79 и 21.51 секунды. Разброс оказался
больше эффекта, который искали, и по одиночным цифрам делались неверные
выводы «стало хуже».

Берём минимум — он ближе всего к времени без посторонней нагрузки,
и медиану как проверку стабильности.

    python tools/bench_calc.py --db tgid_gis --fragment 2 --runs 3
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


def one_run(args):
    cmd = [args.venv, args.sety,
           '-type_of_net', '1',
           '-server', args.host, '-database', args.db,
           '-user', args.user, '-port', str(args.port),
           '-fileID', str(args.fragment), '-Tn', args.tn,
           '-GWS', '1', '-GWS2', '1',
           '-sopr', '0', '-roP', '0.975', '-roO', '0.975', '-ro_temp',
           '-rdbms', 'postgreSQL',
           '-out_file', os.path.join(os.environ.get('TEMP', '.'), 'bench.txt'),
           '-user_gid', 'bench']
    if args.password:
        cmd += ['-password', args.password]

    t0 = time.time()
    p = subprocess.run(cmd, capture_output=True, text=True,
                       encoding='utf-8', errors='replace')
    wall = time.time() - t0
    if p.returncode != 0:
        print('ПРОВАЛ, код %d' % p.returncode)
        tail = (p.stderr or p.stdout or '').strip().splitlines()[-5:]
        print('\n'.join(tail))
        return None, None

    m = RE_TIME.search(p.stdout or '')
    inner = float(m.group(1)) if m else None
    return wall, inner


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', required=True)
    ap.add_argument('--fragment', type=int, default=2)
    ap.add_argument('--tn', default='-25')
    ap.add_argument('--runs', type=int, default=3)
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', default=os.environ.get('TGID_PORT', '5440'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--venv', default=VENV)
    ap.add_argument('--sety', default=SETY)
    ap.add_argument('--label', default='')
    args = ap.parse_args()

    walls, inners = [], []
    for i in range(args.runs):
        w, inner = one_run(args)
        if w is None:
            return 1
        walls.append(w)
        if inner:
            inners.append(inner)
        print('  прогон %d: %.1f с (ядро сообщает %.1f с)'
              % (i + 1, w, inner or 0))

    print('\n%s%s, фрагмент %d, прогонов %d'
          % (args.label + ': ' if args.label else '', args.db,
             args.fragment, args.runs))
    print('  минимум:  %6.1f с' % min(walls))
    print('  медиана:  %6.1f с' % statistics.median(walls))
    print('  максимум: %6.1f с' % max(walls))
    print('  разброс:  %6.1f с (%.0f %% от минимума)'
          % (max(walls) - min(walls),
             100.0 * (max(walls) - min(walls)) / min(walls)))
    if inners:
        print('  минимум по счётчику ядра: %.1f с' % min(inners))
    return 0


if __name__ == '__main__':
    sys.exit(main())
