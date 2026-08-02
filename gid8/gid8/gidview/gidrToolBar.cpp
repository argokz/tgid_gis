#include <QtGui>
#include <QtSql>
#include <QtWidgets>
//#include <QtPrinter>
#include <cmath>

#include "std.h"

#include "mainwindow.h"
#include <gidview/GidWidget.h>


std::map<QString, QAction *> map_toolbar_action;
std::map<QAction *, QToolBar *> map_toolbar;

void GidWidget::onToolbarToggle(bool on)
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        auto it = map_toolbar.find(action);
        if (it != map_toolbar.end()) {
            it->second->setVisible(on);
        }
    }
}

void GidWidget::onToolbarToggleExcl(bool on)
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        auto it = map_toolbar.find(action);
        if (it != map_toolbar.end()) {
            QToolBar *bar = it->second;
            bar->setVisible(on);

            if (on) {
                for (auto & it : map_toolbar_action) {
                    QAction *a2 = it.second;
                    if (a2 != action) {
                        auto it2 = map_toolbar.find(a2);
                        if (it2 != map_toolbar.end()) {
                            QToolBar *bar = it2->second;
                            bar->setVisible(false);
                        }
                    }
                }
            }
        }
    }

    for (auto &it : map_toolbar) {
        QAction *a = it.first;
        QToolBar *bar = it.second;
        a->setChecked(bar->isVisible());
    }
}

QAction *addToolbarToggle(QWidget *widget, const QString & text, const QString & text2, const QString & icon_name, bool excl)
{
    QAction *a = new QAction(text);
    if (icon_name != "") {
        a->setIcon(QIcon(icon_name));
    }

    if (excl) {
        map_toolbar_action[text] = a;
    }

    a->setData(QVariant(text));

    a->setToolTip(text2);
//    if (slot_name) {
//        widget->connect(a, SIGNAL(toggled(bool)), widget, slot_name);
//    }

    if (excl) {
        widget->connect(a, SIGNAL(toggled(bool)), widget, SLOT(onToolbarToggleExcl(bool)));
    }
    else {
        widget->connect(a, SIGNAL(toggled(bool)), widget, SLOT(onToolbarToggle(bool)));
    }

    a->setCheckable(true);
    a->setChecked(false);

//    if (group) {
//        group->addAction(a);
//    }

    return a;
}





