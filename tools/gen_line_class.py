"""Генерирует DDL объектных таблиц net для линейных подтипов.

Пять типов оборудования остались без класса в net: bypass, reversevalves,
regularmatures, consumptregulators, pressdropregulators. Структура
создаётся сейчас, данные не переносятся — строк в источнике 0, 0, 0, 7 и
13, и все они относятся к оборудованию без концов (см. docs/14, «чердак»).

DDL не пишется руками: общая часть берётся из уже существующей таблицы
класса-образца, собственные колонки — из исходной таблицы подтипа.
Так новый класс гарантированно совпадает с остальными по набору полей,
ограничений и триггеров.

    python tools/gen_line_class.py --db tgid_gis --out sql/130_itp_classes.sql
"""
import argparse
import io
import os
import sys

import psycopg2

# исходная таблица -> имя класса в net
CLASSES = {
    'bypass': 'bypass',
    'reversevalves': 'valve_reverse',
    'regularmatures': 'armature_control',
    'consumptregulators': 'regulator_consumption',
    'pressdropregulators': 'regulator_pressdrop',
}

TEMPLATE = 'damper'          # класс-образец
# Колонки образца, общие для всех линейных классов. Порядок сохраняется.
HEAD = ['id', 'fragment_id', 'node_from', 'node_to', 'node_from_src',
        'node_to_src', 'fileid_src', 'geom', 'removed_at', 'src_id',
        'externalsignlineid', 'location', 'hydrores', 'organizationid',
        'registnum', 'firstpicdate', 'lastmaintdate', 'displaysign',
        'archivechangedate', 'operatorid', 'typ', 'internalnodeid']
TAIL = ['coords_legacy', 'subtype_src_id', 'row_version', 'updated_at',
        'updated_by']
# Колонки подтипа, которые в объектную таблицу не переносятся:
# id заменяется собственным, lineid и nodeid выражены через node_from/to.
SKIP_OWN = {'id', 'lineid', 'nodeid'}


def cols_of(cur, schema, table):
    cur.execute("""
        SELECT column_name, data_type, udt_name, character_maximum_length,
               numeric_precision, numeric_scale, column_default, is_nullable
        FROM information_schema.columns
        WHERE table_schema = %s AND table_name = %s
        ORDER BY ordinal_position""", (schema, table))
    return cur.fetchall()


