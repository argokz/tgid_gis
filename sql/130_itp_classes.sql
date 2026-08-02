-- Объектные таблицы net для оборудования, оставшегося без класса.
--
-- СГЕНЕРИРОВАНО tools/gen_line_class.py по образцу net.damper:
-- общая часть скопирована с образца, собственные колонки взяты
-- из исходной таблицы подтипа. Руками этот файл не правят.
--
-- Данные НЕ переносятся. В источнике этих строк 0, 0, 0, 7 и 13,
-- и все они относятся к оборудованию с пустыми концами, которое
-- не видело и старое приложение (docs/14, раздел про чердак).
-- Структура создаётся, чтобы объект имел своё место в модели;
-- наполнение — отдельное решение заказчика.

BEGIN;

-- bypass: 14 собственных колонок из public.bypass
CREATE TABLE IF NOT EXISTS net.bypass (
    id                     bigint DEFAULT nextval('obj_id_seq'::regclass) NOT NULL,
    fragment_id            integer,
    node_from              bigint NOT NULL,
    node_to                bigint NOT NULL,
    node_from_src          integer,
    node_to_src            integer,
    fileid_src             integer,
    geom                   geometry(LineString, 9998) NOT NULL,
    removed_at             timestamp with time zone,
    src_id                 integer,
    externalsignlineid     integer,
    location               text,
    hydrores               double precision,
    organizationid         integer,
    registnum              text,
    firstpicdate           date,
    lastmaintdate          date,
    displaysign            integer,
    archivechangedate      timestamp without time zone,
    operatorid             integer,
    typ                    text,
    internalnodeid         integer,
    nodeid                 integer,
    standardid             integer DEFAULT 1,
    standardtubelink       integer,
    length                 integer DEFAULT 1,
    diameterinternal       double precision DEFAULT 400,
    tuberoughness          double precision DEFAULT 0.5,
    rescoeffssum           double precision DEFAULT 0,
    locinstall             varchar(50) DEFAULT NULL::character varying,
    h                      double precision DEFAULT 0,
    deltah                 double precision DEFAULT 0,
    deltaq                 double precision DEFAULT 0,
    q                      double precision DEFAULT 0,
    regulatorstateid       integer DEFAULT 1,
    pipelinesignid         integer,
    coords_legacy          text,
    subtype_src_id         integer,
    row_version            bigint DEFAULT 1 NOT NULL,
    updated_at             timestamp with time zone DEFAULT clock_timestamp() NOT NULL,
    updated_by             text DEFAULT CURRENT_USER NOT NULL
);

ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS nodeid                 integer;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS standardid             integer DEFAULT 1;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS standardtubelink       integer;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS length                 integer DEFAULT 1;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS diameterinternal       double precision DEFAULT 400;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS tuberoughness          double precision DEFAULT 0.5;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS rescoeffssum           double precision DEFAULT 0;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS locinstall             varchar(50) DEFAULT NULL::character varying;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS h                      double precision DEFAULT 0;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS deltah                 double precision DEFAULT 0;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS deltaq                 double precision DEFAULT 0;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS q                      double precision DEFAULT 0;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS regulatorstateid       integer DEFAULT 1;
ALTER TABLE net.bypass ADD COLUMN IF NOT EXISTS pipelinesignid         integer;

ALTER TABLE net.bypass DROP CONSTRAINT IF EXISTS bypass_pkey;
ALTER TABLE net.bypass
    ADD CONSTRAINT bypass_pkey PRIMARY KEY (id);
ALTER TABLE net.bypass DROP CONSTRAINT IF EXISTS bypass_fragment_id_fkey;
ALTER TABLE net.bypass
    ADD CONSTRAINT bypass_fragment_id_fkey
    FOREIGN KEY (fragment_id) REFERENCES net.fragment(id);
ALTER TABLE net.bypass DROP CONSTRAINT IF EXISTS bypass_node_from_fkey;
ALTER TABLE net.bypass
    ADD CONSTRAINT bypass_node_from_fkey
    FOREIGN KEY (node_from) REFERENCES net.node_reg(id) ON DELETE RESTRICT;
