"""Генерация sql3/us_net.sql и sql3/ut_net.sql — чтение схемы из net.

Требование совместимости: набор ВОЗВРАЩАЕМЫХ колонок должен совпадать
с прежними us.sql / ut.sql, иначе приложение перестанет находить поля
по имени (оно читает через query.value("имя")).

Второе требование: приложение дописывает фильтр конкатенацией строк —
  read_nodes.cpp:243   q += " AND n.fileID IN (...)"
  read_lines.cpp:212   q += " AND n1.fileID IN (...)"
Поэтому запрос обязан заканчиваться WHERE, а внешний алиас обязан
называться n (для узлов) и n1 (для линий) и иметь колонку fileid.
"""
import argparse
import os

import psycopg2

# Код типа объекта, который прежний us.sql формировал через CASE
NODE_TYPE_CODE = {
    'consumer_real': 'PR',
    'consumer_general': 'PO',
    'heat_source': 'IS',
    'pump_station': 'HS',
    'valve_3way': 'C3',
    'connect_node': 'US2',
    'heat_chamber': 'US',
    'regulator_press': 'US',
    'refill_node': 'US',
    'node_plain': 'US',
}

# Колонки, которые собираются не из одноимённого поля
SPECIAL_NODE = {
    'id': '{t}.src_id',
    'fileid': '{t}.fragment_id',
    'x': 'ST_X({t}.geom) * 100.0',
    'y': '-ST_Y({t}.geom) * 100.0',
    'id2': '{t}.id',
    'type_txt': "'{code}'",
    'removed': "CASE WHEN {t}.removed_at IS NULL THEN 0 ELSE 1 END",
}

# hz — высота здания, поле называется по-разному у двух классов
HZ = {
    'consumer_real': 'buildheight',
    'consumer_general': 'maxbuildingheight',
}

# Колонки результатов расчёта: приходят из *_OUT, в union их нет
CALC_COLS = {
    'nompt', 'uspid', 'usoid', 'calculationidp', 'calculationido',
    'pihp', 'piho', 'tp1', 'to1', 'a15', 'a16', 'a17', 'iszakr', 'nomdr',
}


# Коды типов линейных объектов, как их формировал прежний ut.sql
LINE_TYPE_CODE = {
    'pipe_section': 'UT',
    'pump': 'HC',
    'regulator_press': 'RD',
    'damper': 'ZD',
    'diaphragm': 'DR',
    'elevator': 'EL',
    'radiator': 'RO',
    'heat_exchanger': 'TO',
    'air_heater': 'KU',
    # localhydroresistances2 в прежнем ut.sql не джойнился вовсе,
    # поэтому type_txt у него был NULL — сохраняем поведение.
    'local_resistance': None,
    'line_plain': None,
}

SPECIAL_LINE = {
    'id': '{t}.src_id',
    'fileid': '{t}.fragment_id',
    'nodeid1': '{t}.node_from_src',
    'nodeid2': '{t}.node_to_src',
    'id2': '{t}.id',
    'coords': '{t}.coords_legacy',
    'type_txt': "{code}",
}


def net_columns(cur, table):
    cur.execute("""SELECT column_name FROM information_schema.columns
                   WHERE table_schema = 'net' AND table_name = %s""", (table,))
    return {r[0] for r in cur.fetchall()}


def column_types(cur, tables):
    """Тип каждой выходной колонки — по первой таблице, где она есть.

    Нужен, чтобы NULL в ветвях UNION были типизированы: иначе PostgreSQL
    выводит для них text и падает на несовпадении типов с соседней ветвью.
    """
    cur.execute("""SELECT table_name, column_name, data_type
                   FROM information_schema.columns
                   WHERE table_schema = 'net' AND table_name = ANY(%s)""",
                (list(tables),))
    types = {}
    for _, col, dt in cur.fetchall():
        types.setdefault(col, dt)
    return types


