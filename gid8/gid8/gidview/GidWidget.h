#pragma once

#include <QtSql>
#include <QDateTime>
#include <QWidget>
#include <QMessageBox>
#include <QResizeEvent>

#include "std.h"

#include "Scroll.h"
//#include "shp_dbf.h"
#include <cxema/coordlis.h>
#include <cxema/cxema1.h>
#include <any/download.h>
#include <any/download2.h>

#include <geo/Klassif.h>
#include <geo/geofile.h>
#include <maps/maps_id.h>
//#include "geol.h"

#include "gidrAction1.h"

//#include <property/PropertyDial.h>
    struct PropertyStr;

class QToolBar;
class QMenu;
struct cdxf;

class Ribbon;
class MainWindow;
class QPrinter;
class QTimer;
struct GID_STYLE;
typedef void CLINE2;

enum REGIM {
  R_INIT,
  R_BEGIN_RECT,
  R_BEGIN_DRAW1,
  R_BEGIN_DRAW2,
  R_BEGIN_DRAW_COPY_BMP1,
  R_BEGIN_DRAW_COPY_BMP2,
  R_BEGIN_MOVE_VIEW,
  R_MOVE_VIEW,
  R_MOVE_NODE,
  R_BEGIN_DRAW_COPY1,
  R_BEGIN_DRAW_COPY2,
  R_BEGIN_MOVE_LINE,
  R_MOVE_LINE,
  R_MOVE_LINE1,
  R_BEGIN_LEN1,
  R_BEGIN_LEN2,

  R_BEGIN_GEO_LOC1,
  R_BEGIN_GEO_LOC6_1,
  R_BEGIN_GEO_LOC6_2,
  R_BEGIN_GEO_LOC2_1,
  R_BEGIN_GEO_LOC2_2,

  R_BEGIN_CANAL_1,
  R_BEGIN_CANAL_2,



  R_BEGIN_LEN_OKR1,
  R_BEGIN_LEN_OKR2,
  R_BEGIN_LEN_OKR3,
  R_BEGIN_LEN_OKR4,


  R_BEGIN_DRAW_LINE,
  R_BEGIN_NODE_NEOTR,

  R_BEGIN_DRAW_TYPE,

  R_BEGIN_RECT_1,
  R_FRAGM,
  R_BEGIN_MOVE_FRAGMENT,
};

struct Layer {
    QString title = "";
    QString name = "";
    double wx1, wx2, wy1, wy2;
};

//void findXY(CFPoint p, double md, Klassif *kls, list<KlGeo> & geo_list);
//void findXY(CFPoint p, double md, list<KlGeo> & geo_list);

void findXY(CFPoint p, double md, list<KlGeo> & geo_list, bool loc2 = false);
void findXYkls(CFPoint p, double md, Klassif *kls, list<KlGeo> & geo_list, bool loc2 = false);

class PropertyDial;


class GidWidget : public Scroll
{
    Q_OBJECT

public:
    GidWidget(QWidget *parent = 0);
    ~GidWidget();

    void alseco(PropertyDial *prop, const QString & table, const QString &  column, const QString &  kod, const QString &  name);

    bool save();
    bool saveAs();

    bool connectSQL(int rdbms, const QString & host, int port, const QString & baza, const QString & geo, const QString & user, const QString & password);
    bool connectSqlite(const QString & baza);

    void setDuration(int secs);
    int duration() const;
    QString status(int i);

    bool isGidrInf() {return m_bIsGidrInf;}
    bool astr() { return m_dX != 0; }
    bool isGeo() { return m_bIsGeo; }
    bool isNapr();

    void moveLine(int id);
    void moveNode(int id);

    bool moveText(const QString & parent, const QString & txt);

    void moveGeo(const QString & tn, int id);

    void setToolbars();

    void setTitle();

    QString getTitle();

    void setFileID(int fileID, bool mov = false);

    CCxema *getCxema() { return &m_cxema;};

    QString http_route(const QString & param);
    QString http_get_q(const QString & db, const QString & q, qint32 id1, qint32 id2);
    QString http_get_config();

    void drawCl(QPainter *painter, const CCoordList &cl, int dx, int dy, int cl_loc);

    void draw(QPainter *painter, double w, double h, const ScrollGeom &geom1);
    void Passport(CNode2* nl, CNode2* n1, int ms_rs, int id);
    bool isMark();

    void create_ribbon(Ribbon *ribbon);
    void savePjezo();


protected:
    void RButMenu(QPoint qpt, CFPoint pt);

    void moveNode(const CNode2* n);
    void moveLine(const CLINE2* l);
    void initInf();
    void Kam(int internalNodeID);
    void KamObr(bool redraw = true);
    bool isPodpName(const QString & name);
    void reopen(set<int> & set_fileID);
    bool setState(CLINE2 *line, int state);
    bool initCxema(CNode2* parent);
    bool initUS(const CNode2* parent);
    bool insertNode(CNode2 *n, CLINE2 *line, double r);

    CNode2 * addUS1(int fileID, int parent_id, int externalSignID, CFPoint point);
    CNode2 * addUS2(int fileID, int parent_id, int connectID, int externalSignID, CFPoint point);

    void vyd_tubing(int tubingID);

    bool delGeoObj(const QString & tn, int id, bool ask = true);

    void setMaps(int internetMap);

    bool setSomething(bool is_node, int typ, const QString & col, const QVariant &def_value, QVariant &val);
    bool setValue(int typ, const QString & col, const QVariant &val);

    bool update_view(std::map<QString, std::map<int, std::map<QString, PropertyStr> > > &map_edited);

    void outView(int node_or_line, const QString & table, const QString & title, const QString & qq, bool mark);

    void viewNode(CNode2 *n, bool autodelete = false);
    void viewLine(CLINE2 *ll, bool autodelete = false);
    void viewLineVnutr(CLINE2 *ll);
    void viewGeo(KlGeo *klgeo);

    bool MoveFragm(double dx, double dy);


public slots:

    void onCtrlF5();
    void fragmentNode();
    void fragmentLine();
    void viewNode();
    void viewKam();
    void delKam();
    void viewNodeNagr();
    void viewTG();
    void viewNodeCalc();
    void viewDR();
    void viewZN();
    void viewUP();
    void delZN();
    void delUP();
    void delText();
    void editText();
    void viewNodePodpis();
    void viewLinePodpis();
    void setLength();
    void viewLine();
    void viewLineCalc();
    void viewLinePts();
    void viewGeo();
    void delGeo();
    void ismGeo();
    void vnutrGeo();
    void markGeo();
    void createTU();

