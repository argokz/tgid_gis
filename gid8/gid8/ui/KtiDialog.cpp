#include <QtGui>
#include <QtWidgets>

#include "KtiDialog.h"
#include <edit/menu.h>

CKtiDialog::CKtiDialog(QWidget *parent)
  : QDialog(parent) , save(this, "CKtiDialog")
{
    ui.setupUi(this);

/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    connect(ui.edit_Tn, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Tg, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_T1, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_T2, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Dt, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Dt2, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Ras, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Ras2, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Len, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Diametr, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Diametr_usl, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Tol, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Dateend, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Kti, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Kti1, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Kti2, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Tpn, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.edit_Tpf, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(ui.check_Kolwork, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui.combo_Typ_pr, SIGNAL(currentTextChanged(const QString &)), this, SLOT(currentTextChanged(const QString &)));
    connect(ui.bg_Radio1, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));



    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui.push_Save, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(ui.push__help, SIGNAL(clicked()), this, SLOT(on_help()));
    setRadioId(ui.bg_Radio1);

//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));
}


void CKtiDialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CKtiDialog::write()
{
    ui.edit_Tn->setText(m_strTn);
    ui.edit_Tg->setText(m_strTg);
    ui.edit_T1->setText(m_strT1);
    ui.edit_T2->setText(m_strT2);
    ui.edit_Dt->setText(m_strDt);
    ui.edit_Dt2->setText(m_strDt2);
    ui.edit_Ras->setText(m_strRas);
    ui.edit_Ras2->setText(m_strRas2);
    ui.edit_Len->setText(m_strLen);
    ui.edit_Diametr->setText(m_strDiametr);
    ui.edit_Diametr_usl->setText(m_strDiametr_usl);
    ui.edit_Tol->setText(m_strTol);
    ui.edit_Dateend->setText(m_strDateend);
    ui.edit_Kti->setText(m_strKti);
    ui.edit_Kti1->setText(m_strKti1);
    ui.edit_Kti2->setText(m_strKti2);
    ui.edit_Tpn->setText(m_strTpn);
    ui.edit_Tpf->setText(m_strTpf);
    ui.check_Kolwork->setCheckState(m_bKolwork ? Qt::Checked : Qt::Unchecked);
    ui.combo_Typ_pr->setCurrentIndex(m_cmbTyp_pr);
    {
        QAbstractButton *but = ui.bg_Radio1->button(m_radioRadio1);
        if (but) but->setChecked(true);
    }

}

void CKtiDialog::read()
{
    m_strTn = ui.edit_Tn->text();
    m_strTg = ui.edit_Tg->text();
    m_strT1 = ui.edit_T1->text();
    m_strT2 = ui.edit_T2->text();
    m_strDt = ui.edit_Dt->text();
    m_strDt2 = ui.edit_Dt2->text();
    m_strRas = ui.edit_Ras->text();
    m_strRas2 = ui.edit_Ras2->text();
    m_strLen = ui.edit_Len->text();
    m_strDiametr = ui.edit_Diametr->text();
    m_strDiametr_usl = ui.edit_Diametr_usl->text();
    m_strTol = ui.edit_Tol->text();
    m_strDateend = ui.edit_Dateend->text();
    m_strKti = ui.edit_Kti->text();
    m_strKti1 = ui.edit_Kti1->text();
    m_strKti2 = ui.edit_Kti2->text();
    m_strTpn = ui.edit_Tpn->text();
    m_strTpf = ui.edit_Tpf->text();
    m_bKolwork = ui.check_Kolwork->checkState() == Qt::Checked;
    m_cmbTyp_pr = ui.combo_Typ_pr->currentIndex();
    m_radioRadio1 = ui.bg_Radio1->checkedId();

}

#if 1

void CKtiDialog::onSave() {
    read();
    write();
}

void CKtiDialog::on_help() {
    read();
    write();
}
#endif


void CKtiDialog::changed()
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

void CKtiDialog::stateChanged(int)
{
    changed();
}

void CKtiDialog::buttonClicked(QAbstractButton*)
{
    changed();
}


void  CKtiDialog::textChanged(QString txt)
{
    changed();
}

void  CKtiDialog::currentTextChanged(QString txt)
{
    changed();
}

void CKtiDialog::okClicked()
{
    read();

    done(QDialog::Accepted);
}

