"""Разложение public.fragments на три таблицы net.

В public.fragments 156 колонок, из них 132 — массивы, выписанные в
плоскую строку: 12 параметров × 5 систем теплоснабжения и 6 параметров
× 12 месяцев. Обоснование разложения — docs/15.

Скрипт не знает состав колонок заранее: он читает его из БД, сам делит
на три группы и печатает DDL вместе с переносом данных. Руками такой
файл не написать без опечаток, а опечатка в имени вроде tpod_fakt_11
молча потеряет столбец данных.

    python tools/gen_fragment_split.py --db tgid_gis --out sql/150_fragment_split.sql
"""
import argparse
import io
import os
import re
import sys

import psycopg2

# Группы, разложенные по месяцам 1..12. Проверяются ПЕРВЫМИ: имя
# tn_long_1 подходит и под месячную группу, и под «что-то с суффиксом 1».
MONTH_GROUPS = ['tn_long', 'tgr_long', 'tpod_long',
                'tn_fakt', 'tgr_fakt', 'tpod_fakt']
# Группы, разложенные по системам теплоснабжения 1..5.
SYSTEM_GROUPS = ['tvn', 'g1n_sys', 'g2n_sys', 'gnz_sys',
                 'qn_pot', 'qn_gv_pot', 'q1n_tp', 'q2n_tp',
                 't1n_pot', 't2n_pot', 't1n_sys', 't2n_sys']
# Колонки, которые в net.fragment не переносятся: у объекта net свои
# признаки удаления и свой идентификатор.
SKIP = {'removed', 'idremoved'}


def split_columns(cols):
    month, system, scalar = {}, {}, []
    names = {c[0] for c in cols}
    for name, dtype in cols:
        if name in SKIP:
            continue
        m = re.match(r'^(.*)_(\d+)$', name)
        if m:
            base, idx = m.group(1), int(m.group(2))
            if base in MONTH_GROUPS and 1 <= idx <= 12:
                month.setdefault(base, {})[idx] = (name, dtype)
                continue
            if base in SYSTEM_GROUPS and 1 <= idx <= 5:
                system.setdefault(base, {})[idx] = (name, dtype)
                continue
        scalar.append((name, dtype))
    return scalar, system, month