    void createAlseco();
    void addAlseco1();
    void addAlseco2();

    void naprGeo();
    void ch_type_line();
    void ch_type_node();
    void delLine();

    void moveLineGeo();
    
    void moveLine();
    void moveLine1();
    void delNode();
    void new_fragment();

    void closeLineVnutr(CLINE2 *ll);
    void closeLine();
    void closePotr();

    void activeFragmentLine();
    void activeFragmentNode();

    void vydMag();
    void vydMS();
    void vydRS();

    void viewNode2();
    void viewLine2();
    void viewGeo2();

    void addNodeMenuItem(QMenu * subMenu, CNode2 *n, const QString & text, const char* slot_name);
    void addLineMenuItem(QMenu * subMenu, CLINE2 *l, const QString & text, const char* slot_name);
    void addGeoMenuItem(QMenu * subMenu, KlGeo &kg, const QString & text, const char* slot_name);

    void onFinished(int, QProcess::ExitStatus);
    void onFinishedImport(int, QProcess::ExitStatus);
    void onFinishedPassport(int, QProcess::ExitStatus);

signals:
    void timeout();
    void updateStatusBar();


public:
    bool find_point(CFPoint pt);
    void find_point2(CFPoint pt);
    bool sfind(const CFPoint &point);


    bool readAddress(const QString & txt);
    bool readElevation(const QString & txt);

    bool moveVyd();
    bool moveCurrent();
    bool moveRect(CFRect rect);
    bool movePoint(CFPoint pt);

    bool moveVydIf();
    void googleElevation(CFPoint pt);

    int fileID() { return m_cxema.fileID();};

    bool read_wms_2(const QString & txt);
    bool read_wms_3(const QString & txt);
    bool read_wms_ask(const QString & txt);
    bool read_wmts_ask(const QString & txt);

    bool read_wms(const QString & txt);
    bool read_wmts(const QString & txt);



private slots:
    void updateUI();

public slots:
    void unknown();
    void onFindcoord(); // Поиск по координатам
    void onFindcoordDeg(); // Поиск по координатам



/*
    void view_db();
    void MasAll();
    void Plus();
    void Minus();

    void mashtab();
    void doit_dr();
    void doit();


    void find_node();
    void print();
    void pjezo();
    void klassif();
    void list_pjezo();

*/

    void geo_file();
    void find_node(const CNodeName & nodename);
    void find_line(const CNodeName & nodename1, const CNodeName & nodename2);

    void escape();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();


    void onRegim(QAction *);
    void onEdit(QAction *);
    void onEscape();
    void onKlassif();


    void onToolbarToggle(bool on);
    void onToolbarToggleExcl(bool on);
    void onToggleValue(bool on);


//----------------------------------
    void onFileClose(); // Закрыть
    void onSqlSave(); // Создание копии Базы данных
    void onSqlCopyFragment(); // Вставить Фрагмент схемы
    void onSqlDelFragment(); // Удалить Фрагмент схемы
    void onAddFragment(); // Добавить Фрагмент схемы
    void onExportFragment(); // Экспорт фрагмента
    void onExportFragmentVyd(); // Экспорт выделенного фрагмента
    void onExportFragmentsMulty(); // Экспорт всех фрагментов
    void onImportFragment(); // Импорт фрагмента
    void onImportFragmentMdb(); // Конвертор ТГИД-05/ ТГИД-07
    void onMainCxema(); // Активная схема
    void onAddDop(); // Отобразить дополнительную схему
    void onBmp(); // Файл графической подложки...
    void onBmpFind(); // Найти графическую подложку
    void onCatalog(); // Каталоги
    void onFilePrint(); // Печать...\tCtrl+P
    void onFilePrintPreview(); // Предварительный просмотр
    void onPrintFr(); // Фрагмент для печати
    void onFilePrintSetup(); // Параметры страницы...
    void onAppExit(); // Выход
    void onEditFindNode(); // Найти узел...\tCtrl+F
    void onEditFindPs(); // Найти узел ПТС

    void onPrAccepted();

    void onEditFindNodeAccepted();
    void onEditFindLineAccepted();
    void onPropertyAccepted();
    void onAnalizAccepted();
    void onPropertyRejected();

    void onResetGid(); 
    void onToGid9(); 
    

    void onEditUndoAccepted();

    void onEditUndo(); // Отменить\tCtrl+Z
    void onEditUndoTU(); // Отменить\tCtrl+Z

    void onEditUndoDel(); // Отменить удаления
    void onZapVnCx(); // Узлы с внутренней схемой
    void onZapNullGeo(); // Узлы с нулевой геодезической отметкой
    void onOdnolin(); // Однолинейные участки
    void onRaznolin(); // Участки с разными подачей и обраткой
    void onRasprMag(); // Расчетные схемы
    void onRasprMag2(); // Объекты системы теплоснабжения
    void onIstSety(); // Источник
    void onIstSetyNew(); // Источник
    void onVyd(); // Фрагмент сети
    void onBalans(); // Балансовая принадлежность
    void onTubing1(); // Канальная
    void onTubing2(); // Бесканальная
    void onTubing4(); // Надземная
    void onTubing3(); // Подвальная
    void onTubing5(); // Обвязка узлов и насосных станций
    void onFindKti(); // По коэффициенту тепловых испытаний
    void onMark(); // Выделить область
    void onSetOtv(); // Установить ФИО техников
    void onSetTr(); // Установить Код расчетных температур
    void onSetUr(); // Установить Код удельных расходов
    void onSetKvPt(); // Установить Коэффициенты вариации по потребителям
    void onSetUf(); // Установить Коэффициенты смешения элеватора
    void onSetTp(); // Установить Тепловые пункты по потребителям
    void onSetUdobVent(); // Установить Удельный объем системы вентиляции
    void onSetUdobOt(); // Установить Удельный объем системы отопления
    void onSetOpenKoef(); // Коэф. часовой неравномерности
    void onSetOpenRez(); // Расчетные тепловые потери в рециркуляц. контуре ГВС
    void onSetOpenRezT(); // Температура в рециркуляционном трубопроводе ГВС
    void onSetOpenGvsT(); // Расчетная температура горячей воды
    void onSetDiams(); // Установить Диаметр
    void onSetLosesShare(); // Установить Долю местных потерь
    void onSetKolChas(); // Установить Количество часов работы
    void onSetKvUt(); // Установить Коэффициенты вариации по участкам
    void onSetKti(); // Установить Коэффициенты тепловых испытаний
    void onSetOrg(); // Установить Организации
    void onSetPipeRemontType(); // Установить Признак ремонта
    void onSetTubingType(); // Установить Тип прокладки
    void onSetSher(); // Установить Эквивалентную шероховатость
    void onSetDate2(); // Установить Дату первичного ввода в эксплуатацию
    void onSetDate1(); // Установить Дату последней перекладки
    void onSetDate3(); // Установить Дату планируемого ремонта
    void onSetKodRs(); // Установить Код расчетной схемы
    void onGeodz(); // Установить Отметки узлов горизонтали
    void onSetKorrozia(); // Установить Индикаторы коррозии
    void onSetPodpOn(); // Показать Надписи
    void onSetPodpOff(); // Не отображать Надписи
    void onFragmentPoly(); // Выделить многоугольный фрагмент
    void onMoveVyd(); // Передвинуть фрагмент
    void onFragment(); // Выделить фрагмент

