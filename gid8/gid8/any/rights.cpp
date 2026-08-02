#include "rights.h"

int UserRight::global_user_right = 0;

void UserRight::setRight(int user_right)
{
    global_user_right = user_right;
}

bool UserRight::isAdmin()
{
    return ! (global_user_right & (2 << R_admin));
}

bool UserRight::isEditGid()
{
    return global_user_right & (2 << R_regim);
}

bool UserRight::isAct()
{
    return ! (global_user_right & (2 << R_akt));
}

bool UserRight::isEditGeo()
{
    return ! (global_user_right & (2 << R_geo));
}

bool UserRight::isNoDelete()
{
    return global_user_right & (2 << R_neud);
}

bool UserRight::isPTS()
{
    return global_user_right & (2 << R_proiz);
}

bool UserRight::isKorrozia()
{
    return global_user_right & (2 << R_indikator);
}

bool UserRight::isWebRead()
{
    return global_user_right & (2 << R_web_read);
}

bool UserRight::isWebWrite()
{
    return global_user_right & (2 << R_web_write);
}

bool UserRight::isRemont()
{
    return global_user_right & (2 << R_remont);
}