def build_union(cur, targets, out_cols, code_map, hz_map):
    types = column_types(cur, targets)
    branches = []
    for t in targets:
        have = net_columns(cur, t)
        code = code_map.get(t, 'US')
        parts = []
        for c in out_cols:
            if c in CALC_COLS:
                continue
            if c in SPECIAL_NODE:
                parts.append('%s AS %s'
                             % (SPECIAL_NODE[c].format(t=t, code=code), c))
            elif c == 'hz':
                src = hz_map.get(t)
                parts.append('%s AS hz'
                             % ('%s.%s' % (t, src) if src and src in have
                                else 'NULL::double precision'))
            elif c == 'consumerstateid':
                parts.append('%s AS consumerstateid'
                             % ('%s.consumerstateid' % t
                                if 'consumerstateid' in have else '1'))
            elif c in have:
                parts.append('%s.%s' % (t, c))
            else:
                parts.append('NULL::%s AS %s' % (types.get(c, 'text'), c))
        # removed_at IS NULL ставим В ВЕТВЬ, а не во внешний WHERE.
        # Иначе планировщик не может сопоставить условие с частичным
        # индексом ... WHERE removed_at IS NULL и уходит в Seq Scan.
        # Наружу отдаём константу 0 — контракт колонки removed сохраняется.
        parts.append('0 AS removed')
        branches.append('SELECT\n    %s\nFROM net.%s %s\nWHERE %s.removed_at IS NULL'
                        % (',\n    '.join(parts), t, t, t))
    return '\nUNION ALL\n'.join(branches)


HEAD = """-- СГЕНЕРИРОВАНО tools/gen_read_sql.py
--
-- Замена {old} для схемы net. Набор возвращаемых колонок совпадает
-- с прежним, поэтому приложение менять не требуется — достаточно
-- подложить этот файл вместо старого.
--
-- Запрос заканчивается WHERE: приложение дописывает
--   AND {alias}.fileID IN (...)
-- конкатенацией строки.
"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--outdir', default='sql/sql3')
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int, default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'almatygid'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    args = ap.parse_args()

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()

    us_cols = ('id,fileid,internalnodeid,externalcodeid,externalsignid,'
               'displaysign,x,y,nodetypeid,externalnodename,nodename,'
               'geomarktoptube,geomarknodearea,calcpressflow,calcpressret,'
               'consumerstateid,hz,nompt,connectid,automdegid,calchldep,'
               'calchlindep,calchlventil,calchlparall,calchlgvsparall,'
               'avghlcompparall,calchlmix,calchlgvsmix,avghlcompmix,'
               'calchlconseq,calchlgvsconseq,avghlcompconseq,calchlpreon,'
               'calchlgvspreon,avghlcomppreon,avghlgvsopensysflow,'
               'avghlgvsopensysret,avghlcompopen,calcinternhd,expendhwpart,'
               'avghlcond,avghlclosesys,avghlopensysflow,avghlopensysret,'
               'avghlgvsopenflow,avghlgvsopenret,avghlgvscloseparall,'
               'avghlgvsclosemix,avghlgvscloseconseq,avghlgvsclosepreon,'
               'wemeteringdeviceid,uspid,usoid,calculationidp,calculationido,'
               'pihp,piho,tp1,to1,a15,a16,a17,iszakr,nomdr,schemenum,id2,'
               'type_txt').split(',')

    union = build_union(cur, list(NODE_TYPE_CODE), us_cols,
                        NODE_TYPE_CODE, HZ)

    outer = []
    for c in us_cols:
        if c == 'nompt':
            outer.append('pt_out.id AS nompt')
        elif c == 'nomdr':
            outer.append('dr_out.id AS nomdr')
        elif c == 'uspid':
            outer.append('usp.id AS uspid')
        elif c == 'usoid':
            outer.append('uso.id AS usoid')
        elif c == 'calculationidp':
            outer.append('usp.calculationid AS calculationidp')
        elif c == 'calculationido':
            outer.append('uso.calculationid AS calculationido')
        elif c == 'pihp':
            outer.append('usp.pih AS pihp')
        elif c == 'piho':
            outer.append('uso.pih AS piho')
        elif c == 'tp1':
            outer.append('usp.t AS tp1')
        elif c == 'to1':
            outer.append('uso.t AS to1')
        elif c in ('a15', 'a16', 'a17'):
            outer.append('pt_out.%s' % c)
        elif c == 'iszakr':
            outer.append("CASE WHEN pt_out.a3 = 'Закрыта' THEN 1 ELSE 0 END "
                         "AS iszakr")
        else:
            outer.append('n.%s' % c)

    sql = HEAD.format(old='us.sql', alias='n') + """
SELECT
    {cols}
FROM (
{union}
) n
LEFT JOIN (
    SELECT c.fileid, max(c.id) AS cid FROM public.calculation c GROUP BY c.fileid
) calc ON calc.fileid = n.fileid
LEFT JOIN public.us_out usp ON usp.nodeid = n.id AND usp.externalsign = 1
                           AND usp.calculationid = calc.cid
LEFT JOIN public.us_out uso ON uso.nodeid = n.id AND uso.externalsign = 2
                           AND uso.calculationid = calc.cid