    void onBdDel(); // Удалить базу
    
    void onEditCopy(); // Копировать\tCtrl+C
    void onEditCopyPng(); // Копировать в PNG
    void onViewToolbarMain(bool on); // Главная
//    void onViewToolbarCxema(bool on); // Схема
//    void onViewToolbarZ(bool on); // Запросы
//    void onViewToolbar2PtsNew(); // Паспортизация
//    void onTuOnOff(); // Технические условия
//    void onViewToolbarElectro(); // Электрические сети
//    void onViewToolbarControlTu(); // Контроль технического состояния
//    void onViewToolbar2Remont(); // Ремонт
//    void onViewToolbarOpressovka(); // Опрессовки
//    void onViewToolbarKorrozia(); // Индикаторы коррозии
    void onNavigator(bool on); // Навигатор
    void onLeftBar(bool on); // Проводник карты
    void onMagAlma(bool on); // Проводник структуры сети
    void onBottomRemont(bool on); // Проводник мониторинга
    void onProtocol(bool on); // Протокол
    void onMovew(); // Перемещение\tM
    void onPovorot(); // Повернуть
    void onPovorot2(); // Возврат
    void onMasall(); // Отобразить всю схему\tF3
    void onPlus(); // Увеличить\t+
    void onMinus(); // Уменьшить\t-
    void onMasshtab(); // Масштаб...\tCtrl+F3
    void onZoom(); // Масштабировать
    void onPicture(); // Графическая подложка
//    void onF5(); // Перерисовать схему\tF5
    void onGeobaza(bool on); // Показать геобазу
    void onFindGeo(); // Поиск в геобазе...
    void onGeoFindNext(); // Продолжение поиска
    void onFindGeoAddr(); // Поиск по адресу
    void onGeoInfo(); // Информация из геобазы
    void onSetLength(); // Установить длины
    void on1000(); // Создать файл полей
    void on1001(); // Переименовать поля
    void on1002(); // Сохранить переименованные поля
//    void onNode(); // Установить узел\tN
//    void onLine(); // Линейный объект\tL
//    void onRename(); // Изменить тип объекта
//    void onZn(); // Узел с заданным напором
//    void onKam(); // Внутренняя схема
//    void onNodeNeotr(); // Установка конвертированных узлов\tC
    void onZapNezak(); // Незаконченные узлы
    void onSetCoordNull(); // Удалить координаты выделенных узлов
    void onNodeMove(); // Перенести\tV
    void onDel(); // Удалить объект\tD
    void onOpenClose(); // Закрыть/открыть объект
    void onNodeMove1(); // Разъединить линии
    void onSwap(); // Изменить направление потока
    void onMoveLineEnd(); // Перенести линию
    void onText(); // Установить текст
    void onDoItDr(); // Плановый...
    void onDoIt(); // Фактический...
    void onDoItListDr(); // Плановый по списку...
    void onDoItList(); // Фактический по списку...
    void onMagFragment(); // Магистральный фрагмент
    void onNapr(); // Выбpать напpавление
    void onSavePjezo(); // Сохранить направление...
    void onListPjezo(); // Список направлений...
    void onClearOut(); // Удалить старые расчеты
    void onInfo(); // Свойства объекта\tI
    void onViewDb(); // Таблицы...
    void onNovisual(); // Системные параметры...
    void onMoveTo(); // Таблица объекта
    void onInfoGid(); // Результат расчета
    void onOut2(); // Отчеты...
    void onTableDan(); // Редактор таблиц...
    void onExcel2(); // Таблицы Excel...
    void onInf(); // Отображаемая информация...
    void onGidinf(bool on); // Отображать информацию
    void onPodp(bool on); // Отображать имена узлов
    void onPodpPo(); // Отоброжать схему ПТС
    void onPodpRes(bool on); // Отображать имена узлов с #
    void onPribor(); // Отображать Приборы учета
    void onRezhim(bool on); // Отображать вспомогательные участки
    void onPopupPodpOn(); // Показать скрытые имена
    void onPopupPodpOff(); // Не показывать скрытые имена
    void onPodpYesNo(bool on); // Отображать/Не отображать надпись
    void onNaprGid(); // Распределение потоков
    void onF4(); // Измерить расстояние\tF4
    void onAnal(); // Анализ...
    void onAdmissibilityAnalysis(); // Анализ...
    void onZaprosy(); // 
    void onZaprosy2(); // 
    void onRunProgram(); // 
    void onAnalVyd(); // Анализ
    void onColorLine(); // Участки
    void onColorLine1(); // Потребители

    // Цветовое выделение узлов. Один цвет для многих id
    void applyNodeColor(const QVector<int>& ids, int color);

    // Цветовое выделение узлов. Свой цвет для каждого id
    void applyNodeColors(const QHash<int, int>& idToColor);

