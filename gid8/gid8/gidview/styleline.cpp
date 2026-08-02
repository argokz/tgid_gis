#include <QApplication>
#include <QtGui>
#include <QProgressDialog>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>
#include <QInputDialog>
#include "std.h"
#include "sldrawerwidget.h"
#include "colorgtd.h"


#include <ui/Colors2Dialog.h>

SLDrawerWidget *customWidget = nullptr;


double toDouble(const QString &s0)
{
    bool ok;
    QString s = s0;
    s.replace(",", ".");
    return s.toDouble(&ok);
}


void get_l1_l2(CColors2Dialog *prop, int &l1, int &l2, int &l3)
{
    l1 = 0;
    l2 = 3;
    l3 = 0;

    switch (prop->m_cmbMag_ras) {
    case 0: l1 = 0; break;      // Магистраль
    case 1: l1 = 1; break;      // Распредсеть
    case 2: l1 = 2; break;      // Паропровод
    }

    switch (prop->m_cmbTyp) {
    case 0: l2 = 0; break;      // Надземные
    case 1: l2 = 1; break;      // Бесканальные
    case 2: l2 = 2; break;      // Канальные
    case 3: l2 = 3; break;      // Подвальные
    }

    switch (prop->m_cmbOpen) {
    case 0: l3 = 0; break;      // Нагруженная
    case 1: l3 = 1; break;      // Не нагруженная
    case 2: l3 = 2; break;      // Закрытая
    }

}


bool styleline_callback(CColors2Dialog *prop);


bool init0(CColors2Dialog *prop)
{
    int l1, l2, l3;
    get_l1_l2(prop, l1, l2, l3);

    GID_STYLE style1 = m_colors.style(l1, l2);

    prop->m_iLine_thickness = style1.m_thickness;
    prop->m_strDistance = QString("%1").arg(style1.m_distance);


    prop->m_bPunkt = m_colors.style(l1, l2).m_style;


    styleline_callback(prop);
    /*


    bool ok;
    m_colors.style(l1, l2).m_thickness = prop->m_iLine_thickness;
    m_colors.style(l1, l2).m_distance = prop->m_strDistance.toDouble(&ok);

    customWidget->mag_ras = l1;
    customWidget->tubingTypeID = l2;
    customWidget->open = (l3 != 2);
    customWidget->q = (l3 == 0);

    customWidget->repaint();
*/

    return true;
}

bool styleline_callback(CColors2Dialog *prop)
{
    int l1, l2, l3;

    get_l1_l2(prop, l1, l2, l3);

    m_colors.style(l1, l2).m_thickness = prop->m_iLine_thickness;
    m_colors.style(l1, l2).m_distance = toDouble(prop->m_strDistance);


    for (int i = 0; i < 2; i++) {
        m_colors.style(i, l2).m_style = prop->m_bPunkt;
    }


    customWidget->mag_ras = l1;
    customWidget->tubingTypeID = l2;
    customWidget->open = (l3 != 2);
    customWidget->q = (l3 == 0);
    customWidget->m_ring = toDouble(prop->m_strRing);

    customWidget->repaint();

    return true;
}



void CColors2Dialog::onPod() {
    read();

    int l1, l2, l3;
    get_l1_l2(this, l1, l2, l3);

    unsigned long color1 = m_colors.pod(l3, l1, 0);

    QColor color = QColorDialog::getColor(QColor(color1), this);

    if (color.isValid()) {
        m_colors.pod(l3, l1, 0) = color.rgb();
    }

    write();
}

void CColors2Dialog::onObr() {
    read();

    int l1, l2, l3;
    get_l1_l2(this, l1, l2, l3);

    unsigned long color1 = m_colors.obr(l3, l1, 0);

    QColor color = QColorDialog::getColor(QColor(color1), this);

    if (color.isValid()) {
        m_colors.obr(l3, l1, 0) = color.rgb();
    }
    
    write();
}

void CColors2Dialog::onDefault() {
    read();
    m_colors.set_def();
    init0(this);
//    styleline_callback(this);
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
    m_colors.set_defPS();
    init0(this);
//    styleline_callback(this);
    write();
}


void GidWidget::onColors() // Стиль линии
{
    CColors2Dialog dlg(this);

//     QDoubleValidator *validator = new QDoubleValidator(0.0, 10.0, 1);
//     QDoubleValidator *validator = new QDoubleValidator(0.0, 10.0, 1);

    QRegularExpression doubleExp;
//    QRegularExpression *validator = new QRegularExpression();
    doubleExp.setPattern("^(\\d*[\\.,]?\\d+)$");
//    validator->setNotation(QDoubleValidator::StandardNotation); // Обычный ф
    QValidator *validator = new QRegularExpressionValidator(doubleExp);


    dlg.ui.push_Podf->setVisible(false);
    dlg.ui.push_Obrf->setVisible(false);
    dlg.ui.push_Color_uzel->setVisible(false);

    dlg.ui.edit_Distance->setValidator(validator);
    dlg.ui.edit_Ring->setValidator(validator);

    dlg.ui.combo_Open->addItem("Нагруженная");
    dlg.ui.combo_Open->addItem("Не нагруженная");
    dlg.ui.combo_Open->addItem("Закрытая");
    dlg.ui.combo_Mag_ras->addItem("Магистраль");
    dlg.ui.combo_Mag_ras->addItem("Распредсеть");
    dlg.ui.combo_Mag_ras->addItem("Паропровод");
    dlg.ui.combo_Typ->addItem("Надземные");
    dlg.ui.combo_Typ->addItem("Бесканальные");
    dlg.ui.combo_Typ->addItem("Канальные");
    dlg.ui.combo_Typ->addItem("Подвальные");

    dlg.m_cmbMag_ras = 0;
    dlg.m_cmbTyp = 0;
    dlg.m_cmbOpen = 0;

    dlg.m_bPunkt = false;
    dlg.m_iLine_thickness = 1;

    customWidget = new SLDrawerWidget(&dlg);

    dlg.ui.grLayout->addWidget(customWidget);

//    dlg.m_labelColors_window = QString("%1").arg(0);

    GID_STYLE style1 = m_colors.style(0, 0);
    
//    dlg.m_strDistance = QString("%1").arg(1.5);
//    dlg.m_strRing = QString("%1").arg(1);

    dlg.m_iLine_thickness = style1.m_thickness;
    dlg.m_strDistance = QString("%1").arg(style1.m_distance);
    dlg.m_strRing = QString("%1").arg(m_ring);
    dlg.m_bPunkt = style1.m_style;
    
    dlg.setCallback(styleline_callback);
    dlg.setCallback0(init0);
    
    styleline_callback(&dlg);

    dlg.write();


    m_colors.save1();

    if (dlg.exec() == QDialog::Accepted) {
        int m_regim = (m_idMenu == 4) ? 1 : 0;
        m_colors.save(m_regim);
        m_ring = toDouble(dlg.m_strRing);
    }
    else {
        m_colors.load1();
    }
}

