#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QDomDocument>

//#define HASP_HL 1

#ifdef HASP_HL

#include "hasp.h"
#include "hasp/hasp_hl.h"

//#define NET 1
//#define COUNTER 1
//#define OST 1500

#include "hasp_cities.h"

QString getHaspErrorMessage(int errorCode);

static int hasp_ost = -1;

int get_ost_hasp()
{
    return hasp_ost;
}

bool is_hasp()
{
#ifdef HASP_HL
    return true;
#else
    return false;
#endif
}

static QString port_type = "";
static QString port_address = "";

static QString serveraddress_protocol = "";
static QString serveraddress_address = "";


QString getHaspInfo()
{
    QString s = "";

    if (is_hasp()) {
        s = "Защищенная";
        if (serveraddress_protocol == "IP") {
            s = QString(QObject::tr("Защищенная, ключ на сервере %1")).arg(serveraddress_address);
        }
        if (hasp_ost >= 0) {
            s += QString(" осталось %1 запусков").arg(hasp_ost);
        }
    }

    return s;
}

bool check_password();

bool check_hasp(hasp_status_t ret)
{
    if (ret) {
        QMessageBox::warning(nullptr, "", getHaspErrorMessage(ret));
        return false;
    }
    return true;
}


bool strict_hasp(hasp_handle_t handle)
{
#ifndef STRICT_HASP
    return true;
#endif
    int strict_hasp;

    hasp_status_t ret =  hasp_read(handle, HASP_FILEID_MAIN, 0, 4, (void*)&strict_hasp);

    if (!check_hasp(ret)) {
        return false;
    }

    if (strict_hasp != STRICT_HASP) {
        if (!check_password()) return false;
        strict_hasp = STRICT_HASP;
        ret = hasp_write(handle, HASP_FILEID_MAIN, 0, 4, (void*)&strict_hasp);
    }
    return true;
}

bool hasp_counter(hasp_handle_t handle)
{
#ifndef COUNTER
    return true;
#endif

    long ost;
    hasp_status_t ret =  hasp_read(handle, HASP_FILEID_MAIN, 4, 4, (void*)&ost);
    if (!check_hasp(ret)) {
        return false;
    }

    ost--;

    hasp_ost = ost;

    if (ost <= 0 || ost > 60000) {
        if (!check_password()) return false;
        ost = OST;
    }

    ret =  hasp_write(handle, HASP_FILEID_MAIN, 4, 4, (void*)&ost);

//    QMessageBox::warning(nullptr, "", QString("У вас осталось %1 запусков").arg(ost));
    return true;
}

#define GET_VARIABLE_NAME(Variable) (#Variable)

#define GEMAI "27qqbQX9Z33r+KM0xoyFpbYInaElBgPPfANtmsQ1yrABNllW2ZQ+HJXEMrFJ+MV+EWW74XYxfxhk511FmyKD/0XQNWv75lusPC+JW2WaxYFUMyua+Vr7bSg03oRPMhABYY7KluOye6dhdFBzTZUSoeCYV96vmjpCQt76Kf1gb0Zo7eL09GerAImcoAhlQH7r9vCYdNOFrCX17AzRjOMfc/v3CDy5oVaAfjXk1HFVT4oVPanTAT3wziltKp2deYNJSZzC1ml1fce4qxutszuGI8rmgZ22jZe2M9DxGHH+z5XMsbG/By6gooWJ/58wajFg7GcXY/wBnjv1GLhPyVP1yhFL13ksvl+kOd4VbsGki/8QMsMDFvoY/xSVKk/xTSNWQl4ufJBzqVCSNLeHwwhhjx2ysg2XhYnOLy1ElUmpwd2VeKZGhV5rqxfAT4ZVGCPVtFfVUf5a8g7cnEoXX44Xzzg73S95vy3UWCPkBSOtBjCe7LewaXU9ecUdqEd9bo892qTAOUmrH2UUWUqyk/Neht5eneK7Df+ZJh005CL6DrC/odeszQS02nv84I3bAaWAAjzgQlhlcynluFdeMaACdUA143GEmfDLpwh9tUMRVWEXYF0SI5pO06vNIBj6caSSZU4glKKcAGSQBhve1oQfT9K85QXpkWm3txXuoYEVpRko/eegtmWR92xlR6DrPdSgijNDBnyWZwaTvU8PwPeTVjPwvfEBCw=="