    void onZap1(); // Объем сети
    void onZap2(); // Длина теплопроводов
    void onZap7(); // Длина теплопроводов по диаметрам
    void onZap71(); // Длина теплопроводов по диаметрам и способам прокладки
    void onTimePr(); // Время прохождения
    void onPoteri(); // Тепловые потери
    void onZap3(); // Теплопотребление полученное
    void onZap4(); // Теплопотребление в закрытых системах полученное
    void onZap5(); // Теплопотребление в открытых системах полученное
    void onZap6(); // Закрытые потребители
    void onPotNagr0(); // С нулевой нагрузкой
    void onPotrOtkl(); // Отключенные потребители
    void onUtZakr(); // Закрытые участки
    void onUtZakrAll(); // Отключенные участки
    void onZapOtr(); // Отрицательные перепады
    void onPtTempMin(); // Низкие температуры
    void onZavozd(); // Завоздушивание
    void onIsprLast(); // Изменённые объекты
    void onPjezo(); // Пьезометр
    void onZona(); // Гидростатические зоны
    void onNagrOrg(); // Юридические лица
    void onTuTable(); // Технические условия
    void onTuYear(); // ТУ по годам
    void onTuSost(); // ТУ состояние
    void onTuExcel(); // Итоговый тепловой баланс
    void onTuSvod(); // Итоговые по годам
    void onTuZhurnal(); // Журнал регистрации ТУ
    void onFindTuIst(); // ТУ по источнику
    void onTuFind(); // ТУ, Договора
    void onPrisNagrEdit(); // Присоединенная нагрузка по источникам
    void onQGvs(); // Коэффициент перевода k=Qгвmax/Qгвср
    void onNagrZd(); // 
    void onNagrZdNeiz(); // Физические лица
    void onNagrOrgNeiz(); // Юридические лица
    void onTuTableNeiz(); // Ненайденные ТУ на карте
    void onTuIst(); // Отчет по источникам

    void onPtsAdd();  // Установить объект ПТС
    void onPtsInfo(); // Информация по объекту
    void onPtsTable(); // Таблица
    void onPtsDel(); // Удалить объект
    void onProtPass(); // Протяженность
    void onProtNew(); // Протяженность выделенных трубопроводов
    void onProtMag(); // Протяженность магистралей
    void onProtRs(); // Протяженность распредсетей
    void onProtKot(); // Протяженность котельных
    void onPaspNew(); // Паспорт сети

//-- Ремонты

/*
    void onToolbarRemont2Defect(bool on); // Панель Нарушений
    void onToolbarRemont2Shurf(bool on); // Панель Шурфовки
    void onToolbarRemont2Osmotr(bool on); // Панель Осмотра
    void onToolbarRemont2Remont(bool on); // Панель Ремонтов
    void onToolbarRemont2Opres(bool on); // Панель Опрессовки
*/
    void onRemontPovrDefAdd(); // Установить нарушение
    void onDefectZhurnalExpluatacia(); // Нарушения эксплуатации
    void onDefectZhurnalShurf(); // Нарушения в шурфе
    void onDefectZhurnalOsmotr(); // Нарушения осмотра
    void onDefectZhurnalOpress(); // Нарушения опрессовки
    void onDefectZhurnalNarushenie(); // Нарушения до ремонта
    void onDefectZhurnalRemont(); // Нарушения в процессе ремонта
    void onDefectZhurnalPosleRemont(); // Нарушения после ремонта
    void onDefectZhurnalAll(); // Отобразить все нарушения
    void onDefectWord1(); // Журнал нарушений
    void onDefectWord2(); // Нарушения/ремонты трубопроводов тепловой сети(документ)
    void onDefectDocTable(); // Нарушения/ремонты трубопроводов тепловой сети
    void onDefectAnaliz(); // Анализ нарушений
    void onRemontPovrDefOtobr(); // Включить/отключить отображение объектов
    void onRemontPovrOtop(); // Выбор отопительного сезона
    void onRemontOsmotrTeploSet(); // Выбор фрагмента сети по Начальнику участка
    void onDefectBezUch(); // Нарушения без участков
    void onRemontPovrShurfAdd(); // Установить плановый шурф
    void onRemontPovrShurfAddNeplan(); // Установить НЕплановый шурф
    void onRemontShurfPlan(); // Плановые шурфы
    void onRemontShurfPredpis(); // Шурфы предписания
    void onRemontShurfNarush(); // Шурфы нарушения
    void onShurfProcess(); // Шурфы в процессе выполнения
    void onShurfVypolneno(); // Шурфы выполненные
    void onShurfAll(); // Отобразить все Шурфы
    void onRemontShurfPlanUtverdit(); // Утвердить План шурфов Отопительного сезона
    void onRemontShurfPlanExcel(); // План шурфов (документ)
    void onRemontShurfPlanExcelMonth(); // План шурфов по месяцам (документ)
    void onShurfTable(); // Анализ Плана шурфов
    void onRemontShurfOtobr(); // Включить/отключить отображение объектов
//    void onRemontPovrOtop(); // Выбор отопительного сезона
//    void onRemontOsmotrTeploSet(); // Выбор фрагмента сети по Начальнику участка
    void onShurfBezUch(); // Шурфовки без участков
    void onOsmotrAdd(); // Создать контур осмотра
    void onOsmotrSave(); // Обновить сохранение контура
    void onListOsmotr2(); // Выполнить осмотр контура
    void onLastOsmotr2(); // Последний контур осмотра
    void onLastFaktory(); // Выбор трубопроводов для осмотра
    void onOsmotrZapros1(); // Осмотренные фрагменты сети
    void onOsmotrZapros2(); // Осмотренные трубопроводы за период
    void onOsmotrZapros3(); // Неосмотренные трубопроводы за период
    void onOsmotrAnaliz2(); // Анализ осмотра контуров
    void onOsmotrAnaliz(); // Анализ осмотра трубопроводов контура
//    void onRemontPovrDefOtobr(); // Включить/отключить отображение объектов
//    void onRemontPovrOtop(); // Выбор отопительного сезона
//    void onRemontOsmotrTeploSet(); // Выбор фрагмента сети по Начальнику участка
    void onRemontAddPlan(); // Сохранить контур плана ремонта
    void onRemontAddCurrent(); // Сохранить контур текущего ремонта
    void onRemontSave(); // Обновить сохранение контура
    void onListRemont2(); // Список контуров ремонтов
    void onLastRemont2(); // Последний контур ремонта
    void onRemontTrub(); // Выбор трубопроводов контура ремонта
    void onRemontPlan(); // Контуры капитального/инвестиционного ремонтов
    void onRemontCurrent(); // Контуры текущих ремонтов
    void onRemontProcess(); // Ремонт в процессе выполнения
    void onRemontVypolneno(); // Законченные ремонты
    void onRemontAll2(); // Отобразить все контура
    void onRemontWord1(); // График выполнения ремонтов (документ)
    void onRemontWord2(); // План капитальных/инвестиционных ремонтов (документ)
    void onRemontWord3(); // План ремонтов по месяцам (документ)
    void onRemontAnaliz2(); // Анализ контуров ремонта
    void onRemontAnaliz(); // График выполнения ремонтов
//    void onRemontPovrDefOtobr(); // Включить/отключить отображение объектов
//    void onRemontPovrOtop(); // Выбор отопительного сезона
//    void onRemontOsmotrTeploSet(); // Выбор фрагмента сети по Начальнику участка
    void onRemontVyborNach(); // Создать контур ремонта
    void onMarkRev(); // Добавить/удалить трубопровод к контуру
    void onOpresAddPlan(); // Сохранить контур плана опрессовки
    void onOpresSave(); // Обновить сохранение контура
    void onOpresGranitsaRazdela(); // Границы раздела
    void onListOpres(); // Список контуров опрессовок
    void onLastOpres(); // Последний контур опрессовки
    void onOpresPlan(); // Контуры планов опрессовок сезона
    void onOpresProcess(); // Опрессовка в процессе выполнения
    void onOpresVypolneno(); // Завершенные опрессовки
    void onOpresAll2(); // Отобразить все контура
    void onOpresWord1(); // План опрессовок (документ)
    void onOpresWord2(); // График опрессовок по месяцам (документ)
    void onOpresAnaliz2(); // Анализ контуров опрессовок
    void onOpresAnaliz(); // Анализ нарушений контура
//    void onRemontPovrDefOtobr(); // Включить/отключить отображение объектов
//    void onRemontPovrOtop(); // Выбор отопительного сезона
//    void onRemontOsmotrTeploSet(); // Выбор фрагмента сети по Начальнику участка
    void onOpresVyborNach(); // Создать контур опрессовки
//    void onListRemont2(); // Список контуров ремонтов
//    void onMarkRev(); // Добавить/удалить трубопровод к контуру
    void onRemontPovrKarta(); // Карта нарушений
    void onRemontPovrKarta2(); // Карта осмотра шурфа


//----------



