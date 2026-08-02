
#include <QtGui>
#include <QtWidgets>

#include "Ch_password_right_newDialog.h"
#include <edit/menu.h>

CCh_password_right_newDialog::CCh_password_right_newDialog(QWidget *parent)
  : QDialog(parent) , save(this, "CCh_password_right_newDialog")
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


void CCh_password_right_newDialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CCh_password_right_newDialog::write()
{
    ui.edit_Password->setEchoMode(QLineEdit::Password);
    ui.edit_Password2->setEchoMode(QLineEdit::Password);
    ui.edit_User->setText(m_strUser);
    ui.edit_Password->setText(m_strPassword);
    ui.edit_Password2->setText(m_strPassword2);
    ui.lstb_Filtr->setCurrentRow(m_lstbFiltr);

}


void CCh_password_right_newDialog::read()
{
    m_strUser = ui.edit_User->text();
    m_strPassword = ui.edit_Password->text();
    m_strPassword2 = ui.edit_Password2->text();
    m_lstbFiltr = ui.lstb_Filtr->currentRow();

}



void CCh_password_right_newDialog::okClicked()
{
    read();

//    QMessageBox::warning(this, "", user);
//    QSettings settings;
//    settings.setValue("user", user);

    done(QDialog::Accepted);
}

