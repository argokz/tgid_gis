"""Генерация слоя совместимости для таблиц-подтипов.

Без него приложение продолжает писать в public.realconsumers и подобные
как в обычные таблицы, и созданный объект не попадает в net: он остаётся
в node_plain, а тип теряется. Проверено — так и происходит.

Каждая таблица-подтип заменяется представлением над объектной таблицей net
с INSTEAD OF триггерами:
  INSERT — переносит объект в нужный класс (net.reclass_node/_line)
           и заполняет поля;
  UPDATE — правит поля объекта;
  DELETE — возвращает объект в *_plain, то есть снимает тип.
"""
import argparse
import json
import os

import psycopg2

HEADER = """-- СГЕНЕРИРОВАНО tools/gen_subtype_views.py
--
-- Слой совместимости для таблиц-подтипов. Представления создаются
-- в схеме net под именами v_<исходная таблица>, оригиналы не трогаются.
-- Подстановка вместо public.* — отдельным шагом (sql/060_switch_subtypes.sql).
"""

VIEW = """
-- {src} -> net.{target}
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_{src} CASCADE;
CREATE VIEW net.v_{src} AS
SELECT o.subtype_src_id AS id,
       o.id             AS {link}{cols}
FROM net.{target} o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS {link}{vcols}
FROM net.object_variant v
WHERE v.src_table = '{src}' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_{src}_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.{link} IS NULL THEN
        RAISE EXCEPTION '{src}: не указан {link}';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_{kind}(NEW.{link}, '{target}');
    UPDATE net.{target} SET subtype_src_id = NEW.id{sets_new}
    WHERE id = NEW.{link};
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_{src}_ins ON net.v_{src};
CREATE TRIGGER v_{src}_ins INSTEAD OF INSERT ON net.v_{src}
    FOR EACH ROW EXECUTE FUNCTION net.v_{src}_ins();

CREATE OR REPLACE FUNCTION net.v_{src}_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.{target} SET subtype_src_id = NEW.id{sets_new}
    WHERE id = OLD.{link};
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_{src}_upd ON net.v_{src};
CREATE TRIGGER v_{src}_upd INSTEAD OF UPDATE ON net.v_{src}
    FOR EACH ROW EXECUTE FUNCTION net.v_{src}_upd();

CREATE OR REPLACE FUNCTION net.v_{src}_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_{kind}(OLD.{link}, '{plain}');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_{src}_del ON net.v_{src};
CREATE TRIGGER v_{src}_del INSTEAD OF DELETE ON net.v_{src}
    FOR EACH ROW EXECUTE FUNCTION net.v_{src}_del();
"""


def net_columns(cur, table):
    cur.execute("""SELECT column_name FROM information_schema.columns
                   WHERE table_schema = 'net' AND table_name = %s""", (table,))
    return {r[0] for r in cur.fetchall()}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mapping', required=True)
    ap.add_argument('--out', default='sql/055_subtype_views.sql')
    ap.add_argument('--switch', default='sql/060_switch_subtypes.sql')
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int, default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'tgid_gis'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    args = ap.parse_args()

    with open(args.mapping, encoding='utf-8') as f:
        m = json.load(f)

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()

    out = [HEADER]
    switch = ['-- СГЕНЕРИРОВАНО tools/gen_subtype_views.py',
              '-- Подстановка представлений вместо таблиц-подтипов.',
              '-- Оригиналы сохраняются под именами *_legacy.',
              '\\set ON_ERROR_STOP on', '', 'BEGIN;', '']
    rollback = ['-- Откат подстановки таблиц-подтипов.',
                '\\set ON_ERROR_STOP on', '', 'BEGIN;', '']

    pairs = ([(e, 'node', 'node_plain') for e in m['class_node']] +
             [(e, 'line', 'line_plain') for e in m['class_line']])

    for e, kind, plain in pairs:
        src, target = e['source'], e['target']
        link = e['link_column']
        have = net_columns(cur, target)
        cols = [c for c in e['columns'] if c in have]

        cur.execute("""SELECT column_name, data_type
                       FROM information_schema.columns
                       WHERE table_schema = 'net' AND table_name = %s""",
                    (target,))
        types = dict(cur.fetchall())

        out.append(VIEW.format(
            src=src, target=target, link=link, kind=kind, plain=plain,
            cols=''.join(',\n       o.%s' % c for c in cols),
            vcols=''.join(",\n       (v.payload ->> '%s')::%s" %
                          (c, types.get(c, 'text')) for c in cols),
            sets_new=''.join(',\n        %s = NEW.%s' % (c, c) for c in cols)))

        switch.append('ALTER TABLE public.%s RENAME TO %s_legacy;' % (src, src))
        switch.append('CREATE VIEW public.%s AS SELECT * FROM net.v_%s;'
                      % (src, src))
        rollback.append('DROP VIEW IF EXISTS public.%s;' % src)
        rollback.append('ALTER TABLE public.%s_legacy RENAME TO %s;'
                        % (src, src))

    switch += ['', 'COMMIT;', '',
               "\\echo 'Таблицы-подтипов заменены представлениями над net'"]
    rollback += ['', 'COMMIT;', '',
                 "\\echo 'Таблицы-подтипов возвращены'"]

    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    with open(args.out, 'w', encoding='utf-8') as f:
        f.write('\n'.join(out))
    with open(args.switch, 'w', encoding='utf-8') as f:
        f.write('\n'.join(switch) + '\n')
    with open(args.switch.replace('060_switch', '061_rollback'), 'w',
              encoding='utf-8') as f:
        f.write('\n'.join(rollback) + '\n')

    print('-> %s (%d подтипов)' % (args.out, len(pairs)))
    print('-> %s' % args.switch)
    conn.close()


if __name__ == '__main__':
    main()