void GidWidget::createToolBar(MainWindow *main_window)
{
  QToolBar *bar;

  QList<QToolBar *> toolbars = main_window->findChildren<QToolBar *>();

  for (auto b : toolbars) {
      main_window->removeToolBar(b);
  }

  bar = main_window->addToolBar(tr("Гидравлика"));
  bar->setObjectName(tr("Гидравлика"));

//    bar->addAction(gidrAction.aFileNew);
//    bar->addSeparator();
    bar->addAction(gidrAction.aFilePrintPreview);
    bar->addAction(gidrAction.aFilePrint);
    bar->addSeparator();
    bar->addAction(gidrAction.aPrintFr);
    bar->addSeparator();
    bar->addAction(gidrAction.aExcel2);
    bar->addSeparator();
//    bar->addAction(gidrAction.aZam);
    bar->addSeparator();
    bar->addAction(gidrAction.aEditCopy);
    bar->addSeparator();
    bar->addAction(gidrAction.aEditFindNode);
    bar->addAction(gidrAction.aNovisual);
    bar->addAction(gidrAction.aInfo);
    bar->addSeparator();
    bar->addAction(gidrAction.aMoveTo);
    bar->addSeparator();
    bar->addAction(gidrAction.aViewDb);
//    bar->addAction(gidrAction.aViewDbAlma);
    bar->addSeparator();
    bar->addAction(gidrAction.aDoItDr);
    bar->addAction(gidrAction.aDoIt);
    bar->addSeparator();
    bar->addAction(gidrAction.aTeplopoteri);
    bar->addSeparator();
    bar->addAction(gidrAction.aOut2);
//    bar->addAction(gidrAction.aTableDan);  // Это Даниил в свое время делал
    bar->addAction(gidrAction.aAnal);
    bar->addSeparator();
    bar->addAction(gidrAction.aNapr);
    bar->addAction(gidrAction.aPjezo);
//    bar->addAction(gidrAction.aOpengl);
    bar->addSeparator();
    bar->addAction(gidrAction.aZoom);
    bar->addAction(gidrAction.aMovew);
    bar->addAction(gidrAction.aPlus);
    bar->addAction(gidrAction.aMinus);
    bar->addAction(gidrAction.aMasall);
    bar->addSeparator();
    bar->addAction(gidrAction.aNaprGid);
    bar->addAction(gidrAction.aInf);
    bar->addAction(gidrAction.aGidinf);
//    bar->addAction(gidrAction.aRezhim);
    bar->addAction(gidrAction.aPodp);
    bar->addSeparator();
    bar->addAction(gidrAction.aEditor1);
    bar->addAction(gidrAction.aEditor4);
    bar->addSeparator();
    bar->addAction(gidrAction.aF4);
    bar->addSeparator();
    bar->addAction(gidrAction.aHttp2);

    m_barGid = bar;
//    gidrAction.aViewToolbarMain->setChecked(m_barGid && m_barGid->isVisible() );

  bar = main_window->addToolBar(tr("Схема"));
  bar->setObjectName(tr("Схема"));
    bar->addAction(gidrAction.aNode);
    bar->addSeparator();
    bar->addAction(gidrAction.aDel);
    bar->addAction(gidrAction.aLine);
    bar->addAction(gidrAction.aNodeMove);
    bar->addAction(gidrAction.aNodeMove1);
    bar->addAction(gidrAction.aSwap);
    bar->addAction(gidrAction.aRename);
    bar->addAction(gidrAction.aZn);
    bar->addSeparator();
    bar->addAction(gidrAction.aKam);

    m_barCxema = bar;

  bar = main_window->addToolBar(tr("Запросы"));
  bar->setObjectName(tr("Запросы"));

    bar->addAction(gidrAction.aRasprMag);
    bar->addAction(gidrAction.aRasprMag2);
    bar->addAction(gidrAction.aIstSety);
    bar->addAction(gidrAction.aIstSetyNew);
    bar->addAction(gidrAction.aZapOtr);
    bar->addAction(gidrAction.aZapVnCx);
    bar->addAction(gidrAction.aZapNullGeo);
//    bar->addAction(gidrAction.aZapOrp);
    bar->addAction(gidrAction.aVyd);
    bar->addAction(gidrAction.aZapNezak);
    bar->addAction(gidrAction.aZap1);
    bar->addAction(gidrAction.aZap2);
    bar->addAction(gidrAction.aZap6);
    bar->addAction(gidrAction.aZap7);
    bar->addAction(gidrAction.aZap71);
    bar->addAction(gidrAction.aTimePr);
    bar->addAction(gidrAction.aPoteri);
    bar->addAction(gidrAction.aZap3);
    bar->addAction(gidrAction.aZap4);
    bar->addAction(gidrAction.aZap5);

    m_barZapr = bar;

  bar = main_window->addToolBar(tr("Нарушения"));
  bar->setObjectName(tr("Нарушения"));

      bar->addAction(gidrAction.aRemontPovrDefAdd); // Установить нарушение
      bar->addAction(gidrAction.aRemontPovrDefDel); // Удалить нарушение
      bar->addAction(gidrAction.aRemontPovrDefMove); // Перенести нарушение
      bar->addAction(gidrAction.aRemontPovrDefInfo); // Свойства объекта Нарушение
      bar->addSeparator();
      bar->addAction(gidrAction.aDefectZhurnalExpluatacia); // Нарушения эксплуатации
      bar->addAction(gidrAction.aDefectZhurnalShurf); // Нарушения в шурфе
      bar->addAction(gidrAction.aDefectZhurnalOsmotr); // Нарушения осмотра
      bar->addAction(gidrAction.aDefectZhurnalOpress); // Нарушения опрессовки
      bar->addSeparator();
      bar->addAction(gidrAction.aDefectZhurnalNarushenie); // Нарушения до ремонта
      bar->addAction(gidrAction.aDefectZhurnalRemont); // Нарушения в процессе ремонта
      bar->addAction(gidrAction.aDefectZhurnalPosleRemont); // Нарушения после ремонта
      bar->addAction(gidrAction.aDefectZhurnalAll); // Отобразить все нарушения
      bar->addSeparator();
      bar->addAction(gidrAction.aDefectWord1); // Журнал нарушений
      bar->addAction(gidrAction.aDefectWord2); // Нарушения/ремонты трубопроводов тепловой сети(документ)
      bar->addSeparator();
      bar->addAction(gidrAction.aDefectDocTable); // Нарушения/ремонты трубопроводов тепловой сети
      bar->addAction(gidrAction.aDefectAnaliz); // Анализ нарушений
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontPovrDefOtobr); // Включить/отключить отображение объектов
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontPovrOtop); // Выбор отопительного сезона
      bar->addAction(gidrAction.aRemontOsmotrTeploSet); // Выбор фрагмента сети по Начальнику участка
