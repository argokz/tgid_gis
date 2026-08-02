#pragma once

#include <QToolButton>
#include <QFontMetrics>
#include <QResizeEvent>

class WToolButton : public QToolButton {
    Q_OBJECT

public:
//    explicit
    WToolButton(int _size, QWidget* parent = nullptr);
    ~WToolButton();

    void setText(const QString &text); // override;


protected:
//    void actionEvent(QActionEvent *event);

    bool eventFilter(QObject *watched, QEvent *event) override;
    
    void resizeEvent(QResizeEvent* event) override {
        QToolButton::resizeEvent(event);
        adjustWidth();
    }


private:
    void adjustWidth();
    bool br = false;
    QString _text = "";
};
