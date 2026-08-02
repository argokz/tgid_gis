#include "stdafx.h"
#include "MainFrm.h"
#include <locale.h>
#include "css.h"
#include "ado.h"
#include "win.h"
#include <fstream>
#include <sstream>

static CSS css;
void print_index1(FILE* f, const char* title);
void print_index2(FILE* f);
FILE* print_doc(FILE* fi, const char* fn, const char* title, void (*filtr) (CString& s1) = NULL);
void print_doc_end(FILE* f);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
void printTd(CAdoFile* ado, FILE* f, const char* param, const int type, int count = 1);
void printLE(FILE* g, FILE* f, CAdoFile* mAdo, CString argPath);
void printPriemnik(FILE* g, FILE* f, CAdoFile* mAdo, CString argPath);
void printTransformer(FILE* g, FILE* f, CAdoFile* mAdo, CString argPath);
void printElectroEngine(FILE* g, FILE* file, CAdoFile* mAdo, CString argPath);
void printDGS(FILE* g, FILE* f, CAdoFile* mAdo, CString argPath);
void printLiftingEqipment(FILE* g, FILE* file, CAdoFile* mAdo, CString argPath);

void printTd(FILE* g, double data);
void printTd(FILE* g, int data);

void initPassportElectricalNetwork(CAdoFile* ado, HWND CWnd, CString argPath)
{

    CWaitCursor wait;

    CString tmpName;

    tmpName.Format("%s\\index_electro.html", getenv("TMP"));


    FILE* f = fopen(tmpName, "w");
    if (!f) {
        AfxMessageBox("Закройте предыдущий паспорт");
        return;
    }

    setlocale(LC_NUMERIC, "");

    print_index1(f, "");

    FILE* g = 0;

    /*Линии Электропередач*/
    printLE(g, f, ado, argPath);
    /*Приемник*/
    printPriemnik(g, f, ado, argPath);
    /*Трансформаторы*/
    printTransformer(g, f, ado, argPath);
    /*Электродвигатели*/
    printElectroEngine(g, f, ado, argPath);
    /*Дизель-генер.установки*/
    printDGS(g, f, ado, argPath);
    /*Грузоподъемное оборудование*/
    printLiftingEqipment(g, f, ado, argPath);

    print_index2(f);
    fclose(f);
    setlocale(LC_NUMERIC, "eng");

    tmpName.Format("%s\\index_electro.html", getenv("TMP"));

    CString cssName;
    cssName.Format("%s\\style.css", getenv("TMP"));
    css.print(cssName);


    HINSTANCE hRun = ShellExecute(CWnd, "open", "excel", "\"" + tmpName + "\"", NULL, SW_SHOW);

}


void printTd(FILE* g, double data) {
    if (data != 0)
        fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", data);
    else
        fprintf(g, "<td style='text-align: center; vertical-align: middle;'></td>");
}

void printTd(FILE* g, int data) {
    if (data != 0)
        fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%d</td>", data);
    else
        fprintf(g, "<td style='text-align: center; vertical-align: middle;'></td>");
}