ALTER TABLE net.bypass DROP CONSTRAINT IF EXISTS bypass_node_to_fkey;
ALTER TABLE net.bypass
    ADD CONSTRAINT bypass_node_to_fkey
    FOREIGN KEY (node_to) REFERENCES net.node_reg(id) ON DELETE RESTRICT;

CREATE INDEX IF NOT EXISTS bypass_geom_idx ON net.bypass USING gist (geom);
CREATE INDEX IF NOT EXISTS bypass_fragment_id_idx ON net.bypass (fragment_id);

DROP TRIGGER IF EXISTS bypass_reg ON net.bypass;
CREATE TRIGGER bypass_reg AFTER INSERT OR DELETE ON net.bypass
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('bypass');
DROP TRIGGER IF EXISTS object_touch ON net.bypass;
CREATE TRIGGER object_touch BEFORE UPDATE ON net.bypass
    FOR EACH ROW EXECUTE FUNCTION net.touch_object_row();
DROP TRIGGER IF EXISTS object_insert_log ON net.bypass;
CREATE TRIGGER object_insert_log AFTER INSERT ON net.bypass
    FOR EACH ROW EXECUTE FUNCTION net.log_object_insert();
DROP TRIGGER IF EXISTS line_topology ON net.bypass;
CREATE TRIGGER line_topology BEFORE INSERT OR UPDATE OF node_from, node_to, fragment_id, geom, removed_at
    ON net.bypass FOR EACH ROW
    EXECUTE FUNCTION net.validate_line_topology();
DROP TRIGGER IF EXISTS geometry_change_audit ON net.bypass;
CREATE TRIGGER geometry_change_audit AFTER UPDATE OF geom ON net.bypass
    FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change();

-- regulator_consumption: 11 собственных колонок из public.consumptregulators
CREATE TABLE IF NOT EXISTS net.regulator_consumption (
    id                     bigint DEFAULT nextval('obj_id_seq'::regclass) NOT NULL,
    fragment_id            integer,
    node_from              bigint NOT NULL,
    node_to                bigint NOT NULL,
    node_from_src          integer,
    node_to_src            integer,
    fileid_src             integer,
    geom                   geometry(LineString, 9998) NOT NULL,
    removed_at             timestamp with time zone,
    src_id                 integer,
    externalsignlineid     integer,
    location               text,
    hydrores               double precision,
    organizationid         integer,
    registnum              text,
    firstpicdate           date,
    lastmaintdate          date,
    displaysign            integer,
    archivechangedate      timestamp without time zone,
    operatorid             integer,
    typ                    text,
    internalnodeid         integer,
    nodeid                 integer,
    regconsmean            double precision DEFAULT 0,
    workattrid             integer,
    hydroresopen           double precision DEFAULT 0,
    hydroresclose          double precision DEFAULT 0,
    regvalvecap            integer,
    relatleakage           double precision DEFAULT 0,
    plumsconsumption       double precision DEFAULT 0,
    opc                    varchar(50) DEFAULT NULL::character varying,
    deltah                 double precision DEFAULT 0,
    regulatorstateid       integer DEFAULT 1,
    coords_legacy          text,
    subtype_src_id         integer,
    row_version            bigint DEFAULT 1 NOT NULL,
    updated_at             timestamp with time zone DEFAULT clock_timestamp() NOT NULL,
    updated_by             text DEFAULT CURRENT_USER NOT NULL
);

ALTER TABLE net.regulator_consumption ADD COLUMN IF NOT EXISTS nodeid                 integer;
ALTER TABLE net.regulator_consumption ADD COLUMN IF NOT EXISTS regconsmean            double precision DEFAULT 0;
ALTER TABLE net.regulator_consumption ADD COLUMN IF NOT EXISTS workattrid             integer;
ALTER TABLE net.regulator_consumption ADD COLUMN IF NOT EXISTS hydroresopen           double precision DEFAULT 0;
ALTER TABLE net.regulator_consumption ADD COLUMN IF NOT EXISTS hydroresclose          double precision DEFAULT 0;
ALTER TABLE net.regulator_consumption ADD COLUMN IF NOT EXISTS regvalvecap            integer;
ALTER TABLE net.regulator_consumption ADD COLUMN IF NOT EXISTS relatleakage           double precision DEFAULT 0;
ALTER TABLE net.regulator_consumption ADD COLUMN IF NOT EXISTS plumsconsumption       double precision DEFAULT 0;
ALTER TABLE net.regulator_consumption ADD COLUMN IF NOT EXISTS opc                    varchar(50) DEFAULT NULL::character varying;
ALTER TABLE net.regulator_consumption ADD COLUMN IF NOT EXISTS deltah                 double precision DEFAULT 0;
ALTER TABLE net.regulator_consumption ADD COLUMN IF NOT EXISTS regulatorstateid       integer DEFAULT 1;

