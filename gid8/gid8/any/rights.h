#pragma once

enum RIGHT {
  R_admin = 0,
  R_regim,
  R_akt,
  R_geo,
  R_neud,
  R_proiz,
  R_indikator,
  R_web_read,
  R_web_write,
  R_remont
};

struct CRight {
  int ID;
  const char *r;
};

class UserRight
{
public:
    static int global_user_right;
    
    static void setRight(int user_right);
    static bool isAdmin();
    static bool isEditGid();
    static bool isAct();
    static bool isEditGeo();
    static bool isNoDelete();
    static bool isPTS();
    static bool isKorrozia();
    static bool isWebRead();
    static bool isWebWrite();
    static bool isRemont();
};
