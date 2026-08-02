"""Нативное чтение схемы net — СГЕНЕРИРОВАНО tools/gen_sety_net_mode.py.

Подключается из read_gid.py. Если схемы net нет, is_net() возвращает False
и ядро работает прежними запросами к public.
"""

# Соответствие «таблица-подтип -> объектная таблица net»
NODE_CLASS = {
    'connectnodes': 'connect_node',
    'generalizedconsumers': 'consumer_general',
    'heatchambers': 'heat_chamber',
    'heatsources': 'heat_source',
    'pumpstations': 'pump_station',
    'realconsumers': 'consumer_real',
    'refillnodes': 'refill_node',
    'threewayvalves': 'valve_3way',
}

LINE_CLASS = {
    'airheaters': 'air_heater',
    'dampers': 'damper',
    'diaphragms': 'diaphragm',
    'elevators': 'elevator',
    'heatexchangers': 'heat_exchanger',
    'heatpipesections': 'pipe_section',
    'localhydroresistances2': 'local_resistance',
    'pressregulators': 'regulator_press',
    'pumps': 'pump',
    'systemradiators': 'radiator',
}

# Эти пять классов создаются поздней продуктовой миграцией
# sql/130_itp_classes.sql и поэтому намеренно отсутствуют в карте
# конвертера (исходные 20 строк решено не переносить автоматически).
# Для sety таблицы всё равно являются нативными источниками, в том числе
# когда они пусты: запрос не должен откатываться к удалённым legacy-именам.
LINE_CLASS.update({
    'bypass': 'bypass',
    'consumptregulators': 'regulator_consumption',
    'pressdropregulators': 'regulator_pressdrop',
    'regularmatures': 'armature_control',
    'reversevalves': 'valve_reverse',
})