ALTER TABLE net.regulator_consumption DROP CONSTRAINT IF EXISTS regulator_consumption_pkey;
ALTER TABLE net.regulator_consumption
    ADD CONSTRAINT regulator_consumption_pkey PRIMARY KEY (id);
ALTER TABLE net.regulator_consumption DROP CONSTRAINT IF EXISTS regulator_consumption_fragment_id_fkey;
ALTER TABLE net.regulator_consumption
    ADD CONSTRAINT regulator_consumption_fragment_id_fkey
    FOREIGN KEY (fragment_id) REFERENCES net.fragment(id);
ALTER TABLE net.regulator_consumption DROP CONSTRAINT IF EXISTS regulator_consumption_node_from_fkey;
ALTER TABLE net.regulator_consumption
    ADD CONSTRAINT regulator_consumption_node_from_fkey
    FOREIGN KEY (node_from) REFERENCES net.node_reg(id) ON DELETE RESTRICT;
ALTER TABLE net.regulator_consumption DROP CONSTRAINT IF EXISTS regulator_consumption_node_to_fkey;
ALTER TABLE net.regulator_consumption
    ADD CONSTRAINT regulator_consumption_node_to_fkey
    FOREIGN KEY (node_to) REFERENCES net.node_reg(id) ON DELETE RESTRICT;

CREATE INDEX IF NOT EXISTS regulator_consumption_geom_idx ON net.regulator_consumption USING gist (geom);
CREATE INDEX IF NOT EXISTS regulator_consumption_fragment_id_idx ON net.regulator_consumption (fragment_id);

DROP TRIGGER IF EXISTS regulator_consumption_reg ON net.regulator_consumption;
CREATE TRIGGER regulator_consumption_reg AFTER INSERT OR DELETE ON net.regulator_consumption
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('regulator_consumption');
DROP TRIGGER IF EXISTS object_touch ON net.regulator_consumption;
CREATE TRIGGER object_touch BEFORE UPDATE ON net.regulator_consumption
    FOR EACH ROW EXECUTE FUNCTION net.touch_object_row();
DROP TRIGGER IF EXISTS object_insert_log ON net.regulator_consumption;
CREATE TRIGGER object_insert_log AFTER INSERT ON net.regulator_consumption
    FOR EACH ROW EXECUTE FUNCTION net.log_object_insert();
DROP TRIGGER IF EXISTS line_topology ON net.regulator_consumption;
CREATE TRIGGER line_topology BEFORE INSERT OR UPDATE OF node_from, node_to, fragment_id, geom, removed_at
    ON net.regulator_consumption FOR EACH ROW
    EXECUTE FUNCTION net.validate_line_topology();
DROP TRIGGER IF EXISTS geometry_change_audit ON net.regulator_consumption;
CREATE TRIGGER geometry_change_audit AFTER UPDATE OF geom ON net.regulator_consumption
    FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change();

