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
void printHeatPoint(CString tn, FILE* g, FILE* f, CAdoFile* mAdo, CGraph2* graph, CString argPath, bool isFull);

void initPassport(CAdoFile* ado, CGraph2* graph, HWND CWnd, CString argPath)
{
    CString tn;
    bool res = false;
    res = create_vyd_line_table(ado, graph, tn);

    CWaitCursor wait;

    CString tmpName;

    tmpName.Format("%s\\index_heat_point.html", getenv("TMP"));


    FILE* f = fopen(tmpName, "w");
    if (!f) {
        AfxMessageBox("Закройте предыдущий паспорт");
        return;
    }

    setlocale(LC_NUMERIC, "");

    print_index1(f, "");

    FILE* g = 0;

    /*Тепловые пункты*/
    printHeatPoint(tn, g, f, ado, graph, argPath, !res);
    /*Потребители*/
    //printDistrictSite(g, f, m_cxema->m_ado);


    print_index2(f);
    fclose(f);
    setlocale(LC_NUMERIC, "eng");

    tmpName.Format("%s\\index_heat_point.html", getenv("TMP"));

    CString cssName;
    cssName.Format("%s\\style.css", getenv("TMP"));
    css.print(cssName);


    HINSTANCE hRun = ShellExecute(CWnd, "open", "excel", "\"" + tmpName + "\"", NULL, SW_SHOW);

}

struct HeatPoint { /*Тепловые пункты*/
    int id;
    CString name, type,
        code, nameNode, attach;
    double r1, r2, r3, r4;
    int countCompany, countUser;
    double s1, v1, v2, v3;
    HeatPoint* next;
};
void printHeatPointRealConsumer(FILE* g, FILE* f, CAdoFile* mAdo, CGraph2* graph, std::string streamIds, bool heatPointNotEmpty, CString argPath);
void printHeatPoint(CString tn, FILE* g, FILE* file, CAdoFile* mAdo, CGraph2* graph, CString argPath, bool isFull)
{
    HeatPoint heatPoint; 
    HeatPoint* tmpHeatPoint = &heatPoint;
    
    g = print_doc(file, "heatPoint.html", _TR("Тепловой пункт (ТП)"));
    if (g) {
        CString q, fStr;
        std::stringstream stream;
        CString qName;
        if (isFull)
            qName.Format("heatPointPassportFull");
        else
            qName.Format("heatPointPassport");
        fStr.Format("%ssql\\objects\\%s.sql", argPath, qName);
        ifstream f(fStr);
        if (f.good())
        {
            q = readFile(fStr);
            q.Replace("myTableName", tn);

            bool ret = mAdo->openTable0(q);
            if (!ret)
            {
                AfxMessageBox("Ошибка в запросе при формировании паспорта (Тепловые пункты)");
                return;
            }

            bool heatPointNotEmpty = false;
            while (!mAdo->isEOF()) {
                tmpHeatPoint->id = mAdo->read_long("id");

                tmpHeatPoint->name = mAdo->readStr("name");
                tmpHeatPoint->type = mAdo->readStr("type");

                //tmpHeatPoint->code;
                //tmpHeatPoint->nameNode;
                //tmpHeatPoint->attach;

                tmpHeatPoint->r1 = mAdo->read_double("r1");
                tmpHeatPoint->r2 = mAdo->read_double("r2");
                tmpHeatPoint->r3 = mAdo->read_double("r3");
                tmpHeatPoint->r4 = mAdo->read_double("r4");

                tmpHeatPoint->countUser = mAdo->read_long("countUser");
                tmpHeatPoint->countCompany = mAdo->read_long("countCompany");

                tmpHeatPoint->s1 = mAdo->read_double("s1");
                tmpHeatPoint->v1 = mAdo->read_double("v1");
                tmpHeatPoint->v2 = mAdo->read_double("v2");
                tmpHeatPoint->v3 = mAdo->read_double("v3");

                mAdo->MoveNext();
                if (mAdo->isEOF())
                {
                    tmpHeatPoint->next = nullptr;
                    stream << tmpHeatPoint->id;
                }

                else
                {
                    tmpHeatPoint->next = new struct HeatPoint;
                    stream << tmpHeatPoint->id << ", ";
                }
                tmpHeatPoint = tmpHeatPoint->next;
                if (!heatPointNotEmpty)
                    heatPointNotEmpty = true;
            }



            tmpHeatPoint = &heatPoint;  
            if (heatPointNotEmpty)
            {
                while (tmpHeatPoint != nullptr) {
                    fprintf(g, "<tr>");

                    fprintf(g, "<td>%s</td>", tmpHeatPoint->name);
                    fprintf(g, "<td>%s</td>", tmpHeatPoint->type);

                    fprintf(g, "<td>%s</td>", tmpHeatPoint->code);
                    fprintf(g, "<td>%s</td>", tmpHeatPoint->nameNode);
                    fprintf(g, "<td>%s</td>", tmpHeatPoint->attach);

                    fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpHeatPoint->r1);
                    fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpHeatPoint->r2);
                    fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpHeatPoint->r3);
                    fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpHeatPoint->r4);

                    fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%d</td>", tmpHeatPoint->countCompany);
                    fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%d</td>", tmpHeatPoint->countUser);

                    fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpHeatPoint->s1);
                    fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpHeatPoint->v1);
                    fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpHeatPoint->v2);
                    fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpHeatPoint->v3);

                    fprintf(g, "</tr>");

                    tmpHeatPoint = tmpHeatPoint->next;


                }
            }

            print_doc_end(g);
            printHeatPointRealConsumer(g, file, mAdo, graph, stream.str(), heatPointNotEmpty, argPath);

            if (heatPointNotEmpty) {
                tmpHeatPoint = heatPoint.next;
                heatPoint.next = nullptr;
                while (tmpHeatPoint != nullptr)
                {
                    HeatPoint* forDelete = tmpHeatPoint;
                    tmpHeatPoint = tmpHeatPoint->next;
                    forDelete->next = nullptr;
                    delete forDelete;
                }
            }


        }

    }



}