void printLE(FILE* g, FILE* file, CAdoFile* mAdo, CString argPath)
{
    struct LE { /*Линии Электропередач*/
        CString nameReceiver, nameSource,
            location;
        double voltage;
        CString typeStrip, mark;
        int countLine;
        double extentLine, extentCabel;
        CString workFider, reservFider, date;
        LE* next;
    };
    LE le;
    LE* tmpLe = &le;

    g = print_doc(file, "electro\\a0.htm", _TR("Ф1.ЛЭП"));
    if (g) {
        CString q, fStr;
        CString qName;
        qName.Format("le");

        fStr.Format("%ssql\\objects\\electro\\%s.sql", argPath, qName);
        ifstream f(fStr);
        if (f.good())
        {
            q = readFile(fStr);

            bool ret = mAdo->openTable0(q);
            if (!ret)
            {
                AfxMessageBox("Ошибка в запросе при формировании паспорта (Линии Электропередач)");
                return;
            }

            bool notEmpty = false;
            while (!mAdo->isEOF()) {

                tmpLe->nameReceiver = mAdo->readStr("naimenovanie_priemnika_es");
                tmpLe->nameSource = mAdo->readStr("naimenovanie_istochnika_es");
                tmpLe->location = mAdo->readStr("mestopolozhenie");

                tmpLe->voltage = mAdo->read_double("napryazhenie__kv");
                tmpLe->typeStrip = mAdo->readStr("tip_prokladki_kabelya");
                tmpLe->mark = mAdo->readStr("marka_kabelya");

                tmpLe->countLine = mAdo->read_long("count_lep");
                tmpLe->extentLine = mAdo->read_double("protyazhennost__linii_m");
                tmpLe->extentCabel = mAdo->read_double("protyazhennost__m");
                tmpLe->workFider = mAdo->readStr("fidera");
                tmpLe->reservFider = mAdo->readStr("fidera_rez");
                tmpLe->date = mAdo->readStr("data_vvoda_v_ekspluatatsiyu");

                mAdo->MoveNext();
                if (mAdo->isEOF())
                {
                    tmpLe->next = nullptr;
                }

                else
                {
                    tmpLe->next = new struct LE;
                }
                tmpLe = tmpLe->next;
                if (!notEmpty)
                    notEmpty = true;
            }



            tmpLe = &le;
            if (notEmpty)
            {
                while (tmpLe != nullptr) {
                    fprintf(g, "<tr>");

                    fprintf(g, "<td>%s</td>", tmpLe->nameReceiver);
                    fprintf(g, "<td>%s</td>", tmpLe->nameSource);
                    fprintf(g, "<td>%s</td>", tmpLe->location);

                    printTd(g, tmpLe->voltage);

                    fprintf(g, "<td>%s</td>", tmpLe->typeStrip);
                    fprintf(g, "<td>%s</td>", tmpLe->mark);

                    printTd(g, tmpLe->countLine);

                    printTd(g, tmpLe->extentLine);
                    printTd(g, tmpLe->extentCabel);

                    fprintf(g, "<td>%s</td>", tmpLe->workFider);
                    fprintf(g, "<td>%s</td>", tmpLe->reservFider);
                    fprintf(g, "<td>%s</td>", tmpLe->date);


                    fprintf(g, "</tr>");

                    tmpLe = tmpLe->next;


                }
            }
            print_doc_end(g);
            if (notEmpty) {
                tmpLe = le.next;
                le.next = nullptr;
                while (tmpLe != nullptr)
                {
                    LE* forDelete = tmpLe;
                    tmpLe = tmpLe->next;
                    forDelete->next = nullptr;
                    delete forDelete;
                }
            }
        }
    }
}

void printPriemnik(FILE* g, FILE* file, CAdoFile* mAdo, CString argPath)
{
    struct Priemnik { /*Приемник*/
        CString nameReceiver, type;
        double maxLoad;
        int countIndustrialTransformer, countOwnTransformer, countElectroSupply, countElectroReturn, countElectroDrainage, countElectroCrimping, countDiselGeneratorInstall;
        CString frequencyControlledDrive;
        Priemnik* next;
    };
    Priemnik priemnik;
    Priemnik* tmpPriemnik = &priemnik;

    g = print_doc(file, "electro\\a1.htm", _TR("Ф2.Приемники электроснабжения"));
    if (g) {
        CString q, fStr;
        CString qName;
        qName.Format("priemnik");

        fStr.Format("%ssql\\objects\\electro\\%s.sql", argPath, qName);
        ifstream f(fStr);
        if (f.good())
        {
            q = readFile(fStr);

            bool ret = mAdo->openTable0(q);
            if (!ret)
            {
                AfxMessageBox("Ошибка в запросе при формировании паспорта (Приемники электроснабжения)");
                return;
            }

            bool notEmpty = false;
            while (!mAdo->isEOF()) {

                tmpPriemnik->nameReceiver = mAdo->readStr("naimenovanie_priemnika_es");
                tmpPriemnik->type = mAdo->readStr("typ_priemnika");

                tmpPriemnik->maxLoad = mAdo->read_double("maksimalno_dopustimaya_nagruzka_vneshnego_vvoda_rp__kvt");

                tmpPriemnik->countIndustrialTransformer = mAdo->read_long("trans_pro_kol");
                tmpPriemnik->countOwnTransformer = mAdo->read_long("trans_sob_kol");
                tmpPriemnik->countElectroSupply = mAdo->read_long("edv_pod_kol");
                tmpPriemnik->countElectroReturn = mAdo->read_long("edv_obr_kol");
                tmpPriemnik->countElectroDrainage = mAdo->read_long("edv_dren_kol");
                tmpPriemnik->countElectroCrimping = mAdo->read_long("edv_opr_kol");
                tmpPriemnik->countDiselGeneratorInstall = mAdo->read_long("diz_kol");

                int r = mAdo->read_long("chast_reg_reg");
                tmpPriemnik->frequencyControlledDrive = r ?  "имеется" : "не имеется";

                mAdo->MoveNext();
                if (mAdo->isEOF())
                {
                    tmpPriemnik->next = nullptr;
                }

                else
                {
                    tmpPriemnik->next = new struct Priemnik;
                }
                tmpPriemnik = tmpPriemnik->next;
                if (!notEmpty)
                    notEmpty = true;
            }



            tmpPriemnik = &priemnik;
            if (notEmpty)
            {
                while (tmpPriemnik != nullptr) {
                    fprintf(g, "<tr>");

                    fprintf(g, "<td>%s</td>", tmpPriemnik->nameReceiver);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->type);

                    printTd(g, tmpPriemnik->maxLoad);

                    printTd(g, tmpPriemnik->countIndustrialTransformer);
                    printTd(g, tmpPriemnik->countOwnTransformer);
                    printTd(g, tmpPriemnik->countElectroSupply);
                    printTd(g, tmpPriemnik->countElectroReturn);
                    printTd(g, tmpPriemnik->countElectroDrainage);
                    printTd(g, tmpPriemnik->countElectroCrimping);
                    printTd(g, tmpPriemnik->countDiselGeneratorInstall);

                    fprintf(g, "<td>%s</td>", tmpPriemnik->frequencyControlledDrive);


                    fprintf(g, "</tr>");

                    tmpPriemnik = tmpPriemnik->next;


                }
            }
            print_doc_end(g);
            if (notEmpty) {
                tmpPriemnik = priemnik.next;
                priemnik.next = nullptr;
                while (tmpPriemnik != nullptr)
                {
                    Priemnik* forDelete = tmpPriemnik;
                    tmpPriemnik = tmpPriemnik->next;
                    forDelete->next = nullptr;
                    delete forDelete;
                }
            }
        }
    }
}


