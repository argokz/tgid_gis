#include <QtGui>
#include <QtWidgets>

#include "UtfDialog.h"
#include <edit/menu.h>

CUtfDialog::CUtfDialog(QWidget *parent)
  : QDialog(parent) , save(this, "CUtfDialog")
{
    ui.setupUi(this);

/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    connect(ui.edit_T1, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_T2, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_T3, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_G, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_G0, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Uf, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.bg_M1, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));



    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));

    setRadioId(ui.bg_M1);

//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));
}


void CUtfDialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CUtfDialog::write()
{
    ui.edit_T1->setText(m_strT1);
    ui.edit_T2->setText(m_strT2);
    ui.edit_T3->setText(m_strT3);
    ui.edit_G->setText(m_strG);
    ui.edit_G0->setText(m_strG0);
    ui.edit_Uf->setText(m_strUf);
    {
        QAbstractButton *but = ui.bg_M1->button(m_radioM1);
        if (but) but->setChecked(true);
    }

}

void CUtfDialog::read()
{
    m_strT1 = ui.edit_T1->text();
    m_strT2 = ui.edit_T2->text();
    m_strT3 = ui.edit_T3->text();
    m_strG = ui.edit_G->text();
    m_strG0 = ui.edit_G0->text();
    m_strUf = ui.edit_Uf->text();
    m_radioM1 = ui.bg_M1->checkedId();

}



void CUtfDialog::changed()
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

void CUtfDialog::stateChanged(int)
{
    changed();
}

void CUtfDialog::buttonClicked(QAbstractButton*)
{
    changed();
}


void  CUtfDialog::textChanged(QString txt)
{
    changed();
}


void CUtfDialog::okClicked()
{
    read();

    done(QDialog::Accepted);
}

