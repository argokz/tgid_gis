#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "OpenServerDialog.h"

#include <mystd.h>

const int D_WIDTH300 = 300;
const int D_WIDTH100 = 130;

QString getDB(QWidget *wnd, ConnectStr cs, bool is_gid);
void init_cs(ConnectStr &cs);

void OpenServerDialog::onBdGid()
{
    read_cs();
    QString s = getDB(this, cs, true);
    if (s != "") {
        QLineEdit *le = (QLineEdit *) getWidget(5);

        if (le) {
            le->setText(s);
        }
    }
}

void OpenServerDialog::onBdGeo()
{
    read_cs();
    QString s = getDB(this, cs, false);

    if (s != "") {
        QLineEdit *le = (QLineEdit *) getWidget(6);

        if (le) {
            le->setText(s);
        }
    }
}


void OpenServerDialog::onBdGid9()
{
    read_cs();
    QString s = getDB(this, cs, true);
    if (s != "") {
        QLineEdit *le = (QLineEdit *) getWidget(7);

        if (le) {
            le->setText(s);
        }
    }
}



//#define INIT_CONFIG 1


#include <config.h>


OpenServerDialog::OpenServerDialog(QWidget *parent, const QString & title)
    : MyDialog(parent, title)
    , save(this, "OpenServerDialog", title)
{

    QPushButton *okButton = new QPushButton(tr("&Ok"));
    okButton->setDefault(true);
    okButton->setEnabled(true);

    QPushButton *closeButton = new QPushButton(tr("Отменить"));

    QPushButton *newButton = new QPushButton(tr("Новая база"));
    QPushButton *checkButton = new QPushButton(tr("Проверка связи"));

//    QPushButton *helpButton = new QPushButton(tr("Помощь"));

    auto buttonBox1 = new QHBoxLayout;
    auto buttonBox2 = new QHBoxLayout;

    QVBoxLayout *l1Layout = new QVBoxLayout;

    init_cs(cs);

    InitConfig *ini =  get_ini();

    QComboBox *cb = addComboBox(8, D_WIDTH300);
    cb->addItem("MS SQL", 1);
    cb->addItem("PostgreSQL", 2);
    cb->setCurrentIndex(cs.rdbms);

    addRow(l1Layout, 8, "СУБД", D_WIDTH100, (QWidget *)cb);

    addRowLineEdit(l1Layout, 1, "Имя хоста/IP", cs.host, D_WIDTH100, D_WIDTH300, ini->enable_host);
    addRowLineEdit(l1Layout, 2, "Пользователь", cs.user, D_WIDTH100, D_WIDTH300, ini->enable_user);
    QLineEdit *l = addRowLineEdit(l1Layout, 3, "Пароль", cs.password, D_WIDTH100, 200, ini->enable_password);
    if (l) l->setEchoMode(QLineEdit::Password);

    QVariant p = QVariant::fromValue((uint16_t)cs.port);

    addRowLineEdit(l1Layout, 4, "Порт", p, D_WIDTH100, 100, ini->enable_port);

    QVBoxLayout *l2Layout = new QVBoxLayout;

//    QIcon actionIcon;
//    QSize sz(16, 16);
//    actionIcon.addPixmap(this->style()->standardIcon(QStyle::SP_TitleBarUnshadeButton).pixmap(sz));

    l = addRowLineEdit(l2Layout, 5, "Схема сети", cs.baza, D_WIDTH100, D_WIDTH300, ini->enable_baza);
    {
        QAction *myAction = l->addAction(*actionIcon, QLineEdit::TrailingPosition);
        this->connect(myAction, SIGNAL(triggered()), this, SLOT(onBdGid()));
    }


    l = addRowLineEdit(l2Layout, 6, "Карта города", cs.geo, D_WIDTH100, D_WIDTH300, ini->enable_geo);
    {
        QAction *myAction = l->addAction(*actionIcon, QLineEdit::TrailingPosition);
        this->connect(myAction, SIGNAL(triggered()), this, SLOT(onBdGeo()));
    }

#if USE_GID9
    if (ini->enable_gid9) {
        l = addRowLineEdit(l2Layout, 7, "База для синхронизации", cs.gid9, D_WIDTH100, D_WIDTH300, ini->enable_gid9);
        {
            QAction *myAction = l->addAction(*actionIcon, QLineEdit::TrailingPosition);
            this->connect(myAction, SIGNAL(triggered()), this, SLOT(onBdGid9()));
        }
    }
#endif


    cb->setEnabled(ini->enable_rdbms);


    l2Layout->addStretch();

    buttonBox1->addWidget(checkButton);
    buttonBox1->addStretch();

    if (ini->enable_newbaza) {
        buttonBox2->addWidget(newButton);
    }
    buttonBox2->addStretch();
    buttonBox2->addWidget(okButton);
    buttonBox2->addWidget(closeButton);


    auto mainLayout = new QVBoxLayout;

    mainLayout->addLayout(l1Layout);
    mainLayout->addLayout(buttonBox1);
    mainLayout->addLayout(l2Layout);
    mainLayout->addLayout(buttonBox2);
    setLayout(mainLayout);

    setWindowTitle(title);



    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
//    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
//    connect(closeButton, &QPushButton::clicked, this, &QDialog::reject);


    connect(checkButton, SIGNAL(clicked()), this, SLOT(checkClicked()));
    connect(newButton, SIGNAL(clicked()), this, SLOT(newClicked()));
}

void OpenServerDialog::helpClicked()
{
}

void setRDBMS(int rdbms);


void OpenServerDialog::read_cs()
{
    cs.rdbms =     getCurentCombo(8);

    setRDBMS(cs.rdbms);

    cs.host =      getValueStr(1); 
    cs.user =      getValueStr(2); 
    cs.password =  getValueStr(3); 
    cs.port =      getValueInt(4); 
    cs.baza =      getValueStr(5);
    cs.geo =       getValueStr(6);
#if USE_GID9
    cs.gid9 =      getValueStr(7);
#endif
}


void OpenServerDialog::okClicked()
{
    read_cs();

    void saveConnectStr(ConnectStr &cs);
    saveConnectStr(cs);

    done(QDialog::Accepted);
}

#include <db/db.h>


void OpenServerDialog::checkClicked()
{
    read_cs();

    QSqlDatabase m_db;

    QString name = "";
    if (cs.rdbms == 1) { // POSTGRESQL
        name = "template1";
    }

    if (cs.user == "") {
        QMessageBox::about(this, "", "Не задано имя пользователя");
        return;
    }
    if (cs.password == "") {
        QMessageBox::about(this, "", "Не задан пароль");
        return;
    }


    bool ok = connectSQL0(cs.rdbms, cs.host, cs.port, name, cs.user, cs.password, m_db);
    
    if (ok) {
        QMessageBox::about(this, "", "OK!");
    }
    else {
        QMessageBox::warning(this, "", m_db.lastError().text());
    }
}

void newBaza0(QWidget *widget, ConnectStr &cs);

void OpenServerDialog::newClicked()
{
    read_cs();
    newBaza0(this, cs);
}
