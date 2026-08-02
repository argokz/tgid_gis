
#include <QtGui>
#include <QtWidgets>

#include "Ch_password2Dialog.h"
#include <edit/menu.h>

CCh_password2Dialog::CCh_password2Dialog(QWidget *parent)
  : QDialog(parent) , save(this, "CCh_password2Dialog")
{
    ui.setupUi(this);

/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));


//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));


}


void CCh_password2Dialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CCh_password2Dialog::write()
{
    ui.edit_Oldpassword->setEchoMode(QLineEdit::Password);
    ui.edit_Password->setEchoMode(QLineEdit::Password);
    ui.edit_Password2->setEchoMode(QLineEdit::Password);
    ui.edit_Oldpassword->setText(m_strOldpassword);
    ui.edit_Password->setText(m_strPassword);
    ui.edit_Password2->setText(m_strPassword2);

}


void CCh_password2Dialog::read()
{
    m_strOldpassword = ui.edit_Oldpassword->text();
    m_strPassword = ui.edit_Password->text();
    m_strPassword2 = ui.edit_Password2->text();

}



void CCh_password2Dialog::okClicked()
{
    read();

//    QMessageBox::warning(this, "", user);
//    QSettings settings;
//    settings.setValue("user", user);

    done(QDialog::Accepted);
}