# Собственные колонки исходных таблиц-подтипов. Они нужны не для
# создания ещё одного слоя совместимости в БД, а для точной подстановки
# SELECT прямо в запрос sety: основная строка читается из объектной
# таблицы, проигравшие варианты/дубли — из net.extra_*.
NODE_COLUMNS = {'connectnodes': ['connectid'], 'generalizedconsumers': ['quarter', 'maxbuildingheight', 'accumcoeff', 'hydromodesignid', 'specexpendid', 'calctemperatureid', 'gvsloadgraphid', 'varcoeffid', 'normhlosflow', 'normhlosret', 'calchldep', 'calcinternhddep', 'adjcalchldep', 'adjcalchddep', 'perspcalchldep', 'perspcalcinternhddep', 'calctempdep', 'connectionschemeid', 'mixfactcoeffdep', 'calchlindep', 'calcinternhdindep', 'adjcalchlindep', 'adjcalcinternhdindep', 'perspcalchlindep', 'perspcalcinternhdindep', 'calctempindep', 'mixfactcoeffindep', 'calchlventil', 'adjcalchlventil', 'perspcalchlventil', 'calchlcond', 'adjcalchlcond', 'perspcalchlcond', 'calchlclosesys', 'calchlopensysflow', 'calchlopensysret', 'adjcalchlclosesys', 'adjcalchlopensysflow', 'adjcalchlopensysret', 'perspcalchlclosesys', 'perspcalchlopensysflow', 'perspcalchlopensysret', 'expendhwpart', 'calctempha', 'calchlparall', 'internhdparall', 'adjcalchlparall', 'adjinternhdparall', 'perspcalchlparall', 'perspinternhdparall', 'schemeparallid', 'calchlgvsparall', 'adjcalchlgvsparall', 'perspcalchlgvsparall', 'hourirregcoeffparall', 'avghlcompparall', 'temprecircpipeparall', 'calctemphrparall', 'calctemphwdoparall', 'amrdepparall', 'calchlmix', 'internhdmix', 'adjcalchlmix', 'adjinternhdmix', 'perspcalchlmix', 'perspinternhdmix', 'schememixid', 'calchlgvsmix', 'adjcalchlgvsmix', 'perspcalchlgvsmix', 'hourirregcoeffmix', 'avghlcompmix', 'temprecircpipemix', 'calctemphrmix', 'calctemphwdomix', 'amrdepmix', 'calchlconseq', 'internhdconseq', 'adjcalchlconseq', 'adjinternhdconseq', 'perspcalchlconseq', 'perspinternhdconseq', 'schemeconseqid', 'calchlgvsconseq', 'adjcalchlgvsconseq', 'perspcalchlgvsconseq', 'hourirregcoeffconseq', 'avghlcompconseq', 'temprecircpipeconseq', 'calctemphrconseq', 'calctemphwdoconseq', 'amrdepconseq', 'calchlpreon', 'internhdpreon', 'adjcalchlpreon', 'adjinternhdpreon', 'perspcalchlpreon', 'perspinternhdpreon', 'schemepreonid', 'calchlgvspreon', 'adjcalchlgvspreon', 'perspcalchlgvspreon', 'hourirregcoeffpreon', 'avghlcomppreon', 'temprecircpipepreon', 'calctemphrpreon', 'calctemphwdopreon', 'amrdeppreon', 'avghlgvsopensysflow', 'avghlgvsopensysret', 'adjavghlgvsopensysflow', 'adjavghlgvsopensysret', 'perspavghlgvsopensysflow', 'perspavghlgvsopensysret', 'hourirregcoeffopen', 'avghlcompopen', 'temprecircpipeopen', 'calctemphwdoopen', 'pdvalveinstalllocid', 'setpdonregulator', 'setleakageflow', 'setleakageret', 'wemeteringdeviceid', 'volwaterhs', 'volwatervs', 'minthrustds', 'hydroresclosesys', 'hydroresclosesyssummer', 'closesyscalcsignid', 'hydroreswdoflow', 'calcsignopensysflowid', 'hydroreswdoret', 'calcsignreswdoretid', 'calcsignsetloadclosesys', 'calcsignsetloadopensysflow', 'calcsignsetloadopensysret', 'hydrothrustin', 'hydrothrustout', 'calcexpenddep', 'calcexpendindep', 'calcexpendventil', 'calcexpendcond', 'calcexpendhwflow', 'calcexpendhwret', 'calcexpendrecircopen', 'calcexpendhwparall', 'calcexpendhwmix', 'calcexpendhwconseq', 'calcexpendhwpreon', 'calcthrustinwsd', 'consumerstateid', 'name'], 'heatchambers': ['name', 'slotscount', 'slotsweight', 'internalheight', 'internalwidth', 'internallength', 'wallmaterial', 'stairscount', 'stoparmaturecount', 'ballvalvescount', 'airventscount'], 'heatsources': ['sourcename', 'name', 'stateid', 'hsourcetypeid', 'hsourceid', 'hsourcepower', 'hsourcepowerinst', 'hseasonbegindate', 'hseasonenddate', 'hsourcecode', 'temperdwflowsummer', 'temperdwretsummer', 'specvolhs', 'specvolvent', 'expenddwnorm1', 'expenddwnorm2', 'expenddwnorm3', 'expenddwnorm4', 'expenddwnorm5', 'managerphone', 'controllerphone', 'powerset', 'poweravailable', 'heighttubemark', 'heightareamark', 'repairworks', 't1_summer', 't2_summer', 'name_tg', 'heatloscalcyear', 'graphtypeid', 'tn_1', 'tn_5', 'tvn_r', 't1_r', 't2_r', 't3_r', 'q_r', 't1_2r', 't1_4r', 'tvb_tr', 'uf', 't2_2r', 'q_gv', 'tg_r', 'tx_r', 't2_gv', 'pr', 'g1', 'g2', 't_gv1', 'deployedtempgraphid', 'v', 'date_on', 'name_exe', 'name_manager', 'dt2_co', 't2pod_parl', 'dt2v_sm_noavm', 'dt2v_sm_avm', 't1pod_sm', 't1pod_posll', 'dt2v_posl', 'length_1', 'length_2', 'length_3', 'length_4', 'length_5', 'length_6', 'length_7', 'length_8', 'length_9', 'length_10', 'length_11', 'length_12', 't_1', 't_2', 't_3', 't_4', 't_5', 't_6', 't_7', 't_8', 't_9', 't_10', 't_11', 't_12', 't1_1', 't1_2', 't1_3', 't1_4', 't1_5', 't1_6', 't1_7', 't1_8', 't1_9', 't1_10', 't1_11', 't1_12', 't2_1', 't2_2', 't2_3', 't2_4', 't2_5', 't2_6', 't2_7', 't2_8', 't2_9', 't2_10', 't2_11', 't2_12', 'length_1_fakt', 'length_2_fakt', 'length_3_fakt', 'length_4_fakt', 'length_5_fakt', 'length_6_fakt', 'length_7_fakt', 'length_8_fakt', 'length_9_fakt', 'length_10_fakt', 'length_11_fakt', 'length_12_fakt', 't_1_fakt', 't_2_fakt', 't_3_fakt', 't_4_fakt', 't_5_fakt', 't_6_fakt', 't_7_fakt', 't_8_fakt', 't_9_fakt', 't_10_fakt', 't_11_fakt', 't_12_fakt', 't1_1_fakt', 't1_2_fakt', 't1_3_fakt', 't1_4_fakt', 't1_5_fakt', 't1_6_fakt', 't1_7_fakt', 't1_8_fakt', 't1_9_fakt', 't1_10_fakt', 't1_11_fakt', 't1_12_fakt', 't2_1_fakt', 't2_2_fakt', 't2_3_fakt', 't2_4_fakt', 't2_5_fakt', 't2_6_fakt', 't2_7_fakt', 't2_8_fakt', 't2_9_fakt', 't2_10_fakt', 't2_11_fakt', 't2_12_fakt', 'gpod_1_fakt', 'gpod_2_fakt', 'gpod_3_fakt', 'gpod_4_fakt', 'gpod_5_fakt', 'gpod_6_fakt', 'gpod_7_fakt', 'gpod_8_fakt', 'gpod_9_fakt', 'gpod_10_fakt', 'gpod_11_fakt', 'gpod_12_fakt', 'ggvs_pribor_1_fakt', 'ggvs_pribor_2_fakt', 'ggvs_pribor_3_fakt', 'ggvs_pribor_4_fakt', 'ggvs_pribor_5_fakt', 'ggvs_pribor_6_fakt', 'ggvs_pribor_7_fakt', 'ggvs_pribor_8_fakt', 'ggvs_pribor_9_fakt', 'ggvs_pribor_10_fakt', 'ggvs_pribor_11_fakt', 'ggvs_pribor_12_fakt', 'gakt_tex_1_fakt', 'gakt_tex_2_fakt', 'gakt_tex_3_fakt', 'gakt_tex_4_fakt', 'gakt_tex_5_fakt', 'gakt_tex_6_fakt', 'gakt_tex_7_fakt', 'gakt_tex_8_fakt', 'gakt_tex_9_fakt', 'gakt_tex_10_fakt', 'gakt_tex_11_fakt', 'gakt_tex_12_fakt', 'gakt_avarija_1_fakt', 'gakt_avarija_2_fakt', 'gakt_avarija_3_fakt', 'gakt_avarija_4_fakt', 'gakt_avarija_5_fakt', 'gakt_avarija_6_fakt', 'gakt_avarija_7_fakt', 'gakt_avarija_8_fakt', 'gakt_avarija_9_fakt', 'gakt_avarija_10_fakt', 'gakt_avarija_11_fakt', 'gakt_avarija_12_fakt', 'id_old'], 'pumpstations': ['name', 'purpose', 'capacity', 'pumpcountflow', 'pumpcountret', 'state', 'stateid', 'heighttubemark', 'heightareamark'], 'realconsumers': ['name', 'consumerstateid', 'automdegid', 'sectconsumercode', 'schemenum', 'buildheight', 'accumcoeff', 'specexpendid', 'calctemperatureid', 'gvsloadgraphid', 'varcoeffid', 'calchldep', 'calchlindep', 'relloadfacade', 'calcinternhd', 'contcalchldep', 'contcalchlindep', 'contrelloadfacade', 'continternhd', 'perspcalchldep', 'perspcalchlindep', 'persprelloadfacade', 'perspinternhd', 'calchlventil', 'expendhwpart', 'contcalchlventil', 'perspcalchlventil', 'avghlcond', 'contavghlcond', 'perspavghlcond', 'avghlclosesys', 'avghlopensysflow', 'avghlopensysret', 'contavghlclosesys', 'contavghlopensysflow', 'contavghlopensysret', 'perspavghlclose', 'perspavghlopenflow', 'contavghlopenret', 'avghlgvsopenflow', 'avghlgvsopenret', 'avghlgvscloseparall', 'avghlgvsclosemix', 'avghlgvscloseconseq', 'avghlgvsclosepreon', 'contavghlgvsopenflow', 'contavghlgvsopenret', 'contavghlgvscloseparall', 'contavghlgvsclosemix', 'contavghlgvscloseconseq', 'contavghlgvsclosepreon', 'perspavghlgvsopenflow', 'perspavghlgvsopenret', 'perspavghlgvscloseparall', 'perspavghlgvsclosemix', 'perspavghlgvscloseconseq', 'perspavghlgvsclosepreon', 'hydromodesignid', 'mixfactcoeff', 'hourirregcoeff', 'circhlosopen', 'temprecircpipe', 'setleakageflow', 'setleakageret', 'wemeteringdeviceid', 'volwaterhs', 'volwatervs', 'hydroresclosesys', 'hydroresclosesyssummer', 'closesyscalcsignid', 'hydroreswdoflow', 'calcsignopensysflowid', 'hydroreswdoret', 'calcsignreswdoretid', 'calcsignsetloadclosesys', 'calcsignsetloadopensysflow', 'calcsignsetloadopensysret', 'hydrothrustin', 'hydrothrustout', 'calcexpenddep', 'calcexpendindep', 'calcexpendventil', 'calcexpendcond', 'calcexpendhwflow', 'calcexpendhwret', 'calcexpendrecircopen', 'calcexpendhwparall', 'calcexpendhwmix', 'calcexpendhwconseq', 'calcexpendhwpreon', 'throtstagesignid', 'diameterthrotdiaph', 'diameterelevnozzle', 'temperchartsignid', 'calcsignres', 'calcsignhl', 'parallheaterscount1', 'parallheaterscount2', 'parallheaterscountindep', 'calcthrustloshs', 'calcthrustlosah', 'calcthrustlosac', 'calcthrustlosflow', 'calcthrustlosflowcirc', 'calcthrustinwdo', 'calcthrustlosheaters1', 'calcthrustlosheaters2', 'pdvalveinstalllocid', 'setpdonregulator', 'calcferdiametersignid', 'calctemphr', 'calctempvs', 'calctemphwdo', 'responsibleid', 'contractnumber', 'stopvalvetypeid', 'meterdevworksign', 'meterdevsafety', 'meterdevstampnum', 'isolationtype', 'buildingtypeid', 'heatsourceptsid', 'heatpointid', 'countusergv', 'countbusinessconsumers', 'area', 'buildingvolume', 'basementvolume', 'builtinvolume', 'reducebuildingvolume', 'countfloors', 'builtyear', 'streetid', 'housenumber', 'note_1', 'note_2', 'note_pts', 'note_regime', 'b23', 'b28', 'b32', 'b20', 'b14', 'b38', 'b35', 'b36', 'zd7', 'zd8', 'zd27', 'zd28', 'zd36', 'zd38', 'zd39', 'zd48', 'zd49', 'zd50', 'elevatornuminst', 'diameternozzle'], 'refillnodes': ['externalsignid', 'refillexpend', 'wdo', 'refillloss', 'diameterinternal', 'watervolup', 'watervoldown', 'watervolupset', 'potscount', 'potssumvol', 'potworkingsign', 'chargeexpend', 'dischargeexpend', 'setpressret'], 'threewayvalves': ['structure', 'state', 'purpose', 'calcmixfactcoeff', 'calcthrustlos', 'expendcharstraight', 'expendcharvert', 'authority', 'calccapdeviation', 'type', 'capacity', 'regulator', 'maxpd', 'diameternomin', 'calcexpendnodemix', 'pumpstationname']}

