#include "stdafx.h"
#include "ado.h"
#include "win.h"
#include <fstream>
#include <map>

FILE* print_doc(FILE* fi, const char* fn, const char* title, void (*filtr) (CString& s1) = NULL);
void print_doc_end(FILE* f);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
void printTd(CAdoFile* ado, FILE* f, const char* param, const int type, int count = 1);

bool checkFileOpen(CString doc_name) {
    FILE* fOld = fopen(doc_name, "w");
    if (!fOld) {
        AfxMessageBox("Закройте предыдущий паспорт", MB_OK | MB_ICONINFORMATION);
        return false;
    }
    fclose(fOld);
    return true;
}
/*механическое оборудование запорная арматура*/
void printMechanicalZa(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{
    struct Params {
        CString type, design, material, construction, purpose;
    };
    struct Values {
        float diametr;
        int count = 0;
    };
    struct MechEqZA { /*Механическое оборудование запорная арматура*/
        CString beginPts, endPts, pavilion;
        Params paramsP, paramsO;
        Values valuesDrP, valuesZaSP, valuesZaRP, valuesVozP, valuesDTP, valuesPrmP;
        Values valuesDrO, valuesZaSO, valuesZaRO, valuesVozO, valuesDTO, valuesPrmO;
        MechEqZA* next;
        int fill = 0; /*old 32 DrP, 16 DrO, 8 ZaP, 4 ZaO, 2 VozP, 1 VozO (63 все заполнено, 42 подача заполнена, 21 обратка заполнена) old*/
         /*
         2048 DrP, 1024 DrO,
         512 ZaSP, 256 ZaSO,
         128 ZaRP, 64 ZaRO,
         32 VozP, 16 VozO,
         8 valueDTP, 4 valueDTO,
         2 valuePrmP, 1 valuePrmO
         (4095 все заполнено, 2730 подача заполнена, 1365 обратка заполнена)*/
    };

    auto checkParams = [](Params a, Params b) {
        if (strcmp(a.construction, b.construction) == 0 &&
            strcmp(a.design, b.design) == 0 &&
            strcmp(a.material, b.material) == 0 &&
            strcmp(a.purpose, b.purpose) == 0 &&
            strcmp(a.type, b.type) == 0)
            return true;
        return false;
    };

    auto writeObject = [](int externalID, CString tn, CAdoFile* mAdo, MechEqZA* mechEqZA) {
        if (strcmp(tn, "drenazhnyy_kran") == 0) {
            if (externalID == 2)
            {
                mechEqZA->valuesDrP.diametr = mAdo->read_double(8);
                mechEqZA->valuesDrP.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 2048;
            }
            else {
                mechEqZA->valuesDrO.diametr = mAdo->read_double(8);
                mechEqZA->valuesDrO.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 1024;
            }
        }
        else if (strcmp(tn, "zapornaya_armatura_s") == 0) {
            if (externalID == 2)
            {
                mechEqZA->valuesZaSP.diametr = mAdo->read_double(8);
                mechEqZA->valuesZaSP.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 512;
            }
            else {
                mechEqZA->valuesZaSO.diametr = mAdo->read_double(8);
                mechEqZA->valuesZaSO.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 256;
            }
        }
        else if (strcmp(tn, "zapornaya_armatura_r") == 0) {
            if (externalID == 2)
            {
                mechEqZA->valuesZaRP.diametr = mAdo->read_double(8);
                mechEqZA->valuesZaRP.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 128;
            }
            else {
                mechEqZA->valuesZaRO.diametr = mAdo->read_double(8);
                mechEqZA->valuesZaRO.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 64;
            }
        }
        else if (strcmp(tn, "vozdushnik") == 0) {
            if (externalID == 2)
            {
                mechEqZA->valuesVozP.diametr = mAdo->read_double(8);
                mechEqZA->valuesVozP.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 32;
            }
            else {
                mechEqZA->valuesVozO.diametr = mAdo->read_double(8);
                mechEqZA->valuesVozO.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 16;
            }
        }

        else if (strcmp(tn, "drenazhnyy_truboprovod") == 0) {
            if (externalID == 2)
            {
                mechEqZA->valuesDTP.diametr = mAdo->read_double(8);
                mechEqZA->valuesDTP.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 8;
            }
            else {
                mechEqZA->valuesDTO.diametr = mAdo->read_double(8);
                mechEqZA->valuesDTO.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 4;
            }
        }
        else if (strcmp(tn, "peremychki") == 0) {
            if (externalID == 2)
            {
                mechEqZA->valuesPrmP.diametr = mAdo->read_double(8);
                mechEqZA->valuesPrmP.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 2;
            }
            else {
                mechEqZA->valuesPrmO.diametr = mAdo->read_double(8);
                mechEqZA->valuesPrmO.count = mAdo->read_long(9);
                mechEqZA->fill = mechEqZA->fill | 1;
            }
        }
    };

    MechEqZA mechEqZA; /*Создаем структуру*/
    MechEqZA* tmpMech = &mechEqZA; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f2_1_mechanical_equipment.html", _TR("Ф2_1.Механическое оборудование"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from getPts(%d,'%s','%s') order by orderID", id, table, fragments);
        //q.Format("select * from getPts(%d,'%s') order by orderID", 96, "ms");
        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Механическое оборудование)");
            return;
        }

        bool isGeneral = false;
        while (!mAdo->isEOF()) {
            tmpMech->beginPts = mAdo->readStr(1);
            tmpMech->endPts = mAdo->readStr(2);
            tmpMech->pavilion = mAdo->readStr("pavilion");
            int externalId = mAdo->read_long(3);
            int externalIdObj = mAdo->read_long("externalID2");
            if (externalIdObj != 0)
                externalId = externalIdObj;
            if (externalId == 1) {
                if (!isGeneral)
                {
                    isGeneral = true;
                    externalId = 2;
                }
                else
                {
                    isGeneral = false;
                    externalId = 3;
                }
            }
            CString tn = mAdo->readStr(0);
            CString purposeType = mAdo->readStr("purposeType");
            if (strcmp(tn, "zapornaya_armatura") == 0) {
                if (strcmp(purposeType, "Секционирующая") == 0)
                    tn = "zapornaya_armatura_s";
                else
                    tn = "zapornaya_armatura_r";
            }
            switch (externalId)
            {
            case 2:
                tmpMech->paramsP.type = mAdo->readStr(4);
                tmpMech->paramsP.design = mAdo->readStr(5);
                tmpMech->paramsP.construction = mAdo->readStr(6);
                tmpMech->paramsP.material = mAdo->readStr(7);
                tmpMech->paramsP.purpose = purposeType;
                writeObject(2, tn, mAdo, tmpMech);
                break;
            case 3:
                tmpMech->paramsO.type = mAdo->readStr(4);
                tmpMech->paramsO.design = mAdo->readStr(5);
                tmpMech->paramsO.construction = mAdo->readStr(6);
                tmpMech->paramsO.material = mAdo->readStr(7);
                tmpMech->paramsO.purpose = purposeType;
                writeObject(3, tn, mAdo, tmpMech);
                break;
            }

            if (!isGeneral)
                mAdo->MoveNext();

            if (mAdo->isEOF())
                tmpMech->next = nullptr;
            else
                tmpMech->next = new struct MechEqZA;
            tmpMech = tmpMech->next;
        }

        tmpMech = &mechEqZA;  /*Приравниваем указатель в начало структуры*/
        /*пока не заполнится последняя структура*/
        if (tmpMech->fill != 0)
            while (tmpMech != nullptr) {
                MechEqZA* next = tmpMech->next;
                MechEqZA* prev = tmpMech;

                while (!(next == nullptr)) {
                    bool changePrev = false;
                    /*проверка заполнена ли подача и обратка, и в случае если что то не заполнено и совпадают участки, заполняем, иначе проверяем заполнения объектов (при заполнении смещать структуру)*/
                    /*если подачи нет и следующий элемент это подача с совпадением данных заполняем*/
                    if ((tmpMech->fill & 2730) == 0
                        && (next->fill & 2730) != 0
                        && ((tmpMech->fill << 1) == next->fill)  /*сдвигаем fill влево так обратка правее подачи для уравнения их*/
                        && strcmp(tmpMech->beginPts, next->beginPts) == 0
                        && strcmp(tmpMech->endPts, next->endPts) == 0
                        && strcmp(tmpMech->pavilion, next->pavilion) == 0) {
                        tmpMech->paramsP.construction = next->paramsP.construction;
                        tmpMech->paramsP.design = next->paramsP.design;
                        tmpMech->paramsP.material = next->paramsP.material;
                        tmpMech->paramsP.type = next->paramsP.type;
                        tmpMech->paramsP.purpose = next->paramsP.purpose;
                        changePrev = true;
                        if ((next->fill & 2048) != 0) {
                            /*Добавляем дренажный кран подачу*/
                            tmpMech->valuesDrP.count = next->valuesDrP.count;
                            tmpMech->valuesDrP.diametr = next->valuesDrP.diametr;
                            tmpMech->fill = tmpMech->fill | 2048;
                        }
                        else
                            if ((next->fill & 512) != 0) {
                                /*Добавляем запорную арматуру секционирующую подачу*/
                                tmpMech->valuesZaSP.count = next->valuesZaSP.count;
                                tmpMech->valuesZaSP.diametr = next->valuesZaSP.diametr;
                                tmpMech->fill = tmpMech->fill | 512;
                            }
                            else
                                if ((next->fill & 128) != 0) {
                                    /*Добавляем запорную арматуру регулирующую подачу*/
                                    tmpMech->valuesZaRP.count = next->valuesZaRP.count;
                                    tmpMech->valuesZaRP.diametr = next->valuesZaRP.diametr;
                                    tmpMech->fill = tmpMech->fill | 128;
                                }
                                else
                                    if ((next->fill & 32) != 0) {
                                        /*Добавляем воздушник подачу*/
                                        tmpMech->valuesVozP.count = next->valuesVozP.count;
                                        tmpMech->valuesVozP.diametr = next->valuesVozP.diametr;
                                        tmpMech->fill = tmpMech->fill | 32;

                                    }
                                    else
                                        if ((next->fill & 8) != 0) {
                                            /*Добавляем дренажный трубопровод подачу*/
                                            tmpMech->valuesDTP.count = next->valuesDTP.count;
                                            tmpMech->valuesDTP.diametr = next->valuesDTP.diametr;
                                            tmpMech->fill = tmpMech->fill | 8;

                                        }
                                        else
                                            if ((next->fill & 2) != 0) {
                                                /*Добавляем перемычку подачу*/
                                                tmpMech->valuesPrmP.count = next->valuesPrmP.count;
                                                tmpMech->valuesPrmP.diametr = next->valuesPrmP.diametr;
                                                tmpMech->fill = tmpMech->fill | 2;

                                            }
                    }
                    /*если обратки нет и следующий элемент это обратка с совпадением данных заполняем*/
                    else if ((tmpMech->fill & 1365) == 0
                        && (next->fill & 1365) != 0
                        && strcmp(tmpMech->beginPts, next->beginPts) == 0
                        && strcmp(tmpMech->endPts, next->endPts) == 0
                        && ((tmpMech->fill >> 1) == next->fill) /*сдвигаем fill вправо так подача левее обратки для уравнения их*/
                        && strcmp(tmpMech->pavilion, next->pavilion) == 0) {
                        tmpMech->paramsO.construction = next->paramsO.construction;
                        tmpMech->paramsO.design = next->paramsO.design;
                        tmpMech->paramsO.material = next->paramsO.material;
                        tmpMech->paramsO.type = next->paramsO.type;
                        tmpMech->paramsO.purpose = next->paramsO.purpose;
                        changePrev = true;
                        if ((next->fill & 1024) != 0) {
                            /*Добавляем дренажный кран обратку*/
                            tmpMech->valuesDrO.count = next->valuesDrO.count;
                            tmpMech->valuesDrO.diametr = next->valuesDrO.diametr;
                            tmpMech->fill = tmpMech->fill | 1024;
                        }
                        else
                            if ((next->fill & 256) != 0) {
                                /*Добавляем запорную арматуру секционирующую обратку*/
                                tmpMech->valuesZaSO.count = next->valuesZaSO.count;
                                tmpMech->valuesZaSO.diametr = next->valuesZaSO.diametr;
                                tmpMech->fill = tmpMech->fill | 256;
                            }
                            else
                                if ((next->fill & 64) != 0) {
                                    /*Добавляем запорную арматуру регулирующую обратку*/
                                    tmpMech->valuesZaRO.count = next->valuesZaRO.count;
                                    tmpMech->valuesZaRO.diametr = next->valuesZaRO.diametr;
                                    tmpMech->fill = tmpMech->fill | 64;
                                }
                                else
                                    if ((next->fill & 16) != 0) {
                                        /*Добавляем воздушник обратку*/
                                        tmpMech->valuesVozO.count = next->valuesVozO.count;
                                        tmpMech->valuesVozO.diametr = next->valuesVozO.diametr;
                                        tmpMech->fill = tmpMech->fill | 16;

                                    }
                                    else
                                        if ((next->fill & 4) != 0) {
                                            /*Добавляем дренажный трубопровод обратку*/
                                            tmpMech->valuesDTO.count = next->valuesDTO.count;
                                            tmpMech->valuesDTO.diametr = next->valuesDTO.diametr;
                                            tmpMech->fill = tmpMech->fill | 4;

                                        }
                                        else
                                            if ((next->fill & 1) != 0) {
                                                /*Добавляем перемычку обратку*/
                                                tmpMech->valuesPrmO.count = next->valuesPrmO.count;
                                                tmpMech->valuesPrmO.diametr = next->valuesPrmO.diametr;
                                                tmpMech->fill = tmpMech->fill | 1;

                                            }

                    }
                    /*иначе пытаемя найти совпадения и заполнить*/
                    else {
                        if (strcmp(tmpMech->beginPts, next->beginPts) == 0
                            && strcmp(tmpMech->endPts, next->endPts) == 0
                            && strcmp(tmpMech->pavilion, next->pavilion) == 0)
                            if ((next->fill & 2048) != 0 && checkParams(tmpMech->paramsP, next->paramsP)
                                && (tmpMech->fill & 2048) != 0 && next->valuesDrP.diametr == tmpMech->valuesDrP.diametr) {
                                /*Добавляем дренажный кран подачу*/
                                tmpMech->valuesDrP.count = tmpMech->valuesDrP.count + next->valuesDrP.count;
                                tmpMech->valuesDrP.diametr = next->valuesDrP.diametr;
                                tmpMech->fill = tmpMech->fill | 2048;
                                changePrev = true;
                            }
                            else
                                if ((next->fill & 512) != 0 && checkParams(tmpMech->paramsP, next->paramsP)
                                    && (tmpMech->fill & 512) != 0 && next->valuesZaSP.diametr == tmpMech->valuesZaSP.diametr) {
                                    /*Добавляем запорную арматуру секционирующую подачу*/
                                    tmpMech->valuesZaSP.count = tmpMech->valuesZaSP.count + next->valuesZaSP.count;
                                    tmpMech->valuesZaSP.diametr = next->valuesZaSP.diametr;
                                    tmpMech->fill = tmpMech->fill | 512;
                                    changePrev = true;
                                }
                                else
                                    if ((next->fill & 128) != 0 && checkParams(tmpMech->paramsP, next->paramsP)
                                        && (tmpMech->fill & 128) != 0 && next->valuesZaRP.diametr == tmpMech->valuesZaRP.diametr) {
                                        /*Добавляем запорную арматуру регулирующую подачу*/
                                        tmpMech->valuesZaRP.count = tmpMech->valuesZaRP.count + next->valuesZaRP.count;
                                        tmpMech->valuesZaRP.diametr = next->valuesZaRP.diametr;
                                        tmpMech->fill = tmpMech->fill | 128;
                                        changePrev = true;
                                    }
                                    else
                                        if ((next->fill & 32) != 0 && checkParams(tmpMech->paramsP, next->paramsP)
                                            && (tmpMech->fill & 32) != 0 && next->valuesVozP.diametr == tmpMech->valuesVozP.diametr) {
                                            /*Добавляем воздушник подачу*/
                                            tmpMech->valuesVozP.count = tmpMech->valuesVozP.count + next->valuesVozP.count;
                                            tmpMech->valuesVozP.diametr = next->valuesVozP.diametr;
                                            tmpMech->fill = tmpMech->fill | 32;
                                            changePrev = true;
                                        }
                                        else
                                            if ((next->fill & 8) != 0 && checkParams(tmpMech->paramsP, next->paramsP)
                                                && (tmpMech->fill & 8) != 0 && next->valuesDTP.diametr == tmpMech->valuesDTP.diametr) {
                                                /*Добавляем дренажный трубопровод подачу*/
                                                tmpMech->valuesDTP.count = tmpMech->valuesDTP.count + next->valuesDTP.count;
                                                tmpMech->valuesDTP.diametr = next->valuesDTP.diametr;
                                                tmpMech->fill = tmpMech->fill | 8;
                                                changePrev = true;
                                            }
                                            else
                                                if ((next->fill & 2) != 0 && checkParams(tmpMech->paramsP, next->paramsP)
                                                    && (tmpMech->fill & 2) != 0 && next->valuesPrmP.diametr == tmpMech->valuesPrmP.diametr) {
                                                    /*Добавляем перемычку подачу*/
                                                    tmpMech->valuesPrmP.count = tmpMech->valuesPrmP.count + next->valuesPrmP.count;
                                                    tmpMech->valuesPrmP.diametr = next->valuesPrmP.diametr;
                                                    tmpMech->fill = tmpMech->fill | 2;
                                                    changePrev = true;
                                                }
                                                else
                                                    if ((next->fill & 1024) != 0 && checkParams(tmpMech->paramsO, next->paramsO)
                                                        && (tmpMech->fill & 1024) != 0 && next->valuesDrO.diametr == tmpMech->valuesDrO.diametr) {
                                                        /*Добавляем дренажный кран обратку*/
                                                        tmpMech->valuesDrO.count = tmpMech->valuesDrO.count + next->valuesDrO.count;
                                                        tmpMech->valuesDrO.diametr = next->valuesDrO.diametr;
                                                        tmpMech->fill = tmpMech->fill | 1024;
                                                        changePrev = true;
                                                    }
                                                    else
                                                        if ((next->fill & 256) != 0 && checkParams(tmpMech->paramsO, next->paramsO)
                                                            && (tmpMech->fill & 256) != 0 && next->valuesZaSO.diametr == tmpMech->valuesZaSO.diametr) {
                                                            /*Добавляем запорную арматуру секционирующую обратку*/
                                                            tmpMech->valuesZaSO.count = tmpMech->valuesZaSO.count + next->valuesZaSO.count;
                                                            tmpMech->valuesZaSO.diametr = next->valuesZaSO.diametr;
                                                            tmpMech->fill = tmpMech->fill | 256;
                                                            changePrev = true;
                                                        }
                                                        else
                                                            if ((next->fill & 64) != 0 && checkParams(tmpMech->paramsO, next->paramsO)
                                                                && (tmpMech->fill & 64) != 0 && next->valuesZaRO.diametr == tmpMech->valuesZaRO.diametr) {
                                                                /*Добавляем запорную арматуру регулирующую обратку*/
                                                                tmpMech->valuesZaRO.count = tmpMech->valuesZaRO.count + next->valuesZaRO.count;
                                                                tmpMech->valuesZaRO.diametr = next->valuesZaRO.diametr;
                                                                tmpMech->fill = tmpMech->fill | 64;
                                                                changePrev = true;
                                                            }
                                                            else
                                                                if ((next->fill & 16) != 0 && checkParams(tmpMech->paramsO, next->paramsO)
                                                                    && (tmpMech->fill & 16) != 0 && next->valuesVozO.diametr == tmpMech->valuesVozO.diametr) {
                                                                    /*Добавляем воздушник обратку*/
                                                                    tmpMech->valuesVozO.count = tmpMech->valuesVozO.count + next->valuesVozO.count;
                                                                    tmpMech->valuesVozO.diametr = next->valuesVozO.diametr;
                                                                    tmpMech->fill = tmpMech->fill | 16;
                                                                    changePrev = true;
                                                                }
                                                                else
                                                                    if ((next->fill & 4) != 0 && checkParams(tmpMech->paramsO, next->paramsO)
                                                                        && (tmpMech->fill & 4) != 0 && next->valuesDTO.diametr == tmpMech->valuesDTO.diametr) {
                                                                        /*Добавляем дренажный трубопровод обратку*/
                                                                        tmpMech->valuesDTO.count = tmpMech->valuesDTO.count + next->valuesDTO.count;
                                                                        tmpMech->valuesDTO.diametr = next->valuesDTO.diametr;
                                                                        tmpMech->fill = tmpMech->fill | 4;
                                                                        changePrev = true;
                                                                    }
                                                                    else
                                                                        if ((next->fill & 1) != 0 && checkParams(tmpMech->paramsO, next->paramsO)
                                                                            && (tmpMech->fill & 1) != 0 && next->valuesPrmO.diametr == tmpMech->valuesPrmO.diametr) {
                                                                            /*Добавляем перемычку обратку*/
                                                                            tmpMech->valuesPrmO.count = tmpMech->valuesPrmO.count + next->valuesPrmO.count;
                                                                            tmpMech->valuesPrmO.diametr = next->valuesPrmO.diametr;
                                                                            tmpMech->fill = tmpMech->fill | 1;
                                                                            changePrev = true;
                                                                        }
                    }
                    if (changePrev)
                        prev->next = next->next;
                    else
                    {
                        //prev->next = next;/*проверить без нее*/
                        prev = next;
                    }
                    next = next->next;
                }
                tmpMech = tmpMech->next;
            }

        /*сортировка начало*/
        /*переходим в начало*/
        tmpMech = &mechEqZA;
        int countObj = 0, current = 0;

        /* ищем длину структуры*/
        if (tmpMech->fill != 0)
            while (tmpMech != nullptr) {
                countObj++;
                tmpMech = tmpMech->next;
            }
        /*переходим в начало*/
        tmpMech = &mechEqZA;
        struct SortedMechEqZA {
            MechEqZA* data;
        };
        MechEqZA** sortedMech = new MechEqZA * [countObj];

        if (tmpMech->fill != 0)
        {
            /*проверяем соответствует ли объект одному из типов*/
            auto checkObject = [](int number, int fill) {

                switch (number)
                {
                case 1:
                    return (fill & 128) != 0 || (fill & 64) != 0;
                case 2:
                    return  (fill & 512) != 0 || (fill & 256) != 0;
                case 3:
                    return  (fill & 32) != 0 || (fill & 16) != 0;
                case 4:
                    return  (fill & 2048) != 0 || (fill & 1024) != 0;
                case 5:
                    return  (fill & 8) != 0 || (fill & 4) != 0;
                case 6:
                    return  (fill & 2) != 0 || (fill & 1) != 0;
                default:
                    return false;
                }

            };
            /*Обозначаем начальную группу (начальный и конечный узел)*/
            CString beginPts = tmpMech->beginPts, endPts = tmpMech->endPts;
            MechEqZA* beginGroup = tmpMech;
            while (tmpMech != nullptr) {
                /*формируем массив с последовательным порядком объектов*/
                for (int i = 1; i <= 6; i++) {
                    while (tmpMech != nullptr && strcmp(tmpMech->beginPts, beginPts) == 0 && strcmp(tmpMech->endPts, endPts) == 0)
                    {
                        if (checkObject(i, tmpMech->fill))
                        {
                            sortedMech[current] = tmpMech;
                            current++;
                        }
                        tmpMech = tmpMech->next;
                    }
                    tmpMech = beginGroup;
                }
                do {
                    tmpMech = tmpMech->next;
                } while (tmpMech != nullptr && strcmp(tmpMech->beginPts, beginPts) == 0 && strcmp(tmpMech->endPts, endPts) == 0);
                if (tmpMech != nullptr) {
                    /*меняем группу на следующую*/
                    beginGroup = tmpMech;
                    beginPts = tmpMech->beginPts;
                    endPts = tmpMech->endPts;
                }

            }
        }
        /*сортировка конец*/

        //tmpMech = &mechEqZA;  /*Приравниваем указатель в начало структуры*/

        for (int i = 0; i < countObj; i++) {
            tmpMech = sortedMech[i];
            fprintf(g, "<tr>");
            fprintf(g, "<td>%s</td>", tmpMech->beginPts);/*Начальный ПТС*/
            fprintf(g, "<td>%s</td>", tmpMech->endPts);/*Конечный ПТС*/

            fprintf(g, "<td>%s</td>", tmpMech->pavilion); /*Камера/павильон*/

            /*Подающий трубопровод*/
            if ((tmpMech->fill & 2730) != 0) {
                fprintf(g, "<td>%s</td>", tmpMech->paramsP.purpose); /*Назначение*/
                fprintf(g, "<td>%s</td>", tmpMech->paramsP.type); /*Тип*/
                fprintf(g, "<td>%s</td>", tmpMech->paramsP.design); /*Исполнение*/
                fprintf(g, "<td>%s</td>", tmpMech->paramsP.material); /*Материал*/
                fprintf(g, "<td>%s</td>", tmpMech->paramsP.construction); /*Конструкция*/
            }
            else
            {
                printTd(mAdo, g, "", 0, 5);
            }

            /*Обратный трубопровод*/
            if ((tmpMech->fill & 1365) != 0) {
                fprintf(g, "<td>%s</td>", tmpMech->paramsO.purpose); /*Назначение*/
                fprintf(g, "<td>%s</td>", tmpMech->paramsO.type); /*Тип*/
                fprintf(g, "<td>%s</td>", tmpMech->paramsO.design); /*Исполнение*/
                fprintf(g, "<td>%s</td>", tmpMech->paramsO.material); /*Материал*/
                fprintf(g, "<td>%s</td>", tmpMech->paramsO.construction); /*Конструкция*/
            }
            else
            {
                printTd(mAdo, g, "", 0, 5);
            }

            /*Запорная арматура результирующая*/
            if ((tmpMech->fill & 128) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesZaRP.diametr); /*Диаметр подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 64) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesZaRO.diametr); /*Диаметр обратка*/
            else
                fprintf(g, "<td></td>");

            if ((tmpMech->fill & 128) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesZaRP.count); /*Количество подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 64) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesZaRO.count); /*Количество обратка*/
            else
                fprintf(g, "<td></td>");

            /*Запорная арматура секционирующая*/
            if ((tmpMech->fill & 512) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesZaSP.diametr); /*Диаметр подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 256) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesZaSO.diametr); /*Диаметр обратка*/
            else
                fprintf(g, "<td></td>");

            if ((tmpMech->fill & 512) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesZaSP.count); /*Количество подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 256) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesZaSO.count); /*Количество обратка*/
            else
                fprintf(g, "<td></td>");

            /*Воздушники*/
            if ((tmpMech->fill & 32) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesVozP.diametr); /*Диаметр подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 16) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesVozO.diametr); /*Диаметр обратка*/
            else
                fprintf(g, "<td></td>");

            if ((tmpMech->fill & 32) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesVozP.count); /*Количество подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 16) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesVozO.count); /*Количество обратка*/
            else
                fprintf(g, "<td></td>");

            /*Дренажный кран*/
            if ((tmpMech->fill & 2048) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesDrP.diametr); /*Диаметр подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 1024) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesDrO.diametr); /*Диаметр обратка*/
            else
                fprintf(g, "<td></td>");

            if ((tmpMech->fill & 2048) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesDrP.count); /*Количество подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 1024) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesDrO.count); /*Количество обратка*/
            else
                fprintf(g, "<td></td>");

            /*Дренажный трубопровод*/
            if ((tmpMech->fill & 8) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesDTP.diametr); /*Диаметр подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 4) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesDTO.diametr); /*Диаметр обратка*/
            else
                fprintf(g, "<td></td>");

            if ((tmpMech->fill & 8) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesDTP.count); /*Количество подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 4) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesDTO.count); /*Количество обратка*/
            else
                fprintf(g, "<td></td>");

            /*Перемычка*/
            if ((tmpMech->fill & 2) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesPrmP.diametr); /*Диаметр подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 1) != 0)
                fprintf(g, "<td>%f</td>", tmpMech->valuesPrmO.diametr); /*Диаметр обратка*/
            else
                fprintf(g, "<td></td>");

            if ((tmpMech->fill & 2) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesPrmP.count); /*Количество подача*/
            else
                fprintf(g, "<td></td>");
            if ((tmpMech->fill & 1) != 0)
                fprintf(g, "<td>%d</td>", tmpMech->valuesPrmO.count); /*Количество обратка*/
            else
                fprintf(g, "<td></td>");

            fprintf(g, "</tr>");

        }
        delete[] sortedMech;
        print_doc_end(g);

    }

}

