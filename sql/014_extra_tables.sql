-- СГЕНЕРИРОВАНО tools/gen_extra_tables.py
--
-- Строки подтипов, не ставшие объектом: дубли и проигравшие
-- чужому классу. Хранятся с настоящими колонками, а не в jsonb —
-- см. заголовок генератора о разнице в 211 секунд.


-- лишние строки connectnodes
CREATE TABLE IF NOT EXISTS net.extra_connectnodes (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
  , connectid                        int
);
CREATE INDEX IF NOT EXISTS extra_connectnodes_obj_idx ON net.extra_connectnodes (obj_id);

-- лишние строки generalizedconsumers
CREATE TABLE IF NOT EXISTS net.extra_generalizedconsumers (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
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
CREATE INDEX IF NOT EXISTS extra_generalizedconsumers_obj_idx ON net.extra_generalizedconsumers (obj_id);

-- лишние строки heatchambers
CREATE TABLE IF NOT EXISTS net.extra_heatchambers (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
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
CREATE INDEX IF NOT EXISTS extra_heatchambers_obj_idx ON net.extra_heatchambers (obj_id);

-- лишние строки heatsources
CREATE TABLE IF NOT EXISTS net.extra_heatsources (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
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
CREATE INDEX IF NOT EXISTS extra_heatsources_obj_idx ON net.extra_heatsources (obj_id);

-- лишние строки pumpstations
CREATE TABLE IF NOT EXISTS net.extra_pumpstations (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
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
CREATE INDEX IF NOT EXISTS extra_pumpstations_obj_idx ON net.extra_pumpstations (obj_id);

-- лишние строки realconsumers
CREATE TABLE IF NOT EXISTS net.extra_realconsumers (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
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
CREATE INDEX IF NOT EXISTS extra_realconsumers_obj_idx ON net.extra_realconsumers (obj_id);

-- лишние строки refillnodes
CREATE TABLE IF NOT EXISTS net.extra_refillnodes (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
  , externalsignid                   int
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
CREATE INDEX IF NOT EXISTS extra_refillnodes_obj_idx ON net.extra_refillnodes (obj_id);

-- лишние строки threewayvalves
CREATE TABLE IF NOT EXISTS net.extra_threewayvalves (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
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
CREATE INDEX IF NOT EXISTS extra_threewayvalves_obj_idx ON net.extra_threewayvalves (obj_id);

-- лишние строки airheaters
CREATE TABLE IF NOT EXISTS net.extra_airheaters (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
  , faninstall                       text
  , scheme                           text
  , airheaterscount                  double precision
  , rowscount                        double precision
  , storescount                      double precision
  , airheatertype                    text
  , contamincoeff                    double precision
  , presscoeff                       double precision
  , location                         text
);
CREATE INDEX IF NOT EXISTS extra_airheaters_obj_idx ON net.extra_airheaters (obj_id);

-- лишние строки dampers
CREATE TABLE IF NOT EXISTS net.extra_dampers (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
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
);
CREATE INDEX IF NOT EXISTS extra_dampers_obj_idx ON net.extra_dampers (obj_id);

-- лишние строки diaphragms
CREATE TABLE IF NOT EXISTS net.extra_diaphragms (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
  , throtdiaphloc                    text
  , diameterinternal                 double precision
  , consinstdiaphcount               int
  , entrymark                        text
  , stateid                          int
);
CREATE INDEX IF NOT EXISTS extra_diaphragms_obj_idx ON net.extra_diaphragms (obj_id);

-- лишние строки elevators
CREATE TABLE IF NOT EXISTS net.extra_elevators (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
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
);
CREATE INDEX IF NOT EXISTS extra_elevators_obj_idx ON net.extra_elevators (obj_id);

-- лишние строки heatexchangers
CREATE TABLE IF NOT EXISTS net.extra_heatexchangers (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
  , heatexchtype                     text
  , heatexchcode                     int
  , location                         int
  , stateid                          int
);
CREATE INDEX IF NOT EXISTS extra_heatexchangers_obj_idx ON net.extra_heatexchangers (obj_id);

-- лишние строки heatpipesections
CREATE TABLE IF NOT EXISTS net.extra_heatpipesections (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
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
  , organizationid                   int
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
);
CREATE INDEX IF NOT EXISTS extra_heatpipesections_obj_idx ON net.extra_heatpipesections (obj_id);

-- лишние строки localhydroresistances2
CREATE TABLE IF NOT EXISTS net.extra_localhydroresistances2 (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
  , name_mest                        text
  , s_mest                           double precision
  , k_mest                           int
  , sum_mest                         double precision
);
CREATE INDEX IF NOT EXISTS extra_localhydroresistances2_obj_idx ON net.extra_localhydroresistances2 (obj_id);

-- лишние строки pressregulators
CREATE TABLE IF NOT EXISTS net.extra_pressregulators (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
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
);
CREATE INDEX IF NOT EXISTS extra_pressregulators_obj_idx ON net.extra_pressregulators (obj_id);

-- лишние строки pumps
CREATE TABLE IF NOT EXISTS net.extra_pumps (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
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
);
CREATE INDEX IF NOT EXISTS extra_pumps_obj_idx ON net.extra_pumps (obj_id);

-- лишние строки systemradiators
CREATE TABLE IF NOT EXISTS net.extra_systemradiators (
    id      int PRIMARY KEY
  , obj_id  bigint NOT NULL
  , "name"                           text
  , type                             text
  , count                            text
  , totalequivsurface                text
  , stateid                          int
);
CREATE INDEX IF NOT EXISTS extra_systemradiators_obj_idx ON net.extra_systemradiators (obj_id);
