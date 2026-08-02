#include <QtGui>
#include <QtWidgets>
#include <gidview/GidWidget.h>

#include <stdio.h>
#include <string.h>

#include <map>

#if 0

FILE* print_doc(FILE* fi, QString fn, QString title, void (*filtr) (QString& s1) = NULL);
void print_doc_end(FILE* f);
QString readQ(QSqlDatabase &db, QString tn, QString tn2, int fileID);
void printTd(QSqlDatabase &db, FILE* f, QString param, const int type, int count = 1);

bool checkFileOpen(QString doc_name) {
/*
    FILE* fOld = fopen(doc_name, "w");
    if (!fOld) {
        AfxMessageBox("Закройте предыдущий паспорт", MB_OK | MB_ICONINFORMATION);
        return false;
    }
    fclose(fOld);
*/
    return true;
}
/*механическое оборудование запорная арматура*/
void printMechanicalZa(FILE* g, FILE* f, int ms_rs, int id, QSqlDatabase &db, QString fragments)
{
    struct Params {
        QString type, design, material, construction, purpose;
    };
    struct Values {
        float diametr;
        int count = 0;
    };
    struct MechEqZA { /*Механическое оборудование запорная арматура*/
        QString beginPts, endPts, pavilion;
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
        if (a.construction == b.construction &&
            a.design == b.design &&
            a.material == b.material &&
            a.purpose == b.purpose &&
            a.type == b.type)
            return true;
        return false;
    };

    auto writeObject = [](int externalID, QString tn, QSqlQuery &query, MechEqZA* mechEqZA) {
        if (tn == "drenazhnyy_kran") {
            if (externalID == 2)
            {
                mechEqZA->valuesDrP.diametr = query.value(8).toDouble();
                mechEqZA->valuesDrP.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 2048;
            }
            else {
                mechEqZA->valuesDrO.diametr = query.value(8).toDouble();
                mechEqZA->valuesDrO.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 1024;
            }
        }
        else if (tn == "zapornaya_armatura_s") {
            if (externalID == 2)
            {
                mechEqZA->valuesZaSP.diametr = query.value(8).toDouble();
                mechEqZA->valuesZaSP.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 512;
            }
            else {
                mechEqZA->valuesZaSO.diametr = query.value(8).toDouble();
                mechEqZA->valuesZaSO.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 256;
            }
        }
        else if (tn == "zapornaya_armatura_r") {
            if (externalID == 2)
            {
                mechEqZA->valuesZaRP.diametr = query.value(8).toDouble();
                mechEqZA->valuesZaRP.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 128;
            }
            else {
                mechEqZA->valuesZaRO.diametr = query.value(8).toDouble();
                mechEqZA->valuesZaRO.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 64;
            }
        }
        else if (tn == "vozdushnik") {
            if (externalID == 2)
            {
                mechEqZA->valuesVozP.diametr = query.value(8).toDouble();
                mechEqZA->valuesVozP.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 32;
            }
            else {
                mechEqZA->valuesVozO.diametr = query.value(8).toDouble();
                mechEqZA->valuesVozO.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 16;
            }
        }

        else if (tn == "drenazhnyy_truboprovod") {
            if (externalID == 2)
            {
                mechEqZA->valuesDTP.diametr = query.value(8).toDouble();
                mechEqZA->valuesDTP.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 8;
            }
            else {
                mechEqZA->valuesDTO.diametr = query.value(8).toDouble();
                mechEqZA->valuesDTO.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 4;
            }
        }
        else if (tn == "peremychki") {
            if (externalID == 2)
            {
                mechEqZA->valuesPrmP.diametr = query.value(8).toDouble();
                mechEqZA->valuesPrmP.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 2;
            }
            else {
                mechEqZA->valuesPrmO.diametr = query.value(8).toDouble();
                mechEqZA->valuesPrmO.count = query.value(9).toInt();
                mechEqZA->fill = mechEqZA->fill | 1;
            }
        }
    };

    MechEqZA mechEqZA; /*Создаем структуру*/
    MechEqZA* tmpMech = &mechEqZA; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f2_1_mechanical_equipment.html", QObject::tr("Ф2_1.Механическое оборудование"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from getPts(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);
        //q = QString("select * from getPts(%d,'%s') order by orderID", 96, "ms");
        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Механическое оборудование)");
            return;
        }

        bool isGeneral = false;
        while (!mAdo->isEOF()) {
            tmpMech->beginPts = query.value(1).toString();
            tmpMech->endPts = query.value(2).toString();
            tmpMech->pavilion = query.value("pavilion").toString();
            int externalId = query.value(3).toInt();
            int externalIdObj = query.value("externalID2").toInt();
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
            QString tn = query.value(0).toString();
            QString purposeType = query.value("purposeType").toString();
            if (tn == "zapornaya_armatura") {
                if (purposeType == "Секционирующая")
                    tn = "zapornaya_armatura_s";
                else
                    tn = "zapornaya_armatura_r";
            }
            switch (externalId)
            {
            case 2:
                tmpMech->paramsP.type = query.value(4).toString();
                tmpMech->paramsP.design = query.value(5).toString();
                tmpMech->paramsP.construction = query.value(6).toString();
                tmpMech->paramsP.material = query.value(7).toString();
                tmpMech->paramsP.purpose = purposeType;
                writeObject(2, tn, mAdo, tmpMech);
                break;
            case 3:
                tmpMech->paramsO.type = query.value(4).toString();
                tmpMech->paramsO.design = query.value(5).toString();
                tmpMech->paramsO.construction = query.value(6).toString();
                tmpMech->paramsO.material = query.value(7).toString();
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
                        && tmpMech->beginPts == next->beginPts
                        && tmpMech->endPts == next->endPts
                        && tmpMech->pavilion == next->pavilion) {
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
                        && tmpMech->beginPts == next->beginPts
                        && tmpMech->endPts == next->endPts
                        && ((tmpMech->fill >> 1) == next->fill) /*сдвигаем fill вправо так подача левее обратки для уравнения их*/
                        && tmpMech->pavilion == next->pavilion) {
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
                        if (tmpMech->beginPts == next->beginPts
                            && tmpMech->endPts == next->endPts
                            && tmpMech->pavilion == next->pavilion)
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
            QString beginPts = tmpMech->beginPts, endPts = tmpMech->endPts;
            MechEqZA* beginGroup = tmpMech;
            while (tmpMech != nullptr) {
                /*формируем массив с последовательным порядком объектов*/
                for (int i = 1; i <= 6; i++) {
                    while (tmpMech != nullptr && tmpMech->beginPts == beginPts && tmpMech->endPts == endPts)
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
                } while (tmpMech != nullptr && tmpMech->beginPts == beginPts && tmpMech->endPts == endPts);
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

void printTubes(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
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
        QString typeTube, typeTubing;
    };
    struct Values {
        double diametr = 0.0, volume = 0.0, lenght = 0.0;
        int thickness = 0;

    };
    struct Tube { /*Механическое оборудование запорная арматура*/
        QString beginPts, endPts;
        Params params;
        Values valueP, valueO;
        Tube* next;
        int fill = 0; /* 2 подача, 1 обратка, 3 все заполнено*/
    };

    auto checkParams = [](Params a, Params b) {
        if (a.typeTube == b.typeTube &&
            a.typeTubing == b.typeTubing)
            return true;
        return false;
    };

    auto writeObject = [&](int externalID, QSqlQuery &query, Tube* tube) {
        double lenght = query.value("l").toDouble();
        double diametr = query.value("d").toDouble();
        if (externalID == 2)
        {
            tube->valueP.diametr = diametr;
            tube->valueP.volume = query.value("v").toDouble();
            tube->valueP.lenght = lenght;
            tube->valueP.thickness = query.value("wall").toInt();
            tube->fill = tube->fill | 2;
            mapDiametrLenght[diametr].lenghtP += lenght;
            lenghtP = lenghtP + lenght;
        }
        else {
            tube->valueO.diametr = diametr;
            tube->valueO.volume = query.value("v").toDouble();
            tube->valueO.lenght = lenght;
            tube->valueO.thickness = query.value("wall").toInt();
            tube->fill = tube->fill | 1;
            mapDiametrLenght[diametr].lenghtO += lenght;
            lenghtO = lenghtO + lenght;
        }

    };

    Tube tube; /*Создаем структуру*/
    Tube* tmpTube = &tube; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "a1-new.html", QObject::tr("Ф1.Трубы"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from getTubesPts(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Трубы)");
            return;
        }

        bool isGeneral = false;
        while (!mAdo->isEOF()) {
            tmpTube->beginPts = query.value("beginPts").toString();
            tmpTube->endPts = query.value("endPts").toString();
            tmpTube->params.typeTube = query.value("kind_tube").toString();
            tmpTube->params.typeTubing = query.value("type_tube").toString();
            int externalId = query.value("externalID").toInt();

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
                        && tmpTube->beginPts == next->beginPts
                        && tmpTube->endPts == next->endPts
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
                        && tmpTube->beginPts == next->beginPts
                        && tmpTube->endPts == next->endPts
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
void printMechanicalKk(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{
    struct Kolodec {
        QString purpose, characteristic, materialL, constructionL, characteristicL;
        int count = 0;
    };
    struct Kompensator {
        float diametr;
        QString construction;
        int count = 0;
    };
    struct MechEqKK { /*Механическое оборудование компенсаторы и колодцы*/
        QString beginPts, endPts, pavilion;
        Kompensator kompensatorP, kompensatorO;
        Kolodec kolodecP, kolodecO, kolodecG;
        MechEqKK* next;
        int fill = 0; /*16 KompP, 8 KompO, 4 KolP, 2 KolO, 1 KolG (31 все заполнено)*/
    };

    auto checkParamsKompensator = [](Kompensator a, Kompensator b) {
        if (a.construction == b.construction &&
            a.diametr == b.diametr)
            return true;
        return false;
    };

    auto checkParamsKolodec = [](Kolodec a, Kolodec b) {
        if (a.purpose == b.purpose &&
            a.characteristic == b.characteristic &&
            a.materialL == b.materialL &&
            a.constructionL == b.constructionL &&
            a.characteristicL == b.characteristicL)
            return true;
        return false;
    };

    auto writeObject = [](int externalID, QString tn, QSqlQuery &query, MechEqKK* mechEqKK) {
        if (tn == "kompensator") {
            if (externalID == 2)
            {
                mechEqKK->kompensatorP.diametr = query.value("diametr_truboprovoda").toDouble();
                mechEqKK->kompensatorP.construction = query.value("constructionTypes").toString();
                mechEqKK->kompensatorP.count += 1;
                mechEqKK->fill = mechEqKK->fill | 16;
            }
            else {
                mechEqKK->kompensatorO.diametr = query.value("diametr_truboprovoda").toDouble();
                mechEqKK->kompensatorO.construction = query.value("constructionTypes").toString();
                mechEqKK->kompensatorO.count += 1;
                mechEqKK->fill = mechEqKK->fill | 8;
            }
        }
        else if (tn == "kolodtsy") {
            if (externalID == 2)
            {
                mechEqKK->kolodecP.characteristic = query.value("characteristicTypes").toString();
                mechEqKK->kolodecP.purpose = query.value("purposeTypes").toString();
                mechEqKK->kolodecP.constructionL = query.value("constructionTypesLyuki").toString();
                mechEqKK->kolodecP.materialL = query.value("materialTypesLyuki").toString();
                mechEqKK->kolodecP.characteristicL = query.value("characteristicTypesLyuki").toString();
                mechEqKK->kolodecP.count += 1;
                mechEqKK->fill = mechEqKK->fill | 4;
            }
            else if (externalID == 3) {
                mechEqKK->kolodecO.characteristic = query.value("characteristicTypes").toString();
                mechEqKK->kolodecO.purpose = query.value("purposeTypes").toString();
                mechEqKK->kolodecO.constructionL = query.value("constructionTypesLyuki").toString();
                mechEqKK->kolodecO.materialL = query.value("materialTypesLyuki").toString();
                mechEqKK->kolodecO.characteristicL = query.value("characteristicTypesLyuki").toString();
                mechEqKK->kolodecO.count += 1;
                mechEqKK->fill = mechEqKK->fill | 2;
            }
            else {
                mechEqKK->kolodecG.characteristic = query.value("characteristicTypes").toString();
                mechEqKK->kolodecG.purpose = query.value("purposeTypes").toString();
                mechEqKK->kolodecG.constructionL = query.value("constructionTypesLyuki").toString();
                mechEqKK->kolodecG.materialL = query.value("materialTypesLyuki").toString();
                mechEqKK->kolodecG.characteristicL = query.value("characteristicTypesLyuki").toString();
                mechEqKK->kolodecG.count += 1;
                mechEqKK->fill = mechEqKK->fill | 1;
            }
        }
    };

    MechEqKK mechEqKK; /*Создаем структуру*/
    MechEqKK* tmpMech = &mechEqKK; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f2_2_mechanical_equipment.html", QObject::tr("Ф2_2.Механическое оборудование"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from getPts_kompensator_kolodtsy(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);
        //q = QString("select * from getPts_kompensator_kolodtsy(%1,'%2','%3') order by orderID", 21, "ms");
        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Механическое оборудование. Компенсаторы и колодцы.)");
            return;
        }

        bool isGeneral = false;
        while (!mAdo->isEOF()) {
            tmpMech->beginPts = query.value("beginNode").toString();
            tmpMech->endPts = query.value("endNode").toString();
            tmpMech->pavilion = query.value("pavilion").toString();
            int externalId = query.value(3).toInt();
            QString tn = query.value("tblName").toString();
            if (externalId == 1 && tn == "kompensator") {
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
                        && tmpMech->beginPts == next->beginPts
                        && tmpMech->endPts == next->endPts
                        && tmpMech->pavilion == next->pavilion) {
                        changePrev = true;
                        tmpMech->kompensatorP.diametr = next->kompensatorP.diametr;
                        tmpMech->kompensatorP.construction = next->kompensatorP.construction;
                        tmpMech->kompensatorP.count = next->kompensatorP.count;
                        tmpMech->fill = tmpMech->fill | 16;
                    }
                    /*если компенсатора обратки нет и следующий элемент это обратка с совпадением данных заполняем*/
                    else if ((tmpMech->fill & 8) == 0
                        && (next->fill & 8) != 0
                        && tmpMech->beginPts == next->beginPts
                        && tmpMech->endPts == next->endPts
                        && tmpMech->pavilion == next->pavilion) {
                        changePrev = true;
                        tmpMech->kompensatorO.diametr = next->kompensatorO.diametr;
                        tmpMech->kompensatorO.construction = next->kompensatorO.construction;
                        tmpMech->kompensatorO.count = next->kompensatorO.count;
                        tmpMech->fill = tmpMech->fill | 8;

                    }
                    /*если колодца подачи нет и следующий элемент это подача с совпадением данных заполняем*/
                    else if ((tmpMech->fill & 4) == 0
                        && (next->fill & 4) != 0
                        && tmpMech->beginPts == next->beginPts
                        && tmpMech->endPts == next->endPts
                        && tmpMech->pavilion == next->pavilion) {
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
                        && tmpMech->beginPts == next->beginPts
                        && tmpMech->endPts == next->endPts) {
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
                        && tmpMech->beginPts == next->beginPts
                        && tmpMech->endPts == next->endPts
                        && tmpMech->pavilion == next->pavilion) {
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
                        if (tmpMech->beginPts == next->beginPts
                            && tmpMech->endPts == next->endPts
                            && tmpMech->pavilion == next->pavilion)
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


void printChannels(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct Channel { /*Каналы*/
        QString beginPts, endPts, typeChannel, typeOverlap;
        double width, height, diametr;
        QString constructionType;
        double lenght;
        QString yearOfFoundation, yearLastRepair;
        QString elementChannel;
        double lenghtRepair;
        QString note;
        Channel* next;
    };

    Channel channel; /*Создаем структуру*/
    Channel* tmpChannel = &channel; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f3_channels.html", QObject::tr("Ф3.Каналы"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from getPts_kanal(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Каналы)");
            return;
        }

        bool channelIsNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpChannel->beginPts = query.value("beginNode").toString();
            tmpChannel->endPts = query.value("endNode").toString();

            tmpChannel->typeChannel = query.value("channelType").toString();
            tmpChannel->typeOverlap = query.value("overlapType").toString();

            tmpChannel->width = query.value("width").toDouble();
            tmpChannel->height = query.value("height").toDouble();

            tmpChannel->diametr = query.value("d_hps").toDouble();
            tmpChannel->constructionType = query.value("constructionType").toString();
            tmpChannel->lenght = query.value("lenChannel").toDouble();

            tmpChannel->yearOfFoundation = query.value("god_vvoda").toString();
            tmpChannel->yearLastRepair = query.value("last_date_remont").toString();
            tmpChannel->elementChannel = query.value("workListChannel").toString();
            tmpChannel->lenghtRepair = query.value("len_channel_remont").toDouble();

            tmpChannel->note = query.value("primechanie").toString();

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


void printChambers(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct Chamber { /*Камеры*/
        QString beginPts, endPts, name, constructionType;
        double height, lenght, width;
        QString typeOverlap, presentOfGratings, yearOfFoundation, yearLastRepair;
        int count;
        QString elementChamber, area, fullName;
        QString balans, note;
        Chamber* next;
    };

    Chamber chamber; /*Создаем структуру*/
    Chamber* tmpChamber = &chamber; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f4_chambers.html", QObject::tr("Ф4.Камеры"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from getPts_tkamera(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Камеры)");
            return;
        }

        bool chambersNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpChamber->beginPts = query.value("beginNode").toString();
            tmpChamber->endPts = query.value("endNode").toString();
            tmpChamber->name = query.value("pavilion").toString();
            tmpChamber->constructionType = query.value("constructionType").toString();
            tmpChamber->height = query.value("height").toDouble();
            tmpChamber->lenght = query.value("lenKamera").toDouble();
            tmpChamber->width = query.value("width").toDouble();
            tmpChamber->typeOverlap = query.value("constructionOverlapType").toString();
            tmpChamber->count = query.value("count_lyukov").toDouble();

            tmpChamber->presentOfGratings = query.value("nalichie_reshetok").toString();
            if (tmpChamber->presentOfGratings == "TRUE")
                tmpChamber->presentOfGratings = "Да";
            else
                tmpChamber->presentOfGratings = "Нет";

            tmpChamber->yearOfFoundation = query.value("god_vvoda").toString();
            tmpChamber->yearLastRepair = query.value("last_date_remont").toString();
            tmpChamber->elementChamber = query.value("workListKamera").toString();
            tmpChamber->area = query.value("naimenovanie_uchastka").toString();
            tmpChamber->fullName = query.value("fio").toString();
            tmpChamber->balans = query.value("organizations").toString();

            tmpChamber->note = query.value("primechanie").toString();

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

                fprintf(g, "<td>%s</td>", tmpChamber->beginPts);/*Начальный ПТС*/
                fprintf(g, "<td>%s</td>", tmpChamber->endPts);/*Конечный ПТС*/

                fprintf(g, "<td>%s</td>", tmpChamber->name);/*Наименование узла*/
                fprintf(g, "<td>%s</td>", tmpChamber->constructionType);/*Конструкция*/
                fprintf(g, "<td>%f</td>", tmpChamber->height); /*высота*/
                fprintf(g, "<td>%f</td>", tmpChamber->lenght); /*длина*/
                fprintf(g, "<td>%f</td>", tmpChamber->width); /*ширина*/
                fprintf(g, "<td>%s</td>", tmpChamber->typeOverlap);/*Тип перекрытия*/
                fprintf(g, "<td>%d</td>", tmpChamber->count);/*Количество люков*/
                fprintf(g, "<td>%s</td>", tmpChamber->presentOfGratings);/*Наличие решеток*/
                fprintf(g, "<td>%s</td>", tmpChamber->yearOfFoundation);/*год ввода*/
                fprintf(g, "<td>%s</td>", tmpChamber->yearLastRepair);/*год последнего ремонта*/
                fprintf(g, "<td>%s</td>", tmpChamber->elementChamber);/*Элементы*/
                fprintf(g, "<td>%s</td>", tmpChamber->area);/*Участок*/
                fprintf(g, "<td>%s</td>", tmpChamber->fullName);/*ФИО*/
                fprintf(g, "<td>%s</td>", tmpChamber->balans);/*Баланс*/
                fprintf(g, "<td>%s</td>", tmpChamber->note);/*Примечание*/


                fprintf(g, "</tr>");

                tmpChamber = tmpChamber->next;

            }
        }

        print_doc_end(g);
    }

}