void printTubes(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{
    struct CountLenght {
        double lenghtP = 0.0, lenghtO = 0.0;
    };
    /*итоговая длина подачи*/
    double lenghtP = 0.0;
    /*итоговая длина обратки*/
    double lenghtO = 0.0;
    /*Хранит итоговую длину для конкретного диаметра*/
    map<double, CountLenght> mapDiametrLenght;
    struct Params {
        CString typeTube, typeTubing;
    };
    struct Values {
        double diametr = 0.0, volume = 0.0, lenght = 0.0;
        int thickness = 0;

    };
    struct Tube { /*Механическое оборудование запорная арматура*/
        CString beginPts, endPts;
        Params params;
        Values valueP, valueO;
        Tube* next;
        int fill = 0; /* 2 подача, 1 обратка, 3 все заполнено*/
    };

    auto checkParams = [](Params a, Params b) {
        if (strcmp(a.typeTube, b.typeTube) == 0 &&
            strcmp(a.typeTubing, b.typeTubing) == 0)
            return true;
        return false;
    };

    auto writeObject = [&](int externalID, CAdoFile* mAdo, Tube* tube) {
        double lenght = mAdo->read_double("l");
        double diametr = mAdo->read_double("d");
        if (externalID == 2)
        {
            tube->valueP.diametr = diametr;
            tube->valueP.volume = mAdo->read_double("v");
            tube->valueP.lenght = lenght;
            tube->valueP.thickness = mAdo->read_long("wall");
            tube->fill = tube->fill | 2;
            mapDiametrLenght[diametr].lenghtP += lenght;
            lenghtP = lenghtP + lenght;
        }
        else {
            tube->valueO.diametr = diametr;
            tube->valueO.volume = mAdo->read_double("v");
            tube->valueO.lenght = lenght;
            tube->valueO.thickness = mAdo->read_long("wall");
            tube->fill = tube->fill | 1;
            mapDiametrLenght[diametr].lenghtO += lenght;
            lenghtO = lenghtO + lenght;
        }

    };

    Tube tube; /*Создаем структуру*/
    Tube* tmpTube = &tube; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "a1-new.html", _TR("Ф1.Трубы"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from getTubesPts(%d,'%s','%s') order by orderID", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Трубы)");
            return;
        }

        bool isGeneral = false;
        while (!mAdo->isEOF()) {
            tmpTube->beginPts = mAdo->readStr("beginPts");
            tmpTube->endPts = mAdo->readStr("endPts");
            tmpTube->params.typeTube = mAdo->readStr("kind_tube");
            tmpTube->params.typeTubing = mAdo->readStr("type_tube");
            int externalId = mAdo->read_long("externalID");

            if (externalId == 1) {
                if (!isGeneral)
                {
                    isGeneral = true;
                    externalId = 2;
                }
                else
                {
                    isGeneral = false;
                    externalId = 3;
                }
            }
            switch (externalId)
            {
            case 2:
                writeObject(2, mAdo, tmpTube);
                break;
            case 3:
                writeObject(3, mAdo, tmpTube);
                break;
            }

            if (!isGeneral)
                mAdo->MoveNext();

            if (mAdo->isEOF())
                tmpTube->next = nullptr;
            else
                tmpTube->next = new struct Tube;
            tmpTube = tmpTube->next;
        }


        tmpTube = &tube;  /*Приравниваем указатель в начало структуры*/
        /*пока не заполнится последняя структура*/
        if (tmpTube->fill != 0)
            while (tmpTube != nullptr) {
                Tube* next = tmpTube->next;
                Tube* prev = tmpTube;

                while (!(next == nullptr)) {
                    bool changePrev = false;
                    /*проверка заполнена ли подача и обратка, и в случае если что то не заполнено и совпадают участки, заполняем*/
                    /*если подачи нет и следующий элемент это подача с совпадением данных заполняем*/
                    if ((tmpTube->fill & 2) == 0
                        && (next->fill & 2) != 0
                        && strcmp(tmpTube->beginPts, next->beginPts) == 0
                        && strcmp(tmpTube->endPts, next->endPts) == 0
                        && checkParams(tmpTube->params, next->params)) {
                        changePrev = true;
                        /*Добавляем подачу*/
                        tmpTube->valueP.diametr = next->valueP.diametr;
                        tmpTube->valueP.lenght = next->valueP.lenght;
                        tmpTube->valueP.volume = next->valueP.volume;
                        tmpTube->valueP.thickness = next->valueP.thickness;
                        tmpTube->fill = tmpTube->fill | 2;
                    }
                    /*если обратки нет и следующий элемент это обратка с совпадением данных заполняем*/
                    else if ((tmpTube->fill & 1) == 0
                        && (next->fill & 1) != 0
                        && strcmp(tmpTube->beginPts, next->beginPts) == 0
                        && strcmp(tmpTube->endPts, next->endPts) == 0
                        && checkParams(tmpTube->params, next->params)) {
                        changePrev = true;
                        /*Добавляем обратку*/
                        tmpTube->valueO.diametr = next->valueO.diametr;
                        tmpTube->valueO.lenght = next->valueO.lenght;
                        tmpTube->valueO.volume = next->valueO.volume;
                        tmpTube->valueO.thickness = next->valueO.thickness;
                        tmpTube->fill = tmpTube->fill | 1;

                    }


                    if (changePrev)
                        prev->next = next->next;
                    else
                        prev = next;

                    next = next->next;
                }
                tmpTube = tmpTube->next;
            }

        tmpTube = &tube;  /*Приравниваем указатель в начало структуры*/
        if (tmpTube->fill != 0)
        {
            while (tmpTube != nullptr) {
                fprintf(g, "<tr>");
                fprintf(g, "<td>%s</td>", tmpTube->beginPts);/*Начальный ПТС*/
                fprintf(g, "<td>%s</td>", tmpTube->endPts);/*Конечный ПТС*/

                /*Подающий труба*/
                if ((tmpTube->fill & 2) != 0) {
                    fprintf(g, "<td>%f</td>", tmpTube->valueP.diametr); /*Диаметр*/
                    fprintf(g, "<td>%g</td>", tmpTube->valueP.lenght); /*Длина*/
                }
                else
                {
                    printTd(mAdo, g, "", 0, 2);
                }

                /*Обратная труба*/
                if ((tmpTube->fill & 1) != 0) {
                    fprintf(g, "<td>%f</td>", tmpTube->valueO.diametr); /*Диаметр*/
                    fprintf(g, "<td>%g</td>", tmpTube->valueO.lenght); /*Длина*/
                }
                else
                {
                    printTd(mAdo, g, "", 0, 2);
                }

                /*Толщина стенки*/
                if ((tmpTube->fill & 2) != 0)
                    fprintf(g, "<td>%d</td>", tmpTube->valueP.thickness);
                else
                    printTd(mAdo, g, "", 0);
                if ((tmpTube->fill & 1) != 0)
                    fprintf(g, "<td>%d</td>", tmpTube->valueO.thickness);
                else
                    printTd(mAdo, g, "", 0);

                /*Объем трубы*/
                if ((tmpTube->fill & 2) != 0)
                    fprintf(g, "<td>%.2f</td>", tmpTube->valueP.volume);
                else
                    printTd(mAdo, g, "", 0);
                if ((tmpTube->fill & 1) != 0)
                    fprintf(g, "<td>%.2f</td>", tmpTube->valueO.volume);
                else
                    printTd(mAdo, g, "", 0);
                /*Тип прокладки*/
                fprintf(g, "<td>%s</td>", tmpTube->params.typeTubing);
                /*Вид трубы*/
                fprintf(g, "<td>%s</td>", tmpTube->params.typeTube);

                fprintf(g, "</tr>");

                tmpTube = tmpTube->next;

            }
            for (auto it = mapDiametrLenght.begin(); it != mapDiametrLenght.end(); ++it)
            {
                fprintf(g, "<tr>");
                fprintf(g, "<td>Итого</td>");
                printTd(mAdo, g, "", 0);
                fprintf(g, "<td>%f</td>", (*it).first); /*Диаметр*/
                fprintf(g, "<td>%g</td>", (*it).second.lenghtP); /*Общая длина*/
                fprintf(g, "<td>%f</td>", (*it).first); /*Диаметр*/
                fprintf(g, "<td>%g</td>", (*it).second.lenghtO); /*Общая длина*/
                printTd(mAdo, g, "", 0, 6);
                fprintf(g, "</tr>");

            }

            fprintf(g, "<tr>");
            fprintf(g, "<td>Всего</td>");
            printTd(mAdo, g, "", 0, 2);
            fprintf(g, "<td>%g</td>", lenghtP); /*Общая длина*/
            printTd(mAdo, g, "", 0);
            fprintf(g, "<td>%g</td>", lenghtO); /*Общая длина*/
            printTd(mAdo, g, "", 0, 6);
            fprintf(g, "</tr>");
        }

        print_doc_end(g);
    }

}


