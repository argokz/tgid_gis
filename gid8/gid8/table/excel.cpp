#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include <db/db.h>

//#include <OpenXLSX.hpp>
//using namespace OpenXLSX;

// [0] include QXlsx headers
#include "xlsxdocument.h"
//#include "xlsxchartsheet.h"
//#include "xlsxcellrange.h"
//#include "xlsxchart.h"
//#include "xlsxrichstring.h"
//#include "xlsxworkbook.h"
using namespace QXlsx;

#include <property/variantdelegate.h>

QString findColumnRusName(const QString & d, const QString & n1, const QString & n2);
QString findNewColumnName(const QString & tn, const QString & old_name);
QString findTableRusName(const QString & d, const QString & n);


#if 0

bool write_sheet(QXlsx::Document &xlsxW, QSqlDatabase &db, const QString & tn, const QString & q, int *hide, bool head, int row1, int col1)
{
    QXlsx::Format row_data, simple_green, title_stile;

    row_data.setBorderStyle(QXlsx::Format::BorderThin);
    row_data.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    row_data.setNumberFormat("0.000");
    
    simple_green.setFontColor(QColor(Qt::green));
    simple_green.setBorderStyle(QXlsx::Format::BorderThin);

    title_stile.setFontBold(true);
    title_stile.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    title_stile.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    title_stile.setPatternBackgroundColor(QColor(225,225,225));
    title_stile.setBorderStyle(QXlsx::Format::BorderMedium);

    title_stile.setTextWrap(true);


    QProgressDialog pc("Операция выполняется, подождите пожалуйста...", "Прервать", 0, 100, nullptr);

    long n_rows = sizeOfQ(db, q);

    int col_len[256];

    QSqlQuery query(db);

    if (query_exec(query, q)) {
        QSqlRecord record = query.record();

        int cnt = -1;
        int row = 1 + row1;

/*
        QHeaderView *hv = this->horizontalHeader();

        for (int c = 0; c < hv->count(); c++) {
            QString name = this->model()->headerData(c, Qt::Horizontal).toString();
            bool y = this->isColumnHidden(c);
                
            if (!y) {
                xlsxW.write(row, col, name, title_stile);
                col_len[col] = name.length()/3+2;
                col += 1;
            }
        }
*/

        cnt = record.count();

        int col = 1 + col1;

        for (int c = 0; c < cnt; c++) {
            QSqlField field = record.field(c);
            QString name = field.name();
            bool y = hide ? hide[c] : false;

            if (!y) {
                QString old_name = name;
                QString new_name = findNewColumnName(tn, old_name);
                QString fn = findColumnRusName("gid", tn, new_name);

                if (head) {
                    xlsxW.write(row, col, fn, title_stile);
                }
                col_len[col] = name.length()/3+2;
                col += 1;
            }
        }

        if (head) {
            row += 1;
        }

        while (query.next()) {
            QApplication::processEvents();
            pc.setValue(row*100/n_rows);
            
            QSqlRecord record = query.record();
            int col = col1 + 1;

            for (int c = 0; c < cnt; c++) {
//                QString name = this->model()->headerData(c, Qt::Horizontal).toString();
                bool y = hide ? hide[c] : false;
                
                if (!y) {
                    QXlsx::Format row_data;

                    QSqlField field = record.field(c);

                    row_data.setBorderStyle(QXlsx::Format::BorderThin);

                    if (field.metaType().id() == QMetaType::QDateTime ||
                        field.metaType().id() == QMetaType::QDate
                    ) {
                        row_data.setNumberFormat("dd.mm.yyyy");
                    }

                    if (field.metaType().id() == QMetaType::QString) {
                        row_data.setHorizontalAlignment(QXlsx::Format::AlignHGeneral);
                    }

                    QString s = VariantDelegate::displayText(field.value());

                    int len = s.length()+3;
                    if (len < 0) len = 1;
                    if (len > 50) len = 50;

                    if (len > col_len[col]) {
                        col_len[col] = len;
                    }

                    xlsxW.write(row, col, field.value(), row_data);
                    col += 1;
                }
            }
            row += 1;
        }

        if (head) {
            col = 1;

            for (int c = 0; c < cnt; c++) {
                bool y = hide ? hide[c] : false;

                if (!y) {
                    xlsxW.setColumnWidth(col, col, col_len[col]);
                    col += 1;
                }
            }
        }
    }

    return true;
}

