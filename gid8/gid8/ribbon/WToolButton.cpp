#include <QToolButton>
#include <QFontMetrics>
#include <QResizeEvent>
#include "WToolButton.h"

void WToolButton::adjustWidth() 
{
//    QFontMetrics metrics(font());
//    int textWidth = metrics.horizontalAdvance(text()) + 10; // Добавляем запас
//    setMinimumWidth(textWidth);
}


WToolButton::WToolButton(int typ, QWidget* parent) :  QToolButton(parent)
{
    br = (typ == 1);
    this->installEventFilter(this);  // Устанавливаем фильтр событий
    adjustWidth();
}

WToolButton::~WToolButton() 
{
}

QString ispr_text(const QString &atext);


void WToolButton::setText(const QString &text)
{
//    QString customText = "Мой текст вместо " + text;  // Заменяем текст на свой
    QString atext = ispr_text(text);

    if (!br) {
        atext = atext.replace("\n", " ");
    }
    _text = atext;


    QToolButton::setText(atext);  // Устанавливаем замененный текст
}


bool WToolButton::eventFilter(QObject *watched, QEvent *event)
{
//    qDebug() << "Event type:" << event->type();

    // Проверяем, что событие произошло с кнопкой и что это событие изменения текста
    if (watched == this && event->type() == QEvent::ActionChanged) {

        // Проверяем, если изменился текст
        if (property("text") == QVariant(text())) {
            QActionEvent *ae = (QActionEvent*)(event);

            if (ae) {
                QAction *action = ae->action();

                if (action) {
//                    qDebug() << "Action changed. New action text:" << action-> text();
                    this->setChecked(action->isChecked());
//                    this->setEnabled(action->enabled());

//                    QString customText = ispr_text(text());
//                    if (!br) {
//                        customText = customText.replace("\n", " ");
//                    }
//                    qDebug() << customText;
//                    QToolButton::setText(customText);  // Устанавливаем новый текст

                    QToolButton::setText(_text);  // Устанавливаем новый текст
                }
            }
            
            return true;  // Обработка завершена, событие перехвачено

        }
    }
    return QToolButton::eventFilter(watched, event);
}
