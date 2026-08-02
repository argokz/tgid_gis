#include <QtGui>
#include <QtWidgets>

#include "PotrcxemaDialog.h"
#include <edit/menu.h>

#include <gidview/GidWidget.h>

CPotrcxemaDialog::CPotrcxemaDialog(QWidget *parent)
  : QDialog(parent) , save(this, "CPotrcxemaDialog")
{
    ui.setupUi(this);

    customWidget = new RPDrawerWidget(this);

    // Устанавливаем политику размеров
    customWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Добавляем виджет в макет
    ui.verticalLayout->addWidget(customWidget);
//    ui.groupBox_cxema->setStyleSheet("background-color: white");

    ui.groupBox_cxema->setStyleSheet("QGroupBox::title { font-weight: bold; }");
    
/*    
    ui.groupBox_cxema->setStyleSheet(R"V0G0N(

QGroupBox {
    background-color: white;
    margin-top:1em;
}

QGroupBox::title {
    subcontrol-origin: padding;
    subcontrol-position: left top;
    background: transparent;
    margin-top: -2.5em;
}
    )V0G0N");
*/
/*
    QSettings settings;
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
*/

    write();

    connect(ui.check_Rez, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui.check_Ventil, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui.check_Otopl, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui.check_Gws, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui.bg_Zavis, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
    connect(ui.bg_Elevat, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
    connect(ui.bg_Podacha, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));



    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));

    setRadioId(ui.bg_Zavis);
    setRadioId(ui.bg_Elevat);
    setRadioId(ui.bg_Podacha);

//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
//    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

//    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));
}

void CPotrcxemaDialog::setSchemeNumber(QString number){

    customWidget->schemeNumber = number;
    customWidget->repaint();
    customWidget->update();
}
void CPotrcxemaDialog::paintEvent(QPaintEvent * /* event */)
{
    // QPainter painter(ui.group_Cxema_pr);

    // // Устанавливаем параметры рисования (опционально)
    // painter.setRenderHint(QPainter::Antialiasing); // Сглаживание
    // painter.setPen(QPen(Qt::blue, 2));             // Синий цвет, толщина 2
    // painter.setBrush(Qt::NoBrush);                // Без заливки

    // // Проходим по родителям и ищем GidWidget
    // QObject *current = this->parent();
    // while (current) {
    //     if (auto gidWidget = qobject_cast<GidWidget *>(current)) {
    //         // Рисуем внутри ui.group_Cxema_pr
    //         painter.drawLine(10, 10, 100, 100); // Пример: линия от (10,10) до (100,100)
    //         painter.drawPoint(50, 50);         // Пример: точка в (50,50)
    //         painter.drawLine(20, 80, 150, 30); // Еще одна линия

    //         // Вызываем метод рисования GidWidget, если нужно
    //         //gidWidget->draw(&painter);

    //         // Обновляем виджеты
    //         ui.group_Cxema_pr->update();
    //         update();

    //         qDebug() << "Родитель является GidWidget";
    //         break;
    //     }
    //     current = current->parent();
    // }

    // if (!current) {
    //     qDebug() << "Родитель не найден среди GidWidget";
    // }
}

void CPotrcxemaDialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CPotrcxemaDialog::write()
{
//    ui.label_Nomer->setText(m_labelNomer);

    ui.groupBox_cxema->setTitle("Номер схемы: " + m_labelNomer);
    ui.groupBox_cxema->setStyleSheet("QGroupBox::title { font-weight: bold; }");

    ui.check_Rez->setCheckState(m_bRez ? Qt::Checked : Qt::Unchecked);
    ui.check_Ventil->setCheckState(m_bVentil ? Qt::Checked : Qt::Unchecked);
    ui.check_Otopl->setCheckState(m_bOtopl ? Qt::Checked : Qt::Unchecked);
    ui.check_Gws->setCheckState(m_bGws ? Qt::Checked : Qt::Unchecked);
    {
        QAbstractButton *but = ui.bg_Zavis->button(m_radioZavis);
        if (but) but->setChecked(true);
    }
    {
        QAbstractButton *but = ui.bg_Elevat->button(m_radioElevat);
        if (but) but->setChecked(true);
    }
    {
        QAbstractButton *but = ui.bg_Podacha->button(m_radioPodacha);
        if (but) but->setChecked(true);
    }

}

void CPotrcxemaDialog::read()
{
//    m_labelNomer = ui.label_Nomer->text();
    m_bRez = ui.check_Rez->checkState() == Qt::Checked;
    m_bVentil = ui.check_Ventil->checkState() == Qt::Checked;
    m_bOtopl = ui.check_Otopl->checkState() == Qt::Checked;
    m_bGws = ui.check_Gws->checkState() == Qt::Checked;
    m_radioZavis = ui.bg_Zavis->checkedId();
    m_radioElevat = ui.bg_Elevat->checkedId();
    m_radioPodacha = ui.bg_Podacha->checkedId();

}



void CPotrcxemaDialog::changed()
{
    if (this->isVisible())  {
        if (prop_callback) {
            read();
            if ((*prop_callback)(this)) {
                write();
            }
        }
    }
}

void CPotrcxemaDialog::stateChanged(int)
{
    changed();
}

void CPotrcxemaDialog::buttonClicked(QAbstractButton*)
{
    changed();
}



//bool CPotrcxemaDialog::eventFilter( QObject * o, QEvent * e )
//{
//  return QDialog::eventFilter( o, e );
//}

void CPotrcxemaDialog::okClicked()
{
    read();

//    QMessageBox::warning(this, "", user);
//    QSettings settings;
//    settings.setValue("user", user);

    done(QDialog::Accepted);
}