void printTransformer(FILE* g, FILE* file, CAdoFile* mAdo, CString argPath)
{
    struct Transformer { /*Трансформатор*/
        CString nameReceiver, typeReceiver, purpose, type;
        int count;
        double sumOfPower, highVoltagePower, lowVoltagePower, shortСircuitVoltage, idleVoltage;
        int date;
        Transformer* next;
    };
    Transformer priemnik;
    Transformer* tmpPriemnik = &priemnik;

    g = print_doc(file, "electro\\a2.htm", _TR("Ф3.Трансформаторы"));
    if (g) {
        CString q, fStr;
        CString qName;
        qName.Format("transformer");

        fStr.Format("%ssql\\objects\\electro\\%s.sql", argPath, qName);
        ifstream f(fStr);
        if (f.good())
        {
            q = readFile(fStr);

            bool ret = mAdo->openTable0(q);
            if (!ret)
            {
                AfxMessageBox("Ошибка в запросе при формировании паспорта (Трансформаторы)");
                return;
            }

            bool notEmpty = false;
            while (!mAdo->isEOF()) {

                tmpPriemnik->nameReceiver = mAdo->readStr("naimenovanie_priemnika_es");
                tmpPriemnik->typeReceiver = mAdo->readStr("typ_priemnika");
                tmpPriemnik->purpose = mAdo->readStr("transfPurpo");
                tmpPriemnik->type = mAdo->readStr("transfType");

                tmpPriemnik->count = mAdo->read_long("kol");

                tmpPriemnik->sumOfPower = mAdo->read_double("pow");
                tmpPriemnik->highVoltagePower = mAdo->read_double("voltageHi");
                tmpPriemnik->lowVoltagePower = mAdo->read_double("voltageLo");
                tmpPriemnik->shortСircuitVoltage = mAdo->read_double("Ukz");
                tmpPriemnik->idleVoltage = mAdo->read_double("Ixx");

                tmpPriemnik->date = mAdo->read_long("god");

                mAdo->MoveNext();
                if (mAdo->isEOF())
                {
                    tmpPriemnik->next = nullptr;
                }

                else
                {
                    tmpPriemnik->next = new struct Transformer;
                }
                tmpPriemnik = tmpPriemnik->next;
                if (!notEmpty)
                    notEmpty = true;
            }



            tmpPriemnik = &priemnik;
            if (notEmpty)
            {
                while (tmpPriemnik != nullptr) {
                    fprintf(g, "<tr>");

                    fprintf(g, "<td>%s</td>", tmpPriemnik->nameReceiver);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->typeReceiver);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->purpose);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->type);

                    printTd(g, tmpPriemnik->count);

                    printTd(g,  tmpPriemnik->sumOfPower);
                    printTd(g,  tmpPriemnik->highVoltagePower);
                    printTd(g,  tmpPriemnik->lowVoltagePower);
                    printTd(g,  tmpPriemnik->shortСircuitVoltage);
                    printTd(g,  tmpPriemnik->idleVoltage);

                    printTd(g, tmpPriemnik->date);

                    fprintf(g, "</tr>");

                    tmpPriemnik = tmpPriemnik->next;


                }
            }
            print_doc_end(g);
            if (notEmpty) {
                tmpPriemnik = priemnik.next;
                priemnik.next = nullptr;
                while (tmpPriemnik != nullptr)
                {
                    Transformer* forDelete = tmpPriemnik;
                    tmpPriemnik = tmpPriemnik->next;
                    forDelete->next = nullptr;
                    delete forDelete;
                }
            }
        }
    }
}


