#include <QApplication>
#include <QtGui>
#include <QtSql>
#include <QtWidgets>
//#include <QtPrinter>
#include <cmath>

#include "std.h"

#include "mainwindow.h"
#include <gidview/GidWidget.h>


void GidWidget::createMenu()
{
    QMenu *menu1;
    QMenu *menu2;
    QMenu *menu3;
    QMenu *menu4;
    QMenuBar *menu = main_window->menuBar();

    menu->setWindowIcon(QIcon());

    menu->clear();

    menu1 = menu->addMenu(tr("Файл"));
        menu1->setToolTipsVisible(true);

        menu1->addAction(main_window->mainAction.aFileNew); // Подключить
        menu1->addAction(main_window->mainAction.aFileSqlite); // Подключить

        menu1->addAction(gidrAction.aFileClose); // Закрыть
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Режим"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aEditor1); // Режимы
            menu2->addAction(gidrAction.aEditor2); // Просмотр схемы
            menu2->addAction(gidrAction.aEditor4); // АРМ
        menu1->addAction(gidrAction.aSqlSave); // Создание копии Базы данных

        menu2 = menu1->addMenu(tr("Работа с фрагментами"));
//            menu2->addAction(gidrAction.aSqlCopyFragment); // Вставить Фрагмент схемы
            menu2->addAction(gidrAction.aSqlDelFragment); // Удалить Фрагмент схемы
            menu2->addAction(gidrAction.aAddFragment); // Добавить Фрагмент схемы
            menu2->addAction(gidrAction.aExportFragment); // Экспорт фрагмента
            menu2->addAction(gidrAction.aExportFragmentVyd); // Экспорт выделенного фрагмента
//            menu2->addAction(gidrAction.aExportFragmentsMulty); // Экспорт всех фрагментов
            menu2->addAction(gidrAction.aImportFragment); // Импорт фрагмента

        menu1->addAction(gidrAction.aImportFragmentMdb); // Конвертор ТГИД-05/ ТГИД-07
        menu1->addSeparator();
        menu1->addAction(gidrAction.aMainCxema); // Активная схема
        menu1->addAction(gidrAction.aAddDop); // Отобразить дополнительную схему
        menu1->addSeparator();
        menu1->addAction(gidrAction.aBmp); // Файл графической подложки...
        menu1->addAction(gidrAction.aBmpFind); // Найти графическую подложку
        menu1->addSeparator();
        menu1->addAction(gidrAction.aCatalog); // Каталоги
        menu1->addSeparator();
        menu1->addAction(gidrAction.aFilePrint); // Печать...\tCtrl+P
        menu1->addAction(gidrAction.aFilePrintPreview); // Предварительный просмотр
        menu1->addAction(gidrAction.aPrintFr); // Фрагмент для печати
        menu1->addAction(gidrAction.aFragment); // Выделить фрагмент
        menu1->addSeparator();
//        menu1->addAction(gidrAction.aBdDel); // Удалить базу
//        menu1->addSeparator();

//        menu1->addAction(gidrAction.aFilePrintSetup); // Параметры страницы...
        menu1->addAction(main_window->mainAction.aAppExit); // Выход
    menu1 = menu->addMenu(tr("Правка"));
        menu1->setToolTipsVisible(true);

        menu1->addAction(gidrAction.aResetGid); // Перезагрузить схему
        menu1->addAction(gidrAction.aToGid9);
        
        menu1->addSeparator();


        menu1->addAction(gidrAction.aEditFindNode); // Найти узел...\tCtrl+F
        menu1->addSeparator();
        menu1->addAction(gidrAction.aEditUndo); // Отменить\tCtrl+Z
//        menu1->addAction(gidrAction.aEditUndoDel); // Отменить удаления
        menu1->addSeparator();
        menu1->addAction(gidrAction.aZapVnCx); // Узлы с внутренней схемой
        menu1->addAction(gidrAction.aZapNullGeo); // Узлы с нулевой геодезической отметкой
        menu1->addAction(gidrAction.aOdnolin); // Однолинейные участки
        menu1->addAction(gidrAction.aRaznolin); // Участки с разными подачей и обраткой
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Выделить"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aRasprMag); // Расчетные схемы
            menu2->addAction(gidrAction.aRasprMag2); // Объекты системы теплоснабжения
            menu2->addAction(gidrAction.aIstSety); // Источник
            menu2->addAction(gidrAction.aIstSetyNew); // Источник
            menu2->addAction(gidrAction.aVyd); // Фрагмент сети
            menu2->addAction(gidrAction.aBalans); // Балансовая принадлежность
            menu3 = menu2->addMenu(tr("По виду прокладки"));
                menu3->setToolTipsVisible(true);
                menu3->addAction(gidrAction.aTubing1); // Канальная
                menu3->addAction(gidrAction.aTubing2); // Бесканальная
                menu3->addAction(gidrAction.aTubing4); // Надземная
                menu3->addAction(gidrAction.aTubing3); // Подвальная
                menu3->addAction(gidrAction.aTubing5); // Обвязка узлов и насосных станций
            menu2->addAction(gidrAction.aFindKti); // По коэффициенту тепловых испытаний
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Область"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aMark); // Выделить область
            menu2->addSeparator();
            menu3 = menu2->addMenu(tr("Потребители"));
                menu3->setToolTipsVisible(true);
                menu3->addAction(gidrAction.aSetOtv); // Установить ФИО техников
                menu3->addAction(gidrAction.aSetTr); // Установить Код расчетных температур
                menu3->addAction(gidrAction.aSetUr); // Установить Код удельных расходов
                menu3->addAction(gidrAction.aSetKvPt); // Установить Коэффициенты вариации по потребителям
                menu3->addAction(gidrAction.aSetUf); // Установить Коэффициенты смешения элеватора
                menu3->addAction(gidrAction.aSetTp); // Установить Тепловые пункты по потребителям
                menu3->addAction(gidrAction.aSetUdobVent); // Установить Удельный объем системы вентиляции
                menu3->addAction(gidrAction.aSetUdobOt); // Установить Удельный объем системы отопления
                menu4 = menu3->addMenu(tr("Установить параметры для открытой ГВС"));
                    menu4->setToolTipsVisible(true);
                    menu4->addAction(gidrAction.aSetOpenKoef); // Коэф. часовой неравномерности
                    menu4->addAction(gidrAction.aSetOpenRez); // Расчетные тепловые потери в рециркуляц. контуре ГВС
                    menu4->addAction(gidrAction.aSetOpenRezT); // Температура в рециркуляционном трубопроводе ГВС
                    menu4->addAction(gidrAction.aSetOpenGvsT); // Расчетная температура горячей воды
            menu3 = menu2->addMenu(tr("Участки теплопроводов"));
                menu3->setToolTipsVisible(true);
                menu3->addAction(gidrAction.aSetDiams); // Установить Диаметр
                menu3->addAction(gidrAction.aSetLosesShare); // Установить Долю местных потерь
                menu3->addAction(gidrAction.aSetKolChas); // Установить Количество часов работы
                menu3->addAction(gidrAction.aSetKvUt); // Установить Коэффициенты вариации по участкам
                menu3->addAction(gidrAction.aSetKti); // Установить Коэффициенты тепловых испытаний
                menu3->addAction(gidrAction.aSetOrg); // Установить Организации
                menu3->addAction(gidrAction.aSetPipeRemontType); // Установить Признак ремонта
                menu3->addAction(gidrAction.aSetTubingType); // Установить Тип прокладки
                menu3->addAction(gidrAction.aSetSher); // Установить Эквивалентную шероховатость
                menu3->addSeparator();
                menu3->addAction(gidrAction.aSetDate2); // Установить Дату первичного ввода в эксплуатацию
                menu3->addAction(gidrAction.aSetDate1); // Установить Дату последней перекладки
                menu3->addAction(gidrAction.aSetDate3); // Установить Дату планируемого ремонта
            menu2->addAction(gidrAction.aSetKodRs); // Установить Код расчетной схемы
            menu2->addAction(gidrAction.aGeodz); // Установить Отметки узлов горизонтали
            menu2->addAction(gidrAction.aSetKorrozia); // Установить Индикаторы коррозии
            menu2->addSeparator();
            menu2->addAction(gidrAction.aSetPodpOn); // Показать Надписи
            menu2->addAction(gidrAction.aSetPodpOff); // Не отображать Надписи
        menu1->addSeparator();
        menu1->addAction(gidrAction.aFragmentPoly); // Выделить многоугольный фрагмент
        menu1->addAction(gidrAction.aMoveVyd); // Передвинуть фрагмент
        menu1->addSeparator();
        menu1->addAction(gidrAction.aFragment); // Выделить фрагмент
        menu1->addAction(gidrAction.aEditCopy); // Копировать\tCtrl+C
        menu1->addAction(gidrAction.aEditCopyPng); // Копировать в PNG
    menu1 = menu->addMenu(tr("Вид"));
        menu1->setToolTipsVisible(true);
        menu2 = menu1->addMenu(tr("Панель инструментов"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aViewToolbarMain); // Главная
            menu2->addAction(gidrAction.aViewToolbarCxema); // Схема
            menu2->addAction(gidrAction.aViewToolbarZ); // Запросы
            menu2->addSeparator();
            menu2->addAction(gidrAction.aViewToolbar2PtsNew); // Паспортизация
            menu2->addAction(gidrAction.aTuOnOff); // Технические условия
            menu2->addAction(gidrAction.aViewToolbarElectro); // Электрические сети
            menu2->addSeparator();
//            menu2->addAction(gidrAction.aViewToolbarControlTu); // Контроль технического состояния
//            menu2->addAction(gidrAction.aViewToolbar2Remont); // Ремонт
//            menu2->addAction(gidrAction.aViewToolbarOpressovka); // Опрессовки
            menu2->addAction(gidrAction.aViewToolbarKorrozia); // Индикаторы коррозии

            menu2->addSeparator();
            menu2->addAction(gidrAction.aToolbarRemont2Defect); // Панель Нарушений
            menu2->addAction(gidrAction.aToolbarRemont2Shurf); // Панель Шурфовки
            menu2->addAction(gidrAction.aToolbarRemont2Osmotr); // Панель Осмотра
            menu2->addAction(gidrAction.aToolbarRemont2Remont); // Панель Ремонтов
            menu2->addAction(gidrAction.aToolbarRemont2Opres); // Панель Опрессовки


//        menu1->addAction(gidrAction.aNavigator); // Навигатор
        menu1->addAction(gidrAction.aLeftBar); // Проводник карты
        menu1->addAction(gidrAction.aMagAlma); // Проводник структуры сети
        menu1->addAction(gidrAction.aBottomRemont); // Проводник мониторинга
//        menu1->addAction(gidrAction.aProtocol); // Протокол
        menu1->addSeparator();
        menu1->addAction(gidrAction.aMovew); // Перемещение\tM
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Поворот"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aPovorot); // Повернуть
            menu2->addAction(gidrAction.aPovorot2); // Возврат
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Масштаб"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aMasall); // Отобразить всю схему\tF3
            menu2->addAction(gidrAction.aPlus); // Увеличить\t+
            menu2->addAction(gidrAction.aMinus); // Уменьшить\t-
            menu2->addAction(gidrAction.aMasshtab); // Масштаб...\tCtrl+F3
        menu1->addAction(gidrAction.aZoom); // Масштабировать
        menu1->addSeparator();
        menu1->addAction(gidrAction.aPicture); // Графическая подложка
        menu1->addAction(gidrAction.aF5); // Перерисовать схему\tF5
        menu1->addAction(gidrAction.aCtrlF5); // Перерисовать схему\tF5
    menu1 = menu->addMenu(tr("Геобаза"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aGeobaza); // Показать геобазу
        menu1->addAction(gidrAction.aFindGeo); // Поиск в геобазе...
        menu1->addAction(gidrAction.aGeoFindNext); // Продолжение поиска
        menu1->addAction(gidrAction.aFindGeoAddr); // Поиск по адресу
        menu1->addAction(gidrAction.aGeoInfo); // Информация из геобазы
        menu1->addSeparator();
        menu1->addAction(gidrAction.aSetLength); // Установить длины
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Переименование полей"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.a1000); // Создать файл полей
            menu2->addAction(gidrAction.a1001); // Переименовать поля
            menu2->addAction(gidrAction.a1002); // Сохранить переименованные поля
    menu1 = menu->addMenu(tr("Схема"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aNode); // Установить узел\tN
        menu1->addAction(gidrAction.aLine); // Линейный объект\tL
        menu1->addAction(gidrAction.aRename); // Изменить тип объекта
        menu1->addAction(gidrAction.aZn); // Узел с заданным напором
        menu1->addSeparator();
        menu1->addAction(gidrAction.aKam); // Внутренняя схема
        menu1->addSeparator();
        menu1->addAction(gidrAction.aNodeNeotr); // Установка конвертированных узлов\tC
        menu1->addAction(gidrAction.aZapNezak); // Незаконченные узлы
        menu1->addAction(gidrAction.aSetCoordNull); // Удалить координаты выделенных узлов
        menu1->addSeparator();
        menu1->addAction(gidrAction.aNodeMove); // Перенести\tV
        menu1->addAction(gidrAction.aDel); // Удалить объект\tD
        menu1->addAction(gidrAction.aOpenClose); // Закрыть/открыть объект
        menu1->addSeparator();
        menu1->addAction(gidrAction.aNodeMove1); // Разъединить линии
        menu1->addAction(gidrAction.aSwap); // Изменить направление потока
        menu1->addAction(gidrAction.aMoveLineEnd); // Перенести линию
        menu1->addSeparator();
        menu1->addAction(gidrAction.aText); // Установить текст
    menu1 = menu->addMenu(tr("Процессор режимов"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aDoItDr); // Плановый...
        menu1->addAction(gidrAction.aDoIt); // Фактический...
        menu1->addSeparator();
        menu1->addAction(gidrAction.aDoItListDr); // Плановый по списку...
        menu1->addAction(gidrAction.aDoItList); // Фактический по списку...
        menu1->addSeparator();
        menu1->addAction(gidrAction.aMagFragment); // Магистральный фрагмент
        menu1->addSeparator();
        menu1->addAction(gidrAction.aNapr); // Выбpать напpавление
        menu1->addAction(gidrAction.aSavePjezo); // Сохранить направление...
        menu1->addAction(gidrAction.aListPjezo); // Список направлений...
        menu1->addSeparator();
        menu1->addAction(gidrAction.aClearOut); // Удалить старые расчеты
        menu1->addSeparator();
        menu1->addAction(gidrAction.aProtocol); // Протокол
    menu1 = menu->addMenu(tr("Информация"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aInfo); // Свойства объекта\tI
        menu1->addAction(gidrAction.aViewDb); // Таблицы...
        menu1->addAction(gidrAction.aNovisual); // Системные параметры...
        menu1->addAction(gidrAction.aMoveTo); // Таблица объекта
        menu1->addSeparator();
        menu1->addAction(gidrAction.aInfoGid); // Результат расчета
        menu1->addAction(gidrAction.aOut2); // Отчеты...
        menu1->addAction(gidrAction.aTableDan); // Редактор таблиц...
        menu1->addSeparator();
        menu1->addAction(gidrAction.aExcel2); // Таблицы Excel...
        menu1->addSeparator();
        menu1->addAction(gidrAction.aInf); // Отображаемая информация...
        menu1->addAction(gidrAction.aGidinf); // Отображать информацию
        menu1->addAction(gidrAction.aPodp); // Отображать имена узлов
        menu1->addAction(gidrAction.aPodpPo); // Отображать схему ПТС
        menu1->addAction(gidrAction.aPodpRes); // Отображать имена узлов с #
        menu1->addAction(gidrAction.aPribor); // Отображать Приборы учета
        menu1->addAction(gidrAction.aRezhim); // Отображать вспомогательные участки
        menu1->addSeparator();
        menu1->addAction(gidrAction.aPopupPodpOn); // Показать скрытые имена
        menu1->addAction(gidrAction.aPopupPodpOff); // Не показывать скрытые имена
        menu1->addSeparator();
        menu1->addAction(gidrAction.aPodpYesNo); // Отображать/Не отображать надпись
        menu1->addSeparator();
        menu1->addAction(gidrAction.aNaprGid); // Распределение потоков
        menu1->addSeparator();
        menu1->addAction(gidrAction.aF4); // Измерить расстояние\tF4
    menu1 = menu->addMenu(tr("Анализ"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aAnal); // Анализ...
        menu2 = menu1->addMenu(tr("Анализ-выделение"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aColorLine); // Участки
            menu2->addAction(gidrAction.aColorLine1); // Потребители
        menu2 = menu1->addMenu(tr("Запросы"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aZap1); // Объем сети
            menu2->addAction(gidrAction.aZap2); // Длина теплопроводов
            menu2->addAction(gidrAction.aZap7); // Длина теплопроводов по диаметрам
            menu2->addAction(gidrAction.aZap71); // Длина теплопроводов по диаметрам и способам прокладки
            menu2->addAction(gidrAction.aTimePr); // Время прохождения
            menu2->addAction(gidrAction.aPoteri); // Тепловые потери
            menu2->addAction(gidrAction.aZap3); // Теплопотребление полученное
            menu2->addAction(gidrAction.aZap4); // Теплопотребление в закрытых системах полученное
            menu2->addAction(gidrAction.aZap5); // Теплопотребление в открытых системах полученное
        menu2 = menu1->addMenu(tr("Отключенные потребители"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aZap6); // Закрытые потребители
            menu2->addAction(gidrAction.aPotNagr0); // С нулевой нагрузкой
            menu2->addAction(gidrAction.aPotrOtkl); // Отключенные потребители
        menu2 = menu1->addMenu(tr("Отключенные участки"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aUtZakr); // Закрытые участки
            menu2->addAction(gidrAction.aUtZakrAll); // Отключенные участки
        menu1->addAction(gidrAction.aZapOtr); // Отрицательные перепады
        menu1->addAction(gidrAction.aPtTempMin); // Низкие температуры
        menu1->addAction(gidrAction.aZavozd); // Завоздушивание
        menu1->addAction(gidrAction.aIsprLast); // Изменённые объекты
        menu1->addSeparator();
        menu1->addAction(gidrAction.aPjezo); // Пьезометр
        menu1->addAction(gidrAction.aZona); // Гидростатические зоны
    menu1 = menu->addMenu(tr("ТУ"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aTuOnOff); // Объекты ТУ
        menu2 = menu1->addMenu(tr("Договорные нагрузки (ДН)"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aNagrZd); // Физические лица
            menu2->addAction(gidrAction.aNagrOrg); // Юридические лица
        menu1->addSeparator();
        menu1->addAction(gidrAction.aTuTable); // Технические условия
        menu1->addAction(gidrAction.aTuYear); // ТУ по годам
        menu1->addAction(gidrAction.aTuSost); // ТУ состояние
        menu1->addSeparator();
        menu1->addAction(gidrAction.aTuExcel); // Итоговый тепловой баланс
        menu1->addAction(gidrAction.aTuSvod); // Итоговые по годам
        menu1->addAction(gidrAction.aTuZhurnal); // Журнал регистрации ТУ
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Поиск"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aFindTuIst); // ТУ по источнику
            menu2->addAction(gidrAction.aTuFind); // ТУ, Договора
        menu1->addAction(gidrAction.aPrisNagrEdit); // Присоединенная нагрузка по источникам
        menu1->addAction(gidrAction.aQGvs); // Коэффициент перевода k=Qгвmax/Qгвср
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Ненайденные ДН на карте"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aNagrZdNeiz); // Физические лица
            menu2->addAction(gidrAction.aNagrOrgNeiz); // Юридические лица
        menu1->addAction(gidrAction.aTuTableNeiz); // Ненайденные ТУ на карте
        menu1->addAction(gidrAction.aTuIst); // Отчет по источникам
    menu1 = menu->addMenu(tr("Паспорта"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aViewToolbar2PtsNew); // Панель Объекты ПТС
        menu1->addAction(gidrAction.aPtsAdd); // Установить объект
        menu1->addAction(gidrAction.aPtsInfo); // Информация по объекту
        menu1->addAction(gidrAction.aPtsTable); // Таблица
        menu1->addAction(gidrAction.aPtsDel); // Удалить объект
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Протяженность"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aProtPass); // Протяженность
            menu2->addAction(gidrAction.aProtNew); // Протяженность выделенных трубопроводов
            menu2->addAction(gidrAction.aProtMag); // Протяженность магистралей
            menu2->addAction(gidrAction.aProtRs); // Протяженность распредсетей
            menu2->addAction(gidrAction.aProtKot); // Протяженность котельных
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Паспорта"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aPaspNew); // Паспорт сети


    menu1 = menu->addMenu(tr("Ремонт"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aToolbarRemont2Defect); // Панель Нарушений
        menu1->addAction(gidrAction.aToolbarRemont2Shurf); // Панель Шурфовки
        menu1->addAction(gidrAction.aToolbarRemont2Osmotr); // Панель Осмотра
        menu1->addAction(gidrAction.aToolbarRemont2Remont); // Панель Ремонтов
        menu1->addAction(gidrAction.aToolbarRemont2Opres); // Панель Опрессовки
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Нарушения"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aRemontPovrDefAdd); // Установить нарушение
            menu2->addAction(gidrAction.aRemontPovrDefDel); // Удалить нарушение
            menu2->addAction(gidrAction.aRemontPovrDefMove); // Перенести нарушение
            menu2->addAction(gidrAction.aRemontPovrDefInfo); // Свойства объекта Нарушение
            menu2->addSeparator();
            menu2->addAction(gidrAction.aDefectZhurnalExpluatacia); // Нарушения эксплуатации
            menu2->addAction(gidrAction.aDefectZhurnalShurf); // Нарушения в шурфе
            menu2->addAction(gidrAction.aDefectZhurnalOsmotr); // Нарушения осмотра
            menu2->addAction(gidrAction.aDefectZhurnalOpress); // Нарушения опрессовки
            menu2->addSeparator();
            menu2->addAction(gidrAction.aDefectZhurnalNarushenie); // Нарушения до ремонта
            menu2->addAction(gidrAction.aDefectZhurnalRemont); // Нарушения в процессе ремонта
            menu2->addAction(gidrAction.aDefectZhurnalPosleRemont); // Нарушения после ремонта
            menu2->addAction(gidrAction.aDefectZhurnalAll); // Отобразить все нарушения
            menu2->addSeparator();
            menu2->addAction(gidrAction.aDefectWord1); // Журнал нарушений
            menu2->addAction(gidrAction.aDefectWord2); // Нарушения/ремонты трубопроводов тепловой сети(документ)
            menu2->addSeparator();
            menu2->addAction(gidrAction.aDefectDocTable); // Нарушения/ремонты трубопроводов тепловой сети
            menu2->addAction(gidrAction.aDefectAnaliz); // Анализ нарушений
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontPovrDefOtobr); // Включить/отключить отображение объектов
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontPovrOtop); // Выбор отопительного сезона
            menu2->addAction(gidrAction.aRemontOsmotrTeploSet); // Выбор фрагмента сети по Начальнику участка
            menu2->addSeparator();
            menu2->addAction(gidrAction.aDefectBezUch); // Нарушения без участков
        menu2 = menu1->addMenu(tr("Шурфовка"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aRemontPovrShurfAdd); // Установить плановый шурф
            menu2->addAction(gidrAction.aRemontPovrShurfAddNeplan); // Установить НЕплановый шурф
            menu2->addAction(gidrAction.aRemontPovrShurfDel); // Удалить шурф
            menu2->addAction(gidrAction.aRemontPovrShurfMove); // Перенести шурф
            menu2->addAction(gidrAction.aRemontPovrShurfInfo); // Свойства объекта Шурф
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontShurfPlan); // Плановые шурфы
            menu2->addAction(gidrAction.aRemontShurfPredpis); // Шурфы предписания
            menu2->addAction(gidrAction.aRemontShurfNarush); // Шурфы нарушения
            menu2->addSeparator();
            menu2->addAction(gidrAction.aShurfProcess); // Шурфы в процессе выполнения
            menu2->addAction(gidrAction.aShurfVypolneno); // Шурфы выполненные
            menu2->addAction(gidrAction.aShurfAll); // Отобразить все Шурфы
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontShurfPlanUtverdit); // Утвердить План шурфов Отопительного сезона
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontShurfPlanExcel); // План шурфов (документ)
            menu2->addAction(gidrAction.aRemontShurfPlanExcelMonth); // План шурфов по месяцам (документ)
            menu2->addAction(gidrAction.aShurfTable); // Анализ Плана шурфов
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontShurfOtobr); // Включить/отключить отображение объектов
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontPovrOtop); // Выбор отопительного сезона
            menu2->addAction(gidrAction.aRemontOsmotrTeploSet); // Выбор фрагмента сети по Начальнику участка
            menu2->addSeparator();
            menu2->addAction(gidrAction.aShurfBezUch); // Шурфовки без участков
        menu2 = menu1->addMenu(tr("Осмотр"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aOsmotrAdd); // Создать контур осмотра
            menu2->addAction(gidrAction.aOsmotrSave); // Обновить сохранение контура
            menu2->addAction(gidrAction.aListOsmotr2); // Выполнить осмотр контура
            menu2->addAction(gidrAction.aLastOsmotr2); // Последний контур осмотра
            menu2->addAction(gidrAction.aLastFaktory); // Выбор трубопроводов для осмотра
            menu2->addAction(gidrAction.aOsmotrZapros1); // Осмотренные фрагменты сети
            menu2->addAction(gidrAction.aOsmotrZapros2); // Осмотренные трубопроводы за период
            menu2->addAction(gidrAction.aOsmotrZapros3); // Неосмотренные трубопроводы за период
            menu2->addSeparator();
            menu2->addAction(gidrAction.aOsmotrAnaliz2); // Анализ осмотра контуров
            menu2->addAction(gidrAction.aOsmotrAnaliz); // Анализ осмотра трубопроводов контура
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontPovrDefOtobr); // Включить/отключить отображение объектов
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontPovrOtop); // Выбор отопительного сезона
            menu2->addAction(gidrAction.aRemontOsmotrTeploSet); // Выбор фрагмента сети по Начальнику участка
        menu2 = menu1->addMenu(tr("Ремонт"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aRemontAddPlan); // Сохранить контур плана ремонта
            menu2->addAction(gidrAction.aRemontAddCurrent); // Сохранить контур текущего ремонта
            menu2->addAction(gidrAction.aRemontSave); // Обновить сохранение контура
            menu2->addSeparator();
            menu2->addAction(gidrAction.aListRemont2); // Список контуров ремонтов
            menu2->addAction(gidrAction.aLastRemont2); // Последний контур ремонта
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontTrub); // Выбор трубопроводов контура ремонта
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontPlan); // Контуры капитального/инвестиционного ремонтов
            menu2->addAction(gidrAction.aRemontCurrent); // Контуры текущих ремонтов
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontProcess); // Ремонт в процессе выполнения
            menu2->addAction(gidrAction.aRemontVypolneno); // Законченные ремонты
            menu2->addAction(gidrAction.aRemontAll2); // Отобразить все контура
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontWord1); // График выполнения ремонтов (документ)
            menu2->addAction(gidrAction.aRemontWord2); // План капитальных/инвестиционных ремонтов (документ)
            menu2->addAction(gidrAction.aRemontWord3); // План ремонтов по месяцам (документ)
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontAnaliz2); // Анализ контуров ремонта
            menu2->addAction(gidrAction.aRemontAnaliz); // График выполнения ремонтов
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontPovrDefOtobr); // Включить/отключить отображение объектов
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontPovrOtop); // Выбор отопительного сезона
            menu2->addAction(gidrAction.aRemontOsmotrTeploSet); // Выбор фрагмента сети по Начальнику участка
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontVyborNach); // Создать контур ремонта
            menu2->addAction(gidrAction.aMarkRev); // Добавить/удалить трубопровод к контуру
        menu2 = menu1->addMenu(tr("Опрессовка"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aOpresAddPlan); // Сохранить контур плана опрессовки
            menu2->addAction(gidrAction.aOpresSave); // Обновить сохранение контура
            menu2->addSeparator();
            menu2->addAction(gidrAction.aOpresGranitsaRazdela); // Границы раздела
            menu2->addAction(gidrAction.aListOpres); // Список контуров опрессовок
            menu2->addAction(gidrAction.aLastOpres); // Последний контур опрессовки
            menu2->addSeparator();
            menu2->addAction(gidrAction.aOpresPlan); // Контуры планов опрессовок сезона
            menu2->addSeparator();
            menu2->addAction(gidrAction.aOpresProcess); // Опрессовка в процессе выполнения
            menu2->addAction(gidrAction.aOpresVypolneno); // Завершенные опрессовки
            menu2->addAction(gidrAction.aOpresAll2); // Отобразить все контура
            menu2->addSeparator();
            menu2->addAction(gidrAction.aOpresWord1); // План опрессовок (документ)
            menu2->addAction(gidrAction.aOpresWord2); // График опрессовок по месяцам (документ)
            menu2->addSeparator();
            menu2->addAction(gidrAction.aOpresAnaliz2); // Анализ контуров опрессовок
            menu2->addAction(gidrAction.aOpresAnaliz); // Анализ нарушений контура
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontPovrDefOtobr); // Включить/отключить отображение объектов
            menu2->addSeparator();
            menu2->addAction(gidrAction.aRemontPovrOtop); // Выбор отопительного сезона
            menu2->addAction(gidrAction.aRemontOsmotrTeploSet); // Выбор фрагмента сети по Начальнику участка
            menu2->addSeparator();
            menu2->addAction(gidrAction.aOpresVyborNach); // Создать контур опрессовки
            menu2->addAction(gidrAction.aListRemont2); // Список контуров ремонтов
            menu2->addAction(gidrAction.aMarkRev); // Добавить/удалить трубопровод к контуру
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Печать шаблона"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aRemontPovrKarta); // Карта нарушений
            menu2->addAction(gidrAction.aRemontPovrKarta2); // Карта осмотра шурфа
    menu1 = menu->addMenu(tr("Коррозия"));
        menu1->setToolTipsVisible(true);
//    map_menu[gidrAction].push_back(menu1);
        menu1->addAction(gidrAction.aViewToolbarKorrozia); // Панель Индикаторов коррозии
        menu1->addAction(gidrAction.aKorrozAdd); // Установить индикатор коррозии
        menu1->addAction(gidrAction.aKorrozDel); // Удалить индикатор коррозии
        menu1->addAction(gidrAction.aFragmentPolyKorroziaAr); // Удалить выбранные индикаторы
        menu1->addAction(gidrAction.aKorrozInfo); // Информация по индикатору
        menu1->addSeparator();
        menu1->addAction(gidrAction.aKorroziaUpdate); // Выбор шаблона плана
        menu1->addAction(gidrAction.aKorrozDoc3); // План установки
        menu1->addSeparator();
        menu1->addAction(gidrAction.aKorrozTable); // Таблица индикаторов коррозии
        menu1->addSeparator();
        menu1->addAction(gidrAction.aKorroziaZhurnal1); // План индикаторов
        menu1->addAction(gidrAction.aKorroziaZhurnal2); // Индикаторы установленные 
        menu1->addAction(gidrAction.aKorroziaZhurnal3); // Индикаторы извлеченные
        menu1->addAction(gidrAction.aKorroziaCurrent); // Все индикаторы сезона
        menu1->addSeparator();
        menu1->addAction(gidrAction.aKorrozDoc2); // Оценка за отопительный сезон
        menu1->addAction(gidrAction.aKorrozDoc1); // Оценка за выбранные сезоны
        menu1->addSeparator();
        menu1->addAction(gidrAction.aKorroziaOnoff); // Включить/отключить отображение объектов
        menu1->addAction(gidrAction.aKorroziaSezon); // Выбор отопительного сезона


    menu1 = menu->addMenu(tr("Износ"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aIznos); // Износ оборудования
        menu1->addAction(gidrAction.aRemontPlan); // Планирование ремонтов
    menu1 = menu->addMenu(tr("Электросеть"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aViewToolbarElectro); // Панель Электросеть
        menu1->addAction(gidrAction.aElectroAdd); // Установить объект
        menu1->addAction(gidrAction.aElectroInfo); // Информация по объекту
        menu1->addAction(gidrAction.aElectroTable); // Таблица
        menu1->addAction(gidrAction.aElectroDel); // Удалить объект
        menu1->addAction(gidrAction.aZhurnalElectro); // Журнал
    menu1 = menu->addMenu(tr("Теплопотери"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aTeplopoteri); // Теплопотери
    menu1 = menu->addMenu(tr("Сервис"));
        menu1->setToolTipsVisible(true);
        menu2 = menu1->addMenu(tr("Настройки"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aColors); // Стиль линии
            menu2->addAction(gidrAction.aFon); // Цвет фона
            menu3 = menu2->addMenu(tr("Шрифты"));
                menu3->setToolTipsVisible(true);
//                menu3->addAction(gidrAction.aFont2); // Шрифт текста
                menu3->addAction(gidrAction.aFont); // Шрифт надписей
//                menu3->addAction(gidrAction.aFontMag); // Шрифт названия магистрали
                menu3->addAction(gidrAction.aFontPanel); // Шрифт в проводнике
            menu2->addAction(gidrAction.aTransp); // Прозрачность
            menu2->addSeparator();
            menu2->addAction(gidrAction.aAstr); // Сетка
            menu2->addAction(gidrAction.aRamka); // Рамка схемы
            menu2->addSeparator();
            menu2->addAction(gidrAction.aIsMas); // Сохранять пропорции
            menu2->addAction(gidrAction.aMasPic); // Номинальный масштаб
            menu2->addAction(gidrAction.aMasPodpis); // Масштаб откл.надписей
//            menu2->addAction(gidrAction.aTrio); // Передвинуть
//            menu2->addAction(gidrAction.aIndexes); // Обновление индексов
//            menu2->addAction(gidrAction.aHttp2); // Веб-сервер
            menu3 = menu2->addMenu(tr("Дополнительная обработка"));
                menu3->setToolTipsVisible(true);
                menu3->addAction(gidrAction.aQuery); // Запрос

/*                
                menu3->addAction(gidrAction.aExportPts); // Экспорт фрагмента ПТС
                menu3->addAction(gidrAction.aImportPts); // Импорт фрагмента ПТС
                menu3->addAction(gidrAction.aSetAvtoOn); // Сделать потребители автоматическими
                menu3->addAction(gidrAction.aSetAvtoOff); // Сделать потребители неавтоматическими
                menu3->addAction(gidrAction.aCreateSortNode); // Создание таблицы sortNodesForUchastok
                menu3->addAction(gidrAction.aRasList); // Список расчетов
                menu3->addAction(gidrAction.aSetPsMap); // Установить объекты ПС
                menu3->addAction(gidrAction.aSavePng); // Сохранить как PNG
                menu3->addAction(gidrAction.aUtNapr2); // Сортировка участков
                menu3->addAction(gidrAction.aInvert); // Инвертировать выделение
                menu3->addAction(gidrAction.aSetAddr); // Адреса потребителей
                menu3->addAction(gidrAction.aAlma); // Паспортизация
                menu3->addAction(gidrAction.aTuZav); // Установить статус Завершен для ТУ
                menu3->addAction(gidrAction.aExport); // Экспорт гидравлики в TXT
                menu3->addAction(gidrAction.aSetLineid); // !
                menu3->addAction(gidrAction.aSetIst); // Расстановка источников
                menu3->addAction(gidrAction.aUchList); // Список участков ПТС
                menu3->addAction(gidrAction.aFindcoord); // Поиск по координатам
                menu3->addAction(gidrAction.aFindcoordDeg); // Поиск по координатам
                menu3->addAction(gidrAction.aAsyncCheck); // Проверка Async Task
                menu3->addAction(gidrAction.aCheckPo); // Проверка
                menu3->addAction(gidrAction.aResetGeo); // Обновить надписи
*/
            menu2->addAction(gidrAction.aPsAstanaName); // Отображение имен узлов
            menu2->addAction(gidrAction.aColorOnlyPts); // Цвета только для участков ПТС
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Экспорт"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aExportDxf); // Автокад DXF
            menu2->addAction(gidrAction.aExportShape); // ESRI shape-файлы
        menu1->addSeparator();
        menu1->addAction(gidrAction.aPrPo); // Реальные -> обобщенные
        menu1->addAction(gidrAction.aUtNapr); // Сортировка участков
        menu1->addSeparator();
        menu2 = menu1->addMenu(tr("Трехходовые клапаны"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aC3Del); // Удалить внутренние схемы
            menu2->addAction(gidrAction.aC3Add); // Создать внутренние схемы
        menu1->addSeparator();
        menu1->addAction(gidrAction.aIspravl); // Восстановить файл
        menu1->addAction(gidrAction.aOpc); // Настройка OPC сервера
/*
        menu2 = menu1->addMenu(tr("Интернет карты"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aNomap); // Нет карты
            menu3 = menu2->addMenu(tr("Google"));
                menu3->setToolTipsVisible(true);
                menu3->addAction(gidrAction.aGoogleMap); // Карта
                menu3->addAction(gidrAction.aGoogleSat); // Спутник
                menu3->addAction(gidrAction.aGoogleHybrid); // Гибридная
            menu2->addAction(gidrAction.aVisicomMap); // Карта VISICOM
            menu3 = menu2->addMenu(tr("Карты WMS"));
                menu3->setToolTipsVisible(true);
                menu3->addAction(gidrAction.aWmsMap); // Карта WMS
                menu3->addAction(gidrAction.aWmsCust); // Настройка
            menu3 = menu2->addMenu(tr("Дополнительные карты"));
                menu3->setToolTipsVisible(true);
                menu3->addAction(gidrAction.aMapSemey); // Семей (город)
                menu3->addAction(gidrAction.aMapSemeyTs); // Семей (тепловые сети)
            menu2->addAction(gidrAction.aOpenstreetmap); // OpenStreetMap
            menu2->addAction(gidrAction.a2gisMap); // 2ГИС
            menu2->addAction(gidrAction.aGoogleElevation); // Высота по Google
            menu2->addAction(gidrAction.aGoogleElevationSet); // Высота по Google
            menu2->addAction(gidrAction.aMapSearch); // Поиск
            menu2->addAction(gidrAction.aMapPath); // Каталог
            menu2->addAction(gidrAction.aMapYes); // Восстанавливать подключение
*/
        menu1->addAction(gidrAction.aFileOpenOld); // Импорт



    menu2 = menu->addMenu(tr("Интернет карты"));
        menu2->setToolTipsVisible(true);
        menu2->addAction(gidrAction.aNomap); // Нет карты
        menu3 = menu2->addMenu(tr("Google"));
            menu3->setToolTipsVisible(true);
            menu3->addAction(gidrAction.aGoogleMap); // Карта
            menu3->addAction(gidrAction.aGoogleSat); // Спутник
            menu3->addAction(gidrAction.aGoogleHybrid); // Гибридная
        menu2->addAction(gidrAction.aVisicomMap); // Карта VISICOM
        menu3 = menu2->addMenu(tr("Карты WMS"));
            menu3->setToolTipsVisible(true);
            menu3->addAction(gidrAction.aWmsMap); // Карта WMS
            menu3->addAction(gidrAction.aWmsCust); // Настройка
        menu3 = menu2->addMenu(tr("Дополнительные карты"));
            menu3->setToolTipsVisible(true);
            menu3->addAction(gidrAction.aMapSemey); // Семей (город)
            menu3->addAction(gidrAction.aMapSemeyTs); // Семей (тепловые сети)
        menu2->addAction(gidrAction.aOpenstreetmap); // OpenStreetMap
        menu2->addAction(gidrAction.a2gisMap); // 2ГИС
        menu2->addAction(gidrAction.aEsriSatMap); // 2ГИС
        menu2->addAction(gidrAction.aMaptilerStreets); // MapTiler Улицы
        menu2->addAction(gidrAction.aMaptilerTopo); // MapTiler Топография
        menu2->addAction(gidrAction.aMaptilerHybrid); // MapTiler Гибридная
        menu2->addAction(gidrAction.aMaptilerOsm); // MapTiler OpenStreetMap
        menu2->addAction(gidrAction.aGoogleElevation); // Высота по Google
        menu2->addAction(gidrAction.aGoogleElevationSet); // Высота по Google
        menu2->addAction(gidrAction.aMapSearch); // Поиск
        menu2->addAction(gidrAction.aMapPath); // Каталог
        menu2->addAction(gidrAction.aMapYes); // Восстанавливать подключение



    menu1 = menu->addMenu(tr("Администратор"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aAddUser); // Регистрация пользователей
        menu1->addAction(gidrAction.aPassword); // Изменить доступ пользователя
        menu1->addAction(gidrAction.aPassword2); // Изменить пароль
    menu1 = menu->addMenu(tr("Вид"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aViewStatusBar); // Строка статуса
    menu1 = menu->addMenu(tr("&?"));
        menu1->setToolTipsVisible(true);
        menu1->addAction(gidrAction.aHelpFinder); // Содержание\tF1
        menu1->addSeparator();
        menu1->addAction(main_window->mainAction.aUpdate); // Последнее обновление
        menu1->addAction(main_window->mainAction.aUpdateSetup); // Настройка обновлений
        menu1->addAction(main_window->mainAction.aAppAbout); // О программе
//        menu1->addAction(main_window->mainAction.aQtAbout); // О Qt


}
