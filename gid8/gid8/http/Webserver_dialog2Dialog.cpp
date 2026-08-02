#include <QApplication>
#include <QtGui>
#include <QtWidgets>

#include "Webserver_dialog2Dialog.h"
#include <edit/menu.h>

CWebserver_dialog2Dialog::CWebserver_dialog2Dialog(GidWidget *parent)
  : QDialog((QWidget*)parent)
{

    this->gid = parent;
    
    ui.setupUi(this);

    ui.spin_Port->setRange(1, 65535);
    ui.spin_Port_https->setRange(1, 65535);


/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/
    QSettings settings;
    m_iPort = settings.value("http/port", 8081).toInt(); 




    write();

    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
//    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui.push_Start, SIGNAL(clicked()), this, SLOT(onStart()));
    connect(ui.push_Stop, SIGNAL(clicked()), this, SLOT(onStop()));
    connect(ui.push_Reset, SIGNAL(clicked()), this, SLOT(onReset()));
    connect(ui.push_Homedirbrowse, SIGNAL(clicked()), this, SLOT(onHomedirbrowse()));
    connect(ui.push_Privatebrowse, SIGNAL(clicked()), this, SLOT(onPrivatebrowse()));
    connect(ui.push_Publicbrowse, SIGNAL(clicked()), this, SLOT(onPublicbrowse()));
    connect(ui.push_Tile_dir_browse, SIGNAL(clicked()), this, SLOT(onTile_dir_browse()));
    connect(ui.push_Web_dir_browse, SIGNAL(clicked()), this, SLOT(onWeb_dir_browse()));

//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));


}


void CWebserver_dialog2Dialog::write()
{
    ui.edit_Homedir->setText(m_strHomedir);
    ui.edit_Tiles_dir->setText(m_strTiles_dir);
    ui.edit_Defindexfile->setText(m_strDefindexfile);
    ui.edit_Port->setText(m_strPort);
    ui.edit_Pto->setText(m_strPto);
    ui.edit_Private->setText(m_strPrivate);
    ui.edit_Public->setText(m_strPublic);
    ui.edit_Port_https->setText(m_strPort_https);
    ui.edit_Web_dir->setText(m_strWeb_dir);
    ui.check_Https->setCheckState(m_bHttps ? Qt::Checked : Qt::Unchecked);
    ui.spin_Port->setValue(m_iPort);
    ui.spin_Port_https->setValue(m_iPort_https);

}


void CWebserver_dialog2Dialog::read()
{
    m_strHomedir = ui.edit_Homedir->text();
    m_strTiles_dir = ui.edit_Tiles_dir->text();
    m_strDefindexfile = ui.edit_Defindexfile->text();
    m_strPort = ui.edit_Port->text();
    m_strPto = ui.edit_Pto->text();
    m_strPrivate = ui.edit_Private->text();
    m_strPublic = ui.edit_Public->text();
    m_strPort_https = ui.edit_Port_https->text();
    m_strWeb_dir = ui.edit_Web_dir->text();
    m_bHttps = ui.check_Https->checkState() == Qt::Checked;
    m_iPort = ui.spin_Port->value();
    m_iPort_https = ui.spin_Port_https->value();

}

#if 0
void CWebserver_dialog2Dialog::onStart() {
    QMessageBox::warning(this, "", "Start");
}
void CWebserver_dialog2Dialog::onStop() {
    QMessageBox::warning(this, "", "Stop");
}
void CWebserver_dialog2Dialog::onReset() {
    QMessageBox::warning(this, "", "Reset");
}
void CWebserver_dialog2Dialog::onHomedirbrowse() {
    QMessageBox::warning(this, "", "Homedirbrowse");
}
void CWebserver_dialog2Dialog::onPrivatebrowse() {
    QMessageBox::warning(this, "", "Privatebrowse");
}
void CWebserver_dialog2Dialog::onPublicbrowse() {
    QMessageBox::warning(this, "", "Publicbrowse");
}
void CWebserver_dialog2Dialog::onTile_dir_browse() {
    QMessageBox::warning(this, "", "Tile_dir_browse");
}
void CWebserver_dialog2Dialog::onWeb_dir_browse() {
    QMessageBox::warning(this, "", "Web_dir_browse");
}
#endif


void CWebserver_dialog2Dialog::okClicked()
{
    read();

    QSettings settings;
    settings.value("http/port", m_iPort); 

    done(QDialog::Accepted);
}

