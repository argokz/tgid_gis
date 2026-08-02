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
void addParagraph(duckx::Paragraph& ps, BSTR main, vector<BSTR> stringArray, duckx::formatting_flag f);
void addParagraph2(duckx::Paragraph& ps, BSTR main, vector<pair<BSTR, bool>> stringArray, duckx::formatting_flag f);
bool checkFileOpen(CString doc_name);

#include "PropertyGrid\test\PropGridDlg.h"

void OpresTable1(CPropertyGrid* wnd, int id, duckx::Document* myDoc = nullptr) {

    CAdoFile* ado = getAdo(getPsAdoName());
    CString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc.paragraphs().insert_paragraph(L"Табл.1 Фрагменты магистральных тепловых сетей контура опрессовки", fBoldLeft);
        doc.paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
        duckx::TableRow rowHeader = table1.insert_row().set_child(L"w:tblHeader");
        rowHeader.insert_cell(L"Наименование/Адрес контура", fBold);
        rowHeader.insert_cell(L"Магистральная сеть(МС)", fBold);
        rowHeader.insert_cell(L"Район эксплуатации", fBold);
        rowHeader.insert_cell(L"Участок эксплуатации", fBold);
        rowHeader.insert_cell(L"Объём трубопроводов контура, м3", fBold);
        rowHeader.insert_cell(L"Начальник эксплуатационного участка", fBold);

        while (!ado->isEOF()) {

            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(getWideString(ado->read("Наименование/Адрес контура")), fNormLeft);

            row1.insert_cell(getWideString(ado->read("Магистральная сеть(МС)")), fNormLeft);

            row1.insert_cell(getWideString(ado->read("Район эксплуатации")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Участок эксплуатации")), fNormLeft);
            /*ss.Format("%.2f", ado->read("Объём трубопроводов по участку, м3").dblVal);
            row1.insert_cell(ss.AllocSysString(), fNormLeft);*/
            ss.Format("%.2f", ado->read("Объём трубопроводов контура, м3").dblVal);
            row1.insert_cell(ss.AllocSysString(), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Начальник участка")), fNormLeft);
            ado->MoveNext();
        }
    };

    COleVariant v;
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("Фрагменты магистральных тепловых сетей контура опрессовки.docx"));
    CString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s\\excel2\\docx\\empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    CString q, fStr;


    fStr.Format("%ssql\\docxsql\\opres_t1.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        CString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc.open();
            drawTable(doc);
            doc.save();
            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
        }
        else {
            drawTable(*myDoc);
        }


    }
}


void OpresTable2(CPropertyGrid* wnd, int id, duckx::Document* myDoc = nullptr) {

    CAdoFile* ado = getAdo(getPsAdoName());
    CString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc.paragraphs().insert_paragraph(L"Табл.2 Фрагменты распределительных тепловых сетей контура опрессовки", fBoldLeft);
        doc.paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
        duckx::TableRow rowHeader = table1.insert_row().set_child(L"w:tblHeader");
        rowHeader.insert_cell(L"Наименование/Адрес контура", fBold);
        rowHeader.insert_cell(L"Распределительная сеть (РС)", fBold);
        rowHeader.insert_cell(L"Район эксплуатации", fBold);
        rowHeader.insert_cell(L"Участок эксплуатации", fBold);
        rowHeader.insert_cell(L"Объём трубопроводов контура, м3", fBold);
        rowHeader.insert_cell(L"Начальник эксплуатационного участка", fBold);

        while (!ado->isEOF()) {

            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(getWideString(ado->read("Наименование/Адрес контура")), fNormLeft);

            row1.insert_cell(getWideString(ado->read("Распределительная сеть (РС)")), fNormLeft);

            row1.insert_cell(getWideString(ado->read("Район эксплуатации")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Участок эксплуатации")), fNormLeft);
            /*ss.Format("%.2f", ado->read("Объём трубопроводов по участку, м3").dblVal);
            row1.insert_cell(ss.AllocSysString(), fNormLeft);*/
            ss.Format("%.2f", ado->read("Объём трубопроводов контура, м3").dblVal);
            row1.insert_cell(ss.AllocSysString(), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Начальник участка")), fNormLeft);
            ado->MoveNext();
        }
    };

    COleVariant v;
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("Фрагменты распределительных тепловых сетей контура опрессовки.docx"));
    CString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s\\excel2\\docx\\empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    CString q, fStr;


    fStr.Format("%ssql\\docxsql\\opres_t2.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        CString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc.open();
            drawTable(doc);
            doc.save();
            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
        }
        else {
            drawTable(*myDoc);
        }


    }
}

