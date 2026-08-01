"""Раскладывает оставшиеся таблицы public по схемам подсистем.

Правило из docs/15: перепроектировать заново — только то, что рисует
карта или читает расчёт; всё остальное переносится в свою схему как есть.

Здесь — механический перенос. Принадлежность определяется по имени,
но список в итоговом SQL пишется ЯВНО: миграцию продуктовой БД читают
глазами при разборе, и она не должна зависеть от того, как сегодня
написана регулярка.

Что НЕ трогаем:
  * spatial_ref_sys — служебная таблица PostGIS;
  * всё, что читает движок расчёта или SQL репозитория (fragments,
    setpressnodes, heatsystem, температурные графики, потери и т.п.):
    их место определится, когда расчётный контур доведут до net;
  * changed_object и audit_log — журнал изменений, у него свой
    механизм в 090_object_history.sql, трогать отдельно.

    python tools/gen_subsystems.py --db tgid_gis --out sql/140_subsystems.sql
"""
import argparse
import io
import os
import re
import sys

import psycopg2

# Схема -> список регулярок по имени таблицы.
RULES = [
    ('ops', [
        r'^kapremont', r'^kapitalnyy_remont$', r'^remont_',
        r'^opressovk', r'^ochered_opressovok$',
        r'^shurf', r'^vosstanovitelnye_raboty_shurf$',
        r'^defekt', r'^namechennye_meropriyatiya',
        r'^faktiznos_', r'^faktriska_',
        r'korroz', r'^prichiny_(razryva|svischa)', r'^vid_korrozii$',
        r'^tolschinometr$', r'^diag$', r'^sostoyanie_',
        r'^agressivnost_setevoy_vody$', r'^harakter_grunta$',
        r'^izolyatsiya_truby$', r'^konstruktsiya_',
        r'^material_(pokrovnogo_sloya|teploizolyatsionnyy)$',
        r'^plity_perekrytiya', r'^tip_izolyatsii$',
        r'^vidy_(elementov|trub)$', r'^perspektivnye_truby$',
        r'^pasport_uchastka_', r'^uchastok_(ms|rs)$',
    ]),
    ('org', [
        r'^organizatsii', r'^proektnaya_organizatsiya$',
        r'^serviceorganizations$', r'^rabotniki$', r'^dolzhnosti$',
        r'^nachalniki_uchastkov$', r'^familii_masterov',
        r'^uchastki_ekspluatatsii$', r'^rayon_ekspluatatsii$',
        r'^prinadlezhnost$', r'^zavod_izgotovitel$',
        r'^factorymanufacturers$', r'^passwords?$',
    ]),
    ('el', [
        r'^energosistemy$', r'^marki_(kabeley|transformatorov)_es$',
        r'_elektricheskih_setey$', r'^tipy_lep$',
    ]),
    ('doc', [
        r'^passports?$', r'^passportsdocs$', r'^tehnicheskie_usloviya$',
        r'^texts$', r'^help_old$', r'^oshibki_vstavki$',
        r'^audit_group_comments$',
    ]),
    ('addr', [
        r'^addresses$', r'^geodz$', r'^zdaniya', r'^vvod(y)?_v_zdanie$',
        r'^spisok_(ulits|zhilyh_massivov)', r'^ulitsy$',
        r'^sposob_vvoda_v_zdanie$', r'^funktsionalnoe_naznachenie',
        r'^tip_(massiva|kladbischa)$', r'^buildingentries$',
        r'^zhile1?$', r'^obekty$', r'^tablitsa1$',
    ]),
]

# Не переносим ни при каких условиях.
KEEP = {
    'spatial_ref_sys',          # PostGIS
    'changed_object', 'audit_log',   # журнал изменений, свой механизм
    # читает движок расчёта или SQL репозитория:
    'fragments', 'setpressnodes', 'heatsystem', 'deployeddirections',
    'hsourceworkmonthgraphs', 'varcoefficients', 'calctemperatures',
    'specexpends', 'airgroundmonthtemperatures', 'localhydroresistances',
    'magistrals', 'magistrali', 'normtemperaturesgraphs',
    'qualityregtempgraphs', 'gvsloadgraphs', 'calcschemes',
    'losesbyfilling', 'perehod_diametra', 'truby_rekonstruiruemye',
    'prisoedinennaya_nagruzka_istochnikov',
    'prisoedinennaya_nagruzka_istochnikov3',
    'nagruzki', 'istochniki_tepla', 'kotelnye', 'raspredseti',
    'heating_seasons', 'teplo', 'trps', 'tip_truboprovoda',
    # объекты сети, у которых уже есть класс в net — разбирать отдельно
    'bypass', 'consumptregulators', 'pressdropregulators',
    'regularmatures', 'reversevalves',
    'opora', 'lyuki', 'ugol_povorota_truboprovoda', 'wdodevices',
    'klapany__ventili__zapornye', 'pavilions', 'overgroundnodes',
    'undergroundnodes', 'uninstallednodes',
    'tab', 'table',             # мусорные имена, разбирать отдельно
}


