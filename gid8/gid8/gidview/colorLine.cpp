#include <QApplication>
#include <QtGui>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>

#include <QInputDialog>
#include <any/rights.h>

#include <db/db.h>

#include "std.h"

#include <ui2/param_ut.h>


struct UT_FN {
  QString fn;
  double d1, d2;
  QString rus;
  bool out;
};

static UT_FN ut_fn[] = {
{"a18", 6,15, "Располагаемый напор в конечном узле", true},  //  Располагаемый напор в конечном узле участка
{"a17", 0.01,2, "Общие потери напора", true},  //  Общие потери напора на участке
{"a14", 0.1,1, "Удельные линейные потери напора", true},  //  Удельные линейные потери напора на участке
{"a7", 10,100, "Длина,м", true},  //  Длина участка трубопровода
{"a8", 200,400, "Внутренний диаметр,мм", true},  //  Внутренний диаметр трубы участка
{"a9", 0.1,10, "Объем воды,м3", true},  //  Объем воды на участке трубопровода
{"a10", 0.5,2, "Скорость потока", true},  //  Скорость потока сетевой воды
{"t1", 0,70, "Температура на входе", true},
{"a13", 0,10, "Расход сетевой воды", true},  //  Расход сетевой воды на участке
{"a20", 0,360, "Геодезич.отметка в конечном узле", true},  //  Геодезич.отметка местности в конечном узле участка
//{"tzam", 0, 24, ""},  //  
{"tubeRoughness", 0.5, 10, "Шероховатость", false},  //  

};


class ParamUT : public QDialog {
     Q_OBJECT
public:
    ParamUT(QWidget *parent = nullptr) : QDialog(parent) {
        ui.setupUi(this);  // ui - это объект, сгенерированный из .ui файла

        for (int i = 0; i < sizeof(ut_fn)/sizeof(ut_fn[0]); i++) {
            ui.comboBox->addItem(ut_fn[i].rus);
        }

        m_color[0+3] = m_color[0] = 0x0000FF;
        m_color[1+3] = m_color[1] = 0x00FF00;
        m_color[2+3] = m_color[2] = 0xFF0000;

        m_color[0+3] = m_color[0] = 0xFF0000;
        m_color[1+3] = m_color[1] = 0x00FF00;
        m_color[2+3] = m_color[2] = 0x0000FF;

        ui.pushButton_P_1->setStyleSheet(QString("background-color: #%1").arg(m_color[0], 6, 16, QChar('0')));
        ui.pushButton_P_2->setStyleSheet(QString("background-color: #%1").arg(m_color[1], 6, 16, QChar('0')));
        ui.pushButton_P_3->setStyleSheet(QString("background-color: #%1").arg(m_color[2], 6, 16, QChar('0')));

        ui.pushButton_O_1->setStyleSheet(QString("background-color: #%1").arg(m_color[3], 6, 16, QChar('0')));
        ui.pushButton_O_2->setStyleSheet(QString("background-color: #%1").arg(m_color[4], 6, 16, QChar('0')));
        ui.pushButton_O_3->setStyleSheet(QString("background-color: #%1").arg(m_color[5], 6, 16, QChar('0')));

        connect(ui.pushButton_P_1, &QPushButton::clicked, this, &ParamUT::onButtonClicked);
        connect(ui.pushButton_P_2, &QPushButton::clicked, this, &ParamUT::onButtonClicked);
        connect(ui.pushButton_P_3, &QPushButton::clicked, this, &ParamUT::onButtonClicked);
        connect(ui.pushButton_O_1, &QPushButton::clicked, this, &ParamUT::onButtonClicked);
        connect(ui.pushButton_O_2, &QPushButton::clicked, this, &ParamUT::onButtonClicked);
        connect(ui.pushButton_O_3, &QPushButton::clicked, this, &ParamUT::onButtonClicked);

        QDoubleValidator *validator = new QDoubleValidator(-10000.0, 10000.0, 2); // Диапазон от -100 до 100, 2 знака после запятой
        validator->setNotation(QDoubleValidator::StandardNotation); // Обычный ф

        ui.lineEdit_P_1->setValidator(validator);
        ui.lineEdit_P_2->setValidator(validator);
        ui.lineEdit_O_1->setValidator(validator);
        ui.lineEdit_O_2->setValidator(validator);

        ui.lineEdit_P_1_->setReadOnly(true);
        ui.lineEdit_P_3->setReadOnly(true);
        ui.lineEdit_O_1_->setReadOnly(true);
        ui.lineEdit_O_3->setReadOnly(true);

        QObject::connect(ui.lineEdit_P_1, &QLineEdit::textChanged, ui.lineEdit_P_1_, &QLineEdit::setText);
        QObject::connect(ui.lineEdit_P_2, &QLineEdit::textChanged, ui.lineEdit_P_3, &QLineEdit::setText);
        QObject::connect(ui.lineEdit_O_1, &QLineEdit::textChanged, ui.lineEdit_O_1_, &QLineEdit::setText);
        QObject::connect(ui.lineEdit_O_2, &QLineEdit::textChanged, ui.lineEdit_O_3, &QLineEdit::setText);

        QObject::connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &ParamUT::ok );
        QObject::connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