LINE_COLUMNS = {'airheaters': ['faninstall', 'scheme', 'airheaterscount', 'rowscount', 'storescount', 'airheatertype', 'contamincoeff', 'presscoeff', 'location'], 'dampers': ['dispatcherswitch', 'diametercondit', 'partdempopen', 'name', 'standarddamplink', 'turncount', 'gatecontrol', 'clue', 'thrustcollar', 'relatleakage', 'opc', 'damperarmaturestateid'], 'diaphragms': ['throtdiaphloc', 'diameterinternal', 'consinstdiaphcount', 'entrymark', 'stateid'], 'elevators': ['elevatortype', 'elevatornuminst', 'diameternozzle', 'entrymark', 'diameterchamber', 'length', 'diameterinletflange', 'diameteroutletflange', 'diametersuctionpipe', 'material', 'stateid'], 'heatexchangers': ['heatexchtype', 'heatexchcode', 'location', 'stateid'], 'heatpipesections': ['pipesectionid', 'pipesectstateidflow', 'pipesectstateidret', 'standardid', 'standardtubelink', 'tubescount', 'diameterinternal', 'diametercondit', 'diameterexternal', 'wallthickness', 'pipesectlength', 'tuberoughness', 'locallosesshare', 'localressum', 'varcoeffidflow', 'varcoeffidret', 'calcheatlossignid', 'tubingtypeid', 'piperemonttypeid', 'channelid', 'constrchanwidth', 'constrchanheight', 'heattestscoeff', 'signnumwork', 'isolmaterialid', 'isolthickness', 'isolmaterialhccoeff', 'pipelinelayingdepth', 'isolhtcoeffabove', 'isolhtcoeffunder', 'airgroundhtcoeffunder', 'groundhccoeff', 'pipelineaxesdist', 'damagenum', 'lasttransdate', 'lastisoldate', 'repairdatecapital', 'picdatecapital', 'repairdatemaint', 'picdatemaint', 'repairdateplantp', 'firstpicdatehp', 'lastmaintdatehp', 'sectexploitperiod', 'buildingconstrstateid', 'specdamagecoeff', 'specdamagecoeff2', 'powcabinstcount10', 'powcabinstcount5', 'powcabinstcount3', 'powcabinstcount1', 'powcabinstcount0', 'powcabinterscount', 'gasecpcount10', 'gasecpcount5', 'gasecpcount3', 'gasecpcount1', 'gasecpcount0', 'gasecpinterscount', 'eltranspcount10', 'eltranspcount5', 'eltranspcount3', 'eltranspcount1', 'eltranspcount0', 'eltranspinterscount', 'potentialdifflevelid', 'corrosiondegoutid', 'corrosiondeginid', 'floodintensitygwid', 'floodintensityfwid', 'floodintensitytwid', 'floodintensitybwid', 'jointstightnessinfr', 'chanconstrfract', 'projsoldeviation', 'constrbearreduce', 'fixedsuppdestr', 'sectwaterdumpid', 'breakcomplexityid', 'breakconsumvolid', 'erwdifficultyid', 'tubingpedestrianid', 'tubingwayid', 'populdamageid', 'infrastrdamageid', 'hydratestsdate', 'sectexpend', 'sectthrustloses', 'heattestsdate', 'temperoutair', 'temperground', 'expenddwflow', 'expenddwret', 'tempercoolflow', 'tempercoolret', 'temperdwflow', 'temperdwret', 'opc', 'tubecharactid', 'tubetypeid', 'tubematerial', 'tempermax', 'factorymanufid', 'externmaterialid', 'isolationtypeid', 'externcoverthick', 'anticorrmaterialid', 'magistralsite', 'distsite', 'exploitreg', 'net', 'magistral', 'exploitsite', 'nettype', 'crimpingquesite', 'h', 'deltah', 'deltaq', 'q', 'primechanie', 'name', 'inventnumber', 'objecttypeid', 'sreda', 'press', 'temperature', 'spoksluzhbirash', 'ressurs', 'pusk', 'organizationid', 'elektrich', 'transportelekricht', 'ponezial', 'vodootved', 'slozhokluzh', 'otkluzhgkal', 'avariivipoln', 'pesehod', 'zhddorogi', 'ludiproklad', 'uzherbludi', 'uzherbsity', 'vnesniivid', 'sostoborudovania', 'vnesnkorrozia', 'vnunrenkorrozia', 'sostkonstrukz', 'kategorii', 'mestn'], 'localhydroresistances2': ['name_mest', 's_mest', 'k_mest', 'sum_mest'], 'pressregulators': ['nodeid', 'valvehydroresopen', 'valvehydroresclose', 'regvalverelcap', 'relleakage', 'consdrip', 'workattrid', 'deltah', 'regulatorstateid', 'h', 'pipelinesignid'], 'pumps': ['offreason', 'pumpstationid', 'number', 'thrust', 'standardpumpid', 'parallagregcount', 'drivetypeid', 'rotordiametertypeid', 'standardemid', 'r0', 'r1', 'r2', 'e0', 'e1', 'e2', 'k0', 'k1', 'k2', 'r0_z', 'r1_z', 'r2_z', 'e0_z', 'e1_z', 'e2_z', 'k0_z', 'k1_z', 'k2_z', 'rotorrotspeedset', 'rotordiameterset', 'lastpumpreplacedate', 'lastemreplacedate', 'lastagregreplacedate', 'repaircountpump', 'replacecountagreg', 'repaircountem', 'opc', 'stateid'], 'systemradiators': ['name', 'type', 'count', 'totalequivsurface', 'stateid']}