/*механическое оборудование компенсаторы колодцы*/
void printMechanicalKk(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{
    struct Kolodec {
        CString purpose, characteristic, materialL, constructionL, characteristicL;
        int count = 0;
    };
    struct Kompensator {
        float diametr;
        CString construction;
        int count = 0;
    };
    struct MechEqKK { /*Механическое оборудование компенсаторы и колодцы*/
        CString beginPts, endPts, pavilion;
        Kompensator kompensatorP, kompensatorO;
        Kolodec kolodecP, kolodecO, kolodecG;
        MechEqKK* next;
        int fill = 0; /*16 KompP, 8 KompO, 4 KolP, 2 KolO, 1 KolG (31 все заполнено)*/
    };

    auto checkParamsKompensator = [](Kompensator a, Kompensator b) {
        if (strcmp(a.construction, b.construction) == 0 &&
            a.diametr == b.diametr)
            return true;
        return false;
    };

    auto checkParamsKolodec = [](Kolodec a, Kolodec b) {
        if (strcmp(a.purpose, b.purpose) == 0 &&
            strcmp(a.characteristic, b.characteristic) == 0 &&
            strcmp(a.materialL, b.materialL) == 0 &&
            strcmp(a.constructionL, b.constructionL) == 0 &&
            strcmp(a.characteristicL, b.characteristicL) == 0)
            return true;
        return false;
    };

    auto writeObject = [](int externalID, CString tn, CAdoFile* mAdo, MechEqKK* mechEqKK) {
        if (strcmp(tn, "kompensator") == 0) {
            if (externalID == 2)
            {
                mechEqKK->kompensatorP.diametr = mAdo->read_double("diametr_truboprovoda");
                mechEqKK->kompensatorP.construction = mAdo->readStr("constructionTypes");
                mechEqKK->kompensatorP.count += 1;
                mechEqKK->fill = mechEqKK->fill | 16;
            }
            else {
                mechEqKK->kompensatorO.diametr = mAdo->read_double("diametr_truboprovoda");
                mechEqKK->kompensatorO.construction = mAdo->readStr("constructionTypes");
                mechEqKK->kompensatorO.count += 1;
                mechEqKK->fill = mechEqKK->fill | 8;
            }
        }
        else if (strcmp(tn, "kolodtsy") == 0) {
            if (externalID == 2)
            {
                mechEqKK->kolodecP.characteristic = mAdo->readStr("characteristicTypes");
                mechEqKK->kolodecP.purpose = mAdo->readStr("purposeTypes");
                mechEqKK->kolodecP.constructionL = mAdo->readStr("constructionTypesLyuki");
                mechEqKK->kolodecP.materialL = mAdo->readStr("materialTypesLyuki");
                mechEqKK->kolodecP.characteristicL = mAdo->readStr("characteristicTypesLyuki");
                mechEqKK->kolodecP.count += 1;
                mechEqKK->fill = mechEqKK->fill | 4;
            }
            else if (externalID == 3) {
                mechEqKK->kolodecO.characteristic = mAdo->readStr("characteristicTypes");
                mechEqKK->kolodecO.purpose = mAdo->readStr("purposeTypes");
                mechEqKK->kolodecO.constructionL = mAdo->readStr("constructionTypesLyuki");
                mechEqKK->kolodecO.materialL = mAdo->readStr("materialTypesLyuki");
                mechEqKK->kolodecO.characteristicL = mAdo->readStr("characteristicTypesLyuki");
                mechEqKK->kolodecO.count += 1;
                mechEqKK->fill = mechEqKK->fill | 2;
            }
            else {
                mechEqKK->kolodecG.characteristic = mAdo->readStr("characteristicTypes");
                mechEqKK->kolodecG.purpose = mAdo->readStr("purposeTypes");
                mechEqKK->kolodecG.constructionL = mAdo->readStr("constructionTypesLyuki");
                mechEqKK->kolodecG.materialL = mAdo->readStr("materialTypesLyuki");
                mechEqKK->kolodecG.characteristicL = mAdo->readStr("characteristicTypesLyuki");
                mechEqKK->kolodecG.count += 1;
                mechEqKK->fill = mechEqKK->fill | 1;
            }
        }
    };

    MechEqKK mechEqKK; /*Создаем структуру*/
    MechEqKK* tmpMech = &mechEqKK; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f2_2_mechanical_equipment.html", _TR("Ф2_2.Механическое оборудование"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from getPts_kompensator_kolodtsy(%d,'%s','%s') order by orderID", id, table, fragments);
        //q.Format("select * from getPts_kompensator_kolodtsy(%d,'%s','%s') order by orderID", 21, "ms");
        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Механическое оборудование. Компенсаторы и колодцы.)");
            return;
        }

        bool isGeneral = false;
        while (!mAdo->isEOF()) {
            tmpMech->beginPts = mAdo->readStr("beginNode");
            tmpMech->endPts = mAdo->readStr("endNode");
            tmpMech->pavilion = mAdo->readStr("pavilion");
            int externalId = mAdo->read_long(3);
            CString tn = mAdo->readStr("tblName");
            if (externalId == 1 && strcmp(tn, "kompensator") == 0) {
                if (!isGeneral)
                {
                    isGeneral = true;
                    externalId = 2;
                }
                else
                {
                    isGeneral = false;
                    externalId = 3;
                }
            }
            writeObject(externalId, tn, mAdo, tmpMech);

            if (!isGeneral)
                mAdo->MoveNext();

            if (mAdo->isEOF())
                tmpMech->next = nullptr;
            else
                tmpMech->next = new struct MechEqKK;
            tmpMech = tmpMech->next;
        }

        tmpMech = &mechEqKK;  /*Приравниваем указатель в начало структуры*/
        /*пока не заполнится последняя структура*/
        if (tmpMech->fill != 0)
            while (tmpMech != nullptr) {
                MechEqKK* next = tmpMech->next;
                MechEqKK* prev = tmpMech;

                while (!(next == nullptr)) {
                    bool changePrev = false;
                    /*16 KompP, 8 KompO, 4 KolP, 2 KolO, 1 KolG (31 все заполнено)*/
                    /*проверка заполнена ли подача и обратка, и в случае если что то не заполнено и совпадают участки, заполняем, иначе проверяем заполнения объектов (при заполнении смещать структуру)*/
                    /*если компенсатора подачи нет и следующий элемент это подача с совпадением данных заполняем*/
                    if ((tmpMech->fill & 16) == 0
                        && (next->fill & 16) != 0
                        && strcmp(tmpMech->beginPts, next->beginPts) == 0
                        && strcmp(tmpMech->endPts, next->endPts) == 0
                        && strcmp(tmpMech->pavilion, next->pavilion) == 0) {
                        changePrev = true;
                        tmpMech->kompensatorP.diametr = next->kompensatorP.diametr;
                        tmpMech->kompensatorP.construction = next->kompensatorP.construction;
                        tmpMech->kompensatorP.count = next->kompensatorP.count;
                        tmpMech->fill = tmpMech->fill | 16;
                    }
                    /*если компенсатора обратки нет и следующий элемент это обратка с совпадением данных заполняем*/
                    else if ((tmpMech->fill & 8) == 0
                        && (next->fill & 8) != 0
                        && strcmp(tmpMech->beginPts, next->beginPts) == 0
                        && strcmp(tmpMech->endPts, next->endPts) == 0
                        && strcmp(tmpMech->pavilion, next->pavilion) == 0) {
                        changePrev = true;
                        tmpMech->kompensatorO.diametr = next->kompensatorO.diametr;
                        tmpMech->kompensatorO.construction = next->kompensatorO.construction;
                        tmpMech->kompensatorO.count = next->kompensatorO.count;
                        tmpMech->fill = tmpMech->fill | 8;

                    }
                    /*если колодца подачи нет и следующий элемент это подача с совпадением данных заполняем*/
                    else if ((tmpMech->fill & 4) == 0
                        && (next->fill & 4) != 0
                        && strcmp(tmpMech->beginPts, next->beginPts) == 0
                        && strcmp(tmpMech->endPts, next->endPts) == 0
                        && strcmp(tmpMech->pavilion, next->pavilion) == 0) {
                        changePrev = true;
                        tmpMech->kolodecP.characteristic = next->kolodecP.characteristic;
                        tmpMech->kolodecP.purpose = next->kolodecP.purpose;
                        tmpMech->kolodecP.constructionL = next->kolodecP.constructionL;
                        tmpMech->kolodecP.materialL = next->kolodecP.materialL;
                        tmpMech->kolodecP.characteristicL = next->kolodecP.characteristicL;
                        tmpMech->kolodecP.count = next->kolodecP.count;
                        tmpMech->fill = tmpMech->fill | 4;
                    }
                    /*если колодца обратки нет и следующий элемент это обратка с совпадением данных заполняем*/
                    else if ((tmpMech->fill & 2) == 0
                        && (next->fill & 2) != 0
                        && strcmp(tmpMech->beginPts, next->beginPts) == 0
                        && strcmp(tmpMech->endPts, next->endPts) == 0) {
                        changePrev = true;
                        tmpMech->kolodecO.characteristic = next->kolodecO.characteristic;
                        tmpMech->kolodecO.purpose = next->kolodecO.purpose;
                        tmpMech->kolodecO.constructionL = next->kolodecO.constructionL;
                        tmpMech->kolodecO.materialL = next->kolodecO.materialL;
                        tmpMech->kolodecO.characteristicL = next->kolodecO.characteristicL;
                        tmpMech->kolodecO.count = next->kolodecO.count;
                        tmpMech->fill = tmpMech->fill | 2;
                    }
                    /*если колодца общего нет и следующий элемент это общий с совпадением данных заполняем*/
                    else if ((tmpMech->fill & 1) == 0
                        && (next->fill & 1) != 0
                        && strcmp(tmpMech->beginPts, next->beginPts) == 0
                        && strcmp(tmpMech->endPts, next->endPts) == 0
                        && strcmp(tmpMech->pavilion, next->pavilion) == 0) {
                        changePrev = true;
                        tmpMech->kolodecG.characteristic = next->kolodecG.characteristic;
                        tmpMech->kolodecG.purpose = next->kolodecG.purpose;
                        tmpMech->kolodecG.constructionL = next->kolodecG.constructionL;
                        tmpMech->kolodecG.materialL = next->kolodecG.materialL;
                        tmpMech->kolodecG.characteristicL = next->kolodecG.characteristicL;
                        tmpMech->kolodecG.count = next->kolodecG.count;
                        tmpMech->fill = tmpMech->fill | 1;
                    }
                    /*иначе пытаемя найти совпадения и заполнить*/
                    else {
                        if (strcmp(tmpMech->beginPts, next->beginPts) == 0
                            && strcmp(tmpMech->endPts, next->endPts) == 0
                            && strcmp(tmpMech->pavilion, next->pavilion) == 0)
                            if ((next->fill & 16) != 0 && checkParamsKompensator(tmpMech->kompensatorP, next->kompensatorP)) {
                                /*Обновляем компенсатор подачу*/
                                tmpMech->kompensatorP.count = tmpMech->kompensatorP.count + next->kompensatorP.count;
                                changePrev = true;
                            }
                            else
                                if ((next->fill & 8) != 0 && checkParamsKompensator(tmpMech->kompensatorO, next->kompensatorO)) {
                                    /*Обновляем компенсатор обратку*/
                                    tmpMech->kompensatorO.count = tmpMech->kompensatorO.count + next->kompensatorO.count;
                                    changePrev = true;
                                }
                                else
                                    if ((next->fill & 4) != 0 && checkParamsKolodec(tmpMech->kolodecP, next->kolodecP)) {
                                        /*Обновляем колодец подачу*/
                                        tmpMech->kolodecP.count = tmpMech->kolodecP.count + next->kolodecP.count;
                                        changePrev = true;
                                    }
                                    else
                                        if ((next->fill & 2) != 0 && checkParamsKolodec(tmpMech->kolodecO, next->kolodecO)) {
                                            /*Обновляем колодец обратку*/
                                            tmpMech->kolodecO.count = tmpMech->kolodecO.count + next->kolodecO.count;
                                            changePrev = true;
                                        }
                                        else
                                            if ((next->fill & 1) != 0 && checkParamsKolodec(tmpMech->kolodecG, next->kolodecG)) {
                                                /*Обновляем колодец общий*/
                                                tmpMech->kolodecG.count = tmpMech->kolodecG.count + next->kolodecG.count;
                                                changePrev = true;
                                            }
                    }
                    if (changePrev)
                        prev->next = next->next;
                    else
                    {
                        //prev->next = next;/*проверить без нее*/
                        prev = next;
                    }
                    next = next->next;
                }
                tmpMech = tmpMech->next;
            }
        tmpMech = &mechEqKK;  /*Приравниваем указатель в начало структуры*/
        if (tmpMech->fill != 0)
            while (tmpMech != nullptr) {
                fprintf(g, "<tr>");
                fprintf(g, "<td>%s</td>", tmpMech->beginPts);/*Начальный ПТС*/
                fprintf(g, "<td>%s</td>", tmpMech->endPts);/*Конечный ПТС*/

                fprintf(g, "<td>%s</td>", tmpMech->pavilion); /*Камера/павильон*/

                /*Компенсатор*/
                if ((tmpMech->fill & 16) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kompensatorP.construction); /*Конструкция подача*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 8) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kompensatorO.construction); /*Конструкция обратка*/
                else
                    fprintf(g, "<td></td>");

                if ((tmpMech->fill & 16) != 0)
                    fprintf(g, "<td>%f</td>", tmpMech->kompensatorP.diametr); /*Диаметр подача*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 8) != 0)
                    fprintf(g, "<td>%f</td>", tmpMech->kompensatorO.diametr); /*Диаметр обратка*/
                else
                    fprintf(g, "<td></td>");

                if ((tmpMech->fill & 16) != 0)
                    fprintf(g, "<td>%d</td>", tmpMech->kompensatorP.count); /*Количество подача*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 8) != 0)
                    fprintf(g, "<td>%d</td>", tmpMech->kompensatorO.count); /*Количество обратка*/
                else
                    fprintf(g, "<td></td>");

                /*Колодец*/

                if (tmpMech->kolodecP.purpose == "") tmpMech->kolodecP.purpose = "Колодец управления";
                if (tmpMech->kolodecO.purpose == "") tmpMech->kolodecO.purpose = "Колодец управления";
                if (tmpMech->kolodecG.purpose == "") tmpMech->kolodecG.purpose = "Колодец управления";

                if ((tmpMech->fill & 4) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecP.purpose); /*Назначение подача*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 2) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecO.purpose); /*Назначение обратка*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 1) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecG.purpose); /*Назначение общий*/
                else
                    fprintf(g, "<td></td>");

                if ((tmpMech->fill & 4) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecP.characteristic); /*Характеристика подача*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 2) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecO.characteristic); /*Характеристика обратка*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 1) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecG.characteristic); /*Характеристика общий*/
                else
                    fprintf(g, "<td></td>");

                if ((tmpMech->fill & 4) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecP.materialL); /*Материал подача*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 2) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecO.materialL); /*Материал обратка*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 1) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecG.materialL); /*Материал общий*/
                else
                    fprintf(g, "<td></td>");

                if ((tmpMech->fill & 4) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecP.constructionL); /*Конструкция люка подача*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 2) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecO.constructionL); /*Конструкция люка обратка*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 1) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecG.constructionL); /*Конструкция люка общий*/
                else
                    fprintf(g, "<td></td>");

                if ((tmpMech->fill & 4) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecP.characteristicL); /*Характеристика люка подача*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 2) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecO.characteristicL); /*Характеристика люка обратка*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 1) != 0)
                    fprintf(g, "<td>%s</td>", tmpMech->kolodecG.characteristicL); /*Характеристика люка общий*/
                else
                    fprintf(g, "<td></td>");

                if ((tmpMech->fill & 4) != 0)
                    fprintf(g, "<td>%d</td>", tmpMech->kolodecP.count); /*Количество подача*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 2) != 0)
                    fprintf(g, "<td>%d</td>", tmpMech->kolodecO.count); /*Количество обратка*/
                else
                    fprintf(g, "<td></td>");
                if ((tmpMech->fill & 1) != 0)
                    fprintf(g, "<td>%d</td>", tmpMech->kolodecG.count); /*Количество общий*/
                else
                    fprintf(g, "<td></td>");

                fprintf(g, "</tr>");

                tmpMech = tmpMech->next;

            }
        print_doc_end(g);

    }

}