    void onViewToolbarRemont2(); // Панель Контуры ремонта
    void onNaprRemont2(); // Создать Контур ремонта...
//  void onListRemont2(); // Выбрать Контур ремонта...
    void onNaprTableRemont2(); // Таблица участков Контура ремонта...
    void onSaveRemont2(); // Сохранить текущий Контур ремонта...
    void onSaveRemont2New(); // Сохранить новый Контур ремонта...
    void onRemontsInfo(); // Информация по объекту
    void onRemontsTable(); // Таблица
    void onRemontsDel(); // Удалить объект
    void onZhurnalDefect(); // Повреждения
    void onZhurnalDiag(); // Диагностика
    void onZhurnalRemont(); // Ремонты
    void onZhurnalExcel(); // Excel
    void onNaprOpres(); // Создать Контур опрессовок...
//    void onListOpres(); // Выбрать Контур опрессовок...
    void onRemontsAddOpres(); // Создать Повреждение опрессовок...
    void onZhurnalDefectOpres(); // Журнал Повреждений опрессовок...
    void onNaprTableOpres(); // Таблица участков Контура опрессовок...
    void onSaveOpres(); // Сохранить текущий Контур опрессовок...
    void onSaveOpresNew(); // Сохранить новый Контур опрессовок...
    void onKorrozAdd(); // Установить индикатор коррозии
    void onKorrozDel(); // Удалить индикатор коррозии
    void onFragmentPolyKorroziaAr(); // Удалить выбранные индикаторы
    void onKorrozInfo(); // Информация по индикатору
    void onKorroziaUpdate(); // Выбор шаблона плана
    void onKorrozDoc3(); // План установки
    void onKorrozTable(); // Таблица индикаторов коррозии
    void onKorroziaZhurnal1(); // План индикаторов
    void onKorroziaZhurnal2(); // Индикаторы установленные 
    void onKorroziaZhurnal3(); // Индикаторы извлеченные
    void onKorroziaCurrent(); // Все индикаторы сезона
    void onKorrozDoc2(); // Оценка за отопительный сезон
    void onKorrozDoc1(); // Оценка за выбранные сезоны
    void onPassportOnOff(); // Включить/отключить отображение объектов ПТС
    void onKorroziaOnoff(); // Включить/отключить отображение
    void onKorroziaSezon(); // Выбор отопительного сезона
    void onIznos(); // Износ оборудования
//    void onRemontPlan(); // Планирование ремонтов
    void onElectroAdd(); // Установить объект
    void onElectroInfo(); // Информация по объекту
    void onElectroTable(); // Таблица
    void onElectroDel(); // Удалить объект
    void onZhurnalElectro(); // Журнал
    void onTeplopoteri(); // Теплопотери
    void onColors(); // Стиль линии
    void onFon(); // Цвет фона
//    void onFont2(); // Шрифт текста
    void onFont(); // Шрифт надписей
    void onFontMag(); // Шрифт названия магистрали
    void onFontPanel(); // Шрифт в проводнике
    void onTransp(); // Прозрачность
    void onAstr(bool on); // Сетка
    void onRamka(bool on); // Рамка схемы
    void onIsMas(bool on); // Сохранять пропорции

    void onSetupGid9();

