#ifndef SLDRAWERWIDGET_H
#define SLDRAWERWIDGET_H

#include <QWidget>
#include <QPainter>
class SLDrawerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SLDrawerWidget(QWidget *parent = nullptr);

public:
    int mag_ras = 0;
    int tubingTypeID = 0;
    bool open = true;
    bool q = true;
    double m_ring = 1;
protected:
    void paintEvent(QPaintEvent *event) override;
signals:
};

#endif // SLDRAWERWIDGET_H
