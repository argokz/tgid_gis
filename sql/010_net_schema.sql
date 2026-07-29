-- СГЕНЕРИРОВАНО tools/gen_ddl.py — править руками не нужно.
-- Целевая схема ТГИД: одна таблица — один тип объекта.
--
-- id всех объектов выдаёт общая последовательность net.obj_id_seq, поэтому он
-- уникален глобально. Реестры net.node_reg / net.line_reg нужны ради настоящих
-- внешних ключей от линий и дочерних таблиц к узлам.

CREATE SCHEMA IF NOT EXISTS net;
CREATE SEQUENCE IF NOT EXISTS net.obj_id_seq AS bigint START 1;

CREATE TABLE IF NOT EXISTS net.fragment (
    id          int PRIMARY KEY,
    name        text NOT NULL,
    name_sys    text,
    nasel_point text,
    sezon       text,
    "year"      text,
    removed_at  timestamptz
);

CREATE TABLE IF NOT EXISTS net.node_reg (
    id bigint PRIMARY KEY, kind text NOT NULL, tbl text NOT NULL);
CREATE INDEX IF NOT EXISTS node_reg_kind_idx ON net.node_reg (kind);

CREATE TABLE IF NOT EXISTS net.line_reg (
    id bigint PRIMARY KEY, kind text NOT NULL, tbl text NOT NULL);
CREATE INDEX IF NOT EXISTS line_reg_kind_idx ON net.line_reg (kind);

-- Линии, концы которых не разрешаются в узлы: геометрию построить нельзя.
CREATE TABLE IF NOT EXISTS net.line_orphan (
    id        bigint PRIMARY KEY,
    src_table text NOT NULL,
    nodeid1   int,
    nodeid2   int,
    reason    text NOT NULL,
    payload   jsonb NOT NULL
);

-- Объекты, отвергнутые конвертером, и причины — для разбора после миграции.
CREATE TABLE IF NOT EXISTS net.conversion_reject (
    id        bigserial PRIMARY KEY,
    src_table text NOT NULL,
    src_id    bigint,
    reason    text NOT NULL,
    detail    jsonb
);

-- Триггеры реестра устроены так, чтобы объект можно было ПЕРЕНЕСТИ из одной
-- объектной таблицы в другую, не разрывая внешние ключи от линий.
--
-- INSERT обновляет запись реестра (ON CONFLICT), а DELETE удаляет её только
-- если реестр всё ещё указывает на эту таблицу. Тогда перенос «вставить
-- в новую таблицу, удалить из старой» оставляет строку реестра на месте,
-- и ссылки net.line.node_from не ломаются.
CREATE OR REPLACE FUNCTION net.reg_node_sync() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO net.node_reg (id, kind, tbl)
        VALUES (NEW.id, TG_ARGV[0], TG_TABLE_NAME)
        ON CONFLICT (id) DO UPDATE
            SET kind = EXCLUDED.kind, tbl = EXCLUDED.tbl;
        RETURN NEW;
    END IF;
    DELETE FROM net.node_reg
    WHERE id = OLD.id AND tbl = TG_TABLE_NAME;
    RETURN OLD;
END $$;

CREATE OR REPLACE FUNCTION net.reg_line_sync() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO net.line_reg (id, kind, tbl)
        VALUES (NEW.id, TG_ARGV[0], TG_TABLE_NAME)
        ON CONFLICT (id) DO UPDATE
            SET kind = EXCLUDED.kind, tbl = EXCLUDED.tbl;
        RETURN NEW;
    END IF;
    DELETE FROM net.line_reg
    WHERE id = OLD.id AND tbl = TG_TABLE_NAME;
    RETURN OLD;
END $$;

-- Перенос объекта в другой класс: общие колонки копируются, остальные
-- получают значения по умолчанию. Порядок важен — сначала вставка
-- (она обновит реестр), потом удаление (реестр уже указывает на новую
-- таблицу, поэтому строка реестра уцелеет).
CREATE OR REPLACE FUNCTION net.reclass_node(p_id bigint, p_target text)
RETURNS void
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $$
DECLARE
    src  text;
    cols text;
BEGIN
    SELECT tbl INTO src FROM net.node_reg WHERE id = p_id;
    IF src IS NULL THEN
        RAISE EXCEPTION 'узел % не найден в реестре', p_id;
    END IF;
    IF src = p_target THEN
        RETURN;
    END IF;

    SELECT string_agg(quote_ident(a.column_name), ', ')
    INTO cols
    FROM information_schema.columns a
    JOIN information_schema.columns b
      ON b.table_schema = 'net' AND b.table_name = p_target
     AND b.column_name = a.column_name
    WHERE a.table_schema = 'net' AND a.table_name = src
      AND a.is_generated = 'NEVER' AND b.is_generated = 'NEVER';

    EXECUTE format('INSERT INTO net.%I (%s) SELECT %s FROM net.%I WHERE id = $1',
                   p_target, cols, cols, src) USING p_id;
    EXECUTE format('DELETE FROM net.%I WHERE id = $1', src) USING p_id;
END $$;


-- ============ ТОЧЕЧНЫЕ КЛАССЫ ============