void OpresTable3(CPropertyGrid* wnd, int id, duckx::Document* myDoc = nullptr) {

    CAdoFile* ado = getAdo(getPsAdoName());
    CString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc.paragraphs().insert_paragraph(L"Табл.3 Граница раздела контура опрессовки", fBoldLeft);
        doc.paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
        duckx::TableRow rowHeader = table1.insert_row().set_child(L"w:tblHeader");
        rowHeader.insert_cell(L"Узел", fBold);
        rowHeader.insert_cell(L"Признак трубопровода", fBold);
        rowHeader.insert_cell(L"Состояние", fBold);
        rowHeader.insert_cell(L"Номер", fBold);
        rowHeader.insert_cell(L"Диаметр условный, мм", fBold);
        rowHeader.insert_cell(L"Примечание", fBold);

        while (!ado->isEOF()) {

            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(getWideString(ado->read("Узел")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Признак трубопровода")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Состояние")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Номер")), fNormLeft);
            ss.Format("%.2f", ado->read("Диаметр условный, мм").dblVal);
            row1.insert_cell(ss.AllocSysString(), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Примечание")), fNormLeft);


            ado->MoveNext();
        }
    };

    COleVariant v;
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("Граница раздела контура опрессовки.docx"));
    CString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s\\excel2\\docx\\empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    CString q, fStr;


    fStr.Format("%ssql\\docxsql\\granitsa_razdela_kontura_opessovki.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        CString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc.open();
            drawTable(doc);
            doc.save();
            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
        }
        else {
            drawTable(*myDoc);
        }


    }
}


void OpresTable4(CPropertyGrid* wnd, int id, duckx::Document* myDoc = nullptr) {

    CAdoFile* ado = getAdo(getPsAdoName());
    CString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc.paragraphs().insert_paragraph(L"Табл.4 План установки манометров", fBoldLeft);
        doc.paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
        duckx::TableRow rowHeader = table1.insert_row().set_child(L"w:tblHeader");
        rowHeader.insert_cell(L"Наименование/Адрес контура", fBold);
        rowHeader.insert_cell(L"Наименование тепловой сети МС/РС", fBold);
        rowHeader.insert_cell(L"Район эксплуатации", fBold);
        rowHeader.insert_cell(L"Камера", fBold);
        rowHeader.insert_cell(L"Признак трубопровода", fBold);
        rowHeader.insert_cell(L"Количество", fBold);
        rowHeader.insert_cell(L"ФИО ответственного за установку манометров и расходомеров", fBold);
        rowHeader.insert_cell(L"Должность ответственного за установку манометров и расходомеров", fBold);
        rowHeader.insert_cell(L"Подразделение  ответственного за установку манометров и расходомеров", fBold);

        while (!ado->isEOF()) {

            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(getWideString(ado->read("Наименование/Адрес контура")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Наименование тепловой сети МС/РС")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Район эксплуатации")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Камера")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Признак трубопровода")), fNormLeft);
            ss.Format("%i", ado->read("Количество").lVal);
            row1.insert_cell(ss.AllocSysString(), fNormLeft);
            row1.insert_cell(getWideString(ado->read("ФИО ответственного за установку манометров и расходомеров")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Должность ответственного за установку манометров и расходомеров")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Подразделение  ответственного за установку манометров и расходомеров")), fNormLeft);



            ado->MoveNext();
        }
    };

    COleVariant v;
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("План установки манометров.docx"));
    CString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s\\excel2\\docx\\empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    CString q, fStr;


    fStr.Format("%ssql\\docxsql\\plan_manometrov.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        CString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc.open();
            drawTable(doc);
            doc.save();
            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
        }
        else {
            drawTable(*myDoc);
        }


    }
}