LEFT JOIN public.pt_out  ON pt_out.nodeid = n.id AND pt_out.calculationid = calc.cid
LEFT JOIN public.dr_out  ON dr_out.nodeid = n.id AND dr_out.calculationid = calc.cid
WHERE n.removed = 0
""".format(cols=',\n    '.join(outer), union=union)

    os.makedirs(args.outdir, exist_ok=True)
    path = os.path.join(args.outdir, 'us_net.sql')
    with open(path, 'w', encoding='utf-8') as f:
        f.write(sql)
    print('-> %s (%d колонок, %d ветвей union)'
          % (path, len(us_cols), len(NODE_TYPE_CODE)))

    build_lines(cur, args.outdir)
    conn.close()


def build_lines(cur, outdir):
    ut_cols = ('id,fileid,nodeid1,nodeid2,externalsignlineid,displaysign,'
               'organizationid,dru_home,magistral,distsite,magistralsite,'
               'tubingtypeid,diameterinternal,diameterexternal,diametercondit,'
               'pipesectlength,wallthickness,crimpingquesite,pipesectionid,'
               'pipesectstateidflow,pipesectstateidret,id2,type_txt,nomgp,'
               'nomgo,pod_poter,pod_w,pod_time1,pod_a14,pod_a15,pod_a16,'
               'pod_a17,pod_tzam,pod_tpot,obr_poter,obr_w,obr_time1,obr_a14,'
               'obr_a15,obr_a16,obr_a17,obr_tzam,obr_tpot,pod_b101,pod_b102,'
               'pod_b103,pod_b104,pod_b105,pod_b106,obr_b101,obr_b102,'
               'obr_b103,obr_b104,pod_q,obr_q,coords').split(',')

    # Колонки, приходящие из таблиц результатов расчёта
    calc_line = {c for c in ut_cols
                 if c.startswith(('pod_', 'obr_', 'nomg'))}

    targets = list(LINE_TYPE_CODE)
    types = column_types(cur, targets)
    branches = []
    for t in targets:
        have = net_columns(cur, t)
        code = LINE_TYPE_CODE[t]
        code_sql = "'%s'::text" % code if code else 'NULL::text'
        parts = []
        for c in ut_cols:
            if c in calc_line:
                continue
            if c in SPECIAL_LINE:
                parts.append('%s AS %s'
                             % (SPECIAL_LINE[c].format(t=t, code=code_sql), c))
            elif c in have:
                parts.append('%s.%s' % (t, c))
            else:
                parts.append('NULL::%s AS %s' % (types.get(c, 'text'), c))
        parts.append('0 AS removed')
        branches.append(
            'SELECT\n    %s\nFROM net.%s %s\nWHERE %s.removed_at IS NULL'
            % (',\n    '.join(parts), t, t, t))

    union = '\nUNION ALL\n'.join(branches)

    outer = []
    for c in ut_cols:
        if c in calc_line:
            side = 'utp' if c.startswith('pod_') else 'uto'
            if c in ('nomgp', 'nomgo'):
                outer.append('%s.id AS %s'
                             % ('utp' if c == 'nomgp' else 'uto', c))
            else:
                # В ut_out поля называются a10..a17: имена pod_*/obr_*
                # — это псевдонимы прежнего ut.sql, сохраняем соответствие.
                suffix = c.split('_', 1)[1]
                real = {'q': 'a13', 'poter': 'a14',
                        'w': 'a10', 'time1': 'a11'}.get(suffix, suffix)
                outer.append('%s.%s AS %s' % (side, real, c))
        else:
            outer.append('n1.%s' % c)

    sql = HEAD.format(old='ut.sql', alias='n1') + """
SELECT
    {cols}
FROM (
{union}
) n1
LEFT JOIN (
    SELECT c.fileid, max(c.id) AS cid FROM public.calculation c GROUP BY c.fileid
) calc ON calc.fileid = n1.fileid
LEFT JOIN public.ut_out utp ON utp.lineid = n1.id
                           AND utp.externalsignlineid IN (2, 4)
                           AND utp.calculationid = calc.cid
LEFT JOIN public.ut_out uto ON uto.lineid = n1.id
                           AND uto.externalsignlineid IN (3, 5)
                           AND uto.calculationid = calc.cid
WHERE n1.removed = 0
""".format(cols=',\n    '.join(outer), union=union)

    path = os.path.join(outdir, 'ut_net.sql')
    with open(path, 'w', encoding='utf-8') as f:
        f.write(sql)
    print('-> %s (%d колонок, %d ветвей union)'
          % (path, len(ut_cols), len(targets)))


if __name__ == '__main__':
    main()