void printPavilions(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct Pavilion { /*Павильоны*/
        QString name, location, constructionType, area, fullName,
            equipment, firefighting, signaling, lightAvailable,
            schemeAvailable, elementChamber;
        double square;
        QString yearOfFoundation, yearLastRepair;
        QString balans, note;
        Pavilion* next;
    };

    Pavilion pavilion; /*Создаем структуру*/
    Pavilion* tmpPavilion = &pavilion; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f5_pavilions.html", QObject::tr("Ф5.Павильоны"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from getPts_pavilion(%1,'%2','%3')").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Павильоны)");
            return;
        }

        bool pavilionsNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpPavilion->name = query.value("beginNode").toString();
            tmpPavilion->location = query.value("locationType").toString();
            tmpPavilion->constructionType = query.value("constructionType").toString();
            tmpPavilion->square = query.value("s").toDouble();
            tmpPavilion->equipment = query.value("oborudovanie_pavilona").toString();

            tmpPavilion->firefighting = (query.value("sredstva_pozharotushenija").toString() == "TRUE") ? "Да" : "Нет";
            tmpPavilion->signaling = (query.value("signalizacija").toString() == "TRUE") ? "Да" : "Нет";
            tmpPavilion->lightAvailable = (query.value("nalichie_osveshhenija").toString() == "TRUE") ? "Да" : "Нет";
            tmpPavilion->schemeAvailable = (query.value("nalichie_shem_truboprovodov").toString() == "TRUE") ? "Да" : "Нет";

            tmpPavilion->yearOfFoundation = query.value("god_vvoda").toString();
            tmpPavilion->yearLastRepair = query.value("last_date_remont").toString();
            tmpPavilion->elementChamber = query.value("workListPavilion").toString();
            tmpPavilion->area = query.value("naimenovanie_uchastka").toString();
            tmpPavilion->fullName = query.value("fio").toString();
            tmpPavilion->balans = query.value("organizations").toString();

            tmpPavilion->note = query.value("primechanie").toString();

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
                fprintf(g, "<td>%s</td>", tmpPavilion->yearLastRepair);/*год последнего ремонта*/
                fprintf(g, "<td>%s</td>", tmpPavilion->elementChamber);/*Элементы */
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