-- connect_node  <-  public.connectnodes  (1527 строк, class_node)
CREATE TABLE IF NOT EXISTS net.connect_node (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , geom         geometry(Point, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , internalnodeid                   int
  , externalcodeid                   int
  , externalnodename                 text
  , externalsignid                   int
  , addressid                        int
  , geomarktoptube                   double precision
  , geomarknodearea                  double precision
  , calcpressflow                    double precision
  , calcpressret                     double precision
  , picdate                          date
  , lastrepairdate                   date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , nodename                         text
  , scheme                           text
  , memo                             text
  , gpscoords                        text
  , belonghn                         text
  , passport                         text
  , inventnumber                     text
  , pipelinesign                     text
  , nodetypeid                       int
  , isbusy                           int
  , isloaded                         int
  , belongmagistralsite              int
  , belongdistsite                   int
  , organizationid                   int
  , magistralsite                    int
  , distsite                         int
  , registnumber                     text
  , connectid                        int
);
CREATE INDEX IF NOT EXISTS connect_node_geom_idx ON net.connect_node USING gist (geom);
CREATE INDEX IF NOT EXISTS connect_node_frag_idx ON net.connect_node (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS connect_node_src_uidx ON net.connect_node (src_id);
DROP TRIGGER IF EXISTS connect_node_reg ON net.connect_node;
CREATE TRIGGER connect_node_reg AFTER INSERT OR DELETE ON net.connect_node
    FOR EACH ROW EXECUTE FUNCTION net.reg_node_sync('connect_node');

-- consumer_general  <-  public.generalizedconsumers  (29051 строк, class_node)
CREATE TABLE IF NOT EXISTS net.consumer_general (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , geom         geometry(Point, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , internalnodeid                   int
  , externalcodeid                   int
  , externalnodename                 text
  , externalsignid                   int
  , addressid                        int
  , geomarktoptube                   double precision
  , geomarknodearea                  double precision
  , calcpressflow                    double precision
  , calcpressret                     double precision
  , picdate                          date
  , lastrepairdate                   date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , nodename                         text
  , scheme                           text
  , memo                             text
  , gpscoords                        text
  , belonghn                         text
  , passport                         text
  , inventnumber                     text
  , pipelinesign                     text
  , nodetypeid                       int
  , isbusy                           int
  , isloaded                         int
  , belongmagistralsite              int
  , belongdistsite                   int
  , organizationid                   int
  , magistralsite                    int
  , distsite                         int
  , registnumber                     text
  , quarter                          int
  , maxbuildingheight                double precision
  , accumcoeff                       double precision
  , hydromodesignid                  int
  , specexpendid                     int
  , calctemperatureid                int
  , gvsloadgraphid                   int
  , varcoeffid                       int
  , normhlosflow                     double precision
  , normhlosret                      double precision
  , calchldep                        double precision
  , calcinternhddep                  double precision
  , adjcalchldep                     double precision
  , adjcalchddep                     double precision
  , perspcalchldep                   double precision
  , perspcalcinternhddep             double precision
  , calctempdep                      double precision
  , connectionschemeid               int
  , mixfactcoeffdep                  double precision
  , calchlindep                      double precision
  , calcinternhdindep                double precision
  , adjcalchlindep                   double precision
  , adjcalcinternhdindep             double precision
  , perspcalchlindep                 double precision
  , perspcalcinternhdindep           double precision
  , calctempindep                    double precision
  , mixfactcoeffindep                double precision
  , calchlventil                     double precision
  , adjcalchlventil                  double precision
  , perspcalchlventil                double precision
  , calchlcond                       double precision
  , adjcalchlcond                    double precision
  , perspcalchlcond                  double precision
  , calchlclosesys                   double precision
  , calchlopensysflow                double precision
  , calchlopensysret                 double precision
  , adjcalchlclosesys                double precision
  , adjcalchlopensysflow             double precision
  , adjcalchlopensysret              double precision
  , perspcalchlclosesys              double precision
  , perspcalchlopensysflow           double precision
  , perspcalchlopensysret            double precision
  , expendhwpart                     double precision
  , calctempha                       double precision
  , calchlparall                     double precision
  , internhdparall                   double precision
  , adjcalchlparall                  double precision
  , adjinternhdparall                double precision
  , perspcalchlparall                double precision
  , perspinternhdparall              double precision
  , schemeparallid                   int
  , calchlgvsparall                  double precision
  , adjcalchlgvsparall               double precision
  , perspcalchlgvsparall             double precision
  , hourirregcoeffparall             double precision
  , avghlcompparall                  double precision
  , temprecircpipeparall             double precision
  , calctemphrparall                 double precision
  , calctemphwdoparall               double precision
  , amrdepparall                     double precision
  , calchlmix                        double precision
  , internhdmix                      double precision
  , adjcalchlmix                     double precision
  , adjinternhdmix                   double precision
  , perspcalchlmix                   double precision
  , perspinternhdmix                 double precision
  , schememixid                      int
  , calchlgvsmix                     double precision
  , adjcalchlgvsmix                  double precision
  , perspcalchlgvsmix                double precision
  , hourirregcoeffmix                double precision
  , avghlcompmix                     double precision
  , temprecircpipemix                double precision
  , calctemphrmix                    double precision
  , calctemphwdomix                  double precision
  , amrdepmix                        double precision
  , calchlconseq                     double precision
  , internhdconseq                   double precision
  , adjcalchlconseq                  double precision
  , adjinternhdconseq                double precision
  , perspcalchlconseq                double precision
  , perspinternhdconseq              double precision
  , schemeconseqid                   int
  , calchlgvsconseq                  double precision
  , adjcalchlgvsconseq               double precision
  , perspcalchlgvsconseq             double precision
  , hourirregcoeffconseq             double precision
  , avghlcompconseq                  double precision
  , temprecircpipeconseq             double precision
  , calctemphrconseq                 double precision
  , calctemphwdoconseq               double precision
  , amrdepconseq                     double precision
  , calchlpreon                      double precision
  , internhdpreon                    double precision
  , adjcalchlpreon                   double precision
  , adjinternhdpreon                 double precision
  , perspcalchlpreon                 double precision
  , perspinternhdpreon               double precision
  , schemepreonid                    int
  , calchlgvspreon                   double precision
  , adjcalchlgvspreon                double precision
  , perspcalchlgvspreon              double precision
  , hourirregcoeffpreon              double precision
  , avghlcomppreon                   double precision
  , temprecircpipepreon              double precision
  , calctemphrpreon                  double precision
  , calctemphwdopreon                double precision
  , amrdeppreon                      double precision
  , avghlgvsopensysflow              double precision
  , avghlgvsopensysret               double precision
  , adjavghlgvsopensysflow           double precision
  , adjavghlgvsopensysret            double precision
  , perspavghlgvsopensysflow         double precision
  , perspavghlgvsopensysret          double precision
  , hourirregcoeffopen               double precision
  , avghlcompopen                    double precision
  , temprecircpipeopen               double precision
  , calctemphwdoopen                 double precision
  , pdvalveinstalllocid              int
  , setpdonregulator                 double precision
  , setleakageflow                   double precision
  , setleakageret                    double precision
  , wemeteringdeviceid               int
  , volwaterhs                       double precision
  , volwatervs                       double precision
  , minthrustds                      double precision
  , hydroresclosesys                 double precision
  , hydroresclosesyssummer           double precision
  , closesyscalcsignid               int
  , hydroreswdoflow                  double precision
  , calcsignopensysflowid            int
  , hydroreswdoret                   double precision
  , calcsignreswdoretid              int
  , calcsignsetloadclosesys          int
  , calcsignsetloadopensysflow       int
  , calcsignsetloadopensysret        int
  , hydrothrustin                    double precision
  , hydrothrustout                   double precision
  , calcexpenddep                    double precision
  , calcexpendindep                  double precision
  , calcexpendventil                 double precision
  , calcexpendcond                   double precision
  , calcexpendhwflow                 double precision
  , calcexpendhwret                  double precision
  , calcexpendrecircopen             double precision
  , calcexpendhwparall               double precision
  , calcexpendhwmix                  double precision
  , calcexpendhwconseq               double precision
  , calcexpendhwpreon                double precision
  , calcthrustinwsd                  int
  , consumerstateid                  int
  , "name"                           text
);
CREATE INDEX IF NOT EXISTS consumer_general_geom_idx ON net.consumer_general USING gist (geom);
CREATE INDEX IF NOT EXISTS consumer_general_frag_idx ON net.consumer_general (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS consumer_general_src_uidx ON net.consumer_general (src_id);
DROP TRIGGER IF EXISTS consumer_general_reg ON net.consumer_general;
CREATE TRIGGER consumer_general_reg AFTER INSERT OR DELETE ON net.consumer_general
    FOR EACH ROW EXECUTE FUNCTION net.reg_node_sync('consumer_general');

-- heat_chamber  <-  public.heatchambers  (91 строк, class_node)
CREATE TABLE IF NOT EXISTS net.heat_chamber (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , geom         geometry(Point, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , internalnodeid                   int
  , externalcodeid                   int
  , externalnodename                 text
  , externalsignid                   int
  , addressid                        int
  , geomarktoptube                   double precision
  , geomarknodearea                  double precision
  , calcpressflow                    double precision
  , calcpressret                     double precision
  , picdate                          date
  , lastrepairdate                   date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , nodename                         text
  , scheme                           text
  , memo                             text
  , gpscoords                        text
  , belonghn                         text
  , passport                         text
  , inventnumber                     text
  , pipelinesign                     text
  , nodetypeid                       int
  , isbusy                           int
  , isloaded                         int
  , belongmagistralsite              int
  , belongdistsite                   int
  , organizationid                   int
  , magistralsite                    int
  , distsite                         int
  , registnumber                     text
  , "name"                           text
  , slotscount                       int
  , slotsweight                      text
  , internalheight                   double precision
  , internalwidth                    double precision
  , internallength                   double precision
  , wallmaterial                     text
  , stairscount                      int
  , stoparmaturecount                int
  , ballvalvescount                  int
  , airventscount                    int
);
CREATE INDEX IF NOT EXISTS heat_chamber_geom_idx ON net.heat_chamber USING gist (geom);
CREATE INDEX IF NOT EXISTS heat_chamber_frag_idx ON net.heat_chamber (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS heat_chamber_src_uidx ON net.heat_chamber (src_id);
DROP TRIGGER IF EXISTS heat_chamber_reg ON net.heat_chamber;
CREATE TRIGGER heat_chamber_reg AFTER INSERT OR DELETE ON net.heat_chamber
    FOR EACH ROW EXECUTE FUNCTION net.reg_node_sync('heat_chamber');

-- heat_source  <-  public.heatsources  (164 строк, class_node)
CREATE TABLE IF NOT EXISTS net.heat_source (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , geom         geometry(Point, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , internalnodeid                   int
  , externalcodeid                   int
  , externalnodename                 text
  , externalsignid                   int
  , addressid                        int
  , geomarktoptube                   double precision
  , geomarknodearea                  double precision
  , calcpressflow                    double precision
  , calcpressret                     double precision
  , picdate                          date
  , lastrepairdate                   date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , nodename                         text
  , scheme                           text
  , memo                             text
  , gpscoords                        text
  , belonghn                         text
  , passport                         text
  , inventnumber                     text
  , pipelinesign                     text
  , nodetypeid                       int
  , isbusy                           int
  , isloaded                         int
  , belongmagistralsite              int
  , belongdistsite                   int
  , organizationid                   int
  , magistralsite                    int
  , distsite                         int
  , registnumber                     text
  , sourcename                       text
  , "name"                           text
  , stateid                          int
  , hsourcetypeid                    int
  , hsourceid                        int
  , hsourcepower                     double precision
  , hsourcepowerinst                 double precision
  , hseasonbegindate                 date
  , hseasonenddate                   date
  , hsourcecode                      text
  , temperdwflowsummer               double precision
  , temperdwretsummer                double precision
  , specvolhs                        double precision
  , specvolvent                      double precision
  , expenddwnorm1                    double precision
  , expenddwnorm2                    double precision
  , expenddwnorm3                    double precision
  , expenddwnorm4                    double precision
  , expenddwnorm5                    double precision
  , managerphone                     int
  , controllerphone                  int
  , powerset                         double precision
  , poweravailable                   double precision
  , heighttubemark                   int
  , heightareamark                   int
  , repairworks                      double precision
  , t1_summer                        double precision
  , t2_summer                        double precision
  , name_tg                          text
  , heatloscalcyear                  text
  , graphtypeid                      int
  , tn_1                             double precision
  , tn_5                             double precision
  , tvn_r                            double precision
  , t1_r                             double precision
  , t2_r                             double precision
  , t3_r                             double precision
  , q_r                              double precision
  , t1_2r                            double precision
  , t1_4r                            double precision
  , tvb_tr                           double precision
  , uf                               double precision
  , t2_2r                            double precision
  , q_gv                             double precision
  , tg_r                             double precision
  , tx_r                             double precision
  , t2_gv                            double precision
  , pr                               int
  , g1                               double precision
  , g2                               double precision
  , t_gv1                            double precision
  , deployedtempgraphid              int
  , v                                double precision
  , date_on                          double precision
  , name_exe                         text
  , name_manager                     text
  , dt2_co                           double precision
  , t2pod_parl                       double precision
  , dt2v_sm_noavm                    double precision
  , dt2v_sm_avm                      double precision
  , t1pod_sm                         double precision
  , t1pod_posll                      double precision
  , dt2v_posl                        double precision
  , length_1                         int
  , length_2                         int
  , length_3                         int
  , length_4                         int
  , length_5                         int
  , length_6                         int
  , length_7                         int
  , length_8                         int
  , length_9                         int
  , length_10                        int
  , length_11                        int
  , length_12                        int
  , t_1                              double precision
  , t_2                              double precision
  , t_3                              double precision
  , t_4                              double precision
  , t_5                              double precision
  , t_6                              double precision
  , t_7                              double precision
  , t_8                              double precision
  , t_9                              double precision
  , t_10                             double precision
  , t_11                             double precision
  , t_12                             double precision
  , t1_1                             double precision
  , t1_2                             double precision
  , t1_3                             double precision
  , t1_4                             double precision
  , t1_5                             double precision
  , t1_6                             double precision
  , t1_7                             double precision
  , t1_8                             double precision
  , t1_9                             double precision
  , t1_10                            double precision
  , t1_11                            double precision
  , t1_12                            double precision
  , t2_1                             double precision
  , t2_2                             double precision
  , t2_3                             double precision
  , t2_4                             double precision
  , t2_5                             double precision
  , t2_6                             double precision
  , t2_7                             double precision
  , t2_8                             double precision
  , t2_9                             double precision
  , t2_10                            double precision
  , t2_11                            double precision
  , t2_12                            double precision
  , length_1_fakt                    int
  , length_2_fakt                    int
  , length_3_fakt                    int
  , length_4_fakt                    int
  , length_5_fakt                    int
  , length_6_fakt                    int
  , length_7_fakt                    int
  , length_8_fakt                    int
  , length_9_fakt                    int
  , length_10_fakt                   int
  , length_11_fakt                   int
  , length_12_fakt                   int
  , t_1_fakt                         double precision
  , t_2_fakt                         double precision
  , t_3_fakt                         double precision
  , t_4_fakt                         double precision
  , t_5_fakt                         double precision
  , t_6_fakt                         double precision
  , t_7_fakt                         double precision
  , t_8_fakt                         double precision
  , t_9_fakt                         double precision
  , t_10_fakt                        double precision
  , t_11_fakt                        double precision
  , t_12_fakt                        double precision
  , t1_1_fakt                        double precision
  , t1_2_fakt                        double precision
  , t1_3_fakt                        double precision
  , t1_4_fakt                        double precision
  , t1_5_fakt                        double precision
  , t1_6_fakt                        double precision
  , t1_7_fakt                        double precision
  , t1_8_fakt                        double precision
  , t1_9_fakt                        double precision
  , t1_10_fakt                       double precision
  , t1_11_fakt                       double precision
  , t1_12_fakt                       double precision
  , t2_1_fakt                        double precision
  , t2_2_fakt                        double precision
  , t2_3_fakt                        double precision
  , t2_4_fakt                        double precision
  , t2_5_fakt                        double precision
  , t2_6_fakt                        double precision
  , t2_7_fakt                        double precision
  , t2_8_fakt                        double precision
  , t2_9_fakt                        double precision
  , t2_10_fakt                       double precision
  , t2_11_fakt                       double precision
  , t2_12_fakt                       double precision
  , gpod_1_fakt                      double precision
  , gpod_2_fakt                      double precision
  , gpod_3_fakt                      double precision
  , gpod_4_fakt                      double precision
  , gpod_5_fakt                      double precision
  , gpod_6_fakt                      double precision
  , gpod_7_fakt                      double precision
  , gpod_8_fakt                      double precision
  , gpod_9_fakt                      double precision
  , gpod_10_fakt                     double precision
  , gpod_11_fakt                     double precision
  , gpod_12_fakt                     double precision
  , ggvs_pribor_1_fakt               double precision
  , ggvs_pribor_2_fakt               double precision
  , ggvs_pribor_3_fakt               double precision
  , ggvs_pribor_4_fakt               double precision
  , ggvs_pribor_5_fakt               double precision
  , ggvs_pribor_6_fakt               double precision
  , ggvs_pribor_7_fakt               double precision
  , ggvs_pribor_8_fakt               double precision
  , ggvs_pribor_9_fakt               double precision
  , ggvs_pribor_10_fakt              double precision
  , ggvs_pribor_11_fakt              double precision
  , ggvs_pribor_12_fakt              double precision
  , gakt_tex_1_fakt                  double precision
  , gakt_tex_2_fakt                  double precision
  , gakt_tex_3_fakt                  double precision
  , gakt_tex_4_fakt                  double precision
  , gakt_tex_5_fakt                  double precision
  , gakt_tex_6_fakt                  double precision
  , gakt_tex_7_fakt                  double precision
  , gakt_tex_8_fakt                  double precision
  , gakt_tex_9_fakt                  double precision
  , gakt_tex_10_fakt                 double precision
  , gakt_tex_11_fakt                 double precision
  , gakt_tex_12_fakt                 double precision
  , gakt_avarija_1_fakt              double precision
  , gakt_avarija_2_fakt              double precision
  , gakt_avarija_3_fakt              double precision
  , gakt_avarija_4_fakt              double precision
  , gakt_avarija_5_fakt              double precision
  , gakt_avarija_6_fakt              double precision
  , gakt_avarija_7_fakt              double precision
  , gakt_avarija_8_fakt              double precision
  , gakt_avarija_9_fakt              double precision
  , gakt_avarija_10_fakt             double precision
  , gakt_avarija_11_fakt             double precision
  , gakt_avarija_12_fakt             double precision
  , id_old                           int
);
CREATE INDEX IF NOT EXISTS heat_source_geom_idx ON net.heat_source USING gist (geom);
CREATE INDEX IF NOT EXISTS heat_source_frag_idx ON net.heat_source (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS heat_source_src_uidx ON net.heat_source (src_id);
DROP TRIGGER IF EXISTS heat_source_reg ON net.heat_source;
CREATE TRIGGER heat_source_reg AFTER INSERT OR DELETE ON net.heat_source
    FOR EACH ROW EXECUTE FUNCTION net.reg_node_sync('heat_source');

-- pump_station  <-  public.pumpstations  (227 строк, class_node)
CREATE TABLE IF NOT EXISTS net.pump_station (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , geom         geometry(Point, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , internalnodeid                   int
  , externalcodeid                   int
  , externalnodename                 text
  , externalsignid                   int
  , addressid                        int
  , geomarktoptube                   double precision
  , geomarknodearea                  double precision
  , calcpressflow                    double precision
  , calcpressret                     double precision
  , picdate                          date
  , lastrepairdate                   date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , nodename                         text
  , scheme                           text
  , memo                             text
  , gpscoords                        text
  , belonghn                         text
  , passport                         text
  , inventnumber                     text
  , pipelinesign                     text
  , nodetypeid                       int
  , isbusy                           int
  , isloaded                         int
  , belongmagistralsite              int
  , belongdistsite                   int
  , organizationid                   int
  , magistralsite                    int
  , distsite                         int
  , registnumber                     text
  , "name"                           text
  , purpose                          text
  , capacity                         double precision
  , pumpcountflow                    int
  , pumpcountret                     int
  , state                            text
  , stateid                          int
  , heighttubemark                   double precision
  , heightareamark                   double precision
);
CREATE INDEX IF NOT EXISTS pump_station_geom_idx ON net.pump_station USING gist (geom);
CREATE INDEX IF NOT EXISTS pump_station_frag_idx ON net.pump_station (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS pump_station_src_uidx ON net.pump_station (src_id);
DROP TRIGGER IF EXISTS pump_station_reg ON net.pump_station;
CREATE TRIGGER pump_station_reg AFTER INSERT OR DELETE ON net.pump_station
    FOR EACH ROW EXECUTE FUNCTION net.reg_node_sync('pump_station');

-- consumer_real  <-  public.realconsumers  (26202 строк, class_node)
CREATE TABLE IF NOT EXISTS net.consumer_real (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , geom         geometry(Point, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , internalnodeid                   int
  , externalcodeid                   int
  , externalnodename                 text
  , externalsignid                   int
  , addressid                        int
  , geomarktoptube                   double precision
  , geomarknodearea                  double precision
  , calcpressflow                    double precision
  , calcpressret                     double precision
  , picdate                          date
  , lastrepairdate                   date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , nodename                         text
  , scheme                           text
  , memo                             text
  , gpscoords                        text
  , belonghn                         text
  , passport                         text
  , inventnumber                     text
  , pipelinesign                     text
  , nodetypeid                       int
  , isbusy                           int
  , isloaded                         int
  , belongmagistralsite              int
  , belongdistsite                   int
  , organizationid                   int
  , magistralsite                    int
  , distsite                         int
  , registnumber                     text
  , "name"                           text
  , consumerstateid                  int
  , automdegid                       int
  , sectconsumercode                 text
  , schemenum                        text
  , buildheight                      double precision
  , accumcoeff                       double precision
  , specexpendid                     int
  , calctemperatureid                int
  , gvsloadgraphid                   int
  , varcoeffid                       int
  , calchldep                        double precision
  , calchlindep                      double precision
  , relloadfacade                    double precision
  , calcinternhd                     double precision
  , contcalchldep                    double precision
  , contcalchlindep                  double precision
  , contrelloadfacade                double precision
  , continternhd                     double precision
  , perspcalchldep                   double precision
  , perspcalchlindep                 double precision
  , persprelloadfacade               double precision
  , perspinternhd                    double precision
  , calchlventil                     double precision
  , expendhwpart                     double precision
  , contcalchlventil                 double precision
  , perspcalchlventil                double precision
  , avghlcond                        double precision
  , contavghlcond                    double precision
  , perspavghlcond                   double precision
  , avghlclosesys                    double precision
  , avghlopensysflow                 double precision
  , avghlopensysret                  double precision
  , contavghlclosesys                double precision
  , contavghlopensysflow             double precision
  , contavghlopensysret              double precision
  , perspavghlclose                  double precision
  , perspavghlopenflow               double precision
  , contavghlopenret                 double precision
  , avghlgvsopenflow                 double precision
  , avghlgvsopenret                  double precision
  , avghlgvscloseparall              double precision
  , avghlgvsclosemix                 double precision
  , avghlgvscloseconseq              double precision
  , avghlgvsclosepreon               double precision
  , contavghlgvsopenflow             double precision
  , contavghlgvsopenret              double precision
  , contavghlgvscloseparall          double precision
  , contavghlgvsclosemix             double precision
  , contavghlgvscloseconseq          double precision
  , contavghlgvsclosepreon           double precision
  , perspavghlgvsopenflow            double precision
  , perspavghlgvsopenret             double precision
  , perspavghlgvscloseparall         double precision
  , perspavghlgvsclosemix            double precision
  , perspavghlgvscloseconseq         double precision
  , perspavghlgvsclosepreon          double precision
  , hydromodesignid                  int
  , mixfactcoeff                     double precision
  , hourirregcoeff                   double precision
  , circhlosopen                     double precision
  , temprecircpipe                   double precision
  , setleakageflow                   double precision
  , setleakageret                    double precision
  , wemeteringdeviceid               int
  , volwaterhs                       double precision
  , volwatervs                       double precision
  , hydroresclosesys                 double precision
  , hydroresclosesyssummer           double precision
  , closesyscalcsignid               int
  , hydroreswdoflow                  double precision
  , calcsignopensysflowid            int
  , hydroreswdoret                   double precision
  , calcsignreswdoretid              int
  , calcsignsetloadclosesys          int
  , calcsignsetloadopensysflow       int
  , calcsignsetloadopensysret        int
  , hydrothrustin                    double precision
  , hydrothrustout                   double precision
  , calcexpenddep                    double precision
  , calcexpendindep                  double precision
  , calcexpendventil                 double precision
  , calcexpendcond                   double precision
  , calcexpendhwflow                 double precision
  , calcexpendhwret                  double precision
  , calcexpendrecircopen             double precision
  , calcexpendhwparall               double precision
  , calcexpendhwmix                  double precision
  , calcexpendhwconseq               double precision
  , calcexpendhwpreon                double precision
  , throtstagesignid                 int
  , diameterthrotdiaph               double precision
  , diameterelevnozzle               double precision
  , temperchartsignid                int
  , calcsignres                      int
  , calcsignhl                       int
  , parallheaterscount1              int
  , parallheaterscount2              int
  , parallheaterscountindep          int
  , calcthrustloshs                  double precision
  , calcthrustlosah                  double precision
  , calcthrustlosac                  double precision
  , calcthrustlosflow                double precision
  , calcthrustlosflowcirc            double precision
  , calcthrustinwdo                  double precision
  , calcthrustlosheaters1            double precision
  , calcthrustlosheaters2            double precision
  , pdvalveinstalllocid              int
  , setpdonregulator                 double precision
  , calcferdiametersignid            int
  , calctemphr                       double precision
  , calctempvs                       double precision
  , calctemphwdo                     double precision
  , responsibleid                    int
  , contractnumber                   text
  , stopvalvetypeid                  int
  , meterdevworksign                 int
  , meterdevsafety                   int
  , meterdevstampnum                 text
  , isolationtype                    double precision
  , buildingtypeid                   int
  , heatsourceptsid                  int
  , heatpointid                      int
  , countusergv                      int
  , countbusinessconsumers           int
  , area                             double precision
  , buildingvolume                   double precision
  , basementvolume                   double precision
  , builtinvolume                    double precision
  , reducebuildingvolume             double precision
  , countfloors                      int
  , builtyear                        int
  , streetid                         int
  , housenumber                      text
  , note_1                           text
  , note_2                           text
  , note_pts                         text
  , note_regime                      text
  , b23                              double precision
  , b28                              double precision
  , b32                              double precision
  , b20                              double precision
  , b14                              double precision
  , b38                              double precision
  , b35                              double precision
  , b36                              double precision
  , zd7                              int
  , zd8                              int
  , zd27                             int
  , zd28                             int
  , zd36                             int
  , zd38                             int
  , zd39                             int
  , zd48                             int
  , zd49                             int
  , zd50                             int
  , elevatornuminst                  int
  , diameternozzle                   double precision
);
CREATE INDEX IF NOT EXISTS consumer_real_geom_idx ON net.consumer_real USING gist (geom);
CREATE INDEX IF NOT EXISTS consumer_real_frag_idx ON net.consumer_real (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS consumer_real_src_uidx ON net.consumer_real (src_id);
DROP TRIGGER IF EXISTS consumer_real_reg ON net.consumer_real;
CREATE TRIGGER consumer_real_reg AFTER INSERT OR DELETE ON net.consumer_real
    FOR EACH ROW EXECUTE FUNCTION net.reg_node_sync('consumer_real');

-- refill_node  <-  public.refillnodes  (1 строк, class_node)
CREATE TABLE IF NOT EXISTS net.refill_node (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , geom         geometry(Point, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , internalnodeid                   int
  , externalcodeid                   int
  , externalnodename                 text
  , externalsignid                   int
  , addressid                        int
  , geomarktoptube                   double precision
  , geomarknodearea                  double precision
  , calcpressflow                    double precision
  , calcpressret                     double precision
  , picdate                          date
  , lastrepairdate                   date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , nodename                         text
  , scheme                           text
  , memo                             text
  , gpscoords                        text
  , belonghn                         text
  , passport                         text
  , inventnumber                     text
  , pipelinesign                     text
  , nodetypeid                       int
  , isbusy                           int
  , isloaded                         int
  , belongmagistralsite              int
  , belongdistsite                   int
  , organizationid                   int
  , magistralsite                    int
  , distsite                         int
  , registnumber                     text
  , refillexpend                     double precision
  , wdo                              double precision
  , refillloss                       double precision
  , diameterinternal                 double precision
  , watervolup                       double precision
  , watervoldown                     double precision
  , watervolupset                    double precision
  , potscount                        int
  , potssumvol                       double precision
  , potworkingsign                   text
  , chargeexpend                     double precision
  , dischargeexpend                  double precision
  , setpressret                      int
);
CREATE INDEX IF NOT EXISTS refill_node_geom_idx ON net.refill_node USING gist (geom);
CREATE INDEX IF NOT EXISTS refill_node_frag_idx ON net.refill_node (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS refill_node_src_uidx ON net.refill_node (src_id);
DROP TRIGGER IF EXISTS refill_node_reg ON net.refill_node;
CREATE TRIGGER refill_node_reg AFTER INSERT OR DELETE ON net.refill_node
    FOR EACH ROW EXECUTE FUNCTION net.reg_node_sync('refill_node');

-- valve_3way  <-  public.threewayvalves  (14 строк, class_node)
CREATE TABLE IF NOT EXISTS net.valve_3way (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , geom         geometry(Point, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , internalnodeid                   int
  , externalcodeid                   int
  , externalnodename                 text
  , externalsignid                   int
  , addressid                        int
  , geomarktoptube                   double precision
  , geomarknodearea                  double precision
  , calcpressflow                    double precision
  , calcpressret                     double precision
  , picdate                          date
  , lastrepairdate                   date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , nodename                         text
  , scheme                           text
  , memo                             text
  , gpscoords                        text
  , belonghn                         text
  , passport                         text
  , inventnumber                     text
  , pipelinesign                     text
  , nodetypeid                       int
  , isbusy                           int
  , isloaded                         int
  , belongmagistralsite              int
  , belongdistsite                   int
  , organizationid                   int
  , magistralsite                    int
  , distsite                         int
  , registnumber                     text
  , structure                        text
  , state                            text
  , purpose                          text
  , calcmixfactcoeff                 double precision
  , calcthrustlos                    double precision
  , expendcharstraight               text
  , expendcharvert                   text
  , authority                        double precision
  , calccapdeviation                 int
  , type                             text
  , capacity                         double precision
  , regulator                        text
  , maxpd                            double precision
  , diameternomin                    double precision
  , calcexpendnodemix                double precision
  , pumpstationname                  text
);
CREATE INDEX IF NOT EXISTS valve_3way_geom_idx ON net.valve_3way USING gist (geom);
CREATE INDEX IF NOT EXISTS valve_3way_frag_idx ON net.valve_3way (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS valve_3way_src_uidx ON net.valve_3way (src_id);
DROP TRIGGER IF EXISTS valve_3way_reg ON net.valve_3way;
CREATE TRIGGER valve_3way_reg AFTER INSERT OR DELETE ON net.valve_3way
    FOR EACH ROW EXECUTE FUNCTION net.reg_node_sync('valve_3way');

-- node_plain  <-  public.nodes  (остаток строк, class_node)
CREATE TABLE IF NOT EXISTS net.node_plain (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , geom         geometry(Point, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , internalnodeid                   int
  , externalcodeid                   int
  , externalnodename                 text
  , externalsignid                   int
  , addressid                        int
  , geomarktoptube                   double precision
  , geomarknodearea                  double precision
  , calcpressflow                    double precision
  , calcpressret                     double precision
  , picdate                          date
  , lastrepairdate                   date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , nodename                         text
  , scheme                           text
  , memo                             text
  , gpscoords                        text
  , belonghn                         text
  , passport                         text
  , inventnumber                     text
  , pipelinesign                     text
  , nodetypeid                       int
  , isbusy                           int
  , isloaded                         int
  , belongmagistralsite              int
  , belongdistsite                   int
  , organizationid                   int
  , magistralsite                    int
  , distsite                         int
  , registnumber                     text
);
CREATE INDEX IF NOT EXISTS node_plain_geom_idx ON net.node_plain USING gist (geom);
CREATE INDEX IF NOT EXISTS node_plain_frag_idx ON net.node_plain (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS node_plain_src_uidx ON net.node_plain (src_id);
DROP TRIGGER IF EXISTS node_plain_reg ON net.node_plain;
CREATE TRIGGER node_plain_reg AFTER INSERT OR DELETE ON net.node_plain
    FOR EACH ROW EXECUTE FUNCTION net.reg_node_sync('node_plain');

-- ============ ЛИНЕЙНЫЕ КЛАССЫ ============

-- air_heater  <-  public.airheaters  (91 строк, class_line)
CREATE TABLE IF NOT EXISTS net.air_heater (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_from_src int
  , node_to_src   int
  , fileid_src    int
  , geom         geometry(LineString, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , externalsignlineid               int
  , location                         text
  , hydrores                         double precision
  , organizationid                   int
  , registnum                        text
  , firstpicdate                     date
  , lastmaintdate                    date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , typ                              text
  , internalnodeid                   int
  , faninstall                       text
  , scheme                           text
  , airheaterscount                  double precision
  , rowscount                        double precision
  , storescount                      double precision
  , airheatertype                    text
  , contamincoeff                    double precision
  , presscoeff                       double precision
  , coords_legacy text GENERATED ALWAYS AS (net.geom_to_coords(geom, 100.0)) STORED
);
CREATE INDEX IF NOT EXISTS air_heater_geom_idx ON net.air_heater USING gist (geom);
CREATE INDEX IF NOT EXISTS air_heater_frag_idx ON net.air_heater (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS air_heater_src_uidx ON net.air_heater (src_id);
CREATE INDEX IF NOT EXISTS air_heater_from_idx ON net.air_heater (node_from);
CREATE INDEX IF NOT EXISTS air_heater_to_idx ON net.air_heater (node_to);
DROP TRIGGER IF EXISTS air_heater_reg ON net.air_heater;
CREATE TRIGGER air_heater_reg AFTER INSERT OR DELETE ON net.air_heater
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('air_heater');

-- damper  <-  public.dampers  (14868 строк, class_line)
CREATE TABLE IF NOT EXISTS net.damper (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_from_src int
  , node_to_src   int
  , fileid_src    int
  , geom         geometry(LineString, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , externalsignlineid               int
  , location                         text
  , hydrores                         double precision
  , organizationid                   int
  , registnum                        text
  , firstpicdate                     date
  , lastmaintdate                    date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , typ                              text
  , internalnodeid                   int
  , dispatcherswitch                 text
  , diametercondit                   double precision
  , partdempopen                     double precision
  , "name"                           text
  , standarddamplink                 int
  , turncount                        int
  , gatecontrol                      int
  , clue                             int
  , thrustcollar                     int
  , relatleakage                     double precision
  , opc                              text
  , damperarmaturestateid            int
  , coords_legacy text GENERATED ALWAYS AS (net.geom_to_coords(geom, 100.0)) STORED
);
CREATE INDEX IF NOT EXISTS damper_geom_idx ON net.damper USING gist (geom);
CREATE INDEX IF NOT EXISTS damper_frag_idx ON net.damper (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS damper_src_uidx ON net.damper (src_id);
CREATE INDEX IF NOT EXISTS damper_from_idx ON net.damper (node_from);
CREATE INDEX IF NOT EXISTS damper_to_idx ON net.damper (node_to);
DROP TRIGGER IF EXISTS damper_reg ON net.damper;
CREATE TRIGGER damper_reg AFTER INSERT OR DELETE ON net.damper
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('damper');

-- diaphragm  <-  public.diaphragms  (16718 строк, class_line)
CREATE TABLE IF NOT EXISTS net.diaphragm (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_from_src int
  , node_to_src   int
  , fileid_src    int
  , geom         geometry(LineString, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , externalsignlineid               int
  , location                         text
  , hydrores                         double precision
  , organizationid                   int
  , registnum                        text
  , firstpicdate                     date
  , lastmaintdate                    date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , typ                              text
  , internalnodeid                   int
  , throtdiaphloc                    text
  , diameterinternal                 double precision
  , consinstdiaphcount               int
  , entrymark                        text
  , stateid                          int
  , coords_legacy text GENERATED ALWAYS AS (net.geom_to_coords(geom, 100.0)) STORED
);
CREATE INDEX IF NOT EXISTS diaphragm_geom_idx ON net.diaphragm USING gist (geom);
CREATE INDEX IF NOT EXISTS diaphragm_frag_idx ON net.diaphragm (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS diaphragm_src_uidx ON net.diaphragm (src_id);
CREATE INDEX IF NOT EXISTS diaphragm_from_idx ON net.diaphragm (node_from);
CREATE INDEX IF NOT EXISTS diaphragm_to_idx ON net.diaphragm (node_to);
DROP TRIGGER IF EXISTS diaphragm_reg ON net.diaphragm;
CREATE TRIGGER diaphragm_reg AFTER INSERT OR DELETE ON net.diaphragm
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('diaphragm');

-- elevator  <-  public.elevators  (2754 строк, class_line)
CREATE TABLE IF NOT EXISTS net.elevator (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_from_src int
  , node_to_src   int
  , fileid_src    int
  , geom         geometry(LineString, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , externalsignlineid               int
  , location                         text
  , hydrores                         double precision
  , organizationid                   int
  , registnum                        text
  , firstpicdate                     date
  , lastmaintdate                    date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , typ                              text
  , internalnodeid                   int
  , elevatortype                     int
  , elevatornuminst                  int
  , diameternozzle                   double precision
  , entrymark                        text
  , diameterchamber                  double precision
  , length                           double precision
  , diameterinletflange              double precision
  , diameteroutletflange             double precision
  , diametersuctionpipe              double precision
  , material                         text
  , stateid                          int
  , coords_legacy text GENERATED ALWAYS AS (net.geom_to_coords(geom, 100.0)) STORED
);
CREATE INDEX IF NOT EXISTS elevator_geom_idx ON net.elevator USING gist (geom);
CREATE INDEX IF NOT EXISTS elevator_frag_idx ON net.elevator (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS elevator_src_uidx ON net.elevator (src_id);
CREATE INDEX IF NOT EXISTS elevator_from_idx ON net.elevator (node_from);
CREATE INDEX IF NOT EXISTS elevator_to_idx ON net.elevator (node_to);
DROP TRIGGER IF EXISTS elevator_reg ON net.elevator;
CREATE TRIGGER elevator_reg AFTER INSERT OR DELETE ON net.elevator
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('elevator');

-- heat_exchanger  <-  public.heatexchangers  (232 строк, class_line)
CREATE TABLE IF NOT EXISTS net.heat_exchanger (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_from_src int
  , node_to_src   int
  , fileid_src    int
  , geom         geometry(LineString, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , externalsignlineid               int
  , location                         text
  , hydrores                         double precision
  , organizationid                   int
  , registnum                        text
  , firstpicdate                     date
  , lastmaintdate                    date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , typ                              text
  , internalnodeid                   int
  , heatexchtype                     text
  , heatexchcode                     int
  , stateid                          int
  , coords_legacy text GENERATED ALWAYS AS (net.geom_to_coords(geom, 100.0)) STORED
);
CREATE INDEX IF NOT EXISTS heat_exchanger_geom_idx ON net.heat_exchanger USING gist (geom);
CREATE INDEX IF NOT EXISTS heat_exchanger_frag_idx ON net.heat_exchanger (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS heat_exchanger_src_uidx ON net.heat_exchanger (src_id);
CREATE INDEX IF NOT EXISTS heat_exchanger_from_idx ON net.heat_exchanger (node_from);
CREATE INDEX IF NOT EXISTS heat_exchanger_to_idx ON net.heat_exchanger (node_to);
DROP TRIGGER IF EXISTS heat_exchanger_reg ON net.heat_exchanger;
CREATE TRIGGER heat_exchanger_reg AFTER INSERT OR DELETE ON net.heat_exchanger
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('heat_exchanger');

-- pipe_section  <-  public.heatpipesections  (98019 строк, class_line)
CREATE TABLE IF NOT EXISTS net.pipe_section (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_from_src int
  , node_to_src   int
  , fileid_src    int
  , geom         geometry(LineString, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , externalsignlineid               int
  , location                         text
  , hydrores                         double precision
  , organizationid                   int
  , registnum                        text
  , firstpicdate                     date
  , lastmaintdate                    date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , typ                              text
  , internalnodeid                   int
  , pipesectionid                    int
  , pipesectstateidflow              int
  , pipesectstateidret               int
  , standardid                       int
  , standardtubelink                 int
  , tubescount                       int
  , diameterinternal                 double precision
  , diametercondit                   double precision
  , diameterexternal                 double precision
  , wallthickness                    double precision
  , pipesectlength                   double precision
  , tuberoughness                    double precision
  , locallosesshare                  double precision
  , localressum                      double precision
  , varcoeffidflow                   int
  , varcoeffidret                    int
  , calcheatlossignid                int
  , tubingtypeid                     int
  , piperemonttypeid                 int
  , channelid                        int
  , constrchanwidth                  double precision
  , constrchanheight                 double precision
  , heattestscoeff                   double precision
  , signnumwork                      int
  , isolmaterialid                   int
  , isolthickness                    double precision
  , isolmaterialhccoeff              double precision
  , pipelinelayingdepth              double precision
  , isolhtcoeffabove                 double precision
  , isolhtcoeffunder                 double precision
  , airgroundhtcoeffunder            double precision
  , groundhccoeff                    double precision
  , pipelineaxesdist                 double precision
  , damagenum                        int
  , lasttransdate                    date
  , lastisoldate                     date
  , repairdatecapital                date
  , picdatecapital                   date
  , repairdatemaint                  date
  , picdatemaint                     date
  , repairdateplantp                 date
  , firstpicdatehp                   date
  , lastmaintdatehp                  date
  , sectexploitperiod                int
  , buildingconstrstateid            int
  , specdamagecoeff                  double precision
  , specdamagecoeff2                 double precision
  , powcabinstcount10                int
  , powcabinstcount5                 int
  , powcabinstcount3                 int
  , powcabinstcount1                 int
  , powcabinstcount0                 int
  , powcabinterscount                int
  , gasecpcount10                    int
  , gasecpcount5                     int
  , gasecpcount3                     int
  , gasecpcount1                     int
  , gasecpcount0                     int
  , gasecpinterscount                int
  , eltranspcount10                  int
  , eltranspcount5                   int
  , eltranspcount3                   int
  , eltranspcount1                   int
  , eltranspcount0                   int
  , eltranspinterscount              int
  , potentialdifflevelid             int
  , corrosiondegoutid                int
  , corrosiondeginid                 int
  , floodintensitygwid               int
  , floodintensityfwid               int
  , floodintensitytwid               int
  , floodintensitybwid               int
  , jointstightnessinfr              int
  , chanconstrfract                  int
  , projsoldeviation                 int
  , constrbearreduce                 int
  , fixedsuppdestr                   int
  , sectwaterdumpid                  int
  , breakcomplexityid                int
  , breakconsumvolid                 int
  , erwdifficultyid                  int
  , tubingpedestrianid               int
  , tubingwayid                      int
  , populdamageid                    int
  , infrastrdamageid                 int
  , hydratestsdate                   date
  , sectexpend                       double precision
  , sectthrustloses                  double precision
  , heattestsdate                    date
  , temperoutair                     double precision
  , temperground                     double precision
  , expenddwflow                     double precision
  , expenddwret                      double precision
  , tempercoolflow                   double precision
  , tempercoolret                    double precision
  , temperdwflow                     double precision
  , temperdwret                      double precision
  , opc                              text
  , tubecharactid                    int
  , tubetypeid                       int
  , tubematerial                     text
  , tempermax                        double precision
  , factorymanufid                   int
  , externmaterialid                 int
  , isolationtypeid                  int
  , externcoverthick                 double precision
  , anticorrmaterialid               int
  , magistralsite                    int
  , distsite                         int
  , exploitreg                       int
  , net                              text
  , magistral                        int
  , exploitsite                      int
  , nettype                          int
  , crimpingquesite                  int
  , h                                double precision
  , deltah                           double precision
  , deltaq                           double precision
  , q                                double precision
  , primechanie                      text
  , "name"                           text
  , inventnumber                     text
  , objecttypeid                     int
  , sreda                            int
  , press                            double precision
  , temperature                      double precision
  , spoksluzhbirash                  int
  , ressurs                          int
  , pusk                             int
  , elektrich                        int
  , transportelekricht               int
  , ponezial                         int
  , vodootved                        int
  , slozhokluzh                      int
  , otkluzhgkal                      int
  , avariivipoln                     int
  , pesehod                          int
  , zhddorogi                        int
  , ludiproklad                      int
  , uzherbludi                       int
  , uzherbsity                       int
  , vnesniivid                       int
  , sostoborudovania                 int
  , vnesnkorrozia                    int
  , vnunrenkorrozia                  int
  , sostkonstrukz                    int
  , kategorii                        int
  , mestn                            text
  , coords_legacy text GENERATED ALWAYS AS (net.geom_to_coords(geom, 100.0)) STORED
);
CREATE INDEX IF NOT EXISTS pipe_section_geom_idx ON net.pipe_section USING gist (geom);
CREATE INDEX IF NOT EXISTS pipe_section_frag_idx ON net.pipe_section (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS pipe_section_src_uidx ON net.pipe_section (src_id);
CREATE INDEX IF NOT EXISTS pipe_section_from_idx ON net.pipe_section (node_from);
CREATE INDEX IF NOT EXISTS pipe_section_to_idx ON net.pipe_section (node_to);
DROP TRIGGER IF EXISTS pipe_section_reg ON net.pipe_section;
CREATE TRIGGER pipe_section_reg AFTER INSERT OR DELETE ON net.pipe_section
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('pipe_section');

-- local_resistance  <-  public.localhydroresistances2  (2 строк, class_line)
CREATE TABLE IF NOT EXISTS net.local_resistance (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_from_src int
  , node_to_src   int
  , fileid_src    int
  , geom         geometry(LineString, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , externalsignlineid               int
  , location                         text
  , hydrores                         double precision
  , organizationid                   int
  , registnum                        text
  , firstpicdate                     date
  , lastmaintdate                    date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , typ                              text
  , internalnodeid                   int
  , name_mest                        text
  , s_mest                           double precision
  , k_mest                           int
  , sum_mest                         double precision
  , coords_legacy text GENERATED ALWAYS AS (net.geom_to_coords(geom, 100.0)) STORED
);
CREATE INDEX IF NOT EXISTS local_resistance_geom_idx ON net.local_resistance USING gist (geom);
CREATE INDEX IF NOT EXISTS local_resistance_frag_idx ON net.local_resistance (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS local_resistance_src_uidx ON net.local_resistance (src_id);
CREATE INDEX IF NOT EXISTS local_resistance_from_idx ON net.local_resistance (node_from);
CREATE INDEX IF NOT EXISTS local_resistance_to_idx ON net.local_resistance (node_to);
DROP TRIGGER IF EXISTS local_resistance_reg ON net.local_resistance;
CREATE TRIGGER local_resistance_reg AFTER INSERT OR DELETE ON net.local_resistance
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('local_resistance');

-- regulator_press  <-  public.pressregulators  (509 строк, class_line)
CREATE TABLE IF NOT EXISTS net.regulator_press (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_from_src int
  , node_to_src   int
  , fileid_src    int
  , geom         geometry(LineString, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , externalsignlineid               int
  , location                         text
  , hydrores                         double precision
  , organizationid                   int
  , registnum                        text
  , firstpicdate                     date
  , lastmaintdate                    date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , typ                              text
  , internalnodeid                   int
  , nodeid                           int
  , valvehydroresopen                double precision
  , valvehydroresclose               double precision
  , regvalverelcap                   int
  , relleakage                       double precision
  , consdrip                         double precision
  , workattrid                       int
  , deltah                           double precision
  , regulatorstateid                 int
  , h                                double precision
  , pipelinesignid                   int
  , coords_legacy text GENERATED ALWAYS AS (net.geom_to_coords(geom, 100.0)) STORED
);
CREATE INDEX IF NOT EXISTS regulator_press_geom_idx ON net.regulator_press USING gist (geom);
CREATE INDEX IF NOT EXISTS regulator_press_frag_idx ON net.regulator_press (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS regulator_press_src_uidx ON net.regulator_press (src_id);
CREATE INDEX IF NOT EXISTS regulator_press_from_idx ON net.regulator_press (node_from);
CREATE INDEX IF NOT EXISTS regulator_press_to_idx ON net.regulator_press (node_to);
DROP TRIGGER IF EXISTS regulator_press_reg ON net.regulator_press;
CREATE TRIGGER regulator_press_reg AFTER INSERT OR DELETE ON net.regulator_press
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('regulator_press');

-- pump  <-  public.pumps  (286 строк, class_line)
CREATE TABLE IF NOT EXISTS net.pump (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_from_src int
  , node_to_src   int
  , fileid_src    int
  , geom         geometry(LineString, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , externalsignlineid               int
  , location                         text
  , hydrores                         double precision
  , organizationid                   int
  , registnum                        text
  , firstpicdate                     date
  , lastmaintdate                    date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , typ                              text
  , internalnodeid                   int
  , offreason                        int
  , pumpstationid                    text
  , number                           text
  , thrust                           double precision
  , standardpumpid                   int
  , parallagregcount                 int
  , drivetypeid                      int
  , rotordiametertypeid              int
  , standardemid                     int
  , r0                               double precision
  , r1                               double precision
  , r2                               double precision
  , e0                               double precision
  , e1                               double precision
  , e2                               double precision
  , k0                               double precision
  , k1                               double precision
  , k2                               double precision
  , r0_z                             double precision
  , r1_z                             double precision
  , r2_z                             double precision
  , e0_z                             double precision
  , e1_z                             double precision
  , e2_z                             double precision
  , k0_z                             double precision
  , k1_z                             double precision
  , k2_z                             double precision
  , rotorrotspeedset                 double precision
  , rotordiameterset                 double precision
  , lastpumpreplacedate              date
  , lastemreplacedate                date
  , lastagregreplacedate             date
  , repaircountpump                  int
  , replacecountagreg                int
  , repaircountem                    int
  , opc                              text
  , stateid                          int
  , coords_legacy text GENERATED ALWAYS AS (net.geom_to_coords(geom, 100.0)) STORED
);
CREATE INDEX IF NOT EXISTS pump_geom_idx ON net.pump USING gist (geom);
CREATE INDEX IF NOT EXISTS pump_frag_idx ON net.pump (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS pump_src_uidx ON net.pump (src_id);
CREATE INDEX IF NOT EXISTS pump_from_idx ON net.pump (node_from);
CREATE INDEX IF NOT EXISTS pump_to_idx ON net.pump (node_to);
DROP TRIGGER IF EXISTS pump_reg ON net.pump;
CREATE TRIGGER pump_reg AFTER INSERT OR DELETE ON net.pump
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('pump');

-- radiator  <-  public.systemradiators  (622 строк, class_line)
CREATE TABLE IF NOT EXISTS net.radiator (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_from_src int
  , node_to_src   int
  , fileid_src    int
  , geom         geometry(LineString, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , externalsignlineid               int
  , location                         text
  , hydrores                         double precision
  , organizationid                   int
  , registnum                        text
  , firstpicdate                     date
  , lastmaintdate                    date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , typ                              text
  , internalnodeid                   int
  , "name"                           text
  , type                             text
  , count                            text
  , totalequivsurface                text
  , stateid                          int
  , coords_legacy text GENERATED ALWAYS AS (net.geom_to_coords(geom, 100.0)) STORED
);
CREATE INDEX IF NOT EXISTS radiator_geom_idx ON net.radiator USING gist (geom);
CREATE INDEX IF NOT EXISTS radiator_frag_idx ON net.radiator (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS radiator_src_uidx ON net.radiator (src_id);
CREATE INDEX IF NOT EXISTS radiator_from_idx ON net.radiator (node_from);
CREATE INDEX IF NOT EXISTS radiator_to_idx ON net.radiator (node_to);
DROP TRIGGER IF EXISTS radiator_reg ON net.radiator;
CREATE TRIGGER radiator_reg AFTER INSERT OR DELETE ON net.radiator
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('radiator');

-- line_plain  <-  public.linesobj  (остаток строк, class_line)
CREATE TABLE IF NOT EXISTS net.line_plain (
    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , fragment_id  int REFERENCES net.fragment(id)
  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE RESTRICT
  , node_from_src int
  , node_to_src   int
  , fileid_src    int
  , geom         geometry(LineString, 9998) NOT NULL
  , removed_at   timestamptz
  , src_id       int
  , externalsignlineid               int
  , location                         text
  , hydrores                         double precision
  , organizationid                   int
  , registnum                        text
  , firstpicdate                     date
  , lastmaintdate                    date
  , displaysign                      int
  , archivechangedate                timestamp
  , operatorid                       int
  , typ                              text
  , internalnodeid                   int
  , coords_legacy text GENERATED ALWAYS AS (net.geom_to_coords(geom, 100.0)) STORED
);
CREATE INDEX IF NOT EXISTS line_plain_geom_idx ON net.line_plain USING gist (geom);
CREATE INDEX IF NOT EXISTS line_plain_frag_idx ON net.line_plain (fragment_id) WHERE removed_at IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS line_plain_src_uidx ON net.line_plain (src_id);
CREATE INDEX IF NOT EXISTS line_plain_from_idx ON net.line_plain (node_from);
CREATE INDEX IF NOT EXISTS line_plain_to_idx ON net.line_plain (node_to);
DROP TRIGGER IF EXISTS line_plain_reg ON net.line_plain;
CREATE TRIGGER line_plain_reg AFTER INSERT OR DELETE ON net.line_plain
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('line_plain');

-- ============ САМОСТОЯТЕЛЬНЫЕ СЛОИ ============

-- repair_section  <-  public.kapremont_uchastki_remonta  (5 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.repair_section (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , id_remont                        int
  , idp1                             int
  , ido1                             int
  , idp2                             int
  , ido2                             int
);
CREATE INDEX IF NOT EXISTS repair_section_geom_idx ON net.repair_section USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS repair_section_src_uidx ON net.repair_section (src_id);

-- manhole  <-  public.lyuki  (18392 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.manhole (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , "name"                           text
  , elevation_ktp                    double precision
  , elevation_tr                     double precision
  , elevation_tr_2                   double precision
  , elevation_tr_3                   double precision
  , ground_control                   text
  , territoryid                      int
  , god_sdachi_expl                  text
  , srok_expl                        text
  , date_autor                       date
  , podtip                           int
  , nomer                            text
  , type                             text
  , owner                            text
  , year_overhaul                    text
  , note                             text
);
CREATE INDEX IF NOT EXISTS manhole_geom_idx ON net.manhole USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS manhole_src_uidx ON net.manhole (src_id);

-- support  <-  public.opora  (3611 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.support (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , mestoraspolozhenie               int
  , priznak_truboprovoda             int
  , vysotnaya_otmetka                double precision
  , stroitelnaya_konstruktsiya       int
  , nomer_opory                      text
  , prinadlezhnost                   text
  , vid_opory                        int
  , shema                            text
  , primechanie                      text
  , vysota_stroitelnaya_konstruktsi  double precision
  , geodezicheskaya_otmetka          double precision
  , naimenovanie_nachalnogo_uzla_truby text
  , naimenovanie_konechnogo_uzla_truby text
  , naimenovanie_uzla_kamery_pavilona text
  , nomer_shvellera                  text
  , kolichestvo_uporov               int
  , constructiontypesid              int
  , angle                            double precision
);
CREATE INDEX IF NOT EXISTS support_geom_idx ON net.support USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS support_src_uidx ON net.support (src_id);

-- pressure_test_defect  <-  public.opressovka_defekt  (1 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.pressure_test_defect (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , opressovki_uchastok_ocheredi     int
  , data_provedeniya                 date
  , kod_rs_nachalnoy_kamery          text
  , nachalnaya_kamera                text
  , kod_rs_konechnoy_kamery          text
  , konechnaya_kamera                text
);
CREATE INDEX IF NOT EXISTS pressure_test_defect_geom_idx ON net.pressure_test_defect USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS pressure_test_defect_src_uidx ON net.pressure_test_defect (src_id);

-- diameter_change  <-  public.perehod_diametra  (4 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.diameter_change (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , priznak_truboprovoda             int
  , vysotnaya_otmetka                double precision
  , nachalnyy_diametr                int
  , konechnyy_diametr                int
  , mesto_raspolozheniya             text
  , primechanie                      text
  , kod                              text
  , uzel                             text
  , id_geopoint                      int
  , prinadlezhnost                   text
  , geodezicheskaya_otmetka          double precision
  , naimenovanie_nachalnogo_uzla_truby text
  , angle                            double precision
);
CREATE INDEX IF NOT EXISTS diameter_change_geom_idx ON net.diameter_change USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS diameter_change_src_uidx ON net.diameter_change (src_id);

-- pipe_reconstructed  <-  public.truby_rekonstruiruemye  (44 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.pipe_reconstructed (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , sostoyanie_stroitelstva          text
  , gorod                            text
  , mikrorayon                       text
  , ulitsa                           text
  , dom                              text
  , kommentariy                      text
  , nachalnaya_kamera                text
  , konechnaya_kamera                text
  , nomer_tu                         text
  , data_vydachi_tu                  date
  , sostoyanie_dogovora              text
  , naimenovanie_organizatsii__zaprashivayuschey_tu text
  , naimenovanie_obekta              text
  , rayon_ekspluatatsii              text
  , istochnik                        text
  , dlina                            double precision
  , diametr_uslovnyy                 double precision
  , diametr_vnutrenniy               double precision
  , tolschina_stenki                 double precision
  , tip_truby                        text
  , vid_prokladki                    text
  , harakteristika_truby             text
);
CREATE INDEX IF NOT EXISTS pipe_reconstructed_geom_idx ON net.pipe_reconstructed USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS pipe_reconstructed_src_uidx ON net.pipe_reconstructed (src_id);

-- pipe_turn  <-  public.ugol_povorota_truboprovoda  (25214 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.pipe_turn (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , priznak_truboprovoda             int
  , kod1                             text
  , uzel1                            text
  , kod2                             text
  , uzel2                            text
  , diametr_truboprovoda             text
  , ugol_povorota                    int
  , primechanie                      text
  , vysotnaya_otmetka_ktp            double precision
  , vysotnaya_otmetka_truby          double precision
  , geodezicheskaya_organizatsiya    text
  , god_sdachi_v_ekspluatatsiyu      text
  , geodezicheskaya_otmetka          double precision
  , naimenovanie_nachalnogo_uzla_truby text
  , naimenovanie_konechnogo_uzla_truby text
  , vysotnaya_otmetka                double precision
  , id_geopoint                      int
  , angle                            double precision
);
CREATE INDEX IF NOT EXISTS pipe_turn_geom_idx ON net.pipe_turn USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS pipe_turn_src_uidx ON net.pipe_turn (src_id);

-- building_inlet  <-  public.vvod_v_zdanie  (5463 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.building_inlet (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , "name"                           text
  , elevation_ktp                    double precision
  , elevation_tr                     double precision
  , ground_control                   text
  , territoryid                      int
  , god_sdachi_expl                  text
  , srok_expl                        text
  , date_autor                       date
  , podtip                           int
  , note                             text
  , heat                             double precision
  , gvs                              double precision
  , vent                             double precision
);
CREATE INDEX IF NOT EXISTS building_inlet_geom_idx ON net.building_inlet USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS building_inlet_src_uidx ON net.building_inlet (src_id);

-- building_inlet_line  <-  public.vvody_v_zdanie  (5463 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.building_inlet_line (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , vysotnaya_otmetka                double precision
  , primechanie                      text
  , sposob_vvoda                     int
  , angle                            double precision
);
CREATE INDEX IF NOT EXISTS building_inlet_line_geom_idx ON net.building_inlet_line USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS building_inlet_line_src_uidx ON net.building_inlet_line (src_id);

-- building  <-  public.zdaniya_2  (187203 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.building (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , objectid                         numeric
  , floor                            text
  , number_1                         text
  , id_adr_mas                       text
  , id_admraio                       numeric
  , year_of_fo                       text
  , street_nam                       text
  , shape_leng                       numeric
  , shape_area                       numeric
  , otop                             double precision
  , gvs                              double precision
  , vent                             double precision
  , par                              double precision
  , nagr                             double precision
  , txt                              text
  , mkr2                             text
  , street2                          text
  , house2                           text
  , otop_cxema                       int
  , gvs_cxema                        int
  , potrebitel                       text
);
CREATE INDEX IF NOT EXISTS building_geom_idx ON net.building USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS building_src_uidx ON net.building (src_id);

-- building_consumer  <-  public.zdaniya_potrebiteley  (12466 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.building_consumer (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , gorod                            text
  , mikrorayon                       text
  , ulitsa                           text
  , dom                              text
  , kod_rs_uzla_prisoedineniya       text
  , uzel_prisoedineniya              text
);
CREATE INDEX IF NOT EXISTS building_consumer_geom_idx ON net.building_consumer USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS building_consumer_src_uidx ON net.building_consumer (src_id);

-- building_tu  <-  public.zdaniya_tu  (5197 строк, самостоятельный слой)
CREATE TABLE IF NOT EXISTS net.building_tu (
    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')
  , geom    geometry(Geometry, 9998) NOT NULL
  , src_id  int
  , gorod                            text
  , mikrorayon                       text
  , ulitsa                           text
  , dom                              text
  , kommentariy                      text
  , istochnik                        int
  , rayon_ekspluatatsii              int
  , kod_rs_uzla_prisoedineniya       text
  , uzel_prisoedineniya              text
);
CREATE INDEX IF NOT EXISTS building_tu_geom_idx ON net.building_tu USING gist (geom);
CREATE UNIQUE INDEX IF NOT EXISTS building_tu_src_uidx ON net.building_tu (src_id);

-- ============ АСПЕКТЫ И ДОЧЕРНИЕ ТАБЛИЦЫ ============

-- node_press_setting  <-  public.setpressnodes  (5740 строк, aspect к узлу)
CREATE TABLE IF NOT EXISTS net.node_press_setting (
    id      bigserial PRIMARY KEY
  , node_id bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE CASCADE
  , src_id  int
  , pressflow                        double precision
  , pressret                         double precision
  , fragment_resultid                int
  , file_result                      text
  , kod_m                            text
  , uzel_m                           text
  , fileid                           int
);
CREATE INDEX IF NOT EXISTS node_press_setting_node_idx ON net.node_press_setting (node_id);
CREATE UNIQUE INDEX IF NOT EXISTS node_press_setting_node_uidx ON net.node_press_setting (node_id);

-- node_direction  <-  public.deployeddirections  (2210 строк, child к узлу)
CREATE TABLE IF NOT EXISTS net.node_direction (
    id      bigserial PRIMARY KEY
  , node_id bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE CASCADE
  , src_id  int
  , directionid                      int
);
CREATE INDEX IF NOT EXISTS node_direction_node_idx ON net.node_direction (node_id);

-- node_metering_device  <-  public.wdodevices  (3385 строк, child к узлу)
CREATE TABLE IF NOT EXISTS net.node_metering_device (
    id      bigserial PRIMARY KEY
  , node_id bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE CASCADE
  , src_id  int
  , externalsignid                   int
  , wdo                              double precision
  , hydrores                         double precision
);
CREATE INDEX IF NOT EXISTS node_metering_device_node_idx ON net.node_metering_device (node_id);
