#include <QApplication>
#include <QtGui>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>

#include <QInputDialog>
#include <any/rights.h>

#include <db/db.h>

#include "std.h"

#include <ui2/param_pt.h>

struct PT_FN {
  QString fn;
  double d1, d2;
};


#if 0
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
#endif
static PT_FN pt_fn[] = {
{"Qotz", 0, 1,}, //  Тепл.нагрузка  на отопление, зав.сх.   
{"Qotn", 0, 1,}, //  Тепл.нагрузка  на отопление, незав.сх. 
{"dop12", 0, 1,}, //  Тепловая нагрузка потребителя на вентиляцию 
{"dop13", 0, 1,}, //  Тепл.нагрузка на кондиционирование  
{"dop17", 0, 0.5,}, //  Тепл.нагрузка на ГВ в закрытой системе  0, 
{"dop18", 0, 0.5,}, //  Тепл.нагрузка на ГВ из под.теплопр. в откр.сист.  0, 
{"dop19", 0, 0.5,}, //  Тепл.нагрузка на ГВ из обр.теплопр. в откр.сист.  0, 
{"dop20", 0, 0.2,}, //  Тепл.нагрузка в цирк.теплопроводе ГВС 0, 
{"Qsum_z", 0, 1.5,}, //  Суммарная тепл.нагрузка  в закрытой системе  0, 
{"Qfact", 0, 1.5,}, //  Суммарная тепловая нагрузка систем теплопотребления 0, 
{"Q_obesp_min", 0, 95},  //  Требуемый расход тепла  на горячее водоснабжение  в открытой системе из под.трубопр.
{"Q_obesp_otopl_min", 0, 95},  //  Требуемый расход тепла  на горячее водоснабжение  в открытой системе из обр.трубопр.
{"Q_obesp_gvs_min", 0, 95},  //  Требуемый расход тепла на рециркуляцию в  в открытой системе ГВ
};


QString findColumnRusName(const QString & d, const QString & n1, const QString & n2);

class ParamPT : public QDialog {
     Q_OBJECT
public:
    ParamPT(QWidget *parent = nullptr) : QDialog(parent) {
        ui.setupUi(this);  // ui - это объект, сгенерированный из .ui файла

        for (int i = 0; i < sizeof(pt_fn)/sizeof(pt_fn[0]); i++) {
            QString rus = findColumnRusName("gid", "PT_OUT", pt_fn[i].fn);
            ui.comboBox->addItem(rus);
        }

        m_color[0+3] = m_color[0] = 0x0000FF;
        m_color[1+3] = m_color[1] = 0x00FF00;
        m_color[2+3] = m_color[2] = 0xFF0000;

        ui.pushButton_P_1->setStyleSheet(QString("background-color: #%1").arg(m_color[0], 6, 16, QChar('0')));
        ui.pushButton_P_2->setStyleSheet(QString("background-color: #%1").arg(m_color[1], 6, 16, QChar('0')));
        ui.pushButton_P_3->setStyleSheet(QString("background-color: #%1").arg(m_color[2], 6, 16, QChar('0')));

        connect(ui.pushButton_P_1, &QPushButton::clicked, this, &ParamPT::onButtonClicked);
        connect(ui.pushButton_P_2, &QPushButton::clicked, this, &ParamPT::onButtonClicked);
        connect(ui.pushButton_P_3, &QPushButton::clicked, this, &ParamPT::onButtonClicked);

        QDoubleValidator *validator = new QDoubleValidator(-10000.0, 10000.0, 2); // Диапазон от -100 до 100, 2 знака после запятой
        validator->setNotation(QDoubleValidator::StandardNotation); // Обычный ф

        ui.lineEdit_P_1->setValidator(validator);
        ui.lineEdit_P_2->setValidator(validator);

        ui.lineEdit_P_1_->setReadOnly(true);
        ui.lineEdit_P_3->setReadOnly(true);

        QObject::connect(ui.lineEdit_P_1, &QLineEdit::textChanged, ui.lineEdit_P_1_, &QLineEdit::setText);
        QObject::connect(ui.lineEdit_P_2, &QLineEdit::textChanged, ui.lineEdit_P_3, &QLineEdit::setText);

        QObject::connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &ParamPT::ok );
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
        m_index = ui.comboBox->currentIndex();
        if (m_index >= 0) {
            m_fn = pt_fn[m_index];
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
    PT_FN m_fn;

private:

    void init(int index) {
        if (index < 0) return;

        PT_FN ut = pt_fn[index];

        ui.lineEdit_P_1->setText(QString("%1").arg(ut.d1));
        ui.lineEdit_P_1_->setText(QString("%1").arg(ut.d1));
        ui.lineEdit_P_2->setText(QString("%1").arg(ut.d2));
        ui.lineEdit_P_3->setText(QString("%1").arg(ut.d2));
    }

    Ui_ParamPT ui;  // Имя класса, сгенерированного в .ui
};



void GidWidget::onColorLine1() // Потребители
{
    ParamPT dlg(this);


    if (dlg.exec() != QDialog::Accepted || dlg.m_index < 0) return;
        
        QString q = QString(
            "SELECT id, %1 FROM PT_OUT\n"
            " JOIN\n"
            " (\n"
            "     SELECT\n"
            "     c.fileID,\n"
            "     max(c.id) AS cid\n"
            "     FROM CALCULATION c\n"
            "     LEFT JOIN fragments fr ON fr.id = c.fileID\n"
            "     GROUP BY c.fileID\n"
            " ) calc ON PT_OUT.calculationID = calc.cid\n"

            " WHERE fileID IN (%2)").arg(dlg.m_fn.fn, m_cxema.m_par);

        m_cxema.m_graph->init_find_node_nom_pt_out();

        QSqlQuery query(m_cxema.m_db);
        query.setForwardOnly(true);
        if (query_exec(m_cxema.m_db, query, q)) {
            while (query.next()) {
                int id = query.value("id").toInt();
//                int po = query.value("externalSignLineID").toInt();
                double d = query.value(dlg.m_fn.fn).toDouble();

                CNode2* n = m_cxema.m_graph->find_pt_out(id);

                if (n) {

                    if (d <= dlg.vP1) n->node.colorP = dlg.m_color[0];
                    else if (d <= dlg.vP2) n->node.colorP = dlg.m_color[1];
                    else n->node.colorP = dlg.m_color[2];
                }
            }
            repaint();
        }
    }
bool GidWidget::setNodeColorById(int id, int color) {
    if (!m_cxema.m_graph) return false;

    if (CNode2* n = m_cxema.m_graph->find(id)) {
        n->node.colorP = color;
        n->node.isMarkAnalyze = true;
        return true;
    }

    return false;
}

void GidWidget::applyNodeColor(const QVector<int>& ids, int color) {
    if (ids.isEmpty()) return;
    //m_cxema.m_graph->init_find_node_nom_pt_out();
    //m_cxema.m_graph->init_find_node_2();
    for (int id : ids) {
        setNodeColorById(id, color);
    }
    repaint();
}

void GidWidget::applyNodeColors(const QHash<int, int>& idToColor) {
    if (idToColor.isEmpty()) return;
    //m_cxema.m_graph->init_find_node_nom_pt_out();
    //m_cxema.m_graph->init_find_node_2();
    for (auto it = idToColor.constBegin(); it != idToColor.constEnd(); ++it) {
        setNodeColorById(it.key(), it.value());
    }
    repaint();
}


#include "colorLine1.moc"