void printChannels(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct Channel { /*Каналы*/
        CString beginPts, endPts, typeChannel, typeOverlap;
        double width, height, diametr;
        CString constructionType;
        double lenght;
        CString yearOfFoundation, yearLastRepair;
        CString elementChannel;
        double lenghtRepair;
        CString note;
        Channel* next;
    };

    Channel channel; /*Создаем структуру*/
    Channel* tmpChannel = &channel; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f3_channels.html", _TR("Ф3.Каналы"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from getPts_kanal(%d,'%s','%s') order by orderID", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Каналы)");
            return;
        }

        bool channelIsNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpChannel->beginPts = mAdo->readStr("beginNode");
            tmpChannel->endPts = mAdo->readStr("endNode");

            tmpChannel->typeChannel = mAdo->readStr("channelType");
            tmpChannel->typeOverlap = mAdo->readStr("overlapType");

            tmpChannel->width = mAdo->read_double("width");
            tmpChannel->height = mAdo->read_double("height");

            tmpChannel->diametr = mAdo->read_double("d_hps");
            tmpChannel->constructionType = mAdo->readStr("constructionType");
            tmpChannel->lenght = mAdo->read_double("lenChannel");

            tmpChannel->yearOfFoundation = mAdo->readStr("god_vvoda");
            tmpChannel->yearLastRepair = mAdo->readStr("last_date_remont");
            tmpChannel->elementChannel = mAdo->readStr("workListChannel");
            tmpChannel->lenghtRepair = mAdo->read_double("len_channel_remont");

            tmpChannel->note = mAdo->readStr("primechanie");

            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpChannel->next = nullptr;
            else
                tmpChannel->next = new struct Channel;
            tmpChannel = tmpChannel->next;
            if (!channelIsNotEmpty)
                channelIsNotEmpty = true;
        }



        tmpChannel = &channel;  /*Приравниваем указатель в начало структуры*/
        if (channelIsNotEmpty)
        {
            while (tmpChannel != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpChannel->beginPts);/*Начальный ПТС*/
                fprintf(g, "<td>%s</td>", tmpChannel->endPts);/*Конечный ПТС*/

                fprintf(g, "<td>%s</td>", tmpChannel->typeChannel);/*Тип канала*/
                fprintf(g, "<td>%s</td>", tmpChannel->typeOverlap);/*Тип перекрытия*/

                fprintf(g, "<td>%f</td>", tmpChannel->width); /*ширина*/
                fprintf(g, "<td>%f</td>", tmpChannel->height); /*высота*/
                fprintf(g, "<td>%f</td>", tmpChannel->diametr); /*диаметр условный*/
                fprintf(g, "<td>%s</td>", tmpChannel->constructionType);/*Конструкция канала*/
                fprintf(g, "<td>%f</td>", tmpChannel->lenght); /*длина канала*/
                fprintf(g, "<td>%s</td>", tmpChannel->yearOfFoundation);/*год ввода*/
                fprintf(g, "<td>%s</td>", tmpChannel->yearLastRepair);/*год последнего ремонта*/
                fprintf(g, "<td>%s</td>", tmpChannel->elementChannel);/*Элементы канала*/
                fprintf(g, "<td>%f</td>", tmpChannel->lenghtRepair); /*длина участка ремонта*/
                fprintf(g, "<td>%s</td>", tmpChannel->note);/*Примечание*/


                fprintf(g, "</tr>");

                tmpChannel = tmpChannel->next;

            }
        }

        print_doc_end(g);
    }

}