_is_net = None


def is_net(conn):
    """Обслуживается ли БД схемой net. Результат кэшируется на процесс.

    Проверять существование схемы net недостаточно. В исходной БД после
    пробной конвертации схема net остаётся рядом с настоящими таблицами
    public, и данные в ней устаревают. Чтение из неё в таком состоянии
    даёт неполный результат БЕЗ ошибки: расчёт отрабатывает, пишет
    меньше строк и сообщает «во фрагменте нет источников тепла».

    Признак настоящего перевода — public.nodes НЕ является базовой
    таблицей. Три состояния:

      * базовая таблица  — данные живут в public, БД не переведена;
      * представление    — переходная БД со слоем совместимости;
      * нет вовсе        — продуктовая БД, слой совместимости снят.

    Проверять «стало представлением» нельзя: как только слой
    совместимости удаляют, признак исчезает, и движок откатывается на
    старые запросы к таблицам, которых уже нет. Ровно это и случилось
    при снятии compat.
    """
    global _is_net
    if _is_net is None:
        cur = conn.cursor()
        cur.execute("SELECT count(*) FROM information_schema.tables "
                    "WHERE table_schema = 'net' AND table_name = 'node_reg'")
        has_net = cur.fetchone()[0] > 0
        cur.execute("SELECT count(*) FROM information_schema.tables "
                    "WHERE table_schema = 'public' AND table_name = 'nodes' "
                    "AND table_type = 'BASE TABLE'")
        legacy_table = cur.fetchone()[0] > 0
        cur.close()
        _is_net = has_net and not legacy_table
    return _is_net


