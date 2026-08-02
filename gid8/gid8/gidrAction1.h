struct GidrAction
{
QAction *aFileClose; // Закрыть
QAction *aEditor2; // Просмотр схемы
QAction *aEditor4; // АРМ
QAction *aSqlSave; // Создание копии Базы данных
QAction *aSqlCopyFragment; // Вставить Фрагмент схемы
QAction *aSqlDelFragment; // Удалить Фрагмент схемы
QAction *aAddFragment; // Добавить Фрагмент схемы
QAction *aExportFragment; // Экспорт фрагмента
QAction *aExportFragmentVyd; // Экспорт выделенного фрагмента
QAction *aExportFragmentsMulty; // Экспорт всех фрагментов
QAction *aImportFragment; // Импорт фрагмента
QAction *aImportFragmentMdb; // Конвертор ТГИД-05/ ТГИД-07
QAction *aMainCxema; // Активная схема
QAction *aAddDop; // Отобразить дополнительную схему
QAction *aBmp; // Файл графической подложки...
QAction *aBmpFind; // Найти графическую подложку
QAction *aCatalog; // Каталоги
QAction *aFilePrint; // Печать...\tCtrl+P
QAction *aFilePrintPreview; // Предварительный просмотр
QAction *aPrintFr; // Фрагмент для печати
QAction *aFragment; // Выделить фрагмент
QAction *aBdDel; // 

QAction *aFilePrintSetup; // Параметры страницы...
QAction *aAppExit; // Выход
QAction *aEditFindNode; // Найти узел...\tCtrl+F
QAction *aEditUndo; // Отменить\tCtrl+Z
QAction *aEditUndoTU; // Отменить\tCtrl+Z
QAction *aEditUndoDel; // Отменить удаления
QAction *aZapVnCx; // Узлы с внутренней схемой
QAction *aZapNullGeo; // Узлы с нулевой геодезической отметкой
QAction *aOdnolin; // Однолинейные участки
QAction *aRaznolin; // Участки с разными подачей и обраткой
QAction *aRasprMag; // Расчетные схемы
QAction *aRasprMag2; // Объекты системы теплоснабжения
QAction *aIstSety; // Источник
QAction *aIstSetyNew; // Источник
QAction *aVyd; // Фрагмент сети
QAction *aBalans; // Балансовая принадлежность
QAction *aTubing1; // Канальная
QAction *aTubing2; // Бесканальная
QAction *aTubing4; // Надземная
QAction *aTubing3; // Подвальная
QAction *aTubing5; // Обвязка узлов и насосных станций
QAction *aFindKti; // По коэффициенту тепловых испытаний
QAction *aMark; // Выделить область
QAction *aSetOtv; // Установить ФИО техников
QAction *aSetTr; // Установить Код расчетных температур
QAction *aSetUr; // Установить Код удельных расходов
QAction *aSetKvPt; // Установить Коэффициенты вариации по потребителям
QAction *aSetUf; // Установить Коэффициенты смешения элеватора
QAction *aSetTp; // Установить Тепловые пункты по потребителям
QAction *aSetUdobVent; // Установить Удельный объем системы вентиляции
QAction *aSetUdobOt; // Установить Удельный объем системы отопления
QAction *aSetOpenKoef; // Коэф. часовой неравномерности
QAction *aSetOpenRez; // Расчетные тепловые потери в рециркуляц. контуре ГВС
QAction *aSetOpenRezT; // Температура в рециркуляционном трубопроводе ГВС
QAction *aSetOpenGvsT; // Расчетная температура горячей воды
QAction *aSetDiams; // Установить Диаметр
QAction *aSetLosesShare; // Установить Долю местных потерь
QAction *aSetKolChas; // Установить Количество часов работы
QAction *aSetKvUt; // Установить Коэффициенты вариации по участкам
QAction *aSetKti; // Установить Коэффициенты тепловых испытаний
QAction *aSetOrg; // Установить Организации
QAction *aSetPipeRemontType; // Установить Признак ремонта
QAction *aSetTubingType; // Установить Тип прокладки
QAction *aSetSher; // Установить Эквивалентную шероховатость
QAction *aSetDate2; // Установить Дату первичного ввода в эксплуатацию
QAction *aSetDate1; // Установить Дату последней перекладки
QAction *aSetDate3; // Установить Дату планируемого ремонта
QAction *aSetKodRs; // Установить Код расчетной схемы
QAction *aGeodz; // Установить Отметки узлов горизонтали
QAction *aSetKorrozia; // Установить Индикаторы коррозии
QAction *aSetPodpOn; // Показать Надписи
QAction *aSetPodpOff; // Не отображать Надписи
QAction *aFragmentPoly; // Выделить многоугольный фрагмент
QAction *aMoveVyd; // Передвинуть фрагмент
//QAction *aFragment; // Выделить фрагмент
QAction *aEditCopy; // Копировать\tCtrl+C
QAction *aEditCopyPng; // Копировать в PNG
QAction *aViewToolbarMain; // Главная
QAction *aViewToolbarCxema; // Схема
QAction *aViewToolbarZ; // Запросы
QAction *aViewToolbar2PtsNew; // Паспортизация
QAction *aTuOnOff; // Технические условия
QAction *aViewToolbarElectro; // Электрические сети
QAction *aViewToolbarControlTu; // Контроль технического состояния
QAction *aViewToolbar2Remont; // Ремонт
QAction *aViewToolbarOpressovka; // Опрессовки
QAction *aViewToolbarKorrozia; // Индикаторы коррозии
QAction *aNavigator; // Навигатор
QAction *aLeftBar; // Проводник карты
QAction *aMagAlma; // Проводник структуры сети
QAction *aBottomRemont; // Проводник мониторинга
QAction *aProtocol; // Протокол
QAction *aMovew; // Перемещение\tM
QAction *aPovorot; // Повернуть
QAction *aPovorot2; // Возврат
QAction *aMasall; // Отобразить всю схему\tF3
QAction *aPlus; // Увеличить\t+
QAction *aMinus; // Уменьшить\t-
QAction *aMasshtab; // Масштаб...\tCtrl+F3
QAction *aZoom; // Масштабировать
QAction *aPicture; // Графическая подложка
QAction *aF5; // Перерисовать схему\tF5
QAction *aCtrlF5; // Перерисовать схему\tCtrl+F5
QAction *aGeobaza; // Показать геобазу
QAction *aFindGeo; // Поиск в геобазе...
QAction *aGeoFindNext; // Продолжение поиска
QAction *aFindGeoAddr; // Поиск по адресу
QAction *aGeoInfo; // Информация из геобазы
QAction *aSetLength; // Установить длины
QAction *a1000; // Создать файл полей
QAction *a1001; // Переименовать поля
QAction *a1002; // Сохранить переименованные поля
QAction *aNode; // Установить узел\tN
QAction *aLine; // Линейный объект\tL
QAction *aRename; // Изменить тип объекта
QAction *aZn; // Узел с заданным напором
QAction *aKam; // Внутренняя схема
QAction *aNodeNeotr; // Установка конвертированных узлов\tC
QAction *aZapNezak; // Незаконченные узлы
QAction *aSetCoordNull; // Удалить координаты выделенных узлов
QAction *aNodeMove; // Перенести\tV
QAction *aDel; // Удалить объект\tD
QAction *aOpenClose; // Закрыть/открыть объект
QAction *aNodeMove1; // Разъединить линии
QAction *aSwap; // Изменить направление потока
QAction *aMoveLineEnd; // Перенести линию
QAction *aMoveLineEndNext; // Перенести линию Продолжение
QAction *aText; // Установить текст
QAction *aDoItDr; // Плановый...
QAction *aDoIt; // Фактический...
QAction *aDoItListDr; // Плановый по списку...
QAction *aDoItList; // Фактический по списку...
QAction *aMagFragment; // Магистральный фрагмент
QAction *aNapr; // Выбpать напpавление
QAction *aSavePjezo; // Сохранить направление...
QAction *aListPjezo; // Список направлений...
QAction *aClearOut; // Удалить старые расчеты
//QAction *aProtocol; // Протокол
QAction *aInfo; // Свойства объекта\tI
QAction *aViewDb; // Таблицы...
QAction *aNovisual; // Системные параметры...
QAction *aMoveTo; // Таблица объекта
QAction *aInfoGid; // Результат расчета
QAction *aOut2; // Отчеты...
QAction *aTableDan; // Редактор таблиц...
QAction *aExcel2; // Таблицы Excel...
QAction *aInf; // Отображаемая информация...
QAction *aGidinf; // Отображать информацию
QAction *aPodp; // Отображать имена узлов
QAction *aPodpAll; 
QAction *aPodpPo; // Отоброжать схему ПТС
QAction *aPodpRes; // Отображать имена узлов с #
QAction *aPribor; // Отображать Приборы учета
QAction *aRezhim; // Отображать вспомогательные участки
QAction *aPopupPodpOn; // Показать скрытые имена
QAction *aPopupPodpOff; // Не показывать скрытые имена
QAction *aPodpYesNo; // Отображать/Не отображать надпись
QAction *aNaprGid; // Распределение потоков
QAction *aF4; // Измерить расстояние\tF4
QAction *aAnal; // Анализ...
QAction *aZaprosy; // Запросы
QAction *aAnalVyd; // Анализ...
QAction *aColorLine; // Участки
QAction *aColorLine1; // Потребители
QAction *aZap1; // Объем сети
QAction *aZap2; // Длина теплопроводов
QAction *aZap7; // Длина теплопроводов по диаметрам
QAction *aZap71; // Длина теплопроводов по диаметрам и способам прокладки
QAction *aTimePr; // Время прохождения
QAction *aPoteri; // Тепловые потери
QAction *aZap3; // Теплопотребление полученное
QAction *aZap4; // Теплопотребление в закрытых системах полученное
QAction *aZap5; // Теплопотребление в открытых системах полученное
QAction *aZap6; // Закрытые потребители
QAction *aPotNagr0; // С нулевой нагрузкой
QAction *aPotrOtkl; // Отключенные потребители
QAction *aUtZakr; // Закрытые участки
QAction *aUtZakrAll; // Отключенные участки
QAction *aZapOtr; // Отрицательные перепады
QAction *aPtTempMin; // Низкие температуры
QAction *aZavozd; // Завоздушивание
QAction *aIsprLast; // Изменённые объекты
QAction *aPjezo; // Пьезометр
QAction *aZona; // Гидростатические зоны
//QAction *aTuOnOff; // Объекты ТУ
QAction *aNagrZd; // Физические лица
QAction *aNagrOrg; // Юридические лица
QAction *aTuTable; // Технические условия
QAction *aTuYear; // ТУ по годам
QAction *aTuSost; // ТУ состояние
QAction *aTuExcel; // Итоговый тепловой баланс
QAction *aTuSvod; // Итоговые по годам
QAction *aTuZhurnal; // Журнал регистрации ТУ
QAction *aFindTuIst; // ТУ по источнику
QAction *aTuFind; // ТУ, Договора
QAction *aPrisNagrEdit; // Присоединенная нагрузка по источникам
QAction *aQGvs; // Коэффициент перевода k=Qгвmax/Qгвср
QAction *aNagrZdNeiz; // Физические лица
QAction *aNagrOrgNeiz; // Юридические лица
QAction *aTuTableNeiz; // Ненайденные ТУ на карте
QAction *aTuIst; // Отчет по источникам
//QAction *aViewToolbar2PtsNew; // Панель Объекты ПТС
QAction *aPtsAdd; // Установить объект
QAction *aPtsInfo; // Информация по объекту
QAction *aPtsTable; // Таблица
QAction *aPtsDel; // Удалить объект
QAction *aProtPass; // Протяженность
QAction *aProtNew; // Протяженность выделенных трубопроводов
QAction *aProtMag; // Протяженность магистралей
QAction *aProtRs; // Протяженность распредсетей
QAction *aProtKot; // Протяженность котельных
QAction *aPaspNew; // Паспорт сети
QAction *aViewToolbarRemont2; // Панель Контуры ремонта
QAction *aNaprRemont2; // Создать Контур ремонта...
QAction *aListRemont2; // Выбрать Контур ремонта...
QAction *aNaprTableRemont2; // Таблица участков Контура ремонта...
QAction *aSaveRemont2; // Сохранить текущий Контур ремонта...
QAction *aSaveRemont2New; // Сохранить новый Контур ремонта...
//QAction *aViewToolbar2Remont; // Панель Ремонт
QAction *aRemontsAdd; // Создать объект ремонта
QAction *aRemontsInfo; // Информация по объекту
QAction *aRemontsTable; // Таблица
QAction *aRemontsDel; // Удалить объект
QAction *aZhurnalDefect; // Повреждения
QAction *aZhurnalDiag; // Диагностика
QAction *aZhurnalRemont; // Ремонты
QAction *aZhurnalExcel; // Excel
//QAction *aViewToolbarOpressovka; // Панель Опрессовок
QAction *aNaprOpres; // Создать Контур опрессовок...
QAction *aListOpres; // Выбрать Контур опрессовок...
QAction *aRemontsAddOpres; // Создать Повреждение опрессовок...
QAction *aZhurnalDefectOpres; // Журнал Повреждений опрессовок...
QAction *aNaprTableOpres; // Таблица участков Контура опрессовок...
QAction *aSaveOpres; // Сохранить текущий Контур опрессовок...
QAction *aSaveOpresNew; // Сохранить новый Контур опрессовок...
//QAction *aViewToolbarKorrozia; // Панель Индикаторов коррозии
QAction *aKorrozAdd; // Установить индикатор коррозии
QAction *aKorrozDel; // Удалить индикатор коррозии
QAction *aFragmentPolyKorroziaAr; // Удалить выбранные индикаторы
QAction *aKorrozInfo; // Информация по индикатору
QAction *aKorroziaUpdate; // Выбор шаблона плана
QAction *aKorrozDoc3; // План установки
QAction *aKorrozTable; // Таблица индикаторов коррозии
QAction *aKorroziaZhurnal1; // План индикаторов
QAction *aKorroziaZhurnal2; // Индикаторы установленные 
QAction *aKorroziaZhurnal3; // Индикаторы извлеченные
QAction *aKorroziaCurrent; // Все индикаторы сезона
QAction *aKorrozDoc2; // Оценка за отопительный сезон
QAction *aKorrozDoc1; // Оценка за выбранные сезоны
QAction *aKorroziaOnoff; // Включить/отключить отображение
QAction *aKorroziaSezon; // Выбор отопительного сезона
QAction *aIznos; // Износ оборудования
QAction *aRemontPlan; // Планирование ремонтов
//QAction *aViewToolbarElectro; // Панель Электросеть
QAction *aElectroAdd; // Установить объект
QAction *aElectroInfo; // Информация по объекту
QAction *aElectroTable; // Таблица
QAction *aElectroDel; // Удалить объект
QAction *aZhurnalElectro; // Журнал
QAction *aTeplopoteri; // Теплопотери
QAction *aColors; // Стиль линии
QAction *aFon; // Цвет фона
QAction *aFont2; // Шрифт текста
QAction *aFont; // Шрифт надписей
QAction *aFontMag; // Шрифт названия магистрали
QAction *aFontPanel; // Шрифт в проводнике
QAction *aTransp; // Прозрачность
QAction *aAstr; // Сетка
QAction *aRamka; // Рамка схемы
QAction *aIsMas; // Сохранять пропорции

QAction *aSetupGid9; // 


QAction *aMasPic; // Номинальный масштаб
QAction *aMasPodpis; // Масштаб откл.надписей
QAction *aTrio; // Передвинуть
QAction *aUpdateSetup; // Настройка обновлений
QAction *aHttp2; // Веб-сервер

QAction *aQuery; // 

QAction *aExportPts; // Экспорт фрагмента ПТС
QAction *aImportPts; // Импорт фрагмента ПТС
QAction *aSetAvtoOn; // Сделать потребители автоматическими
QAction *aSetAvtoOff; // Сделать потребители неавтоматическими
QAction *aCreateSortNode; // Создание таблицы sortNodesForUchastok
QAction *aRasList; // Список расчетов
QAction *aSetPsMap; // Установить объекты ПС
QAction *aSavePng; // Сохранить как PNG
QAction *aUtNapr2; // Сортировка участков
QAction *aInvert; // Инвертировать выделение
QAction *aSetAddr; // Адреса потребителей
QAction *aAlma; // Паспортизация
QAction *aTuZav; // Установить статус Завершен для ТУ
QAction *aExport; // Экспорт гидравлики в TXT
QAction *aSetLineid; // !
QAction *aSetIst; // Расстановка источников
QAction *aUchList; // Список участков ПТС
QAction *aFindcoordDeg; // Поиск по координатам
QAction *aAsyncCheck; // Проверка Async Task
QAction *aCheckPo; // Проверка
QAction *aResetGeo; // Обновить надписи
QAction *aPsAstanaName; // Отображение имен узлов
QAction *aColorOnlyPts; // Цвета только для участков ПТС
QAction *aExportDxf; // Автокад DXF
QAction *aExportShape; // ESRI shape-файлы
QAction *aPrPo; // Реальные -> обобщенные
QAction *aUtNapr; // Сортировка участков
QAction *aC3Del; // Удалить внутренние схемы
QAction *aC3Add; // Создать внутренние схемы
QAction *aIspravl; // Восстановить файл
QAction *aOpc; // Настройка OPC сервера
QAction *aNomap; // Нет карты
QAction *aGoogleMap; // Карта
QAction *aGoogleSat; // Спутник
QAction *aGoogleHybrid; // Гибридная
QAction *aVisicomMap; // Карта VISICOM
QAction *aWmsMap; // Карта WMS
QAction *aWmsCust; // Настройка
QAction *aMapSemey; // Семей (город)
QAction *aMapSemeyTs; // Семей (тепловые сети)
QAction *aOpenstreetmap; // OpenStreetMap
QAction *a2gisMap; // 2ГИС
QAction *aEsriSatMap; // 2ГИС

QAction *aGoogleElevation; // Высота по Google
QAction *aGoogleElevationSet; // Высота по Google настройка
QAction *aMapSearch; // Поиск
QAction *aMapPath; // Каталог
QAction *aMapYes; // Восстанавливать подключение
QAction *aFileOpenOld; // Импорт
QAction *aAddUser; // Регистрация пользователей
QAction *aPassword; // Изменить доступ пользователя
QAction *aPassword2; // Изменить пароль
QAction *aViewStatusBar; // Строка статуса
QAction *aViewApplookWin2000; // Windows &2000
QAction *aViewApplookOffXp; // Office &XP
QAction *aViewApplookWinXp; // &Windows XP
QAction *aViewApplookOff2003; // Office 200&3
QAction *aViewApplookVs2005; // Visual Studio 200&5
QAction *aViewApplookVs2008; // Visual Studio 200&8
QAction *aViewApplookOff2007Blue; // &Blue Style
QAction *aViewApplookOff2007Black; // B&lack Style
QAction *aViewApplookOff2007Silver; // &Silver Style
QAction *aViewApplookOff2007Aqua; // &Aqua Style
QAction *aHelpFinder; // Содержание\tF1
QAction *aUpdate; // Последнее обновление
//QAction *aUpdateSetup; // Настройка обновлений
QAction *aAppAbout; // О программе

//------------------------------------------------------
QAction *aEsc; // 


};