#endif


bool write_sheet(QXlsx::Document &xlsxW, QSqlDatabase &db, const QString & tn, const QString & q, int *hide, bool head, int row1, int col1)
{
    QXlsx::Format row_data, simple_green, title_stile;

    row_data.setBorderStyle(QXlsx::Format::BorderThin);
    row_data.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    row_data.setNumberFormat("0.000");
    
    simple_green.setFontColor(QColor(Qt::green));
    simple_green.setBorderStyle(QXlsx::Format::BorderThin);

    title_stile.setFontBold(true);
    title_stile.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    title_stile.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    title_stile.setPatternBackgroundColor(QColor(225,225,225));
    title_stile.setBorderStyle(QXlsx::Format::BorderMedium);

    title_stile.setTextWrap(true);


    QProgressDialog pc("Операция выполняется, подождите пожалуйста...", "Прервать", 0, 100, nullptr);

    long n_rows = sizeOfQ(db, q);

    int col_len[256];

    QSqlQuery query(db);

//    q = ispr_q(q);

//    QInputDialog::getMultiLineText(nullptr, "q", "q", q);

    if (query_exec(db, query, q)) {
        QSqlRecord record = query.record();

        int cnt = -1;
        int row = 1 + row1;

/*
        QHeaderView *hv = this->horizontalHeader();

        for (int c = 0; c < hv->count(); c++) {
            QString name = this->model()->headerData(c, Qt::Horizontal).toString();
            bool y = this->isColumnHidden(c);
                
            if (!y) {
                xlsxW.write(row, col, name, title_stile);
                col_len[col] = name.length()/3+2;
                col += 1;
            }
        }
*/

        cnt = record.count();

        int col = 1;

        for (int c = 0; c < cnt; c++) {
            QSqlField field = record.field(c);
            QString name = field.name();
            bool y = hide ? hide[c] : false;

            if (!y) {
                QString old_name = name;
                QString new_name = findNewColumnName(tn, old_name);
                QString fn = findColumnRusName("gid", tn, new_name);

                if (head) {
                    xlsxW.write(row, col+col1, fn, title_stile);
                }
                col_len[col] = fn.length()/3+2;
                col += 1;
            }
        }

        if (head) {
            row += 1;
        }

        while (query.next()) {
            pc.setValue(row*100/n_rows);
            QApplication::processEvents();
            
            QSqlRecord record = query.record();
            int col = 1;

            for (int c = 0; c < cnt; c++) {
//                QString name = this->model()->headerData(c, Qt::Horizontal).toString();
                bool y = hide ? hide[c] : false;
                
                if (!y) {
                    QXlsx::Format row_data;

                    QSqlField field = record.field(c);

                    row_data.setBorderStyle(QXlsx::Format::BorderThin);

                    if (field.metaType().id() == QMetaType::QDateTime ||
                        field.metaType().id() == QMetaType::QDate
                    ) {
                        row_data.setNumberFormat("dd.mm.yyyy");
                    }

                    if (field.metaType().id() == QMetaType::QString) {
                        row_data.setHorizontalAlignment(QXlsx::Format::AlignHGeneral);
                    }

                    QString s = VariantDelegate::displayText(field.value());

                    int len = s.length()+3;
                    if (len < 0) len = 1;
                    if (len > 50) len = 50;

                    if (len > col_len[col]) {
                        col_len[col] = len;
                    }

                    xlsxW.write(row, col+col1, field.value(), row_data);
                    col += 1;
                }
            }
            row += 1;
        }

        if (head) {
            col = 1;

            for (int c = 0; c < cnt; c++) {
                bool y = hide ? hide[c] : false;

                if (!y) {
                    xlsxW.setColumnWidth(col+col1, col+col1, col_len[col]);
                    col += 1;
                }
            }
        }
    }

    return true;
}

bool save_excel(const QString & filename, QSqlDatabase &db, const QString & tn, const QString & q, int *hide)
{
//    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();
//    QString q = query.executedQuery();

    QXlsx::Document xlsxW;

    QString tn_rus = findTableRusName("gid", tn);

    xlsxW.addSheet(tn_rus);
    write_sheet(xlsxW, db, tn, q, hide, true, 0, 0);

    if ( xlsxW.saveAs(filename) )
    {
        return true;
    }

    return false;
}

