#include <QApplication>
#include <QtGui>
#include <QtWidgets>

#include "Param1Dialog.h"
#include <edit/menu.h>

#include "ParamDialog.h"


void CParam1Dialog::setup()
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

CParam1Dialog::CParam1Dialog(QWidget *parent)
  : QDialog(parent) , save(this, "CParam1Dialog")
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

    connect(ui.push_Setup, SIGNAL(clicked()), this, SLOT(setup()));

    setRadioId(ui.bg_Ur);
    setRadioId(ui.bg_Dop_nagr);


//    connect(ui.bg_Ur, SIGNAL(toggled(bool)), this, SLOT(q()));
    QObject::connect(ui.radio_Ur, &QRadioButton::toggled, [&]() {
        bool ur = ui.radio_Ur->isChecked();
        ui.check_Teplopoter->setEnabled(!ur);
        ui.check_Uf_calc->setEnabled(!ur);
        ui.check_Uf_new->setEnabled(!ur);
        ui.check_Avtomat->setEnabled(!ur);
    });



//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));


}


void CParam1Dialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CParam1Dialog::write()
{
    ui.edit_kol_iter->setText(m_str_kol_iter);

    ui.edit_Tn->setText(m_strTn);
    ui.edit_Name->setText(m_strName);
    ui.check_Teplopoter->setCheckState(m_bTeplopoter ? Qt::Checked : Qt::Unchecked);
    ui.check_Uf_calc->setCheckState(m_bUf_calc ? Qt::Checked : Qt::Unchecked);
    ui.check_Teplovyd->setCheckState(m_bTeplovyd ? Qt::Checked : Qt::Unchecked);
    ui.check_Veter->setCheckState(m_bVeter ? Qt::Checked : Qt::Unchecked);
    ui.check_Dross->setCheckState(m_bDross ? Qt::Checked : Qt::Unchecked);
    ui.check_Avtomat->setCheckState(m_bAvtomat ? Qt::Checked : Qt::Unchecked);
    ui.check_Uf_new->setCheckState(m_bUf_new ? Qt::Checked : Qt::Unchecked);
    ui.check_Char_sety->setCheckState(m_bChar_sety ? Qt::Checked : Qt::Unchecked);
    ui.check_Zn0->setCheckState(m_bZn0 ? Qt::Checked : Qt::Unchecked);
    ui.check_Utechki->setCheckState(m_bUtechki ? Qt::Checked : Qt::Unchecked);
    ui.check_Mag_fragment->setCheckState(m_bMag_fragment ? Qt::Checked : Qt::Unchecked);
    ui.check_Save_po->setCheckState(m_bSave_po ? Qt::Checked : Qt::Unchecked);
    ui.check_Kv->setCheckState(m_bKv ? Qt::Checked : Qt::Unchecked);

    ui.check_copy_calc->setCheckState(m_copy_calc ? Qt::Checked : Qt::Unchecked);

    ui.check_Plan->setCheckState(m_bPlan ? Qt::Checked : Qt::Unchecked);
    ui.combo_Metod->setCurrentIndex(m_cmbMetod);
    ui.combo_Trtp->setCurrentIndex(m_cmbTrtp);
    {
        QAbstractButton *but = ui.bg_Ur->button(m_radioUr);
        if (but) but->setChecked(true);
    }
    {
        QAbstractButton *but = ui.bg_Dop_nagr->button(m_radioDop_nagr);
        if (but) but->setChecked(true);
    }

}


void CParam1Dialog::read()
{
    m_str_kol_iter = ui.edit_kol_iter->text();

    m_strTn = ui.edit_Tn->text();
    m_strName = ui.edit_Name->text();
    m_bTeplopoter = ui.check_Teplopoter->checkState() == Qt::Checked;
    m_bUf_calc = ui.check_Uf_calc->checkState() == Qt::Checked;
    m_bTeplovyd = ui.check_Teplovyd->checkState() == Qt::Checked;
    m_bVeter = ui.check_Veter->checkState() == Qt::Checked;
    m_bDross = ui.check_Dross->checkState() == Qt::Checked;
    m_bAvtomat = ui.check_Avtomat->checkState() == Qt::Checked;
    m_bUf_new = ui.check_Uf_new->checkState() == Qt::Checked;
    m_bChar_sety = ui.check_Char_sety->checkState() == Qt::Checked;
    m_bZn0 = ui.check_Zn0->checkState() == Qt::Checked;
    m_bUtechki = ui.check_Utechki->checkState() == Qt::Checked;
    m_bMag_fragment = ui.check_Mag_fragment->checkState() == Qt::Checked;
    m_bSave_po = ui.check_Save_po->checkState() == Qt::Checked;
    m_bKv = ui.check_Kv->checkState() == Qt::Checked;

    m_copy_calc = ui.check_copy_calc->checkState() == Qt::Checked;

    m_bPlan = ui.check_Plan->checkState() == Qt::Checked;
    m_cmbMetod = ui.combo_Metod->currentIndex();
    m_cmbTrtp = ui.combo_Trtp->currentIndex();
    m_radioUr = ui.bg_Ur->checkedId();
    m_radioDop_nagr = ui.bg_Dop_nagr->checkedId();

}



void CParam1Dialog::okClicked()
{
    read();

//    QMessageBox::warning(this, "", user);
//    QSettings settings;
//    settings.setValue("user", user);

    done(QDialog::Accepted);
}

