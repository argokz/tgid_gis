"""Подстановка имён compat -> net.v_* в SQL внутри исходников gid8.

Заменяются только имена в позиции таблицы (после FROM/JOIN/UPDATE/INTO),
чтобы не задеть одноимённые переменные и поля C++. Комментарии и блоки
`#if 0` пропускаются: в GidWidget.cpp внутри `#if 0` лежит отключённый
DELETE по heatpipesections, править его незачем и вредно — правка
создала бы впечатление живого кода.

Переводы строк сохраняются побайтно (newline=''). Дважды в этом проекте
перезапись файла с нормализацией CRLF->LF превращала правку в трёх
строках в diff на тысячи строк и прятала суть изменения.

    python tools/subst_menu_names.py --dry
    python tools/subst_menu_names.py
"""
import argparse
import io
import os
import re
import sys

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GID8 = os.path.join(BASE, 'gid8', 'gid8')

MAP = {
    'heatpipesections': 'net.v_heatpipesections',
    'linesobj': 'net.v_linesobj',
    'nodes': 'net.v_nodes',
    'refillnodes': 'net.v_refillnodes',
    'localhydroresistances2': 'net.v_localhydroresistances2',
}

RE_HIT = re.compile(
    r'\b(from|join|update|into)(\s+)(%s)\b' % '|'.join(MAP),
    re.I)


def dead_ranges(text):
    """Строки внутри `#if 0 ... #endif` — их не трогаем."""
    dead, depth = set(), 0
    for i, line in enumerate(text.splitlines()):
        s = line.strip()
        if re.match(r'#if\s+0\b', s):
            depth += 1
        if depth:
            dead.add(i)
        if depth and s.startswith('#endif'):
            depth -= 1
    return dead


def process(path, dry):
    raw = io.open(path, encoding='utf-8', errors='replace', newline='').read()
    dead = dead_ranges(raw)
    lines = raw.splitlines(keepends=True)
    changed = []
    for i, line in enumerate(lines):
        if i in dead:
            continue
        if line.lstrip().startswith('//'):
            continue

        def rep(m):
            kw, gap, name = m.group(1), m.group(2), m.group(3)
            return '%s%s%s' % (kw, gap, MAP[name.lower()])

        new = RE_HIT.sub(rep, line)
        if new != line:
            changed.append((i + 1, line.strip(), new.strip()))
            lines[i] = new
    if changed and not dry:
        io.open(path, 'w', encoding='utf-8', newline='').write(''.join(lines))
    return changed


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--dry', action='store_true')
    args = ap.parse_args()

    total = 0
    for dp, dn, fs in os.walk(GID8):
        for fn in sorted(fs):
            if not fn.endswith(('.cpp', '.h')):
                continue
            p = os.path.join(dp, fn)
            ch = process(p, args.dry)
            if ch:
                print('%s  (%d)' % (os.path.relpath(p, BASE), len(ch)))
                for ln, a, b in ch:
                    print('  %5d - %s' % (ln, a[:100]))
                    print('        + %s' % b[:100])
                total += len(ch)
    print('\n%s замен: %d' % ('НЕ применено (--dry),' if args.dry else 'Применено', total))
    return 0


if __name__ == '__main__':
    sys.stdout.reconfigure(encoding='utf-8')
    sys.exit(main())