-- regulator_pressdrop: 11 собственных колонок из public.pressdropregulators
CREATE TABLE IF NOT EXISTS net.regulator_pressdrop (
    id                     bigint DEFAULT nextval('obj_id_seq'::regclass) NOT NULL,
    fragment_id            integer,
    node_from              bigint NOT NULL,
    node_to                bigint NOT NULL,
    node_from_src          integer,
    node_to_src            integer,
    fileid_src             integer,
    geom                   geometry(LineString, 9998) NOT NULL,
    removed_at             timestamp with time zone,
    src_id                 integer,
    externalsignlineid     integer,
    location               text,
    hydrores               double precision,
    organizationid         integer,
    registnum              text,
    firstpicdate           date,
    lastmaintdate          date,
    displaysign            integer,
    archivechangedate      timestamp without time zone,
    operatorid             integer,
    typ                    text,
    internalnodeid         integer,
    nodeid                 integer,
    pressdropmean          double precision DEFAULT 0,
    regvalvehydrores       double precision DEFAULT 0,
    consthroughregvalve    double precision DEFAULT 0,
    thrustdropmean         double precision DEFAULT 0,
    regvalverelcap         integer,
    maxleakageclosevalve   double precision DEFAULT 0,
    consdrip               double precision DEFAULT 0,
    workattrid             integer,
    deltah                 double precision DEFAULT 0,
    regulatorstateid       integer DEFAULT 1,
    coords_legacy          text,
    subtype_src_id         integer,
    row_version            bigint DEFAULT 1 NOT NULL,
    updated_at             timestamp with time zone DEFAULT clock_timestamp() NOT NULL,
    updated_by             text DEFAULT CURRENT_USER NOT NULL
);

ALTER TABLE net.regulator_pressdrop ADD COLUMN IF NOT EXISTS nodeid                 integer;
ALTER TABLE net.regulator_pressdrop ADD COLUMN IF NOT EXISTS pressdropmean          double precision DEFAULT 0;
ALTER TABLE net.regulator_pressdrop ADD COLUMN IF NOT EXISTS regvalvehydrores       double precision DEFAULT 0;
ALTER TABLE net.regulator_pressdrop ADD COLUMN IF NOT EXISTS consthroughregvalve    double precision DEFAULT 0;
ALTER TABLE net.regulator_pressdrop ADD COLUMN IF NOT EXISTS thrustdropmean         double precision DEFAULT 0;
ALTER TABLE net.regulator_pressdrop ADD COLUMN IF NOT EXISTS regvalverelcap         integer;
ALTER TABLE net.regulator_pressdrop ADD COLUMN IF NOT EXISTS maxleakageclosevalve   double precision DEFAULT 0;
ALTER TABLE net.regulator_pressdrop ADD COLUMN IF NOT EXISTS consdrip               double precision DEFAULT 0;
ALTER TABLE net.regulator_pressdrop ADD COLUMN IF NOT EXISTS workattrid             integer;
ALTER TABLE net.regulator_pressdrop ADD COLUMN IF NOT EXISTS deltah                 double precision DEFAULT 0;
ALTER TABLE net.regulator_pressdrop ADD COLUMN IF NOT EXISTS regulatorstateid       integer DEFAULT 1;

ALTER TABLE net.regulator_pressdrop DROP CONSTRAINT IF EXISTS regulator_pressdrop_pkey;
ALTER TABLE net.regulator_pressdrop
    ADD CONSTRAINT regulator_pressdrop_pkey PRIMARY KEY (id);
ALTER TABLE net.regulator_pressdrop DROP CONSTRAINT IF EXISTS regulator_pressdrop_fragment_id_fkey;
ALTER TABLE net.regulator_pressdrop
    ADD CONSTRAINT regulator_pressdrop_fragment_id_fkey
    FOREIGN KEY (fragment_id) REFERENCES net.fragment(id);
ALTER TABLE net.regulator_pressdrop DROP CONSTRAINT IF EXISTS regulator_pressdrop_node_from_fkey;
ALTER TABLE net.regulator_pressdrop
    ADD CONSTRAINT regulator_pressdrop_node_from_fkey
    FOREIGN KEY (node_from) REFERENCES net.node_reg(id) ON DELETE RESTRICT;
ALTER TABLE net.regulator_pressdrop DROP CONSTRAINT IF EXISTS regulator_pressdrop_node_to_fkey;
ALTER TABLE net.regulator_pressdrop
    ADD CONSTRAINT regulator_pressdrop_node_to_fkey
    FOREIGN KEY (node_to) REFERENCES net.node_reg(id) ON DELETE RESTRICT;