    void onMasPic(); // Номинальный масштаб
    void onMasPodpis(); // Масштаб откл.надписей
    void onTrio(); // Передвинуть
    void onHttp2(); // Веб-сервер
    void onQuery(); // Выполнить запрос
    void onExportPts(); // Экспорт фрагмента ПТС
    void onImportPts(); // Импорт фрагмента ПТС
    void onSetAvtoOn(); // Сделать потребители автоматическими
    void onSetAvtoOff(); // Сделать потребители неавтоматическими
    void onCreateSortNode(); // Создание таблицы sortNodesForUchastok
    void onRasList(); // Список расчетов
    void onSetPsMap(); // Установить объекты ПС
    void onSavePng(); // Сохранить как PNG
    void onUtNapr2(); // Сортировка участков
    void onInvert(); // Инвертировать выделение
    void onSetAddr(); // Адреса потребителей
    void onAlma(); // Паспортизация
    void onTuZav(); // Установить статус Завершен для ТУ
    void onExport(); // Экспорт гидравлики в TXT
    void onSetLineid(); // !
    void onSetIst(); // Расстановка источников
    void onUchList(); // Список участков ПТС
    void onAsyncCheck(); // Проверка Async Task
    void onCheckPo(); // Проверка
    void onResetGeo(); // Обновить надписи
    void onPsAstanaName(); // Отображение имен узлов
    void onColorOnlyPts(); // Цвета только для участков ПТС
    void onExportDxf(); // Автокад DXF
    void onExportShape(); // ESRI shape-файлы
    void onPrPo(); // Реальные -> обобщенные
    void onUtNapr(); // Сортировка участков
    void onC3Del(); // Удалить внутренние схемы
    void onC3Add(); // Создать внутренние схемы
    void onIspravl(); // Восстановить файл
    void onOpc(); // Настройка OPC сервера
//    void onNomap(); // Нет карты
//    void onGoogleMap(); // Карта
//    void onGoogleSat(); // Спутник
//    void onGoogleHybrid(); // Гибридная
//    void onVisicomMap(); // Карта VISICOM
//    void onWmsMap(); // Карта WMS
    void onWmsCust(); // Настройка
//    void onMapSemey(); // Семей (город)
//    void onMapSemeyTs(); // Семей (тепловые сети)
//    void onOpenstreetmap(); // OpenStreetMap
//    void on2gisMap(); // 2ГИС
    void onGoogleElevation(); // Высота по Google
    void onGoogleElevationSet(); // Высота по Google
    void onMapSearch(); // Поиск
    void onMapPath(); // Каталог
    void onMapYes(bool on); // Восстанавливать подключение
    void onFileOpenOld(); // Импорт
    void onAddUser(); // Регистрация пользователей
    void onPassword(); // Изменить доступ пользователя
    void onPassword2(); // Изменить пароль
    void onViewStatusBar(); // Строка статуса
    void onHelpFinder(); // Содержание\tF1
    void onUpdate(); // Последнее обновление
    void onIndexes(); // Обновление индексов
    void onUpdateSetup(); // Настройка обновлений
//    void onAppAbout(); // О программе


    void onSelect6();   // "Выделить",  
    void onExport6();   // "Экспорт",   
    void onRezhim6();   // "Режим",     
    void onSetup6 ();   // "Настройка", 
    void onFile6 ();   // "Файл", 
    void onGeobaza6 ();   // "Геобаза", 
    void onFind6 ();   // 

void onNagrAlmatyTable(); // Объекты АЛСЕКО
void onNagrAlmatyAdd1(); // Добавить Объект (МЖД) АЛСЕКО
void onNagrAlmatyAdd2(); // Добавить Объект (Прочие) АЛСЕКО
void onNagrAlmaty(); // Объекты АЛСЕКО по адресам
void onNagrAlmatyOnOff(); // Вывести нагрузки АЛСЕКО
void onNagrAlmaXlsx(); // Реестр объектов АЛСЕКО
void onNagrAlmaNenaid1(); // Реестр ненайденных объектов (МЖД) АЛСЕКО
void onNagrAlmaNenaid2(); // Реестр ненайденных объектов (Прочие) АЛСЕКО
void onNagrAlmaNenaid3(); // Здания АЛСЕКО без потребителя



//----------------------------------

//----------------------------------
// Проверить, наверное нужно удалить

void onProtocol();
void onElectroIst();
void onElectroPriem();
void onElectroLine();
void onElectroMufta();
void onElectroOpora();
void onElectroGil();
void onElectroKanal();
void onElectroZhurnal();
void onElectroOnoff();
void onSetMsNew();
void onSetRsNew();
void onRasprMagNew();
void onMagNode();
void onNapr2();
void onVyd4();
void onMarkDel();
void onMsNode1();
void onMsNode2();
void onSaveMAG();
void onSaveMS();
void onSaveRS();
void onMsPassport();
void onAktCreate();
void onTuNew();
void onPtsZhurnal();





//----------------------------------

    void init_print_param(const CFPoint & m_pt1, const CFPoint & m_pt2);
    void printPage(QPrinter *printer, QPainter *painter, int page);
    void printPages(QPrinter *printer);
    void printPreview(QPrinter *printer);
    void onMaps(QAction *action);

    void setGidrInf(bool b) {m_bIsGidrInf = b; repaint(); }
    void setAstr(bool b) { 
      m_dX = b ? 10*20 : 0; repaint();
    }
    void isGeo(bool b) { m_bIsGeo = b; repaint();}

protected:
    
    void closeEvent(QCloseEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent ( QMouseEvent * event );
    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);
    void wheelEvent ( QWheelEvent * event );

    void DrawPolygon2(QPainter *painter, const CCoordList& cl, int loc);
    void DrawPolygon3(QPainter *painter, const CCoordList& cl);
    void DrawPolygon(QPainter *painter, const CCoordList & cl, int loc, int typ);
    void drawObject(QPainter *painter, const CGeoObject *gl, Klassif *kls, bool redraw);
    void drawGeo(QPainter *painter, bool is_loc1);
    void drawGeoKl(QPainter *painter, Klassif &kl);
    void draw(QPainter *painter);
    void drawBorder(QPainter *painter);
    bool redrawMap(QPainter* dc, CFRect& m_rect, int typ_map, bool is_reread);
    int drawMap(QPainter* dc, CFRect& m_rect, int typ_map, int nn);
    void deleteMap(CFRect& m_rect, int map_typ, int nn);
    void deleteMapAll(CFRect& m_rect, int map_typ);
    int maxNmap(int map_typ);
    bool addNode(CFPoint point);
    bool addLine(CNode2 *n1, CNode2 *n2, CCoordList &cl);

    bool ch_type_node(CNode2 *n);
    bool ch_type_node2(CNode2 *node, int typ);
    bool ch_type_line(CLINE2 *l);

//bool vydel4(CNode2* node, CLINE2* line);
//bool vydel(CNode2* node);

private:
    void viewZN(CNode2 *n);
    void closeLine(CLINE2 *ll);
    void closePotr(CNode2 *n);
    
    void hideEvent(QHideEvent *event);

//    void createActions();
    void createMenus();
    void createToolBars();

    double okr(double x, int dx);

    bool isEditGid();
    bool isPot() {return false;};
    void SetModifiedFlag(bool) {}; 

    bool openGeo(const QString & fn);

    bool del(CFPoint point){ return true; };
    bool info_gid(CFPoint point);
    bool zd(CFPoint point){ return true; };
    bool neotr_node(CNode2 *node, CFPoint point);
    bool ism(CLINE2 *line){ return true; };
    bool edit(const char *fn, int n){ return true; };

    void beginDraw(const CNode2 *node);
    void beginDraw(const CLINE2 *Line);
    void endDraw();

    QString str_python(const QString & command, const QString & args, bool conn = true);
    bool run_python(const QString & command, const QString & args);