void OpresTable5(CPropertyGrid* wnd, int id, duckx::Document* myDoc = nullptr) {

    CAdoFile* ado = getAdo(getPsAdoName());
    CString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc.paragraphs().insert_paragraph(L"Табл.5 План установки расходомеров", fBoldLeft);
        doc.paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
        duckx::TableRow rowHeader = table1.insert_row().set_child(L"w:tblHeader");
        rowHeader.insert_cell(L"Наименование/Адрес контура", fBold);
        rowHeader.insert_cell(L"Наименование тепловой сети МС/РС", fBold);
        rowHeader.insert_cell(L"Район эксплуатации", fBold);
        rowHeader.insert_cell(L"Камера", fBold);
        rowHeader.insert_cell(L"Признак трубопровода", fBold);
        rowHeader.insert_cell(L"Количество", fBold);
        rowHeader.insert_cell(L"ФИО ответственного за установку манометров и расходомеров", fBold);
        rowHeader.insert_cell(L"Должность ответственного за установку манометров и расходомеров", fBold);
        rowHeader.insert_cell(L"Подразделение  ответственного за установку манометров и расходомеров", fBold);

        while (!ado->isEOF()) {

            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(getWideString(ado->read("Наименование/Адрес контура")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Наименование тепловой сети МС/РС")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Район эксплуатации")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Камера")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Признак трубопровода")), fNormLeft);
            ss.Format("%i", ado->read("Количество").lVal);
            row1.insert_cell(ss.AllocSysString(), fNormLeft);
            row1.insert_cell(getWideString(ado->read("ФИО ответственного за установку манометров и расходомеров")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Должность ответственного за установку манометров и расходомеров")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Подразделение  ответственного за установку манометров и расходомеров")), fNormLeft);



            ado->MoveNext();
        }
    };

    COleVariant v;
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("План установки расходомеров.docx"));
    CString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s\\excel2\\docx\\empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    CString q, fStr;


    fStr.Format("%ssql\\docxsql\\plan_rashodomerov.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        CString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc.open();
            drawTable(doc);
            doc.save();
            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
        }
        else {
            drawTable(*myDoc);
        }


    }
}

void OpresTable7(CPropertyGrid* wnd, int id, duckx::Document* myDoc = nullptr) {

    CAdoFile* ado = getAdo(getPsAdoName());
    CString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc.paragraphs().insert_paragraph(L"Табл.7 Список подключаемых потребителей к тепловой сети (контуру) опрессовки", fBoldLeft);
        doc.paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
        duckx::TableRow rowHeader = table1.insert_row().set_child(L"w:tblHeader");
        rowHeader.insert_cell(L"Наименование/Адрес контура", fBold);
        rowHeader.insert_cell(L"Наименование тепловой сети МС/РС", fBold);
        rowHeader.insert_cell(L"Район эксплуатации", fBold);
        rowHeader.insert_cell(L"Наименование узла присоединения потребителя", fBold);
        rowHeader.insert_cell(L"Адрес", fBold);
        rowHeader.insert_cell(L"Вид потребителя", fBold);
        rowHeader.insert_cell(L"Дата начала опрессовки", fBold);
        rowHeader.insert_cell(L"Дата окончания опрессовки", fBold);
        rowHeader.insert_cell(L"Расчётная нагрузка, Гкал/ч", fBold);

        while (!ado->isEOF()) {

            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(getWideString(ado->read("Наименование/Адрес контура")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Наименование тепловой сети МС/РС")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Район эксплуатации")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Наименование узла присоединения потребителя")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Адрес")), fNormLeft);

            row1.insert_cell(getWideString(ado->read("Вид потребителя")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Дата начала опрессовки")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Дата окончания опрессовки")), fNormLeft);
            ss.Format("%f", ado->read("Расчётная нагрузка, Гкал/ч").dblVal);
            row1.insert_cell(ss.AllocSysString(), fNormLeft);



            ado->MoveNext();
        }
    };

    COleVariant v;
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("Список подключаемых потребителей к тепловой сети (контуру) опрессовки.docx"));
    CString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s\\excel2\\docx\\empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    CString q, fStr;


    fStr.Format("%ssql\\docxsql\\spisok_podkl_pootreb.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        CString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc.open();
            drawTable(doc);
            doc.save();
            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
        }
        else {
            drawTable(*myDoc);
        }


    }
}