void printElectroEngine(FILE* g, FILE* file, CAdoFile* mAdo, CString argPath)
{
    struct ElectroEngine { /*Электродвигатели*/
        CString nameReceiver, typeReceiver, purpose, type, mark;
        double power, rotationFrequency;
        CString seria;
        double rangeSpeed, speedControlAccuracy;
        ElectroEngine* next;
    };
    ElectroEngine priemnik;
    ElectroEngine* tmpPriemnik = &priemnik;

    g = print_doc(file, "electro\\a3.htm", _TR("Ф4.Электродвигатели"));
    if (g) {
        CString q, fStr;
        CString qName;
        qName.Format("electro_engine");

        fStr.Format("%ssql\\objects\\electro\\%s.sql", argPath, qName);
        ifstream f(fStr);
        if (f.good())
        {
            q = readFile(fStr);

            bool ret = mAdo->openTable0(q);
            if (!ret)
            {
                AfxMessageBox("Ошибка в запросе при формировании паспорта (Электродвигатели)");
                return;
            }

            bool notEmpty = false;
            while (!mAdo->isEOF()) {

                tmpPriemnik->nameReceiver = mAdo->readStr("naimenovanie_priemnika_es");
                tmpPriemnik->typeReceiver = mAdo->readStr("typ_priemnika");
                tmpPriemnik->purpose = mAdo->readStr("edvPurpos");
                tmpPriemnik->type = mAdo->readStr("typeEdv");
                tmpPriemnik->mark = mAdo->readStr("marka_edv");

                tmpPriemnik->power = mAdo->read_double("pow");
                tmpPriemnik->rotationFrequency = mAdo->read_double("freq");

                tmpPriemnik->seria = mAdo->readStr("seria");

                tmpPriemnik->rangeSpeed = mAdo->read_double("diap");
                tmpPriemnik->speedControlAccuracy = mAdo->read_double("prec");


                mAdo->MoveNext();
                if (mAdo->isEOF())
                {
                    tmpPriemnik->next = nullptr;
                }

                else
                {
                    tmpPriemnik->next = new struct ElectroEngine;
                }
                tmpPriemnik = tmpPriemnik->next;
                if (!notEmpty)
                    notEmpty = true;
            }



            tmpPriemnik = &priemnik;
            if (notEmpty)
            {
                while (tmpPriemnik != nullptr) {
                    fprintf(g, "<tr>");

                    fprintf(g, "<td>%s</td>", tmpPriemnik->nameReceiver);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->typeReceiver);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->purpose);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->type);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->mark);

                    printTd(g,  tmpPriemnik->power);
                    printTd(g,  tmpPriemnik->rotationFrequency);

                    fprintf(g, "<td>%s</td>", tmpPriemnik->seria);

                    printTd(g,  tmpPriemnik->rangeSpeed);
                    printTd(g,  tmpPriemnik->speedControlAccuracy);

                    fprintf(g, "</tr>");

                    tmpPriemnik = tmpPriemnik->next;
                }
            }
            print_doc_end(g);
            if (notEmpty) {
                tmpPriemnik = priemnik.next;
                priemnik.next = nullptr;
                while (tmpPriemnik != nullptr)
                {
                    ElectroEngine* forDelete = tmpPriemnik;
                    tmpPriemnik = tmpPriemnik->next;
                    forDelete->next = nullptr;
                    delete forDelete;
                }
            }
        }
    }
}