def fragment_table(conn):
    """Имя таблицы фрагментов для подстановки в запрос.

    В net объект называется fragment (единственное число, как остальные
    классы), в старой схеме — fragments. Имена не совпадают, поэтому
    search_path тут не помогает и подстановка нужна явная.
    """
    return 'net.fragment' if is_net(conn) else 'fragments'


def _subtype_relation(name):
    """SELECT-источник подтипа без net.v_*.

    Старая модель допускала несколько строк подтипов на один объект.
    Конвертер хранит выбранный вариант в объектной таблице, остальные —
    в extra_<source>. Оба источника обязательны для совпадения расчёта.
    """
    key = name.lower()
    if key in NODE_CLASS:
        target = NODE_CLASS[key]
        cols = NODE_COLUMNS[key]
        link = 'nodeid'
    elif key in LINE_CLASS and key in LINE_COLUMNS:
        target = LINE_CLASS[key]
        cols = LINE_COLUMNS[key]
        link = 'lineid'
    else:
        return None

    own = ''.join(', o.%s' % c for c in cols)
    extra = ''.join(', x.%s' % c for c in cols)
    return ('(SELECT o.subtype_src_id AS id, o.id AS %s%s '
            'FROM net.%s o UNION ALL '
            'SELECT x.id, x.obj_id AS %s%s FROM net.extra_%s x)'
            % (link, own, target, link, extra, key))


