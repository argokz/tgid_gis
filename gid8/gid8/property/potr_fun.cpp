#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "PropertyDial.h"

#include <ui/PotrcxemaDialog.h>

bool readCxema(const QString & schemeNum, QString &otop, QString &gv, QString &vent, QString &teh, QString &cond, QString &rez, QString &gvs_all, bool &isEl)
{
    if (schemeNum == "") return false;

    isEl = false;
    otop = ""; // зависимая

    static QRegularExpression re("^(C?)([0-9]+)\\.([0-9]+)$");
    QRegularExpressionMatch match = re.match(schemeNum);
    if (match.hasMatch()) {
        QString c1 = match.captured(1);
        int n1 = match.captured(2).toInt();
        int n2 = match.captured(3).toInt();

        isEl = false;
//        if (n2 == 5 || n2 == 6 || n2 == 7 || n2 == 8 || schemeNum == "1.3" || schemeNum == "1.4" || schemeNum == "1.1") {

        if (schemeNum == "2.1") {  // только вентиляция
            gv = "";
            otop = ""; 
            isEl = false;
            vent = "1";
            return true;
        }
        
        
        if (n2 == 5 || n2 == 6 || n2 == 7 || n2 == 8 || schemeNum == "1.3" || schemeNum == "1.4") {
            otop = "z"; // Зависимая
            isEl = true;
        }

        else if (n2 == 1 || n2 == 2 || n2 == 3 || n2 == 4 || schemeNum == "2.1") {
            otop = "z"; // Зависимая
            isEl = false;
        }

        if (n2 == 9 || n2 == 10 || n2 == 11 || n2 == 12 || schemeNum == "1.5" || schemeNum == "1.6") {
            otop = "n"; // Независимая
            isEl = false;
        }

        if (c1 != "") {
            otop = "";
        }


        gv = "";

        switch(n1) {
        case 5: gv = "ps"; break;
        case 4: gv = "sm"; break;
        case 3: gv = "pr"; break;
        case 15: gv = "pw"; break;
        case 6: gv = "op"; break;
        case 7: gv = "oo"; break;
        }

        if (gv != "") gvs_all = "1";

        vent = "0";

        if (n1 > 2 && (n2 == 3 || n2 == 4 || n2 == 7 || n2 == 8 || n2 == 11 || n2 == 12 ) || n1 == 1 && (n2 == 2 || n2 == 4 || n2 == 6) || n1 == 2 && n2 == 1) {
            vent = "1";
        }

        rez = "0";
        if (n1 > 2 && n2%2 == 0 && (gv == "op" || gv == "oo")) {
            rez = "1";
        }
//        teh = "1";
//        cond = "1";
    }

    return true;
}

QString get_nomer(CPotrcxemaDialog &dlg)
{
    int ventil = dlg.m_bVentil ? 1 : 0;

    int otopln = dlg.m_bOtopl && dlg.m_radioZavis == 1 ? 1 : 0;
    int otoplz = dlg.m_bOtopl && dlg.m_radioZavis == 0 ? 1 : 0;
    int elevat = dlg.m_radioElevat == 0;
    
    int rez2 = dlg.m_bRez ? 1 : 0;
    int n1 = 0;
    int n2 = 0;

    if (dlg.m_bGws) {
        if (dlg.m_radioPodacha == 0) n1 = 6;
        if (dlg.m_radioPodacha == 1) n1 = 7;
        if (dlg.m_radioPodacha == 2) n1 = 3;
        if (dlg.m_radioPodacha == 3) n1 = 4;
        if (dlg.m_radioPodacha == 4) n1 = 5;
        if (dlg.m_radioPodacha == 5) n1 = 15;
    }
    else {
        n1 = 1;
    }


    if (!dlg.m_bOtopl) {
        n2 = 1;
    }


    if (!dlg.m_bOtopl && ventil && !dlg.m_bGws) {
        n1 = 2;
        n2 = 1;
        return "2.1";
    }
    else {
        if (n1 > 2) {
            if (otoplz) n2 = 1;
            if (otoplz && elevat) n2 = 5;
            if (otopln) n2 = 9;
            if (ventil) n2 += 2;
            if (rez2) n2 += 1;
        }
        if (n1 == 1) {
            if (otoplz) n2 = 1;
            if (otoplz && elevat) n2 = 3;
            if (otopln) n2 = 5;
            if (ventil) n2 += 1;
        }
    }


    QString c2 = QString("%1.%2").arg(n1).arg(n2);
    if (dlg.m_bOtopl == 0) c2 = "C" + c2;

    return c2;
}