void printSupports(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct Support { /*Опоры*/
        QString beginNode, endNode, constructionType, supportType;
        double diametr;
        int count;
        QString note;
        Support* next;
    };

    Support support; /*Создаем структуру*/
    Support* tmpSupport = &support; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f6_supports.html", QObject::tr("Ф6.Опоры"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from getPts_opora(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Опоры)");
            return;
        }

        bool supportsNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpSupport->beginNode = query.value("beginNode").toString();
            tmpSupport->endNode = query.value("endNode").toString();
            tmpSupport->diametr = query.value("diameterCondit").toDouble();
            tmpSupport->supportType = query.value("oporaType").toString();
            tmpSupport->constructionType = query.value("constructionType").toString();
            tmpSupport->count = 1;
            tmpSupport->note = query.value("primechanie").toString();

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

                    if (tmpSupport->beginNode == next->beginNode
                        && tmpSupport->endNode == next->endNode
                        && tmpSupport->constructionType == next->constructionType
                        && tmpSupport->supportType == next->supportType
                        && tmpSupport->diametr == next->diametr) {
                        changePrev = true;
                        tmpSupport->count++;
                        if (!tmpSupport->note == next->note && !next->note.IsEmpty())
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


void printSpecConstructions(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct SpecConstruction { /*Спец конструкции*/
        QString beginNode, endNode, description, numberScheme;
        double lenght;
        int count;
        QString note;
        SpecConstruction* next;
    };

    SpecConstruction specConstruction; /*Создаем структуру*/
    SpecConstruction* tmpSpConstr = &specConstruction; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f7_spec_constr.html", QObject::tr("Ф7.Спец.констр."));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from getPts_duker_shield_bridge(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Спец. конструкции)");
            return;
        }

        bool specConstructionNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpSpConstr->beginNode = query.value("beginNode").toString();
            tmpSpConstr->endNode = query.value("endNode").toString();
            tmpSpConstr->lenght = query.value("lenght").toDouble();
            tmpSpConstr->description = query.value("description").toString();
            tmpSpConstr->numberScheme = query.value("nomer_chertezha").toString();
            tmpSpConstr->count = 0;/*query.value("kolichestvo_uporov").toInt();*/
            tmpSpConstr->note = query.value("primechanie").toString();

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

                    if (tmpSpConstr->beginNode == next->beginNode
                        && tmpSpConstr->endNode == next->endNode
                        && tmpSpConstr->description == next->description
                        && tmpSpConstr->numberScheme == next->numberScheme
                        && tmpSpConstr->lenght == next->lenght) {
                        changePrev = true;
                        tmpSpConstr->count++;
                        if (!tmpSpConstr->note == next->note && !next->note.IsEmpty())
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

void printIsolations(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct Isolation { /*Спец конструкции*/
        QString beginNode, endNode, material1, material2, material3;
        double thickness1, thickness2;
        QString note;
        Isolation* next;
    };

    Isolation isolation; /*Создаем структуру*/
    Isolation* tmpIsolation = &isolation; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f8_isolation.html", QObject::tr("Ф8.Изоляция труб"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from getIsolTubesPts(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Изоляция труб)");
            return;
        }

        bool isolationNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpIsolation->beginNode = query.value("beginNode").toString();
            tmpIsolation->endNode = query.value("endNode").toString();
            tmpIsolation->material1 = query.value("isolMaterial").toString();
            tmpIsolation->thickness1 = query.value("isolThickness").toDouble();
            tmpIsolation->material2 = query.value("externalMaterial").toString();
            tmpIsolation->thickness2 = query.value("externCoverThick").toDouble();
            tmpIsolation->material3 = query.value("anticorrMaterial").toString();
            tmpIsolation->note = query.value("primechanie").toString();

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