CREATE INDEX IF NOT EXISTS regulator_pressdrop_geom_idx ON net.regulator_pressdrop USING gist (geom);
CREATE INDEX IF NOT EXISTS regulator_pressdrop_fragment_id_idx ON net.regulator_pressdrop (fragment_id);

DROP TRIGGER IF EXISTS regulator_pressdrop_reg ON net.regulator_pressdrop;
CREATE TRIGGER regulator_pressdrop_reg AFTER INSERT OR DELETE ON net.regulator_pressdrop
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('regulator_pressdrop');
DROP TRIGGER IF EXISTS object_touch ON net.regulator_pressdrop;
CREATE TRIGGER object_touch BEFORE UPDATE ON net.regulator_pressdrop
    FOR EACH ROW EXECUTE FUNCTION net.touch_object_row();
DROP TRIGGER IF EXISTS object_insert_log ON net.regulator_pressdrop;
CREATE TRIGGER object_insert_log AFTER INSERT ON net.regulator_pressdrop
    FOR EACH ROW EXECUTE FUNCTION net.log_object_insert();
DROP TRIGGER IF EXISTS line_topology ON net.regulator_pressdrop;
CREATE TRIGGER line_topology BEFORE INSERT OR UPDATE OF node_from, node_to, fragment_id, geom, removed_at
    ON net.regulator_pressdrop FOR EACH ROW
    EXECUTE FUNCTION net.validate_line_topology();
DROP TRIGGER IF EXISTS geometry_change_audit ON net.regulator_pressdrop;
CREATE TRIGGER geometry_change_audit AFTER UPDATE OF geom ON net.regulator_pressdrop
    FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change();

-- armature_control: 18 собственных колонок из public.regularmatures
CREATE TABLE IF NOT EXISTS net.armature_control (
    id                     bigint DEFAULT nextval('obj_id_seq'::regclass) NOT NULL,
    fragment_id            integer,
    node_from              bigint NOT NULL,
    node_to                bigint NOT NULL,
    node_from_src          integer,
    node_to_src            integer,
    fileid_src             integer,
    geom                   geometry(LineString, 9998) NOT NULL,
    removed_at             timestamp with time zone,
    src_id                 integer,
    externalsignlineid     integer,
    location               text,
    hydrores               double precision,
    organizationid         integer,
    registnum              text,
    firstpicdate           date,
    lastmaintdate          date,
    displaysign            integer,
    archivechangedate      timestamp without time zone,
    operatorid             integer,
    typ                    text,
    internalnodeid         integer,
    nodeid                 integer,
    regpdmean              double precision DEFAULT 0,
    diametercondit         double precision DEFAULT 0,
    name                   varchar(50) DEFAULT NULL::character varying,
    regarmtype             varchar(50) DEFAULT NULL::character varying,
    regarmsign             integer,
    damperopendeg          double precision DEFAULT 0,
    rotationcount          integer,
    gatecontrol            integer,
    clue                   integer,
    thrustcollar           integer,
    relleakage             double precision DEFAULT 0,
    opc                    varchar(50) DEFAULT NULL::character varying,
    h                      double precision DEFAULT 0,
    deltah                 double precision DEFAULT 0,
    deltaq                 double precision DEFAULT 0,
    q                      double precision DEFAULT 0,
    damperarmaturestateid  integer DEFAULT 1,
    coords_legacy          text,
    subtype_src_id         integer,
    row_version            bigint DEFAULT 1 NOT NULL,
    updated_at             timestamp with time zone DEFAULT clock_timestamp() NOT NULL,
    updated_by             text DEFAULT CURRENT_USER NOT NULL
);

ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS nodeid                 integer;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS regpdmean              double precision DEFAULT 0;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS diametercondit         double precision DEFAULT 0;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS name                   varchar(50) DEFAULT NULL::character varying;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS regarmtype             varchar(50) DEFAULT NULL::character varying;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS regarmsign             integer;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS damperopendeg          double precision DEFAULT 0;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS rotationcount          integer;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS gatecontrol            integer;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS clue                   integer;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS thrustcollar           integer;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS relleakage             double precision DEFAULT 0;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS opc                    varchar(50) DEFAULT NULL::character varying;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS h                      double precision DEFAULT 0;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS deltah                 double precision DEFAULT 0;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS deltaq                 double precision DEFAULT 0;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS q                      double precision DEFAULT 0;
ALTER TABLE net.armature_control ADD COLUMN IF NOT EXISTS damperarmaturestateid  integer DEFAULT 1;

