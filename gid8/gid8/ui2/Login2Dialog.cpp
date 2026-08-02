#include <QtGui>
#include <QtWidgets>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QMessageBox>

#include "Login2Dialog.h"
#include <edit/menu.h>

/*
    Поменял
*/


bool loadUsers(QSqlDatabase &db, std::map <QString, CUser>& list_user);


CLogin2Dialog::CLogin2Dialog(QSqlDatabase &db, QWidget *parent)
  : QDialog(parent) , save(this, "CLogin2Dialog")
{
    ui.setupUi(this);


//    ui.label_sirius->setPixmap(QPixmap(":/images/sirius.png"));
//    ui.label_sirius->setPixmap(QPixmap(":/images/idc_spla2.png"));

    ui.edit_PASSWORD->setEchoMode(QLineEdit::Password);

    QSettings settings;
    QString user = settings.value("user").toString();

    if (loadUsers(db, list_user)) {
        int cur_i = 0;
        int i = 0;
        for (auto & it: list_user) {
            if (it.first == user) {
                cur_i = i;
            }
            i++;
            ui.combo_USER->addItem(it.first, QVariant::fromValue(it.second));
        }
        ui.combo_USER->setCurrentIndex(cur_i);
    }


/*
    ui.edit_PASSWORD->setEchoMode(QLineEdit::Password);
    ui.edit_PORT->setValidator(new QIntValidator(1, 65535, this));

    cs.host = "45.132.85.23";
    cs.user = "Lifan";
    cs.password = "";
    cs.port = 1433;
    cs.baza = "AstanaGID";
    cs.geo = "Astana";

    cs.host = "localhost";
    cs.user = "tgid";
    cs.password = "";

    QSettings settings;

    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
    cs.port =     settings.value("cs/port"    , QVariant(cs.port)).toInt();
    cs.user =     settings.value("cs/user"    , QVariant(cs.user)).toString();
    cs.password = settings.value("cs/password", QVariant(cs.password)).toString();
    cs.baza =     settings.value("cs/baza"    , QVariant(cs.baza)).toString();
    cs.geo =      settings.value("cs/geo"     , QVariant(cs.geo)).toString();
*/

    write();

    connect(ui.push_OK, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_CANCEL, SIGNAL(clicked()), this, SLOT(close()));

//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));


}


void CLogin2Dialog::write()
{
/*
    ui.edit_IP->setText(cs.host);
    ui.edit_USER->setText(cs.user);
    ui.edit_PASSWORD->setText(cs.password);
    ui.edit_PORT->setText(QString("%1").arg(cs.port));
    ui.edit_BD_GID->setText(cs.baza);
    ui.edit_BD_GEO->setText(cs.geo);
*/
}


void CLogin2Dialog::read()
{
/*
    cs.host =     ui.edit_IP->text();
    cs.port =     ui.edit_PORT->text().toInt();
    cs.user =     ui.edit_USER->text();
    cs.password = ui.edit_PASSWORD->text();
    cs.baza =     ui.edit_BD_GID->text();
    cs.geo =      ui.edit_BD_GEO->text();
*/
}

int md5hash(const char *msg, size_t len, char *hash);


void CLogin2Dialog::okClicked()
{
    read();

    QSettings settings;

    QString user = ui.combo_USER->currentText();
    QString password = ui.edit_PASSWORD->text();

    char hash[265];
    md5hash(password.toLocal8Bit(), password.length(), hash);


    auto it = list_user.find(user);

    if (it != list_user.end()) {

        QString pp = it->second.password;
        auto p = pp.toLocal8Bit();
        const char *hash2 = p.constData();
        bool y = true;

        for (int i = 0; i < 32; i++) {
            if (hash[i] != hash2[i]) {
                y = false;
                break;
            }
        }

        if (!y && !(password == "" && pp == "")) {
            QMessageBox::warning(this, "", tr("Неверный пароль"));
        }
        else {
            settings.setValue("user", user);
            user_right = it->second.user_right;
            m_user = it->second.id;
            m_strUser = user;
            m_password = password;

        //    settings.setValue("cs/geo", cs.geo);
            done(QDialog::Accepted);
        }
    }
}

