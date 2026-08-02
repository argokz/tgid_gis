#include <QtGui>
#include <QtWidgets>
#include <gidview/GidWidget.h>

#include <property/PropertyDial.h>
#include <db/db.h>


//#include "grid/msword8.h"
#include <fstream>

//#include "InputDia.h"

#include <lib/DuckX/src/duckx.hpp>

duckx::Document * open_docx(QString doc_name);


//static COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
using pugi::xml_node;

//BSTR getWideString(COleVariant v);
//void addParagraph(duckx::Paragraph& ps, BSTR main, vector<BSTR> stringArray, duckx::formatting_flag f);
//void addParagraph2(duckx::Paragraph& ps, BSTR main, vector<pair<BSTR, bool>> stringArray, duckx::formatting_flag f);
//bool checkFileOpen(QString doc_name);

//#include "PropertyGrid\test\PropGridDlg.h"

void show_docx(QWidget* wnd, duckx::Document *doc);


bool CopyFile(QString fn1, QString fn2, bool bFailIfExists)
{
    bool ex2 = QFile::exists(fn2);

    if (bFailIfExists && ex2) {
        return false;
    }

    if (ex2) {
        QFile::remove(fn2);
    }
    return QFile::copy(fn1, fn2);
}

bool checkFileOpen(QString fn)
{
    QFileInfo fi(fn);
    return fi.isWritable();
}


void OpresTable1(QWidget* wnd, QSqlDatabase &db, int id, duckx::Document* myDoc = nullptr) 
{
    QString ss;
    QSqlQuery query(db);
    query.setForwardOnly(true);

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

        while (query.next()) {
            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(query.value("Наименование/Адрес контура").toString().toStdWString().c_str(), fNormLeft);

            row1.insert_cell(query.value("Магистральная сеть(МС)").toString().toStdWString().c_str(), fNormLeft);

            row1.insert_cell(query.value("Район эксплуатации").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Участок эксплуатации").toString().toStdWString().c_str(), fNormLeft);
            /*ss.Format("%.2f", ado->read("Объём трубопроводов по участку, м3").dblVal);
            row1.insert_cell(ss.toStdWString().c_str(), fNormLeft);*/
            ss = QString("%1").arg(query.value("Объём трубопроводов контура, м3").toDouble());
            row1.insert_cell(ss.toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Начальник участка").toString().toStdWString().c_str(), fNormLeft);
        }
    };

    QVariant v;
    QString doc_name = QString("%1/%2").arg(QDir::tempPath(), QObject::tr("Фрагменты магистральных тепловых сетей контура опрессовки.docx"));
    QString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname = QString("%1/excel2/docx/empty_doc_horizontal.docx").arg(argpath());
        CopyFile(fname, doc_name, 0);
    }

    QString q = readQ("sql/docxsql/opres_t1.sql");

    if (q != "") {

        QString myId;

        q.replace("$fileID$", QString("%1").arg(id));

        if (!query_exec(db, query, q))
        {
            ErrorMessage(query);
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document *doc = open_docx(doc_name);
            if (!doc) return;

            drawTable(*doc);
            doc->save();
    //            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
            show_docx(wnd, doc);
        }
        else {
            drawTable(*myDoc);
        }
    }
}

#if 0

