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

-- bypass: 13 собственных колонок из public.bypass
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

ALTER TABLE net.bypass
    ADD CONSTRAINT bypass_pkey PRIMARY KEY (id);
ALTER TABLE net.bypass
    ADD CONSTRAINT bypass_fragment_id_fkey
    FOREIGN KEY (fragment_id) REFERENCES net.fragment(id);
ALTER TABLE net.bypass
    ADD CONSTRAINT bypass_node_from_fkey
    FOREIGN KEY (node_from) REFERENCES net.node_reg(id) ON DELETE RESTRICT;
ALTER TABLE net.bypass
    ADD CONSTRAINT bypass_node_to_fkey
    FOREIGN KEY (node_to) REFERENCES net.node_reg(id) ON DELETE RESTRICT;

CREATE INDEX bypass_geom_idx ON net.bypass USING gist (geom);
CREATE INDEX bypass_fragment_id_idx ON net.bypass (fragment_id);

CREATE TRIGGER bypass_reg AFTER INSERT OR DELETE ON net.bypass
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('bypass');
CREATE TRIGGER object_touch BEFORE UPDATE ON net.bypass
    FOR EACH ROW EXECUTE FUNCTION net.touch_object_row();
CREATE TRIGGER object_insert_log AFTER INSERT ON net.bypass
    FOR EACH ROW EXECUTE FUNCTION net.log_object_insert();
CREATE TRIGGER line_topology BEFORE INSERT OR UPDATE OF node_from, node_to, fragment_id, geom, removed_at
    ON net.bypass FOR EACH ROW
    EXECUTE FUNCTION net.validate_line_topology();
CREATE TRIGGER geometry_change_audit AFTER UPDATE OF geom ON net.bypass
    FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change();

-- regulator_consumption: 8 собственных колонок из public.consumptregulators
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
    regconsmean            double precision DEFAULT 0,
    workattrid             integer,
    hydroresopen           double precision DEFAULT 0,
    hydroresclose          double precision DEFAULT 0,
    regvalvecap            integer,
    plumsconsumption       double precision DEFAULT 0,
    deltah                 double precision DEFAULT 0,
    regulatorstateid       integer DEFAULT 1,
    coords_legacy          text,
    subtype_src_id         integer,
    row_version            bigint DEFAULT 1 NOT NULL,
    updated_at             timestamp with time zone DEFAULT clock_timestamp() NOT NULL,
    updated_by             text DEFAULT CURRENT_USER NOT NULL
);

ALTER TABLE net.regulator_consumption
    ADD CONSTRAINT regulator_consumption_pkey PRIMARY KEY (id);
ALTER TABLE net.regulator_consumption
    ADD CONSTRAINT regulator_consumption_fragment_id_fkey
    FOREIGN KEY (fragment_id) REFERENCES net.fragment(id);
ALTER TABLE net.regulator_consumption
    ADD CONSTRAINT regulator_consumption_node_from_fkey
    FOREIGN KEY (node_from) REFERENCES net.node_reg(id) ON DELETE RESTRICT;
ALTER TABLE net.regulator_consumption
    ADD CONSTRAINT regulator_consumption_node_to_fkey
    FOREIGN KEY (node_to) REFERENCES net.node_reg(id) ON DELETE RESTRICT;

CREATE INDEX regulator_consumption_geom_idx ON net.regulator_consumption USING gist (geom);
CREATE INDEX regulator_consumption_fragment_id_idx ON net.regulator_consumption (fragment_id);

CREATE TRIGGER regulator_consumption_reg AFTER INSERT OR DELETE ON net.regulator_consumption
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('regulator_consumption');
CREATE TRIGGER object_touch BEFORE UPDATE ON net.regulator_consumption
    FOR EACH ROW EXECUTE FUNCTION net.touch_object_row();
CREATE TRIGGER object_insert_log AFTER INSERT ON net.regulator_consumption
    FOR EACH ROW EXECUTE FUNCTION net.log_object_insert();
CREATE TRIGGER line_topology BEFORE INSERT OR UPDATE OF node_from, node_to, fragment_id, geom, removed_at
    ON net.regulator_consumption FOR EACH ROW
    EXECUTE FUNCTION net.validate_line_topology();
CREATE TRIGGER geometry_change_audit AFTER UPDATE OF geom ON net.regulator_consumption
    FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change();

-- regulator_pressdrop: 10 собственных колонок из public.pressdropregulators
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

ALTER TABLE net.regulator_pressdrop
    ADD CONSTRAINT regulator_pressdrop_pkey PRIMARY KEY (id);
ALTER TABLE net.regulator_pressdrop
    ADD CONSTRAINT regulator_pressdrop_fragment_id_fkey
    FOREIGN KEY (fragment_id) REFERENCES net.fragment(id);
ALTER TABLE net.regulator_pressdrop
    ADD CONSTRAINT regulator_pressdrop_node_from_fkey
    FOREIGN KEY (node_from) REFERENCES net.node_reg(id) ON DELETE RESTRICT;
ALTER TABLE net.regulator_pressdrop
    ADD CONSTRAINT regulator_pressdrop_node_to_fkey
    FOREIGN KEY (node_to) REFERENCES net.node_reg(id) ON DELETE RESTRICT;

CREATE INDEX regulator_pressdrop_geom_idx ON net.regulator_pressdrop USING gist (geom);
CREATE INDEX regulator_pressdrop_fragment_id_idx ON net.regulator_pressdrop (fragment_id);

