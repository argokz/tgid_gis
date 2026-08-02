#include <QtGui>
#include <QtWidgets>

#include "Print_frDialog.h"
#include <edit/menu.h>

CPrint_frDialog::CPrint_frDialog(QWidget *parent)
  : QDialog(parent) , save(this, "CPrint_frDialog")
{
    ui.setupUi(this);

/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    connect(ui.combo_Paper, SIGNAL(currentTextChanged(QString)), this, SLOT(currentTextChanged(QString)));
    connect(ui.combo_Mas, SIGNAL(currentTextChanged(QString)), this, SLOT(currentTextChanged(QString)));
    connect(ui.bg_Portret, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));



    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));

    setRadioId(ui.bg_Portret);

//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));
}


void CPrint_frDialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CPrint_frDialog::write()
{
    ui.combo_Paper->setCurrentIndex(m_cmbPaper);
    ui.combo_Mas->setCurrentIndex(m_cmbMas);
    {
        QAbstractButton *but = ui.bg_Portret->button(m_radioPortret);
        if (but) but->setChecked(true);
    }

}

void CPrint_frDialog::read()
{
    m_cmbPaper = ui.combo_Paper->currentIndex();
    m_cmbMas = ui.combo_Mas->currentIndex();
    m_radioPortret = ui.bg_Portret->checkedId();

}

#if 0
#endif


void CPrint_frDialog::changed()
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

void CPrint_frDialog::stateChanged(int)
{
    changed();
}

void CPrint_frDialog::buttonClicked(QAbstractButton*)
{
    changed();
}


void  CPrint_frDialog::textChanged(QString txt)
{
    changed();
}

void  CPrint_frDialog::currentTextChanged(QString txt)
{
    changed();
}

void CPrint_frDialog::okClicked()
{
    read();

    done(QDialog::Accepted);
}

