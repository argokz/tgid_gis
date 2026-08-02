#include <QApplication>
#include <QtGui>
#include "std.h"
#include <db/db.h>
#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>
#include <QRegularExpression>

#include "xlsxdocument.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

bool write_sheet(QXlsx::Document &xlsxW, QSqlDatabase &db, const QString & tn, const QString & q, int *hide, bool head, int row1, int col1);

int getOutID(QSqlDatabase &db, int fileID)
{
    QString q = QString(QObject::tr("SELECT id FROM CALCULATION WHERE id IN (SELECT MAX(id) AS id FROM CALCULATION WHERE fileID=%1 GROUP BY fileID)").arg(fileID));
    return readTableInt(db, q);
}

bool create_vyd_line_table_new_simple(QSqlDatabase &db, CGraph2* graph, QString& tn);
bool create_vyd_line_table_new_all(QSqlDatabase &db, CGraph2* graph, QString& tn, int fileID);
bool create_vyd_node_table_BIG(QSqlDatabase &db, CGraph2* graph, QString& tn, bool is_big);
bool create_vyd_node_table_BIG_all(QSqlDatabase &db, CGraph2* graph, QString& tn, bool is_big, int fileID);



void gidExcel(QSqlDatabase &db, const QString & fn, const QString & path, int fileID, CGraph2 *m_graph)
{
    QFile file(fn);

    int row = 0;
    int h = 0;

    QString xls_name = "";
    QString sql_name = "";

    int calculationID = getOutID(db, fileID);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QXlsx::Document *xls = nullptr;
        
        while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);
            if (row == 0) {
                xls_name = s;
                xls_name.replace("xls/", "xlsx/");
                xls_name.replace("xls\\", "xlsx\\");
                xls_name.replace(".xls", ".xlsx");
                xls = new QXlsx::Document(QString("%1excel2/%2").arg(argpath(), xls_name));
                row = 1;
            }
            else if (row == 1) {
                sql_name = s;
                row = 2;
            }
            else if (row == 2) {
                row = 1;

                int sheet = 0;
                int row0 = 0;

                static QRegularExpression re("^([0-9]+)\\s+([0-9]+)$");
                QRegularExpressionMatch match = re.match(s);
                if (match.hasMatch()) {
                   row0 = match.captured(1).toInt();
                   sheet = match.captured(2).toInt()-1;
                }
                else {
                    row0 = s.toInt();
                }

                QString q = readQ(QString("excel2/%1").arg(sql_name));

                if (q != "") {
                    QString vyd;

                    if (m_graph->isMark()) {
                        if (q.indexOf("n1.fileID=$fileID$") != -1) {
                            create_vyd_line_table_new_simple(db, m_graph, vyd);
                            q.replace("WHERE n1.fileID=$fileID$ AND", QString("join %1 vyd ON vyd.id2=l.id WHERE").arg(vyd));
                            q += QString(" ORDER BY vyd.id1");
                        }
                        else if (q.indexOf("n.fileID=$fileID$") != -1) {
                            create_vyd_node_table_BIG(db, m_graph, vyd, false);
                            q.replace("WHERE n.fileID=$fileID$ AND", QString("join %1 vyd ON vyd.id2=n.id WHERE").arg(vyd));
                            q += QString(" ORDER BY vyd.id1");
                        }
                    }
                    else {
                        if (q.indexOf("n1.fileID=$fileID$") != -1) {
                            create_vyd_line_table_new_all(db, m_graph, vyd, fileID);
                            q.replace("WHERE n1.fileID=", QString("join %1 vyd ON vyd.id2=l.id WHERE n1.fileID=").arg(vyd));
                            q += QString(" ORDER BY vyd.id1");
                        }
                        else if (q.indexOf("n.fileID=$fileID$") != -1) {
                            create_vyd_node_table_BIG_all(db, m_graph, vyd, false, fileID);
                            q.replace("WHERE n.fileID=", QString("join %1 vyd ON vyd.id2=n.id WHERE n.fileID=").arg(vyd));
                            q += QString(" ORDER BY vyd.id1");
                        }
                    }

                    if (fileID >= 0) {
                        q.replace("$fileID$", QString("%1").arg(fileID));
                        q.replace("$calculationID$", QString("%1").arg(calculationID));
                    }

                    if (xls) {
    //                    QInputDialog::getMultiLineText(nullptr, sql_name, "label", q);
                        xls->selectSheet(sheet);
                        write_sheet(*xls, db, sql_name, q, nullptr, false, row0, 0);
                    }
                }
            }
        }

        if (xls) {
            xls->selectSheet(0);

            QString tmpn = QString("%1/tgid.xlsx").arg(QDir::tempPath());
            if ( xls->saveAs(tmpn) ) {
                QString path = QString("file:///%1").arg(tmpn);
                qDebug() << path;
                QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
            }
            delete xls;
        }
    }
}
    
