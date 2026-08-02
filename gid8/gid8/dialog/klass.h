#pragma once

#include <QDialog>

class QListWidget;
class Klassif;
class QLineEdit;
class WExample;


class Klass : public QDialog
{
    Q_OBJECT

public:
    Klass(Klassif *kls, QWidget *parent);
    QString curText;
    int index;

private slots:
    void okClicked();
    void editClicked();
    void helpClicked();
    void colorLClicked();
    void colorBClicked();
    void fontClicked();


private:
    WExample *w;
    Klassif *m_kls;
};