void printRespPersons(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct RespPerson { /* Ответственные лица*/
        QString nameNetwork, regionMaintenance, areaMaintenance, chiefFio, date, position, fio;
        int number;
        RespPerson* next;
    };

    RespPerson respPerson; /*Создаем структуру*/
    RespPerson* tmpRespPerson = &respPerson; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f9_responsible_persons.html", QObject::tr("Ф9.Ответств.лицо"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        //      q = QString("select * from getPts_responsible_person(%1,'%2','%3')").arg(id).arg(table, fragments);
        q = QString("select * from getPts_responsible_person(%1,'%2')").arg(id).arg(table);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Ответств.лицо)");
            return;
        }

        bool respPersonNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpRespPerson->nameNetwork = query.value("naimenovanie_uchastka").toString();//nameNetwork
            tmpRespPerson->regionMaintenance = query.value("naimenovanie_rayona").toString();//regionMaintenance
            tmpRespPerson->areaMaintenance = query.value("nomer_uchastka").toString();//areaMaintenance
            tmpRespPerson->chiefFio = query.value("fio").toString();//chiefFio
            tmpRespPerson->number = query.value("nomer_prikaza_otv").toInt();//number
            tmpRespPerson->date = query.value("data_prikaza_otv").toString();//date
            tmpRespPerson->position = query.value("otv_dolzhnost").toString();//position
            tmpRespPerson->fio = query.value("otv_fio").toString();//fio

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