def tbl(conn, name):
    """Источник данных для подстановки в запрос чтения.

    Общие поля сетевых объектов уже есть в продуктовых тонких слоях
    карты. Для подтипов строится SELECT напрямую из физической таблицы
    и extra_*; переходные net.v_nodes/net.v_<subtype> не требуются.
    """
    if not is_net(conn):
        return name
    key = name.lower()
    if key == 'nodes':
        return 'net.v_map_nodes'
    if key == 'linesobj':
        return 'net.v_map_lines'
    return _subtype_relation(key) or name


def tbl_sql(conn, name):
    """Имя таблицы для подстановки туда, где раньше стояло br_text(tn).

    Для перенесённых типов возвращает объект net, для остальных —
    исходное имя в кавычках и нижнем регистре, как делал br_text.
    Нужна в read_db, где имя таблицы приходит параметром: замена по
    образцу FROM/JOIN такие места не видит.
    """
    t = tbl(conn, name)
    if t != name:
        return t
    return '"%s"' % name.lower()


def tbl_cached(name):
    """Как tbl, но без соединения — по уже определённому режиму.

    Нужна там, где запрос собирается функцией, у которой соединения нет:
    g2.write_po пишет текст UPDATE в файл, а выполняется он позже.

    Режим к этому моменту уже определён: is_net вызывается при чтении
    схемы, задолго до записи. Если всё же не определён, возвращается
    старое имя — оно разрешится в представление совместимости, пока то
    существует, а когда его удалят, запрос упадёт заметно. Это лучше
    молчаливой записи не туда.
    """
    if _is_net is None:
        return name
    if not _is_net:
        return name
    key = name.lower()
    if key == 'nodes':
        return 'net.v_map_nodes'
    if key == 'linesobj':
        return 'net.v_map_lines'
    return _subtype_relation(key) or name


