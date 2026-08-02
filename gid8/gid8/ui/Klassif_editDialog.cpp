#include <QtGui>
#include <QtWidgets>

#include "Klassif_editDialog.h"
#include <edit/menu.h>

CKlassif_editDialog::CKlassif_editDialog(QWidget *parent)
  : QDialog(parent) , save(this, "CKlassif_editDialog")
{
    ui.setupUi(this);

/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    connect(ui.edit_Name, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Prio, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Width, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Rus, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.check_Label, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui.check_Ramka, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui.check_Punct, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui.check_Label_color, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui.check_Nomas, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui.combo_List_label, SIGNAL(currentTextChanged(const QString &)), this, SLOT(currentTextChanged(const QString &)));
    connect(ui.combo_List_label_color, SIGNAL(currentTextChanged(const QString &)), this, SLOT(currentTextChanged(const QString &)));
    connect(ui.combo_Loc, SIGNAL(currentTextChanged(const QString &)), this, SLOT(currentTextChanged(const QString &)));



    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui.push_Rastr, SIGNAL(clicked()), this, SLOT(onRastr()));
    connect(ui.push_Color, SIGNAL(clicked()), this, SLOT(onColor()));
    connect(ui.push_Color2, SIGNAL(clicked()), this, SLOT(onColor2()));
    connect(ui.push_Font, SIGNAL(clicked()), this, SLOT(onFont()));

//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));
}


void CKlassif_editDialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CKlassif_editDialog::write()
{
    ui.label_Color_w2->setText(m_labelColor_w2);
    ui.label_Color_w->setText(m_labelColor_w);
    ui.label_Width_static->setText(m_labelWidth_static);
    ui.edit_Name->setText(m_strName);
    ui.edit_Prio->setText(m_strPrio);
    ui.edit_Width->setText(m_strWidth);
    ui.edit_Rus->setText(m_strRus);
    ui.check_Label->setCheckState(m_bLabel ? Qt::Checked : Qt::Unchecked);
    ui.check_Ramka->setCheckState(m_bRamka ? Qt::Checked : Qt::Unchecked);
    ui.check_Punct->setCheckState(m_bPunct ? Qt::Checked : Qt::Unchecked);
    ui.check_Label_color->setCheckState(m_bLabel_color ? Qt::Checked : Qt::Unchecked);
    ui.check_Nomas->setCheckState(m_bNomas ? Qt::Checked : Qt::Unchecked);
    ui.combo_List_label->setCurrentIndex(m_cmbList_label);
    ui.combo_List_label_color->setCurrentIndex(m_cmbList_label_color);
    ui.combo_Loc->setCurrentIndex(m_cmbLoc);
    ui.spin_Prio->setValue(m_iPrio);
    ui.spin_Width->setValue(m_iWidth);

}

void CKlassif_editDialog::read()
{
    m_labelColor_w2 = ui.label_Color_w2->text();
    m_labelColor_w = ui.label_Color_w->text();
    m_labelWidth_static = ui.label_Width_static->text();
    m_strName = ui.edit_Name->text();
    m_strPrio = ui.edit_Prio->text();
    m_strWidth = ui.edit_Width->text();
    m_strRus = ui.edit_Rus->text();
    m_bLabel = ui.check_Label->checkState() == Qt::Checked;
    m_bRamka = ui.check_Ramka->checkState() == Qt::Checked;
    m_bPunct = ui.check_Punct->checkState() == Qt::Checked;
    m_bLabel_color = ui.check_Label_color->checkState() == Qt::Checked;
    m_bNomas = ui.check_Nomas->checkState() == Qt::Checked;
    m_cmbList_label = ui.combo_List_label->currentIndex();
    m_cmbList_label_color = ui.combo_List_label_color->currentIndex();
    m_cmbLoc = ui.combo_Loc->currentIndex();
    m_iPrio = ui.spin_Prio->value();
    m_iWidth = ui.spin_Width->value();

}

#if 0

void CKlassif_editDialog::onRastr() {
    read();
    write();
}

void CKlassif_editDialog::onColor() {
    read();
    write();
}

void CKlassif_editDialog::onColor2() {
    read();
    write();
}

void CKlassif_editDialog::onFont() {
    read();
    write();
}
#endif


void CKlassif_editDialog::changed()
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

void CKlassif_editDialog::stateChanged(int)
{
    changed();
}

void CKlassif_editDialog::buttonClicked(QAbstractButton*)
{
    changed();
}


void  CKlassif_editDialog::textChanged(QString txt)
{
    changed();
}

void  CKlassif_editDialog::currentTextChanged(QString txt)
{
    changed();
}

void CKlassif_editDialog::okClicked()
{
    read();

    done(QDialog::Accepted);
}