ALTER TABLE net.armature_control DROP CONSTRAINT IF EXISTS armature_control_pkey;
ALTER TABLE net.armature_control
    ADD CONSTRAINT armature_control_pkey PRIMARY KEY (id);
ALTER TABLE net.armature_control DROP CONSTRAINT IF EXISTS armature_control_fragment_id_fkey;
ALTER TABLE net.armature_control
    ADD CONSTRAINT armature_control_fragment_id_fkey
    FOREIGN KEY (fragment_id) REFERENCES net.fragment(id);
ALTER TABLE net.armature_control DROP CONSTRAINT IF EXISTS armature_control_node_from_fkey;
ALTER TABLE net.armature_control
    ADD CONSTRAINT armature_control_node_from_fkey
    FOREIGN KEY (node_from) REFERENCES net.node_reg(id) ON DELETE RESTRICT;
ALTER TABLE net.armature_control DROP CONSTRAINT IF EXISTS armature_control_node_to_fkey;
ALTER TABLE net.armature_control
    ADD CONSTRAINT armature_control_node_to_fkey
    FOREIGN KEY (node_to) REFERENCES net.node_reg(id) ON DELETE RESTRICT;

CREATE INDEX IF NOT EXISTS armature_control_geom_idx ON net.armature_control USING gist (geom);
CREATE INDEX IF NOT EXISTS armature_control_fragment_id_idx ON net.armature_control (fragment_id);

DROP TRIGGER IF EXISTS armature_control_reg ON net.armature_control;
CREATE TRIGGER armature_control_reg AFTER INSERT OR DELETE ON net.armature_control
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('armature_control');
DROP TRIGGER IF EXISTS object_touch ON net.armature_control;
CREATE TRIGGER object_touch BEFORE UPDATE ON net.armature_control
    FOR EACH ROW EXECUTE FUNCTION net.touch_object_row();
DROP TRIGGER IF EXISTS object_insert_log ON net.armature_control;
CREATE TRIGGER object_insert_log AFTER INSERT ON net.armature_control
    FOR EACH ROW EXECUTE FUNCTION net.log_object_insert();
DROP TRIGGER IF EXISTS line_topology ON net.armature_control;
CREATE TRIGGER line_topology BEFORE INSERT OR UPDATE OF node_from, node_to, fragment_id, geom, removed_at
    ON net.armature_control FOR EACH ROW
    EXECUTE FUNCTION net.validate_line_topology();
DROP TRIGGER IF EXISTS geometry_change_audit ON net.armature_control;
CREATE TRIGGER geometry_change_audit AFTER UPDATE OF geom ON net.armature_control
    FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change();