//      bar->addSeparator();
//      bar->addAction(gidrAction.aDefectBezUch); // Нарушения без участков
    m_barDefect = bar;

  
  bar = main_window->addToolBar(tr("Шурфовка"));
  bar->setObjectName(tr("Шурфовка"));
      bar->addAction(gidrAction.aRemontPovrShurfAdd); // Установить плановый шурф
      bar->addAction(gidrAction.aRemontPovrShurfAddNeplan); // Установить НЕплановый шурф
      bar->addAction(gidrAction.aRemontPovrShurfDel); // Удалить шурф
      bar->addAction(gidrAction.aRemontPovrShurfMove); // Перенести шурф
      bar->addAction(gidrAction.aRemontPovrShurfInfo); // Свойства объекта Шурф
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontShurfPlan); // Плановые шурфы
      bar->addAction(gidrAction.aRemontShurfPredpis); // Шурфы предписания
      bar->addAction(gidrAction.aRemontShurfNarush); // Шурфы нарушения
      bar->addSeparator();
      bar->addAction(gidrAction.aShurfProcess); // Шурфы в процессе выполнения
      bar->addAction(gidrAction.aShurfVypolneno); // Шурфы выполненные
      bar->addAction(gidrAction.aShurfAll); // Отобразить все Шурфы
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontShurfPlanUtverdit); // Утвердить План шурфов Отопительного сезона
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontShurfPlanExcel); // План шурфов (документ)
      bar->addAction(gidrAction.aRemontShurfPlanExcelMonth); // План шурфов по месяцам (документ)
      bar->addAction(gidrAction.aShurfTable); // Анализ Плана шурфов
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontShurfOtobr); // Включить/отключить отображение объектов
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontPovrOtop); // Выбор отопительного сезона
      bar->addAction(gidrAction.aRemontOsmotrTeploSet); // Выбор фрагмента сети по Начальнику участка
      bar->addSeparator();
      bar->addAction(gidrAction.aShurfBezUch); // Шурфовки без участков

    m_barShurf = bar;

  bar = main_window->addToolBar(tr("Осмотр"));
  bar->setObjectName(tr("Осмотр"));
      bar->addAction(gidrAction.aOsmotrAdd); // Создать контур осмотра
      bar->addAction(gidrAction.aOsmotrSave); // Обновить сохранение контура
      bar->addAction(gidrAction.aListOsmotr2); // Выполнить осмотр контура
      bar->addAction(gidrAction.aLastOsmotr2); // Последний контур осмотра
      bar->addAction(gidrAction.aLastFaktory); // Выбор трубопроводов для осмотра
      bar->addAction(gidrAction.aOsmotrZapros1); // Осмотренные фрагменты сети
      bar->addAction(gidrAction.aOsmotrZapros2); // Осмотренные трубопроводы за период
      bar->addAction(gidrAction.aOsmotrZapros3); // Неосмотренные трубопроводы за период
      bar->addSeparator();
      bar->addAction(gidrAction.aOsmotrAnaliz2); // Анализ осмотра контуров
      bar->addAction(gidrAction.aOsmotrAnaliz); // Анализ осмотра трубопроводов контура
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontPovrDefOtobr); // Включить/отключить отображение объектов
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontPovrOtop); // Выбор отопительного сезона
      bar->addAction(gidrAction.aRemontOsmotrTeploSet); // Выбор фрагмента сети по Начальнику участка

      m_barOsmotr = bar;

  bar = main_window->addToolBar(tr("Ремонт"));
  bar->setObjectName(tr("Ремонт"));
      bar->addAction(gidrAction.aRemontAddPlan); // Сохранить контур плана ремонта
      bar->addAction(gidrAction.aRemontAddCurrent); // Сохранить контур текущего ремонта
      bar->addAction(gidrAction.aRemontSave); // Обновить сохранение контура
      bar->addSeparator();
      bar->addAction(gidrAction.aListRemont2); // Список контуров ремонтов
      bar->addAction(gidrAction.aLastRemont2); // Последний контур ремонта
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontTrub); // Выбор трубопроводов контура ремонта
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontPlan); // Контуры капитального/инвестиционного ремонтов
      bar->addAction(gidrAction.aRemontCurrent); // Контуры текущих ремонтов
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontProcess); // Ремонт в процессе выполнения
      bar->addAction(gidrAction.aRemontVypolneno); // Законченные ремонты
      bar->addAction(gidrAction.aRemontAll2); // Отобразить все контура
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontWord1); // График выполнения ремонтов (документ)
      bar->addAction(gidrAction.aRemontWord2); // План капитальных/инвестиционных ремонтов (документ)
      bar->addAction(gidrAction.aRemontWord3); // План ремонтов по месяцам (документ)
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontAnaliz2); // Анализ контуров ремонта
      bar->addAction(gidrAction.aRemontAnaliz); // График выполнения ремонтов
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontPovrDefOtobr); // Включить/отключить отображение объектов
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontPovrOtop); // Выбор отопительного сезона
      bar->addAction(gidrAction.aRemontOsmotrTeploSet); // Выбор фрагмента сети по Начальнику участка
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontVyborNach); // Создать контур ремонта
      bar->addAction(gidrAction.aMarkRev); // Добавить/удалить трубопровод к контуру

    m_barRemont = bar;
  
  bar = main_window->addToolBar(tr("Опрессовка"));
  bar->setObjectName(tr("Опрессовка"));
      bar->addAction(gidrAction.aOpresAddPlan); // Сохранить контур плана опрессовки
      bar->addAction(gidrAction.aOpresSave); // Обновить сохранение контура
      bar->addSeparator();
      bar->addAction(gidrAction.aOpresGranitsaRazdela); // Границы раздела
      bar->addAction(gidrAction.aListOpres); // Список контуров опрессовок
      bar->addAction(gidrAction.aLastOpres); // Последний контур опрессовки
      bar->addSeparator();
      bar->addAction(gidrAction.aOpresPlan); // Контуры планов опрессовок сезона
      bar->addSeparator();
      bar->addAction(gidrAction.aOpresProcess); // Опрессовка в процессе выполнения
      bar->addAction(gidrAction.aOpresVypolneno); // Завершенные опрессовки
      bar->addAction(gidrAction.aOpresAll2); // Отобразить все контура
      bar->addSeparator();
      bar->addAction(gidrAction.aOpresWord1); // План опрессовок (документ)
      bar->addAction(gidrAction.aOpresWord2); // График опрессовок по месяцам (документ)
      bar->addSeparator();
      bar->addAction(gidrAction.aOpresAnaliz2); // Анализ контуров опрессовок
      bar->addAction(gidrAction.aOpresAnaliz); // Анализ нарушений контура
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontPovrDefOtobr); // Включить/отключить отображение объектов
      bar->addSeparator();
      bar->addAction(gidrAction.aRemontPovrOtop); // Выбор отопительного сезона
      bar->addAction(gidrAction.aRemontOsmotrTeploSet); // Выбор фрагмента сети по Начальнику участка
      bar->addSeparator();
      bar->addAction(gidrAction.aOpresVyborNach); // Создать контур опрессовки
      bar->addAction(gidrAction.aListRemont2); // Список контуров ремонтов
      bar->addAction(gidrAction.aMarkRev); // Добавить/удалить трубопровод к контуру

    m_barOpres = bar;


  bar = main_window->addToolBar(tr("Паспортизация"));
  bar->setObjectName(tr("Паспортизация"));