void printChambers(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct Chamber { /*Камеры*/
        CString beginPts, endPts, name, constructionType;
        double height, lenght, width;
        CString typeOverlap, presentOfGratings, yearOfFoundation, yearLastRepair;
        int count;
        CString elementChamber, area, fullName;
        CString balans, note;
        Chamber* next;
    };

    Chamber chamber; /*Создаем структуру*/
    Chamber* tmpChamber = &chamber; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f4_chambers.html", _TR("Ф4.Камеры"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
//        q.Format("select * from getPts_tkamera(%d,'%s','%s') order by orderID", id, table, fragments);
        q.Format("select * from getPts_tkamera(%d,'%s','%s')", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Камеры)");
            return;
        }

        bool chambersNotEmpty = false;
        while (!mAdo->isEOF()) {
//            tmpChamber->beginPts = mAdo->readStr("beginNode");
//            tmpChamber->endPts = mAdo->readStr("endNode");
            tmpChamber->name = mAdo->readStr("pavilion");
            tmpChamber->constructionType = mAdo->readStr("constructionType");
            tmpChamber->height = mAdo->read_double("height");
            tmpChamber->lenght = mAdo->read_double("lenKamera");
            tmpChamber->width = mAdo->read_double("width");
            tmpChamber->typeOverlap = mAdo->readStr("constructionOverlapType");
            tmpChamber->count = mAdo->read_double("count_lyukov");

            tmpChamber->presentOfGratings = mAdo->readStr("nalichie_reshetok");

            if (strcmp(tmpChamber->presentOfGratings, "TRUE") == 0)
                tmpChamber->presentOfGratings = "Да";
            else
                tmpChamber->presentOfGratings = "Нет";

            tmpChamber->yearOfFoundation = mAdo->readStr("god_vvoda");
//            tmpChamber->yearLastRepair = mAdo->readStr("last_date_remont");
//            tmpChamber->elementChamber = mAdo->readStr("workListKamera");
            tmpChamber->area = mAdo->readStr("naimenovanie_uchastka");
            tmpChamber->fullName = mAdo->readStr("fio");
            tmpChamber->balans = mAdo->readStr("organizations");

            tmpChamber->note = mAdo->readStr("primechanie");

            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpChamber->next = nullptr;
            else
                tmpChamber->next = new struct Chamber;
            tmpChamber = tmpChamber->next;
            if (!chambersNotEmpty)
                chambersNotEmpty = true;
        }



        tmpChamber = &chamber;  /*Приравниваем указатель в начало структуры*/
        if (chambersNotEmpty)
        {
            while (tmpChamber != nullptr) {
                fprintf(g, "<tr>");

//                fprintf(g, "<td>%s</td>", tmpChamber->beginPts);/*Начальный ПТС*/
//                fprintf(g, "<td>%s</td>", tmpChamber->endPts);/*Конечный ПТС*/

                fprintf(g, "<td>%s</td>", tmpChamber->name);/*Наименование узла*/ // 1
                fprintf(g, "<td>%s</td>", tmpChamber->constructionType);/*Конструкция*/ // 2
                fprintf(g, "<td>%f</td>", tmpChamber->height); /*высота*/ // 3
                fprintf(g, "<td>%f</td>", tmpChamber->lenght); /*длина*/ // 4
                fprintf(g, "<td>%f</td>", tmpChamber->width); /*ширина*/ // 5
                fprintf(g, "<td>%s</td>", tmpChamber->typeOverlap);/*Тип перекрытия*/ // 6
                fprintf(g, "<td>%d</td>", tmpChamber->count);/*Количество люков*/ // 7
                fprintf(g, "<td>%s</td>", tmpChamber->presentOfGratings);/*Наличие решеток*/ // 8
                fprintf(g, "<td>%s</td>", tmpChamber->yearOfFoundation);/*год ввода*/ // 9
//                fprintf(g, "<td>%s</td>", tmpChamber->yearLastRepair);/*год последнего ремонта*/
//                fprintf(g, "<td>%s</td>", tmpChamber->elementChamber);/*Элементы*/ 
                fprintf(g, "<td>%s</td>", tmpChamber->balans);/*Баланс*/ // 10
                fprintf(g, "<td>%s</td>", tmpChamber->note);/*Примечание*/ // 11
                fprintf(g, "<td>%s</td>", tmpChamber->area);/*Участок*/ // 12
                fprintf(g, "<td>%s</td>", tmpChamber->fullName);/*ФИО*/ // 13

                fprintf(g, "</tr>");

                tmpChamber = tmpChamber->next;

            }
        }

        print_doc_end(g);
    }

}

void printPavilions(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct Pavilion { /*Павильоны*/
        CString name, location, constructionType, area, fullName,
            equipment, firefighting, signaling, lightAvailable,
            schemeAvailable, elementChamber;
        double square;
        CString yearOfFoundation, yearLastRepair;
        CString balans, note;
        Pavilion* next;
    };

    Pavilion pavilion; /*Создаем структуру*/
    Pavilion* tmpPavilion = &pavilion; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f5_pavilions.html", _TR("Ф5.Павильоны"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from getPts_pavilion(%d,'%s','%s')", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Павильоны)");
            return;
        }

        bool pavilionsNotEmpty = false;
        while (!mAdo->isEOF()) {
//            tmpPavilion->name = mAdo->readStr("beginNode");
            tmpPavilion->name = mAdo->readStr("pavilion");
            tmpPavilion->location = mAdo->readStr("locationType");
            tmpPavilion->constructionType = mAdo->readStr("constructionType");
            tmpPavilion->square = mAdo->read_double("s");
            tmpPavilion->equipment = mAdo->readStr("oborudovanie_pavilona");

            tmpPavilion->firefighting = (mAdo->readStr("sredstva_pozharotushenija") == "TRUE") ? "Да" : "Нет";
            tmpPavilion->signaling = (mAdo->readStr("signalizacija") == "TRUE") ? "Да" : "Нет";
            tmpPavilion->lightAvailable = (mAdo->readStr("nalichie_osveshhenija") == "TRUE") ? "Да" : "Нет";
            tmpPavilion->schemeAvailable = (mAdo->readStr("nalichie_shem_truboprovodov") == "TRUE") ? "Да" : "Нет";

            tmpPavilion->yearOfFoundation = mAdo->readStr("god_vvoda");
//            tmpPavilion->yearLastRepair = mAdo->readStr("last_date_remont");
//            tmpPavilion->elementChamber = mAdo->readStr("workListPavilion");
            tmpPavilion->area = mAdo->readStr("naimenovanie_uchastka");
            tmpPavilion->fullName = mAdo->readStr("fio");
            tmpPavilion->balans = mAdo->readStr("organizations");

            tmpPavilion->note = mAdo->readStr("primechanie");

            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpPavilion->next = nullptr;
            else
                tmpPavilion->next = new struct Pavilion;
            tmpPavilion = tmpPavilion->next;
            if (!pavilionsNotEmpty)
                pavilionsNotEmpty = true;
        }



        tmpPavilion = &pavilion;  /*Приравниваем указатель в начало структуры*/
        if (pavilionsNotEmpty)
        {
            while (tmpPavilion != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpPavilion->name);/*Наименование узла*/
                fprintf(g, "<td>%s</td>", tmpPavilion->location);/*Месторасположения*/
                fprintf(g, "<td>%s</td>", tmpPavilion->constructionType);/*Конструкция*/
                fprintf(g, "<td>%f</td>", tmpPavilion->square); /*площадь*/

                fprintf(g, "<td>%s</td>", tmpPavilion->equipment);/*Оборудование*/
                fprintf(g, "<td>%s</td>", tmpPavilion->firefighting);/*Средства пожаротушения*/
                fprintf(g, "<td>%s</td>", tmpPavilion->signaling);/*Сигнализация*/
                fprintf(g, "<td>%s</td>", tmpPavilion->lightAvailable);/*Наличие освещения*/
                fprintf(g, "<td>%s</td>", tmpPavilion->schemeAvailable);/*Наличие схем трубопровода*/

                fprintf(g, "<td>%s</td>", tmpPavilion->yearOfFoundation);/*год ввода*/
//                fprintf(g, "<td>%s</td>", tmpPavilion->yearLastRepair);/*год последнего ремонта*/
//                fprintf(g, "<td>%s</td>", tmpPavilion->elementChamber);/*Элементы */
                fprintf(g, "<td>%s</td>", tmpPavilion->area);/*Участок*/
                fprintf(g, "<td>%s</td>", tmpPavilion->fullName);/*ФИО*/
                fprintf(g, "<td>%s</td>", tmpPavilion->balans);/*Баланс*/
                fprintf(g, "<td>%s</td>", tmpPavilion->note);/*Примечание*/


                fprintf(g, "</tr>");

                tmpPavilion = tmpPavilion->next;

            }
        }

        print_doc_end(g);
    }

}


