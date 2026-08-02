#include <QtGui>
#include <QtWidgets>

#include "Nas_charDialog.h"
#include <edit/menu.h>


CNas_charDialog::CNas_charDialog(QWidget *parent, QSqlDatabase &db, int pumpIdValue)
    : QDialog(parent) , save(this, "CNas_charDialog")
{
    m_db = &db;
    pumpId = pumpIdValue;
    ui.setupUi(this);

    write();

    connect(ui.edit_Edit_fzad, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui.edit_Edit_p, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui.edit_Edit_g, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui.edit_Edit_s, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui.edit_Edit_kol, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui.edit_Edit_dnom, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui.edit_Edit_fnom, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui.bg_RadioFunction, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
    connect(ui.bg_RadioDrive, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
    connect(ui.bg_RadioWheel, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
    connect(ui.bg_RadioResistance, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));



    connect(ui.push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.push_Cancel, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui.push_Button6, SIGNAL(clicked()), this, SLOT(onButton6()));
    connect(ui.push_Calc, SIGNAL(clicked()), this, SLOT(onCalc()));
    connect(ui.push_Calc_diam, SIGNAL(clicked()), this, SLOT(calcDiam()));

    connect(ui.calcResistanceButton, SIGNAL(clicked()), this, SLOT(onButtonCalcResistance()));

    ui.bg_RadioFunction->setId(ui.radio_Radio1, 0);

    ui.bg_RadioWheel->setId(ui.radio_Radio9, 0);
    ui.bg_RadioWheel->setId(ui.radio_Radio10, 1);

    ui.bg_RadioDrive->setId(ui.radio_Radio7, 0);
    ui.bg_RadioDrive->setId(ui.radio_Radio8, 1);

    ui.bg_RadioResistance->setId(ui.radio_Radio5, 0);
    ui.bg_RadioResistance->setId(ui.radio_Radio6, 1);

}


void CNas_charDialog::setRadioId(QButtonGroup * bg)
{
    QList<QAbstractButton *> lst = bg->buttons();
    int i = 0;
    for (auto &but : lst) {
        bg->setId(but, i++);
    }
}

void CNas_charDialog::write()
{
    ui.edit_Edit_fzad->setText(m_strEdit_fzad);
    ui.edit_Edit_p->setText(m_strEdit_p);
    ui.edit_Edit_g->setText(m_strEdit_g);
    ui.edit_Edit_s->setText(m_strEdit_s);
    ui.edit_Edit_kol->setText(m_strEdit_kol);
    ui.edit_Edit_dnom->setText(m_strEdit_dnom);
    ui.edit_Edit_dzad->setText(m_strEdit_dzad);
    ui.edit_Edit_fnom->setText(m_strEdit_fnom);
    {
        QAbstractButton *but = ui.bg_RadioFunction->button(m_radioFunction);
        if (but) but->setChecked(true);
    }
    {
        QAbstractButton *but = ui.bg_RadioWheel->button(m_radioWheel);
        if (but) but->setChecked(true);
    }
    {
        QAbstractButton *but = ui.bg_RadioDrive->button(m_radioDrive);
        if (but) but->setChecked(true);
    }
    {
        QAbstractButton *but = ui.bg_RadioResistance->button(m_radioResistance);
        if (but) but->setChecked(true);
    }

}

void CNas_charDialog::read()
{
    m_strEdit_fzad = ui.edit_Edit_fzad->text();
    m_strEdit_p = ui.edit_Edit_p->text();
    m_strEdit_g = ui.edit_Edit_g->text();
    m_strEdit_s = ui.edit_Edit_s->text();
    m_strEdit_kol = ui.edit_Edit_kol->text();
    m_strEdit_dnom = ui.edit_Edit_dnom->text();
    m_strEdit_dzad = ui.edit_Edit_dzad->text();
    m_strEdit_fnom = ui.edit_Edit_fnom->text();
    m_radioFunction = ui.bg_RadioFunction->checkedId();
    m_radioDrive = ui.bg_RadioDrive->checkedId();
    m_radioWheel = ui.bg_RadioWheel->checkedId();
    m_radioResistance = ui.bg_RadioResistance->checkedId();

}

#if 1

void CNas_charDialog::onButtonCalcResistance() {
    // Получение текстовых значений из полей
    QString pText = ui.edit_Edit_p->text();
    QString gText = ui.edit_Edit_g->text();

    // Проверка на корректность ввода p
    bool isPValid;
    double p = pText.toDouble(&isPValid);
    if (!isPValid) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректное значение в поле 'p'. Убедитесь, что введено число.");
        return;
    }

    // Проверка на корректность ввода g
    bool isGValid;
    double g = gText.toDouble(&isGValid);
    if (!isGValid) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректное значение в поле 'g'. Убедитесь, что введено число.");
        return;
    }

    // Проверка на деление на ноль
    if (g == 0.0) {
        QMessageBox::warning(this, "Ошибка", "Значение 'g' не может быть равно нулю.");
        return;
    }

    // Вычисление сопротивления
    double s = p / (g * g);
    // Установка результата в поле ui.edit_Edit_s
    ui.edit_Edit_s->setText(QString::number(s, 'g', 15)); // Преобразование числа в строку с 6 знаками после запятой


}