//      bar->addAction(gidrAction.); 

      bar->addAction(gidrAction.aPtsAdd);
      bar->addAction(gidrAction.aPtsDel);
      bar->addAction(gidrAction.aPtsInfo);
      bar->addSeparator();
      bar->addAction(gidrAction.aPtsTable);
      bar->addSeparator();
      bar->addAction(gidrAction.aSetMsNew);
      bar->addAction(gidrAction.aSetRsNew);
      bar->addSeparator();
      bar->addAction(gidrAction.aEditFindPs);
      bar->addSeparator();
      bar->addAction(gidrAction.aRasprMagNew);
      bar->addSeparator();
      bar->addAction(gidrAction.aMagNode);
      bar->addSeparator();
      bar->addAction(gidrAction.aNapr2);
      bar->addAction(gidrAction.aVyd4);
      bar->addAction(gidrAction.aNapr2);
      bar->addAction(gidrAction.aMark);
      bar->addAction(gidrAction.aMarkDel);
      bar->addAction(gidrAction.aMarkRev);
      bar->addSeparator();
      bar->addAction(gidrAction.aMsNode1);
      bar->addAction(gidrAction.aMsNode2);
      bar->addSeparator();
      bar->addAction(gidrAction.aSaveMAG);
      bar->addAction(gidrAction.aSaveMS);
      bar->addAction(gidrAction.aSaveRS);
      bar->addSeparator();
      bar->addAction(gidrAction.aMsPassport);
      bar->addSeparator();
      bar->addAction(gidrAction.aAktCreate);
      bar->addAction(gidrAction.aPassportOnOff);
      bar->addSeparator();
      bar->addAction(gidrAction.aPtsZhurnal);

    m_barPts = bar;

  bar = main_window->addToolBar(tr("Технические условия"));
  bar->setObjectName(tr("Технические условия"));

      bar->addAction(gidrAction.aTuNew    );
      bar->addAction(gidrAction.aTuLine   );
      bar->addSeparator();
      bar->addAction(gidrAction.aTuInfo   );
      bar->addAction(gidrAction.aTuDel    );
      bar->addSeparator();
      bar->addAction(gidrAction.aTuFind   );
      bar->addAction(gidrAction.aTuSost   );
      bar->addSeparator();
      bar->addAction(gidrAction.aTuExcel  );
      bar->addAction(gidrAction.aTuSvod   );
      bar->addAction(gidrAction.aTuZhurnal);

    m_barTu = bar;

  bar = main_window->addToolBar(tr("Индикаторы коррозии"));
  bar->setObjectName(tr("Индикаторы коррозии"));
    
      bar->addAction(gidrAction.aKorrozAdd);
      bar->addAction(gidrAction.aKorrozDel);
      bar->addAction(gidrAction.aFragmentPolyKorroziaAr);
      bar->addAction(gidrAction.aKorrozInfo);
      bar->addSeparator();
      bar->addAction(gidrAction.aKorroziaUpdate);
      bar->addAction(gidrAction.aKorrozDoc3);
      bar->addSeparator();
      bar->addAction(gidrAction.aKorrozTable);
      bar->addSeparator();
      bar->addAction(gidrAction.aKorroziaZhurnal1);
      bar->addAction(gidrAction.aKorroziaZhurnal2);
      bar->addAction(gidrAction.aKorroziaZhurnal3);
      bar->addAction(gidrAction.aKorroziaCurrent);
      bar->addSeparator();
      bar->addAction(gidrAction.aKorrozDoc2);
      bar->addAction(gidrAction.aKorrozDoc1);
      bar->addAction(gidrAction.aKorroziaOnoff);
      bar->addAction(gidrAction.aKorroziaSezon);
    
    m_barKorrozia = bar;

    
  bar = main_window->addToolBar(tr("Электричество"));
  bar->setObjectName(tr("Электричество"));

      bar->addAction(gidrAction.aElectroIst);
      bar->addAction(gidrAction.aElectroPriem);
      bar->addAction(gidrAction.aElectroLine);
      bar->addAction(gidrAction.aElectroMufta);
      bar->addAction(gidrAction.aElectroOpora);
      bar->addAction(gidrAction.aElectroGil);
      bar->addAction(gidrAction.aElectroKanal);
      bar->addAction(gidrAction.aElectroTable);
      bar->addAction(gidrAction.aZhurnalElectro);
      bar->addAction(gidrAction.aElectroZhurnal);
      bar->addAction(gidrAction.aElectroOnoff);

    m_barElectro = bar;

    map_toolbar[gidrAction.aViewToolbarElectro] = m_barElectro;
    map_toolbar[gidrAction.aTuOnOff] = m_barTu;
    map_toolbar[gidrAction.aViewToolbarKorrozia] = m_barKorrozia;

    map_toolbar[gidrAction.aViewToolbar2PtsNew] = m_barPts;
    map_toolbar[gidrAction.aViewToolbarMain] = m_barGid;
    map_toolbar[gidrAction.aViewToolbarCxema] = m_barCxema;
    map_toolbar[gidrAction.aViewToolbarZ] = m_barZapr;
    map_toolbar[gidrAction.aToolbarRemont2Defect] = m_barDefect;
    map_toolbar[gidrAction.aToolbarRemont2Shurf] = m_barShurf;
    map_toolbar[gidrAction.aToolbarRemont2Osmotr] = m_barOsmotr;
    map_toolbar[gidrAction.aToolbarRemont2Remont] = m_barRemont;
    map_toolbar[gidrAction.aToolbarRemont2Opres] = m_barOpres;

    for (auto &it : map_toolbar) {
        QAction *a = it.first;
        QToolBar *bar = it.second;
        a->setChecked(bar->isVisible());
    }
}