void printSupports(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct Support { /*Опоры*/
        CString beginNode, endNode, constructionType, supportType;
        double diametr;
        int count;
        CString note;
        Support* next;
    };

    Support support; /*Создаем структуру*/
    Support* tmpSupport = &support; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f6_supports.html", _TR("Ф6.Опоры"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from getPts_opora(%d,'%s','%s') order by orderID", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Опоры)");
            return;
        }

        bool supportsNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpSupport->beginNode = mAdo->readStr("beginNode");
            tmpSupport->endNode = mAdo->readStr("endNode");
            tmpSupport->diametr = mAdo->read_double("diameterCondit");
            tmpSupport->supportType = mAdo->readStr("oporaType");
            tmpSupport->constructionType = mAdo->readStr("constructionType");
            tmpSupport->count = 1;
            tmpSupport->note = mAdo->readStr("primechanie");

            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpSupport->next = nullptr;
            else
                tmpSupport->next = new struct Support;
            tmpSupport = tmpSupport->next;
            if (!supportsNotEmpty)
                supportsNotEmpty = true;
        }

        /*Начало подсчета*/
        tmpSupport = &support;  /*Приравниваем указатель в начало структуры*/
        /*пока не заполнится последняя структура*/
        if (supportsNotEmpty)
            while (tmpSupport != nullptr) {
                Support* next = tmpSupport->next;
                Support* prev = tmpSupport;

                while (!(next == nullptr)) {
                    bool changePrev = false;

                    if (strcmp(tmpSupport->beginNode, next->beginNode) == 0
                        && strcmp(tmpSupport->endNode, next->endNode) == 0
                        && strcmp(tmpSupport->constructionType, next->constructionType) == 0
                        && strcmp(tmpSupport->supportType, next->supportType) == 0
                        && tmpSupport->diametr == next->diametr) {
                        changePrev = true;
                        tmpSupport->count++;
                        if (!strcmp(tmpSupport->note, next->note) == 0 && !next->note.IsEmpty())
                            tmpSupport->note = tmpSupport->note + ";\n" + next->note;

                    }

                    if (changePrev)
                        prev->next = next->next;
                    else
                        prev = next;

                    next = next->next;
                }
                tmpSupport = tmpSupport->next;
            }
        /*Конец подсчета*/


        tmpSupport = &support;  /*Приравниваем указатель в начало структуры*/
        if (supportsNotEmpty)
        {
            while (tmpSupport != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpSupport->beginNode);/*Начальный узел*/
                fprintf(g, "<td>%s</td>", tmpSupport->endNode);/*Конечный узел*/
                fprintf(g, "<td>%f</td>", tmpSupport->diametr); /*диаметр*/
                fprintf(g, "<td>%s</td>", tmpSupport->supportType);/*Тип опоры*/
                fprintf(g, "<td>%s</td>", tmpSupport->constructionType);/*Конструкция*/
                fprintf(g, "<td>%d</td>", tmpSupport->count);/*Количество опор*/
                fprintf(g, "<td>%s</td>", tmpSupport->note);/*Примечание*/


                fprintf(g, "</tr>");

                tmpSupport = tmpSupport->next;

            }
        }

        print_doc_end(g);
    }

}


void printSpecConstructions(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct SpecConstruction { /*Спец конструкции*/
        CString beginNode, endNode, description, numberScheme;
        double lenght;
        int count;
        CString note;
        SpecConstruction* next;
    };

    SpecConstruction specConstruction; /*Создаем структуру*/
    SpecConstruction* tmpSpConstr = &specConstruction; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f7_spec_constr.html", _TR("Ф7.Спец.констр."));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from getPts_duker_shield_bridge(%d,'%s','%s') order by orderID", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Спец. конструкции)");
            return;
        }

        bool specConstructionNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpSpConstr->beginNode = mAdo->readStr("beginNode");
            tmpSpConstr->endNode = mAdo->readStr("endNode");
            tmpSpConstr->lenght = mAdo->read_double("lenght");
            tmpSpConstr->description = mAdo->readStr("description");
            tmpSpConstr->numberScheme = mAdo->readStr("nomer_chertezha");
            tmpSpConstr->count = 0;/*mAdo->read_long("kolichestvo_uporov");*/
            tmpSpConstr->note = mAdo->readStr("primechanie");

            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpSpConstr->next = nullptr;
            else
                tmpSpConstr->next = new struct SpecConstruction;
            tmpSpConstr = tmpSpConstr->next;
            if (!specConstructionNotEmpty)
                specConstructionNotEmpty = true;
        }

        /*Начало подсчета*/
        tmpSpConstr = &specConstruction;  /*Приравниваем указатель в начало структуры*/
        /*пока не заполнится последняя структура*/
        if (specConstructionNotEmpty)
            while (tmpSpConstr != nullptr) {
                SpecConstruction* next = tmpSpConstr->next;
                SpecConstruction* prev = tmpSpConstr;

                while (!(next == nullptr)) {
                    bool changePrev = false;

                    if (strcmp(tmpSpConstr->beginNode, next->beginNode) == 0
                        && strcmp(tmpSpConstr->endNode, next->endNode) == 0
                        && strcmp(tmpSpConstr->description, next->description) == 0
                        && strcmp(tmpSpConstr->numberScheme, next->numberScheme) == 0
                        && tmpSpConstr->lenght == next->lenght) {
                        changePrev = true;
                        tmpSpConstr->count++;
                        if (!strcmp(tmpSpConstr->note, next->note) == 0 && !next->note.IsEmpty())
                            tmpSpConstr->note = tmpSpConstr->note + ";\n" + next->note;

                    }

                    if (changePrev)
                        prev->next = next->next;
                    else
                        prev = next;

                    next = next->next;
                }
                tmpSpConstr = tmpSpConstr->next;
            }
        /*Конец подсчета*/

        tmpSpConstr = &specConstruction;  /*Приравниваем указатель в начало структуры*/
        if (specConstructionNotEmpty)
        {
            while (tmpSpConstr != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpSpConstr->beginNode);/*Начальный узел*/
                fprintf(g, "<td>%s</td>", tmpSpConstr->endNode);/*Конечный узел*/
                fprintf(g, "<td>%f</td>", tmpSpConstr->lenght); /*длина*/
                fprintf(g, "<td>%s</td>", tmpSpConstr->description);/*Описание*/
                fprintf(g, "<td>%s</td>", tmpSpConstr->numberScheme);/*Номер чертежа*/
                fprintf(g, "<td>%d</td>", tmpSpConstr->count);/*Количество*/
                fprintf(g, "<td>%s</td>", tmpSpConstr->note);/*Примечание*/


                fprintf(g, "</tr>");

                tmpSpConstr = tmpSpConstr->next;

            }
        }

        print_doc_end(g);
    }

}

void printIsolations(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct Isolation { /*Спец конструкции*/
        CString beginNode, endNode, material1, material2, material3;
        double thickness1, thickness2;
        CString note;
        Isolation* next;
    };

    Isolation isolation; /*Создаем структуру*/
    Isolation* tmpIsolation = &isolation; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f8_isolation.html", _TR("Ф8.Изоляция труб"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from getIsolTubesPts(%d,'%s','%s') order by orderID", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Изоляция труб)");
            return;
        }

        bool isolationNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpIsolation->beginNode = mAdo->readStr("beginNode");
            tmpIsolation->endNode = mAdo->readStr("endNode");
            tmpIsolation->material1 = mAdo->readStr("isolMaterial");
            tmpIsolation->thickness1 = mAdo->read_double("isolThickness");
            tmpIsolation->material2 = mAdo->readStr("externalMaterial");
            tmpIsolation->thickness2 = mAdo->read_double("externCoverThick");
            tmpIsolation->material3 = mAdo->readStr("anticorrMaterial");
            tmpIsolation->note = mAdo->readStr("primechanie");

            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpIsolation->next = nullptr;
            else
                tmpIsolation->next = new struct Isolation;
            tmpIsolation = tmpIsolation->next;
            if (!isolationNotEmpty)
                isolationNotEmpty = true;
        }



        tmpIsolation = &isolation;  /*Приравниваем указатель в начало структуры*/
        if (isolationNotEmpty)
        {
            while (tmpIsolation != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpIsolation->beginNode);/*Начальный узел*/
                fprintf(g, "<td>%s</td>", tmpIsolation->endNode);/*Конечный узел*/
                fprintf(g, "<td>%s</td>", tmpIsolation->material1); /*тепло изоляц материал*/
                fprintf(g, "<td>%f</td>", tmpIsolation->thickness1);/*толщина тепловой изоляции*/
                fprintf(g, "<td>%s</td>", tmpIsolation->material2); /*наружный материал*/
                fprintf(g, "<td>%f</td>", tmpIsolation->thickness2);/*толщина наружного материала*/
                fprintf(g, "<td>%s</td>", tmpIsolation->material3); /*материал антикорроз. покрытия*/
                fprintf(g, "<td>%s</td>", tmpIsolation->note);/*Примечание*/


                fprintf(g, "</tr>");

                tmpIsolation = tmpIsolation->next;

            }
        }

        print_doc_end(g);
    }

}


void printRespPersons(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct RespPerson { /* Ответственные лица*/
        CString nameNetwork, regionMaintenance, areaMaintenance, chiefFio, date, position, fio;
        int number;
        RespPerson* next;
    };

    RespPerson respPerson; /*Создаем структуру*/
    RespPerson* tmpRespPerson = &respPerson; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f9_responsible_persons.html", _TR("Ф9.Ответств.лицо"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        //      q.Format("select * from getPts_responsible_person(%d,'%s','%s')", id, table, fragments);
        q.Format("select * from getPts_responsible_person(%d,'%s')", id, table);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Ответств.лицо)");
            return;
        }

        bool respPersonNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpRespPerson->nameNetwork = mAdo->readStr("naimenovanie_uchastka");//nameNetwork
            tmpRespPerson->regionMaintenance = mAdo->readStr("naimenovanie_rayona");//regionMaintenance
            tmpRespPerson->areaMaintenance = mAdo->readStr("nomer_uchastka");//areaMaintenance
            tmpRespPerson->chiefFio = mAdo->readStr("fio");//chiefFio
            tmpRespPerson->number = mAdo->read_long("nomer_prikaza_otv");//number
            tmpRespPerson->date = mAdo->readStr("data_prikaza_otv");//date
            tmpRespPerson->position = mAdo->readStr("otv_dolzhnost");//position
            tmpRespPerson->fio = mAdo->readStr("otv_fio");//fio

            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpRespPerson->next = nullptr;
            else
                tmpRespPerson->next = new struct RespPerson;
            tmpRespPerson = tmpRespPerson->next;
            if (!respPersonNotEmpty)
                respPersonNotEmpty = true;
        }



        tmpRespPerson = &respPerson;  /*Приравниваем указатель в начало структуры*/
        if (respPersonNotEmpty)
        {
            while (tmpRespPerson != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpRespPerson->nameNetwork);/*Наимнование фрагмента тепловой сети*/
                fprintf(g, "<td>%s</td>", tmpRespPerson->regionMaintenance);/*Район эксплуатации*/
                fprintf(g, "<td>%s</td>", tmpRespPerson->areaMaintenance); /*Участок эксплуатации*/
                fprintf(g, "<td>%s</td>", tmpRespPerson->chiefFio); /*ФИО начальника участка*/
                fprintf(g, "<td>%d</td>", tmpRespPerson->number);/*Номер приказа о назначении*/
                fprintf(g, "<td>%s</td>", tmpRespPerson->date); /*Дата приказа о назначении*/
                fprintf(g, "<td>%s</td>", tmpRespPerson->position);/*Должность*/
                fprintf(g, "<td>%s</td>", tmpRespPerson->fio);/*ФИО*/

                fprintf(g, "</tr>");

                tmpRespPerson = tmpRespPerson->next;

            }
        }

        print_doc_end(g);
    }

}

