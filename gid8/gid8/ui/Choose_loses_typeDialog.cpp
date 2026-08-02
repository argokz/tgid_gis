#include <QtGui>
#include <QtWidgets>

#include "Choose_loses_typeDialog.h"
#include <edit/menu.h>

CChoose_loses_typeDialog::CChoose_loses_typeDialog(QWidget *parent)
  : QDialog(parent) , save(this, "CChoose_loses_typeDialog")
{
    ui.setupUi(this);

/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    connect(ui.check_Fragment, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui.combo_Combo1, SIGNAL(currentTextChanged(QString)), this, SLOT(currentTextChanged(QString)));
    connect(ui.bg_Radio1, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));



    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
//    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui.push_Add_heating_date, SIGNAL(clicked()), this, SLOT(onAdd_heating_date()));
    connect(ui.push_Del, SIGNAL(clicked()), this, SLOT(onDel()));
    setRadioId(ui.bg_Radio1);

//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));
}


void CChoose_loses_typeDialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CChoose_loses_typeDialog::write()
{
    ui.check_Fragment->setCheckState(m_bFragment ? Qt::Checked : Qt::Unchecked);
    ui.combo_Combo1->setCurrentIndex(m_cmbCombo1);
    {
        QAbstractButton *but = ui.bg_Radio1->button(m_radioRadio1);
        if (but) but->setChecked(true);
    }
    ui.date_Datetimepicker1->setDate(m_dtDatetimepicker1);
    ui.date_Datetimepicker2->setDate(m_dtDatetimepicker2);

}

void CChoose_loses_typeDialog::read()
{
    m_bFragment = ui.check_Fragment->checkState() == Qt::Checked;
    m_cmbCombo1 = ui.combo_Combo1->currentIndex();
    m_radioRadio1 = ui.bg_Radio1->checkedId();
    m_dtDatetimepicker1 = ui.date_Datetimepicker1->date();
    m_dtDatetimepicker2 = ui.date_Datetimepicker2->date();

}

#if 0

void CChoose_loses_typeDialog::onAdd_heating_date() {
    read();
    write();
}

void CChoose_loses_typeDialog::onDel() {
    read();
    write();
}
#endif


void CChoose_loses_typeDialog::changed()
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

void CChoose_loses_typeDialog::stateChanged(int)
{
    changed();
}

void CChoose_loses_typeDialog::buttonClicked(QAbstractButton*)
{
    changed();
}


void  CChoose_loses_typeDialog::textChanged(QString txt)
{
    changed();
}

void  CChoose_loses_typeDialog::currentTextChanged(QString txt)
{
    changed();
}

void CChoose_loses_typeDialog::okClicked()
{
    read();

    done(QDialog::Accepted);
}