bool potrcxemaDialogCallback(CPotrcxemaDialog *prop)
{
    QString schemeNumber = get_nomer(*prop);
    prop->m_labelNomer = schemeNumber;
    prop->setSchemeNumber(schemeNumber);
    bool zavis = prop->m_radioZavis == 0; // Зависимая
    bool gvs = prop->m_bGws;
    bool otop = prop->m_bOtopl;

    int gvs_typ = prop->m_radioPodacha;

    prop->ui.radio_Pr;
    prop->ui.radio_Sm;
    prop->ui.radio_Ps;
    prop->ui.radio_Pw;



    prop->ui.radio_Zavis->setEnabled(otop);
    prop->ui.radio_Nezavis->setEnabled(otop);

    prop->ui.radio_Elevat->setEnabled(zavis && otop);
    prop->ui.radio_Bezelevat->setEnabled(zavis && otop);

    prop->ui.radio_Podacha->setEnabled(gvs);
    prop->ui.radio_Obratka->setEnabled(gvs);
    prop->ui.radio_Pr->setEnabled(gvs);
    prop->ui.radio_Sm->setEnabled(gvs);
    prop->ui.radio_Ps->setEnabled(gvs);
    prop->ui.radio_Pw->setEnabled(gvs);

    bool rez = otop && gvs && gvs_typ <= 2;

    prop->ui.check_Rez->setEnabled(rez);
    if (!rez) {
        prop->ui.check_Rez->setCheckState(Qt::Unchecked);
    }

    prop->ui.check_Rez->setEnabled(otop && gvs && gvs_typ <= 1);

    return true;
}