void OpresTable9(CPropertyGrid* wnd, int id, duckx::Document* myDoc = nullptr) {

    CAdoFile* ado = getAdo(getPsAdoName());
    CString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc.paragraphs().insert_paragraph(L"Табл.9 Список нарушений", fBoldLeft);
        doc.paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
        duckx::TableRow rowHeader1 = table1.insert_row().set_child(L"w:tblHeader");
        rowHeader1.insert_cell(L"Участок трассы", fBold, 2);
        rowHeader1.insert_cell(L"Наименование объекта (адрес)", fBold, 1, 1);
        rowHeader1.insert_cell(L"Вид нарушения", fBold, 1, 1);
        rowHeader1.insert_cell(L"Категория нарушения", fBold, 1, 1);
        rowHeader1.insert_cell(L"Характер повреждения", fBold, 1, 1);
        rowHeader1.insert_cell(L"Описание повреждения", fBold, 1, 1);
        rowHeader1.insert_cell(L"Дата начала работ", fBold, 1, 1);
        rowHeader1.insert_cell(L"Дата окончания работ", fBold, 1, 1);
        rowHeader1.insert_cell(L"Способ ликвидации нарушений", fBold, 1, 1);

        duckx::TableRow rowHeader2 = table1.insert_row(1800);
        rowHeader2.insert_cell(L"Начальный узел", fBold | duckx::vertical_aligment_text);
        rowHeader2.insert_cell(L"Конечный узел", fBold | duckx::vertical_aligment_text);
        rowHeader2.insert_cell(L"", fBold, 1, 2);
        rowHeader2.insert_cell(L"", fBold, 1, 2);
        rowHeader2.insert_cell(L"", fBold, 1, 2);
        rowHeader2.insert_cell(L"", fBold, 1, 2);
        rowHeader2.insert_cell(L"", fBold, 1, 2);
        rowHeader2.insert_cell(L"", fBold, 1, 2);
        rowHeader2.insert_cell(L"", fBold, 1, 2);
        rowHeader2.insert_cell(L"", fBold, 1, 2);

        duckx::TableRow rowHeader3 = table1.insert_row();
        rowHeader3.insert_cell(L"1", fBold);
        rowHeader3.insert_cell(L"2", fBold);
        rowHeader3.insert_cell(L"3", fBold);
        rowHeader3.insert_cell(L"4", fBold);
        rowHeader3.insert_cell(L"5", fBold);
        rowHeader3.insert_cell(L"6", fBold);
        rowHeader3.insert_cell(L"7", fBold);
        rowHeader3.insert_cell(L"8", fBold);
        rowHeader3.insert_cell(L"9", fBold);
        rowHeader3.insert_cell(L"10", fBold);




        while (!ado->isEOF()) {

            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(getWideString(ado->read("Наименование начального узла")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Наименование конечного узла")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Адрес")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Вид нарушения")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Категория нарушения")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Характер повреждения")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Описание повреждения")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Дата начала ремонтных работ")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Дата завершения ремонтных работ")), fNormLeft);
            row1.insert_cell(getWideString(ado->read("Способ ликвидации нарушения")), fNormLeft);


            ado->MoveNext();
        }
    };

    COleVariant v;
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("Список нарушений.docx"));
    CString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s\\excel2\\docx\\empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    CString q, fStr;


    fStr.Format("%ssql\\docxsql\\opres_defect_list.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        CString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc.open();
            drawTable(doc);
            doc.save();
            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
        }
        else {
            drawTable(*myDoc);
        }


    }
}

