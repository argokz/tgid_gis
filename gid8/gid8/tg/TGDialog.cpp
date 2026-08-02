#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include <db/db.h>

#include <table/DbWindow.h>
#include <dialog/SaveGeometry.h>

#include "cxema/cxema1.h"
#include "std.h"

#include <tg/TGDialog.h>
#include <tg/TGraph.h>

#include "cxema/undo.h"


#include <mystd.h>

TGDialog::~TGDialog()
{
    QList<int> list_size = splitter->sizes();

    QList<QVariant> lv;

    for (auto &it : list_size) {
        lv.append(QVariant(it));
    }
    QSettings settings;
    settings.setValue("TGDialog/splitter", lv);
}


TGDialog::TGDialog(QWidget *parent, CCxema *cxema, QSqlDatabase &db, int id, int id2)
    : QDialog(parent)
    , save(this, "TGDialog")
{
    m_cxema = cxema;
    m_db = db;
    m_id = id;
    m_id2 = id2;

//    QHBoxLayout *rowLayout;

    QHBoxLayout *mainLayout = new QHBoxLayout;

    QVBoxLayout *lBox = new QVBoxLayout;
    QVBoxLayout *rBox = new QVBoxLayout;

    QHBoxLayout *buttonBox = new QHBoxLayout;

    w = new TGraph(this);
    w->init0(db, id2);

    int xy = 200;

    w->setMinimumWidth(xy);
    w->setMinimumHeight(xy);

    QPushButton *okButton = new QPushButton(tr("&Ok"));
    okButton->setDefault(true);
    okButton->setEnabled(true);

    QPushButton *b1Button = new QPushButton(tr("Данные"));
    QPushButton *b2Button = new QPushButton(tr("Стационарный"));
    QPushButton *b3Button = new QPushButton(tr("Расчет"));
    QPushButton *b4Button = new QPushButton(tr("В буфер ГРФ"));
    QPushButton *b5Button = new QPushButton(tr("В буфер ТБЛ"));

    buttonBox->addWidget(okButton);
    buttonBox->addWidget(b1Button);
    buttonBox->addWidget(b2Button);
    buttonBox->addWidget(b3Button);
    buttonBox->addWidget(b4Button);
    buttonBox->addWidget(b5Button);
    buttonBox->addStretch();

    lBox->addLayout(buttonBox);
    lBox->addWidget(w);

    QWidget *w1 = new QWidget;
    QWidget *w2 = new QWidget;

    w1->setLayout(lBox);


    QLabel *label = new QLabel("            ");

    QCheckBox *cb1 = new QCheckBox(tr("Подающая"));
    QCheckBox *cb2 = new QCheckBox(tr("Обратная"));
    QCheckBox *cb3 = new QCheckBox(tr("Смешанная"));
    QCheckBox *cb4 = new QCheckBox(tr("Ветер"));

    QHBoxLayout *buttonBox2 = new QHBoxLayout;

    buttonBox2->addWidget(label);

    buttonBox2->addWidget(cb1);
    buttonBox2->addWidget(cb2);
    buttonBox2->addWidget(cb3);
    buttonBox2->addWidget(cb4);
    buttonBox2->addStretch();
    rBox->addLayout(buttonBox2);

    QVBoxLayout *tbBox = new QVBoxLayout;

  QString q = QString(R"SQL(
SELECT TOP 10000 d.id, d.tn, d.Q_otn, d.t1, d.t2, d.t3, d.tv, d.t_bn
FROM deployedTempGraphs d
JOIN heatSources hs ON d.hSourceID=hs.id
WHERE hs.id=%1
ORDER BY d.hSourceID, 
d.tn DESC
    )SQL").arg(id2);

    if (!is_MSSQL()) {
      q = QString(R"SQL(
SELECT * FROM (

SELECT d.id, d.tn, 
round(d.Q_otn::NUMERIC, 3) as Q_otn, 
round(d.t1::NUMERIC, 1) as t1, 
round(d.t2::NUMERIC, 1) as t2, 
round(d.t3::NUMERIC, 1) as t3, 
round(d.tv::NUMERIC, 1) as tv,
round(d.t_bn::NUMERIC, 1) as t_bn
FROM deployedTempGraphs d
JOIN heatSources hs ON d.hSourceID=hs.id
WHERE hs.id=%1
ORDER BY d.hSourceID,
d.tn::NUMERIC

DESC
)
    )SQL").arg(id2);
    }

    qDebug() << q;

    m_q = q;

    tb = getTableView(m_db, "deployedTempGraphs", q, "Температурный график");

    std::set h = {0};
    tb->set_hide(h);
    tb->setEdit(true);

    tbBox->addWidget(tb);

    rBox->addLayout(buttonBox2);
    rBox->addLayout(tbBox);

    w2->setLayout(rBox);

    splitter = new QSplitter;
    splitter->addWidget(w1);
    splitter->addWidget(w2);

    QSettings settings;
    QVariant v = settings.value("TGDialog/splitter");

    if (v != QVariant()) {
        QList<QVariant> lv = v.toList();
        QList<int> list_size;

        for (auto &it : lv) {
            list_size.append(it.toInt());
        }

        splitter->setSizes(list_size);
    }

    mainLayout->addWidget(splitter);

    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));

    connect(b1Button, SIGNAL(clicked()), this, SLOT(b1Clicked()));
    connect(b2Button, SIGNAL(clicked()), this, SLOT(b2Clicked()));
    connect(b3Button, SIGNAL(clicked()), this, SLOT(b3Clicked()));
    connect(b4Button, SIGNAL(clicked()), this, SLOT(b4Clicked()));
    connect(b5Button, SIGNAL(clicked()), this, SLOT(b5Clicked()));

    QSettings setting;

    cb1->setCheckState((Qt::CheckState)setting.value("tg/cb1", Qt::Checked).toInt());
    cb2->setCheckState((Qt::CheckState)setting.value("tg/cb2", Qt::Checked).toInt());
    cb3->setCheckState((Qt::CheckState)setting.value("tg/cb3", Qt::Checked).toInt());
    cb4->setCheckState((Qt::CheckState)setting.value("tg/cb4", Qt::Checked).toInt());

    connect(cb1, SIGNAL(stateChanged(int)), this, SLOT(cb1_stateChanged(int)));
    connect(cb2, SIGNAL(stateChanged(int)), this, SLOT(cb2_stateChanged(int)));
    connect(cb3, SIGNAL(stateChanged(int)), this, SLOT(cb3_stateChanged(int)));
    connect(cb4, SIGNAL(stateChanged(int)), this, SLOT(cb4_stateChanged(int)));


    // в MainWindow или другом родителе
    connect(tb, &DbWindow::dataChanged,
        this, &TGDialog::redraw_tg);


    setWindowTitle(tr("Температурный график"));
}

