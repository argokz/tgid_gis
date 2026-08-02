#pragma once

#include "qwidget.h"
#include <QSettings>
#include <QApplication>

class SaveGeometry
{
    public:
        SaveGeometry(QWidget *_widget, const QString & _name, const QString & _title = "");
#if 0
        {
            QSettings settings;
            if (_title == "") {
                name = QString("save/%1").arg(_name);
            }
            else {
                name = QString("save/%1/%2").arg(_name, _title);
            }
            widget = _widget;
            QVariant val = settings.value(name);
            if (!val.isNull()) {
                widget->restoreGeometry(val.toByteArray());
            }

            else {
/*
                QWidget *parentWindow = QApplication::activeWindow(); // Текущее активное окно

                if (parentWindow) {
                    // Получаем геометрию активного окна
                    QRect parentGeometry = parentWindow->geometry();
                    int x = parentGeometry.center().x() - (widget->width() / 2); // Центр по X
                    int y = parentGeometry.center().y() - (widget->height() / 2); // Центр по Y

                    // Устанавливаем новую позицию
                    widget->move(x, y);
                } else {
*/
                    // Если активного окна нет, центрируем по экрану
                    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();

                    int w = widget->width();
                    int h = widget->height();

                    w = 400;
                    h = 300;


                    int x = (screenGeometry.width() - w) / 2;
                    int y = (screenGeometry.height() - h) / 2;
                    widget->move(x, y);
//                }
            }
        };
#endif
        ~SaveGeometry()
        {
            save();
        };

        QString get_name() 
        {
            return name;
        }

        void save()
        {
            QSettings settings;
            settings.setValue(name, widget->saveGeometry());
        };


    private:
        QString name;
        QWidget *widget;
};

