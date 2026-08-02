#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include <gidview/GidWidget.h>
#include <gidview/colorgtd.h>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>


#include "Pjezo.h"

#include "xlsxdocument.h"

using namespace QXlsx;


//void writePjezo(int i, char po, bool dbl, QVector<QString> &v,
//  vector<CPLine> &m_vlineP, vector<CPLine> &m_vlineO,  vector<CPNode> &m_vnode) {};

void writePjezo(int i, char po, bool dbl, QVector<QString>& v,
    vector<CPLine>& m_vlineP,
    vector<CPLine>& m_vlineO,
    vector<CPNode>& m_vnode
)
{
    int k = 0;
    QString str;

    v.push_back(m_vnode[i].rn[0].n->getKod());
    v.push_back(m_vnode[i].rn[0].n->node.node_name.name);
    v.push_back(po == CPOD ? S_POD : S_OBR);
    v.push_back(m_vnode[i + 1].rn[0].n->getKod());
    v.push_back(m_vnode[i + 1].rn[0].n->node.node_name.name);
    v.push_back(po == CPOD ? S_POD : S_OBR);

    double dh = m_vnode[i].rn[0].h - m_vnode[i + 1].rn[0].h;

    if (!dbl) {
        if (po == CPOD) {
            str = QString("%1").arg(m_vlineP[i].rl[0].g); v.push_back(str);
            str = QString("%1").arg(m_vnode[i].rn[0].pihP1); v.push_back(str);
            str = QString("%1").arg(m_vnode[i + 1].rn[0].pihP1); v.push_back(str);

            str = QString("%1").arg(m_vlineP[i].rl[0].a14); v.push_back(str);
            str = QString("%1").arg(m_vlineP[i].rl[0].a15); v.push_back(str);
            str = QString("%1").arg(m_vlineP[i].rl[0].a16); v.push_back(str);


            double P1 = m_vnode[i].rn[0].pihP1;
            double P2 = m_vnode[i + 1].rn[0].pihP1;

            //    str = QString("%g").arg(m_vnode[i+1].pihP1-m_vnode[i].rn[0].pihP1+dh); v.push_back(str);
            str = QString("%1").arg(m_vlineP[i].rl[0].a17); v.push_back(str);

            str = QString("%1").arg(m_vlineP[i].rl[0].w); v.push_back(str);
        }
        else {
            str = QString("%1").arg(m_vlineO[i].rl[0].g); v.push_back(str);
            str = QString("%1").arg(m_vnode[i].rn[0].pihO1); v.push_back(str);
            str = QString("%1").arg(m_vnode[i + 1].rn[0].pihO1); v.push_back(str);

            str = QString("%1").arg(m_vlineO[i].rl[0].a14); v.push_back(str);
            str = QString("%1").arg(m_vlineO[i].rl[0].a15); v.push_back(str);
            str = QString("%1").arg(m_vlineO[i].rl[0].a16); v.push_back(str);

            //    str = QString("%g").arg(m_vnode[i+1].pihO1-m_vnode[i].rn[0].pihO1+dh); v.push_back(str);
            str = QString("%1").arg(m_vlineO[i].rl[0].a17); v.push_back(str);

            str = QString("%1").arg(m_vlineO[i].rl[0].w); v.push_back(str);
        }
    }

    if (dbl) {
        if (po == CPOD) {
            str = QString("%1").arg(m_vlineP[i].rl[0].g); v.push_back(str);
            str = QString("%1").arg(m_vnode[i].rn[0].pihP1); v.push_back(str);
            str = QString("%1").arg(m_vnode[i + 1].rn[0].pihP1); v.push_back(str);
            str = QString("%1").arg(m_vlineP[i].rl[0].a17); v.push_back(str);
        }
        else {
            str = QString("%1").arg(m_vlineO[i].rl[0].g); v.push_back(str);
            str = QString("%1").arg(m_vnode[i].rn[0].pihO1); v.push_back(str);
            str = QString("%1").arg(m_vnode[i + 1].rn[0].pihO1); v.push_back(str);
            str = QString("%1").arg(m_vlineO[i].rl[0].a17); v.push_back(str);
        }
        if (po == CPOD) {
            str = QString("%1").arg(m_vlineP[i].rl[1].g); v.push_back(str);
            str = QString("%1").arg(m_vnode[i].rn[1].pihP1); v.push_back(str);
            str = QString("%1").arg(m_vnode[i + 1].rn[1].pihP1); v.push_back(str);
            str = QString("%1").arg(m_vlineP[i].rl[1].a17); v.push_back(str);
        }
        else {
            str = QString("%1").arg(m_vlineO[i].rl[1].g); v.push_back(str);
            str = QString("%1").arg(m_vnode[i].rn[1].pihO1); v.push_back(str);
            str = QString("%1").arg(m_vnode[i + 1].rn[1].pihO1); v.push_back(str);
            str = QString("%1").arg(m_vlineO[i].rl[1].a17); v.push_back(str);
        }
    }
    //
    str = QString("%1").arg(m_vlineP[i].rl[0].len); v.push_back(str);
    str = QString("%1").arg(m_vlineP[i].rl[0].diam); v.push_back(str);
    str = QString("%1").arg(m_vnode[i + 1].rn[0].lenO); v.push_back(str);
    str = QString("%1").arg(m_vnode[i + 1].rn[0].vO); v.push_back(str);
}


void write(QXlsx::Document &xlsxW, const QString &range, const QString &text, const QXlsx::Format & format)
{
    xlsxW.mergeCells(range, format);
    QString cell = range.split(":").first();
    xlsxW.write(cell, text, format);
}