def check_complete(groups, expect, kind):
    """Группа обязана быть полной: пропуск означает ошибку разбора."""
    bad = []
    for base, items in sorted(groups.items()):
        missing = [i for i in range(1, expect + 1) if i not in items]
        if missing:
            bad.append('%s: нет %s' % (base, missing))
    if bad:
        raise SystemExit('неполные группы по %s:\n  %s'
                         % (kind, '\n  '.join(bad)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', required=True)
    ap.add_argument('--out', required=True)
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', default=os.environ.get('TGID_PORT', '5440'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    args = ap.parse_args()

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()
    cur.execute("""
        SELECT column_name,
               CASE WHEN data_type = 'character varying'
                    THEN 'text' ELSE data_type END
        FROM information_schema.columns
        WHERE table_schema = 'public' AND table_name = 'fragments'
        ORDER BY ordinal_position""")
    cols = cur.fetchall()
    cur.execute("""
        SELECT column_name FROM information_schema.columns
        WHERE table_schema = 'net' AND table_name = 'fragment'""")
    have = {r[0] for r in cur.fetchall()}
    conn.close()

    if not cols:
        raise SystemExit('public.fragments не найдена')

    scalar, system, month = split_columns(cols)
    check_complete(system, 5, 'системам')
    check_complete(month, 12, 'месяцам')

    add = [(n, t) for n, t in scalar if n not in have]

    out = [
        '-- Фрагмент как объект: три таблицы вместо 156 колонок.',
        '--',
        '-- СГЕНЕРИРОВАНО tools/gen_fragment_split.py по фактическому',
        '-- составу public.fragments. Руками не правят: опечатка в имени',
        '-- вроде tpod_fakt_11 молча потеряла бы столбец данных.',
        '--',
        '-- Обоснование разложения — docs/15-fragment-and-subsystems.md.',
        '-- Коротко: 132 из 156 колонок это массивы, выписанные плоско,',
        '-- а net.fragment не имеет геометрии и слоем карты не является.',
        '--',
        '-- Откат: sql/151_fragment_split_rollback.sql.',
        '',
        'BEGIN;',
        '',
        '-- 1. Недостающие скалярные поля паспорта фрагмента.',
    ]
    for n, t in add:
        out.append('ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS'
                   ' %-18s %s;' % (n, t))
    if not add:
        out.append('-- (все скалярные поля уже есть)')

    def child(table, key, key_max, groups, comment):
        lines = [
            '',
            '-- %s' % comment,
            'CREATE TABLE IF NOT EXISTS net.%s (' % table,
            '    fragment_id  integer NOT NULL'
            ' REFERENCES net.fragment(id) ON DELETE CASCADE,',
            '    %-12s smallint NOT NULL CHECK (%s BETWEEN 1 AND %d),'
            % (key, key, key_max),
        ]
        for base in sorted(groups):
            dtype = list(groups[base].values())[0][1]
            lines.append('    %-12s %s,' % (base, dtype))
        lines += [
            '    PRIMARY KEY (fragment_id, %s)' % key,
            ');',
        ]
        return lines

    out += ['']
    out += child('fragment_system', 'sys_no', 5, system,
                 'Параметры по системам теплоснабжения: 5 строк вместо '
                 '%d колонок.' % (len(system) * 5))
    out += child('fragment_month', 'month', 12, month,
                 'Температуры по месяцам: 12 строк вместо %d колонок.'
                 % (len(month) * 12))

    # --- перенос данных ---
    out += ['', '-- 2. Перенос данных из public.fragments.', '']

    if add:
        sets = ',\n'.join('    %s = s.%s' % (n, n) for n, _ in add)
        out += [
            'UPDATE net.fragment f SET',
            sets,
            'FROM public.fragments s',
            'WHERE s.id = f.id;',
            '',
        ]

    for table, key, rng, groups in (
            ('fragment_system', 'sys_no', 5, system),
            ('fragment_month', 'month', 12, month)):
        bases = sorted(groups)
        out += [
            'INSERT INTO net.%s (fragment_id, %s, %s)'
            % (table, key, ', '.join(bases)),
            'SELECT f.id, v.%s,' % key,
            ',\n'.join(
                '    CASE v.%s %s END AS %s'
                % (key,
                   ' '.join('WHEN %d THEN s.%s' % (i, groups[b][i][0])
                            for i in range(1, rng + 1)),
                   b)
                for b in bases),
            'FROM public.fragments s',
            'JOIN net.fragment f ON f.id = s.id',
            'CROSS JOIN generate_series(1, %d) AS v(%s)' % (rng, key),
            'ON CONFLICT (fragment_id, %s) DO NOTHING;' % key,
            '',
        ]

    out += [
        'COMMIT;',
        '',
        '-- Проверка: строк-потомков должно быть ровно по числу фрагментов.',
        'SELECT (SELECT count(*) FROM net.fragment)          AS fragmentov,',
        '       (SELECT count(*) FROM net.fragment_system)   AS strok_sistem,',
        '       (SELECT count(*) FROM net.fragment_month)    AS strok_mesyacev;',
        '',
    ]

    io.open(args.out, 'w', encoding='utf-8', newline='\n').write(
        '\n'.join(out))
    print('-> %s' % args.out)
    print('  скалярных колонок: %d (добавить %d)' % (len(scalar), len(add)))
    print('  групп по системам: %d -> %d колонок'
          % (len(system), len(system) * 5))
    print('  групп по месяцам:  %d -> %d колонок'
          % (len(month), len(month) * 12))
    return 0


if __name__ == '__main__':
    sys.exit(main())
