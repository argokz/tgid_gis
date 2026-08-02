#pragma once

#include <QDialog>

class QListWidget;
class GeoMap;
class QLineEdit;


class KlDial : public QDialog
{
    Q_OBJECT

public:
    KlDial(const GeoMap &gmap, QWidget *parent);
    QString curText;
    int index;

private slots:
    void okClicked();
    void editClicked();
    void onClicked();
    void offClicked();
    void helpClicked();


private:
    QListWidget *lw;
};