void printRemonts(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{
    struct Remont { /*Ремонт*/
        QString beginNode, endNode,
            typeOfWork,dateOfFoundation, dateOfFinish,
            typeStrip;
        double lenghtReplacedTube, recoveryStrip,  diam1ReplacedTube, diam2ReplacedTube, diam3ReplacedTube, thick, recoveryIsolation, asphalt;
        QString workList1, workList2, workList3, number, date;

        QString subdivisionName, respFullName;

        Remont* next;
    };

    Remont remont; /*Создаем структуру*/
    Remont* tmpRemont = &remont; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f10_remont.html", QObject::tr("Ф10.Ремонт"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from  getPts_remont2(%1,'%2','%3')").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Ремонт)");
            return;
        }

        bool remontNotEmpty = false;
        while (!mAdo->isEOF()) {

            tmpRemont->beginNode = query.value("Наименование начального узла").toString();
            tmpRemont->endNode = query.value("Наименование конечного узла").toString();
            tmpRemont->typeOfWork = query.value("Вид ремонта").toString();
            tmpRemont->dateOfFoundation = query.value("Дата начала ремонтных работ").toString();
            tmpRemont->dateOfFinish = query.value("Дата завершения ремонтных работ").toString();
            tmpRemont->typeStrip = query.value("Тип прокладки").toString();

            tmpRemont->lenghtReplacedTube = query.value("Длина заменённой трубы, м").toDouble();
            tmpRemont->recoveryStrip = query.value("Восстановление канальной прокладки, м").toDouble();
            tmpRemont->diam1ReplacedTube = query.value("Диаметр условный, заменённой трубы, м").toDouble();
            tmpRemont->diam2ReplacedTube = query.value("Диаметр внутренний, заменённой трубы, м").toDouble();
            tmpRemont->diam3ReplacedTube = query.value("Диаметр наружный, заменённой трубы, м").toDouble();
            tmpRemont->thick = query.value("Толщина стенки, , заменённой трубы, мм").toDouble();
            tmpRemont->recoveryIsolation = query.value("Восстановление тепловой изоляции поверхности трубы, м2").toDouble();
            tmpRemont->asphalt = query.value("Асфальтирование, ремонт, м2").toDouble();

            tmpRemont->workList1 = query.value("Перечень работ (трубопровода)").toString();
            tmpRemont->workList2 = query.value("Перечень работ (канал)").toString();
            tmpRemont->workList3 = query.value("Перечень работ (камеры)").toString();

            tmpRemont->number = query.value("Номер приказа на ввод в эксплуатацию").toString();
            tmpRemont->date = query.value("Дата приказа ввода в эксплуацию").toString();

            tmpRemont->subdivisionName = query.value("Подразделение производившее ремонт").toString();
            tmpRemont->respFullName = query.value("Ответственный за ремонт").toString();

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

void printDefects(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct Defect { /*Дефект*/
        QString beginNode, endNode, mode, condition,
            date, address, defectDescription,
            viewDefect, categoryDefect, liquidationMethod,
            dateOfFoundation, dateOfFinish, repairTube,
            repairChanel, repairCamera;
        double lenghtReplacedTube, lenghtReplacedIsolation, lenghtRepairArea;
        QString subdivisionName, respPosition, respFullName;
        QString fullName, area;
        QString note;
        Defect* next;
    };

    Defect defect; /*Создаем структуру*/
    Defect* tmpDefect = &defect; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f11_defect.html", QObject::tr("Ф11.Нарушение"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from  getPts_defect(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Дефект)");
            return;
        }

        bool defectNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpDefect->beginNode = query.value("Наименование начального узла").toString();
            tmpDefect->endNode = query.value("Наименование конечного узла").toString();
            tmpDefect->mode = query.value("Режим").toString();
            tmpDefect->condition = query.value("Состояние").toString();
            tmpDefect->date = query.value("Дата обнаружения нарушения").toString();
            tmpDefect->address = query.value("Адрес").toString();
            tmpDefect->defectDescription = query.value("Описание повреждения").toString();

            tmpDefect->viewDefect = query.value("Вид нарушения").toString();
            tmpDefect->categoryDefect = query.value("Категория нарушения").toString();
            tmpDefect->liquidationMethod = query.value("Способ ликвидации нарушения").toString();
            tmpDefect->dateOfFoundation = query.value("Дата начала ремонтных работ").toString();//dateOfFoundation
            tmpDefect->dateOfFinish = query.value("Дата завершения ремонтных работ").toString();//dateOfFinish
            tmpDefect->repairTube = query.value("Ремонт трубопровода и элементов").toString();//


            tmpDefect->lenghtReplacedTube = query.value("Длина заменённой трубы, м").toDouble();//lenghtReplacedTube
            tmpDefect->lenghtReplacedIsolation = query.value("Длина заменённой изоляции, м").toDouble();//lenghtReplacedIsolation
            tmpDefect->repairChanel = query.value("Ремонт канала").toString();
            tmpDefect->repairCamera = query.value("Ремонт камеры").toString();
            tmpDefect->lenghtRepairArea = query.value("Длина участка ремонта канала").toDouble();//lenghtRepairArea

            tmpDefect->subdivisionName = query.value("Подразделение производившего работы").toString();//subdivisionName
            tmpDefect->respPosition = query.value("Должность ответственного").toString();//respPosition
            tmpDefect->respFullName = query.value("ФИО ответственного").toString();//respFullName
            tmpDefect->area = query.value("Участок эксплуатации").toString();//area
            tmpDefect->fullName = query.value("ФИО").toString();//fullName
            tmpDefect->note = query.value("primechanie").toString();//note

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