#if 0
void GidWidget::onExcel2() // Таблицы Excel...
{
    if (m_fileID <= 0) {
        QMessageBox::information(this, "", tr("Выберите активный фрагмент"));
        return;
    }

    MMenuDial md(this, tr("Выберите таблицу"));

    QDir directory(argpath()+"/excel2");

    QStringList lst = directory.entryList(QStringList() << "*.lst",QDir::Files);
    
    for (QString & filename : lst) {
        QString fn = directory.path()+"/"+filename;

        QString p = GetTitle(filename);
        p.replace("OUT_", "* ");
        p.replace("HS_", "! ");
        md.Add(p, fn);
    }

    if (md.exec()) {
        gidExcel(m_cxema.m_db, md.value().toString(), directory.path(), m_fileID, m_cxema.m_graph);
//        QMessageBox::warning(this, "", directory.path());
    }

}

#endif

struct DosStruct
{
    QString name = "";
    QString file = "";
    QString type = "";
    QString args = "";
};


DosStruct read_json_docs(QWidget *widget, const QString &fileName)
{
    DosStruct docs;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл:" << fileName;
        return docs;
    }

    MMenuDial md(widget, QObject::tr("Выберите таблицу"));


    QByteArray data = file.readAll();
    file.close();

    // Преобразуем данные в JSON-документ
    QJsonDocument doc = QJsonDocument::fromJson(data);

    // Проверяем, является ли JSON объектом и содержит ли массив "docs"
    if (doc.isObject()) {
        QJsonObject jsonObject = doc.object();

        if (jsonObject.contains("docs") && jsonObject["docs"].isArray()) {
            QJsonArray docsArray = jsonObject["docs"].toArray();

            for (const QJsonValue &value : docsArray) {
                if (value.isObject()) {
                    QJsonObject docObject = value.toObject();
                    DosStruct docs;

                    docs.name = docObject["name"].toString();
                    docs.file = docObject["file"].toString();
                    docs.type = docObject["type"].toString();
                    docs.args = docObject["args"].toString();

                    md.Add(docs.name, QVariant::fromValue(docs));
                }
            }
        } 
        else {
            qWarning() << "Ключ 'docs' отсутствует или не является массивом.";
        }
    } 
    else {
        qWarning() << "Некорректный формат JSON.";
    }

    if (md.exec()) {
        return md.value().value<DosStruct>();
    }

    return docs;
}

QString save_mark(const CGraph2 *graph)
{
    QTemporaryFile file;

    file.setAutoRemove(false);

//    if (!file.open(QIODevice::WriteOnly)) {
        if (!file.open()) {
        qDebug() << "Не удалось открыть файл для записи:" << file.errorString();
        return "";
    }
    
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);  // Устанавливаем версию потока для совместимости

    auto p = graph->map_node.begin();

    while (p != graph->map_node.end()) {
        CNode2 * n = p->second;
        for (CLINE2 *l = n->lines; l; l = nextl(l)) {
            if (bline(l)->line.isPjezo || bline(l)->line.isPjezoP || bline(l)->line.isPjezoO) {
                if (bline(l)->line.nomP > 0 && (bline(l)->line.isPjezo || bline(l)->line.isPjezoP)) {
                    out << bline(l)->line.nomP;
                }
                if (bline(l)->line.nomO > 0 && bline(l)->line.nomO != bline(l)->line.nomP && (bline(l)->line.isPjezo || bline(l)->line.isPjezoO)) {
                    out << bline(l)->line.nomO;
                }
            }
        }

        p ++;
    }

    QString fileName = file.fileName();

    return fileName;
}


void GidWidget::onExcel2() // Таблицы Excel...
{
    QString filename = argpath() + "excel3/docs.json";

    DosStruct doc = read_json_docs(this, filename);

    if (doc.name == "") return;

    QString args = doc.args;

    if (args.indexOf("%fragments%") != -1) {
        args.replace("%fragments%", m_cxema.m_par);
    }

    if (args.indexOf("%mark_lines%") != -1) {
        if (m_cxema.m_graph->isMark()) {
            QString file_name = save_mark(m_cxema.m_graph);
            if (file_name != "") {
                args.replace("%mark_lines%", QString("-mark_lines_file \"%1\"").arg(file_name));
            }
        }
        args.replace("%mark_lines%", "");
    }


    QString str = str_python(doc.file, args);
//    QInputDialog::getMultiLineText(this, "", "", str);
    main_window->start(str, this, nullptr);
}
  