void OpresTable2(QWidget* wnd, QSqlDatabase &db, int id, duckx::Document* myDoc = nullptr) {

    QString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc->paragraphs().insert_paragraph(L"Табл.2 Фрагменты распределительных тепловых сетей контура опрессовки", fBoldLeft);
        doc->paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc->tables().insert_table(duckx::align_center | duckx::border);
        duckx::TableRow rowHeader = table1.insert_row().set_child(L"w:tblHeader");
        rowHeader.insert_cell(L"Наименование/Адрес контура", fBold);
        rowHeader.insert_cell(L"Распределительная сеть (РС)", fBold);
        rowHeader.insert_cell(L"Район эксплуатации", fBold);
        rowHeader.insert_cell(L"Участок эксплуатации", fBold);
        rowHeader.insert_cell(L"Объём трубопроводов контура, м3", fBold);
        rowHeader.insert_cell(L"Начальник эксплуатационного участка", fBold);

        while (query.next()) {

            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(query.value("Наименование/Адрес контура").toString().toStdWString().c_str(), fNormLeft);

            row1.insert_cell(query.value("Распределительная сеть (РС)").toString().toStdWString().c_str(), fNormLeft);

            row1.insert_cell(query.value("Район эксплуатации").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Участок эксплуатации").toString().toStdWString().c_str(), fNormLeft);
            /*ss.Format("%.2f", ado->read("Объём трубопроводов по участку, м3").dblVal);
            row1.insert_cell(ss.toStdWString().c_str(), fNormLeft);*/
            ss.Format("%.2f", ado->read("Объём трубопроводов контура, м3").dblVal);
            row1.insert_cell(ss.toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Начальник участка").toString().toStdWString().c_str(), fNormLeft);
        }
    };

    COleVariant v;
    QString doc_name;
    doc_name.Format("%s/%s", QDir::tempPath(), _TR("Фрагменты распределительных тепловых сетей контура опрессовки.docx"));
    QString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s/excel2/docx/empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    QString q, fStr;


    fStr.Format("%ssql/docxsql/opres_t2.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        QString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(wnd, "", "Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc->open();
            drawTable(doc);
            doc->save();
//            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
            show_docx(wnd, doc);
        }
        else {
            drawTable(*myDoc);
        }
    }
}

void OpresTable3(QWidget* wnd, QSqlDatabase &db, int id, duckx::Document* myDoc = nullptr) {

    QString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc->paragraphs().insert_paragraph(L"Табл.3 Граница раздела контура опрессовки", fBoldLeft);
        doc->paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc->tables().insert_table(duckx::align_center | duckx::border);
        duckx::TableRow rowHeader = table1.insert_row().set_child(L"w:tblHeader");
        rowHeader.insert_cell(L"Узел", fBold);
        rowHeader.insert_cell(L"Признак трубопровода", fBold);
        rowHeader.insert_cell(L"Состояние", fBold);
        rowHeader.insert_cell(L"Номер", fBold);
        rowHeader.insert_cell(L"Диаметр условный, мм", fBold);
        rowHeader.insert_cell(L"Примечание", fBold);

        while (query.next()) {
            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(query.value("Узел").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Признак трубопровода").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Состояние").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Номер").toString().toStdWString().c_str(), fNormLeft);
            ss.Format("%.2f", ado->read("Диаметр условный, мм").dblVal);
            row1.insert_cell(ss.toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Примечание").toString().toStdWString().c_str(), fNormLeft);
        }
    };

    COleVariant v;
    QString doc_name;
    doc_name.Format("%s/%s", QDir::tempPath(), _TR("Граница раздела контура опрессовки.docx"));
    QString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s/excel2/docx/empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    QString q, fStr;


    fStr.Format("%ssql/docxsql/granitsa_razdela_kontura_opessovki.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        QString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(wnd, "", "Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc->open();
            drawTable(doc);
            doc->save();
//            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
            show_docx(wnd, doc);
        }
        else {
            drawTable(*myDoc);
        }
    }
}


void OpresTable4(QWidget* wnd, QSqlDatabase &db, int id, duckx::Document* myDoc = nullptr) 
{

    QString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc->paragraphs().insert_paragraph(L"Табл.4 План установки манометров", fBoldLeft);
        doc->paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc->tables().insert_table(duckx::align_center | duckx::border);
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

        while (query.next()) {
            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(query.value("Наименование/Адрес контура").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Наименование тепловой сети МС/РС").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Район эксплуатации").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Камера").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Признак трубопровода").toString().toStdWString().c_str(), fNormLeft);
            ss.Format("%i", ado->read("Количество").lVal);
            row1.insert_cell(ss.toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("ФИО ответственного за установку манометров и расходомеров").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Должность ответственного за установку манометров и расходомеров").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Подразделение  ответственного за установку манометров и расходомеров").toString().toStdWString().c_str(), fNormLeft);
        }
    };

    COleVariant v;
    QString doc_name;
    doc_name.Format("%s/%s", QDir::tempPath(), _TR("План установки манометров.docx"));
    QString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s/excel2/docx/empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    QString q, fStr;


    fStr.Format("%ssql/docxsql/plan_manometrov.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        QString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(wnd, "", "Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc->open();
            drawTable(doc);
            doc->save();
//            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
            show_docx(wnd, doc);
        }
        else {
            drawTable(*myDoc);
        }
    }
}