QString fun_schemeNum(PropertyDial *parent, const QString & schemeNum)
//    QString &otop, QString &gv, QString &vent, QString &teh, QString & cond, QString & rez, QString &gvs_all)
{

//    schemeNum();
//    setValue("name", QVariant("!!!!!"));
//    setId("automDegs", 3);
//    QVariant qq = getValue("name");
//    QMessageBox::warning(this, tr("!"), qq.toString());

//    QString schemeNum = pr.value.toString();
    bool isEl;
    QString otop, gv, vent, teh, cond, rez, gvs_all;
    readCxema(schemeNum, otop, gv, vent, teh, cond, rez, gvs_all, isEl);

    double m_otopl = 0, m_gv = 0, m_ventil = 0, m_rez = 10;
    double m_dog_otopl = 0, m_dog_gv = 0, m_dog_ventil = 0, m_dog_rez = 10;

    if (otop == "n") m_otopl = parent->getValue("calcHLindep").toDouble();
    if (otop == "z") m_otopl = parent->getValue("calcHLdep").toDouble();

    if (gv == "pr") m_gv = parent->getValue("avgHLGVScloseParall").toDouble();
    if (gv == "sm") m_gv = parent->getValue("avgHLGVScloseMix").toDouble();
    if (gv == "ps") m_gv = parent->getValue("avgHLGVScloseConseq").toDouble();
    if (gv == "pw") m_gv = parent->getValue("avgHLGVSclosePreON").toDouble();
    if (gv == "op") m_gv = parent->getValue("avgHLGVSopenFlow").toDouble();
    if (gv == "oo") m_gv = parent->getValue("avgHLGVSopenRet").toDouble();

    if (rez == "1") m_rez = parent->getValue("circHLosOpen").toDouble();
    if (vent == "1") m_ventil = parent->getValue("calcHLventil").toDouble();


    if (otop == "n") m_dog_otopl = parent->getValue("contcalcHLindep").toDouble();
    if (otop == "z") m_dog_otopl = parent->getValue("contCalcHLdep").toDouble();

    if (gv == "pr") m_dog_gv = parent->getValue("contAvgHLGVScloseParall").toDouble();
    if (gv == "sm") m_dog_gv = parent->getValue("contAvgHLGVScloseMix").toDouble();
    if (gv == "ps") m_dog_gv = parent->getValue("contAvgHLGVScloseConseq").toDouble();
    if (gv == "pw") m_dog_gv = parent->getValue("contAvgHLGVSclosePreON").toDouble();
    if (gv == "op") m_dog_gv = parent->getValue("contAvgHLGVSopenFlow").toDouble();
    if (gv == "oo") m_dog_gv = parent->getValue("contAvgHLGVSopenRet").toDouble();

    CPotrcxemaDialog dlg(parent);

    dlg.m_labelNomer = schemeNum;
    dlg.m_bOtopl = otop != "";
    dlg.m_bGws = gv != "";
    dlg.m_bRez = rez == "1";
    dlg.m_bVentil = vent == "1";

    dlg.m_radioZavis = otop == "z" ? 0 : 1;
    dlg.m_radioElevat = isEl ? 0 : 1;
    dlg.m_radioPodacha =
        gv == "op" ? 0 :
        gv == "oo" ? 1 :
        gv == "pr" ? 2 :
        gv == "sm" ? 3 :
        gv == "ps" ? 4 :
        gv == "pw" ? 5 : 6;

    dlg.setCallback(potrcxemaDialogCallback);
    potrcxemaDialogCallback(&dlg);
    dlg.write();
    if (dlg.exec() == QDialog::Accepted) {

        QString c2 = get_nomer(dlg);

        readCxema(c2, otop, gv, vent, teh, cond, rez, gvs_all, isEl);

        parent->setValue1("calcHLindep", 0);
        parent->setValue1("calcHLdep", 0);

        parent->setValue1("avgHLGVScloseParall", 0);
        parent->setValue1("avgHLGVScloseMix", 0);
        parent->setValue1("avgHLGVScloseConseq", 0);
        parent->setValue1("avgHLGVSclosePreON", 0);
        parent->setValue1("avgHLGVSopenFlow", 0);
        parent->setValue1("avgHLGVSopenRet", 0);

        parent->setValue1("circHLosOpen", 0);
        parent->setValue1("calcHLventil", 0);

        parent->setValue1("contcalcHLindep", 0);
        parent->setValue1("contCalcHLdep", 0);

        parent->setValue1("contAvgHLGVScloseParall", 0);
        parent->setValue1("contAvgHLGVScloseMix", 0);
        parent->setValue1("contAvgHLGVScloseConseq", 0);
        parent->setValue1("contAvgHLGVSclosePreON", 0);
        parent->setValue1("contAvgHLGVSopenFlow", 0);
        parent->setValue1("contAvgHLGVSopenRet", 0);


        if (otop == "n") parent->setValue1("calcHLindep", m_otopl);
        if (otop == "z") parent->setValue1("calcHLdep", m_otopl);

        if (gv == "pr") parent->setValue1("avgHLGVScloseParall", m_gv);
        if (gv == "sm") parent->setValue1("avgHLGVScloseMix", m_gv);
        if (gv == "ps") parent->setValue1("avgHLGVScloseConseq", m_gv);
        if (gv == "pw") parent->setValue1("avgHLGVSclosePreON", m_gv);
        if (gv == "op") parent->setValue1("avgHLGVSopenFlow", m_gv);
        if (gv == "oo") parent->setValue1("avgHLGVSopenRet", m_gv);

        if (rez == "1") parent->setValue1("circHLosOpen", m_rez);
        if (vent == "1") parent->setValue1("calcHLventil", m_ventil);


        if (otop == "n") parent->setValue1("contcalcHLindep", m_dog_otopl);
        if (otop == "z") parent->setValue1("contCalcHLdep", m_dog_otopl);

        if (gv == "pr") parent->setValue1("contAvgHLGVScloseParall", m_dog_gv);
        if (gv == "sm") parent->setValue1("contAvgHLGVScloseMix", m_dog_gv);
        if (gv == "ps") parent->setValue1("contAvgHLGVScloseConseq", m_dog_gv);
        if (gv == "pw") parent->setValue1("contAvgHLGVSclosePreON", m_dog_gv);
        if (gv == "op") parent->setValue1("contAvgHLGVSopenFlow", m_dog_gv);
        if (gv == "oo") parent->setValue1("contAvgHLGVSopenRet", m_dog_gv);


        return c2;
//        setValue(pr.key, c2);
    }
    return  "";
}