void printRemonts(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{
    struct Remont { /*Ремонт*/
        CString beginNode, endNode,
            typeOfWork,dateOfFoundation, dateOfFinish,
            typeStrip;
        double lenghtReplacedTube, recoveryStrip,  diam1ReplacedTube, diam2ReplacedTube, diam3ReplacedTube, thick, recoveryIsolation, asphalt;
        CString workList1, workList2, workList3, number, date;

        CString subdivisionName, respFullName;

        Remont* next;
    };

    Remont remont; /*Создаем структуру*/
    Remont* tmpRemont = &remont; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f10_remont.html", _TR("Ф10.Ремонт"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from  getPts_remont2(%d,'%s','%s')", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Ремонт)");
            return;
        }

        bool remontNotEmpty = false;
        while (!mAdo->isEOF()) {

            tmpRemont->beginNode = mAdo->readStr("Наименование начального узла");
            tmpRemont->endNode = mAdo->readStr("Наименование конечного узла");
            tmpRemont->typeOfWork = mAdo->readStr("Вид ремонта");
            tmpRemont->dateOfFoundation = mAdo->readStr("Дата начала ремонтных работ");
            tmpRemont->dateOfFinish = mAdo->readStr("Дата завершения ремонтных работ");
            tmpRemont->typeStrip = mAdo->readStr("Тип прокладки");

            tmpRemont->lenghtReplacedTube = mAdo->read_double("Длина заменённой трубы, м");
            tmpRemont->recoveryStrip = mAdo->read_double("Восстановление канальной прокладки, м");
            tmpRemont->diam1ReplacedTube = mAdo->read_double("Диаметр условный, заменённой трубы, м");
            tmpRemont->diam2ReplacedTube = mAdo->read_double("Диаметр внутренний, заменённой трубы, м");
            tmpRemont->diam3ReplacedTube = mAdo->read_double("Диаметр наружный, заменённой трубы, м");
            tmpRemont->thick = mAdo->read_double("Толщина стенки, , заменённой трубы, мм");
            tmpRemont->recoveryIsolation = mAdo->read_double("Восстановление тепловой изоляции поверхности трубы, м2");
            tmpRemont->asphalt = mAdo->read_double("Асфальтирование, ремонт, м2");

            tmpRemont->workList1 = mAdo->readStr("Перечень работ (трубопровода)");
            tmpRemont->workList2 = mAdo->readStr("Перечень работ (канал)");
            tmpRemont->workList3 = mAdo->readStr("Перечень работ (камеры)");

            tmpRemont->number = mAdo->readStr("Номер приказа на ввод в эксплуатацию");
            tmpRemont->date = mAdo->readStr("Дата приказа ввода в эксплуацию");

            tmpRemont->subdivisionName = mAdo->readStr("Подразделение производившее ремонт");
            tmpRemont->respFullName = mAdo->readStr("Ответственный за ремонт");

            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpRemont->next = nullptr;
            else
                tmpRemont->next = new struct Remont;
            tmpRemont = tmpRemont->next;
            if (!remontNotEmpty)
                remontNotEmpty = true;
        }



        tmpRemont = &remont;  /*Приравниваем указатель в начало структуры*/
        if (remontNotEmpty)
        {
            while (tmpRemont != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpRemont->beginNode);/*Начальный узел*/
                fprintf(g, "<td>%s</td>", tmpRemont->endNode);/*Конечный узел*/

                fprintf(g, "<td>%s</td>", tmpRemont->typeOfWork);/*Вид работ*/
                fprintf(g, "<td>%s</td>", tmpRemont->dateOfFoundation);/*Дата начала работ*/
                fprintf(g, "<td>%s</td>", tmpRemont->dateOfFinish);/*Дата окончания работ */
                fprintf(g, "<td>%s</td>", tmpRemont->typeStrip);

                fprintf(g, "<td>%.2f</td>", tmpRemont->lenghtReplacedTube);
                fprintf(g, "<td>%.2f</td>", tmpRemont->recoveryStrip);
                fprintf(g, "<td>%.2f</td>", tmpRemont->diam1ReplacedTube);
                fprintf(g, "<td>%.2f</td>", tmpRemont->diam2ReplacedTube);
                fprintf(g, "<td>%.2f</td>", tmpRemont->diam3ReplacedTube);
                fprintf(g, "<td>%.2f</td>", tmpRemont->thick);
                fprintf(g, "<td>%.2f</td>", tmpRemont->recoveryIsolation);
                fprintf(g, "<td>%.2f</td>", tmpRemont->asphalt);
                
                fprintf(g, "<td>%s</td>", tmpRemont->workList1);
                fprintf(g, "<td>%s</td>", tmpRemont->workList2);
                fprintf(g, "<td>%s</td>", tmpRemont->workList3);
                fprintf(g, "<td>%s</td>", tmpRemont->number);
                fprintf(g, "<td>%s</td>", tmpRemont->date);
                fprintf(g, "<td>%s</td>", tmpRemont->subdivisionName);
                fprintf(g, "<td>%s</td>", tmpRemont->respFullName);

                fprintf(g, "</tr>");

                tmpRemont = tmpRemont->next;

            }
        }

        print_doc_end(g);
    }

}

void printDefects(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct Defect { /*Дефект*/
        CString beginNode, endNode, mode, condition,
            date, address, defectDescription,
            viewDefect, categoryDefect, liquidationMethod,
            dateOfFoundation, dateOfFinish, repairTube,
            repairChanel, repairCamera;
        double lenghtReplacedTube, lenghtReplacedIsolation, lenghtRepairArea;
        CString subdivisionName, respPosition, respFullName;
        CString fullName, area;
        CString note;
        Defect* next;
    };

    Defect defect; /*Создаем структуру*/
    Defect* tmpDefect = &defect; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f11_defect.html", _TR("Ф11.Нарушение"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from  getPts_defect(%d,'%s','%s') order by orderID", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Дефект)");
            return;
        }

        bool defectNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpDefect->beginNode = mAdo->readStr("Наименование начального узла");
            tmpDefect->endNode = mAdo->readStr("Наименование конечного узла");
            tmpDefect->mode = mAdo->readStr("Режим");
            tmpDefect->condition = mAdo->readStr("Состояние");
            tmpDefect->date = mAdo->readStr("Дата обнаружения нарушения");
            tmpDefect->address = mAdo->readStr("Адрес");
            tmpDefect->defectDescription = mAdo->readStr("Описание повреждения");

            tmpDefect->viewDefect = mAdo->readStr("Вид нарушения");
            tmpDefect->categoryDefect = mAdo->readStr("Категория нарушения");
            tmpDefect->liquidationMethod = mAdo->readStr("Способ ликвидации нарушения");
            tmpDefect->dateOfFoundation = mAdo->readStr("Дата начала ремонтных работ");//dateOfFoundation
            tmpDefect->dateOfFinish = mAdo->readStr("Дата завершения ремонтных работ");//dateOfFinish
            tmpDefect->repairTube = mAdo->readStr("Ремонт трубопровода и элементов");//


            tmpDefect->lenghtReplacedTube = mAdo->read_double("Длина заменённой трубы, м");//lenghtReplacedTube
            tmpDefect->lenghtReplacedIsolation = mAdo->read_double("Длина заменённой изоляции, м");//lenghtReplacedIsolation
            tmpDefect->repairChanel = mAdo->readStr("Ремонт канала");
            tmpDefect->repairCamera = mAdo->readStr("Ремонт камеры");
            tmpDefect->lenghtRepairArea = mAdo->read_double("Длина участка ремонта канала");//lenghtRepairArea

            tmpDefect->subdivisionName = mAdo->readStr("Подразделение производившего работы");//subdivisionName
            tmpDefect->respPosition = mAdo->readStr("Должность ответственного");//respPosition
            tmpDefect->respFullName = mAdo->readStr("ФИО ответственного");//respFullName
            tmpDefect->area = mAdo->readStr("Участок эксплуатации");//area
            tmpDefect->fullName = mAdo->readStr("ФИО");//fullName
            tmpDefect->note = mAdo->readStr("primechanie");//note

            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpDefect->next = nullptr;
            else
                tmpDefect->next = new struct Defect;
            tmpDefect = tmpDefect->next;
            if (!defectNotEmpty)
                defectNotEmpty = true;
        }



        tmpDefect = &defect;  /*Приравниваем указатель в начало структуры*/
        if (defectNotEmpty)
        {
            while (tmpDefect != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpDefect->beginNode);/*Начальный узел*/
                fprintf(g, "<td>%s</td>", tmpDefect->endNode);/*Конечный узел*/


                fprintf(g, "<td>%s</td>", tmpDefect->mode);/*Режим*/
                fprintf(g, "<td>%s</td>", tmpDefect->condition);/*Состояние*/
                fprintf(g, "<td>%s</td>", tmpDefect->date);/*Дата обнаружения нарушения*/
                fprintf(g, "<td>%s</td>", tmpDefect->address);/*Адрес*/
                fprintf(g, "<td>%s</td>", tmpDefect->defectDescription);/*Описание повреждения*/

                fprintf(g, "<td>%s</td>", tmpDefect->viewDefect);/*Вид нарушения*/
                fprintf(g, "<td>%s</td>", tmpDefect->categoryDefect);/*Категория нарушения*/
                fprintf(g, "<td>%s</td>", tmpDefect->liquidationMethod);/*Способ ликвидации нарушения*/
                fprintf(g, "<td>%s</td>", tmpDefect->dateOfFoundation);/*Дата начала работ*/
                fprintf(g, "<td>%s</td>", tmpDefect->dateOfFinish);/*Дата окончания работ */
                fprintf(g, "<td>%s</td>", tmpDefect->repairTube);/*Ремонт трубопровода и элементов*/

                fprintf(g, "<td>%f</td>", tmpDefect->lenghtReplacedTube);/* Длина заменённой трубы*/
                fprintf(g, "<td>%f</td>", tmpDefect->lenghtReplacedIsolation);/* Длина заменённой изоляции, м*/
                fprintf(g, "<td>%s</td>", tmpDefect->repairChanel);/*Ремонт канала*/
                fprintf(g, "<td>%s</td>", tmpDefect->repairCamera);/*Ремонт камеры*/
                fprintf(g, "<td>%f</td>", tmpDefect->lenghtRepairArea);/*Длина  участка ремонта канала,м*/


                fprintf(g, "<td>%s</td>", tmpDefect->subdivisionName);/*Подразделение*/
                fprintf(g, "<td>%s</td>", tmpDefect->respPosition);/*Должность ответственного*/
                fprintf(g, "<td>%s</td>", tmpDefect->respFullName);/*ФИО ответственного*/

                fprintf(g, "<td>%s</td>", tmpDefect->area);/*Участок эксплуатации*/
                fprintf(g, "<td>%s</td>", tmpDefect->fullName);/*ФИО*/

                fprintf(g, "<td>%s</td>", tmpDefect->note);/*Примечание*/


                fprintf(g, "</tr>");

                tmpDefect = tmpDefect->next;

            }
        }

        print_doc_end(g);
    }

}


void printPits(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct Pit { /*Шурфовки*/
        CString beginNode, endNode,
            sign, purpose, address,
            condition, dateBegin, dateFinish,
            results;
        double diametr;
        int number;
        CString subdivisionName, respPosition, respFullName;
        CString fullName, area;
        CString note;
        Pit* next;
    };

    Pit pit; /*Создаем структуру*/
    Pit* tmpPit = &pit; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f12_pits.html", _TR("Ф12.Шурфовки"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from  getPts_shurf(%d,'%s','%s') order by orderID", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Шурфовки)");
            return;
        }

        bool pitNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpPit->beginNode = mAdo->readStr("Наименование начального узла");
            tmpPit->endNode = mAdo->readStr("Наименование конечного узла");
            tmpPit->sign = mAdo->readStr("Признак участка трубопровода");
            tmpPit->diametr = mAdo->read_double("Диаметр трубопровода, мм");//diametr
            tmpPit->purpose = mAdo->readStr("Назначение вскрытия");//purpose
            tmpPit->address = mAdo->readStr("Адрес");
            tmpPit->condition = mAdo->readStr("Состояние");
            tmpPit->dateBegin = mAdo->readStr("Дата начала");
            tmpPit->dateFinish = mAdo->readStr("Дата окончания");
            tmpPit->number = mAdo->read_long("Номер акта");//number
            tmpPit->results = mAdo->readStr("Результаты осмотра");//results
            tmpPit->note = mAdo->readStr("Примечание");//note
            tmpPit->respFullName = mAdo->readStr("ФИО утверждающего");//respFullName
            tmpPit->respPosition = mAdo->readStr("Должность утверждающего");//respPosition

            tmpPit->subdivisionName = mAdo->readStr("Служба утверждающего");//subdivisionName
            tmpPit->area = mAdo->readStr("Участок эксплуатации");//area
            tmpPit->fullName = mAdo->readStr("ФИО начальника участка");//fullName
            

            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpPit->next = nullptr;
            else
                tmpPit->next = new struct Pit;
            tmpPit = tmpPit->next;
            if (!pitNotEmpty)
                pitNotEmpty = true;
        }



        tmpPit = &pit;  /*Приравниваем указатель в начало структуры*/
        if (pitNotEmpty)
        {
            while (tmpPit != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpPit->beginNode);/*Начальный узел*/
                fprintf(g, "<td>%s</td>", tmpPit->endNode);/*Конечный узел*/

                fprintf(g, "<td>%s</td>", tmpPit->sign);/*Признак участка трубопровода*/
                fprintf(g, "<td>%.2f</td>", tmpPit->diametr);/*Диаметр*/
                fprintf(g, "<td>%s</td>", tmpPit->purpose);/*Назначение вскрытия */
                fprintf(g, "<td>%s</td>", tmpPit->address );/*Адрес */
                fprintf(g, "<td>%s</td>", tmpPit->condition);/*Состояние */
                fprintf(g, "<td>%s</td>", tmpPit->dateBegin);/*Дата начала */
                fprintf(g, "<td>%s</td>", tmpPit->dateFinish);/*Дата окончания */
                fprintf(g, "<td>%d</td>", tmpPit->number);/*Номер акта */
                fprintf(g, "<td>%s</td>", tmpPit->results);/*Результаты*/
                fprintf(g, "<td>%s</td>", tmpPit->note);/*Примечание*/

                
                fprintf(g, "<td>%s</td>", tmpPit->respFullName);/*ФИО утверждающего*/
                fprintf(g, "<td>%s</td>", tmpPit->respPosition);/*Должность утверждающего*/
                fprintf(g, "<td>%s</td>", tmpPit->subdivisionName);/*Служба утверждающего*/
                

                fprintf(g, "<td>%s</td>", tmpPit->area);/*Участок эксплуатации*/
                fprintf(g, "<td>%s</td>", tmpPit->fullName);/*ФИО*/

                


                fprintf(g, "</tr>");

                tmpPit = tmpPit->next;

            }
        }

        print_doc_end(g);
    }

}