-- valve_reverse: 13 собственных колонок из public.reversevalves
CREATE TABLE IF NOT EXISTS net.valve_reverse (
    id                     bigint DEFAULT nextval('obj_id_seq'::regclass) NOT NULL,
    fragment_id            integer,
    node_from              bigint NOT NULL,
    node_to                bigint NOT NULL,
    node_from_src          integer,
    node_to_src            integer,
    fileid_src             integer,
    geom                   geometry(LineString, 9998) NOT NULL,
    removed_at             timestamp with time zone,
    src_id                 integer,
    externalsignlineid     integer,
    location               text,
    hydrores               double precision,
    organizationid         integer,
    registnum              text,
    firstpicdate           date,
    lastmaintdate          date,
    displaysign            integer,
    archivechangedate      timestamp without time zone,
    operatorid             integer,
    typ                    text,
    internalnodeid         integer,
    hydroresopen           double precision DEFAULT 0,
    hydroresclose          double precision DEFAULT 0,
    locinstall             varchar(50) DEFAULT NULL::character varying,
    standardrevvalveid     integer,
    standardid             integer DEFAULT 1,
    tubeid                 integer,
    pipelinelength         double precision DEFAULT 0,
    diameterinternal       double precision DEFAULT 0,
    tuberoughness          double precision DEFAULT 0,
    rescoeffssum           double precision DEFAULT 0,
    revvalvecap            integer,
    relatleakage           double precision DEFAULT 0,
    stateid                integer DEFAULT 1,
    coords_legacy          text,
    subtype_src_id         integer,
    row_version            bigint DEFAULT 1 NOT NULL,
    updated_at             timestamp with time zone DEFAULT clock_timestamp() NOT NULL,
    updated_by             text DEFAULT CURRENT_USER NOT NULL
);

ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS hydroresopen           double precision DEFAULT 0;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS hydroresclose          double precision DEFAULT 0;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS locinstall             varchar(50) DEFAULT NULL::character varying;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS standardrevvalveid     integer;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS standardid             integer DEFAULT 1;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS tubeid                 integer;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS pipelinelength         double precision DEFAULT 0;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS diameterinternal       double precision DEFAULT 0;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS tuberoughness          double precision DEFAULT 0;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS rescoeffssum           double precision DEFAULT 0;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS revvalvecap            integer;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS relatleakage           double precision DEFAULT 0;
ALTER TABLE net.valve_reverse ADD COLUMN IF NOT EXISTS stateid                integer DEFAULT 1;

ALTER TABLE net.valve_reverse DROP CONSTRAINT IF EXISTS valve_reverse_pkey;
ALTER TABLE net.valve_reverse
    ADD CONSTRAINT valve_reverse_pkey PRIMARY KEY (id);
ALTER TABLE net.valve_reverse DROP CONSTRAINT IF EXISTS valve_reverse_fragment_id_fkey;
ALTER TABLE net.valve_reverse
    ADD CONSTRAINT valve_reverse_fragment_id_fkey
    FOREIGN KEY (fragment_id) REFERENCES net.fragment(id);
ALTER TABLE net.valve_reverse DROP CONSTRAINT IF EXISTS valve_reverse_node_from_fkey;
ALTER TABLE net.valve_reverse
    ADD CONSTRAINT valve_reverse_node_from_fkey
    FOREIGN KEY (node_from) REFERENCES net.node_reg(id) ON DELETE RESTRICT;
ALTER TABLE net.valve_reverse DROP CONSTRAINT IF EXISTS valve_reverse_node_to_fkey;
ALTER TABLE net.valve_reverse
    ADD CONSTRAINT valve_reverse_node_to_fkey
    FOREIGN KEY (node_to) REFERENCES net.node_reg(id) ON DELETE RESTRICT;

CREATE INDEX IF NOT EXISTS valve_reverse_geom_idx ON net.valve_reverse USING gist (geom);
CREATE INDEX IF NOT EXISTS valve_reverse_fragment_id_idx ON net.valve_reverse (fragment_id);

DROP TRIGGER IF EXISTS valve_reverse_reg ON net.valve_reverse;
CREATE TRIGGER valve_reverse_reg AFTER INSERT OR DELETE ON net.valve_reverse
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('valve_reverse');
DROP TRIGGER IF EXISTS object_touch ON net.valve_reverse;
CREATE TRIGGER object_touch BEFORE UPDATE ON net.valve_reverse
    FOR EACH ROW EXECUTE FUNCTION net.touch_object_row();
DROP TRIGGER IF EXISTS object_insert_log ON net.valve_reverse;
CREATE TRIGGER object_insert_log AFTER INSERT ON net.valve_reverse
    FOR EACH ROW EXECUTE FUNCTION net.log_object_insert();
DROP TRIGGER IF EXISTS line_topology ON net.valve_reverse;
CREATE TRIGGER line_topology BEFORE INSERT OR UPDATE OF node_from, node_to, fragment_id, geom, removed_at
    ON net.valve_reverse FOR EACH ROW
    EXECUTE FUNCTION net.validate_line_topology();
DROP TRIGGER IF EXISTS geometry_change_audit ON net.valve_reverse;
CREATE TRIGGER geometry_change_audit AFTER UPDATE OF geom ON net.valve_reverse
    FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change();

COMMIT;
