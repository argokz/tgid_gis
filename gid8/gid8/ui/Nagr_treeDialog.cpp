#include <QtGui>
#include <QtWidgets>

#include "Nagr_treeDialog.h"
#include <edit/menu.h>

CNagr_treeDialog::CNagr_treeDialog(QWidget *parent)
  : QDialog(parent) , save(this, "CNagr_treeDialog")
{
    ui.setupUi(this);

/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    connect(ui.edit_Name, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Otopl, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Gvs, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Vent, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_All, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));



    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));


//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));
}


void CNagr_treeDialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CNagr_treeDialog::write()
{
    ui.edit_Name->setText(m_strName);
    ui.edit_Otopl->setText(m_strOtopl);
    ui.edit_Gvs->setText(m_strGvs);
    ui.edit_Vent->setText(m_strVent);
    ui.edit_All->setText(m_strAll);

}

void CNagr_treeDialog::read()
{
    m_strName = ui.edit_Name->text();
    m_strOtopl = ui.edit_Otopl->text();
    m_strGvs = ui.edit_Gvs->text();
    m_strVent = ui.edit_Vent->text();
    m_strAll = ui.edit_All->text();

}

#if 0
#endif


void CNagr_treeDialog::changed()
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

void CNagr_treeDialog::stateChanged(int)
{
    changed();
}

void CNagr_treeDialog::buttonClicked(QAbstractButton*)
{
    changed();
}


void  CNagr_treeDialog::textChanged(QString txt)
{
    changed();
}

void  CNagr_treeDialog::currentTextChanged(QString txt)
{
    changed();
}

void CNagr_treeDialog::okClicked()
{
    read();

    done(QDialog::Accepted);
}