void printClippings(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct Clipping { /*Вырезки*/
        CString beginNode, endNode,
            sign, purpose, address,
            condition, dateBegin, dateFinish,
            results;
        double diametr;
        int number;
        CString subdivisionName, respPosition, respFullName;
        CString fullName, area;
        CString metalCondition, degreeOfOuterCorrosion, degreeOfInnerCorrosion, note;
        Clipping* next;
    };

    Clipping clipping; /*Создаем структуру*/
    Clipping* tmpClipping = &clipping; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f13_clippings.html", _TR("Ф13.Вырезки"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from  getPts_cut_out(%d,'%s','%s') order by orderID", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Вырезки)");
            return;
        }

        bool clippingNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpClipping->beginNode = mAdo->readStr("Наименование начального узла");
            tmpClipping->endNode = mAdo->readStr("Наименование конечного узла");
            tmpClipping->sign = mAdo->readStr("Признак участка трубопровода");
            tmpClipping->diametr = mAdo->read_double("Диаметр трубопровода, мм");//diametr
            tmpClipping->purpose = mAdo->readStr("Назначение вскрытия");//purpose
            tmpClipping->address = mAdo->readStr("Адрес");
            tmpClipping->condition = mAdo->readStr("Состояние");
            tmpClipping->dateBegin = mAdo->readStr("Дата начала");
            tmpClipping->dateFinish = mAdo->readStr("Дата окончания");
            tmpClipping->number = mAdo->read_long("Номер акта");//number
            tmpClipping->results = mAdo->readStr("Результаты осмотра");//results
            tmpClipping->metalCondition = mAdo->readStr("Состояние металла трубопровода");//note
            tmpClipping->degreeOfOuterCorrosion = mAdo->readStr("Cтепень внешней коррозии");//note
            tmpClipping->degreeOfInnerCorrosion = mAdo->readStr("Степень внутренней коррозии");//note
            tmpClipping->note = mAdo->readStr("Примечание");//note
            tmpClipping->respFullName = mAdo->readStr("ФИО утверждающего");//respFullName
            tmpClipping->respPosition = mAdo->readStr("Должность утверждающего");//respPosition

            tmpClipping->subdivisionName = mAdo->readStr("Служба утверждающего");//subdivisionName
            tmpClipping->area = mAdo->readStr("Участок эксплуатации");//area
            tmpClipping->fullName = mAdo->readStr("ФИО начальника участка");//fullName

            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpClipping->next = nullptr;
            else
                tmpClipping->next = new struct Clipping;
            tmpClipping = tmpClipping->next;
            if (!clippingNotEmpty)
                clippingNotEmpty = true;
        }



        tmpClipping = &clipping;  /*Приравниваем указатель в начало структуры*/
        if (clippingNotEmpty)
        {
            while (tmpClipping != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpClipping->beginNode);/*Начальный узел*/
                fprintf(g, "<td>%s</td>", tmpClipping->endNode);/*Конечный узел*/

                fprintf(g, "<td>%s</td>", tmpClipping->sign);/*Признак участка трубопровода*/
                fprintf(g, "<td>%.2f</td>", tmpClipping->diametr);/*Диаметр*/
                fprintf(g, "<td>%s</td>", tmpClipping->purpose);/*Назначение вскрытия */
                fprintf(g, "<td>%s</td>", tmpClipping->address);/*Адрес */
                fprintf(g, "<td>%s</td>", tmpClipping->condition);/*Состояние */
                fprintf(g, "<td>%s</td>", tmpClipping->dateBegin);/*Дата начала */
                fprintf(g, "<td>%s</td>", tmpClipping->dateFinish);/*Дата окончания */
                fprintf(g, "<td>%d</td>", tmpClipping->number);/*Номер акта */
                fprintf(g, "<td>%s</td>", tmpClipping->results);/*Результаты*/
                fprintf(g, "<td>%s</td>", tmpClipping->metalCondition);
                fprintf(g, "<td>%s</td>", tmpClipping->degreeOfOuterCorrosion);
                fprintf(g, "<td>%s</td>", tmpClipping->degreeOfInnerCorrosion);
                fprintf(g, "<td>%s</td>", tmpClipping->note);/*Примечание*/


                fprintf(g, "<td>%s</td>", tmpClipping->respFullName);/*ФИО утверждающего*/
                fprintf(g, "<td>%s</td>", tmpClipping->respPosition);/*Должность утверждающего*/
                fprintf(g, "<td>%s</td>", tmpClipping->subdivisionName);/*Служба утверждающего*/


                fprintf(g, "<td>%s</td>", tmpClipping->area);/*Участок эксплуатации*/
                fprintf(g, "<td>%s</td>", tmpClipping->fullName);/*ФИО*/



                fprintf(g, "</tr>");

                tmpClipping = tmpClipping->next;

            }
        }

        print_doc_end(g);
    }

}

void printTrials(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct Trial { /*Испытания*/
        CString beginNode, endNode,
            descriptionContour, view, date;
        double p1, p2;
        CString desicion;
        CString subdivisionName, respPosition, respFullName;
        CString fullName, area;
        CString address, description, liquidationMethod;
        Trial* next;
    };

    Trial trial; /*Создаем структуру*/
    Trial* tmpTrial = &trial; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f14_trials.html", _TR("Ф14.Опрессовки"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from  getPts_test(%d,'%s','%s')", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Опрессовки)");
            return;
        }

        bool trialsNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpTrial->beginNode = mAdo->readStr("Наименование начального узла");
            tmpTrial->endNode = mAdo->readStr("Наименование конечного узла");

            tmpTrial->descriptionContour = mAdo->readStr("Описание контура");
            tmpTrial->view = mAdo->readStr("Вид испытания");
            tmpTrial->date = mAdo->readStr("Дата проведения опрессовки");
            tmpTrial->p1 = mAdo->read_double("Давление опрессовки 1 этапа, кгс/см2");
            tmpTrial->p2 = mAdo->read_double("Давление опрессовки 2 этапа, кгс/см2");
            tmpTrial->desicion = mAdo->readStr("Решение комиссии");
            tmpTrial->address = mAdo->readStr("Адрес нарушения");
            tmpTrial->description = mAdo->readStr("Описание повреждения");
            tmpTrial->liquidationMethod = mAdo->readStr("Способ ликвидации нарушения");

            tmpTrial->respFullName = mAdo->readStr("ФИО руководителя испытаний");
            tmpTrial->respPosition = mAdo->readStr("Должность руководителя испытаний");
            tmpTrial->subdivisionName = mAdo->readStr("Подразделение руководителя испытаний");
            
            
            tmpTrial->area = mAdo->readStr("Участок эксплуатации");//area
            tmpTrial->fullName = mAdo->readStr("ФИО начальника участка");//fullName
        
            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpTrial->next = nullptr;
            else
                tmpTrial->next = new struct Trial;
            tmpTrial = tmpTrial->next;
            if (!trialsNotEmpty)
                trialsNotEmpty = true;
        }



        tmpTrial = &trial;  /*Приравниваем указатель в начало структуры*/
        if (trialsNotEmpty)
        {
            while (tmpTrial != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpTrial->beginNode);/*Начальный узел*/
                fprintf(g, "<td>%s</td>", tmpTrial->endNode);/*Конечный узел*/

                fprintf(g, "<td>%s</td>", tmpTrial->descriptionContour);
                fprintf(g, "<td>%s</td>", tmpTrial->view);
                fprintf(g, "<td>%s</td>", tmpTrial->date);

                fprintf(g, "<td>%.2f</td>", tmpTrial->p1);
                fprintf(g, "<td>%.2f</td>", tmpTrial->p2);

                fprintf(g, "<td>%s</td>", tmpTrial->desicion);
                fprintf(g, "<td>%s</td>", tmpTrial->address);
                fprintf(g, "<td>%s</td>", tmpTrial->description);
                fprintf(g, "<td>%s</td>", tmpTrial->liquidationMethod);

                
                fprintf(g, "<td>%s</td>", tmpTrial->respFullName);/*ФИО ответственного*/
                fprintf(g, "<td>%s</td>", tmpTrial->respPosition);/*Должность ответственного*/
                fprintf(g, "<td>%s</td>", tmpTrial->subdivisionName);/*Подразделение*/

                fprintf(g, "<td>%s</td>", tmpTrial->area);/*Участок эксплуатации*/
                fprintf(g, "<td>%s</td>", tmpTrial->fullName);/*ФИО*/

            


                fprintf(g, "</tr>");

                tmpTrial = tmpTrial->next;

            }
        }

        print_doc_end(g);
    }

}

void printExamination(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString fragments)
{

    struct Examination { /*Освидетельствование*/
        CString beginNode, endNode, sign,
            date, appearance, conditionEquipment, 
            conditionMetal, conditionConstruction, 
            condition1, condition2,
            condition3, condition4,
            condition5, condition6;
        double diametr;
        CString subdivisionName, respFullName;
        CString fullName, area;
        Examination* next;
    };

    Examination examination; /*Создаем структуру*/
    Examination* tmpExamination = &examination; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f15_examination.html", _TR("Ф15.Осмотр"));
    if (g) {
        CString q, table;

        if (ms_rs == 0)
        {
            table.Format("ms");
        }
        else {
            table.Format("rs");
        }
        q.Format("select * from  getPts_osmotr(%d,'%s','%s') order by orderID", id, table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе при формировании паспорта (Осмотр)");
            return;
        }

        bool examinationNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpExamination->beginNode = mAdo->readStr("Наименование начального узла");
            tmpExamination->endNode = mAdo->readStr("Наименование конечного узла");
            tmpExamination->diametr = mAdo->read_double("Диаметр трубопровода, мм");
            tmpExamination->sign = mAdo->readStr("Признак участка трубопровода");
            tmpExamination->date = mAdo->readStr("Дата осмотра");//date
            tmpExamination->appearance = mAdo->readStr("Внешний вид");//appearance
            
            tmpExamination->conditionEquipment = mAdo->readStr("Состояние оборудования");//conditionEquipment
            tmpExamination->conditionMetal = mAdo->readStr("Состояние металла трубопровода");//conditionEquipment
            tmpExamination->conditionConstruction = mAdo->readStr("Состояние строительных конструкций");//conditionConstruction
            tmpExamination->condition1 = mAdo->readStr("Состояние тепловой изоляции (обратный трубопровод)");
            tmpExamination->condition2 = mAdo->readStr("Состояние тепловой изоляции (подающий трубопровод)");
            tmpExamination->condition3 = mAdo->readStr("Состояние наружного покрытия (обратный трубопровод)");
            tmpExamination->condition4 = mAdo->readStr("Состояние наружного покрытия (подающий трубопровод)");
            tmpExamination->condition5 = mAdo->readStr("Состояние противокоррозионного покрытия (обратный трубопровод)");
            tmpExamination->condition6 = mAdo->readStr("Состояние противокоррозионного покрытия (подающий трубопровод)");
            tmpExamination->respFullName = mAdo->readStr("Отвественное лицо");//respFullName
            tmpExamination->subdivisionName = mAdo->readStr("Подразделение проводившее работу");//subdivisionName
        
            tmpExamination->area = mAdo->readStr("Участок эксплуатации");//area
            tmpExamination->fullName = mAdo->readStr("Начальник участка");//fullName


            mAdo->MoveNext();
            if (mAdo->isEOF())
                tmpExamination->next = nullptr;
            else
                tmpExamination->next = new struct Examination;
            tmpExamination = tmpExamination->next;
            if (!examinationNotEmpty)
                examinationNotEmpty = true;
        }



        tmpExamination = &examination;  /*Приравниваем указатель в начало структуры*/
        if (examinationNotEmpty)
        {
            while (tmpExamination != nullptr) {
                fprintf(g, "<tr>");

                fprintf(g, "<td>%s</td>", tmpExamination->beginNode);/*Начальный узел*/
                fprintf(g, "<td>%s</td>", tmpExamination->endNode);/*Конечный узел*/

                fprintf(g, "<td>%.2f</td>", tmpExamination->diametr);/*Диаметр*/
                fprintf(g, "<td>%s</td>", tmpExamination->sign);/*Признак трубопровода*/
                fprintf(g, "<td>%s</td>", tmpExamination->date);/*Дата*/
                fprintf(g, "<td>%s</td>", tmpExamination->appearance);/*Внешний вид*/
                fprintf(g, "<td>%s</td>", tmpExamination->conditionEquipment);/*Состояние оборудования*/
                fprintf(g, "<td>%s</td>", tmpExamination->conditionMetal);
                fprintf(g, "<td>%s</td>", tmpExamination->conditionConstruction);/*Состояние конструкций*/
                fprintf(g, "<td>%s</td>", tmpExamination->condition1);
                fprintf(g, "<td>%s</td>", tmpExamination->condition2);
                fprintf(g, "<td>%s</td>", tmpExamination->condition3);
                fprintf(g, "<td>%s</td>", tmpExamination->condition4);
                fprintf(g, "<td>%s</td>", tmpExamination->condition5);
                fprintf(g, "<td>%s</td>", tmpExamination->condition6);

                fprintf(g, "<td>%s</td>", tmpExamination->respFullName);/*ФИО ответственного*/
                fprintf(g, "<td>%s</td>", tmpExamination->subdivisionName);/*Подразделение*/

    
                fprintf(g, "<td>%s</td>", tmpExamination->area);/*Участок эксплуатации*/
                fprintf(g, "<td>%s</td>", tmpExamination->fullName);/*ФИО*/

                fprintf(g, "</tr>");

                tmpExamination = tmpExamination->next;

            }
        }

        print_doc_end(g);
    }



}