void printPits(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct Pit { /*Шурфовки*/
        QString beginNode, endNode,
            sign, purpose, address,
            condition, dateBegin, dateFinish,
            results;
        double diametr;
        int number;
        QString subdivisionName, respPosition, respFullName;
        QString fullName, area;
        QString note;
        Pit* next;
    };

    Pit pit; /*Создаем структуру*/
    Pit* tmpPit = &pit; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f12_pits.html", QObject::tr("Ф12.Шурфовки"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from  getPts_shurf(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Шурфовки)");
            return;
        }

        bool pitNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpPit->beginNode = query.value("Наименование начального узла").toString();
            tmpPit->endNode = query.value("Наименование конечного узла").toString();
            tmpPit->sign = query.value("Признак участка трубопровода").toString();
            tmpPit->diametr = query.value("Диаметр трубопровода, мм").toDouble();//diametr
            tmpPit->purpose = query.value("Назначение вскрытия").toString();//purpose
            tmpPit->address = query.value("Адрес").toString();
            tmpPit->condition = query.value("Состояние").toString();
            tmpPit->dateBegin = query.value("Дата начала").toString();
            tmpPit->dateFinish = query.value("Дата окончания").toString();
            tmpPit->number = query.value("Номер акта").toInt();//number
            tmpPit->results = query.value("Результаты осмотра").toString();//results
            tmpPit->note = query.value("Примечание").toString();//note
            tmpPit->respFullName = query.value("ФИО утверждающего").toString();//respFullName
            tmpPit->respPosition = query.value("Должность утверждающего").toString();//respPosition

            tmpPit->subdivisionName = query.value("Служба утверждающего").toString();//subdivisionName
            tmpPit->area = query.value("Участок эксплуатации").toString();//area
            tmpPit->fullName = query.value("ФИО начальника участка").toString();//fullName
            

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