void OpresTable5(QWidget* wnd, QSqlDatabase &db, int id, duckx::Document* myDoc = nullptr) {

    CAdoFile* ado = getAdo(getPsAdoName());
    QString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc->paragraphs().insert_paragraph(L"Табл.5 План установки расходомеров", fBoldLeft);
        doc->paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc->tables().insert_table(duckx::align_center | duckx::border);
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

        while (query.next()) {
            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(query.value("Наименование/Адрес контура").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Наименование тепловой сети МС/РС").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Район эксплуатации").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Камера").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Признак трубопровода").toString().toStdWString().c_str(), fNormLeft);
            ss.Format("%i", ado->read("Количество").lVal);
            row1.insert_cell(ss.toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("ФИО ответственного за установку манометров и расходомеров").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Должность ответственного за установку манометров и расходомеров").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Подразделение  ответственного за установку манометров и расходомеров").toString().toStdWString().c_str(), fNormLeft);
        }
    };

    COleVariant v;
    QString doc_name;
    doc_name.Format("%s/%s", QDir::tempPath(), _TR("План установки расходомеров.docx"));
    QString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s/excel2/docx/empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    QString q, fStr;


    fStr.Format("%ssql/docxsql/plan_rashodomerov.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        QString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(wnd, "", "Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc->open();
            drawTable(doc);
            doc->save();
//            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
            show_docx(wnd, doc);
        }
        else {
            drawTable(*myDoc);
        }
    }
}

void OpresTable7(QWidget* wnd, QSqlDatabase &db, int id, duckx::Document* myDoc = nullptr) {

    CAdoFile* ado = getAdo(getPsAdoName());
    QString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc->paragraphs().insert_paragraph(L"Табл.7 Список подключаемых потребителей к тепловой сети (контуру) опрессовки", fBoldLeft);
        doc->paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc->tables().insert_table(duckx::align_center | duckx::border);
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

        while (query.next()) {
            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(query.value("Наименование/Адрес контура").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Наименование тепловой сети МС/РС").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Район эксплуатации").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Наименование узла присоединения потребителя").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Адрес").toString().toStdWString().c_str(), fNormLeft);

            row1.insert_cell(query.value("Вид потребителя").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Дата начала опрессовки").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Дата окончания опрессовки").toString().toStdWString().c_str(), fNormLeft);
            ss.Format("%f", ado->read("Расчётная нагрузка, Гкал/ч").dblVal);
            row1.insert_cell(ss.toStdWString().c_str(), fNormLeft);
        }
    };

    COleVariant v;
    QString doc_name;
    doc_name.Format("%s/%s", QDir::tempPath(), _TR("Список подключаемых потребителей к тепловой сети (контуру) опрессовки.docx"));
    QString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s/excel2/docx/empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    QString q, fStr;


    fStr.Format("%ssql/docxsql/spisok_podkl_pootreb.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        QString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(wnd, "", "Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc->open();
            drawTable(doc);
            doc->save();
//            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
            show_docx(wnd, doc);
        }
        else {
            drawTable(*myDoc);
        }
    }
}