        QPushButton *okButton = ui.buttonBox->button(QDialogButtonBox::Ok);
        if (okButton) {
            okButton->setText("OK");
        }
        QPushButton *cancelButton = ui.buttonBox->button(QDialogButtonBox::Cancel);
        if (cancelButton) {
            cancelButton->setText("Отменить");
        }


        init(0);
        connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));

//        connect(ui.push_Ok, &QPushButton::clicked, this, &Password3::close);
//        connect(ui.push_Cancel, &QPushButton::clicked, this, &Password3::close);
//        connect(ui.pushButton_Edit, &QPushButton::clicked, this, &Password3::edit);
//        connect(ui.pushButton_Delete, &QPushButton::clicked, this, &Password3::del);
    }

public slots:

    void ok()
    {
        vP1 = ui.lineEdit_P_1->text().toDouble();
        vP2 = ui.lineEdit_P_2->text().toDouble();
        vO1 = ui.lineEdit_O_1->text().toDouble();
        vO2 = ui.lineEdit_O_2->text().toDouble();
        m_index = ui.comboBox->currentIndex();
        if (m_index >= 0) {
            m_fn = ut_fn[m_index];
        }
        accept();
    }

    void currentIndexChanged(int index)
    {
        init(index);
    }

    void onButtonClicked() {
        QPushButton *button = qobject_cast<QPushButton *>(sender());
        if (button) {

            QColor backgroundColor = button->palette().color(QPalette::Button);
//            QColor color = QColorDialog::getColor(backgroundColor, this);
            QColorDialog dlg(backgroundColor, this);
            if (dlg.exec()) {
                QColor color = dlg.currentColor().rgb();
                button->setStyleSheet(QString("background-color: #%1").arg(color.name()));
            }


//            qDebug() << "Background color (palette):" << backgroundColor.name();


#if 0
            QString styleSheet = button->styleSheet();
            QRegularExpression regex("background-color:\\s*rgb\\((\\d+),\\s*(\\d+),\\s*(\\d+)\\)");
            QRegularExpressionMatch match = regex.match(styleSheet);

            if (match.hasMatch()) {
                int red = match.captured(1).toInt();
                int green = match.captured(2).toInt();
                int blue = match.captured(3).toInt();

                QColor backgroundColor(red, green, blue);
//                qDebug() << "Background color (styleSheet):" << backgroundColor.name();
                QColor color = QColorDialog::getColor(backgroundColor, this);
            
            } 
            else {
//                qDebug() << "Background color not found in styleSheet.";
            }

#endif
//            QMessageBox::warning(this, "", backgroundColor.name());
        }
    }


public:
    int m_color[6];
    double vP1, vP2;
    double vO1, vO2;
    int m_index = -1;
    UT_FN m_fn;

private:

    void init(int index) {
        if (index < 0) return;

        UT_FN ut = ut_fn[index];

        ui.lineEdit_P_1->setText(QString("%1").arg(ut.d1));
        ui.lineEdit_P_1_->setText(QString("%1").arg(ut.d1));
        ui.lineEdit_P_2->setText(QString("%1").arg(ut.d2));
        ui.lineEdit_P_3->setText(QString("%1").arg(ut.d2));

        ui.lineEdit_O_1->setText(QString("%1").arg(ut.d1));
        ui.lineEdit_O_1_->setText(QString("%1").arg(ut.d1));
        ui.lineEdit_O_2->setText(QString("%1").arg(ut.d2));
        ui.lineEdit_O_3->setText(QString("%1").arg(ut.d2));
    }

    Ui_ParamUT ui;  // Имя класса, сгенерированного в .ui
};