def consumer_update_targets(name):
    """Физические цели UPDATE для расчётных полей потребителя.

    Выбранный вариант лежит в объектной таблице, а проигравшие дубли —
    в extra_*. Старый INSTEAD OF-триггер представления обновлял обе
    ветви. Движок теперь повторяет это явно и не пишет через compat.
    Второй элемент пары — колонка связи с каноническим узлом.
    """
    if not _is_net:
        return [(name, 'nodeid')]
    key = name.lower()
    targets = {
        'generalizedconsumers': [
            ('net.consumer_general', 'id'),
            ('net.extra_generalizedconsumers', 'obj_id'),
        ],
        'realconsumers': [
            ('net.consumer_real', 'id'),
            ('net.extra_realconsumers', 'obj_id'),
        ],
    }
    return targets.get(key, [(name, 'nodeid')])


def node_query(tn, cols, s_fileID):
    """Замена запроса read_node2.

    Объектная таблица net уже содержит колонки узла, поэтому соединение
    с nodes не нужно. Строки подтипов, не ставшие объектом (дубли и
    отнесённые к другому классу), лежат в extra_* и добавляются UNION —
    без них ядро увидит меньше объектов, чем на исходной БД.
    """
    # Ключи отображения в нижнем регистре, а ядро передаёт имя таблицы
    # как в исходнике — 'realConsumers', 'heatChambers'. Без приведения
    # регистра .get не находил класс, возвращал None, и чтение молча
    # уходило на старый запрос к представлениям совместимости.
    cls = NODE_CLASS.get(tn.lower())
    if not cls:
        return None
    own = cols.replace('o.', 'o.')
    return f"""
        SELECT o.subtype_src_id AS id, o.id AS nodeID,
               o.externalSignID, o.externalCodeID, o.externalNodeName,
               o.internalnodeid, {own}
        FROM net.{cls} o
        WHERE o.removed_at IS NULL
          AND o.fragment_id IN ({s_fileID})
          AND o.internalNodeID IS NULL
        UNION ALL
        SELECT x.id, x.obj_id,
               r.externalsignid, r.externalcodeid, r.externalnodename,
               r.internalnodeid, {cols.replace('o.', 'x.')}
        FROM net.extra_{tn} x
        JOIN net.node_reg r ON r.id = x.obj_id
        WHERE NOT r.removed
          AND r.fragment_id IN ({s_fileID})
          AND r.internalnodeid IS NULL
    """


def line_query(tn, cols, s_fileID):
    """Замена запроса read_line2.

    Соединение идёт с реестром net.node_reg — обычной таблицей с индексом,
    а не с представлением nodes из десяти ветвей.
    """
    cls = LINE_CLASS.get(tn.lower())  # см. примечание в node_query
    if not cls:
        return None
    # В пяти поздних классах отдельный legacy-nodeid не хранится:
    # его роль выполняет исходный начальный узел линии. w_data всё ещё
    # просит o.nodeid у bypass, поэтому даём совместимый столбец прямо
    # в нативной выборке, не возвращая физическую колонку в модель.
    cols = cols.replace('o.nodeid', 'o.node_from_src AS nodeid')
    return f"""
        SELECT o.subtype_src_id AS id, o.id AS lineID,
               o.node_from_src AS nodeID1, o.node_to_src AS nodeID2,
               o.externalSignLineID, r.internalNodeID, ec.heatSourceID,
               o.hydroRes, {cols}
        FROM net.{cls} o
        JOIN net.node_reg r ON r.id = o.node_from AND NOT r.removed
        JOIN externalCodes ec ON r.externalCodeID = ec.id
        LEFT JOIN net.node_reg r0 ON r0.id = r.internalNodeID AND NOT r0.removed
        LEFT JOIN net.consumer_real rc ON rc.id = r.internalNodeID
        WHERE o.removed_at IS NULL
          AND r.fragment_id IN ({s_fileID})
          AND (r0.id IS NULL OR NOT r0.removed)
          AND rc.id IS NULL
        ORDER BY o.subtype_src_id
    """


def pt_node_query(tn, cols, s_fileID):
    """Замена запросов read_pt_line и read_pt_vnutr.

    Тот же шаблон, что у node_query, но без условия
    internalNodeID IS NULL: пьезометрия читает и внутренние узлы.
    Полей пять, без internalnodeid — порядок обязан совпадать
    с распаковкой строки в вызывающем коде.
    """
    cls = NODE_CLASS.get(tn.lower())
    if not cls:
        return None
    return f"""
        SELECT o.subtype_src_id AS id, o.id AS nodeID,
               o.externalSignID, o.externalCodeID, o.externalNodeName,
               {cols}
        FROM net.{cls} o
        WHERE o.removed_at IS NULL
          AND o.fragment_id IN ({s_fileID})
        UNION ALL
        SELECT x.id, x.obj_id,
               r.externalsignid, r.externalcodeid, r.externalnodename,
               {cols.replace('o.', 'x.')}
        FROM net.extra_{tn} x
        JOIN net.node_reg r ON r.id = x.obj_id
        WHERE NOT r.removed
          AND r.fragment_id IN ({s_fileID})
    """