void OpresTableAkt(CPropertyGrid* wnd, int id) {
    struct Chief {
        CString chief, area, name, sign;
    };
    struct WarmNet {
        CString name, node1, node2, sign;
    };

    COleVariant v;
    CString ss;
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("АКТ На проведение гидравлических испытаний тепломагистралей.docx"));
    CString fname;

    if (!checkFileOpen(doc_name))
        return;

    fname.Format("%s\\excel2\\docx\\empty_doc_horizontal.docx", argpath());

    CopyFile(fname, doc_name, 0);

    CString q, fStr;

    CAdoFile* ado = getAdo(getPsAdoName());
    fStr.Format("%ssql\\docxsql\\opres_journal.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        CString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }

        duckx::Document doc(doc_name.GetString());

        doc.open();

        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fBoldRight = duckx::align_right | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_12 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;

        if (!ado->isEOF()) {
            doc.paragraphs().insert_paragraph(L"«Утверждаю»", fBoldRight);
            doc.paragraphs().insert_paragraph(getWideString(ado->read("Должность утвердаещего")), fBoldRight);
            doc.paragraphs().insert_paragraph(getWideString(ado->read("Подразделение утверждающего")), fBoldRight);
            doc.paragraphs().insert_paragraph(getWideString(ado->read("ФИО утверждающего")), fBoldRight);
            doc.paragraphs().insert_paragraph(L"«____» _______________ 20__г.", fBoldRight);
            doc.paragraphs().insert_paragraph(L"", fBold);
            doc.paragraphs().insert_paragraph(getWideString(ado->read("Дата утверждения акта испытаний")), fBoldRight);

            for (int i = 0; i < 3; i++)
                doc.paragraphs().insert_paragraph(L"", fBold);
            doc.paragraphs().insert_paragraph(L"АКТ", fBold);
            doc.paragraphs().insert_paragraph(L"На проведение гидравлических", fBold);
            doc.paragraphs().insert_paragraph(L"испытаний тепломагистралей", fBold);
            doc.paragraphs().insert_paragraph(L"", fBold);

            double d2 = ado->read_double("Давление опрессовки 2 этапа, кгс/см2");
            long cont = ado->read_long("Продолжительность опрессовки");
            CString desicion = ado->readStr("Решение комиссии");

            CString position1 = ado->readStr("Должность руководителя испытаний");
            CString subdivision1 = ado->readStr("Подразделение руководителя испытаний");
            CString chief1 = ado->readStr("ФИО руководителя испытаний");

            CString position2 = ado->readStr("Должность ответственного по СНиП контура источника тепла");
            CString subdivision2 = ado->readStr("Подразделение ответственного по СНиП контура источника тепла");
            CString chief2 = ado->readStr("ФИО ответственного по СНиП контура источника тепла");

            doc.paragraphs().insert_paragraph(L"Комиссия в составе:", fNormLeft);

            duckx::Table table1 = doc.tables().insert_table(duckx::align_left);
            duckx::TableRow rowHeader1 = table1.insert_row();
            rowHeader1.insert_cell(L"Руководителя испытаний:", fNormLeft);
            rowHeader1.insert_cell(position1.AllocSysString(), fNormLeft);
            rowHeader1.insert_cell(subdivision1.AllocSysString(), fNormLeft);
            rowHeader1.insert_cell(chief1.AllocSysString(), fNormLeft);

            duckx::TableRow rowHeader2 = table1.insert_row();
            rowHeader2.insert_cell(L"Отв. контура источника тепла:", fNormLeft);
            rowHeader2.insert_cell(position2.AllocSysString(), fNormLeft);
            rowHeader2.insert_cell(subdivision2.AllocSysString(), fNormLeft);
            rowHeader2.insert_cell(chief2.AllocSysString(), fNormLeft);


            vector<Chief> chiefData;
            fStr.Format("%ssql\\docxsql\\ms_rs_opres_fragments_by_nu.sql", argpath());
            ifstream f(fStr);
            q = readFile(fStr);
            myId.Format("%i", id);
            q.Replace("$fileID$", myId);
            bool ret = ado->openTable0(q);
            if (!ret)
            {
                AfxMessageBox("Ошибка в запросе");
                return;
            }
            while (!ado->isEOF()) {
                Chief* myChief = new Chief();
                myChief->chief = ado->readStr("Начальник участка");
                myChief->name = ado->readStr("Наименование участка");
                myChief->area = ado->readStr("Район эксплуатации");
                myChief->sign = ado->readStr("Признак сети");
                chiefData.push_back(*myChief);
                ado->MoveNext();
            }

            vector<WarmNet> warmNetData;
            fStr.Format("%ssql\\docxsql\\opres_pl.sql", argpath());
            ifstream f2(fStr);
            q = readFile(fStr);
            myId.Format("%i", id);
            q.Replace("$fileID$", myId);
            ret = ado->openTable0(q);
            if (!ret)
            {
                AfxMessageBox("Ошибка в запросе");
                return;
            }
            while (!ado->isEOF()) {
                WarmNet* myWarmNet = new WarmNet();
                myWarmNet->name = ado->readStr("Наименование участка");
                myWarmNet->node1 = ado->readStr("Наименование начального узла");
                myWarmNet->node2 = ado->readStr("Наименование конечного узла");
                myWarmNet->sign = ado->readStr("Признак сети");
                warmNetData.push_back(*myWarmNet);
                ado->MoveNext();
            }

            for (int i = 0; i < chiefData.size(); i++) {
                duckx::TableRow rowHeaderChief = table1.insert_row();
                rowHeaderChief.insert_cell(L"начальника", fNormLeft);
                rowHeaderChief.insert_cell(chiefData[i].name.AllocSysString(), fNormLeft);
                rowHeaderChief.insert_cell(chiefData[i].area.AllocSysString(), fNormLeft);
                rowHeaderChief.insert_cell(chiefData[i].chief.AllocSysString(), fNormLeft);
            }

            doc.paragraphs().insert_paragraph(L"", fBold);
            doc.paragraphs().insert_paragraph(L"составили настоящий акт о проведении гидравлического испытания:", fNormLeft);

            duckx::Table table2 = doc.tables().insert_table(duckx::align_left);
            for (int i = 0; i < warmNetData.size(); i++) {
                duckx::TableRow rowHeader1T2 = table2.insert_row();

                CString s = warmNetData[i].node1 + " - " + warmNetData[i].node2;
                rowHeader1T2.insert_cell(s.AllocSysString(), fNormLeft);

//                rowHeader1T2.insert_cell(warmNetData[i].name.AllocSysString(), fNormLeft);
                CString ss;
                ss.Format("(%s)", warmNetData[i].sign);
                rowHeader1T2.insert_cell(ss.AllocSysString(), fNormLeft);
            }

            doc.paragraphs().insert_paragraph(L"", fBold);
            CString ss;
            ss.Format("давлением %.2f кгс/см2 в течение %i минут с полным наружным осмотром.", d2, cont);
            doc.paragraphs().insert_paragraph(ss.AllocSysString(), fNormLeft);
            doc.paragraphs().insert_paragraph(L"", fNormLeft);
            ss.Format("Решение комиссии: %s", desicion);
            doc.paragraphs().insert_paragraph(ss.AllocSysString(), fNormLeft);
            doc.paragraphs().insert_paragraph(L"", fNormLeft);

            duckx::Table table3 = doc.tables().insert_table(duckx::align_left);
            duckx::TableRow rowHeader1T3 = table3.insert_row();
        
            rowHeader1T3.insert_cell(position1.AllocSysString(), fNormLeft);
            rowHeader1T3.insert_cell(subdivision1.AllocSysString(), fNormLeft);
            rowHeader1T3.insert_cell(L"_________", fNormLeft);
            rowHeader1T3.insert_cell(chief1.AllocSysString(), fNormLeft);

            duckx::TableRow rowHeader2T3 = table3.insert_row();
    
            rowHeader2T3.insert_cell(position2.AllocSysString(), fNormLeft);
            rowHeader2T3.insert_cell(subdivision2.AllocSysString(), fNormLeft);
            rowHeader2T3.insert_cell(L"_________", fNormLeft);
            rowHeader2T3.insert_cell(chief2.AllocSysString(), fNormLeft);

            for (int i = 0; i < chiefData.size(); i++) {
                duckx::TableRow rowHeaderChief = table3.insert_row();
                CString ss;
                ss.Format("начальник %s", chiefData[i].name);
                rowHeaderChief.insert_cell(ss.AllocSysString(), fNormLeft);
                rowHeaderChief.insert_cell(chiefData[i].area.AllocSysString(), fNormLeft);
                rowHeaderChief.insert_cell(L"_________", fNormLeft);
                rowHeaderChief.insert_cell(chiefData[i].chief.AllocSysString(), fNormLeft);
            }
            doc.paragraphs().insert_next_page();
            doc.paragraphs().insert_paragraph(L"Приложение", fBoldRight);
            doc.paragraphs().insert_paragraph(L"", fNormLeft);
        }
        OpresTable9(wnd, id, &doc);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);
        OpresTable1(wnd, id, &doc);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);
        OpresTable2(wnd, id, &doc);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);
        OpresTable3(wnd, id, &doc);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);
        OpresTable4(wnd, id, &doc);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);
        OpresTable5(wnd, id, &doc);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);
        OpresTable7(wnd, id, &doc);

        doc.save();

        HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
    }
}