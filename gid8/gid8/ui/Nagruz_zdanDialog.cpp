#include <QtGui>
#include <QtWidgets>

#include "Nagruz_zdanDialog.h"
#include <edit/menu.h>

CNagruz_zdanDialog::CNagruz_zdanDialog(QWidget *parent)
  : QDialog(parent) , save(this, "CNagruz_zdanDialog")
{
    ui.setupUi(this);

/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    connect(ui.edit_Edit2, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Edit3, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Edit5, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Edit6, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Edit7, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Edit4, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Edit8, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.combo_Combo1, SIGNAL(currentTextChanged(const QString &)), this, SLOT(currentTextChanged(const QString &)));



    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui.push__help, SIGNAL(clicked()), this, SLOT(on_help()));

//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));
}


void CNagruz_zdanDialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CNagruz_zdanDialog::write()
{
    ui.edit_Edit7->setReadOnly(true);
    ui.edit_Edit4->setReadOnly(true);
    ui.edit_Edit8->setReadOnly(true);
    ui.edit_Edit2->setText(m_strEdit2);
    ui.edit_Edit3->setText(m_strEdit3);
    ui.edit_Edit5->setText(m_strEdit5);
    ui.edit_Edit6->setText(m_strEdit6);
    ui.edit_Edit7->setText(m_strEdit7);
    ui.edit_Edit4->setText(m_strEdit4);
    ui.edit_Edit8->setText(m_strEdit8);
    ui.combo_Combo1->setCurrentIndex(m_cmbCombo1);

}

void CNagruz_zdanDialog::read()
{
    m_strEdit2 = ui.edit_Edit2->text();
    m_strEdit3 = ui.edit_Edit3->text();
    m_strEdit5 = ui.edit_Edit5->text();
    m_strEdit6 = ui.edit_Edit6->text();
    m_strEdit7 = ui.edit_Edit7->text();
    m_strEdit4 = ui.edit_Edit4->text();
    m_strEdit8 = ui.edit_Edit8->text();
    m_cmbCombo1 = ui.combo_Combo1->currentIndex();

}

#if 1

void CNagruz_zdanDialog::on_help() {
    read();
    write();
}
#endif


void CNagruz_zdanDialog::changed()
{
    if (this->isVisible())  {
        if (prop_callback) {
            read();
            if ((*prop_callback)(this)) {
                write();
            }
        }
    }
}

void CNagruz_zdanDialog::stateChanged(int)
{
    changed();
}

void CNagruz_zdanDialog::buttonClicked(QAbstractButton*)
{
    changed();
}


void  CNagruz_zdanDialog::textChanged(QString txt)
{
    changed();
}

void  CNagruz_zdanDialog::currentTextChanged(QString txt)
{
    changed();
}

void CNagruz_zdanDialog::okClicked()
{
    read();

    done(QDialog::Accepted);
}