void printDGS(FILE* g, FILE* file, CAdoFile* mAdo, CString argPath)
{
    struct DGS { /*Дизель-генераторные установки*/
        CString nameReceiver, typeReceiver;
        double power;
        CString description, note;
        DGS* next;
    };
    DGS priemnik;
    DGS* tmpPriemnik = &priemnik;

    g = print_doc(file, "electro\\a4.htm", _TR("Ф5.Дизель-генер.установки"));
    if (g) {
        CString q, fStr;
        CString qName;
        qName.Format("dgs");

        fStr.Format("%ssql\\objects\\electro\\%s.sql", argPath, qName);
        ifstream f(fStr);
        if (f.good())
        {
            q = readFile(fStr);

            bool ret = mAdo->openTable0(q);
            if (!ret)
            {
                AfxMessageBox("Ошибка в запросе при формировании паспорта (Дизель-генераторные установки)");
                return;
            }

            bool notEmpty = false;
            while (!mAdo->isEOF()) {

                tmpPriemnik->nameReceiver = mAdo->readStr("naimenovanie_priemnika_es");
                tmpPriemnik->typeReceiver = mAdo->readStr("typ_priemnika");

                tmpPriemnik->power = mAdo->read_double("pow");

                tmpPriemnik->description = mAdo->readStr("description");
                tmpPriemnik->note = mAdo->readStr("primechanie");

                mAdo->MoveNext();
                if (mAdo->isEOF())
                {
                    tmpPriemnik->next = nullptr;
                }

                else
                {
                    tmpPriemnik->next = new struct DGS;
                }
                tmpPriemnik = tmpPriemnik->next;
                if (!notEmpty)
                    notEmpty = true;
            }



            tmpPriemnik = &priemnik;
            if (notEmpty)
            {
                while (tmpPriemnik != nullptr) {
                    fprintf(g, "<tr>");

                    fprintf(g, "<td>%s</td>", tmpPriemnik->nameReceiver);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->typeReceiver);

                    printTd(g, tmpPriemnik->power);

                    fprintf(g, "<td>%s</td>", tmpPriemnik->description);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->note);

                    fprintf(g, "</tr>");

                    tmpPriemnik = tmpPriemnik->next;
                }
            }
            print_doc_end(g);
            if (notEmpty) {
                tmpPriemnik = priemnik.next;
                priemnik.next = nullptr;
                while (tmpPriemnik != nullptr)
                {
                    DGS* forDelete = tmpPriemnik;
                    tmpPriemnik = tmpPriemnik->next;
                    forDelete->next = nullptr;
                    delete forDelete;
                }
            }
        }
    }
}

void printLiftingEqipment(FILE* g, FILE* file, CAdoFile* mAdo, CString argPath)
{
    struct LiftingEqipment { /*Грузоподъемное оборудование*/
        CString nameReceiver, typeReceiver;
        CString description, note;
        LiftingEqipment* next;
    };
    LiftingEqipment priemnik;
    LiftingEqipment* tmpPriemnik = &priemnik;

    g = print_doc(file, "electro\\a5.htm", _TR("Ф6.Грузоподъемное оборудование"));
    if (g) {
        CString q, fStr;
        CString qName;
        qName.Format("lifting_equipment");

        fStr.Format("%ssql\\objects\\electro\\%s.sql", argPath, qName);
        ifstream f(fStr);
        if (f.good())
        {
            q = readFile(fStr);

            bool ret = mAdo->openTable0(q);
            if (!ret)
            {
                AfxMessageBox("Ошибка в запросе при формировании паспорта (Грузоподъемное оборудование)");
                return;
            }

            bool notEmpty = false;
            while (!mAdo->isEOF()) {

                tmpPriemnik->nameReceiver = mAdo->readStr("naimenovanie_priemnika_es");
                tmpPriemnik->typeReceiver = mAdo->readStr("typ_priemnika");

                tmpPriemnik->description = mAdo->readStr("description");
                tmpPriemnik->note = mAdo->readStr("primechanie");

                mAdo->MoveNext();
                if (mAdo->isEOF())
                {
                    tmpPriemnik->next = nullptr;
                }

                else
                {
                    tmpPriemnik->next = new struct LiftingEqipment;
                }
                tmpPriemnik = tmpPriemnik->next;
                if (!notEmpty)
                    notEmpty = true;
            }



            tmpPriemnik = &priemnik;
            if (notEmpty)
            {
                while (tmpPriemnik != nullptr) {
                    fprintf(g, "<tr>");

                    fprintf(g, "<td>%s</td>", tmpPriemnik->nameReceiver);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->typeReceiver);

                    fprintf(g, "<td>%s</td>", tmpPriemnik->description);
                    fprintf(g, "<td>%s</td>", tmpPriemnik->note);

                    fprintf(g, "</tr>");

                    tmpPriemnik = tmpPriemnik->next;
                }
            }
            print_doc_end(g);
            if (notEmpty) {
                tmpPriemnik = priemnik.next;
                priemnik.next = nullptr;
                while (tmpPriemnik != nullptr)
                {
                    LiftingEqipment* forDelete = tmpPriemnik;
                    tmpPriemnik = tmpPriemnik->next;
                    forDelete->next = nullptr;
                    delete forDelete;
                }
            }
        }
    }
}