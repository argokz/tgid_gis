#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include <table/DbWindow.h>
#include <dialog/SaveGeometry.h>
#include <db/db.h>
#include "mystd.h"
#include "property/HelpDialog.h"
#include "std.h"

#include "MestSopr3.h"

bool is_POSTGRESQL();

MestSopr3::MestSopr3(QWidget *parent, QSqlDatabase &db, int id, const QString & mestn)
    : QDialog(parent)
    , save(this, "MestSopr3")
{
    QSplitter *splitter = new QSplitter;

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QVBoxLayout *tBox = new QVBoxLayout;
    QHBoxLayout *row2Box = new QHBoxLayout;

    QString q = "SELECT * FROM sprav.dbo.[08_mestnye_gidravlicheskie_soprotivleniya]";

    q = QString("SELECT sp.id, sp.name_rm, sp.mestnoe_min, sp.mestnoe_max, hr.k_mest, hr.sum_mest FROM sprav.dbo.[08_mestnye_gidravlicheskie_soprotivleniya] sp\n"
//                "LEFT JOIN localHydroResistances2 hr on hr.name_mest=sp.name_rm and lineID=%1\n"
                "order by sp.name_rm"); //.arg(id);

    q = QString("SELECT sp.id, sp.name_rm, sp.mestnoe_min, sp.mestnoe_max FROM sprav.dbo.[08_mestnye_gidravlicheskie_soprotivleniya] sp\n"
                "order by sp.name_rm"); //.arg(id);


    if (is_POSTGRESQL()) {
//        q = QString("SELECT sp.id, sp.name_rm, sp.mestnoe_min, sp.mestnoe_max, hr.k_mest, hr.sum_mest FROM \"08_mestnye_gidravlicheskie_soprotivleniya\" sp\n"
//                "LEFT JOIN localHydroResistances2 hr on hr.name_mest=sp.name_rm and lineID=%1\n"
        q = QString("SELECT sp.id, sp.name_rm, sp.mestnoe_min, sp.mestnoe_max FROM \"08_mestnye_gidravlicheskie_soprotivleniya\" sp\n"
                "order by sp.name_rm");
    }

    q = readQ("sql/sp/s08.sql");

    QSqlQuery query(db);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString name_mest = query.value(1).toString();
            double v1 = query.value(2).toDouble();
            double v2 = query.value(3).toDouble();

//            double s_mest = query.value(4).toDouble();
//            double k_mest = query.value(5).toInt();


//            auto lineEdit = new QLineEdit;
//            lineEdit->setText(name_mestn);

            QHBoxLayout *rowLayout = new QHBoxLayout;


            QString name;

            if (v1 == v2) {
                name = QString("%1 %2").arg(name_mest).arg(v1);
            }
            else {
                name = QString("%1 %2-%3").arg(name_mest).arg(v1).arg(v2);
            }


            auto label = new QLabel(name);

            double width = 300;
//            label->setFixedWidth(width);

            auto *kEdit = new QSpinBox();
            kEdit->setValue(0);
//            kEdit->setValue(k_mest);

            kEdit->setFixedWidth(150);

            auto *vEdit = new QDoubleSpinBox();

            vEdit->setFixedWidth(150);
            
            vEdit->setValue(v1);
            vEdit->setRange(v1, v2);

            map_l[id] = label;
            map_k[id] = kEdit;
            map_v[id] = vEdit;

            rowLayout->addWidget(label);
            rowLayout->addWidget(kEdit);
            rowLayout->addWidget(vEdit);
            
            tBox->addLayout(rowLayout);

            connect(vEdit, SIGNAL(textChanged(const QString &)),
                    this, SLOT(vChanged(const QString &)));

            connect(kEdit, SIGNAL(textChanged(const QString &)),
                    this, SLOT(vChanged(const QString &)));
        }
    }

    QLabel *label = new QLabel(tr("Сумма коэффициентов местных сопротивлений"));

    sopr = new QLineEdit;
    sopr->setReadOnly(true);

    QPushButton *okButton = new QPushButton(tr("&Ok"));
    QPushButton *closeButton = new QPushButton(tr("Отменить"));
    QPushButton *helpButton = new QPushButton(tr("Помощь"));

    m_sum = calcSum();

    sopr->setText(QString("%1").arg(m_sum));

    row2Box->addWidget(label);
    row2Box->addWidget(sopr);
    row2Box->addStretch();
    row2Box->addWidget(helpButton);
    row2Box->addWidget(okButton);
    row2Box->addWidget(closeButton);