    void draw_pr(QPainter *painter, const QString & cxema, int w, int h);
    void draw(QPainter *painter, CCxema *cxema);
    void drawLine1(QPainter *painter, const CLINE2* ll, int redr, bool is_text);
    void drawLine2(QPainter *painter, const CLINE2 *l, int ris, double xx1, double yy1, double xx2, double yy2, char podobr, int redr, bool is_text, int t);
    void drawLine(QPainter *painter, const CLINE2 *ll, int redr, bool is_text);
    void drawLine(QPainter *painter, const CLINE2 *ll, int ris, double xx1, double yy1, double xx2, double yy2, int redr, bool is_text, int t);
    void drawNodeNull(QPainter *painter, double x, double y, QColor color);
    void drawNode(QPainter *painter, const CNode2 *node, int redr, bool is_text);
    void drawNode0(QPainter *painter, const CNode2 *node, int redr);
    void drawPodpNode(QPainter *painter, CNode2 *node);
    void drawPoints(QPainter *painter);
    void uch_pod(QPainter *painter, const CLINE2 *ll, double dx1, double dy1, double dx2, double dy2);

//    void drawLineTyp(QPainter *painter, GID_STYLE style, double x1, double y1, double x2, double y2, int typ, QColor color, QColor colorF, int ris, bool napr, bool strelka, bool isQ);
    void drawNodeTyp(QPainter *painter, double x, double y, int typ, QColor color,
                     double markerPx = -1.0);

    void drawLineTyp(QPainter *painter, double thickness, double x1, double y1, double x2, double y2, int typ, QColor color, QColor colorT, QColor colorF, int ris, bool punct, bool napr, bool strelka, bool isQ, bool no_map) ;
    void drawLineTypL(QPainter *painter, CLine2* l, int po, int redr, double thickness, double x1, double y1, double x2, double y2, int typ, QColor color, QColor colorT, QColor colorF, int ris, bool punct, bool napr, bool strelka, bool isQ);
    void drawLine00(QPainter *painter, const CLINE2* ll, int ris, double x1, double y1, double x2, double y2, char podobr, int redr, int t, int typ, int vyd);
//    void drawLinePic(QPainter *painter, int thickness, double x1, double y1, double x2, double y2, QColor color, QColor colorF, QColor bk_color,  const int *pics, double mas_otn, bool no_mas);

    void drawLinePodp(QPainter* painter, const CLINE2* ll, double dx1, double dy1, double dx2, double dy2);


    void DrawLabel3(QPainter* painter, const CCoordList& cl, const QString & text, double x, double y, double alpha, double coef, int loc, const Klassif *m_kls);

    void DrawLabel2(QPainter* painter, const CCoordList& cl, const QString & text, double x, double y, double alpha, double coef, int loc, const Klassif *m_kls);
    
    /*
    void DrawLabel3_MI(QPainter* painter, const CCoordList& cl, const QString & text, double x, double y, double alpha, double coef, int loc, const Klassif *m_kls);
    void DrawLabel2(QPainter* painter, CPoint pt, const QString & text, short align, double x, double y, double alpha, double coef, int loc, const Klassif *m_kls);
    void DrawLabel2(QPainter* painter, const CCoordList& cl, const QString & text, short align, double x, double y, double alpha, double coef, const Klassif *m_kls);
    void DrawLabel22(QPainter* painter, const CCoordList& cl, const QString & text, short align, double x, double y, double alpha, double coef, const Klassif *m_kls);
    void DrawLabel2(QPainter* painter, const CCoordList& cl, const QString & text);

    void DrawLabel1(QPainter* painter, const CCoordList& cl);
    void DrawLabel1_MI(QPainter* painter, const CCoordList& cl, Klassif* m_kls2);
    void DrawLabel2(QPainter* painter, const CCoordList& cl);
    void DrawLabel2(QPainter* painter, CPoint pt, const CCoordList& cl, const QString & s);
    void DrawLabel3(QPainter* painter, const CCoordList& cl, Klassif* m_kls2);
*/

    inline bool setNodeColorById(int id, int color);

    bool openGid(const QString & baza);
    bool openGidGeo(int rdbms, const QString & host, int port, const QString & geo_name, const QString & user, const QString & password);


    void barOnOff(bool on);

    bool delNode(CNode2 *node, bool ask = true);
    bool delLine(CLINE2 *line, bool ask = true);
    bool delNode2(CNode2 *node);
    bool setXY(CNode2 *n, CFPoint &pt);
    bool setCoord(CLINE2 *line, CCoordList &cl);
    bool setCoord1(CLINE2 *line, CCoordList &cl);
    bool swap(CLINE2 *line);

    void info(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, PrTypes pr_type, const QString & s_shape = "shape", const QString & s_id = "id", bool autodelete = false);
    void info_with_line(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, const QString & table2, int id1_2, int id2_2, PrTypes pr_type, const QString & s_shape = "shape", const QString & s_id = "id", bool autodelete = false);
//    void info_gid(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, PrTypes pr_type);
    void info_gid(QSqlDatabase &db, const QString & title, const QString & table, const QString & out_table, int id1, int id2, PrTypes pr_type);
    void viewGeo(Klassif * kls, int id);

    void viewToolbar(bool on, QToolBar *m_bar);

    void DrawRect(CFPoint pt, int w = 3, int h = 3);
    void DrawLine();
    void SetCur(QAction *regim);

    void drawMark(QPainter *painter);

    void SetMas(double m, double bx0, double by0);
    void ChMas(CFRect rect);
    void cl2QPolygon(const CCoordList &cl, QPolygon &pg);
    void XDraw(QPainter *painter, double dx, double dy);

    bool IsMain() { return m_parent_id == 0; }

    bool moveLineEnd(CNode2 *n1, CNode2 *node, CLINE2 *line);


    void zhurnal(int idd, const QString & fn, const QString & tn, const QString & title, const map<QString, QString>* map_fn);
    void remontZhurnal(int idd, const QString & fn, const QString & tn, const QString & title, const QString & state_cond);
    void remontZhurnalShurf(int idd, const QString & fn, const QString & tn, const QString & title, const QString & state_cond);
    void ispr_remont_q(std::map<QString, QString> &map_fn);

    void geoOnOffFile(const QString & file_name, const QString & title);
    void geoOnOff(const map<QString, Klassif*> *map_pts, const QString & title);

    void ListOpres(const QString & opres, const QString & title, const QString & name_name, const QString & date_name, int& m_id_opr);
    void LastOpres(const QString & opres, const QString & title, int& m_id_opr, int& m_id_opr_old, bool view_dialog);

