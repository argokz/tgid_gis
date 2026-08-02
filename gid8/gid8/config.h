#pragma once

#include <QString>

struct ConnectStr
{
    int rdbms;
    QString host;
    unsigned short port;
    QString user;
    QString password;
    QString baza;
    QString geo;
    QString gid9;
};

struct InitConfig
{
    bool ok = false;
    ConnectStr cs;

    bool enable_rdbms = false;
    bool enable_host = false;
    bool enable_port = false;
    bool enable_user = false;
    bool enable_password = false;
    bool enable_baza = false;
    bool enable_geo = false;
    bool enable_newbaza = false;

    bool enable_gid9 = false;
    bool enable_alseco = false;
    bool enable_tu = false;

    int PROT_LEFT = 5;
    bool protection = true;
    QString protection_url = "";
    QString elevation_url = "";
};

bool init_config();
InitConfig * get_ini();