void printHeatPointRealConsumer(FILE* g, FILE* f, CAdoFile* mAdo, CGraph2* graph, std::string myIds, bool heatPointNotEmpty, CString argPath)
{
    struct RealConsumer {
        CString name, type, fullName,
            code, nameNode, attach, numberContract,
            meterAvailability, street, numberHouse;
        int countFloor;
        CString dateInput;
        int countCompany, countUser;
        double s1, v1, v2, v3;
        double r1, r2, r3, r4;
        CString note1, note2;
        RealConsumer* next;
    };
    RealConsumer realConsumer; 
    RealConsumer* tmpRealConsumer = &realConsumer; 
    
    g = print_doc(f, "heatPointRealConsumers.html", _TR("Потребители ТП"));
    if (g) {
        bool realConsumerNotEmpty = false;
        if (heatPointNotEmpty)
        {
            CString q, table, fStr;
            fStr.Format("%ssql\\objects\\%s.sql", argPath, "getRealConsumersByHeatPoint");
            CString ss;

            ss.Format("fileID in (%s)", graph->m_cxema->getParAll());
            fStr.Replace("fileID != 122", ss);
            ifstream f(fStr);
            if (f.good())
            {
                q = readFile(fStr);
                CString idName;
                idName.Format("(%s)", myIds.c_str());
                q.Replace("myId", idName);
            }

            bool ret = mAdo->openTable0(q);
            if (!ret)
            {
                AfxMessageBox("Ошибка в запросе при формировании паспорта (Потребители (ТП))");
                return;
            }
            while (!mAdo->isEOF()) {
                tmpRealConsumer->name = mAdo->readStr("hpName");
                tmpRealConsumer->type = mAdo->readStr("hpType");
                tmpRealConsumer->fullName = mAdo->readStr("rcName");

                tmpRealConsumer->code = mAdo->readStr("ecName");
                tmpRealConsumer->nameNode = mAdo->readStr("externalNodeName");
                tmpRealConsumer->attach = mAdo->readStr("hsName");

                tmpRealConsumer->numberContract = mAdo->readStr("contractNumber");
                tmpRealConsumer->meterAvailability = mAdo->readStr("meter");

                tmpRealConsumer->street = mAdo->readStr("streetName");
                tmpRealConsumer->numberHouse = mAdo->readStr("houseNumber");

                tmpRealConsumer->countFloor = mAdo->read_double("countFloors");
                tmpRealConsumer->dateInput = mAdo->readStr("PICdate");

                tmpRealConsumer->countCompany = mAdo->read_double("countBusinessconsumers");
                tmpRealConsumer->countUser = mAdo->read_double("countUserGV");

                tmpRealConsumer->s1 = mAdo->read_double("area");
                tmpRealConsumer->v1 = mAdo->read_double("buildingVolume");
                tmpRealConsumer->v2 = mAdo->read_double("basementVolume");
                tmpRealConsumer->v3 = mAdo->read_double("builtInVolume");
                tmpRealConsumer->r1 = mAdo->read_double("calcHL");
                tmpRealConsumer->r2 = mAdo->read_double("avgHLGVS");
                tmpRealConsumer->r3 = mAdo->read_double("maxGV");
                tmpRealConsumer->r4 = mAdo->read_double("calcHLventil");

                tmpRealConsumer->note1 = mAdo->readStr("note_1");
                tmpRealConsumer->note2 = mAdo->readStr("note_2");
                mAdo->MoveNext();

                if (mAdo->isEOF())
                {
                    tmpRealConsumer->next = nullptr;
                    
                }

                else
                {
                    tmpRealConsumer->next = new struct RealConsumer;
                }

                tmpRealConsumer = tmpRealConsumer->next;
                if (!realConsumerNotEmpty)
                    realConsumerNotEmpty = true;
            }
        }


        tmpRealConsumer = &realConsumer;
        if (realConsumerNotEmpty)
        {
            while (tmpRealConsumer != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpRealConsumer->name);
                fprintf(g, "<td>%s</td>", tmpRealConsumer->type);
                fprintf(g, "<td>%s</td>", tmpRealConsumer->fullName);

                fprintf(g, "<td>%s</td>", tmpRealConsumer->code);
                fprintf(g, "<td>%s</td>", tmpRealConsumer->nameNode);
                fprintf(g, "<td>%s</td>", tmpRealConsumer->attach);

                fprintf(g, "<td>%s</td>", tmpRealConsumer->numberContract);
                fprintf(g, "<td>%s</td>", tmpRealConsumer->meterAvailability);
                fprintf(g, "<td>%s</td>", tmpRealConsumer->street);
                fprintf(g, "<td>%s</td>", tmpRealConsumer->numberHouse);

                fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%d</td>", tmpRealConsumer->countFloor);
                fprintf(g, "<td>%s</td>", tmpRealConsumer->dateInput);

                fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%d</td>", tmpRealConsumer->countCompany);
                fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%d</td>", tmpRealConsumer->countUser);

                fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpRealConsumer->s1);
                fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpRealConsumer->v1);
                fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpRealConsumer->v2);
                fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpRealConsumer->v3);

                fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpRealConsumer->r1);
                fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpRealConsumer->r2);
                fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpRealConsumer->r3);
                fprintf(g, "<td style='text-align: center; vertical-align: middle;'>%f</td>", tmpRealConsumer->r4);

                fprintf(g, "<td>%s</td>", tmpRealConsumer->note1);
                fprintf(g, "<td>%s</td>", tmpRealConsumer->note2);

                fprintf(g, "</tr>");
                //HeatPoint** forDelete = &tmpHeatPoint;
                tmpRealConsumer = tmpRealConsumer->next;
                //delete forDelete;

            }
        }

        print_doc_end(g);

        if (realConsumerNotEmpty) {
            tmpRealConsumer = realConsumer.next;
            realConsumer.next = nullptr;
            while (tmpRealConsumer != nullptr)
            {
                RealConsumer* forDelete = tmpRealConsumer;
                tmpRealConsumer = tmpRealConsumer->next;
                forDelete->next = nullptr;
                delete forDelete;
            }
        }


    }



}