    void set_geo();
    void set_geo_line();

    QString get_Layer(const QString & r);
    bool read_ini(const QString & ini);

    bool isRezhim(const CNode2* n);
    bool isRezhim(const CLINE2* l);

    int create_geo(Klassif *kls, CCoordList & cl, std::map<QString, QVariant> &map_value);
    bool create_geo_line(Klassif *kls, CCoordList & cl);

    void saveFlags();
    void restoreFlags();

    int import_tgid0(const QString & fn);

    bool openFragment(int fileID);
    bool closeFragment(int fileID);
    bool reopenFragments(const QString & par1, const QString & par2);

    bool isPR();

//    void initActionValue();


    bool pts_del(const CFPoint &point, const std::map<QString, Klassif*> *map_pts);
    bool pts_move(const CFPoint &point, const std::map<QString, Klassif*> *map_pts);
    bool pts_info(const CFPoint &point, const std::map<QString, Klassif*> *map_pts);

    CNode2 *m_napr1 = nullptr;
    CNode2 *m_n1 = nullptr, *m_n2 = nullptr;
    CNode2 *m_node_for_move = nullptr;
    CLINE2 *m_line_for_move = nullptr;

    bool is_move1 = false; // Показывает, что мы хотим разъединить линию

    KlGeo m_geo_for_move;

    CCoordList m_cl_for_move;
    int m_n_for_move;

    CLINE2 *m_line = nullptr;
    CNode2 *m_CurNode = nullptr;  // Выделенный узел
    CLINE2 *m_CurLine = nullptr;  // Выделенный участок
    CGeoObject *m_CurGeoObject = nullptr;
    Klassif *m_CurKlassif = nullptr;

    bool m_tic;

    CNode2 *m_parent_kam = nullptr;


    CCxema m_cxema;
    GeoFile m_geo;

//    GeoMap m_gmap;

    Klassif *m_kls = nullptr;

    QAction *m_nRegim = nullptr;
    QAction *m_nMenu = nullptr;
    int m_idMenu = 1;
    int regim;
    int oldregim;

//    bool m_bIsInfo[20];
    QSet<int> m_Info;

    bool inInfo(int id);

    bool m_isOpen;
    QString m_fileName;

    int m_parent_id;
    int m_parent_id_old;
    int m_vnutr_typ = 1; 

    int m_fileID;

    QAction *m_internetMapAction;

    list<KlGeo> geo_list;

    cdxf* m_dxf = nullptr;

    void createActions();
    void createMenu();
    void createToolBar(MainWindow *main_window);

    void beginMove(CFPoint pt);
    bool DR_otkr(CLine2* l);
    bool ZD_otkr(CLine2* l);

    void viewOpresDialog(const QString & opres, int m_id_opr, bool autodelete);
    void viewOpresProtokol(const QString & opres, int &m_opr_id);


public:

    void createShortcuts();
    
    void setOpres(const QString & opres, int id);
    QSizeF draw_b5(QPainter *painter, const QString &cx);
    QSizeF draw_example_style_line(QPainter *painter, int mag_ras, int tubingTypeID, bool open, bool q, double ring);
    int m_id_opr = -1;
    int m_id_osmotr2 = -1;

    int m_id_opr_old = -1;
    int m_id_remont2 = -1;
    int m_id_remont2_old = -1;
    int m_id_osvidet2 = -1;
    int m_id_osvidet2_old = -1;

    int m_id_osmotr2_old = -1;



public:
    bool addGeo(Klassif* kls, std::map<QString, QVariant> *m_map_value = nullptr);

public:
    GidrAction gidrAction;

private:
    DownloadManager download_manager;
    DownloadManager2 download_manager2;

//    QMenu *fileMenu;
//    QMenu *editMenu;
//    QMenu *selectSubMenu;
//    QMenu *toolsMenu;
//    QMenu *optionsMenu;
//    QMenu *helpMenu;

    set<int> m_set_fileID; // фрагменты для расчета


private:
    std::map<QString, QVariant> m_map_value;

private: // Всякие флаги

    int m_sezon_korrozia = -1;
    CLINE2 *m_l_canal = nullptr;
    CFPoint m_pt1_canal;
    CFPoint m_pt2_canal;

    int m_wi = 0;
    int m_hi = 0;


    bool m_bIsAstr = false;
    bool m_bIsGeo = true;
    bool m_bIsGidrInf = false;
    bool m_bIsMapYes = true;
    bool m_bIsNaprGid = true;
    bool m_bIsPodp = true;
    bool m_bIsPodpAll = false;
    bool m_bIsPodpRes = false;
    bool m_bIsPts = false;
    bool m_bIsRamka = true;
    bool m_bIsRezhim = false;

    int m_nTransp = 100;

    int m_internetMap = ID_NOMAP;

    QToolBar *m_barGid = nullptr;

    QToolBar *m_barCxema = nullptr;
    QToolBar *m_barZapr = nullptr;

    QToolBar *m_barDefect = nullptr; // Панель Нарушений
    QToolBar *m_barShurf = nullptr;  // Панель Шурфовки
    QToolBar *m_barOsmotr = nullptr; // Панель Осмотра
    QToolBar *m_barRemont = nullptr; // Панель Ремонтов
    QToolBar *m_barOpres = nullptr;  // Панель Опрессовки

    QToolBar *m_barElectro = nullptr;  // Панель Электричество
    QToolBar *m_barKorrozia = nullptr;  // Панель Коррозии
    QToolBar *m_barPts = nullptr;  // Панель ПТС

    QToolBar *m_barTu = nullptr;  // Технические условия

    QString passport_file_name = "";

    int m_user = 1;
    QString m_strUser = "";
    QString m_password = "";

    // Для WMS

//    QString rovno = "http://45.132.85.23:8085/geoserver/tgid/wms";
    QString rovno = "";

    void setWMS(const QString & _rovno, const QString & _layer);

    double m_print_mas = 500.;
    QSize m_print_size = QSize(210, 297);
    bool m_bIsFragment = false;
    bool m_bIsPolyFragment = false;
    CCoordList poly_fragment;

    double m_ring = 1.0;

    bool m_is_print_fr = false;
    CFPoint m_pt1_1;
    CFPoint m_pt1_2;

    CFPoint m_print_1;
    CFPoint m_print_2;


    QDateTime m_sezon_date1;
    QDateTime m_sezon_date2;

    QString m_wms = "";
    std::list<Layer> m_layers; // "";


//    std::map<QAction *> list_action;

};