void CPjezo::onExcel()
{

    QXlsx::Document xlsxW;


    xlsxW.addSheet("Техн.информация");

#if 1
/*
    CExcel ex;

    QString xn1;

    int n1 = 14;

    if (m_dbl) {
        xn1 = QString("%sExcel2/xls/PZ2.xls").arg(argpath());
        n1 = 13;
        n1 = 15;
    }
    else {
        xn1 = QString("%sExcel2/xls/PZ1.xls").arg(argpath());
        n1 = 14;
    }

    ex.open(xn1);

    if (ex.IsOpen()) {
        ex.setTab(1);


        ex.init_arr(m_vnode.size() * 2 + 20, 20);

*/
        int i;

        QXlsx::Format format;

        format.setTextWrap(true);

        format.setVerticalAlignment(QXlsx::Format::AlignVCenter); 
        format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

        format.setBorderStyle(QXlsx::Format::BorderThin); // Тонкая рамка

        QXlsx::Format row_data;

        xlsxW.write(3, 5, "ТЕХНОЛОГИЧЕСКАЯ ИНФОРМАЦИЯ К ПЬЕЗОМЕТРИЧЕСКОМУ ГРАФИКУ", row_data);


        write(xlsxW, "A6:C7", "Начальный узел участка", format);
        write(xlsxW, "D6:F7", "Конечный узел участка", format);
        write(xlsxW, "G6:N7", "Параметры режима", format);
        write(xlsxW, "A8:A13", "Код расчетной схемы", format);
        write(xlsxW, "B8:B13", "Наименование узла", format);
        write(xlsxW, "C8:C13", "Признак трубопровода", format);

        write(xlsxW, "D8:D13", "Код расчетной схемы", format);
        write(xlsxW, "E8:E13", "Наименование узла", format);
        write(xlsxW, "F8:F13", "Признак трубопровода", format);


        write(xlsxW, "G8:G13", "Расход воды на участке, т/ч", format);
        write(xlsxW, "H8:I9", "Пьезометрический напор, м", format);
        write(xlsxW, "H10:H13", "в начальном узле", format);
        write(xlsxW, "I10:I13", "в конечном узле", format);

        write(xlsxW, "J8:M9", "Потери напора на участке, м", format);
        write(xlsxW, "J10:J13", "удельные", format);
        write(xlsxW, "K10:K13", "линейные", format);
        write(xlsxW, "L10:L13", "местные", format);
        write(xlsxW, "M10:M13", "общие", format);

        write(xlsxW, "N8:N13", "Скорость потока воды на участке, м/c", format);
        write(xlsxW, "O6:O13", "Длина участка трубопровода, м", format);

        write(xlsxW, "P6:P13", "Внутренний диаметр трубопровода, мм", format);
        write(xlsxW, "Q6:Q13", "Расстояние от начального узла направления, м", format);
        write(xlsxW, "R6:R13", "Объем от начального узла направленияч, м3", format);

        int row0 = 14;

       for (int k = 0; k < 18; k++) {
            int col = k+1;
            QXlsx::Format row_data;

            xlsxW.write(row0, col, QString("%1").arg(k+1), format);
        }

        for (i = 0; i < m_vnode.size() - 1; i++) {
            {
                QVector<QString> v;
                writePjezo(i, CPOD, m_dbl, v, m_vlineP, m_vlineO, m_vnode);

                for (int k = 0; k < v.size(); k++) {
//                    ex.set_arr2(CRange(i * 2 + 1, k + 1), COleVariant(v[k]));
                    int col = k+1;
                    int row = i*2+1;
                    QXlsx::Format row_data;
                    xlsxW.write(row0+row, col, v[k], row_data);
                }
            }
            {
                QVector<QString> v;
                writePjezo(i, COBR, m_dbl, v, m_vlineP, m_vlineO, m_vnode);

                for (int k = 0; k < v.size(); k++) {
//                    ex.set_arr2(CRange(i * 2 + 2, k + 1), COleVariant(v[k]));
                    int col = k+1;
                    int row = i*2+2;
                    QXlsx::Format row_data;
                    xlsxW.write(row0+row, col, v[k], row_data);
                }
            }
        }
/*

        ex.set_typ(CRange(1, 1, m_vnode.size() * 2 + 20 + 20, 20), dbText);
        ex.example2(CRange(n1, 0), 1);

        CRect rect = m_rect;

        m_rect = CRect(0, 0, 1024, 768);
        CopyBlockP(m_rect);
        m_rect = rect;

        ex.copy(CRange(1, 1), 2);
        //    ShowExcel();
    }
*/
#endif

    QString fn = QString("%1/tgid.xlsx").arg(QDir::tempPath());

//    QString filename = "";


    xlsxW.addSheet("Пьез.график");

    onEditCopy();
    // Получаем изображение из буфера обмена
    const QClipboard *clipboard = QApplication::clipboard();
    QPixmap pixmap = clipboard->pixmap();

    if (pixmap.isNull()) {
        qDebug() << "Буфер обмена пуст или не содержит изображение!";
        return;
    }

    // Сохраняем изображение во временный файл
    QString imagePath = "clipboard_image.png";

    imagePath = QString("%1/clipboard_image.png").arg(QDir::tempPath());

    if (!pixmap.save(imagePath)) {
        qDebug() << "Ошибка сохранения изображения!";
        return;
    }

    QImage image (imagePath);
    image.setDotsPerMeterX(11811);
    image.setDotsPerMeterY(11811);

    xlsxW.insertImage(0, 0, image);

    QString firstSheet = xlsxW.sheetNames().first();
    xlsxW.selectSheet(firstSheet);

    qDebug() << fn;

    if ( xlsxW.saveAs(fn) )
    {
        QString path = QString("file:///%1").arg(fn);
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    }

}