def sql_type(row):
    name, dtype, udt, clen, nprec, nscale = row[:6]
    if dtype == 'USER-DEFINED':
        return 'geometry(LineString, 9998)' if udt == 'geometry' else udt
    if dtype == 'character varying':
        return 'varchar(%d)' % clen if clen else 'text'
    if dtype == 'character':
        return 'char(%d)' % clen if clen else 'char'
    if dtype == 'numeric' and nprec:
        return 'numeric(%d,%d)' % (nprec, nscale or 0)
    return dtype


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

    tmpl = {r[0]: r for r in cols_of(cur, 'net', TEMPLATE)}
    missing = [c for c in HEAD + TAIL if c not in tmpl]
    if missing:
        print('в образце net.%s нет колонок: %s' % (TEMPLATE, missing))
        return 2

    out = [
        '-- Объектные таблицы net для оборудования, оставшегося без класса.',
        '--',
        '-- СГЕНЕРИРОВАНО tools/gen_line_class.py по образцу net.%s:' % TEMPLATE,
        '-- общая часть скопирована с образца, собственные колонки взяты',
        '-- из исходной таблицы подтипа. Руками этот файл не правят.',
        '--',
        '-- Данные НЕ переносятся. В источнике этих строк 0, 0, 0, 7 и 13,',
        '-- и все они относятся к оборудованию с пустыми концами, которое',
        '-- не видело и старое приложение (docs/14, раздел про чердак).',
        '-- Структура создаётся, чтобы объект имел своё место в модели;',
        '-- наполнение — отдельное решение заказчика.',
        '',
        'BEGIN;',
        '',
    ]

    for src, cls in sorted(CLASSES.items()):
        # Нельзя отбрасывать колонку только потому, что одноимённое поле
        # есть в таблице-образце. В CREATE ниже из образца берутся лишь
        # HEAD/TAIL; старое условие `r[0] not in tmpl` поэтому потеряло
        # relatleakage, diametercondit, name и ещё несколько полей.
        common = set(HEAD + TAIL)
        own = [r for r in cols_of(cur, 'public', src)
               if r[0] not in SKIP_OWN and r[0] not in common]
        def decl(row):
            # DEFAULT и NOT NULL берутся из образца: без них id новых
            # объектов не получал бы значение из net.obj_id_seq, и
            # вставка из Qt или QGIS падала бы.
            name, default, nullable = row[0], row[6], row[7]
            out = '    %-22s %s' % (name, sql_type(row))
            if default:
                out += ' DEFAULT %s' % default
            if nullable == 'NO':
                out += ' NOT NULL'
            return out

        lines = [decl(tmpl[c]) for c in HEAD]
        lines += [decl(r) for r in own]
        lines += [decl(tmpl[c]) for c in TAIL]

        out += [
            '-- %s: %d собственных колонок из public.%s' % (cls, len(own), src),
            'CREATE TABLE IF NOT EXISTS net.%s (' % cls,
            ',\n'.join(lines),
            ');',
            '',
        ]

        # CREATE TABLE IF NOT EXISTS не добавляет новые колонки в уже
        # собранную БД. Отдельные ADD делают исправление генератора
        # идемпотентной миграцией, без удаления даже пустой таблицы.
        out += [
            'ALTER TABLE net.%s ADD COLUMN IF NOT EXISTS %s;'
            % (cls, decl(r).strip()) for r in own
        ]

        out += [
            '',
            'ALTER TABLE net.%s DROP CONSTRAINT IF EXISTS %s_pkey;'
            % (cls, cls),
            'ALTER TABLE net.%s' % cls,
            '    ADD CONSTRAINT %s_pkey PRIMARY KEY (id);' % cls,
            'ALTER TABLE net.%s DROP CONSTRAINT IF EXISTS %s_fragment_id_fkey;'
            % (cls, cls),
            'ALTER TABLE net.%s' % cls,
            '    ADD CONSTRAINT %s_fragment_id_fkey' % cls,
            '    FOREIGN KEY (fragment_id) REFERENCES net.fragment(id);',
            'ALTER TABLE net.%s DROP CONSTRAINT IF EXISTS %s_node_from_fkey;'
            % (cls, cls),
            'ALTER TABLE net.%s' % cls,
            '    ADD CONSTRAINT %s_node_from_fkey' % cls,
            '    FOREIGN KEY (node_from) REFERENCES net.node_reg(id)'
            ' ON DELETE RESTRICT;',
            'ALTER TABLE net.%s DROP CONSTRAINT IF EXISTS %s_node_to_fkey;'
            % (cls, cls),
            'ALTER TABLE net.%s' % cls,
            '    ADD CONSTRAINT %s_node_to_fkey' % cls,
            '    FOREIGN KEY (node_to) REFERENCES net.node_reg(id)'
            ' ON DELETE RESTRICT;',
            '',
            'CREATE INDEX IF NOT EXISTS %s_geom_idx ON net.%s USING gist (geom);'
            % (cls, cls),
            'CREATE INDEX IF NOT EXISTS %s_fragment_id_idx ON net.%s (fragment_id);'
            % (cls, cls),
            '',
            'DROP TRIGGER IF EXISTS %s_reg ON net.%s;' % (cls, cls),
            "CREATE TRIGGER %s_reg AFTER INSERT OR DELETE ON net.%s"
            % (cls, cls),
            "    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('%s');" % cls,
            'DROP TRIGGER IF EXISTS object_touch ON net.%s;' % cls,
            'CREATE TRIGGER object_touch BEFORE UPDATE ON net.%s' % cls,
            '    FOR EACH ROW EXECUTE FUNCTION net.touch_object_row();',
            'DROP TRIGGER IF EXISTS object_insert_log ON net.%s;' % cls,
            'CREATE TRIGGER object_insert_log AFTER INSERT ON net.%s' % cls,
            '    FOR EACH ROW EXECUTE FUNCTION net.log_object_insert();',
            'DROP TRIGGER IF EXISTS line_topology ON net.%s;' % cls,
            'CREATE TRIGGER line_topology BEFORE INSERT OR UPDATE OF'
            ' node_from, node_to, fragment_id, geom, removed_at',
            '    ON net.%s FOR EACH ROW' % cls,
            '    EXECUTE FUNCTION net.validate_line_topology();',
            'DROP TRIGGER IF EXISTS geometry_change_audit ON net.%s;' % cls,
            'CREATE TRIGGER geometry_change_audit AFTER UPDATE OF geom'
            ' ON net.%s' % cls,
            '    FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change();',
            '',
        ]

    out += ['COMMIT;', '']
    conn.close()

    os.makedirs(os.path.dirname(args.out) or '.', exist_ok=True)
    io.open(args.out, 'w', encoding='utf-8', newline='\n').write(
        '\n'.join(out))
    print('-> %s (классов %d)' % (args.out, len(CLASSES)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
