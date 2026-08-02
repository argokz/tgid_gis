#include <QApplication>
#include <QtGui>
#include <QtWidgets>

#include "Param2Dialog.h"
#include <edit/menu.h>

#include "ParamDialog.h"


void CParam2Dialog::setup()
{
    ParamDialog dialog;
    dialog.setParam2Index(m_sopr);
    dialog.setRoP(m_roP);
    dialog.setRoO(m_roO);
    dialog.setCheck1(m_ro_temp);

    if (dialog.exec() == QDialog::Accepted) {
//        QString param1 = dialog.getParam1();
        m_sopr = dialog.getParam2Index();
        m_roP = dialog.getRoP();
        m_roO = dialog.getRoO();
        m_ro_temp = dialog.getCheck1();
//        qDebug() << "Введенные параметры:" << param1 << "Номер выбранного параметра:" << param2Index;
    }
}


CParam2Dialog::CParam2Dialog(QWidget *parent)
  : QDialog(parent) 
//  , save(this, "CParam2Dialog")
{
    ui.setupUi(this);

/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    ui.edit_kol_iter->setValidator(new QIntValidator(1, 1000, this));

    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui.push_T_ist, SIGNAL(clicked()), this, SLOT(onT_ist()));

    connect(ui.push_Setup, SIGNAL(clicked()), this, SLOT(setup()));

    setRadioId(ui.bg_Detaliz);

//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));

    QObject::connect(ui.check_Leto, &QCheckBox::stateChanged, [&](int state) {
//        if (state == 2) {
//        }
        ui.radio_Equiv->setEnabled(state == 2);   // Эквиваленитное

        if (state == 2) {   // Летнее
            ui.radio_Detaliz->setChecked(true);
            ui.radio_Equiv->setChecked(false);
            ui.check_save_leto->setEnabled(true);
        }
        else {
            ui.radio_Detaliz->setEnabled(true); 
            ui.radio_Equiv->setEnabled(true); 

            ui.check_save_leto->setEnabled(false);
            ui.check_save_leto->setChecked(false);
        }

    });

    QObject::connect(ui.radio_Detaliz, &QRadioButton::toggled, [&](bool checked) {
//        ui.check_Leto->setEnabled(!checked);
        if (!checked) {
            ui.check_Leto->setChecked(false);
        }

    });
}


void CParam2Dialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CParam2Dialog::write()
{
    ui.edit_kol_iter->setText(m_str_kol_iter);

    ui.edit_Tn->setText(m_strTn);
    ui.edit_Name->setText(m_strName);
    ui.check_Avtomat->setCheckState(m_bAvtomat ? Qt::Checked : Qt::Unchecked);
    ui.check_Char_sety->setCheckState(m_bChar_sety ? Qt::Checked : Qt::Unchecked);
    ui.check_Veter->setCheckState(m_bVeter ? Qt::Checked : Qt::Unchecked);
    ui.check_Save_po->setCheckState(m_bSave_po ? Qt::Checked : Qt::Unchecked);
    ui.check_Leto->setCheckState(m_bLeto ? Qt::Checked : Qt::Unchecked);
    ui.check_Mag_fragment->setCheckState(m_bMag_fragment ? Qt::Checked : Qt::Unchecked);
    ui.check_Kv->setCheckState(m_bKv ? Qt::Checked : Qt::Unchecked);
    ui.check_copy_calc->setCheckState(m_copy_calc ? Qt::Checked : Qt::Unchecked);

    
    ui.combo_Gws->setCurrentIndex(m_cmbGws);

    
    ui.check_save_leto->setCheckState(m_bSaveLeto ? Qt::Checked : Qt::Unchecked);

    
    ui.combo_Gws2->setCurrentIndex(m_cmbGws2);
    {
        QAbstractButton *but = ui.bg_Detaliz->button(m_radioDetaliz);
        if (but) but->setChecked(true);
    }

}


void CParam2Dialog::read()
{
    m_str_kol_iter = ui.edit_kol_iter->text();

    m_strTn = ui.edit_Tn->text();
    m_strName = ui.edit_Name->text();

    m_bSaveLeto = ui.check_save_leto->checkState() == Qt::Checked;
    
    m_bAvtomat = ui.check_Avtomat->checkState() == Qt::Checked;
    m_bChar_sety = ui.check_Char_sety->checkState() == Qt::Checked;
    m_bVeter = ui.check_Veter->checkState() == Qt::Checked;
    m_bSave_po = ui.check_Save_po->checkState() == Qt::Checked;
    m_bLeto = ui.check_Leto->checkState() == Qt::Checked;
    m_bMag_fragment = ui.check_Mag_fragment->checkState() == Qt::Checked;
    m_bKv = ui.check_Kv->checkState() == Qt::Checked;
    m_copy_calc = ui.check_copy_calc->checkState() == Qt::Checked;

    
    m_cmbGws = ui.combo_Gws->currentIndex();
    m_cmbGws2 = ui.combo_Gws2->currentIndex();
    m_radioDetaliz = ui.bg_Detaliz->checkedId();

}

void CParam2Dialog::onT_ist() {
    QMessageBox::warning(this, "", "T_ist");
}


void CParam2Dialog::okClicked()
{
    read();

//    QMessageBox::warning(this, "", user);
//    QSettings settings;
//    settings.setValue("user", user);

    done(QDialog::Accepted);
}