void CNas_charDialog::onButton6() {
    read();
    write();
}

void CNas_charDialog::onCalc() {
    changed();
    int k_kol = ui.edit_Edit_kol->text().toInt();

    if (k_kol > 15) {
        QMessageBox::critical(this, "Ошибка в данных!", "Количество насосов не может быть больше 15");
        return;
    }
    if (k_kol < 1) {
        QMessageBox::critical(this, "Ошибка в данных!", "Количество насосов не может быть меньше 1");
        return;
    }

    if (ui.radio_Radio1->isChecked()){
        PumpComputeDialog dlg(this, *m_db, pumpId, true);
        dlg.addGraph(pumpId, grp, k_kol);

        if (dlg.exec()){
            pReport = dlg.pReport;
            pReport.pump_id = pumpId;

            int driveTypeID = 1;
            if (ui.radio_Radio8->isChecked())
                driveTypeID = 2;
            pReport.driveTypeID = driveTypeID;

            int rotorDiameterTypeID = 1;
            if (ui.radio_Radio10->isChecked())
                rotorDiameterTypeID = 2;
            pReport.rotorDiameterTypeID = rotorDiameterTypeID;


        }
    }

}
#endif


void CNas_charDialog::changed()
{
    if (this->isVisible())  {
        if (prop_callback) {
            read();
            if ((*prop_callback)(this)) {
                write();
            }
        }
    }
    grp.Spr = false;
    grp.IsDnom = true;
    grp.IsFnom = true;
    grp.diam = ui.edit_Edit_dzad->text().toDouble();
    grp.c_vr = ui.edit_Edit_fzad->text().toDouble();
    if (ui.radio_Radio1->isChecked()){
        //функция апроксимации включена


        ui.group_Drive->setEnabled(true);
        if (ui.radio_Radio8->isChecked()){
            //частотно регулируемый привод
            ui.group_DriveData->setEnabled(true);
            grp.IsFnom = false;

        }
        else{
            ui.group_DriveData->setEnabled(false);

        }
        ui.group_WheelData->setEnabled(false);
        ui.group_Wheel->setEnabled(true);

        if (ui.radio_Radio10->isChecked()){
            //срезанное колесо
            ui.group_WheelData->setEnabled(true);
            grp.IsDnom = false;

        }
        else{
            ui.group_WheelData->setEnabled(false);

        }
        ui.group_Resistance->setEnabled(true);
        if (ui.radio_Radio5->isChecked()){
            //учитывать сопротивление
            ui.group_ResistanceData->setEnabled(true);
            grp.Spr = true;
            grp.S = ui.edit_Edit_s->text().toDouble();

        }
        else{
            ui.group_ResistanceData->setEnabled(false);
        }
    }

}

void CNas_charDialog::stateChanged(int)
{
    changed();
}

void CNas_charDialog::buttonClicked(QAbstractButton*)
{
    changed();
}

void CNas_charDialog::calcDiam()
{
    changed();
    int k_kol = ui.edit_Edit_kol->text().toInt();
    PumpComputeDialog dlg(this, *m_db, pumpId, false);
    dlg.addGraph(pumpId, grp, k_kol);
    if (dlg.exec()){
        if (dlg.pReport.d_obt > 0)
            ui.edit_Edit_dzad->setText(QString::number(dlg.pReport.d_obt));
    }
}

void  CNas_charDialog::textChanged(QString txt)
{
    changed();
}

void  CNas_charDialog::currentTextChanged(QString txt)
{
    changed();
}

void CNas_charDialog::okClicked()
{
    int k_kol = ui.edit_Edit_kol->text().toInt();

    if (k_kol > 15) {
        QMessageBox::critical(this, "Ошибка в данных!", "Количество насосов не может быть больше 15");
        return;
    }
    if (k_kol < 1) {
        QMessageBox::critical(this, "Ошибка в данных!", "Количество насосов не может быть меньше 1");
        return;
    }
    if (pReport.calc)

    {
        read();
        emit dialogAccepted(); // Отправляем сигнал
        this->accept();
    }
    else{
        QMessageBox::critical(
            this,
            "Требуется расчет",
            "Перед сохранением данных, пожалуйста, выполните расчет:\n"
            "- Нажмите кнопку \"Расчет\".\n"
            "- В диалоге расчета нажмите кнопку \"Принять\"."
            );
    }
}

