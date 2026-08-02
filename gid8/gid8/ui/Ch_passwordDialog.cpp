#include <QtGui>
#include <QtWidgets>

#include "Ch_passwordDialog.h"
#include <edit/menu.h>

CCh_passwordDialog::CCh_passwordDialog(QWidget *parent)
  : QDialog(parent) , save(this, "CCh_passwordDialog")
{
    ui.setupUi(this);

/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));


//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));


}


void CCh_passwordDialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CCh_passwordDialog::write()
{
    ui.edit_User->setText(m_strUser);
    ui.check_Right1->setCheckState(m_bRight1 ? Qt::Checked : Qt::Unchecked);
    ui.check_Right2->setCheckState(m_bRight2 ? Qt::Checked : Qt::Unchecked);
    ui.check_Right3->setCheckState(m_bRight3 ? Qt::Checked : Qt::Unchecked);
    ui.check_Right4->setCheckState(m_bRight4 ? Qt::Checked : Qt::Unchecked);
    ui.check_Right5->setCheckState(m_bRight5 ? Qt::Checked : Qt::Unchecked);
    ui.check_Right6->setCheckState(m_bRight6 ? Qt::Checked : Qt::Unchecked);
    ui.check_Right7->setCheckState(m_bRight7 ? Qt::Checked : Qt::Unchecked);
    ui.check_Right8->setCheckState(m_bRight8 ? Qt::Checked : Qt::Unchecked);
    ui.check_Right9->setCheckState(m_bRight9 ? Qt::Checked : Qt::Unchecked);

//    ui.check_Check2->setCheckState(m_bCheck2 ? Qt::Checked : Qt::Unchecked);

}


void CCh_passwordDialog::read()
{
    m_strUser = ui.edit_User->text();
//    m_bCheck2 = ui.check_Check2->checkState() == Qt::Checked;

    m_bRight1 = ui.check_Right1->checkState() == Qt::Checked;
    m_bRight2 = ui.check_Right2->checkState() == Qt::Checked;
    m_bRight3 = ui.check_Right3->checkState() == Qt::Checked;
    m_bRight4 = ui.check_Right4->checkState() == Qt::Checked;
    m_bRight5 = ui.check_Right5->checkState() == Qt::Checked;
    m_bRight6 = ui.check_Right6->checkState() == Qt::Checked;
    m_bRight7 = ui.check_Right7->checkState() == Qt::Checked;
    m_bRight8 = ui.check_Right8->checkState() == Qt::Checked;
    m_bRight9 = ui.check_Right9->checkState() == Qt::Checked;

}



void CCh_passwordDialog::okClicked()
{
    read();

//    QMessageBox::warning(this, "", user);
//    QSettings settings;
//    settings.setValue("user", user);

    done(QDialog::Accepted);
}

