#pragma once

#include <QDialog>
#include <dialog/SaveGeometry.h>

class TGraph;
class DbWindow;

class TGDialog : public QDialog
{
    Q_OBJECT

public:
    TGDialog(QWidget *parent, CCxema *cxema, QSqlDatabase &db, int id, int id2);
    ~TGDialog();

private slots:
    void okClicked();
    void b1Clicked();
    void b2Clicked();
    void b3Clicked();
    void b4Clicked();
    void b5Clicked();

    void onStatAccepted();

    void cb1_stateChanged(int state);
    void cb2_stateChanged(int state);
    void cb3_stateChanged(int state);
    void cb4_stateChanged(int state);

//    void editClicked();
    void helpClicked();
//    void colorLClicked();
//    void colorBClicked();
//    void fontClicked();

    void redraw_tg();


private:
    CCxema *m_cxema;
    QSqlDatabase m_db;
    int m_id;
    int m_id2;

    QString m_q = "";

    SaveGeometry save;

    QSplitter *splitter;
    TGraph *w;
    DbWindow *tb;
};