CREATE TRIGGER regulator_pressdrop_reg AFTER INSERT OR DELETE ON net.regulator_pressdrop
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('regulator_pressdrop');
CREATE TRIGGER object_touch BEFORE UPDATE ON net.regulator_pressdrop
    FOR EACH ROW EXECUTE FUNCTION net.touch_object_row();
CREATE TRIGGER object_insert_log AFTER INSERT ON net.regulator_pressdrop
    FOR EACH ROW EXECUTE FUNCTION net.log_object_insert();
CREATE TRIGGER line_topology BEFORE INSERT OR UPDATE OF node_from, node_to, fragment_id, geom, removed_at
    ON net.regulator_pressdrop FOR EACH ROW
    EXECUTE FUNCTION net.validate_line_topology();
CREATE TRIGGER geometry_change_audit AFTER UPDATE OF geom ON net.regulator_pressdrop
    FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change();

-- armature_control: 10 собственных колонок из public.regularmatures
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
    regpdmean              double precision DEFAULT 0,
    regarmtype             varchar(50) DEFAULT NULL::character varying,
    regarmsign             integer,
    damperopendeg          double precision DEFAULT 0,
    rotationcount          integer,
    relleakage             double precision DEFAULT 0,
    h                      double precision DEFAULT 0,
    deltah                 double precision DEFAULT 0,
    deltaq                 double precision DEFAULT 0,
    q                      double precision DEFAULT 0,
    coords_legacy          text,
    subtype_src_id         integer,
    row_version            bigint DEFAULT 1 NOT NULL,
    updated_at             timestamp with time zone DEFAULT clock_timestamp() NOT NULL,
    updated_by             text DEFAULT CURRENT_USER NOT NULL
);

ALTER TABLE net.armature_control
    ADD CONSTRAINT armature_control_pkey PRIMARY KEY (id);
ALTER TABLE net.armature_control
    ADD CONSTRAINT armature_control_fragment_id_fkey
    FOREIGN KEY (fragment_id) REFERENCES net.fragment(id);
ALTER TABLE net.armature_control
    ADD CONSTRAINT armature_control_node_from_fkey
    FOREIGN KEY (node_from) REFERENCES net.node_reg(id) ON DELETE RESTRICT;
ALTER TABLE net.armature_control
    ADD CONSTRAINT armature_control_node_to_fkey
    FOREIGN KEY (node_to) REFERENCES net.node_reg(id) ON DELETE RESTRICT;

CREATE INDEX armature_control_geom_idx ON net.armature_control USING gist (geom);
CREATE INDEX armature_control_fragment_id_idx ON net.armature_control (fragment_id);

CREATE TRIGGER armature_control_reg AFTER INSERT OR DELETE ON net.armature_control
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('armature_control');
CREATE TRIGGER object_touch BEFORE UPDATE ON net.armature_control
    FOR EACH ROW EXECUTE FUNCTION net.touch_object_row();
CREATE TRIGGER object_insert_log AFTER INSERT ON net.armature_control
    FOR EACH ROW EXECUTE FUNCTION net.log_object_insert();
CREATE TRIGGER line_topology BEFORE INSERT OR UPDATE OF node_from, node_to, fragment_id, geom, removed_at
    ON net.armature_control FOR EACH ROW
    EXECUTE FUNCTION net.validate_line_topology();
CREATE TRIGGER geometry_change_audit AFTER UPDATE OF geom ON net.armature_control
    FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change();

-- valve_reverse: 12 собственных колонок из public.reversevalves
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
    stateid                integer DEFAULT 1,
    coords_legacy          text,
    subtype_src_id         integer,
    row_version            bigint DEFAULT 1 NOT NULL,
    updated_at             timestamp with time zone DEFAULT clock_timestamp() NOT NULL,
    updated_by             text DEFAULT CURRENT_USER NOT NULL
);

ALTER TABLE net.valve_reverse
    ADD CONSTRAINT valve_reverse_pkey PRIMARY KEY (id);
ALTER TABLE net.valve_reverse
    ADD CONSTRAINT valve_reverse_fragment_id_fkey
    FOREIGN KEY (fragment_id) REFERENCES net.fragment(id);
ALTER TABLE net.valve_reverse
    ADD CONSTRAINT valve_reverse_node_from_fkey
    FOREIGN KEY (node_from) REFERENCES net.node_reg(id) ON DELETE RESTRICT;
ALTER TABLE net.valve_reverse
    ADD CONSTRAINT valve_reverse_node_to_fkey
    FOREIGN KEY (node_to) REFERENCES net.node_reg(id) ON DELETE RESTRICT;

CREATE INDEX valve_reverse_geom_idx ON net.valve_reverse USING gist (geom);
CREATE INDEX valve_reverse_fragment_id_idx ON net.valve_reverse (fragment_id);

CREATE TRIGGER valve_reverse_reg AFTER INSERT OR DELETE ON net.valve_reverse
    FOR EACH ROW EXECUTE FUNCTION net.reg_line_sync('valve_reverse');
CREATE TRIGGER object_touch BEFORE UPDATE ON net.valve_reverse
    FOR EACH ROW EXECUTE FUNCTION net.touch_object_row();
CREATE TRIGGER object_insert_log AFTER INSERT ON net.valve_reverse
    FOR EACH ROW EXECUTE FUNCTION net.log_object_insert();
CREATE TRIGGER line_topology BEFORE INSERT OR UPDATE OF node_from, node_to, fragment_id, geom, removed_at
    ON net.valve_reverse FOR EACH ROW
    EXECUTE FUNCTION net.validate_line_topology();
CREATE TRIGGER geometry_change_audit AFTER UPDATE OF geom ON net.valve_reverse
    FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change();

COMMIT;
