#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include <table/DbWindow.h>
#include <dialog/SaveGeometry.h>

#include "std.h"

#include "MestSopr2.h"

bool is_POSTGRESQL();

MestSopr2::MestSopr2(QWidget *parent, QSqlDatabase &db, int id)
    : QDialog(parent)
    , save(this, "MestSopr2")
{

    QSplitter *splitter = new QSplitter;

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QVBoxLayout *tBox = new QVBoxLayout;
    QVBoxLayout *bBox = new QVBoxLayout;
    QHBoxLayout *row1Box = new QHBoxLayout;
    QHBoxLayout *row2Box = new QHBoxLayout;


    QString q = "SELECT * FROM sprav.dbo.[08_mestnye_gidravlicheskie_soprotivleniya]";
    if (is_POSTGRESQL()) {
        q = "SELECT * FROM \"08_mestnye_gidravlicheskie_soprotivleniya\"";
    }

    

    DbWindow *tb1 = getTableView(db, "08_mestnye_gidravlicheskie_soprotivleniya", q, "Местные гидравлические сопротивления");

    QString q2 = QString("SELECT id, name_mest, s_mest, k_mest, sum_mest FROM localHydroResistances2 WHERE lineID=%1").arg(id);

    if (is_POSTGRESQL()) {
        q = "SELECT * FROM \"08_mestnye_gidravlicheskie_soprotivleniya\"";
    }


    DbWindow *tb2 = getTableView(db, "localHydroResistances2", q2, "Установленные местные гидравлические сопротивления");

    QPushButton *bAddButton = new QPushButton(tr("Установить"));
    QPushButton *bDelButton = new QPushButton(tr("Удалить"));

    row1Box->addStretch();
    row1Box->addWidget(bAddButton);
    row1Box->addWidget(bDelButton);
    row1Box->addStretch();

    QLabel *label = new QLabel(tr("Сумма коэффициентов местных сопротивлений"));
    QLineEdit *sopr = new QLineEdit;
    sopr->setReadOnly(true);

    QPushButton *okButton = new QPushButton(tr("&Ok"));
    QPushButton *closeButton = new QPushButton(tr("Отменить"));
    QPushButton *helpButton = new QPushButton(tr("Помощь"));

    row2Box->addWidget(label);
    row2Box->addWidget(sopr);
    row2Box->addStretch();
    row2Box->addWidget(helpButton);
    row2Box->addWidget(okButton);
    row2Box->addWidget(closeButton);

    tBox->addWidget(tb1);
    tBox->addLayout(row1Box);
    bBox->addWidget(tb2);

    splitter->setOrientation(Qt::Vertical);

    QWidget *w1 = new QWidget;
    QWidget *w2 = new QWidget;

    w1->setLayout(tBox);
    w2->setLayout(bBox);

    splitter->addWidget(w1);
    splitter->addWidget(w2);

    mainLayout->addWidget(splitter);
    mainLayout->addLayout(row2Box);

    setLayout(mainLayout);

    connect(bAddButton, SIGNAL(clicked()), this, SLOT(onAdd()));
    connect(bDelButton, SIGNAL(clicked()), this, SLOT(onDel()));

    setWindowTitle(tr("Местные сопротивления"));
}

void MestSopr2::onAdd()
{
    QMessageBox::warning(this, "", "A");
}

void MestSopr2::onDel()
{
    QMessageBox::warning(this, "", "D");
}