def classify(name):
    for schema, pats in RULES:
        for p in pats:
            if re.search(p, name):
                return schema
    return None


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
        SELECT c.relname FROM pg_class c
        JOIN pg_namespace n ON n.oid = c.relnamespace
        WHERE n.nspname = 'public' AND c.relkind = 'r'
        ORDER BY c.relname""")
    tables = [r[0] for r in cur.fetchall()]
    conn.close()

    plan, rest = {}, []
    for t in tables:
        if t in KEEP:
            rest.append(t)
            continue
        s = classify(t)
        if s:
            plan.setdefault(s, []).append(t)
        else:
            rest.append(t)

    schemas = sorted(plan)
    out = [
        '-- Подсистемы переезжают из public в свои схемы.',
        '--',
        '-- СГЕНЕРИРОВАНО tools/gen_subsystems.py. Списки явные: миграцию',
        '-- продуктовой БД читают глазами, и она не должна зависеть от',
        '-- того, как сегодня написана регулярка в генераторе.',
        '--',
        '-- Правило отбора (docs/15): перепроектировать заново — только то,',
        '-- что рисует карта или читает расчёт. Эти подсистемы не рисуются',
        '-- и не считаются, поэтому переносятся как есть, без изменения',
        '-- структуры.',
        '--',
        '-- search_path расширяется новыми схемами: обращения по имени без',
        '-- схемы обязаны продолжать работать, в том числе из старого',
        '-- приложения. Без этого перенос был бы не реорганизацией, а',
        '-- поломкой.',
        '--',
        '-- Откат: sql/141_subsystems_rollback.sql.',
        '',
        'BEGIN;',
        '',
    ]
    for s in schemas:
        out.append('CREATE SCHEMA IF NOT EXISTS %s;' % s)
    out.append('')

    for s in schemas:
        out += [
            '-- %s: %d таблиц' % (s, len(plan[s])),
            'DO $$',
            'DECLARE t text; n int := 0;',
            'BEGIN',
            '    FOREACH t IN ARRAY ARRAY[',
            ',\n'.join("        '%s'" % x for x in plan[s]),
            '    ]',
            '    LOOP',
            '        IF EXISTS (SELECT 1 FROM pg_class c'
            ' JOIN pg_namespace ns ON ns.oid = c.relnamespace',
            "                   WHERE ns.nspname = 'public'"
            " AND c.relname = t AND c.relkind = 'r') THEN",
            "            EXECUTE format('ALTER TABLE public.%%I"
            " SET SCHEMA %s', t);" % s,
            '            n := n + 1;',
            '        END IF;',
            '    END LOOP;',
            "    RAISE NOTICE 'в %s перенесено: %%', n;" % s,
            'END $$;',
            '',
        ]

    path = 'public, net, ref, calc, ' + ', '.join(schemas)
    out += [
        '-- Порядок поиска — свойство БД: его видят все клиенты.',
        'ALTER DATABASE %s SET search_path = %s;' % ('tgid_gis', path),
        '',
        'COMMIT;',
        '',
        'SELECT count(*) AS ostalos_v_public FROM pg_class c',
        'JOIN pg_namespace n ON n.oid = c.relnamespace',
        "WHERE n.nspname = 'public' AND c.relkind = 'r';",
        '',
    ]

    io.open(args.out, 'w', encoding='utf-8', newline='\n').write(
        '\n'.join(out))
    print('-> %s' % args.out)
    for s in schemas:
        print('  %-6s %3d таблиц' % (s, len(plan[s])))
    print('  остаётся в public: %d' % len(rest))
    print('\nостаются:', ', '.join(sorted(rest)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
