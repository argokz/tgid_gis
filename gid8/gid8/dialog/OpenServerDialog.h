#pragma once

#include <QDialog>
#include <QVariant>
#include <QtWidgets>

class QListWidget;
class QLineEdit;


#include <dialog/SaveGeometry.h>

#include <dialog/MyDialog.h>

#include <ui2/OpenServer.h>

class OpenServerDialog : public MyDialog
{
    Q_OBJECT

public:
    OpenServerDialog(QWidget *parent, const QString & title);
    
    ConnectStr cs;


private slots:
    void okClicked();
    void helpClicked();

    void checkClicked();
    void newClicked();

    void onBdGid();
    void onBdGeo();
    void onBdGid9();


private:

    void read_cs();
    
    SaveGeometry save;
};