bool hasp()
{
#ifndef HASP_HL
    return true;
#endif
    unsigned char vendor_code[] = GEMAI;
    hasp_handle_t handle;
    QString name_of_hasp = GET_VARIABLE_NAME(GEMAI);


    hasp_status_t ret;
    QString msg = "";

#ifdef NET

#if NET_AND_LOCAL
    ret = hasp_login(HASP_PROGNUM_FEATURETYPE | HASP_PROGNUM_OPT_TS,(hasp_vendor_code_t) vendor_code, &handle);
    msg = QString(QObject::tr(R"(
        <p>Для запуска этой программы необходимо, чтобы на вашем компьютере был установлен ключ защиты HASP
        с кодом %1 и драйвер HASP или чтобы на одном из компьютеров в вашей сети был установлен ключ защиты HASP
        с кодом %1 и запущен менеджер лицензий HASP</p>)")).arg(name_of_hasp);
#else
    ret = hasp_login(HASP_PROGNUM_OPT_NO_LOCAL | HASP_PROGNUM_FEATURETYPE | HASP_PROGNUM_OPT_TS,(hasp_vendor_code_t) vendor_code, &handle);
    msg = QString(QObject::tr("Для запуска этой программы необходимо, чтобы на одном из компьютеров в вашей сети был установлен ключ защиты HASP с кодом %1 и запущен менеджер лицензий HASP")).arg(name_of_hasp);
#endif

#else
    ret = hasp_login(HASP_PROGNUM_OPT_NO_REMOTE | HASP_PROGNUM_FEATURETYPE,(hasp_vendor_code_t) vendor_code, &handle);
    msg = QString(QObject::tr("Для запуска этой программы необходимо, чтобы на вашем компьютере был установлен ключ защиты HASP с кодом %1 и драйвер HASP")).arg(name_of_hasp);
#endif

    if (ret != 0) {
      if (ret == 7) {
        QMessageBox::warning(nullptr, "", msg);
      }
      else {
          check_hasp(ret);
      }
      return false;
    }

    if (!strict_hasp(handle)) return false;
    if (!hasp_counter(handle)) return false;

#if 1
    char * info1;
    char * info2;
    char * info3;
    ret = hasp_get_sessioninfo(handle,HASP_UPDATEINFO,  &info1);
    ret = hasp_get_sessioninfo(handle,HASP_SESSIONINFO,  &info2);
    ret = hasp_get_sessioninfo(handle,HASP_KEYINFO,  &info3);

    if (ret == 0) {
//        QMessageBox::warning(nullptr, "", info);
//        QInputDialog::getMultiLineText(nullptr, "title", "HASP_UPDATEINFO", info1);
//        QInputDialog::getMultiLineText(nullptr, "title", "HASP_SESSIONINFO", info2);
//        QInputDialog::getMultiLineText(nullptr, "title", "HASP_KEYINFO", info3);

        QDomDocument dom;
        dom.setContent(QString(info2));

        struct Cfeature {
            unsigned int featureid;
            unsigned int maxlogins;
            unsigned int currentlogins;
            QString activations;
        } f;

        auto feature = dom.firstChildElement("hasp_info").firstChildElement("feature");

        f.featureid     = feature.firstChildElement("featureid").firstChild().nodeValue().toInt();
        f.maxlogins     = feature.firstChildElement("maxlogins").firstChild().nodeValue().toInt();
        f.currentlogins = feature.firstChildElement("currentlogins").firstChild().nodeValue().toInt();
        f.activations   = feature.firstChildElement("activations").firstChild().nodeValue();

        QDomDocument dom3;
        dom3.setContent(QString(info3));

        auto serveraddress = dom3.firstChildElement("hasp_info").firstChildElement("keyspec").firstChildElement("serveraddress");
        auto port = dom3.firstChildElement("hasp_info").firstChildElement("keyspec").firstChildElement("port");

        port_type = port.firstChildElement("type").firstChild().nodeValue();
        port_address = port.firstChildElement("address").firstChild().nodeValue();

        serveraddress_protocol = serveraddress.firstChildElement("protocol").firstChild().nodeValue();
        serveraddress_address = serveraddress.firstChildElement("address").firstChild().nodeValue();

        hasp_free(info1);
        hasp_free(info2);
        hasp_free(info3);
    }
    else {
        check_hasp(ret);
    }
#endif

    ret = hasp_logout(handle);

    if (!check_hasp(ret)) {
        return false;
    }

    return true;
}
#else
bool hasp()
{
    return true;
}

QString getHaspInfo()
{
    return "";
}

#endif