void GidWidget::onColorLine() // Участки
{
    ParamUT dlg(this);


    if (dlg.exec() != QDialog::Accepted || dlg.m_index < 0) return;
        
    if (dlg.m_fn.out) {
        QString q = QString(
                        "SELECT id, externalSignLineID, %1 FROM UT_OUT"
                        " JOIN"
                        " ("
                        "     SELECT"
                        "     c.fileID,"
                        "     max(c.id) AS cid"
                        "     FROM CALCULATION c"
                        "     LEFT JOIN fragments fr ON fr.id = c.fileID"
                        "     GROUP BY c.fileID"
                        " ) calc ON UT_OUT.calculationID = calc.cid"

                        " WHERE fileID IN (%2)").arg(dlg.m_fn.fn, m_cxema.m_par);

//        m_cxema.m_graph->init_find_line_nom();
        m_cxema.m_graph->init_find_line_nom_out(TIP_UT);

        QSqlQuery query(m_cxema.m_db);
        query.setForwardOnly(true);
        if (query_exec(m_cxema.m_db, query, q)) {
            while (query.next()) {
                int id = query.value(0).toInt();
                int po = query.value(1).toInt();
                double d = query.value(2).toDouble();

//                CLINE2* ll = m_cxema.m_graph->find_line_nom(id);
                CLINE2* ll = m_cxema.m_graph->find_line_nom_out(id);
                if (ll) {
                    CLine2* l = bline(ll);

                    if (po == 1 || po == 2 || po == 4) {
                        if (d <= dlg.vP1) l->line.colorP = dlg.m_color[0];
                        else if (d <= dlg.vP2) l->line.colorP = dlg.m_color[1];
                        else l->line.colorP = dlg.m_color[2];
                    }
                    if (po == 1 || po == 3 || po == 5) {
                        if (d <= dlg.vP1) l->line.colorO = dlg.m_color[3];
                        else if (d <= dlg.vO2) l->line.colorO = dlg.m_color[4];
                        else l->line.colorO = dlg.m_color[5];
                    }
                }
            }
            repaint();
        }
    }
    else {
        QString q = QString(
                        "SELECT l.id, l.externalSignLineID, hps.%1 FROM heatPipeSections hps\n"
                        "join linesobj l on l.id = hps.lineID\n"
                        "join nodes n on n.id = l.nodeID1\n"
                        " WHERE n.fileID IN (%2)").arg(dlg.m_fn.fn, m_cxema.m_par);


        m_cxema.m_graph->init_find_line_nom();
//        m_cxema.m_graph->init_find_line_nom_out(TIP_UT);

        QSqlQuery query(m_cxema.m_db);
        query.setForwardOnly(true);
        if (query_exec(m_cxema.m_db, query, q)) {
            while (query.next()) {
                int id = query.value("id").toInt();
                int po = query.value("externalSignLineID").toInt();
                double d = query.value(dlg.m_fn.fn).toDouble();

                //                CLINE2* ll = m_cxema.m_graph->find_line_nom(id);
                CLINE2* ll = m_cxema.m_graph->find_line_nom(id);
                if (ll) {
                    CLine2* l = bline(ll);

                    if (po == 1 || po == 2 || po == 4) {
                        if (d <= dlg.vP1) l->line.colorP = dlg.m_color[0];
                        else if (d <= dlg.vP2) l->line.colorP = dlg.m_color[1];
                        else l->line.colorP = dlg.m_color[2];
                    }
                    if (po == 1 || po == 3 || po == 5) {
                        if (d <= dlg.vP1) l->line.colorO = dlg.m_color[3];
                        else if (d <= dlg.vO2) l->line.colorO = dlg.m_color[4];
                        else l->line.colorO = dlg.m_color[5];
                    }
                }
            }
            repaint();
        }

    }

}

#include "colorLine.moc"