def zn0_query(fileID):
    """Замена запроса read_zn0 — источник для узлов с заданным напором.

    Второй по стоимости оператор чтения: два соединения с представлением
    nodes, одно из них внутри подзапроса. Оба переведены на реестр
    net.node_reg.

    Фильтр по removed НЕ добавлен намеренно: в исходном запросе его нет,
    и добавить его — значит изменить результат, а не ускорить запрос.

    LEFT JOIN fragments в подзапросе выбора последнего расчёта убран:
    колонки fr в его выборке не участвуют.
    """
    return f"""
select distinct
ec.id, max(usP.ist)

FROM net.node_press_setting zn

JOIN net.node_reg n ON n.id = zn.node_id
JOIN externalCodes ec ON ec.id = n.externalcodeid

JOIN (
    SELECT n0.id, ec0.name AS kod, n0.externalnodename AS name,
           n0.fragment_id AS fileID
    FROM net.node_reg n0
    JOIN externalCodes ec0 ON ec0.id = n0.externalcodeid
) n0 ON n0.kod = ec.name AND n0.name = n.externalnodename

LEFT JOIN (
    SELECT c.fileID, max(c.id) AS cid
    FROM CALCULATION c
    GROUP BY c.fileID
) calc ON calc.fileID = n0.fileid
JOIN US_OUT usP ON usP.nodeID = n0.id AND usP.externalSign = 1
                AND usP.calculationid = cid
WHERE n.fragment_id = {fileID}
GROUP BY ec.id
"""


def zn_query(s_fileID):
    """Замена запроса read_zn — узлы с заданным напором.

    Исходный запрос дважды соединяется с представлением nodes: один раз
    для самого узла, второй — внутри подзапроса результатов предыдущего
    расчёта. На переведённой БД nodes — представление над net из десяти
    ветвей, и оба соединения стоят дорого: 857 мс из 3.8 с фазы чтения.

    Реестр net.node_reg содержит те же поля (fragment_id вместо fileID)
    обычной таблицей с индексами.
    """
    return f"""
SELECT
n.id,
sp.pressflow, sp.pressret,
us2.pihP, us2.pihO,
us2.t1P, us2.t1O,
us2.t2P, us2.t2O,
ec.name AS kod, n.externalnodename AS name,
us2.id AS id_m,
sp.kod_m, sp.uzel_m,
fr.name AS fr_name,
us2.ist,
sp.fragment_resultid

FROM net.node_press_setting sp
JOIN net.node_reg n ON n.id = sp.node_id AND NOT n.removed
JOIN externalCodes ec ON ec.id = n.externalcodeid

LEFT JOIN
(
    SELECT
    n2.id,
    n2.fragment_id AS fileID,
    ec2.name AS kod, n2.externalnodename AS name,
    usP.pih AS pihP, usP.t AS t1P, usP.t2 AS t2P,
    usO.pih AS pihO, usO.t AS t1O, usO.t2 AS t2O,
    usP.ist

    FROM net.node_reg n2
    JOIN externalCodes ec2 ON ec2.id = n2.externalcodeid
    JOIN US_OUT usP ON usP.nodeID = n2.id AND usP.externalSign = 1
    JOIN US_OUT usO ON usO.nodeID = n2.id AND usO.externalSign = 2

    JOIN
    (
        -- LEFT JOIN fragments здесь был и ничего не давал: колонки
        -- fr в выборке подзапроса не участвуют. Убран заодно с
        -- переводом на net — иначе тянул бы public.fragments.
        SELECT c.fileID, max(c.id) AS cid
        FROM CALCULATION c
        GROUP BY c.fileID
    ) calc ON n2.fragment_id = calc.fileID
          AND usP.calculationID = calc.cid AND usO.calculationID = calc.cid

) us2 ON us2.kod = sp.kod_m AND us2.name = sp.uzel_m
     AND us2.fileID = sp.fragment_resultid
LEFT JOIN net.fragment fr ON fr.id = sp.fragment_resultid

WHERE n.fragment_id IN ({s_fileID})
"""