void printClippings(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct Clipping { /*Вырезки*/
        QString beginNode, endNode,
            sign, purpose, address,
            condition, dateBegin, dateFinish,
            results;
        double diametr;
        int number;
        QString subdivisionName, respPosition, respFullName;
        QString fullName, area;
        QString metalCondition, degreeOfOuterCorrosion, degreeOfInnerCorrosion, note;
        Clipping* next;
    };

    Clipping clipping; /*Создаем структуру*/
    Clipping* tmpClipping = &clipping; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f13_clippings.html", QObject::tr("Ф13.Вырезки"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from  getPts_cut_out(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Вырезки)");
            return;
        }

        bool clippingNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpClipping->beginNode = query.value("Наименование начального узла").toString();
            tmpClipping->endNode = query.value("Наименование конечного узла").toString();
            tmpClipping->sign = query.value("Признак участка трубопровода").toString();
            tmpClipping->diametr = query.value("Диаметр трубопровода, мм").toDouble();//diametr
            tmpClipping->purpose = query.value("Назначение вскрытия").toString();//purpose
            tmpClipping->address = query.value("Адрес").toString();
            tmpClipping->condition = query.value("Состояние").toString();
            tmpClipping->dateBegin = query.value("Дата начала").toString();
            tmpClipping->dateFinish = query.value("Дата окончания").toString();
            tmpClipping->number = query.value("Номер акта").toInt();//number
            tmpClipping->results = query.value("Результаты осмотра").toString();//results
            tmpClipping->metalCondition = query.value("Состояние металла трубопровода").toString();//note
            tmpClipping->degreeOfOuterCorrosion = query.value("Cтепень внешней коррозии").toString();//note
            tmpClipping->degreeOfInnerCorrosion = query.value("Степень внутренней коррозии").toString();//note
            tmpClipping->note = query.value("Примечание").toString();//note
            tmpClipping->respFullName = query.value("ФИО утверждающего").toString();//respFullName
            tmpClipping->respPosition = query.value("Должность утверждающего").toString();//respPosition

            tmpClipping->subdivisionName = query.value("Служба утверждающего").toString();//subdivisionName
            tmpClipping->area = query.value("Участок эксплуатации").toString();//area
            tmpClipping->fullName = query.value("ФИО начальника участка").toString();//fullName

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

