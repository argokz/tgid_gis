#include "stdafx.h"
#include "gid6.h"

#include "gid6Doc.h"
#include "gidrView.h"

#include "win.h"
#include "geofile.h"

#include "maptabs.h"

#include "math.h"


#include "ado.h"


//#include "grid/msword8.h"
#include <fstream>


#include <duckx.hpp>


static COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
using pugi::xml_node;

BSTR getWideString(COleVariant v);

bool checkFileOpen(CString doc_name);

void addParagraph(duckx::Paragraph& ps, BSTR main, vector<BSTR> stringArray, duckx::formatting_flag f);

#include "PropertyGrid\test\PropGridDlg.h"
void InfoAreaDoc(CWnd* wnd)
{
    struct DetailStruct {
        int yearExp;
        int signNumWork;
        int countTR, countKR, countIR, countO;
        CString docA, docTO, docFO;
    };
    struct InfoAreaStruct {
        int numberArea, idArea;
        CString nameArea;
        CString beginArea, endArea;
        double diametr, lenght;
        CString isolation, typeStrip;
        CString yearStrip;
        vector<DetailStruct> detail;
    };



    COleVariant v;
    CString ss;
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("Сведения по участку тепловой сети.docx"));
    CString fname;

    if (!checkFileOpen(doc_name))
        return;

    fname.Format("%s\\excel2\\docx\\empty_doc_horizontal.docx", argpath());

    CopyFile(fname, doc_name, 0);

    CString q, fStr;

    CAdoFile* ado = getAdo(getPsAdoName());
    fStr.Format("%ssql\\docxsql\\defect_doc.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);

        void ispr_q(CString & q);
        ispr_q(q);

        void split_sql_expressions(CString text, std::list<CString> &list_q);

        std::list<CString> list_q;

        split_sql_expressions(q, list_q);

        if (list_q.size() > 1) {
            q = list_q.back();
            list_q.pop_back();

            for (auto& qq : list_q) {
                ado->Execute(qq);
            }
        }

CString split_query(CAdoFile* ado, CString q);

        q = split_query(ado, q);


        //CString myId;
        //myId.Format("%d", id);
        //q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }

        duckx::Document doc(doc_name.GetString());

        doc.open();

        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;

        duckx::formatting_flag fBoldRight = duckx::align_right | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_12 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;


        if (!ado->isEOF()) {
//          doc.paragraphs().insert_paragraph(L"Сведения по участку тепловой сети", fBoldLeft);
        doc.paragraphs().insert_paragraph(L"Нарушения/ремонт трубопроводов тепловой сети", fBoldLeft);
            doc.paragraphs().insert_paragraph(L"");

            duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
            duckx::TableRow rowHeader = table1.insert_row();
            rowHeader.insert_cell(L"№", fBold, 1, 1);
            rowHeader.insert_cell(L"Наименование МС/РС", fBold, 1, 1);
            rowHeader.insert_cell(L"Начало участка", fBold, 1, 1);
            rowHeader.insert_cell(L"Конец участка", fBold, 1, 1);
            rowHeader.insert_cell(L"Условный диаметр, мм", fBold, 1, 1);
            rowHeader.insert_cell(L"Длина, м", fBold, 1, 1);
            rowHeader.insert_cell(L"Вид изоляции", fBold, 1, 1);
            rowHeader.insert_cell(L"Тип прокладки", fBold, 1, 1);
            rowHeader.insert_cell(L"Дата ввода в эксплуатацию (год прокладки)/Дата ввода в эксплуатацию после капитального ремонта (реконструкции)", fBold, 1, 1);
            rowHeader.insert_cell(L"Сведения об эксплуатации участка тепловой сети", fBold, 11);

            duckx::TableRow rowHeader2 = table1.insert_row();
            rowHeader2.insert_cell(L"", fBold, 1, 2);
            rowHeader2.insert_cell(L"", fBold, 1, 2);
            rowHeader2.insert_cell(L"", fBold, 1, 2);
            rowHeader2.insert_cell(L"", fBold, 1, 2);
            rowHeader2.insert_cell(L"", fBold, 1, 2);
            rowHeader2.insert_cell(L"", fBold, 1, 2);
            rowHeader2.insert_cell(L"", fBold, 1, 2);
            rowHeader2.insert_cell(L"", fBold, 1, 2);
            rowHeader2.insert_cell(L"", fBold, 1, 2);

            rowHeader2.insert_cell(L"Год эксплуатации", fBold, 1, 1);
            rowHeader2.insert_cell(L"Наработка основного оборудования", fBold);
            rowHeader2.insert_cell(L"Сведения о ремонтах и обслуживании основного оборудования (текущий ремонт - ТР, капитальный ремонт - КР, инвестиционный ремонт - ИР, опресовка - О), кол-во", fBold, 4);
            rowHeader2.insert_cell(L"Сведения об отказах основного оборудования (авария - А, технологический отказ - ТО, функциональный отказ - ФО)", fBold, 3);

            duckx::TableRow rowHeader3 = table1.insert_row();
            rowHeader3.insert_cell(L"", fBold, 1, 2);
            rowHeader3.insert_cell(L"", fBold, 1, 2);
            rowHeader3.insert_cell(L"", fBold, 1, 2);
            rowHeader3.insert_cell(L"", fBold, 1, 2);
            rowHeader3.insert_cell(L"", fBold, 1, 2);
            rowHeader3.insert_cell(L"", fBold, 1, 2);
            rowHeader3.insert_cell(L"", fBold, 1, 2);
            rowHeader3.insert_cell(L"", fBold, 1, 2);
            rowHeader3.insert_cell(L"", fBold, 1, 2);

            rowHeader3.insert_cell(L"", fBold, 1, 2);

            rowHeader3.insert_cell(L"час", fBold);

            rowHeader3.insert_cell(L"ТР", fBold);
            rowHeader3.insert_cell(L"КР", fBold);
            rowHeader3.insert_cell(L"ИР", fBold);
            rowHeader3.insert_cell(L"О", fBold);

            rowHeader3.insert_cell(L"А", fBold);
            rowHeader3.insert_cell(L"ТО", fBold);
            rowHeader3.insert_cell(L"ФО", fBold);

            duckx::TableRow rowHeader4 = table1.insert_row();
            rowHeader4.insert_cell(L"1", fBold, 1);
            rowHeader4.insert_cell(L"2", fBold, 1);
            rowHeader4.insert_cell(L"3", fBold, 1);
            rowHeader4.insert_cell(L"4", fBold, 1);
            rowHeader4.insert_cell(L"5", fBold, 1);
            rowHeader4.insert_cell(L"6", fBold, 1);
            rowHeader4.insert_cell(L"7", fBold, 1);
            rowHeader4.insert_cell(L"8", fBold, 1);
            rowHeader4.insert_cell(L"9", fBold, 1);
            rowHeader4.insert_cell(L"10", fBold, 1);
            rowHeader4.insert_cell(L"11", fBold, 1);
            rowHeader4.insert_cell(L"12", fBold, 1);
            rowHeader4.insert_cell(L"13", fBold, 1);
            rowHeader4.insert_cell(L"14", fBold, 1);
            rowHeader4.insert_cell(L"15", fBold, 1);
            rowHeader4.insert_cell(L"16", fBold, 1);
            rowHeader4.insert_cell(L"17", fBold, 1);
            rowHeader4.insert_cell(L"18", fBold, 1);


            vector<InfoAreaStruct> data;

            InfoAreaStruct* currentStruct = new InfoAreaStruct();
            currentStruct->idArea = -1;
            currentStruct->yearStrip = "";
            currentStruct->detail.clear();

            DetailStruct* detail = new DetailStruct();
            detail->yearExp = -1;
            detail->docA = "";
            detail->docFO = "";
            detail->docTO = "";
            detail->countIR = 0;

            detail->countTR = 0;
            detail->countO = 0;
            detail->countKR = 0;

            int numberArea = 1;
            while (!ado->isEOF()) {
                int idArea = ado->read_long("pss_id");
                CString yearStrip = ado->readStr("Дата первичного ввода в эксплуатацию");
                int yearExp = ado->read_long("Год эксплуатации");
                if (currentStruct->idArea == -1 && currentStruct->yearStrip.IsEmpty()) {
                    //инициализация участка
                    currentStruct->idArea = idArea;
                    currentStruct->yearStrip = yearStrip;

                    currentStruct->nameArea = ado->readStr("Наименование участка");
                    currentStruct->beginArea = ado->readStr("Наименование начального узла");
                    currentStruct->endArea = ado->readStr("Наименование конечного узла");
                    currentStruct->diametr = ado->read_long("Диаметр");
                    currentStruct->lenght = ado->read_double("Длина участка теплопровода, м");

                    CString isolation1 = ado->readStr("Материал тепловой изоляции");
                    CString isolation2 = ado->readStr("Тип изоляции ППУ");
                    currentStruct->isolation = isolation1 + isolation2;

                    currentStruct->typeStrip = ado->readStr("Тип прокладки");
                    currentStruct->numberArea = numberArea++;
                }
                else if (currentStruct->idArea != idArea) {
                    //иначе если не совпадают участки птс создать новый, старый добавить в data
                    currentStruct->detail.push_back(*detail);
                    data.push_back(*currentStruct);

                    currentStruct = new InfoAreaStruct();
                    currentStruct->idArea = -1;
                    currentStruct->yearStrip = "";
                    currentStruct->detail.clear();

                    detail = new DetailStruct();
                    detail->yearExp = -1;
                    detail->docA = "";
                    detail->docFO = "";
                    detail->docTO = "";
                    detail->countIR = 0;
                    detail->countTR = 0;
                    detail->countO = 0;
                    detail->countKR = 0;

                    //инициализация участка
                    currentStruct->idArea = idArea;
                    currentStruct->yearStrip = yearStrip;

                    currentStruct->nameArea = ado->readStr("Наименование участка");
                    currentStruct->beginArea = ado->readStr("Наименование начального узла");
                    currentStruct->endArea = ado->readStr("Наименование конечного узла");
                    currentStruct->diametr = ado->read_long("Диаметр");
                    currentStruct->lenght = ado->read_double("Длина участка теплопровода, м");

                    CString isolation1 = ado->readStr("Материал тепловой изоляции");
                    CString isolation2 = ado->readStr("Тип изоляции ППУ");
                    currentStruct->isolation = isolation1 + " " + isolation2;

                    currentStruct->typeStrip = ado->readStr("Тип прокладки");
                    currentStruct->numberArea = numberArea++;
                }


                if (detail->yearExp == -1) {

                    //инициализация деталий нарушений по году
                    detail->yearExp = yearExp;
                    detail->signNumWork = ado->read_long("Наработка основного оборудования");

                }
                else if (detail->yearExp != yearExp) {
                    //иначе если не совпадают даты, добавить в массив detail старую и обнулить новую
                    currentStruct->detail.push_back(*detail);

                    detail = new DetailStruct();
                    detail->yearExp = yearExp;
                    detail->signNumWork = ado->read_long("Наработка основного оборудования");
                    detail->docA = "";
                    detail->docFO = "";
                    detail->docTO = "";
                    detail->countIR = 0;
                    detail->countTR = 0;
                    detail->countO = 0;
                    detail->countKR = 0;
                }

                //заполняем текущие данные по нарушениям

                int typeWork = ado->read_long("Вид работ");

                switch (typeWork)
                {
                case 2:detail->countTR++;
                    break;
                case 3:detail->countKR++;
                    break;
                case 4:detail->countIR++;
                    break;
                case 6:detail->countO++;
                    break;
                default:
                    break;
                }

                int typeViolation = ado->read_long("Вид нарушения");

                CString aktNumber = ado->readStr("Номер акта");
                CString dateAkt = ado->readStr("Дата составления акта");
                CString orderNumber = ado->readStr("Номер приказа");
                CString dateOrder = ado->readStr("Дата приказа ввода в эксплуатацию");

                CString resultDoc;
                if (!aktNumber.IsEmpty()) {
                    resultDoc.Format("Акт №%s %s", aktNumber, dateAkt);
                }
                else {
                    if (orderNumber.IsEmpty()) {
                        resultDoc = "-";
                    }
                }
                if (!orderNumber.IsEmpty()) {
                    if (!resultDoc.IsEmpty())
                        resultDoc = resultDoc + "\n";

                    CString ss;
                    ss.Format("Приказ №%s %s", orderNumber, dateOrder);
                    resultDoc = resultDoc + ss;
                }

                if (!resultDoc.IsEmpty())
                    switch (typeViolation)
                    {
                    case 1:
                        if (!detail->docA.IsEmpty())
                            detail->docA = detail->docA + "\n";
                        detail->docA = detail->docA + resultDoc;
                        break;
                    case 2:
                        if (!detail->docTO.IsEmpty())
                            detail->docTO = detail->docTO + "\n";
                        detail->docTO = detail->docTO + resultDoc;
                        break;
                    case 3:
                        if (!detail->docFO.IsEmpty())
                            detail->docFO = detail->docFO + "\n";
                        detail->docFO = detail->docFO + resultDoc;
                        break;
                    default:
                        break;
                    }



                ado->MoveNext();

                if (ado->isEOF())
                {
                    currentStruct->detail.push_back(*detail);
                    data.push_back(*currentStruct);
                }





            }

            //////////////////////////tmp data add
            /*currentStruct = new InfoAreaStruct();
            currentStruct->idArea = 334455;
            currentStruct->yearStrip = "12.03.2009";
            currentStruct->detail.clear();

            for (int i = 0; i < 5; i++) {
                detail = new DetailStruct();
                detail->yearExp = 2016+i;
                detail->docA = "Акт №123 12.03.2017";
                detail->docFO = "Приказ №124 12.01.2017";
                detail->docTO = "Акт №123 12.03.2017\nПриказ №21 11.12.2016";
                detail->countIR = 2;

                detail->countTR = 1;
                detail->countO = 0;
                detail->countKR = 2;
                currentStruct->detail.push_back(*detail);
            }
            data.push_back(*currentStruct);*/
            //////////////////////////tmp data added

            for (int i = 0; i < data.size(); i++) {
                CString ss;
                duckx::TableRow row = table1.insert_row();
                InfoAreaStruct* currentStruct = &data[i];
                int sizeDetail = currentStruct->detail.size();
                int rowVertical = sizeDetail > 1 ? 1 : -1;
                int widthRow = 1;

                ss.Format("%i", currentStruct->numberArea);
                row.insert_cell(ss.AllocSysString(), fNorm, widthRow, rowVertical);

                //id

                row.insert_cell(currentStruct->nameArea.AllocSysString(), fNorm, widthRow, rowVertical);

                row.insert_cell(currentStruct->beginArea.AllocSysString(), fNorm, widthRow, rowVertical);
                row.insert_cell(currentStruct->endArea.AllocSysString(), fNorm, widthRow, rowVertical);

                ss.Format("%.2f", currentStruct->diametr);
                row.insert_cell(ss.AllocSysString(), fNorm, widthRow, rowVertical);
                ss.Format("%.2f", currentStruct->lenght);
                row.insert_cell(ss.AllocSysString(), fNorm, widthRow, rowVertical);

                row.insert_cell(currentStruct->isolation.AllocSysString(), fNorm, widthRow, rowVertical);
                row.insert_cell(currentStruct->typeStrip.AllocSysString(), fNorm, widthRow, rowVertical);

                //year
                row.insert_cell(currentStruct->yearStrip.AllocSysString(), fNorm, widthRow, rowVertical);

                for (int j = 0; j < sizeDetail; j++) {

                    DetailStruct* detailStruct = &currentStruct->detail[j];

                    ss.Format("%i", detailStruct->yearExp);
                    row.insert_cell(ss.AllocSysString(), fNorm, 1);

                    ss.Format("%i", detailStruct->signNumWork);
                    row.insert_cell(ss.AllocSysString(), fNorm);

                    ss.Format("%i", detailStruct->countTR);
                    row.insert_cell(ss.AllocSysString(), fNorm);
                    ss.Format("%i", detailStruct->countKR);
                    row.insert_cell(ss.AllocSysString(), fNorm);
                    ss.Format("%i", detailStruct->countIR);
                    row.insert_cell(ss.AllocSysString(), fNorm);
                    ss.Format("%i", detailStruct->countO);
                    row.insert_cell(ss.AllocSysString(), fNorm);

                    row.insert_cell(detailStruct->docA.AllocSysString(), fNorm);
                    row.insert_cell(detailStruct->docTO.AllocSysString(), fNorm);
                    row.insert_cell(detailStruct->docFO.AllocSysString(), fNorm);

                    if (j != sizeDetail - 1) {
                        row = table1.insert_row();

                        row.insert_cell(L"", fNorm, widthRow, 2);
                        row.insert_cell(L"", fNorm, widthRow, 2);
                        row.insert_cell(L"", fNorm, widthRow, 2);
                        row.insert_cell(L"", fNorm, widthRow, 2);
                        row.insert_cell(L"", fNorm, widthRow, 2);
                        row.insert_cell(L"", fNorm, widthRow, 2);
                        row.insert_cell(L"", fNorm, widthRow, 2);
                        row.insert_cell(L"", fNorm, widthRow, 2);
                        row.insert_cell(L"", fNorm, widthRow, 2);
                    }
                }
            }
            doc.save();

            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);

        }
        else {
            AfxMessageBox("Нет данных");
        }
    }
}
