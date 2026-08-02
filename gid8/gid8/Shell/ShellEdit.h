#pragma once

#include "qplaintextedit.h"
#include <QDialog>
#include <QTextEdit>
#include <QSqlQueryModel>
#include <vector>
#include <set>

class GidWidget;

class ShellEdit : public 
//QTextEdit
QPlainTextEdit 
{
    Q_OBJECT

public:
    ShellEdit(QWidget *parent = nullptr);
    void setGid(QWidget *_gid)
    {
        gid = (GidWidget *) _gid;
    }


public slots:
    void OnMoveTo();
//    void OnFind();
//    void OnFindAll();
//    void onPropertyAccepted();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
//    void hideEvent(QHideEvent *event);


private slots:
//    void helpClicked();

private:
    GidWidget *gid = nullptr;
};


