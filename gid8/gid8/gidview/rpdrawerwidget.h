#ifndef RPDRAWERWIDGET_H
#define RPDRAWERWIDGET_H

#include <QWidget>
#include <QPainter>
class RPDrawerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RPDrawerWidget(QWidget *parent = nullptr);

public:
    QString schemeNumber = "0.0";
protected:
    void paintEvent(QPaintEvent *event) override;
signals:
};

#endif // RPDRAWERWIDGET_H