void printTrials(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct Trial { /*Испытания*/
        QString beginNode, endNode,
            descriptionContour, view, date;
        double p1, p2;
        QString desicion;
        QString subdivisionName, respPosition, respFullName;
        QString fullName, area;
        QString address, description, liquidationMethod;
        Trial* next;
    };

    Trial trial; /*Создаем структуру*/
    Trial* tmpTrial = &trial; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f14_trials.html", QObject::tr("Ф14.Опрессовки"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from  getPts_test(%1,'%2','%3')").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Опрессовки)");
            return;
        }

        bool trialsNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpTrial->beginNode = query.value("Наименование начального узла").toString();
            tmpTrial->endNode = query.value("Наименование конечного узла").toString();

            tmpTrial->descriptionContour = query.value("Описание контура").toString();
            tmpTrial->view = query.value("Вид испытания").toString();
            tmpTrial->date = query.value("Дата проведения опрессовки").toString();
            tmpTrial->p1 = query.value("Давление опрессовки 1 этапа, кгс/см2").toDouble();
            tmpTrial->p2 = query.value("Давление опрессовки 2 этапа, кгс/см2").toDouble();
            tmpTrial->desicion = query.value("Решение комиссии").toString();
            tmpTrial->address = query.value("Адрес нарушения").toString();
            tmpTrial->description = query.value("Описание повреждения").toString();
            tmpTrial->liquidationMethod = query.value("Способ ликвидации нарушения").toString();

            tmpTrial->respFullName = query.value("ФИО руководителя испытаний").toString();
            tmpTrial->respPosition = query.value("Должность руководителя испытаний").toString();
            tmpTrial->subdivisionName = query.value("Подразделение руководителя испытаний").toString();
            
            
            tmpTrial->area = query.value("Участок эксплуатации").toString();//area
            tmpTrial->fullName = query.value("ФИО начальника участка").toString();//fullName
        
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

void printExamination(FILE* g, FILE* f, int ms_rs, int id, QSqlQuery &query, QString fragments)
{

    struct Examination { /*Освидетельствование*/
        QString beginNode, endNode, sign,
            date, appearance, conditionEquipment, 
            conditionMetal, conditionConstruction, 
            condition1, condition2,
            condition3, condition4,
            condition5, condition6;
        double diametr;
        QString subdivisionName, respFullName;
        QString fullName, area;
        Examination* next;
    };

    Examination examination; /*Создаем структуру*/
    Examination* tmpExamination = &examination; /*Приравниваем указатель в начало структуры*/
    /*Заполняем данными из запроса*/
    g = print_doc(f, "f15_examination.html", QObject::tr("Ф15.Осмотр"));
    if (g) {
        QString q, table;

        if (ms_rs == 0)
        {
            table = QString("ms");
        }
        else {
            table = QString("rs");
        }
        q = QString("select * from  getPts_osmotr(%1,'%2','%3') order by orderID").arg(id).arg(table, fragments);

        bool ret = mAdo->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(nullptr, "", "Ошибка в запросе при формировании паспорта (Осмотр)");
            return;
        }

        bool examinationNotEmpty = false;
        while (!mAdo->isEOF()) {
            tmpExamination->beginNode = query.value("Наименование начального узла").toString();
            tmpExamination->endNode = query.value("Наименование конечного узла").toString();
            tmpExamination->diametr = query.value("Диаметр трубопровода, мм").toDouble();
            tmpExamination->sign = query.value("Признак участка трубопровода").toString();
            tmpExamination->date = query.value("Дата осмотра").toString();//date
            tmpExamination->appearance = query.value("Внешний вид").toString();//appearance
            
            tmpExamination->conditionEquipment = query.value("Состояние оборудования").toString();//conditionEquipment
            tmpExamination->conditionMetal = query.value("Состояние металла трубопровода").toString();//conditionEquipment
            tmpExamination->conditionConstruction = query.value("Состояние строительных конструкций").toString();//conditionConstruction
            tmpExamination->condition1 = query.value("Состояние тепловой изоляции (обратный трубопровод)").toString();
            tmpExamination->condition2 = query.value("Состояние тепловой изоляции (подающий трубопровод)").toString();
            tmpExamination->condition3 = query.value("Состояние наружного покрытия (обратный трубопровод)").toString();
            tmpExamination->condition4 = query.value("Состояние наружного покрытия (подающий трубопровод)").toString();
            tmpExamination->condition5 = query.value("Состояние противокоррозионного покрытия (обратный трубопровод)").toString();
            tmpExamination->condition6 = query.value("Состояние противокоррозионного покрытия (подающий трубопровод)").toString();
            tmpExamination->respFullName = query.value("Отвественное лицо").toString();//respFullName
            tmpExamination->subdivisionName = query.value("Подразделение проводившее работу").toString();//subdivisionName
        
            tmpExamination->area = query.value("Участок эксплуатации").toString();//area
            tmpExamination->fullName = query.value("Начальник участка").toString();//fullName


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
#endif