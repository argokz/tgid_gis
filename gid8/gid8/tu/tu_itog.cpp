#include <QApplication>
#include <QtGui>
#include <QInputDialog>

#include <mainwindow.h>
#include <gidview/GidWidget.h>

#include "TextFilterDialog.h"

#include "db/db.h"
#include <geo/Klassif.h>


#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QDebug>

// QXlsx
#include "xlsxdocument.h"
#include "xlsxformat.h"

QString transl(const QString & s);


using namespace QXlsx;

int viewQ(QSqlDatabase &db, const QString &sql)
{
    QSqlQuery query(db);

    query.setForwardOnly(true);

    if (!query.exec(sql)) {
        qDebug() << "SQL error:" << query.lastError().text();
        return 1;
    }

    // 3. Создаем Excel-документ
    Document xlsx;

    // Заголовок
    Format titleFormat;
    titleFormat.setFontBold(true);
    titleFormat.setFontSize(14);
    xlsx.write("A1", "People Report", titleFormat);

    // Шапка колонок
    QSqlRecord rec = query.record();
    Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setHorizontalAlignment(Format::AlignHCenter);

    for (int col = 0; col < rec.count(); ++col) {
        xlsx.write(2, col + 1, rec.fieldName(col), headerFormat); // 2-я строка
    }

    // Данные
    int row = 3;
    while (query.next()) {
        for (int col = 0; col < rec.count(); ++col) {
            xlsx.write(row, col + 1, query.value(col));
        }
        row++;
    }

    // 4. Сохраняем файл
    QString filePath = QCoreApplication::applicationDirPath() + "/report.xlsx";
    if (!xlsx.saveAs(filePath)) {
        qDebug() << "Failed to save XLSX file";
        return 1;
    }

    qDebug() << "Saved:" << filePath;

    // 5. Открываем файл в системе
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));

    return 0;
}


int viewTuItog(QSqlDatabase &db, const QString &sql)
{
    QSqlQuery query(db);

    query.setForwardOnly(true);

    if (!query.exec(sql)) {
        qDebug() << "SQL error:" << query.lastError().text();
        return 1;
    }

    // 3. Создаем Excel-документ
    Document xlsx;

    // Заголовок
    Format titleFormat;
    titleFormat.setFontBold(true);
    titleFormat.setFontSize(14);
    xlsx.write("A1", "People Report", titleFormat);

    // Шапка колонок
    QSqlRecord rec = query.record();
    Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setHorizontalAlignment(Format::AlignHCenter);

    for (int col = 0; col < rec.count(); ++col) {
        xlsx.write(2, col + 1, rec.fieldName(col), headerFormat); // 2-я строка
    }

    // Данные
    int row = 3;
    while (query.next()) {
//        for (int col = 0; col < rec.count(); ++col) {
//            xlsx.write(row, col + 1, query.value(col));
//        }
        
        double d;

        int col = 0;

        QString ist = query.value(transl("Наименование")).toString();
        xlsx.write(row, col ++, ist);
        d = query.value("ust_m").toDouble();
        xlsx.write(row, col ++, d);
        d = query.value(transl("Располагаемая мощность ОВ")).toDouble();
        xlsx.write(row, col ++, d);
        d = query.value(transl("Располагаемая мощность ГВС средняя")).toDouble();
        xlsx.write(row, col ++, d);
        d = query.value(transl("Располагаемая мощность суммарная")).toDouble();
        xlsx.write(row, col ++, d);
        d = query.value(transl("Присоединенная мощность Отопление")).toDouble();
        xlsx.write(row, col ++, d);
        d = query.value(transl("Присоединенная мощность Вентиляция")).toDouble();
        xlsx.write(row, col ++, d);
        d = query.value(transl("Присоединенная мощность ГВС максимальная")).toDouble();
        xlsx.write(row, col ++, d);
        d = query.value(transl("Присоединенная мощность ГВС средняя")).toDouble();
        xlsx.write(row, col ++, d);
        d = query.value(transl("Пар")).toDouble();
        xlsx.write(row, col ++, d);
        d = query.value(transl("Нормативные тепловые потери")).toDouble();
        xlsx.write(row, col ++, d);
        
        row++;
    }

    // 4. Сохраняем файл
    QString filePath = QCoreApplication::applicationDirPath() + "/report.xlsx";
    if (!xlsx.saveAs(filePath)) {
        qDebug() << "Failed to save XLSX file";
        return 1;
    }

    qDebug() << "Saved:" << filePath;

    // 5. Открываем файл в системе
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));

    return 0;
}




static QString kot = "Источники тепла";


static QString TechUsl = "Технические условия";


void print_tu_itog(QSqlDatabase &db, int y1, int y2, int y, double coef)
{
    QString q;

    q = QString("select * from %1 limit 100").arg(transl(TechUsl));

    int m_y0 = 2011;

    q = QString(R"(
            SELECT *, PRN.ustanovlennaya_moschnost AS ust_m 
            FROM %1 PRN 
            INNER JOIN (
                %2 K 
                LEFT JOIN %3 RE ON K.%4 = RE.id
            ) ON PRN.id2 = K.id 
            WHERE PRN.god=%5 ORDER BY %6
            limit 100
            )") 
        .arg(transl("Присоединенная нагрузка источников"))
        .arg(transl(kot))
        .arg(transl("Район эксплуатации"))
        .arg(transl("Район эксплуатации"))
        .arg(m_y0)
        .arg(transl("Номер по порядку"))
        ;

    qDebug() << q;


    viewTuItog(db, q);
}