//    tBox->addWidget(tb1);

//    splitter->setOrientation(Qt::Vertical);

    QWidget *w1 = new QWidget;

    w1->setLayout(tBox);

//    splitter->addWidget(w1);

//    mainLayout->addWidget(splitter);


    QScrollArea *scrollView = new QScrollArea;
    scrollView->setWidget(w1);
    scrollView->setWidgetResizable(true);

    scrollView->setStyleSheet("background-color: white;");

//    mainLayout->addLayout(tBox);
    mainLayout->addWidget(scrollView);

    mainLayout->addLayout(row2Box);

    setLayout(mainLayout);

    setWindowTitle(tr("Местные сопротивления"));

    parse(mestn);

    initColors();

    QSettings settings;

    QString fontFamily = settings.value("property/fontFamily", "Arial").toString();
    int fontSize = settings.value("property/fontSize", 12).toInt();

    QString st1 = QString("QScrollArea, QSplitter, QLabel, CustomDateEdit, QSpinBox, QDoubleSpinBox {background-color: white;}");
    QString st2 = QString("\nQLabel, CustomDateEdit, QSpinBox, QDoubleSpinBox, QLineEdit, QComboBox {font-size: %1px; font-family: %2})").arg(fontSize).arg(fontFamily);

    this->setStyleSheet(st1+st2);

    connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

void showHelp(QWidget *widget, const QString &title, const QString &text)
{
//    QString text = readFileUTF8(fn);

    if (text != "") {
//        QString title = findTableRusName("gid", m_table);

        HelpDialog hlp(title, text, widget);
        hlp.exec();
//        QMessageBox::warning(this, title, text);
    }
}


void MestSopr3::helpClicked()
{
//  QMessageBox::warning(this, tr("!"), "!");

    QString fn = "kls/help/MestSopr.html";

    QString text = readFileUTF8(fn);

    if (text != "") {
        QString title = "Местные сопротивления";

        showHelp(this, title, text);

//        HelpDialog hlp(title, text, this);
//        hlp.exec();
//        QMessageBox::warning(this, title, text);
    }
}


void MestSopr3::parse(const QString & txt)
{
    QStringList lst = txt.split(" ");
    
    for (auto & str: lst) {
        QStringList kv = str.split("|");
        if (kv.size() == 3) {
            int id = kv[0].toInt();
            int k = kv[1].toInt();
            double v = kv[2].toDouble();

            {
                auto it = map_v.find(id);
                if (it != map_v.end()) {
                    QDoubleSpinBox *w = it->second;
                    w->setValue(v);
                }
            }
            {
                auto it = map_k.find(id);
                if (it != map_k.end()) {
                    QSpinBox *w = it->second;
                    w->setValue(k);
                }
            }
        }
    }
    initColors();
}


double MestSopr3::calcSum()
{
    double sum = 0;

    QString s = "";

    for (auto & [id, w]: map_k) {
        int k = w->value();
        if (k > 0) {
            auto it = map_v.find(id);
            if (it != map_v.end()) {
                QDoubleSpinBox *ww = it->second;
                double v = ww->value();

                sum += v*k;

                if (s != "") s += " ";
                s += QString("%1|%2|%3").arg(id).arg(k).arg(v);
            }
        }
    }

    m_sum_list = s;

    return sum;
}


void MestSopr3::initColors()
{
    for (auto & [id, w]: map_k) {
        QLabel * lb = nullptr;
        QDoubleSpinBox *ww = nullptr;

        int k = w->value();

        auto it1 = map_l.find(id);
        if (it1 != map_l.end()) {
            lb = it1->second;
        }

        auto it2 = map_v.find(id);
        if (it2 != map_v.end()) {
            ww = it2->second;
        }

        if (ww && lb) {
            if (k > 0) {
                lb->setEnabled(true);
                ww->setEnabled(true);
            }
            else {
                lb->setEnabled(false);
                ww->setEnabled(false);
            }
        }
    }
}

void MestSopr3::vChanged(const QString &)
{
    m_sum = calcSum();
    sopr->setText(QString("%1").arg(m_sum));
    initColors();
}


void MestSopr3::okClicked()
{
    m_sum = calcSum();

    done(QDialog::Accepted);
}