void OpresTable9(QWidget* wnd, QSqlDatabase &db, int id, duckx::Document* myDoc = nullptr) {

    CAdoFile* ado = getAdo(getPsAdoName());
    QString ss;

    auto drawTable = [&](duckx::Document& doc) {
        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc->paragraphs().insert_paragraph(L"Табл.9 Список нарушений", fBoldLeft);
        doc->paragraphs().insert_paragraph(L"", fNorm);

        duckx::Table table1 = doc->tables().insert_table(duckx::align_center | duckx::border);
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

        while (query.next()) {
            duckx::TableRow row1 = table1.insert_row();

            row1.insert_cell(query.value("Наименование начального узла").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Наименование конечного узла").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Адрес").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Вид нарушения").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Категория нарушения").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Характер повреждения").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Описание повреждения").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Дата начала ремонтных работ").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Дата завершения ремонтных работ").toString().toStdWString().c_str(), fNormLeft);
            row1.insert_cell(query.value("Способ ликвидации нарушения").toString().toStdWString().c_str(), fNormLeft);
        }
    };

    COleVariant v;
    QString doc_name;
    doc_name.Format("%s/%s", QDir::tempPath(), _TR("Список нарушений.docx"));
    QString fname;

    if (myDoc == nullptr && !checkFileOpen(doc_name))
        return;

    if (myDoc == nullptr) {
        fname.Format("%s/excel2/docx/empty_doc_horizontal.docx", argpath());
        CopyFile(fname, doc_name, 0);
    }


    QString q, fStr;


    fStr.Format("%ssql/docxsql/opres_defect_list.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        QString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            QMessageBox::warning(wnd, "", "Ошибка в запросе");
            return;
        }
        if (myDoc == nullptr) {
            duckx::Document doc(doc_name.GetString());
            doc->open();
            drawTable(doc);
            doc->save();
//            HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
            show_docx(wnd, doc);
        }
        else {
            drawTable(*myDoc);
        }
    }
}

#endif

