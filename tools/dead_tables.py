"""Кандидаты на удаление из tgid_gis — со свидетельствами, без удаления.

Инструмент НИЧЕГО не удаляет и не изменяет. Он только собирает
доказательства и печатает список. Решение принимает человек, DDL
пишется отдельно и применяется после резервной копии.

Осторожность здесь не формальность: в этом проекте уже был случай, когда
проверочный запуск оказался исполнением и уничтожил 26 таблиц `calc` и
696 562 строки. Поэтому: только SELECT, никаких DROP, никакого
"--apply".

«Пустая» само по себе не значит «ненужная». Пустыми намеренно созданы
классы оборудования ИТП (решение заказчика: перенести структуру без
данных) и таблицы результатов `calc`, которые наполняются расчётом.
Поэтому кандидатом считается таблица, у которой одновременно:

  * ноль строк (точный count, а не оценка pg_stat);
  * нет ссылок в коде ни одного потребителя;
  * никто не ссылается внешним ключом;
  * не упомянута ни в одном представлении;
  * не упомянута ни в одном теле функции;
  * нет в каталоге слоёв meta.

    python tools/dead_tables.py                # разбор живых схем
    python tools/dead_tables.py --schema attic # отдельно чердак
"""
import argparse
import os
import sys

import psycopg2

TOOLS = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, TOOLS)

import db_consumers  # noqa: E402  — переиспользуем сканер кода

# Схемы, которые разбираем по умолчанию. attic сюда не входит: это
# сознательно сохранённое изъятое, его судьба — отдельное решение.
LIVE = ('net', 'ref', 'ops', 'org', 'doc', 'el', 'meta', 'addr', 'calc')


def code_refs():
    """(ссылки в SQL, упоминания словом) по всем деревьям-потребителям.

    scan() возвращает НЕ отображение «отношение -> сведения», а словарь
    с ключами sql/words/made/dyn/files. Первая версия этой функции
    приняла эти ключи за имена таблиц и в итоге не нашла ссылок вообще
    ни у кого: в кандидаты на удаление попали ops.shurfy (11 файлов
    gid8), ops.iznos (gidrSlot.cpp), ops.osmotr, ops.sortnodesforuchastok
    и ещё семь десятков живых таблиц. Ошибка обнаружилась только сверкой
    с db_consumers --table. Поэтому здесь разбираются именно вложенные
    словари, а любое упоминание — даже просто словом в тексте — считается
    поводом таблицу не трогать.
    """
    sql_refs, word_refs = {}, {}
    for _name, root, _path in db_consumers.CONSUMERS:
        if not os.path.isdir(root):
            continue
        found = db_consumers.scan(root)
        for rel, files in found['sql'].items():
            sql_refs[rel] = sql_refs.get(rel, 0) + len(files)
        for word, files in found['words'].items():
            word_refs[word] = word_refs.get(word, 0) + len(files)
    return sql_refs, word_refs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'tgid_gis'))
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', default=os.environ.get('TGID_PORT', '5440'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--schema', default='')
    args = ap.parse_args()

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    conn.set_session(readonly=True)          # страховка: только чтение
    cur = conn.cursor()

    schemas = (args.schema,) if args.schema else LIVE

    cur.execute("""
        SELECT n.nspname, c.relname, c.oid
        FROM pg_class c JOIN pg_namespace n ON n.oid = c.relnamespace
        WHERE c.relkind = 'r' AND n.nspname = ANY(%s)
        ORDER BY n.nspname, c.relname""", (list(schemas),))
    tables = cur.fetchall()

    # Ссылки внешними ключами НА таблицу.
    cur.execute("""
        SELECT confrelid::regclass::text, count(*)
        FROM pg_constraint WHERE contype = 'f' GROUP BY 1""")
    fk_in = {k.split('.')[-1].strip('"').lower(): v for k, v in cur.fetchall()}

    # Упоминания в представлениях и в телах функций.
    cur.execute("""
        SELECT lower(pg_get_viewdef(c.oid, true))
        FROM pg_class c WHERE c.relkind = 'v'""")
    views_text = '\n'.join(r[0] for r in cur.fetchall())

    cur.execute("SELECT lower(COALESCE(prosrc, '')) FROM pg_proc")
    procs_text = '\n'.join(r[0] for r in cur.fetchall())

    # Каталог слоёв.
    catalog = set()
    cur.execute("SELECT to_regclass('meta.layer')")
    if cur.fetchone()[0]:
        try:
            cur.execute("SELECT lower(table_name) FROM meta.layer")
            catalog = {r[0] for r in cur.fetchall() if r[0]}
        except Exception:
            conn.rollback()

    sql_refs, word_refs = code_refs()

    dead, empty_used, used = [], [], 0
    for schema, table, oid in tables:
        cur.execute('SELECT count(*) FROM %s.%s'
                    % (psycopg2.extensions.quote_ident(schema, cur),
                       psycopg2.extensions.quote_ident(table, cur)))
        rows = cur.fetchone()[0]

        key = db_consumers.norm(table)
        in_code = (sql_refs.get(key, 0) + word_refs.get(key, 0)
                   + sql_refs.get('%s.%s' % (schema, key), 0))
        in_fk = fk_in.get(key, 0)
        in_view = key in views_text
        in_proc = key in procs_text
        in_cat = key in catalog

        if rows or in_code or in_fk or in_view or in_proc or in_cat:
            if rows == 0:
                empty_used.append((schema, table, in_code, in_fk,
                                   in_view, in_proc, in_cat))
            else:
                used += 1
            continue

        cur.execute('SELECT pg_total_relation_size(%s)', (oid,))
        dead.append((schema, table, cur.fetchone()[0]))

    print('Разобрано схем: %s' % ', '.join(schemas))
    print('Таблиц всего: %d' % len(tables))
    print('  с данными или со ссылками : %d' % used)
    print('  пустых, но нужных         : %d' % len(empty_used))
    print('  КАНДИДАТОВ НА УДАЛЕНИЕ    : %d' % len(dead))

    if empty_used:
        print('\nПустые, но удалять нельзя — на них ссылаются:')
        for s, t, c, f, v, p, cat in empty_used:
            why = []
            if c:   why.append('код:%d' % c)
            if f:   why.append('внешних ключей:%d' % f)
            if v:   why.append('представление')
            if p:   why.append('функция')
            if cat: why.append('каталог слоёв')
            print('  %-6s %-34s %s' % (s, t[:34], ', '.join(why)))

    if dead:
        print('\nКандидаты (0 строк, ни одной ссылки нигде):')
        for s, t, size in sorted(dead, key=lambda x: (x[0], x[1])):
            print('  %-6s %-34s %8d Б' % (s, t[:34], size))
        print('\nDDL не генерируется намеренно: список сначала читает '
              'человек.')
    conn.close()
    return 0


if __name__ == '__main__':
    sys.stdout.reconfigure(encoding='utf-8')
    sys.exit(main())
