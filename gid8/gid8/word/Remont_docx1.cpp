#include <QtGui>
#include <QtWidgets>
#include <gidview/GidWidget.h>

#include <property/PropertyDial.h>
#include <db/db.h>


//#include "grid/msword8.h"
#include <fstream>

//#include "InputDia.h"

#include <lib/DuckX/src/duckx.hpp>

QString sezon_name(int y);

bool checkFileOpen(QString doc_name);

const duckx::formatting_flag fNorm = duckx::align_left | duckx::size_8 | duckx::tnr_style;
//const duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
const duckx::formatting_flag fBoldCenter = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
const duckx::formatting_flag fItalicRight = duckx::align_right | duckx::tnr_style | duckx::italic | duckx::size_8;
const duckx::formatting_flag fNormCenter = duckx::align_center | duckx::size_8 | duckx::tnr_style;


bool write_table(duckx::Table &table1, QSqlDatabase &db, QString q, int ncols, const int* cols, bool numb)
{
    QSqlQuery query(db);
    query.setForwardOnly(true);

    QString str;
    int nn = 1;

    if (query_exec(db, query, q)) {
        while (query.next()) {
            duckx::TableRow row = table1.insert_row();

            if (cols) {
                if (numb) {
                    str = QString("%1").arg(nn++);
                }
                for (long i = 0; i < ncols; i++) {
                    std::wstring str = query.value(cols[i]).toString().toStdWString();
                    row.insert_cell(str.c_str(), fNorm);
                }
            }
            else {

                QSqlRecord rec = query.record();
                int nf = rec.count();

                for (long i = 0; i < nf; i++) {
                    str = query.value(i).toString();
                    if (numb && i == 0) {
                        str = QString("%1").arg(nn++);
                    }
                    row.insert_cell(str.toStdWString().c_str(), fNorm);
                }
            }
        }
    }
    else {
        QMessageBox::warning(nullptr, "", "Ошибка в запросе");
        return false;
    }

    return true;
}


duckx::Document * open_docx(QString doc_name)
{
    duckx::Document* doc = nullptr;

    if (!checkFileOpen(doc_name))
        return doc;

    QString fname = QString("%1/excel2/docx/empty_doc_album.docx").arg(argpath());

    QFile::copy(fname, doc_name);

//    std::string QString_to_cp1251(QString s);
//    std::string fn = QString_to_cp1251(doc_name);

    QByteArray byteArray = doc_name.toLocal8Bit();
    std::string fn(byteArray.constData(), byteArray.length());
    doc = new duckx::Document(fn);

    if (doc) {
        doc->open();
    }

    return doc;
}

void show_docx(QWidget* wnd, duckx::Document *doc)
{
    if (!doc) return;

    doc->save();

//    HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc->get_file().c_str(), NULL, NULL, SW_SHOWMAXIMIZED);

    QString path = QString("file:///%1").arg(doc->get_file().c_str());

    QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));

//    QProcess *p = QProcess();
//    p->start("<program>", [<arguments>]);

}

QString sezon_name(int y)
{
    QString s = QString("Отопительный сезон %1-%2").arg(y).arg(y + 1);
    if (y == -1) {
        s = QString("Все отопительные сезоны");
    }
    return s;
}

#include <QInputDialog>

void Remont_docx1(QWidget* wnd, QSqlDatabase &db, QString q, int m_sezon_korrozia)
{
//    QString doc_name = QString("%1/%2").arg(QDir::tempPath(), "График выполнения ремонтов.docx");
    QString doc_name = QString("%1/%2").arg(QDir::tempPath(), "grafik.docx");

//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);


    duckx::Document *doc = open_docx(doc_name);
    if (!doc) return;

    QDateTime v = QDateTime::currentDateTime();


//    s.Format("отопительный сезон/период\nна «%s»", v.Format("%d.%m.%Y"));   
    QString s = QString("%1\nна «%1»").arg(sezon_name(m_sezon_korrozia), v.toString("dd.MM.yyyy"));

    QString energosist = readTableValue(db, "SELECT TOP 1 kratkoe_naimenovanie_es FROM energosistemy");
    doc->paragraphs().insert_paragraph(energosist.toStdWString().c_str(), fItalicRight);
     

//    doc->paragraphs().insert_paragraph(L"Наименование теплоснабжающей организации организации", fItalicRight);
    doc->paragraphs().insert_paragraph(L"", fBoldCenter);
    doc->paragraphs().insert_paragraph(L"График выполнения ремонтов", fBoldCenter);
    doc->paragraphs().insert_paragraph(s.toStdWString().c_str(), fNormCenter);
    doc->paragraphs().insert_paragraph(L"", fBoldCenter);

    duckx::Table table1 = doc->tables().insert_table(duckx::align_center | duckx::border);
    duckx::TableRow row = table1.insert_row();

    row.insert_cell(L"п/п", fBoldCenter, 1, 1);//
    row.insert_cell(L"Контур ремонта", fBoldCenter, 1, 1);//
    row.insert_cell(L"Дата начала ремонта", fBoldCenter, 1, 1);//
    row.insert_cell(L"Дата окончания ремонта", fBoldCenter, 1, 1);//
    row.insert_cell(L"Вид ремонта", fBoldCenter, 1, 1);//
    row.insert_cell(L"Тепловая сеть", fBoldCenter, 1, 1);//
//    row.insert_cell(L"Объем средств, тыс. тг", fBoldCenter, 3, 1);//
//    row.insert_cell(L"Ремонтный персонал, чел.", fBoldCenter, 3, 1);//
    row.insert_cell(L"Проложено труб, м.п", fBoldCenter, 3, 1);//
    row.insert_cell(L"Восстановление тепловой изоляции, м2", fBoldCenter, 3, 1);//
    row.insert_cell(L"Восстановление канальной прокладки, м", fBoldCenter, 3, 1);//
    row.insert_cell(L"Асфальтирование, м2", fBoldCenter, 3, 1);//
    row.insert_cell(L"Ответственный за ремонт", fBoldCenter, 1, 1);//
    row.insert_cell(L"Подразделение производившее ремонт", fBoldCenter, 1, 1);//

    row = table1.insert_row();

    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 

    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

/*
    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

*/
    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 

    write_table(table1, db, q, 0, 0, true);

    show_docx(wnd, doc);
}