void OpresTableAkt(QWidget* wnd, QSqlDatabase &db, int id)
{
    struct Chief {
        QString chief, area, name, sign;
    };
    struct WarmNet {
        QString name, node1, node2, sign;
    };

    QString doc_name = QString("%1/%2").arg(QDir::tempPath(), QObject::tr("АКТ На проведение гидравлических испытаний тепломагистралей.docx"));

    if (!checkFileOpen(doc_name))
        return;

    QString fname = QString("%1/excel2/docx/empty_doc_horizontal.docx").arg(argpath());

    CopyFile(fname, doc_name, 0);

    QString q = readQ("ssql/docxsql/opres_journal.sql");

    if (q != "")
        q.replace("$fileID$", QString("%1").arg(id));

        QSqlQuery query(db);
        query.setForwardOnly(true);

        if (!query_exec(db, query, q))
        {
            ErrorMessage(query);
            return;
        }

        duckx::Document *doc = open_docx(doc_name);
        if (!doc) return;


        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fBoldRight = duckx::align_right | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_12 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;


        if (query.next()) {
            doc->paragraphs().insert_paragraph(L"«Утверждаю»", fBoldRight);
            doc->paragraphs().insert_paragraph(query.value("Должность утвердаещего").toString().toStdWString().c_str(), fBoldRight);
            doc->paragraphs().insert_paragraph(query.value("Подразделение утверждающего").toString().toStdWString().c_str(), fBoldRight);
            doc->paragraphs().insert_paragraph(query.value("ФИО утверждающего").toString().toStdWString().c_str(), fBoldRight);
            doc->paragraphs().insert_paragraph(L"«____» _______________ 20__г.", fBoldRight);
            doc->paragraphs().insert_paragraph(L"", fBold);
            doc->paragraphs().insert_paragraph(query.value("Дата утверждения акта испытаний").toString().toStdWString().c_str(), fBoldRight);

            for (int i = 0; i < 3; i++)
                doc->paragraphs().insert_paragraph(L"", fBold);
            doc->paragraphs().insert_paragraph(L"АКТ", fBold);
            doc->paragraphs().insert_paragraph(L"На проведение гидравлических", fBold);
            doc->paragraphs().insert_paragraph(L"испытаний тепломагистралей", fBold);
            doc->paragraphs().insert_paragraph(L"", fBold);

            double d2 = query.value("Давление опрессовки 2 этапа, кгс/см2").toDouble();
            long cont = query.value("Продолжительность опрессовки").toInt();
            QString desicion = query.value("Решение комиссии").toString();

            QString position1 = query.value("Должность руководителя испытаний").toString();
            QString subdivision1 = query.value("Подразделение руководителя испытаний").toString();
            QString chief1 = query.value("ФИО руководителя испытаний").toString();

            QString position2 = query.value("Должность ответственного по СНиП контура источника тепла").toString();
            QString subdivision2 = query.value("Подразделение ответственного по СНиП контура источника тепла").toString();
            QString chief2 = query.value("ФИО ответственного по СНиП контура источника тепла").toString();

            doc->paragraphs().insert_paragraph(L"Комиссия в составе:", fNormLeft);

            duckx::Table table1 = doc->tables().insert_table(duckx::align_left);
            duckx::TableRow rowHeader1 = table1.insert_row();
            rowHeader1.insert_cell(L"Руководителя испытаний:", fNormLeft);
            rowHeader1.insert_cell(position1.toStdWString().c_str(), fNormLeft);
            rowHeader1.insert_cell(subdivision1.toStdWString().c_str(), fNormLeft);
            rowHeader1.insert_cell(chief1.toStdWString().c_str(), fNormLeft);

            duckx::TableRow rowHeader2 = table1.insert_row();
            rowHeader2.insert_cell(L"Отв. контура источника тепла:", fNormLeft);
            rowHeader2.insert_cell(position2.toStdWString().c_str(), fNormLeft);
            rowHeader2.insert_cell(subdivision2.toStdWString().c_str(), fNormLeft);
            rowHeader2.insert_cell(chief2.toStdWString().c_str(), fNormLeft);

        vector<Chief> chiefData;

        q = readQ("sql/docxsql/ms_rs_opres_fragments_by_nu.sql");
        if (q != "") {
            q.replace("$fileID$", QString("%1").arg(id));

            QSqlQuery query(db);
            query.setForwardOnly(true);

            if (!query_exec(db, query, q))
            {
                ErrorMessage(query);
                return;
            }

            duckx::Document *doc = open_docx(doc_name);
            if (!doc) return;


            while (query.next()) {
                Chief* myChief = new Chief();
                myChief->chief = query.value("Начальник участка").toString();
                myChief->name = query.value("Наименование участка").toString();
                myChief->area = query.value("Район эксплуатации").toString();
                myChief->sign = query.value("Признак сети").toString();
                chiefData.push_back(*myChief);
            }
        }

        vector<WarmNet> warmNetData;
        q = readQ("sql/docxsql/opres_pl.sql");
        if (q != "") {
            q.replace("$fileID$", QString("%1").arg(id));

            QSqlQuery query(db);
            query.setForwardOnly(true);

            if (!query_exec(db, query, q))
            {
                ErrorMessage(query);
                return;
            }

            duckx::Document *doc = open_docx(doc_name);
            if (!doc) return;

            while (query.next()) {
                WarmNet* myWarmNet = new WarmNet();
                myWarmNet->name = query.value("Наименование участка").toString();
                myWarmNet->node1 = query.value("Наименование начального узла").toString();
                myWarmNet->node2 = query.value("Наименование конечного узла").toString();
                myWarmNet->sign = query.value("Признак сети").toString();
                warmNetData.push_back(*myWarmNet);
            }

            for (int i = 0; i < chiefData.size(); i++) {
                duckx::TableRow rowHeaderChief = table1.insert_row();
                rowHeaderChief.insert_cell(L"начальника", fNormLeft);
                rowHeaderChief.insert_cell(chiefData[i].name.toStdWString().c_str(), fNormLeft);
                rowHeaderChief.insert_cell(chiefData[i].area.toStdWString().c_str(), fNormLeft);
                rowHeaderChief.insert_cell(chiefData[i].chief.toStdWString().c_str(), fNormLeft);
            }

            doc->paragraphs().insert_paragraph(L"", fBold);
            doc->paragraphs().insert_paragraph(L"составили настоящий акт о проведении гидравлического испытания:", fNormLeft);

            duckx::Table table2 = doc->tables().insert_table(duckx::align_left);
            for (int i = 0; i < warmNetData.size(); i++) {
                duckx::TableRow rowHeader1T2 = table2.insert_row();

                QString s = warmNetData[i].node1 + " - " + warmNetData[i].node2;
                rowHeader1T2.insert_cell(s.toStdWString().c_str(), fNormLeft);

//                rowHeader1T2.insert_cell(warmNetData[i].name.toStdWString().c_str(), fNormLeft);
                QString ss = QString("(%1)").arg(warmNetData[i].sign);
                rowHeader1T2.insert_cell(ss.toStdWString().c_str(), fNormLeft);
            }

            doc->paragraphs().insert_paragraph(L"", fBold);
            QString ss;
            ss = QString("давлением %1 кгс/см2 в течение %2 минут с полным наружным осмотром.").arg(d2).arg(cont);
            doc->paragraphs().insert_paragraph(ss.toStdWString().c_str(), fNormLeft);
            doc->paragraphs().insert_paragraph(L"", fNormLeft);
            ss = QString("Решение комиссии: %1").arg(desicion);
            doc->paragraphs().insert_paragraph(ss.toStdWString().c_str(), fNormLeft);
            doc->paragraphs().insert_paragraph(L"", fNormLeft);

            duckx::Table table3 = doc->tables().insert_table(duckx::align_left);
            duckx::TableRow rowHeader1T3 = table3.insert_row();
        
            rowHeader1T3.insert_cell(position1.toStdWString().c_str(), fNormLeft);
            rowHeader1T3.insert_cell(subdivision1.toStdWString().c_str(), fNormLeft);
            rowHeader1T3.insert_cell(L"_________", fNormLeft);
            rowHeader1T3.insert_cell(chief1.toStdWString().c_str(), fNormLeft);

            duckx::TableRow rowHeader2T3 = table3.insert_row();
    
            rowHeader2T3.insert_cell(position2.toStdWString().c_str(), fNormLeft);
            rowHeader2T3.insert_cell(subdivision2.toStdWString().c_str(), fNormLeft);
            rowHeader2T3.insert_cell(L"_________", fNormLeft);
            rowHeader2T3.insert_cell(chief2.toStdWString().c_str(), fNormLeft);

            for (int i = 0; i < chiefData.size(); i++) {
                duckx::TableRow rowHeaderChief = table3.insert_row();
                QString ss = QString("начальник %s").arg(chiefData[i].name);
                rowHeaderChief.insert_cell(ss.toStdWString().c_str(), fNormLeft);
                rowHeaderChief.insert_cell(chiefData[i].area.toStdWString().c_str(), fNormLeft);
                rowHeaderChief.insert_cell(L"_________", fNormLeft);
                rowHeaderChief.insert_cell(chiefData[i].chief.toStdWString().c_str(), fNormLeft);
            }
            doc->paragraphs().insert_next_page();
            doc->paragraphs().insert_paragraph(L"Приложение", fBoldRight);
            doc->paragraphs().insert_paragraph(L"", fNormLeft);
        }
#if 0
        OpresTable9(wnd, db, id, &doc);
        doc->paragraphs().insert_paragraph(L"", fNormLeft);
#endif
        OpresTable1(wnd, db, id, doc);
        doc->paragraphs().insert_paragraph(L"", fNormLeft);
#if 0
        OpresTable2(wnd, db, id, &doc);
        doc->paragraphs().insert_paragraph(L"", fNormLeft);
        OpresTable3(wnd, db, id, &doc);
        doc->paragraphs().insert_paragraph(L"", fNormLeft);
        OpresTable4(wnd, db, id, &doc);
        doc->paragraphs().insert_paragraph(L"", fNormLeft);
        OpresTable5(wnd, db, id, &doc);
        doc->paragraphs().insert_paragraph(L"", fNormLeft);
        OpresTable7(wnd, db, id, &doc);
#endif

        doc->save();

//        HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
 
        show_docx(wnd, doc);
    }
}
