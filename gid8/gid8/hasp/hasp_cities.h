#pragma once

//#define UPDATE_HASP ID_KHARKIV
//#define UPDATE_HASP ID_KHARKIV_LOCAL
//#define UPDATE_HASP ID_SEMEY
//#define UPDATE_HASP ID_SEMEY_NOTEBOOK
//#define UPDATE_HASP ID_ODESSA
#define UPDATE_HASP ID_ODESSA_NET
//#define UPDATE_HASP ID_LUTSK
//#define UPDATE_HASP ID_SHYMKENT
//#define UPDATE_HASP ID_TARAZ
//#define UPDATE_HASP ID_EKIBASTUZ
//#define UPDATE_HASP ID_KAZNIPIENERGOPROM

#if UPDATE_HASP == ID_SHYMKENT
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "ShymkentGID"

#elif UPDATE_HASP == ID_KHARKIV
    #define NET      1
    #define COUNTER  1
    #define OST 150
    #define ASTANAGID "KharkivGID"

#elif UPDATE_HASP == ID_KHARKIV_LOCAL
//    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "KharkivGID_local"

#elif UPDATE_HASP == ID_SEMEY
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "SemeyGID"

#elif UPDATE_HASP == ID_SEMEY_NOTEBOOK
//    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "SemeyGID_Notebook"

#elif UPDATE_HASP == ID_ODESSA  
//    #define NET      0
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "OdessaGID"

#elif UPDATE_HASP == ID_ODESSA_NET
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "OdessaGID_net"

#elif UPDATE_HASP == ID_LUTSK
//    #define NET      0
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "LutskGID"

#elif UPDATE_HASP == ID_TARAZ
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "TarazGID"

#elif UPDATE_HASP == ID_KAZNIPIENERGOPROM
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "KazNIIGID"

#elif UPDATE_HASP == ID_EKIBASTUZ
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "Ekibastuz"

#else  
    #define COUNTER  1
    #define OST      150
    #define ASTANAGID ""
#endif
