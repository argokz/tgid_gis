"""Профилирование расчёта по фазам.

Ядро печатает в stderr «Начал читать X» / «Закончил читать X», но без
времени. Обёртка проставляет метки и показывает, на что уходит время:
на чтение данных или на сам решатель.

    python tools/profile_calc.py --db tgid_gis --fragment 2
"""
import argparse
import os
import subprocess
import sys
import time

VENV = r'H:\venv\sety\Scripts\python.exe'
SETY = r'H:\projects\tgid-app-new\gid8\python\sety\sety\ww.py'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', required=True)
    ap.add_argument('--fragment', type=int, default=2)
    ap.add_argument('--tn', default='-25')
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', default=os.environ.get('TGID_PORT', '5440'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--venv', default=VENV)
    ap.add_argument('--sety', default=SETY)
    ap.add_argument('--top', type=int, default=15)
    args = ap.parse_args()

    cmd = [args.venv, args.sety,
           '-type_of_net', '1',
           '-server', args.host, '-database', args.db,
           '-user', args.user, '-port', str(args.port),
           '-fileID', str(args.fragment), '-Tn', args.tn,
           '-GWS', '1', '-GWS2', '1',
           '-sopr', '0', '-roP', '0.975', '-roO', '0.975', '-ro_temp',
           '-rdbms', 'postgreSQL',
           '-out_file', os.path.join(os.environ.get('TEMP', '.'),
                                     'profile_out.txt'),
           '-user_gid', 'profile']
    if args.password:
        cmd += ['-password', args.password]

    t0 = time.time()
    marks = []
    p = subprocess.Popen(cmd, stdout=subprocess.DEVNULL,
                         stderr=subprocess.PIPE, bufsize=1,
                         universal_newlines=True, encoding='utf-8',
                         errors='replace')
    for line in p.stderr:
        line = line.rstrip()
        if line.startswith(('Начал читать', 'Закончил читать')):
            marks.append((time.time() - t0, line))
    p.wait()
    total = time.time() - t0

    # Длительность чтения каждой таблицы: между «Начал» и «Закончил».
    spans = []
    open_at = {}
    for t, line in marks:
        if line.startswith('Начал читать'):
            open_at[line[len('Начал читать '):]] = t
        else:
            name = line[len('Закончил читать '):]
            if name in open_at:
                spans.append((t - open_at.pop(name), name))

    read_total = sum(d for d, _ in spans)
    first_read = marks[0][0] if marks else 0.0
    last_read = marks[-1][0] if marks else 0.0

    print('БД: %s, фрагмент %d' % (args.db, args.fragment))
    print('всего:                 %7.1f с' % total)
    print('до первого чтения:     %7.1f с' % first_read)
    print('фаза чтения:           %7.1f с  (сумма по таблицам %.1f с)'
          % (last_read - first_read, read_total))
    print('после чтения (расчёт): %7.1f с' % (total - last_read))
    print('\nсамые долгие чтения:')
    for d, name in sorted(spans, reverse=True)[:args.top]:
        print('  %7.1f с  %s' % (d, name))
    return p.returncode


if __name__ == '__main__':
    sys.exit(main())
