#include <QtGui>
#include <QtWidgets>

#include "Colors2Dialog.h"
#include <edit/menu.h>

/*
    Поменял
*/


CColors2Dialog::CColors2Dialog(QWidget *parent)
  : QDialog(parent) , save(this, "CColors2Dialog")
{
    ui.setupUi(this);

/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    connect(ui.edit_Line_thickness, SIGNAL(textEdited(const QString &)), this, SLOT(textEdited(const QString &)));
    connect(ui.edit_Distance, SIGNAL(textEdited(const QString &)), this, SLOT(textEdited(const QString &)));
    connect(ui.edit_Ring, SIGNAL(textEdited(const QString &)), this, SLOT(textEdited(const QString &)));
    connect(ui.check_Punkt, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));

    
    connect(ui.combo_Open, SIGNAL(currentTextChanged(const QString &)), this, SLOT(currentTextChanged(const QString &)));
    connect(ui.combo_Typ, SIGNAL(currentTextChanged(const QString &)), this, SLOT(currentTextChanged(const QString &)));
    connect(ui.combo_Mag_ras, SIGNAL(currentTextChanged(const QString &)), this, SLOT(currentTextChanged(const QString &)));


    // Добавил

    connect(ui.spin_Line_thickness, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));


    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui.push_Pod, SIGNAL(clicked()), this, SLOT(onPod()));
    connect(ui.push_Obr, SIGNAL(clicked()), this, SLOT(onObr()));
    connect(ui.push_Default, SIGNAL(clicked()), this, SLOT(onDefault()));
    connect(ui.push_Podf, SIGNAL(clicked()), this, SLOT(onPodf()));
    connect(ui.push_Obrf, SIGNAL(clicked()), this, SLOT(onObrf()));
    connect(ui.push_Color_uzel, SIGNAL(clicked()), this, SLOT(onColor_uzel()));
    connect(ui.push_Default2, SIGNAL(clicked()), this, SLOT(onDefault2()));

//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));
}


void CColors2Dialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CColors2Dialog::write()
{
    ui.label_Colors_window->setText(m_labelColors_window);
    ui.edit_Line_thickness->setText(m_strLine_thickness);
    ui.edit_Distance->setText(m_strDistance);
    ui.edit_Ring->setText(m_strRing);
    ui.check_Punkt->setCheckState(m_bPunkt ? Qt::Checked : Qt::Unchecked);
    ui.combo_Open->setCurrentIndex(m_cmbOpen);
    ui.combo_Mag_ras->setCurrentIndex(m_cmbMag_ras);
    ui.combo_Typ->setCurrentIndex(m_cmbTyp);
    ui.spin_Line_thickness->setValue(m_iLine_thickness);

}

void CColors2Dialog::read()
{
    m_labelColors_window = ui.label_Colors_window->text();
    m_strLine_thickness = ui.edit_Line_thickness->text();
    m_strDistance = ui.edit_Distance->text();
    m_strRing = ui.edit_Ring->text();
    m_bPunkt = ui.check_Punkt->checkState() == Qt::Checked;
    m_cmbOpen = ui.combo_Open->currentIndex();
    m_cmbMag_ras = ui.combo_Mag_ras->currentIndex();
    m_cmbTyp = ui.combo_Typ->currentIndex();
    m_iLine_thickness = ui.spin_Line_thickness->value();

}

#if 0

void CColors2Dialog::onPod() {
    read();
    write();
}

void CColors2Dialog::onObr() {
    read();
    write();
}

void CColors2Dialog::onDefault() {
    read();
    write();
}

void CColors2Dialog::onPodf() {
    read();
    write();
}

void CColors2Dialog::onObrf() {
    read();
    write();
}

void CColors2Dialog::onColor_uzel() {
    read();
    write();
}

void CColors2Dialog::onDefault2() {
    read();
    write();
}
#endif


void CColors2Dialog::changed()
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

void CColors2Dialog::changed0()
{
    if (this->isVisible())  {
        if (prop_callback0) {
            read();
            if ((*prop_callback0)(this)) {
                write();
            }
        }
    }
}


void CColors2Dialog::stateChanged(int)
{
    changed();
}

void CColors2Dialog::buttonClicked(QAbstractButton*)
{
    changed();
}

void CColors2Dialog::valueChanged(int i)
{
    changed();
}

void  CColors2Dialog::textEdited(QString txt)
{
    changed();
}

void  CColors2Dialog::currentTextChanged(QString txt)
{
    changed0();
}

void CColors2Dialog::okClicked()
{
    read();

    done(QDialog::Accepted);
}