void TGDialog::helpClicked()
{
}

void TGDialog::okClicked()
{
  done(QDialog::Accepted);
}

#include <property/PropertyDial.h>
QString findTableRusName(const QString & d, const QString & n);

#include <gid_data/heatSystem.h>


void TGDialog::b1Clicked()  // Данные
{
//    QString table = n->getTableMySQL();
    QString table = "heatSources";
    QString title = findTableRusName("gid", table);

    C_heatSystem d;

    if (getCT(m_db, d)) {
        QString q = QString("UPDATE heatSources SET tn_5=%1, tn_1=%2").arg(d.t_or).arg(d.t_vnew);
        query_exec(m_db, q);
    }

    auto *dlg = new PropertyDial(this, table, "tg", "shape", "id", m_id, -1);
    dlg->init_gid(m_db, title, table, m_id, -1, pr_type_node, m_cxema, true);

    connect(dlg, SIGNAL(accepted()), parentWidget(), SLOT(onPropertyAccepted()));

    dlg->show();
}

#include "tempgraph.h"
#include "map"

template <typename Map, typename Key, typename Value>
Value map_get(const Map& m, const Key& key, const Value& def) {
    auto it = m.find(key);
    return (it != m.end()) ? it->second : def;
}


void TGDialog::redraw_tg()
{
    this->w->init0(m_db, m_id2);
    this->tb->reset_q();
}



void TGDialog::onStatAccepted()
{

    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {

/*

        double tg1 = map_get(dlg->m_params, QString("TG1"), QVariant("60")).toDouble();
        double tg2 = map_get(dlg->m_params, QString("TG2"), QVariant("60")).toDouble();
        double tg3 = map_get(dlg->m_params, QString("TG3"), QVariant("60")).toDouble();
        double tgv = map_get(dlg->m_params, QString("TGV"), QVariant("60")).toDouble();
*/
        double tg1 = dlg->getValue("TG1").toDouble();
        double tg2 = dlg->getValue("TG2").toDouble();
        double tg3 = dlg->getValue("TG3").toDouble();
        double tgv = dlg->getValue("TGV").toDouble();



        if (QMessageBox::question(0, "", "Пересчитать температурный график?") == QMessageBox::Yes) {
            int id = m_id2;
            QString q = QString("UPDATE deployedTempGraphs SET t1=%1, t2=%2, t3=%3, tv=%4 WHERE hSourceID=%5")
                            .arg(tg1).arg(tg2).arg(tg3).arg(tgv).arg(m_id2);

//            change_group_start(m_db, "Пересчет ТГ");

            query_exec(m_db, q);

//            change_group_end(m_db);


            this->w->init0(m_db, m_id2);
            this->tb->reset_q();
        }
    }
}

void TGDialog::b2Clicked()  // Стационарный
{
    auto *dlg = new PropertyDial(this, "tg_stat", "", "shape", "id", -1, -1);
    QSqlDatabase db;

    std::map<QString, QVariant> map_val;

    map_val["TG1"] = "60";
    map_val["TG2"] = "60";
    map_val["TG3"] = "60";
    map_val["TGV"] = "60";

    dlg->init_dialog(&db, "Стационарный температурный график", "tg_stat", map_val);
    dlg->show();

    connect(dlg, SIGNAL(accepted()), this, SLOT(onStatAccepted()));
}

void TGDialog::b3Clicked()  // Расчет
{
    QSqlQuery query(m_db);

    CTempGraph *tg = new CTempGraph(m_db, query, m_id2, true);
    this->w->init0(m_db, m_id2);
    this->tb->reset_q();
}

void TGDialog::b4Clicked()  // В буфер ГРФ
{
    w->copyToClipboard();
}


void TGDialog::b5Clicked()  // В буфер ТБЛ
{
    std::set h = {0};

    copy_q(m_db, m_q);
}

void TGDialog::cb1_stateChanged(int state)
{
    w->m_ch1 = (state == 2);
    w->repaint();
}

void TGDialog::cb2_stateChanged(int state)
{
    w->m_ch2 = (state == 2);
    w->repaint();
}

void TGDialog::cb3_stateChanged(int state)
{
    w->m_ch3 = (state == 2);
    w->repaint();
}

void TGDialog::cb4_stateChanged(int state)
{
    w->m_ch4 = (state == 2);
    w->repaint();
}

