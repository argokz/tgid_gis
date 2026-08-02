#include <QtGui>
#include <QtSql>
#include <QVariant>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/ChMenuDial.h>

#include "info.h"

void GidWidget::initInf()
{
    QSettings settings;

m_Info.clear();

if (settings.value("info/NAPOR", false).toBool()) m_Info.insert(I_NAPOR);
if (settings.value("info/RAS", false).toBool()) m_Info.insert(I_RAS);
if (settings.value("info/DLINA", false).toBool()) m_Info.insert(I_DLINA);
if (settings.value("info/DIAM_V", false).toBool()) m_Info.insert(I_DIAM_V);
if (settings.value("info/DIAM_U", false).toBool()) m_Info.insert(I_DIAM_U);
if (settings.value("info/OB", false).toBool()) m_Info.insert(I_OB);
if (settings.value("info/TEMP", false).toBool()) m_Info.insert(I_TEMP);
if (settings.value("info/RAS_U", false).toBool()) m_Info.insert(I_RAS_U);
if (settings.value("info/SKOR", false).toBool()) m_Info.insert(I_SKOR);
if (settings.value("info/UD_POT", false).toBool()) m_Info.insert(I_UD_POT);
if (settings.value("info/NAGR", false).toBool()) m_Info.insert(I_NAGR);
if (settings.value("info/RASP", false).toBool()) m_Info.insert(I_RASP);
if (settings.value("info/GEOD", false).toBool()) m_Info.insert(I_GEOD);
if (settings.value("info/NAGR_U", false).toBool()) m_Info.insert(I_NAGR_U);
if (settings.value("info/NAPOR2", false).toBool()) m_Info.insert(I_NAPOR2);
}


void GidWidget::onInf()
{
    ChMenuDial dlg(this, tr("Отображаемая информация"));

    QSettings settings;

dlg.Add(QObject::tr("Напоры, H, м.вод.ст."),                          I_NAPOR, settings.value("info/NAPOR", false).toBool());
dlg.Add(QObject::tr("Расходы, G, т/ч"),                               I_RAS, settings.value("info/RAS", false).toBool());
dlg.Add(QObject::tr("Длины, L, м"),                                   I_DLINA, settings.value("info/DLINA", false).toBool());
dlg.Add(QObject::tr("Диаметры внутренние, Двн, мм"),                  I_DIAM_V, settings.value("info/DIAM_V", false).toBool());
dlg.Add(QObject::tr("Диаметры условные, Ду, мм"),                     I_DIAM_U, settings.value("info/DIAM_U", false).toBool());
dlg.Add(QObject::tr("Объемы, V, м3"),                                 I_OB, settings.value("info/OB", false).toBool());
dlg.Add(QObject::tr("Температуры, t, °C"),                            I_TEMP, settings.value("info/TEMP", false).toBool());
dlg.Add(QObject::tr("Узловой расход, G, т/ч"),                        I_RAS_U, settings.value("info/RAS_U", false).toBool());
dlg.Add(QObject::tr("Скорость, w, м/c"),                              I_SKOR, settings.value("info/SKOR", false).toBool());
dlg.Add(QObject::tr("Удельные потери, R, мм.вод.ст./м"),              I_UD_POT, settings.value("info/UD_POT", false).toBool());
dlg.Add(QObject::tr("Нагрузки, G, Гкал/ч"),                           I_NAGR, settings.value("info/NAGR", false).toBool());
dlg.Add(QObject::tr("Располагаемый напор, dH, м"),                    I_RASP, settings.value("info/RASP", false).toBool());
dlg.Add(QObject::tr("Геодезическая отметка, h, м"),                   I_GEOD, settings.value("info/GEOD", false).toBool());
dlg.Add(QObject::tr("Расчетная нагрузка на участках, Q, Гкал/ч"),     I_NAGR_U, settings.value("info/NAGR_U", false).toBool());
dlg.Add(QObject::tr("Полные напоры, H, м.вод.ст."),                   I_NAPOR2, settings.value("info/NAPOR2", false).toBool());


    if (dlg.exec()) {
        for (auto & it : dlg.out) {
settings.setValue("info/NAPOR",      dlg.inOut(I_NAPOR));
settings.setValue("info/RAS",        dlg.inOut(I_RAS));
settings.setValue("info/DLINA",      dlg.inOut(I_DLINA));
settings.setValue("info/DIAM_V",     dlg.inOut(I_DIAM_V));
settings.setValue("info/DIAM_U",     dlg.inOut(I_DIAM_U));
settings.setValue("info/OB",         dlg.inOut(I_OB));
settings.setValue("info/TEMP",       dlg.inOut(I_TEMP));
settings.setValue("info/RAS_U",      dlg.inOut(I_RAS_U));
settings.setValue("info/SKOR",       dlg.inOut(I_SKOR));
settings.setValue("info/UD_POT",     dlg.inOut(I_UD_POT));
settings.setValue("info/NAGR",       dlg.inOut(I_NAGR));
settings.setValue("info/RASP",       dlg.inOut(I_RASP));
settings.setValue("info/GEOD",       dlg.inOut(I_GEOD));
settings.setValue("info/NAGR_U",     dlg.inOut(I_NAGR_U));
settings.setValue("info/NAPOR2",     dlg.inOut(I_NAPOR2));
        }

        m_Info.clear();

        for (auto &it : dlg.out) {
            m_Info.insert(it.toInt());
        }

        m_bIsGidrInf = dlg.out.size() != 0;

        repaint();
    }
}


bool GidWidget::inInfo(int id)
{
    return m_Info.find(id) != m_Info.end();
}

