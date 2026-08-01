"""Что осталось в public и к какой группе оно относится.

«Одна БД net, ничего старого» упирается в вопрос, на который нельзя
ответить на глаз: в public 619 таблиц, и это не только теплосеть.
Там же капремонт, шурфы, опрессовки, дефекты, коррозия, электрические
сети, документы и кадры — рабочие данные, которые никуда не денутся.

Скрипт раскладывает всё, что осталось в public, по группам и считает
строки, чтобы объём работ был виден числом, а не ощущением.

Группы:
  legacy-остаток  — таблицы *_legacy от конвертации, к ним никто не ходит;
  пусто           — ноль строк: скорее всего не используется;
  справочник      — маленькая таблица вида id + name/наименование;
  используется    — встречается в запросах движка sety или в SQL репозитория;
  данные          — всё прочее: рабочие подсистемы со своими данными.

    python tools/public_inventory.py --db tgid_gis
    python tools/public_inventory.py --db tgid_gis --json docs/schema/public_map.json
"""
import argparse
import io
import json
import os
import re
import sys

import psycopg2

SETY_DIR = r'H:\projects\tgid-app-new\gid8\python\sety\sety'
LOOKUP_NAME = re.compile(r'^(name|naimenovanie|nazvanie|title|descr\w*)$', re.I)


def used_names(*roots):
    """Имена, упомянутые в коде движка и в SQL репозитория."""
    seen = set()
    word = re.compile(r'[A-Za-z_][\w]{2,}')
    for root in roots:
        if not os.path.isdir(root):
            continue
        for dirpath, dirnames, files in os.walk(root):
            dirnames[:] = [d for d in dirnames
                           if d not in ('__pycache__', '.git', 'build')]
            for fn in files:
                if not fn.lower().endswith(('.py', '.sql', '.cpp', '.h')):
                    continue
                p = os.path.join(dirpath, fn)
                try:
                    text = io.open(p, encoding='utf-8',
                                   errors='replace').read()
                except OSError:
                    continue
                seen.update(w.lower() for w in word.findall(text))
    return seen


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', required=True)
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', default=os.environ.get('TGID_PORT', '5440'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--sety', default=SETY_DIR)
    ap.add_argument('--repo', default='.')
    ap.add_argument('--json', default='')
    args = ap.parse_args()

    used = used_names(args.sety, os.path.join(args.repo, 'sql'),
                      os.path.join(args.repo, 'tgid_app', 'src'))

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()
    cur.execute("""
        SELECT c.relname
        FROM pg_class c JOIN pg_namespace n ON n.oid = c.relnamespace
        WHERE n.nspname = 'public' AND c.relkind = 'r'
        ORDER BY c.relname""")
    tables = [r[0] for r in cur.fetchall()]

    cur.execute("""
        SELECT table_name, count(*),
               count(*) FILTER (WHERE column_name ~* '^(name|naimenovanie|nazvanie|title)$')
        FROM information_schema.columns
        WHERE table_schema = 'public'
        GROUP BY table_name""")
    colinfo = {t: (n, has) for t, n, has in cur.fetchall()}

    rows = {}
    for t in tables:
        cur.execute('SELECT count(*) FROM public.%s' % psycopg2.extensions
                    .quote_ident(t, cur))
        rows[t] = cur.fetchone()[0]
    conn.close()

    groups = {'legacy-остаток': [], 'пусто': [], 'справочник': [],
              'используется': [], 'данные': []}
    for t in tables:
        n = rows[t]
        ncols, has_name = colinfo.get(t, (0, 0))
        # Порядок важен. «Используется» обязано проверяться РАНЬШЕ, чем
        # «пусто»: таблица бывает пустой именно сейчас, но приложение в
        # неё пишет. Отнести такую к «пусто» — значит предложить её
        # удалить и сломать запись.
        if t.endswith('_legacy'):
            g = 'legacy-остаток'
        elif t in used:
            g = 'используется'
        elif n == 0:
            g = 'пусто'
        elif ncols <= 4 and has_name and n < 500:
            g = 'справочник'
        else:
            g = 'данные'
        groups[g].append({'table': t, 'rows': n, 'cols': ncols})

    print('в public таблиц: %d, строк всего: %d'
          % (len(tables), sum(rows.values())))
    print('\n%-16s %6s %12s' % ('группа', 'таблиц', 'строк'))
    for g in ('legacy-остаток', 'пусто', 'справочник', 'используется', 'данные'):
        lst = groups[g]
        print('%-16s %6d %12d'
              % (g, len(lst), sum(x['rows'] for x in lst)))

    print('\nсамые крупные из группы «данные»:')
    for x in sorted(groups['данные'], key=lambda x: -x['rows'])[:15]:
        print('  %-46s %10d строк' % (x['table'], x['rows']))

    print('\nиспользуемые движком или SQL (их переносить в первую очередь):')
    for x in sorted(groups['используется'], key=lambda x: -x['rows'])[:20]:
        print('  %-46s %10d строк' % (x['table'], x['rows']))

    if args.json:
        os.makedirs(os.path.dirname(args.json) or '.', exist_ok=True)
        with io.open(args.json, 'w', encoding='utf-8') as f:
            json.dump(groups, f, ensure_ascii=False, indent=1)
        print('\nполная карта:', args.json)
    return 0


if __name__ == '__main__':
    sys.exit(main())
