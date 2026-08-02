#include <QtGui>

#include <QtSql>
#include <QtWidgets>
#include <QVariant>
//#include <QtPrinter>
#include <cmath>
#include <QRegularExpression>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QMessageBox>

#include <QInputDialog>


#include "std.h"

#include "mainwindow.h"
#include <gidview/GidWidget.h>
#include <cxema/graph2.h>

//#include <cxema/colorgtd.h>

#include <table/DbWindow.h>

#include <edit/menu.h>
#include <edit/scooterwindow.h>

#include <dialog/ChMenuDial.h>

#include <cxema/undo.h>

#include <ui/PercentDialog.h>
#include <geo/geofile.h>
#include <db/db.h>

#include "colorgtd.h"

//#if USE_ITWIN
//#include "version.h"
//#else
//#include "version_tgid.h"
//#endif

#include "version0.h"

QString getOnlygeo(const QString & tn);
bool isOnlygeo(const QString & tn);

//void findXY(CFPoint p, double md, list<KlGeo> & geo_list);
//void findXY(CFPoint p, double md, Klassif *kls, list<KlGeo> & geo_list);
//void findXY(CFPoint p, double md, list<KlGeo> & geo_list);


int GidWidget::create_geo(Klassif *kls, CCoordList & cl, std::map<QString, QVariant> &map_value)
{
    if (!kls) return -1;

    QString tn = kls->nazv;

    std::map<QString, QVariant> m_map_geo_value;

    if (kls->loc == LOC1 && kls->label_angle) {
        if (cl.size() == 2) {
            CFPoint p1 = cl.first();
            CFPoint p2 = cl.second();

            double angle = atan2(p1.y-p2.y, p1.x-p2.x)*180/M_PI;
            cl.clear();
            cl.push_back(p1);
            cl.angle = angle;
        }

        m_map_geo_value[kls->codlabel_angle] = QVariant(cl.angle);
    }


    for (auto &it: map_value) {
        m_map_geo_value[it.first] = it.second;
    }


    m_map_geo_value[kls->shape] = cl.getWKT(kls->loc, 1);

//    int idv = execInsertQ(*kls->m_db, tn, m_map_geo_value);
    int idv = insertIntoDatabase(*kls->m_db, tn, m_map_geo_value, kls->shape);


    if (idv) {
        CGeoObject* g = new CGeoObject(idv, cl);
        if (!g) return 0;

        g->text00000 = "";
        kls->geo4.push_back(g);

        viewGeo(kls, idv);
    }

    return idv;
}


//int addRemoved(QSqlDatabase & db, int typeID, int id, const QString & text)
//{
//    return 1;
//}


/*
QString GetTitle(const QString & path) {
  QString str;
  int i;

  str = path;
  i = str.ReverseFind(QDir::separator());
  if (i >= 0) str = str.Right(str.GetLength()-i-1);
  i = str.ReverseFind('.');
  if (i >= 0) str = str.Left(i);
  return str;
}
*/


const int Dx = 160, Dy = 160;
#if 0

long okr(double x, int dX) {
    if (dX == 0) return (long) x;
    if (x < 0) x -= dX;
    return (long)(x+dX/2)/dX*dX;
}
#endif


bool createConnection(const QString & mdb, const QString & other);

void GidWidget::saveFlags()
{
    qDebug() << "void GidWidget::saveFlags()";

    QSettings settings;

    settings.setValue("flags/menu", main_window->menuBar()->isVisible());


    settings.setValue("flags/isAstr",  m_bIsAstr);
    settings.setValue("flags/isGeo",  m_bIsGeo);
    settings.setValue("flags/isGidrInf",  m_bIsGidrInf);
    settings.setValue("flags/isMapYes",  m_bIsMapYes);
    settings.setValue("flags/isMas",  m_bIsMas);
    settings.setValue("flags/isNaprGid",  m_bIsNaprGid);
    settings.setValue("flags/isPodp",  m_bIsPodp);
    settings.setValue("flags/isPodpRes",  m_bIsPodpRes);
    settings.setValue("flags/isPTS",  m_bIsPts);
    settings.setValue("flags/isRamka",  m_bIsRamka);
    settings.setValue("flags/isRezhim",  m_bIsRezhim);

    settings.setValue("flags/ring",  m_ring);

    settings.setValue("flags/font1",  m_font1);
    settings.setValue("flags/font2",  m_font2);


    settings.setValue("flags/bk_color",  m_bk_color);
    settings.setValue("flags/dX",  m_dX);

    settings.setValue("flags/internetMap",  m_internetMap);
    settings.setValue("flags/idMenu",  m_idMenu);

    settings.setValue("flags/masPic", getStdMas());
    settings.setValue("flags/masPodp", getMasPodp());

    QString baza = m_fileName;

    settings.setValue("baza/"+baza+"/mas", geom.masx);
    settings.setValue("baza/"+baza+"/bx", geom.bx);
    settings.setValue("baza/"+baza+"/by", geom.by);
}

void GidWidget::hideEvent(QHideEvent *event)
{
//    saveFlags();
    qDebug() << "void GidWidget::hideEvent(QHideEvent *event)";

    Scroll::hideEvent(event);
}

void GidWidget::restoreFlags()
{
    QSettings settings;

//    bool is_menu = settings.value("flags/menu", false).toBool();
    bool is_menu = false;
    main_window->menuBar()->setVisible(is_menu);

    m_bIsAstr =    settings.value("flags/isAstr", m_bIsAstr).toBool();
    m_bIsGidrInf = settings.value("flags/isGidrInf", m_bIsGidrInf).toBool();
    m_bIsMapYes =  settings.value("flags/isMapYes", m_bIsMapYes).toBool();
    m_bIsMas =     settings.value("flags/isMas", m_bIsMas).toBool();
    m_bIsNaprGid = settings.value("flags/isNaprGid", m_bIsNaprGid).toBool();
    m_bIsPodp =    settings.value("flags/isPodp", m_bIsPodp).toBool();
    m_bIsPodpRes = settings.value("flags/isPodpRes", m_bIsPodpRes).toBool();
    m_bIsPts =     settings.value("flags/isPts", m_bIsPts).toBool();
    m_bIsRamka =   settings.value("flags/isRamka", m_bIsRamka).toBool();
    m_bIsRezhim =  settings.value("flags/isRezhim", m_bIsRezhim).toBool();
    m_bIsRezhim = true;

    m_ring =  settings.value("flags/ring", m_ring).toDouble();

    
    m_bIsGeo =     settings.value("flags/isGeo", m_bIsGeo).toBool();

    m_font1 =      qvariant_cast<QFont>(settings.value("flags/font1", m_font1));
    m_font2 =      qvariant_cast<QFont>(settings.value("flags/font2", m_font2));

    setStdMas(settings.value("flags/masPic", getStdMas()).toDouble());
    setMasPodp(settings.value("flags/masPodp", getMasPodp()).toDouble());

    if (m_bIsMapYes) {
        m_internetMap = settings.value("flags/internetMap",  m_internetMap).toInt();
    }

    m_idMenu = settings.value("flags/idMenu",  m_idMenu).toInt();

    m_sezon_korrozia = settings.value("flags/season_korrozia",  m_sezon_korrozia).toInt();

    m_sezon_date1 = settings.value("flags/sezon_date1",  m_sezon_date1).toDateTime();
    m_sezon_date2 = settings.value("flags/sezon_date2",  m_sezon_date2).toDateTime();

    m_bk_color = settings.value("flags/bk_color", m_bk_color).value<QColor>();
//    m_dX = settings.value("flags/dX", m_dX).toInt();

    QString rovno = settings.value("WMS_host", "").toString();
//    QString layer = settings.value("WMS_layer", "").toString();

    m_colors.load(m_idMenu == 4);


    setWMS(rovno, "");
}


GidWidget::~GidWidget()
{
    qDebug() << "GidWidget::~GidWidget()";
//    saveFlags();
}

GidWidget::GidWidget(QWidget *parent)
    : Scroll(parent)
{
    initInf();

//    createActions(main_window);
//    createMenu(main_window);
//    createToolBar(main_window);

    setFocusPolicy( Qt::StrongFocus );
    setFocus ( Qt::MouseFocusReason );

    m_tic = true;

    m_isOpen = false;

    m_parent_kam = nullptr;
    m_napr1 = nullptr;

    m_CurNode = nullptr;  // Выделенный узел
    m_CurLine = nullptr;  // Выделенный участок
    m_CurGeoObject = nullptr;

    m_internetMap = ID_NOMAP;
    m_internetMapAction = nullptr;

// Флаги
    m_idMenu = 1;

    restoreFlags();

//    m_nRegim = gidrAction.aInfo;
    regim = R_INIT;
    m_nRegim = nullptr;


//    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_parent_id = 0;
    m_parent_id_old = 0;

    m_dxf = nullptr;

    m_fileID = -1;

    m_node_for_move = nullptr;
    m_line_for_move = nullptr;
    m_geo_for_move.kls = nullptr;

    startTimer(250);

    createActions();
    createMenu();

//    main_window->menuBar()->setVisible(true);


    QTimer *updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &GidWidget::updateUI);
    updateTimer->start(100); // Проверка состояния каждые 100 мс



#ifndef _RIBBON
    createToolBar(main_window);
#endif
}

void GidWidget::beginDraw(const CNode2 *node)
{
    endDraw();
    m_CurNode = (CNode2*) node;
}

void GidWidget::beginDraw(const CLINE2 *Line)
{
    endDraw();
    m_CurLine = bline(Line);
}

void GidWidget::endDraw()
{
    m_CurGeoObject = nullptr;
    m_CurNode = nullptr;
    m_CurLine = nullptr;
}

void GidWidget::drawMark(QPainter *painter)
{
    if (m_tic) {
        if ( m_CurNode ) {
            drawNode0(painter, m_CurNode, m_tic);
        }
        if ( m_CurLine ) {
            drawLine1(painter, m_CurLine, m_tic, 0);
        }
        if (m_CurGeoObject && m_CurKlassif) {
            drawObject(painter, m_CurGeoObject, m_CurKlassif, m_tic);
        }
    }
}

void GidWidget::timerEvent(QTimerEvent *event)
{
    m_tic = !m_tic;

    if (regim != R_MOVE_VIEW) {
        repaintBackBuffer(0, 0);
    }

    if (download_manager.n_download) {
        download_manager.init();
        update();
    }

}


QString getPar(std::set<int>& v)
{
    QString par = "";

    for (auto &it : v) {
        QString s = QString("%1").arg(it);
        if (par != "") par += ",";
        par += s;
    }
    return par;
}

QString getPar(const std::vector<int>& v)
{
    QString par = "";

    for (auto &it : v) {
        QString s = QString("%1").arg(it);
        if (par != "") par += ",";
        par += s;
    }
    return par;
}


QString getPar(const std::list<int>& v)
{
    QString par = "";

    for (auto &it : v) {
        QString s = QString("%1").arg(it);
        if (par != "") par += ",";
        par += s;
    }
    return par;
}



void setCurrentExternalCode(int fileID, int kod);


void GidWidget::new_fragment()
{
    if (QMessageBox::question(this, "", tr("Добавить новый фрагмент?")) != QMessageBox::Yes) return;

    QString tn = "fragments";
    QString name = QDateTime::currentDateTime().toString("Фрагмент от dd-MM-yyyy hh:mm");
//    QString q = QString("INSERT INTO %1 (name) VALUES ('%2')").arg(tn, name);

    std::map<QString, QVariant> data;
    data["name"] = name;

    int id = insertIntoDatabase(m_cxema.m_db, tn, data);

//    int id = ExecuteInsert(m_cxema.m_db, q);

    if (id) {
//        q = QString("INSERT INTO specExpends (specExpendID, fileID) VALUES ('УР1', %1)").arg(id);
//        int id2 = ExecuteInsert(m_cxema.m_db, q);
        std::map<QString, QVariant> data1;
        data1["fileID"] = id;
        data1["specExpendID"] = "УР1";
        int id2 = insertIntoDatabase(m_cxema.m_db, "specExpendID", data1);

//        q = QString("INSERT INTO calcTemperatures (calcTemperatureID, fileID) VALUES ('ТР1', %1)").arg(id);
//        id2 = ExecuteInsert(m_cxema.m_db, q);

        std::map<QString, QVariant> data2;
        data2["fileID"] = id;
        data2["specExpendID"] = "ТР1";

        id2 = insertIntoDatabase(m_cxema.m_db, "calcTemperatureID", data2);

//        q = QString("INSERT INTO externalCodes (name, fileID) VALUES ('РС1', %1)").arg(id);
//        id2 = ExecuteInsert(m_cxema.m_db, q);

        std::map<QString, QVariant> data3;
        data3["fileID"] = id;
        data3["name"] = "РС1";

        id2 = insertIntoDatabase(m_cxema.m_db, "externalCodes", data3);

        setCurrentExternalCode(id, id2);

        auto *bt = qobject_cast<QPushButton *>(sender());
        if (bt) {
            ChMenuDial *dlg = qobject_cast<ChMenuDial *>(bt->parentWidget());
            if (dlg) {
                dlg->Add(name, id, true);
            }
        }
    }
}


QString menuFragment(QWidget *parent, QSqlDatabase &db, const QString & par1)
{
//    QString q = "SELECT id, name FROM fragments WHERE removed = 0 ORDER BY CASE WHEN Name like'%[0-9]%' THEN Replicate('0', 100 - Len(Name)) + Name ELSE Name END, name";
    bool ret = false;
    QString q = "SELECT id, name FROM fragments WHERE removed = 0 ORDER BY name";

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query_exec(db, query, q);

    ChMenuDial dlg(parent, QObject::tr("Выбрать фрагменты сети"));

    dlg.addButton(QObject::tr("Новый фрагмент"), parent, SLOT(new_fragment()));

    QStringList par_list = par1.split(",");

    while (query.next()) {
        QString name = query.value("name").toString();
        int id = query.value("id").toInt();

        bool on = par_list.indexOf(QString("%1").arg(id)) >= 0 || par_list.size() == 0;
        dlg.Add(name, id, on);
    }

    if (!dlg.exec()) return "";

    QString par = "";

    for (auto & it : dlg.out) {
        if (par != "") par += ",";
        par += QString("%1").arg(it.toString());
    }

    return par;
}

#include <ui2/Login2Dialog.h>
#include <any/rights.h>

bool GidWidget::openGid(const QString & baza)
{
    CLogin2Dialog dlg2(m_cxema.m_db, this);
    if (dlg2.exec() != QDialog::Accepted) return false;

//    LoginDialog login;
  
//    if (login.exec() != QDialog::Accepted) return false;

    this->m_user = dlg2.m_user;
    this->m_strUser = dlg2.m_strUser;
    this->m_password = dlg2.m_password;

    UserRight::setRight(dlg2.user_right);

    QSettings settings;

    QString par1 = settings.value(baza).toString();
    QString par = menuFragment(this, m_cxema.m_db, par1);

    if (par == "") return false;

    bool ret = false;

    m_cxema.m_par = par;

    settings.setValue(baza, par);

    QProgressDialog progress(QString("Открывается %1, подождите пожалуйста...").arg(baza), "Прервать", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    progress.move(screenGeometry.center() - progress.rect().center());

    progress.show();
    progress.setValue(0);
    QApplication::processEvents();

    if (m_cxema.open_cxema(this, par, &progress)) {
        double m = max(m_cxema.rect().Width()/width(), m_cxema.rect().Height()/height());
        SetMas(m, m_cxema.rect().left/geom.masx, m_cxema.rect().top/geom.masy);

        repaint();

        geom.masx = geom.masy = settings.value("baza/"+baza+"/mas", geom.masx).toFloat();
        geom.bx = settings.value("baza/"+baza+"/bx", geom.bx).toFloat();
        geom.by = settings.value("baza/"+baza+"/by", geom.by).toFloat();

        main_window->setCxema(&m_cxema);

        m_fileName = baza;
        m_isOpen = true;
        ret = true;
        main_window->m_ms->init(this, m_cxema.m_db, true);
        main_window->m_rs->init(this, m_cxema.m_db, false);
    }

    // подключаемся для синхронизации

//    m_cxema.m_db_2;


    progress.setValue(100);
    QApplication::processEvents();

    progress.setLabelText(QString("Открывается геобаза %1, подождите пожалуйста...").arg(baza));

    progress.setValue(0);
    QApplication::processEvents();

    m_geo.open_geo(this, "gid", m_cxema.m_db, &progress);

    return ret;
}


bool GidWidget::openGidGeo(int rdbms, const QString & host, int port, const QString & geo_name, const QString & user, const QString & password)
{
    QSqlDatabase *db = nullptr;

    if (geo_name != "") {
        db = new QSqlDatabase();
        bool ok = connectSQL0(rdbms, host, port, geo_name, user, password, *db);
        if (ok) {
            QProgressDialog progress(QString("Открывается %1, подождите пожалуйста...").arg(geo_name), "Прервать", 0, 100, this);
            progress.setWindowModality(Qt::WindowModal);

            QScreen *screen = QGuiApplication::primaryScreen();
            QRect screenGeometry = screen->geometry();
            progress.move(screenGeometry.center() - progress.rect().center());

            progress.show();
            progress.setValue(0);
            QApplication::processEvents();

            m_geo.open_geo(this, geo_name, *db, &progress);
            return true;
        }
    }
    return false;
}



bool GidWidget::connectSQL(int rdbms, const QString & host, int port, const QString & baza, const QString & geo_name, const QString & user, const QString & password)
{
    bool ret = false;

    if (m_cxema.connectSQL(rdbms, host, port, baza, user, password)) {
        ret = openGid(baza);
        if (ret) {
            openGidGeo(rdbms, host, port, geo_name, user, password);
        }
    }
    return ret;
}

bool GidWidget::connectSqlite(const QString & baza)
{
    bool ret = false;

    if (m_cxema.connectSqlite(baza)) {
        ret = openGid(baza);
//        openGidGeo(rdbms, host, port, geo_name, user, password);
    }
    return ret;
}



void GidWidget::SetMas(double m, double bx0, double by0)
{
    double mm = m*geom.dmas;
    if (mm <= 10.) {
//        QMessageBox::information(this, "", QString("Масштаб 1:%1 недопустим").arg(mm));
        return;
    }


    if (mm <= 0) {
        QMessageBox::information(this, "", QString("Масштаб 1:%1 недопустим").arg(mm));
        return;
    }
    else {
        geom.bx = bx0*geom.masx/m;
        geom.by = by0*geom.masy/m;
        geom.masx = m;
        geom.masy = m;
    }
    main_window->updateStatusBar();
}

void GidWidget::onMasshtab() // Масштаб...\tCtrl+F3
{
    bool ok;
    double m = QInputDialog::getDouble(this, tr("Масштаб"),
                                       tr("Введите масштаб"), geom.masx*geom.dmas, 1, 100000, 0, &ok);

    if (ok) {
        SetMas(m/geom.dmas, geom.bx, geom.by);
        repaint();
    }
}

bool GidWidget::moveCurrent(){
    main_window->setCurrent(this);
    onMasall();
    return true;
}

bool GidWidget::moveVyd()
{
    CFRect rect;
    if (m_cxema.m_graph->GetMarkRect(rect)) {
        main_window->setCurrent(this);
        return moveRect(rect);
    }
    return false;
}

bool GidWidget::movePoint(CFPoint pt)
{
    moveXY(pt);
    repaint();
    main_window->setCurrent(this);

    return true;
}


bool GidWidget::moveRect(CFRect rect)
{
    if (rect.left == LONG_MAX) return false;

    int cx = (rect.left + rect.right) / 2;
    int cy = (rect.top + rect.bottom) / 2;
    int w = rect.Width();
    int h = rect.Height();

    if (w <= 1000) w = 1000;
    if (h <= 1000) h = 1000;

    CFRect m_rect = CFRect(0, 0, width(), height());

    int ww = m_rect.Width();
    int hh = m_rect.Height();

    if (ww <= 1000) ww = 1000;
    if (hh <= 1000) hh = 1000;


    if (rect.Width() * hh < ww * rect.Height()) {
        w = h * ww / hh;
    }
    else {
        h = w * hh / ww;
    }

    rect.left = cx - w / 2;
    rect.right = cx + w / 2;
    rect.top = cy - h / 2;
    rect.bottom = cy + h / 2;


    rect.InflateRect(rect.Width() / 4, rect.Height() / 4);
    if (rect.Width() == 0 && rect.Height() == 0) {
        SetMas(500. / geom.dmas, cx / geom.masx / geom.dmas, cy / geom.masy / geom.dmas);
        moveXY(CFPoint(cx, cy));
    }

    ChMas(rect);

    if (geom.masx * geom.dmas < 500.) {
        SetMas(500. / geom.dmas, cx / geom.masx / geom.dmas, cy / geom.masy / geom.dmas);
        moveXY(CFPoint(cx, cy));
    }

    CFPoint p = CoordToScreen(CFPoint(cx, cy));
//    Mxy(p.x, p.y);
    QCursor::setPos(mapToGlobal(QPoint(p.x, p.y)));

    repaint();

    return true;
}

void GidWidget::ChMas(CFRect rect)
{
    double m;

    if (rect.Width() == 0 && rect.Height() == 0) return;

    rect.NormalizeRect();

    m = max((double)rect.Width()/width(), (double)rect.Height()/height());

    SetMas(m, rect.left/geom.masx, rect.top/geom.masy);
    repaint();
}

void GidWidget::onMasall() // Отобразить всю схему\tF3
{
    if (m_cxema.isOpen()) {
        if (m_parent_id == 0) {
            double m = max(m_cxema.rect().Width()/width(), m_cxema.rect().Height()/height());
            SetMas(m, m_cxema.rect().left/geom.masx, m_cxema.rect().top/geom.masy);
        }
        else {
            CFRect rect = m_cxema.m_graph->getBorder(m_parent_id);
            double m = max(rect.Width()/width(), rect.Height()/height());
            SetMas(m, rect.left/geom.masx, rect.top/geom.masy);
        }
    }
    else {
        /*
             CFRect rect = m_gmap.rect();

             if (rect.Width() > 0) {
               double m = max(rect.Width()/width(), rect.Height()/height());
               SetMas(m, rect.left/geom.masx, rect.top/geom.masy);
             }
        */
    }
    repaint();
}

void GidWidget::onMinus() // Уменьшить\t-
{
    geom.bx += width()/2;
    geom.by += height()/2;
    SetMas(geom.masx*1.5, geom.bx, geom.by);
    geom.bx -= width()/2;
    geom.by -= height()/2;
    scaling = true; // Устанавливаем флаг масштабирования
    scaleTimer->start(scalingTime);
    repaint();
}

void GidWidget::onPlus() // Увеличить\t+
{
    geom.bx += width()/2;
    geom.by += height()/2;
    SetMas(geom.masx/1.5, geom.bx, geom.by);
    geom.bx -= width()/2;
    geom.by -= height()/2;
    scaling = true; // Устанавливаем флаг масштабирования
    scaleTimer->start(scalingTime);
    repaint();
}

void GidWidget::moveLeft()
{
    ScrollWindow(Dx, 0);
}

void GidWidget::moveRight()
{
    ScrollWindow(-Dx, 0);
}

void GidWidget::moveUp()
{   ScrollWindow(0, Dy);
}

void GidWidget::moveDown()
{
    ScrollWindow(0, -Dy);
}

QString GidWidget::status(int i)
{
    QString str = "";

    switch (i) {
    case 1 :
        str = QString("  %1, %2  ").arg(-0.01*m_pt.y, 0, 'f', 1).arg(0.01*m_pt.x, 0, 'f', 1);
        break;
    case 2 :
        str = QString("  1:%1  ").arg(geom.masx*geom.dmas, 0, 'f', 1);
        break;
    case 3 :
    {
#if 1
//        CGraph2 *m_graph = m_cxema.graph();
        double delta = geom.masx*D5/mas_otn;
//        CNode2 *node = m_cxema.m_graph->find(m_pt, delta);
//        CLINE2 *line = m_cxema.m_graph->findLine(m_pt, delta);

        double md = geom.masx * D5;

        std::list<CNode2 *> lst_node = m_cxema.m_graph->find_node_list(m_parent_id, m_pt, md);
        std::list<CLINE2 *> lst_line = m_cxema.m_graph->find_line_list(m_parent_id, m_pt, geom.masx * D5 / mas_otn);


        if (lst_node.size() > 0) {
            CNode2 * node = lst_node.front();
            str = QString ("%1").arg(nodef[nodef->typ].table).arg(node->getNameFull());
        }

        else if (lst_line.size() > 0) {
            CLine2 *l = bline(lst_line.front());
            str = QString ("%1").arg(l->getNameFull());
        }
        else {
            if (m_bIsGeo) {
                list<KlGeo> geo_list;
//                void findXY(CFPoint p, double md, list<KlGeo> & geo_list);

                findXY(m_pt, md, geo_list);

                if (geo_list.size()) {
                    KlGeo kg = geo_list.front();

                    if (geo_list.size() > 0) {
                        str = kg.kls->RusName();
                    }
                }
            }
        }


#endif
    }
    break;

    case 4 :
    {
        if (m_pt1.x != 0 && m_pt1.y != 0 && regim != R_INIT) {
            double len1, len2;

            len1 = Length(m_pt1, m_pt2);
            len2 = m_NP.Length();

            double uu ;

            uu = (m_pt1.x == m_pt2.x && m_pt1.y == m_pt2.y) ? 0 :
                 -atan2(m_pt1.x-m_pt2.x, m_pt1.y-m_pt2.y)*180/M_PI;

            str = QString(tr("  Полная длина %1, Длина %2, Угол %3 ")).arg(len2).arg(len1).arg(uu);
            //    Status(0, str);
        }
    }
    }

    return str;
}

void GidWidget::wheelEvent ( QWheelEvent * event )
{
    int zDelta = event->angleDelta().y();

    if (regim != R_MOVE_VIEW) {
        if (zDelta != 0) {
            if (zDelta < 0) {
                for (int i = 0; i < -zDelta; i += 120) {
                    geom.bx += event->position().x();
                    geom.by += event->position().y();
                    SetMas(geom.masx*1.5, geom.bx, geom.by);
                    geom.bx -= event->position().x();
                    geom.by -= event->position().y();
                }
            }
            else {
                for (int i = 0; i < zDelta; i += 120) {
                    geom.bx += event->position().x();
                    geom.by += event->position().y();
                    SetMas(geom.masx/1.5, geom.bx, geom.by);
                    geom.bx -= event->position().x();
                    geom.by -= event->position().y();
                }
            }
            scaling = true; // Устанавливаем флаг масштабирования
            scaleTimer->start(scalingTime);
            repaint();
        }
    }

}

bool reset_shape_line(QSqlDatabase & db, int idP, int idO);

QString shape_text(CNode2 *n1, CNode2 *n2, const CCoordList &cl1);

CFRect GetLineRect(CFPoint pt1, CFPoint pt2, const CCoordList &cl);


#include <string>
#include <sstream>
#include <list>

std::string toWKTLineString(const std::list<CFPoint>& points) {
    std::ostringstream oss;
    oss << "LINESTRING(";

    bool first = true;
    for (const auto& pt : points) {
        if (!first)
            oss << ", ";
        oss << pt.x << " " << pt.y;
        first = false;
    }

    oss << ")";
    return oss.str();
}


std::string toWKTLineString(CLINE2 *l) 
{
    CFPoint pt1 = where(l)->node.coord;
    CFPoint pt2 = other(l)->node.coord;

    std::ostringstream oss;
    oss << "LINESTRING(";

    oss << pt1.x/100. << " " << -pt1.y/100. << ", ";

    for (const auto& pt : bline(l)->line.m_NP) {
        oss << pt.x/100. << " " << -pt.y/100. << ", ";
    }

    oss << pt2.x/100. << " " << -pt2.y/100.;

    oss << ")";
    return oss.str();
}


std::string toWKTPoint(CFPoint pt) 
{
    std::ostringstream oss;
    oss << "POINT(";
    oss << pt.x/100. << " " << -pt.y/100.;
    oss << ")";
    return oss.str();
}




bool GidWidget::setCoord(CLINE2 *line, CCoordList &cl)
{
    CLine2 *l = bline(line);

    QString coord = cl.saveStr();
    QString shape = shape_text(where(line), other(line), cl);

    std::string wkt = toWKTLineString(line);

//    QString q = QString("UPDATE linesobj SET coords='%1', shape=%2 WHERE ID=%3 OR ID=%4").arg(coord, shape).arg(l->line.nomP).arg(l->line.nomO);
    QString q = QString("UPDATE linesobj SET coords='%1', shape=%2, operatorID=%5, archiveChangeDate=%6, sync_tgid=true WHERE ID=%3 OR ID=%4")
        .arg(coord, shape).arg(l->line.nomP).arg(l->line.nomO)
        .arg(m_user)
        .arg(get_now());
        
        
        ;

    QSqlQuery query(m_cxema.m_db);
    bool ret = query_exec(m_cxema.m_db, query, q);

    if (ret) {

#if 0
        QString q = QString("UPDATE gid.heatpipesections SET linesobj_shape=%1 WHERE linesobj_id=%2 or linesobj_id=%3").arg(shape).arg(l->line.nomP).arg(l->line.nomO);
        bool ret = query_exec(m_cxema.m_db_2, q);
#endif

//        QString qq = QString("l.id=%1 OR l.id=%2").arg(l->line.nomP).arg(l->line.nomO);
//        reset_shape_line(m_cxema.m_db, l->line.nomP, l->line.nomO);

        QString text = l->line.m_NP.saveStr();
        if (l->line.nomP > 0) {
            addRemoved(m_cxema.m_db, m_user, CH_T_MOVE_LINE, l->line.nomP, text);
        }
        if (l->line.nomO > 0 && l->line.nomO != l->line.nomP) {
            addRemoved(m_cxema.m_db, m_user, CH_T_MOVE_LINE, l->line.nomO, text);
        }

        l->line.m_NP = cl;
        l->line.rect = GetLineRect(where(l)->node.coord, other(l)->node.coord, cl);
        m_cxema.m_graph->change();
        return true;
    }
//  AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
    return false;
}


// Копировать одну строку linesobj
#if 0
int copyRowLine1(QSqlDatabase & db, const QString & tn, int id, int externalSignLineID, const QString & coord)
{
    QString params1 = "";
    QString params2 = "";

    std::map<QString, AdoField> map_fields;
    get_table_columns(db, tn, map_fields);

    for (auto & [col, v] : map_fields) {

        if (col != "id") {
            if (params1 != "") params1 += ",";
            if (params2 != "") params2 += ",";

            params1 += col;

            if (col.toLower() == "externalsignlineid") {
                QString s = QString("%1").arg(externalSignLineID);
                params2 += s;
            }
            else if (col.toLower() == "coords") {
                QString s = QString("'%1'").arg(coord);
                params2 += s;
            }
            else {
                params2 += col;
            }
        }
    }

//    QString q = QString("insert into %1 (%2) select %3 from %4 where id = %5").arg(tn, params1, params2, tn).arg(id);
//    int id2 = ExecuteInsert(db, q);

    std::map<QString, QVariant> data;
    data["fileID"] = fileID;
    int id2 = insertIntoDatabase(db, tn, data);


    return id2;
}

#endif

int copyRowLine1(QSqlDatabase & db, const QString & tn, int id, int externalSignLineID, const QString & coord, const QString & shape)
{
    QSqlQuery query(db);

    QString selectSql = QString("SELECT * FROM %1 WHERE id = %2").arg(tn).arg(id);

    if (!query.exec(selectSql) || !query.next()) {
        qWarning() << "Не удалось получить строку:" << query.lastError().text();
        return -1;
    }

    QSqlRecord rec = query.record();

    std::map<QString, QVariant> data;

    // 2. Собираем список полей и значений (кроме ключевого)
    for (int i = 0; i < rec.count(); ++i) {
        QString fieldName = rec.fieldName(i);
        if (fieldName == "id") continue;
        if (fieldName.toLower() == "externalsignlineid") continue;
        if (fieldName.toLower() == "coords") continue;
        if (fieldName.toLower() == "shape") continue;
        data[fieldName] = rec.value(i);
    }
    data["externalsignlineid"] = externalSignLineID;
    data["coords"] = coord;
    data["shape"] = shape;

    int id2 = insertIntoDatabase(db, tn, data, "shape");

    return id2;
}


#if 0

DELETE FROM heatpipesections h
USING (
    SELECT MIN(ctid) AS keep_ctid, lineid
    FROM heatpipesections
    GROUP BY lineid
) s
WHERE h.lineid = s.lineid
  AND h.ctid <> s.keep_ctid;


#endif 

int duplicateRow(QSqlDatabase &db, const QString &tableName, int sourceId, const QString &keyField = "id")
{
    QSqlQuery query(db);

    QString selectSql = QString("SELECT * FROM %1 WHERE %2 = %3").arg(tableName, keyField).arg(sourceId);

    if (!query.exec(selectSql) || !query.next()) {
        qWarning() << "Не удалось получить строку:" << query.lastError().text();
        return -1;
    }

    QSqlRecord rec = query.record();

    std::map<QString, QVariant> data;

    // 2. Собираем список полей и значений (кроме ключевого)
    for (int i = 0; i < rec.count(); ++i) {
        QString fieldName = rec.fieldName(i);
        if (fieldName == keyField) continue;
        data[fieldName] = rec.value(i);
    }

    int newId = insertIntoDatabase(db, tableName, data);

    return newId;
}



// Копировать одну строку Участка


int copyRowLine2(QSqlDatabase & db, const QString & tn, int id, int lineID)
{
#if 0
    QString params1 = "";
    QString params2 = "";

    std::map<QString, AdoField> map_fields;
    get_table_columns(db, tn, map_fields);


    for (auto & [col, v] : map_fields) {
        if (col != "id") {
            if (params1 != "") params1 += ",";
            if (params2 != "") params2 += ",";
            params1 += col;

            if (col.toLower() == "lineid") {
                QString s = QString("%1").arg(lineID);
                params2 += s;
            }
            else {
                params2 += col;
            }
        }
    }

    QString q = QString("insert into %1 (%2) select %3 from %4 where id = %5").arg( tn, params1, params2, tn).arg(id);
    int id2 = ExecuteInsert(db, q);
#endif

    QSqlQuery query(db);

    QString selectSql = QString("SELECT * FROM %1 WHERE id = %2").arg(tn).arg(id);

    if (!query.exec(selectSql) || !query.next()) {
        qWarning() << "Не удалось получить строку:" << query.lastError().text();
        return -1;
    }

    QSqlRecord rec = query.record();

    std::map<QString, QVariant> data;

    // 2. Собираем список полей и значений (кроме ключевого)
    for (int i = 0; i < rec.count(); ++i) {
        QString fieldName = rec.fieldName(i);
        if (fieldName == "id") continue;
        if (fieldName.toLower() == "lineid") continue;
        data[fieldName] = rec.value(i);
    }
    data["lineid"] = lineID;

    int id2 = insertIntoDatabase(db, tn, data);

    return id2;
}



// Разъединить

bool GidWidget::setCoord1(CLINE2 *line, CCoordList &cl)
{
    CLine2 *l = bline(line);

    QString coord = cl.saveStr();

    int nomP = l->line.nomP;
    int nomO = l->line.nomO;

    int idP2 = l->line.idP2;
    int idO2 = l->line.idO2;

    QString q = QString("UPDATE linesobj SET externalSignLineID=2 WHERE id=%1").arg(nomP);

    bool ret = query_exec(m_cxema.m_db, q);

    if (ret) {
        if (nomP == nomO) {
            QString shape = shape_text(where(line), other(line), cl);

            nomO = copyRowLine1(m_cxema.m_db, "linesobj", nomP, 3, coord, shape);
            if (nomO > 0) {
                idO2 = copyRowLine2(m_cxema.m_db, l->getTableMySQL(), l->line.idP2, nomO);
            }
            else {
                QMessageBox::information(this, "", QString("Ошибка!"));
                return false;
            }
        }
        else {
            QString shape = shape_text(where(line), other(line), cl);

            q = QString("UPDATE linesobj SET externalSignLineID=3, coords='%1', shape=%2 WHERE id=%3").arg(coord, shape).arg(nomO);
            bool ret = query_exec(m_cxema.m_db, q);

//            reset_shape_line(ado, nomO);
        }


        CLINE2* lineO = m_cxema.m_graph->insert_line(where(line), other(line), cl);

        if (lineO) {
            CLine2* lO = bline(lineO);
            lO->line = l->line;

            lO->line.nomP = -1;
            lO->line.nomO = nomO;

            lO->line.idP2 = -1;
            lO->line.idO2 = idO2;
            lO->line.m_NP = cl;

            l->line.nomO = -1;
            l->line.idO2 = -1;
        }
        repaint();
        return true;
    }




/*

    QString q = QString("UPDATE linesobj SET coords='%1' WHERE ID=%2 OR ID=%3").arg(coord).arg(l->line.nomP).arg(l->line.nomO);

    QSqlQuery query(m_cxema.m_db);
    bool ret = query_exec(m_cxema.m_db, query, q);

    if (ret) {
        QString qq = QString("l.id=%1 OR l.id=%2").arg(l->line.nomP).arg(l->line.nomO);
        reset_shape_line(m_cxema.m_db, qq);

        QString text = l->line.m_NP.saveStr();
        if (l->line.nomP > 0) {
            addRemoved(m_cxema.m_db, CH_T_MOVE_LINE, l->line.nomP, text);
        }
        if (l->line.nomO > 0 && l->line.nomO != l->line.nomP) {
            addRemoved(m_cxema.m_db, CH_T_MOVE_LINE, l->line.nomO, text);
        }

        l->line.m_NP = cl;
        m_cxema.m_graph->change();
        return true;
    }

*/

//  AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
    return false;
}



bool setGeoCoord(QSqlDatabase &db, Klassif *kls, int id, CCoordList &cl)
{
    QString str = cl.getWKT(kls->loc, 1);

    QString q = QString("UPDATE %1 SET %2=%3 WHERE id=%4").arg(kls->nazv, kls->shape, str).arg(id);

    if (kls->label_angle) {
        q = QString("UPDATE %1 SET %2=%3, %4=%5 WHERE id=%6").arg(kls->nazv, kls->shape, str, kls->codlabel_angle).arg(cl.angle).arg(id);
    }

    QSqlQuery query(db);
    return query_exec(db, query, q);
}

int get_node_typ0(const QString & tn);


bool setGeoLineCoord(QSqlDatabase &db, Klassif *kls, int id, Klassif *kls1, int id1, Klassif *kls2, int id2, CCoordList &cl)
{
    QString str = cl.getWKT(kls->loc, 1);

    QString ss1 = ", typeID1=null, nodeID1=null", ss2 = ", typeID2=null, nodeID2=null";

    if (kls1) {
        int typ1 = get_node_typ0(kls1->nazv);
        if (typ1 >= 0) ss1 = QString(", typeID1=%1, nodeID1=%2").arg(typ1).arg(id1);
    }
    if (kls2) {
        int typ2 = get_node_typ0(kls2->nazv);
        if (typ2 >= 0) ss2 = QString(", typeID2=%1, nodeID2=%2").arg(typ2).arg(id2);
    }

    QString q = QString("UPDATE %1 SET %2=%3 %4 %5 WHERE id=%6").arg(kls->nazv, kls->shape, str, ss1, ss2).arg(id);

    QSqlQuery query(db);
    return query_exec(db, query, q);
}




void isprClLoc6(CCoordList &cl)
{
    CFPoint p1 = cl.first();

    if (cl.size() == 1) {
        cl.push_back(CFPoint(p1.x+100, p1.y));
        return;
    }

    CFPoint p2 = cl.second();
    double h = Length(p1, p2);
    p2.x = p1.x+(p2.x-p1.x)*1./h;
    p2.y = p1.y+(p2.y-p1.y)*1./h;
    cl.clear();
    cl.push_back(p1);
    cl.push_back(p2);
}


void getProjectCl(CCoordList &cl, CCoordList& m_NP, int loc)
{
    CFPoint point = m_NP.first();
    CFPoint p1, p2;
    double mas_otn = 1;

    point = cl.GetProjection(point, p1, p2);
    double m5 = 100;
    double h = hypot(p1.x - p2.x, p1.y - p2.y);

    m_NP.clear();

    m_NP.push_back(point);

    if (loc == LOC6) {
        point = CFPoint(point.x + (p2.x - p1.x) * m5 / h / mas_otn, point.y + (p2.y - p1.y) * m5 / mas_otn / h);
        m_NP.push_back(point);
    }
    if (loc == LOC1) {
        CFPoint p1 = point;
        CFPoint pp2 = CFPoint(point.x + (p2.x - p1.x) * m5 / h / mas_otn, point.y + (p2.y - p1.y) * m5 / mas_otn / h);

        m_NP.angle = atan2(p1.y-pp2.y, p1.x-pp2.x)*180/M_PI;
    }
}

void getProject(CLINE2* line, CCoordList& m_NP, int loc)
{
    CFPoint point = m_NP.first();

    CFPoint p1 = where(line)->node.coord;
    CFPoint p2 = other(line)->node.coord;
    double mas_otn = 1;

    CLine2* l = bline(line);
    CCoordList cl;
    cl.push_back(p1);
    CCoordList::const_iterator it = l->line.m_NP.begin();
    for (; it != l->line.m_NP.end(); ++it) {
        cl.push_back(*it);
    }
    cl.push_back(p2);

    getProjectCl(cl, m_NP, loc);
}


bool isPoint(int loc)
{
  return loc == LOC1 || loc == LOC6 || loc == LOC11;
//   || loc == LOC12 || loc == LOC13 || loc == LOC14;
}

void GidWidget::set_geo_line()
{
    CFPoint pt1 = m_cl_for_move.first();
    CFPoint pt2 = m_cl_for_move.last();

    list<KlGeo> geo_list1;
    list<KlGeo> geo_list2;

    double md = geom.masx * D5;

    int id1 = -1, id2 = -1;
    Klassif *kls1 = nullptr, *kls2 = nullptr;

    findXY(pt1, md, geo_list1);
    findXY(pt2, md, geo_list2);

    for (auto &it : geo_list1) {
        if (getOnlygeo(it.kls->nazv) == m_geo_for_move.kls->nazv) {
            if (it.gl->cl.fileID == m_cl_for_move.fileID) {
                id1 = it.gl->nom;
                kls1 = it.kls;
            }
        }
    }

    for (auto &it : geo_list2) {
        if (getOnlygeo(it.kls->nazv) == m_geo_for_move.kls->nazv) {
            if (it.gl->cl.fileID == m_cl_for_move.fileID) {
                id2 = it.gl->nom;
                kls2 = it.kls;
            }
        }
    }



    if (setGeoLineCoord(m_cxema.m_db, m_geo_for_move.kls, m_geo_for_move.gl->nom, kls1, id1, kls2, id2, m_cl_for_move)) {
        m_geo_for_move.gl->cl = m_cl_for_move;
        m_geo_for_move.gl->rect = m_cl_for_move.GetFRect();
        m_geo_for_move.kls = nullptr;
        m_cl_for_move.clear();
    }
}

void GidWidget::set_geo()
{

    // Установить точку с поворотом на участке

    if (m_geo_for_move.kls->loc == LOC6 && !m_geo_for_move.kls->label_angle) {
        isprClLoc6(m_cl_for_move);
    }

    if (isPoint(m_geo_for_move.kls->loc)) {
        CFPoint pt = m_cl_for_move.first();
        double delta = geom.masx*D5;
        CLINE2* line = m_cxema.m_graph->findLine( m_parent_id, pt, delta);

        int loc = m_geo_for_move.kls->loc;
        if (m_geo_for_move.kls->label_angle && loc == LOC6) loc = LOC1;

        if (line) {
            getProject(line, m_cl_for_move, loc);
        }

        else {
            double md = geom.masx * D5;
            list<KlGeo> geo_list;
            Klassif *kls = m_kl_list.findKlN("new_baza.uchastok");

            if (kls) {
                findXYkls(pt, md, kls, geo_list);
                for (KlGeo &kg: geo_list) {
                    if (!kg.kls->ins) continue;
                    getProjectCl(kg.gl->cl, m_cl_for_move, loc);
                }
            }
        }
    }


    if (setGeoCoord(*m_geo_for_move.kls->m_db, m_geo_for_move.kls, m_geo_for_move.gl->nom, m_cl_for_move)) {
        m_geo_for_move.gl->cl = m_cl_for_move;
        m_geo_for_move.gl->rect = m_cl_for_move.GetFRect();
        m_geo_for_move.kls = nullptr;
        m_cl_for_move.clear();
    }
    repaint();
}

void GidWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton )  {
        if (m_line_for_move)  {
            m_cl_for_move.pop_front();
            m_cl_for_move.pop_back();
            if (is_move1 && (bline(m_line_for_move)->line.nomP != -1 && bline(m_line_for_move)->line.nomO != -1)) {  // Разъединить
                if (setCoord1(m_line_for_move, m_cl_for_move)) {
                    m_line_for_move = nullptr;
                    m_cl_for_move.clear();
                }
            }
            else {
                if (setCoord(m_line_for_move, m_cl_for_move)) {
                    m_line_for_move = nullptr;
                    m_cl_for_move.clear();
                }
            }
            repaint();
        }

        if (regim == R_BEGIN_GEO_LOC2_2) {
            create_geo(m_kls, m_NP, m_map_value);
            regim = R_INIT;
            m_NP.clear();
            repaint();
        }

        if (m_geo_for_move.kls)  {
            set_geo();
        }

        if (m_NP.size() > 2 && m_nRegim == gidrAction.aFragmentPoly) {
            m_bIsPolyFragment = true;
            poly_fragment = m_NP;
            m_NP.clear();
            regim = R_INIT;
            repaint();
        }


        if (m_NP.size() > 2 && m_nRegim == gidrAction.aMark) {
            m_cxema.m_graph->setMark(m_NP, true);
            m_nRegim = nullptr;
            m_NP.clear();
            regim = R_INIT;

            repaint();
        }

        if (m_NP.size() > 2 && m_nRegim == gidrAction.aMarkDel) {
            m_cxema.m_graph->setMark(m_NP, false);
            m_nRegim = nullptr;
            m_NP.clear();
            regim = R_INIT;

            repaint();
        }


    }

    Scroll::mouseDoubleClickEvent(event);
}


void GidWidget::mouseMoveEvent ( QMouseEvent * event )
{
    m_isMouseMoved = true;

    m_pt = ScreenToCoord(CFPoint(event->position().x(), event->position().y()));

    if (regim == R_MOVE_VIEW) {
        repaintBackBuffer((m_pt.x-m_pt1.x)/geom.masx, (m_pt.y-m_pt1.y)/geom.masy);
    }
    else if (regim == R_MOVE_NODE) {
        repaint();
    }
    else {
        m_pt2 = m_pt;
        repaintBackBuffer(0, 0);
    }


//  if (regim == R_BEGIN_LEN2 || regim == R_BEGIN_LEN_OKR2 || regim == R_BEGIN_DRAW2 || regim == R_BEGIN_DRAW_LINE) {
//  }


    main_window->updateStatusBar();
    Scroll::mouseMoveEvent(event);
}



void GidWidget::beginMove(CFPoint pt)
{
    drawToBuffer();
    regim = R_MOVE_VIEW;
    m_pt1 = m_pt2 = pt;
    setCursor(Qt::OpenHandCursor);
}



//void GidWidget::endMove(CFPoint pt){}


void sw_p(CLINE2* line)
{
    CLine2* l = bline(line);

    l->line.isPjezo = !l->line.isPjezo;

    if (l->line.isPjezo) {
        where(line)->node.isPjezo = true;
        other(line)->node.isPjezo = true;
    }
    else {
        if (incP(where(line)) == 1) where(line)->node.isPjezo = false;
        if (incP(other(line)) == 1) other(line)->node.isPjezo = false;
    }
}

bool is_onlyline(const QString & tn);


bool GidWidget::sfind(const CFPoint& pt)
{
    double delta = geom.masx*D5;
    CLINE2* line = m_cxema.m_graph->findLine( m_parent_id, pt, delta);

    if (line) {
        sw_p(line);
        repaint();
    }
    return true;
}

bool GidWidget::create_geo_line(Klassif *kls, CCoordList & cl)
{
    CFPoint pt = cl.first();

    CLINE2 *line = m_cxema.m_graph->findLine(m_parent_id, pt, geom.masx*D5/mas_otn);

    if (line) {
        int loc = kls->loc;
        if (kls->label_angle) loc = LOC1;


        bool priznak = column_in_table(*m_kls->m_db, m_kls->nazv, "priznak_truboprovoda");

        if (priznak) {
            QMenu menu(this);

            QAction *aP = menu.addAction("Подающий");
            QAction *aO = menu.addAction("Обратный");
            QAction *aOB = menu.addAction("Общий");
            QAction *aPO = menu.addAction("Подающий+Обратный");
            QAction *a = menu.exec(mapToGlobal(CoordToQPoint(pt)));

            if (a != aP && a != aO && a != aOB && a != aPO) return true;

            int po = 1;

            if (a == aP) po = 2;
            if (a == aO) po = 3;
            if (a == aOB) po = 1;
            if (a == aPO) po = 2;

            m_map_value["priznak_truboprovoda"] = po;

            getProject(line, m_NP, loc);
            create_geo(m_kls, m_NP, m_map_value);

            if (a == aPO) {
                m_map_value["priznak_truboprovoda"] = 3;
                create_geo(m_kls, m_NP, m_map_value);
            }
        }
        else {
            getProject(line, m_NP, loc);
            create_geo(m_kls, m_NP, m_map_value);
        }

        regim = R_INIT;
        m_NP.clear();
        repaint();
        return true;
    }

    return false;
}

void GidWidget::mousePressEvent(QMouseEvent *event)
{

    m_isMouseMoved = false;


//  if (m_nRegim == gidrAction.aInfo)

    m_mouse_press = true;

    CFPoint pt = ScreenToCoord(CFPoint(event->position().x(), event->position().y()));


    Qt::KeyboardModifiers km = QApplication::queryKeyboardModifiers();

    if (event->button() == Qt::LeftButton && km & Qt::ShiftModifier) {
        sfind(pt);
        return;
    }


    if (event->button() == Qt::MiddleButton) {
        beginMove(pt);
        return;
    }

    if (event->button() == Qt::RightButton) {
        RButMenu(QCursor::pos(), pt);
        return;
    }


    switch ( regim ) {

//    case R_INIT :

    case R_BEGIN_CANAL_1:
    case R_BEGIN_CANAL_2:


    case R_BEGIN_DRAW_LINE:
    case R_BEGIN_DRAW_TYPE:
        find_point(pt);
        break;

    case R_BEGIN_DRAW_COPY1:
    case R_BEGIN_DRAW_COPY_BMP1:
        oldregim = regim;
        switch ( regim ) {
        case R_BEGIN_DRAW_COPY1:
            regim = R_BEGIN_DRAW_COPY2;
            break;
        case R_BEGIN_DRAW_COPY_BMP1:
            regim = R_BEGIN_DRAW_COPY_BMP2;
            break;
        }
        m_pt2 = m_pt1 = pt;
        setCursor(Qt::CrossCursor);
        break;

    case R_BEGIN_DRAW1 :
    case R_BEGIN_DRAW2 :
        pt.x = okr(pt.x, m_dX);
        pt.y = okr(pt.y, m_dX);
        m_pt1 = m_pt2 = pt;
        m_NP.push_back(pt);

        regim = R_BEGIN_DRAW2;
        break;

    case R_BEGIN_GEO_LOC2_1:
    case R_BEGIN_GEO_LOC2_2:
        {
            bool fnd = false;

            if (isOnlygeo(m_kls->nazv)) {
                double md = geom.masx * D5;
                list<KlGeo> geo_list;
                findXY(pt, md, geo_list);
                for (auto & it: geo_list) {
                    if (getOnlygeo(it.kls->nazv).toLower() == m_kls->nazv.toLower()) {
                        pt = it.gl->cl.first();
                        fnd = true;
                        break;
                    }
                }

                if (regim == R_BEGIN_GEO_LOC2_1) {
                    if (!fnd) {
                        QMessageBox::warning(this, "", QString("Не могу рисовать %1 где попало").arg(m_kls->nazv));
                        break;
                    }
                }
                else {
                    if (fnd) {
                        m_NP.push_back(pt);
                        create_geo(m_kls, m_NP, m_map_value);
                        regim = R_INIT;
                        m_NP.clear();
                        repaint();
                        break;
                    }
                }
            }

            m_pt1 = m_pt2 = pt;
            m_NP.push_back(pt);

            regim = R_BEGIN_GEO_LOC2_2;
        }

        break;


    case R_BEGIN_GEO_LOC6_1:
        m_pt1 = m_pt2 = pt;
        m_NP.push_back(pt);
        if (is_onlyline(m_kls->nazv)) {
            if (!create_geo_line(m_kls, m_NP)) {
                QMessageBox::warning(this, "", QString("%1 можно размещать только на участке").arg(m_kls->RusName()));
            }
            m_NP.clear();
            regim = R_INIT;
        }
        else {
            regim = R_BEGIN_GEO_LOC6_2;
        }
        break;

    case R_BEGIN_GEO_LOC6_2:
        m_pt1 = m_pt2 = pt;
        m_NP.push_back(pt);

        create_geo(m_kls, m_NP, m_map_value);
        regim = R_INIT;
        m_NP.clear();
        repaint();

        break;

    case R_BEGIN_GEO_LOC1:
        m_pt1 = m_pt2 = pt;
        m_NP.push_back(pt);

        if (is_onlyline(m_kls->nazv)) {
            if (!create_geo_line(m_kls, m_NP)) {
                QMessageBox::warning(this, "", QString("%1 можно размещать только на участке").arg(m_kls->RusName()));
            }
        }
        else {
            create_geo(m_kls, m_NP, m_map_value);
        }

        regim = R_INIT;
        m_NP.clear();
        repaint();
        break;

    case R_BEGIN_LEN1 :
    case R_BEGIN_LEN2 :
        m_pt1 = m_pt2 = pt;
        m_NP.push_back(pt);

        regim = R_BEGIN_LEN2;
        break;

    case R_BEGIN_LEN_OKR1 :
    case R_BEGIN_LEN_OKR2 :
        m_pt1 = m_pt2 = pt;
        m_NP.push_back(pt);

        regim = R_BEGIN_LEN_OKR2;
        break;

    case R_BEGIN_MOVE_LINE:
        if (m_NP.size()) find_point2(pt);
        break;

    /*
        case R_MOVE_LINE:
            pt.x = okr(pt.x, m_dX);
            pt.y = okr(pt.y, m_dX);

            m_pt2 = pt;
    //        m_NP.SetV(tecF, pt);

            regim = R_BEGIN_MOVE_LINE;
            break;
    */

    case R_BEGIN_MOVE_VIEW :
        regim = R_MOVE_VIEW;
        m_pt1 = m_pt2 = pt;
        break;
//-----------------------------------------------
    case R_BEGIN_LEN_OKR3 :
        regim = R_MOVE_VIEW;
        pt.x = okr(pt.x, m_dX);
        pt.y = okr(pt.y, m_dX);
        m_pt1 = m_pt2 = pt;
//      BeginPaint();
//      BeginMoveW(m_NP);
//      EndPaint();
        break;

    case R_BEGIN_RECT_1:  // Закончили рисовать фрагмент для печати

        {
            regim = R_INIT;
            m_bIsFragment = true;
            m_is_print_fr = true;
            m_pt1 = pt;
            double mas = m_print_mas;

            int dx = m_print_size.width() * mas / 10;
            int dy = m_print_size.height() * mas / 10;

            m_pt1_1 = pt;
            m_pt1_2 = m_pt1 + CFPoint(dx, dy);
            m_print_1 = pt;
            m_print_2 = m_pt1_2;
        }
        break;

    case R_INIT :

        if (m_nRegim == gidrAction.aZoom || m_nRegim == gidrAction.aFragment) {
            oldregim = regim;

            regim = R_BEGIN_RECT;
            m_pt2 = m_pt1 = pt;
            setCursor(Qt::CrossCursor);
        }
        else if (m_nRegim == gidrAction.aMoveVyd) {
            regim = R_BEGIN_MOVE_FRAGMENT;
            m_pt2 = m_pt1 = pt;
            setCursor(Qt::OpenHandCursor);
        }
        else {
            find_point(pt);
        }
        break;

//-----------------------------------------------
    default:
        break;
    }
    Scroll::mousePressEvent(event);
}

void GidWidget::init_print_param(const CFPoint & m_pt1, const CFPoint & m_pt2)
{
    int dx = abs(m_pt1.x-m_pt2.x);
    int dy = abs(m_pt1.y-m_pt2.y);

    m_pt1_1 = m_pt1;
    m_pt1_2 = m_pt2;

    if (m_pt1_1.x > m_pt1_2.x) {
        double x = m_pt1_1.x; m_pt1_1.x = m_pt1_2.x; m_pt1_2.x = x;
    }
    if (m_pt1_1.y > m_pt1_2.y) {
        double y = m_pt1_1.y; m_pt1_1.y = m_pt1_2.y; m_pt1_2.y = y;
    }

    int w = m_print_size.width();
    int h = m_print_size.height();
    int ww = w;
    int hh = h;

    double masx = dx*10/w;
    double masy = dy*10/h;

    if (dx > dy) {
        masx = dx*10/h;
        masy = dy*10/w;
        ww = h;
        h = ww;
    }

    if (dx/ww < dy/hh) {
        dx = dy*h/w;
    }
    else {
        dy = dx*w/h;
    }

    m_print_mas = max(masx, masy);

    m_print_1 = m_pt1_1;
    m_print_2 = m_pt1_2;


    qDebug() << "m_print_mas=" << m_print_mas;

//    m_print_mas = max(masx, masy)*geom.dmas;
}



void GidWidget::mouseReleaseEvent(QMouseEvent *event)
{
    CFPoint pt2;
    pt2 = ScreenToCoord(CFPoint(event->position().x(), event->position().y()));

    m_pt.x = okr(m_pt.x, m_dX);
    m_pt.y = okr(m_pt.y, m_dX);


    double delta = geom.masx * D5; // / mas_otn;

    if (regim == R_BEGIN_RECT) {
        if (m_nRegim == gidrAction.aZoom) {
            ChMas(CFRect(m_pt1, pt2));
        }
        else if (m_nRegim == gidrAction.aFragment) {
            m_bIsFragment = true;
            m_print_1 = m_pt1;
            m_print_2 = m_pt2;
            init_print_param(m_print_1, m_print_2);
        }

        regim = R_INIT;
    }
    else if (regim == R_BEGIN_MOVE_FRAGMENT) {  // Закончили переносить фрагмент
        SetCur(m_nRegim);
        regim = R_INIT;
    }

    else if (regim == R_MOVE_VIEW) {
        geom.bx -= (m_pt.x-m_pt1.x)/geom.masx;
        geom.by -= (m_pt.y-m_pt1.y)/geom.masy;
        repaint();
        if (m_nRegim == gidrAction.aMovew) regim = R_BEGIN_MOVE_VIEW;
        else regim = R_INIT;
        SetCur(m_nRegim);
    }
    if (m_node_for_move) {
        CNode2 *node = m_cxema.m_graph->find(m_parent_id, pt2, delta);

        if (node && node != m_node_for_move && node->node.m_graph == m_node_for_move->node.m_graph) {
            QString str = QString(tr("Слишком близко к %1 %2")).arg(node->getTable(), node->getName());
            QMessageBox::warning(this, "", str);
        }
        else {
            //      if (!isEditPS()) return false;
            setXY(m_node_for_move, m_pt);
            //      m_nRegim = ID_NODE_MOVE;
            //      GetDocument()->SetModifiedFlag(true);
        }

        m_node_for_move = nullptr;
        repaint();
    }
    if (m_line_for_move) {
        if (!m_isMouseMoved) {
            int k = m_cl_for_move.find_point(pt2, delta, false);
            if (k >= 0) {
                m_cl_for_move.removeV(k);
            }
        }
        regim = R_INIT;
        repaint();
    }

    if (m_geo_for_move.kls) {
        if (!m_isMouseMoved) {
            Klassif *kls = m_geo_for_move.kls;
            int sz = m_cl_for_move.size();

            if ((kls->loc == LOC2 && sz >= 3) || (kls->loc == LOC3 && sz >= 4)) {
               int k = m_cl_for_move.find_point(pt2, delta, true);
               if (k >= 0) {
                   m_cl_for_move.removeV(k);
               }
            }
        }

        if (isPoint(m_geo_for_move.kls->loc)) {
            set_geo();
        }

        else if (isOnlygeo(m_geo_for_move.kls->nazv)) {
            int sz = m_cl_for_move.size();
            if (m_n_for_move == 0 || m_n_for_move == sz-1) {
                double md = geom.masx * D5;
                list<KlGeo> geo_list;
                findXY(pt2, md, geo_list);
                for (auto & it: geo_list) {
                    if (getOnlygeo(it.kls->nazv).toLower() == m_geo_for_move.kls->nazv.toLower()) {
                        m_cl_for_move.SetV(m_n_for_move, it.gl->cl.first());
                        set_geo_line();
                        break;
                    }
                }
            }
        }


        regim = R_INIT;
        repaint();
    }

    m_isMouseMoved = false;
    m_mouse_press = false;
    Scroll::mouseReleaseEvent(event);
}

void GidWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
        moveLeft();
        scaling = true; // Устанавливаем флаг масштабирования
        scaleTimer->start(scalingTime);
        break;
    case Qt::Key_Right:
        moveRight();
        scaling = true; // Устанавливаем флаг масштабирования
        scaleTimer->start(scalingTime);
        break;
    case Qt::Key_Up:
        moveUp();
        scaling = true; // Устанавливаем флаг масштабирования
        scaleTimer->start(scalingTime);
        break;
    case Qt::Key_Down:
        moveDown();
        scaling = true; // Устанавливаем флаг масштабирования
        scaleTimer->start(scalingTime);
        break;
    case Qt::Key_Escape:
        escape();
        break;
    default :
        QWidget::keyPressEvent(event);
    }
}

void GidWidget::cl2QPolygon(const CCoordList &cl, QPolygon &pg)
{
    pg.clear();

    CCoordList::const_iterator it = cl.begin();

    for ( ; it != cl.end(); ++it)
    {
        QPoint pt((int)it->x/geom.masx-geom.bx, (int)it->y/geom.masy-geom.by);
        pg.push_back(pt);
    }
}

void GidWidget::onEdit(QAction *action)
{
    if (action == gidrAction.aEditor1) {
        m_idMenu = 1;
    }
    else if (action == gidrAction.aEditor2) {
        m_idMenu = 2;
    }
    else if (action == gidrAction.aEditor4) {
        m_idMenu = 4;
    }
    m_colors.load(m_idMenu == 4);

    repaint();
}

void GidWidget::onRegim(QAction *action)
{
    m_nRegim = action;
    regim = R_INIT;

    if (action == gidrAction.aMovew) {
        regim = R_BEGIN_MOVE_VIEW;
        SetCur(m_nRegim);
    }
    else if (action == gidrAction.aFragmentPoly) {
        regim = R_BEGIN_LEN1;
        m_NP_loc = LOC3;
    }
    else if (action == gidrAction.aMark) {
        regim = R_BEGIN_LEN1;
        m_NP_loc = LOC3;
    }
    else if (action == gidrAction.aMarkDel) {
        regim = R_BEGIN_LEN1;
        m_NP_loc = LOC3;
    }
    else if ((action == gidrAction.aKam || action == gidrAction.aKamVnesh) && !IsMain() ) {
        KamObr();
        return;
    }
    else {
        regim = R_INIT;
        SetCur(m_nRegim);
    }
}

void GidWidget::Kam(int internalNodeID)
{
    m_parent_id = internalNodeID;
    m_vnutr_typ = 1;
    geom_old = geom;
    CFRect rect = m_cxema.graph()->getBorder(m_parent_id);
//    SetBorder(rect);
    geom.dmas = 5;
    ChMas(rect);
    repaint();
}

void GidWidget::KamObr(bool redraw)
{
    if (m_parent_id) {
        m_parent_id = 0;
        geom = geom_old;
        CFRect rect = m_cxema.graph()->getBorder(0);
        if (m_cxema.m_graph_old) {
            delete m_cxema.m_graph;
            m_cxema.m_graph = m_cxema.m_graph_old;
            m_cxema.m_graph_old = nullptr;
        }
    }
    if (redraw)
        repaint();
}

void GidWidget::moveGeo(const QString & tn, int id)
{
    Klassif *kls = m_kl_list.findKlN(tn);
    if (kls) {
        CGeoObject *go = kls->getGeoObjectById(id);
        if (go) {
              moveRect(go->rect);
              main_window->setCurrent(this);
        }
    }
}


QString ispr_name(const QString & txt0)
{
    QString txt = txt0;

    static QRegularExpression re(QString("(.+) [%1%2]").arg(S_POD, S_OBR));
    QRegularExpressionMatch match = re.match(txt);

    if (match.hasMatch()) {
        txt = match.captured(1);
    }

    static QRegularExpression re2(QString("^(.+) [%1%2] - (.+)$").arg(S_POD, S_OBR));
    match = re2.match(txt);

    if (match.hasMatch()) {
        QString s1 = match.captured(1);
        QString s2 = match.captured(2);

        txt = s1 + " - " + s2;
    }

    return txt;

}


bool GidWidget::moveText(const QString & parent, const QString & txt)
{
    int parent_id = 0;

    if (parent != "") {
        for (auto &p : m_cxema.m_graph->map_node) {
            CNode2 *pp = p.second;
            QString s = pp->getName();
            if (s == parent) {
                parent_id = pp->id;
                break;
            }
        }
    }

    QString txt2 = ispr_name(txt);


    for (auto &p : m_cxema.m_graph->map_node) {
        CNode2 *pp = p.second;

        if (pp->node.internalNodeID == parent_id) {
            QString s = pp->getName();

            if (s == txt || s == txt2) {
                moveNode(pp);
                return true;
            }

            for (CLINE2 *l = pp->lines; l; l = nextl(l)) {
                QString s = bline(l)->getName();

                if (s == txt || s == txt2) {
                    moveLine(l);
                    return true;
                }
            }
        }
    }
//    QMessageBox::information(this, "", QString("%1 = %2").arg(parent, txt));
    return false;
}

void GidWidget::moveNode(const CNode2* n)
{
    if (n->node.internalNodeID != m_parent_id) {
        KamObr();

        if (n->node.internalNodeID) {
            CNode2* nn = m_cxema.m_graph->find(n->node.internalNodeID);
            if (nn) moveXY(CFPoint(nn->node.coord));

            Kam(n->node.internalNodeID);
        }
    }

    moveXY(CFPoint(n->node.coord));
    beginDraw(n);

    main_window->setCurrent(this);
}

void GidWidget::moveLine(const CLINE2* l)
{
    CNode2 *n = where(l);
    if (n->node.internalNodeID != m_parent_id) {
        KamObr();

        if (n->node.internalNodeID) {
            CNode2* nn = m_cxema.m_graph->find(n->node.internalNodeID);
            if (nn) moveXY(CFPoint(nn->node.coord));

            Kam(n->node.internalNodeID);
        }
    }

    moveXY(CFPoint(n->node.coord));
    beginDraw(l);
    main_window->setCurrent(this);
}

void GidWidget::moveNode(int id)
{
    CNode2 * n = m_cxema.graph()->find(id);

    if (n) {
        moveNode(n);
    }
}


void GidWidget::moveLine(int id)
{
    CLINE2 * l = m_cxema.graph()->findLine(id);

    if (l) {
        moveLine(l);
    }
}



#include <dialog/MMenuDial.h>
#include <dialog/TableWindow.h>


void GidWidget::onEditFindNodeAccepted()
{
    TableWindow * tab = (TableWindow *)sender();
    CNode2* node = m_cxema.m_graph->find(tab->value());

    if (node) {
        moveNode(node);
    }
}
void GidWidget::onEditFindLineAccepted()
{
    TableWindow * tab = (TableWindow *)sender();
    CLINE2* line = m_cxema.m_graph->findLine(tab->value());

    if (line) {
        moveLine(line);
    }
}


void GidWidget::onEditFindNode() // Найти узел...\tCtrl+F
{
    QString q = readQ("sql4/find_node.sql");
    q.replace(QString("$fileID$"), m_cxema.m_par);

    if (m_parent_id != 0) {
        q += QString("\nAND n.internalNodeID = %1").arg(m_parent_id);
    }


//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);

    TableWindow * tab = new TableWindow(m_cxema.m_db, "node", "Выберите узел", q, this);
    tab->show();

    connect(tab, SIGNAL(accepted()), this, SLOT(onEditFindNodeAccepted()));
}


void GidWidget::printPage(QPrinter *printer, QPainter *painter, int page)
{
#if 0

    //    painter->drawText(100, 100, QString("Страница %1").arg(page));

    auto geom_old1 = geom;
    double m_scale_old = m_scale;
    m_scale = 1.;

    if (m_bIsFragment) {
        double m_dPrintMas = m_print_mas;
        int dpi = printer->resolution();

        geom.masx = geom.masy = m_dPrintMas*2.54/dpi;
        geom.dmas = m_dPrintMas/geom.masx;

        CFRect rect(m_pt1_1, m_print_2);
        rect.NormalizeRect();

        geom.bx = rect.left/geom.masx;
        geom.by = rect.top/geom.masy;
//        setRect(CFRect(m_pt1_1, m_pt1_2));
    }

    draw(painter);

    geom = geom_old1;
    m_scale = m_scale_old;
#endif

    QRectF rectP = printer->pageRect(QPrinter::Millimeter);

    int px = rectP.width();
    int py = rectP.height();

    double m_dPrintMas = m_print_mas;
    int dpi = printer->resolution();

    double Dx = abs(m_print_2.x-m_print_1.x)*10/m_dPrintMas;
    double Dy = abs(m_print_2.y-m_print_1.y)*10/m_dPrintMas;

    qDebug() << "dpi:" << dpi;
    qDebug() << "m_dPrintMas:" << m_dPrintMas;

    qDebug() << Dx << " " << Dy;
    qDebug() << rectP;

    int nx = Dx/rectP.width()-0.3 + 1;
    int ny = Dy/rectP.height()-0.3 + 1;

    int xx = (page-1)%nx;
    int yy = (page-1)/nx;

    qDebug() << "page=" << page;

    qDebug() << "xx=" << xx << "yy=" << yy;

    int dx = px*m_dPrintMas/10;
    int dy = py*m_dPrintMas/10;

    qDebug() << "dx=" << dx << "dy=" << dy;

    int m_scale_old = m_scale;
    auto geom_old1 = geom;

    m_scale = 1;

    geom.masx = geom.masy = m_dPrintMas*2.54/dpi;
    geom.dmas = m_dPrintMas/geom.masx;

    CFRect rect(m_print_1, m_print_2);
    rect.NormalizeRect();

    qDebug() << rect.Width() << " " << rect.Height();

    geom.bx = (rect.left+xx*dx)/geom.masx;
    geom.by = (rect.top+yy*dy)/geom.masy;

    qDebug() << "geom.bx " << geom.bx << " geom.by " << geom.by;

/*
    painter->drawText(100, 100, QString("Страница %1 (%2 %3)")
                                   .arg(page)
                                   .arg(rect.left+xx*dx)
                                   .arg(rect.top+yy*dy)
                     );
*/

//    dx /= dpi;
//    dy /= dpi;


//    painter->device()->;

    draw(painter);



//    dy = 1190;
//    dx = 1684;

//    draw(painter, dx, dy, geom);

    //        printPage(printer, &painter, page);


    geom = geom_old1;
    m_scale = m_scale_old;


}

void GidWidget::printPages(QPrinter *printer)
{
//    init_print_param(m_print_1, m_print_2);

    printer->setFullPage(true); // Включаем режим полной страницы
    
    QPageLayout layout = printer->pageLayout();
    QSizeF pageSize = layout.fullRect(QPageLayout::Point).size();

    qDebug() << "pageSize:" << pageSize;

//    printer->setPageOrientation();



    qDebug() << "--------------------------------------";
    
//    int Dx = abs(m_print_2.x-m_print_1.x);
//    int Dy = abs(m_print_2.y-m_print_1.y);

    QRectF rectP = printer->pageRect(QPrinter::Millimeter);

    int px = rectP.width();
    int py = rectP.height();

    double m_dPrintMas = m_print_mas;
    int dpi = printer->resolution();

    double Dx = abs(m_print_2.x-m_print_1.x)*10/m_dPrintMas;
    double Dy = abs(m_print_2.y-m_print_1.y)*10/m_dPrintMas;

    qDebug() << "dpi:" << dpi;
    qDebug() << "m_dPrintMas:" << m_dPrintMas;

    qDebug() << Dx << " " << Dy;
    qDebug() << rectP;

    int nx = Dx/rectP.width()-0.3 + 1;
    int ny = Dy/rectP.height()-0.3 + 1;

    qDebug() << nx << " " << ny;

    int fromPage = printer->fromPage();
    int toPage = printer->toPage();

    int totalPages = nx*ny;

    if (fromPage == 0 && toPage == 0) {
        fromPage = 1;
        toPage = totalPages;
    }

    QPainter painter(printer);

    for (int page = fromPage; page <= toPage; ++page) {
        printPage(printer, &painter, page);
        if (page < toPage) {
            printer->newPage(); // Переход на следующую страницу
        }
    }
}


void GidWidget::onFilePrint() // Печать...\tCtrl+P
{
#ifndef QT_NO_PRINTER
    QPrinter pr;
    double m_dPrintMas = m_print_mas;

    double Dx = abs(m_print_2.x-m_print_1.x)*10/m_dPrintMas;
    double Dy = abs(m_print_2.y-m_print_1.y)*10/m_dPrintMas;

    if (Dx > Dy) {
        pr.setPageOrientation(QPageLayout::Landscape);
    }
    else {
        pr.setPageOrientation(QPageLayout::Portrait);
    }

    QPrintDialog dlg(&pr);

    if (dlg.exec() == QDialog::Accepted) {
        printPages(&pr);
    }
#endif
}

#include <QPrinterInfo>

void GidWidget::printPreview(QPrinter *printer)
{
}



void GidWidget::onFilePrintPreview() // Предварительный просмотр
{

    QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
    for (const QPrinterInfo &printer : printers) {
        qDebug() << "Доступный принтер:" << printer.printerName() << " " << printer.isDefault();
        QPrinter pr;
        pr.setPrinterName(printer.printerName()); // Выбираем первый доступный принтер
        //        printer.setPrinterMode(QPrinter::HighResolution);
        //        qDebug() << "Разрешение:" << pr.resolution();
        qDebug() << "Разрешение:" << pr.supportedResolutions();
    }
    {
        QPrinter pr;
        if (!printers.isEmpty()) {
            pr.setPrinterName(printers.first().printerName()); // Выбираем первый доступный принтер
            //        qDebug() << "Разрешение:" << pr.resolution();
        } else
        {
            //        qDebug() << "Принтеры не найдены";
        }
    }


    if (!m_bIsFragment) {
        CFRect rectScr = ScreenToCoord(CFRect(0, 0, m_wi, m_hi));
        m_print_1 = rectScr.TopLeft();
        m_print_2 = rectScr.BottomRight();

        init_print_param(m_print_1, m_print_2);
    }


#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::ScreenResolution);
    QPrintPreviewDialog previewDialog(&printer, this);

    previewDialog.setWindowFlags ( Qt::Window );
//    connect(&previewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printPreview(QPrinter *)));
    connect(&previewDialog, &QPrintPreviewDialog::paintRequested, this, &GidWidget::printPages);

    previewDialog.exec();
#endif
}

void GidWidget::drawCl(QPainter *painter, const CCoordList &cl, int dx, int dy, int cl_loc)
{
    QPointF pt1 = CoordToQPointF(cl.first());
    QPolygonF p;

    for (auto & it : cl) {
        QPointF pt = CoordToQPointF(it);
        pt = QPointF(pt.x()-dx, pt.y()-dy);

        p.push_back(pt);
    }
    if (cl_loc == LOC3) {
        pt1 = QPointF(pt1.x()-dx, pt1.y()-dy);
        p.push_back(pt1);
    }

    painter->drawPolyline(p);
}

void GidWidget::XDraw(QPainter *painter, double dx, double dy)
{
    geom.bx -= dx;
    geom.by -= dy;

    CFPoint pt1 = CoordToScreen(m_pt1);
    CFPoint pt2 = CoordToScreen(m_pt2);

    if (m_bIsFragment) {
        CFPoint pt1 = CoordToScreen(m_pt1_1);
        CFPoint pt3 = CoordToScreen(m_pt1_2);
        painter->drawRect(pt1.x, pt1.y, pt3.x-pt1.x, pt3.y-pt1.y);
//        return;
    }


    if (m_bIsPolyFragment) {
        int dx = 0;
        int dy = 0;
        if (regim == R_BEGIN_MOVE_FRAGMENT) {
            dx = pt1.x-pt2.x;
            dy = pt1.y-pt2.y;
        }

        drawCl(painter, poly_fragment, dx, dy, LOC3);
    }

    switch ( regim ) {
    case R_BEGIN_RECT:
    case R_BEGIN_DRAW_COPY2:
    case R_BEGIN_DRAW_COPY_BMP2:
        painter->drawRect(pt1.x, pt1.y, pt2.x-pt1.x, pt2.y-pt1.y);
        break;

//    case R_BEGIN_MOVE_FRAGMENT_1:
//        break;


    case R_BEGIN_RECT_1:    // Квадратик для печати

        double mas = m_print_mas;
        int dx = m_print_size.width() * mas / geom.masx / 10;
        int dy = m_print_size.height() * mas / geom.masy / 10;

        painter->drawRect(pt2.x, pt2.y, dx, dy);
        break;
    }

    if (m_NP.size() > 0) {
        QPointF pt, pt2 = CoordToQPointF(m_pt2);
        QPointF pt1 = CoordToQPointF(m_NP.first());
        QPolygonF p;

        for (auto & it : m_NP) {
            pt = CoordToQPointF(it);
            p.push_back(pt);
        }

        if (regim == R_BEGIN_GEO_LOC6_2) {
            double m_thickness = 1.;
            QColor col = m_kls->coli;
            drawLineTyp(painter, m_thickness, pt1.x(), pt1.y(), pt2.x(), pt2.y(), m_kls->typ, col, col, col, 1, false, false, false, true, true);
        }
        else {
            painter->drawPolyline(p);
            painter->drawLine(pt, pt2);
            if (m_NP_loc == LOC3) {
                painter->drawLine(pt1, pt2);
            }
        }
    }

    if (m_node_for_move) {
        CFPoint coord_save = m_node_for_move->node.coord;

        for (CLINE2 *l = m_node_for_move->lines; l; l = nextl(l)) {
            drawLine1(painter, l, 2, false);
        }
        drawNode(painter, m_node_for_move, 2, false);

        m_node_for_move->node.coord = m_pt;

        for (CLINE2 *l = m_node_for_move->lines; l; l = nextl(l)) {
            drawLine1(painter, l, 0, false);
        }
        drawNode(painter, m_node_for_move, 0, false);

        m_node_for_move->node.coord = coord_save;
    }
    if (m_line_for_move) {
        if (is_move1) {
            int nomP = bline(m_line_for_move)->line.nomP;
            int nomO = bline(m_line_for_move)->line.nomO;
            CCoordList cl = bline(m_line_for_move)->line.m_NP;

            drawLine1(painter, m_line_for_move, 2, false);

            bline(m_line_for_move)->line.nomP = nomP;
            bline(m_line_for_move)->line.nomO = -1;

            drawLine1(painter, m_line_for_move, 0, false);

            bline(m_line_for_move)->line.m_NP = m_cl_for_move;

            if (regim == R_MOVE_LINE) {
                m_cl_for_move.SetV(m_n_for_move, m_pt);
            }

            bline(m_line_for_move)->line.nomP = -1;
            bline(m_line_for_move)->line.nomO = nomO;

            drawLine1(painter, m_line_for_move, 0, false);

            for (auto & it : m_cl_for_move) {
                painter->setPen(QPen(Qt::black, 1));
                painter->setBrush(Qt::NoBrush);
                painter->drawEllipse(CoordToQPointF(it), D5, D5);
            }

            bline(m_line_for_move)->line.m_NP = cl;
            bline(m_line_for_move)->line.nomP = nomP;
            bline(m_line_for_move)->line.nomO = nomO;
        }
        else {
            CCoordList cl = bline(m_line_for_move)->line.m_NP;

            drawLine1(painter, m_line_for_move, 2, false);

            bline(m_line_for_move)->line.m_NP = m_cl_for_move;
            if (regim == R_MOVE_LINE) {
                m_cl_for_move.SetV(m_n_for_move, m_pt);
            }

            drawLine1(painter, m_line_for_move, 0, false);

            for (auto & it : m_cl_for_move) {
                painter->setPen(QPen(Qt::black, 1));
                painter->setBrush(Qt::NoBrush);
                painter->drawEllipse(CoordToQPointF(it), D5, D5);
            }

            bline(m_line_for_move)->line.m_NP = cl;
        }

    }

/*
    if (m_line_for_move1) {  // Для разъединения
        drawLine1(painter, m_line_for_move1, 2, false);

        CCoordList cl = bline(m_line_for_move1)->line.m_NP;

        if (regim == R_MOVE_LINE1) {
            m_cl_for_move.SetV(m_n_for_move, m_pt);
        }

        bline(m_line_for_move1)->line.m_NP = m_cl_for_move;
        drawLine1(painter, m_line_for_move1, 0, false);

        bline(m_line_for_move1)->line.m_NP = cl;

        for (auto & it : m_cl_for_move) {
            painter->setPen(QPen(Qt::black, 1));
            painter->setBrush(Qt::NoBrush);
            painter->drawEllipse(CoordToQPointF(it), D5, D5);
        }
    }
*/

    if (m_geo_for_move.kls && m_geo_for_move.kls) {
//        drawLine1(painter, m_line_for_move, 2, false);
        Klassif *kl = m_geo_for_move.kls;
        CGeoObject *gl = m_geo_for_move.gl;

        if (gl->cl.size() == 0) {
        }
        else {
            CCoordList cl_save = gl->cl;

            CFPoint p1, p2;

            if (kl->loc == LOC6) {
                auto it = m_cl_for_move.begin();
                if (it != m_cl_for_move.end()) {
                    p1 = *it;
                    QPointF pt1 = CoordToQPointF(p1);
                    it++;
                    if (it != m_cl_for_move.end()) {
                        p2 = *it;
                        QPointF pt2 = CoordToQPointF(p2);
                        painter->setPen(QPen(Qt::black, 1));
                        painter->drawLine(pt1, pt2);
                    }
                }
            }

            if (regim == R_MOVE_LINE) {
                m_cl_for_move.SetV(m_n_for_move, m_pt);
                if (kl->loc == LOC6) {
                    if (m_n_for_move == 0) {
                        p2.x = m_pt.x + p2.x-p1.x;
                        p2.y = m_pt.y + p2.y-p1.y;
                        m_cl_for_move.SetV(1, p2);
                    }
                }
            }

            Klassif kl2 = *kl;
            kl2.colb = 0xd3d3d3;
            kl2.coli = 0xd3d3d3;
            kl2.color = 0xd3d3d3;

            drawObject(painter, gl, &kl2, false);

            gl->cl = m_cl_for_move;
            drawObject(painter, gl, kl, false);

            gl->cl = cl_save;

            for (auto & it : m_cl_for_move) {
                painter->setPen(QPen(Qt::black, 1));
                painter->setBrush(Qt::NoBrush);
                painter->drawEllipse(CoordToQPointF(it), D5, D5);
            }

        }
    }

    drawMark(painter);

    geom.bx += dx;
    geom.by += dy;

}


void GidWidget::DrawRect(CFPoint pt, int w, int h)
{
//  setcolor(0x7F7F7F);
//  CPoint p = CoordToScreen(pt);
//  xrectangle(p.x-w, p.y-h, p.x+w, p.y+h);
}

void GidWidget::DrawLine()
{
    /*
      setcolor(0x7F7F7F);

      int saveROP;
      saveROP = m_dc->SetROP2(R2_NOTXORPEN);
      DrawPolygon2(m_NP, m_locNP);
      m_dc->SetROP2(saveROP);

      CCoordList::const_iterator it = m_NP.begin();

      for (; it != m_NP.end(); ++it) DrawRect(*it);
    */
}

void GidWidget::SetCur(QAction *regim)
{
    if (m_nRegim == gidrAction.aZoom
            || m_nRegim == gidrAction.aEditCopy
//    || m_nRegim == gidrAction.aEdit_copybmp
       ) {
        setCursor(Qt::CrossCursor);
    }
    else if (m_nRegim == gidrAction.aMovew) {
        setCursor(Qt::OpenHandCursor);
    }
    else {
        setCursor(Qt::ArrowCursor);
    }
}

void GidWidget::escape()
{
    CGraph2 *m_graph = m_cxema.m_graph;

    m_bIsFragment = false;
    m_bIsPolyFragment = false;

    auto p = m_cxema.m_graph->map_node.begin();

    while (p != m_cxema.m_graph->map_node.end()) {
        CNode2* n = p->second;
        n->node.colorP = 0xFFFFFFFF;
        for (CLINE2* l = n->lines; l; l = nextl(l)) {
            bline(l)->line.colorP = bline(l)->line.colorO = 0xFFFFFFFF;
        }
        p++;
    }

    endDraw();

    repaint();

    m_line_for_move = nullptr;
    m_geo_for_move.kls = nullptr;

    m_CurNode = nullptr;
    m_CurLine = nullptr;
    m_CurGeoObject = nullptr;

    m_geo.clearMark();

    m_napr1 = nullptr;
    if (m_graph->reset()) {
        repaint();
    }

//  Invalidate();
//  CScroll::OnEsc();

    CFPoint p0;

    switch ( regim ) {
    case R_BEGIN_LEN2:
    case R_BEGIN_LEN_OKR2:
    case R_BEGIN_DRAW2:
    case R_BEGIN_DRAW_LINE:

    case R_BEGIN_GEO_LOC2_2:

        p0 = m_NP.back();
//    DrawRect(p0);
        m_NP.pop_back();

        if (m_NP.empty()) {
            regim = R_INIT;
            m_pt1 = CFPoint(0, 0);
        }
        else {
            m_pt1 = m_NP.back();
        }
        break;
    /*
        case R_MOVE_LINE:
    //    xt = fNew[tecF].x;
    //    yt = fNew[tecF].y;

    //// 2010.01.20
            m_NP.clear();
    /////
            regim = R_BEGIN_MOVE_LINE;
            break;
    */

    case R_BEGIN_MOVE_LINE:
        m_NP.clear();
        regim = R_INIT;
        SetCur(m_nRegim);
        break;

    default:
//// 2010.01.20
        m_NP.clear();
/////
        regim = R_INIT;
        break;
    }
    repaintBackBuffer(0, 0);
}


void infoDlg(QWidget *parent);

void GidWidget::onEscape()
{
    escape();
}

void GidWidget::onExportDxf() // Автокад DXF
{
    QString fn;

    fn = QFileDialog::getSaveFileName(this, tr("Экспорт"), QString(), tr("Dxf-файл (*.dxf)"));

#if 0
    if (!fn.isEmpty()) m_cxema.exportDxf(fn);
#endif
}

#include <pjezo/Pjezo.h>
#include <any/MyMain.h>

void GidWidget::onPjezo() // Пьезометр
{
    if (m_cxema.m_graph->list_pjezo.empty()) return;

    QString title = tr("Пьезометрический график");

    MyMain *pm = new MyMain(title, this);
    pm->setWidget(new CPjezo(&m_cxema, pm, this));

    main_window->addWidgetToTabs(pm, title);

//    pm->show();

#if 0

//  pjezo->resize(1200, 900);
//  pjezo->show();
    main_window->loadWidget(pjezo);

//  QDockWidget *dock = new QDockWidget(this);
//  main_window->addDockWidget(Qt::AllDockWidgetAreas, dock);
//  dock->setWidget(pjezo);
#endif
}

void GidWidget::geo_file()
{
#if 0
    GeoFDial geof(main_window->geoFiles, this);

    if (geof.exec() == QDialog::Accepted) {

        m_gmap.clear();

        QStringList & list = main_window->geoFiles;

        for (int i = 0; i < list.size(); i++) {
            m_gmap.open(list[i]);
        }

        if (!isGeo()) gidrAction.aGeobaza->toggle();
//    isGeo(true);
        repaint();
    }
#endif
}

void GidWidget::onKlassif()
{
#if 0
    KlDial geof(m_gmap, this);

    if (geof.exec() == QDialog::Accepted) {
        if (!isGeo()) gidrAction.aGeobaza->toggle();

        QString m_geo_name = "D:/GID/База данных/klass.ki2";

        m_kl_list.save(m_geo_name);

//    isGeo(true);
        repaint();
    }
#endif
}

bool GidWidget::save()
{
//    if (isUntitled) {
//        return saveAs();
//    } else {
//        return saveFile(curFile);
//    }
    return false;
}

bool GidWidget::saveAs()
{
    return false;
//    QString fileName =
//            QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
//    if (fileName.isEmpty())
//        return false;

//    return saveFile(fileName);
}


void GidWidget::onListPjezo() // Список направлений...
{
    MMenuDial md(this, tr("Выберите направление"));
    md.AddButton("Удалить", 1);

    QSqlQuery query(m_cxema.m_db);

    QString q = QString("Select id, name From directions WHERE fileID in (%1)").arg(m_cxema.m_par);

    if (query_exec(m_cxema.m_db, query, q)) {
        while (query.next()) {
            md.Add(query.value(1).toString(), query.value(0).toInt());
        }
    }

    if (md.cnt() == 0) {
        QMessageBox::warning(this, "", tr("Нет сохраненных направлений"));
        return;
    }

    if (md.exec() == QDialog::Accepted) {
        int directionID = md.value().toInt();

        if (md.button_clicked() == 1) {
            QString q = QString("DELETE From deployedDirections Where directionID = %1").arg(directionID);

            if (query_exec(m_cxema.m_db, q)) {
            }

            q = QString("DELETE From directions Where id = %1").arg(directionID);

            if (query_exec(m_cxema.m_db, q)) {
            }

            return;
        }


        m_cxema.m_graph->reset();

        CNode2 *m_napr1 = nullptr;


        QString q = QString("Select nodeID From deployedDirections Where directionID = %1").arg(directionID);

        if (query_exec(m_cxema.m_db, query, q)) {
            while (query.next()) {
                int id = query.value(0).toInt();

                CNode2* node = m_cxema.m_graph->find(id);
                if (node) {
                    if (m_napr1 != nullptr) {
                        m_cxema.m_graph->link(m_napr1, node);
                    }
                    m_napr1 = node;
                    node->node.isPjezo = true;
                }
            }
        }
        repaint();
    }
}

bool GidWidget::isNapr()
{
    return m_cxema.graph()->list_pjezo_min.empty();
}


void GidWidget::find_node(const CNodeName & nodename)
{
#if 0
    CGraph *m_graph = m_cxema.graph();
    CNode *node = m_cxema.graph()->find2(CNode(nodename));
    if (node) {
        moveXY(node->coord);
        beginDraw(node);
        main_window->activate(this);
    }
#endif
}

void GidWidget::find_line(const CNodeName & nodename1, const CNodeName & nodename2)
{
#if 0
    CGraph *m_graph = m_cxema.graph();
    CNode *n1 = m_graph->find2(CNode(nodename1));
    CNode *n2 = m_graph->find2(CNode(nodename2));
    if (n1 && n2) {
        CLINE *l;
        l = m_graph->Link(m_graph->find(*n1), m_graph->find(*n2));
        moveXY(CFPoint((n1->coord.x+n2->coord.x)/2, (n1->coord.y+n2->coord.y)/2));
        beginDraw(l);
        main_window->activate(this);
    }
#endif
}




void GidWidget::unknown()
{
//  QMessageBox::warning(this, tr("!"), sender()->objectName());
//  QMessageBox::warning(this, tr("!"), tr("Еще не сделал"));

//    QSqlTableModel model( QObject * parent = 0, QSqlDatabase db = QSqlDatabase() )    ;
    QSqlTableModel *model = new QSqlTableModel(this, m_cxema.m_db);

    QTableView *view = new QTableView;

    view->setModel(model);
    view->show();
}

QString readFile(const QString & fn);



void GidWidget::onFindcoord() // Поиск по координатам
{
    QPoint pt1 = QCursor::pos();

    CFPoint pt;
    QPoint point = mapFromGlobal(QCursor::pos());

    pt = ScreenToCoord(CFPoint(point.x(), point.y()));

    QString text = QString("%1 %2")
                    .arg(pt.x/100., 0, 'f', 2)
                    .arg(-pt.y/100., 0, 'f', 2)
                    ;

//    text = QString("%1 %2  ---  %3 %4").arg(point.x()).arg(point.y()).arg(pt1.x()).arg(pt1.y());

    bool ok;
    QString out = QInputDialog::getText(this,"", tr("Введите новые координаты"), QLineEdit::Normal, text, &ok);

    if (ok) {
        out.replace(",", ".");
        static QRegularExpression re("^\\s*(\\-?[0-9\\.]+)\\s+(\\-?[0-9\\.]+)\\s*$");
        QRegularExpressionMatch match = re.match(out);

        if (!match.hasMatch()) {
            static QRegularExpression re("^\\s*(\\-?[0-9\\.]+),(\\-?[0-9\\.]+)\\s*$");
            match = re.match(out);
        }

        if (match.hasMatch()) {
            double fx = match.captured(1).toDouble();
            double fy = match.captured(2).toDouble();
            if (moveXY(CFPoint(fx*100, -fy*100))) {
//                    toOb(CFPoint(fx*100, -fy*100));
            }
            else {
                text = QString(tr("Точка %1 %2 за пределами карты")).arg(fx).arg(fy);
                QMessageBox::information(this, "", text);
            }
        }
    }
}


#include "geodez/geodez.h"


void GidWidget::onFindcoordDeg() // Поиск по координатам
{
    double x, y;
    CFPoint pt;
    QPoint point = mapFromGlobal(QCursor::pos());

    pt = ScreenToCoord(CFPoint(point.x(), point.y()));

    void get_google_coord(CFPoint pt, double& x, double& y);
    CFPoint get_google_to_coord(double x, double y);

    get_google_coord(pt, x, y);

//    double xx = SK42_WGS84_Lat(x, y, 0);
//    double yy = SK42_WGS84_Long(x, y, 0);

    QString text = QString("%1 %2")
                    .arg(y, 0, 'f', 12)
                    .arg(x, 0, 'f', 12);

    bool ok;
    QString out = QInputDialog::getText(this,"", tr("Введите новые географические координаты"), QLineEdit::Normal, text, &ok);

    if (ok) {
//        out.replace(",", ".");
        static QRegularExpression re("^\\s*(\\-?[0-9\\.]+)\\s+(\\-?[0-9\\.]+)\\s*$");
        QRegularExpressionMatch match = re.match(out);

        if (!match.hasMatch()) {
            static QRegularExpression re("^\\s*(\\-?[0-9\\.]+)\\s*,\\s*(\\-?[0-9\\.]+)\\s*$");
            match = re.match(out);
        }

        if (match.hasMatch()) {
            double fy = match.captured(1).toDouble();
            double fx = match.captured(2).toDouble();
            CFPoint pt2 = get_google_to_coord(fx, fy);

            if (moveXY(pt2)) {
//                toOb(pt2);
            }
            else {
                text = QString(tr("Точка %1 %2 за пределами карты")).arg(fy).arg(fx);
                QMessageBox::information(this, "", text);
            }
        }
    }
}

void GidWidget::closeEvent(QCloseEvent *event)
{

    qDebug() << "void GidWidget::closeEvent(QCloseEvent *event)";
    /*
        m_mdiArea->closeAllSubWindows();
        if (!m_mdiArea->subWindowList().isEmpty()) {
            event->ignore();
        } else {
            event->accept();
        }
    */


    QMessageBox::StandardButton reply = QMessageBox::question(this, "", tr("Закрыть базу данных?"),
                                        QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QSettings settings;
        QString baza = m_fileName;

        settings.setValue("baza/"+baza+"/mas", geom.masx);
        settings.setValue("baza/"+baza+"/bx", geom.bx);
        settings.setValue("baza/"+baza+"/by", geom.by);
        event->accept();

        if (!IsMain() ) {
            KamObr();
        }

        saveFlags();
    }
    else {
        event->ignore();
    }
}


void GidWidget::onFileClose() // Закрыть
{
    qDebug() << "void GidWidget::onFileClose()";
    close();
}


#include <stdio.h>
#include <dxf/dxfi.h>



void GidWidget::onBmp() //V Файл графической подложки...
{
    QString fn = QFileDialog::getOpenFileName(this,
                 tr("Открыть картинку"), QString(),
                 tr("Картинки (*.dxf)"));
    if (fn != "") {
        void dxf_in(FILE *f, cdxf *dxf);
        FILE *f;

        f = fopen(fn.toLocal8Bit().constData(), "r");

        if (f) {
            m_dxf = new cdxf;
            dxf_in(f, m_dxf);
            fclose(f);
        }
    }


    /*
      for (int i = 0; i < list.size(); i++) {
        if (!list[i].isEmpty()) {
          QList<QListWidgetItem *> li = lw->findItems(list[i], 0);
          if (li.size() == 0) {
            QListWidgetItem *item = new QListWidgetItem(list[i], lw);
            item->setCheckState(Qt::Checked);
            lw->addItem(item);
          }
        }

      }
    */
}


void GidWidget::onF4() // Измерить расстояние\tF4
{
    regim = R_BEGIN_LEN1;
    m_NP_loc = LOC2;
}

void GidWidget::onMark() // Выделить область
{
    regim = R_BEGIN_LEN1;
    m_NP_loc = LOC3;
}



#include <dialog/MMenuDial.h>

void GidWidget::onMainCxema() // Активная схема
{
    MMenuDial menu(this, tr("Выберите активный фрагмент расчетной схемы"));

    for (auto & it : m_cxema.map_fragments) {
        menu.Add(it.second.m_name, it.first);
    }
    if (menu.exec() == QDialog::Accepted) {
//        m_fileID = menu.value().toInt();
        setFileID(menu.value().toInt(), false);
        repaint();
    }
}

void GidWidget::setFileID(int fileID, bool mov)
{
    endDraw();

    m_fileID = fileID;

    if (fileID > 0) {
        CFRect rect = m_cxema.graph()->getBorderFile(fileID);
    //    geom.dmas = 5;
        m_cxema.setRect(rect);
        if (mov) {
            ChMas(rect);
        }
    }
    setTitle();
    repaint();
}

bool GidWidget::isEditGid()
{
    return UserRight::isEditGid();
}

bool isRemont(const QString & tn);
#if 0
{
    if (tn == "defect" || tn == "opres"  || tn == "shurfy" || tn == "remont2" || tn == "osmotr")
    {
        return true;
    }
    return false;
}
#endif


bool isEditGeo(const QString & tn, bool isMessage)
{
    QString user = "Пользователь";
    QString str;
//    CGidrView* pView = getView();

//    if (!pView) return FALSE;
//    //Cgid6Doc* pDoc = (Cgid6Doc*)pView->GetDocument();
//    if (!pDoc) return FALSE;
/*
    if (!pView->isEditGid()) {
        if (isMessage) {
            str.Format("Редактирование в данном режиме невозможно");
            AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
        }
        return FALSE;
    }
*/

#if 0

    if (pView->isReadOnly()) {
        if (isMessage) {
            QMessageBox::warning(nullptr, "", "Редактирование в данном режиме невозможно");
        }
        return FALSE;
    }

#endif

    if (QString::compare(tn, "Act") == 0) {
        if (UserRight::isAct()) {
            return TRUE;
        }
        else {
            if (isMessage) {
                QString str = QString("%1 не имеет прав на редактирование Актов раздела").arg(user);
                QMessageBox::warning(nullptr, "", str);
            }
            return false;
        }
    }

    if (tn == "Индикатор_коррозии") {
        if (UserRight::isKorrozia()) {
            return TRUE;
        }
        else {
            if (isMessage) {
                str = QString("%1 не имеет прав на редактирование Индикаторов коррозии").arg(user);
                QMessageBox::warning(nullptr, "", str);
            }
            return FALSE;
        }
    }

    if (isRemont(tn)) {
        if (UserRight::isRemont()) {
            return TRUE;
        }
        else {
            if (isMessage) {
                str = QString("%1 не имеет прав на редактирование Ремонтов").arg(user);
                QMessageBox::warning(nullptr, "", "Редактирование в данном режиме невозможно");
            }
            return FALSE;
        }
    }
    if (UserRight::isEditGeo()) {
        return TRUE;
    }
    else {
        Klassif* kls = m_kl_list.findKlN(tn);
        if (kls) {
            if (isMessage) {
                str = QString("%1 не имеет прав на редактирование объектов Геобазы").arg(user);
                QMessageBox::warning(nullptr, "", str);
            }
            return false;
        }
    }

    return true;
}

bool GidWidget::addGeo(Klassif* kls, std::map<QString, QVariant> *m_map_value)
{
    if (kls) {
        if (!isEditGeo(kls->nazv, true)) {
            return false;
        }

        this->m_map_value.clear();

        if (m_map_value) {
            for (auto &it : *m_map_value) {
                this->m_map_value[it.first] = it.second;
            }
        }

//        if (init) initGeoValue();
        kls->ins = 1;

        repaint();

        switch(kls->loc) {
        case LOC1:
        case LOC11:
            regim = R_BEGIN_GEO_LOC1;

            if (kls->label_angle) {
                regim = R_BEGIN_GEO_LOC6_1;
            }

            break;
        case LOC2:
        case LOC3:
            regim = R_BEGIN_GEO_LOC2_1;
            break;
        case LOC6:
            regim = R_BEGIN_GEO_LOC6_1;
            break;
        }
        m_NP_loc = kls->loc;

        if (kls->nazv == "kanal") {
/*
            m_nRegim = ID_BEGIN_CANAL_1;
*/
//            m_nRegim = gidrAction.aBeginCanal1;

            regim = R_BEGIN_CANAL_1;
//            regim = R_INIT;

            m_l_canal = nullptr;
            m_kls = kls;
            QMessageBox::warning(this, "", QString("Укажите две точки на участке"));
        }
        else {
            QMessageBox::warning(this, "", QString("Начинайте создавать %1").arg(kls->RusName()));
        }

        m_kls = kls;
//        m_isGeobaza = TRUE;

/*
        regim = R_INIT;
        m_nRegim = ID_INFO;
        SetCur(m_nRegim);
        m_nRegim = ID_NEW_GEO;
*/
    }

    return true;
}

void GidWidget::setTitle()
{
    QString sz = "";
    if (m_sezon_korrozia > 0) {
        sz = QString(tr(" [%1-%2 отопительный сезон]").arg(m_sezon_korrozia).arg(m_sezon_korrozia + 1));
    }
    else {
        sz = " [Все сезоны]";
    }

    QString frn = getCxema()->getFileName(m_fileID);

    parentWidget()->parentWidget()->setWindowTitle(QString("%1 %2 %3").arg(m_fileName, frn, sz));
//    main_window->setWindowTitle(QString("%1 %2 %3 %4  %5").arg(VER_FILEDESCRIPTION_STR, VER_PRODUCT_VERSION_STR, m_fileName, frn, sz));
    main_window->setWindowTitle(QString("%1 %2 %3 %4  %5").arg(ITWIN_TEPLO_RUS, VER_PRODUCT_VERSION_STR, m_fileName, frn, sz));

}

QString GidWidget::getTitle(){
    return parentWidget()->parentWidget()->windowTitle();
}

bool GidWidget::moveVydIf()
{
    CFRect rect;

    if (!m_cxema.m_graph->isMark()) {
        QMessageBox::warning(this, "", "Не найдено");
        return false;
    }

    if (m_cxema.m_graph->GetMarkRect(rect)) {
        moveRect(rect);
        return true;
    }
    return false;
}



bool GidWidget::MoveFragm(double dx, double dy)
{
//    if (!isEditGid()) return false;

//    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

//    while (p != m_cxema->m_graph->map_node.end()) {
    auto p = m_cxema.m_graph->map_node.begin();

    while (p != m_cxema.m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.y != 0) {
            if (m_NP.find3(n->node.coord)) {
                n->node.coord.x += dx;
                n->node.coord.y += dy;

                setXY(n, n->node.coord);

                for (CLINE2* l = n->lines; l; l = nextl(l)) {
                    if (l && IsBegin(l) && bline(l)->line.m_NP.size() > 0) {
                        bline(l)->line.m_NP.move(dx, dy);
                        setCoord(l, bline(l)->line.m_NP);
                    }
                }
            }
        }
        p++;
    }

    return true;
}

bool isNeotr(const CNode2* node)
{
    CLINE2* l;
    CNode2* n2;

    for (l = node->lines; l; l = nextl(l)) {
        n2 = other(l);
        if (n2->node.coord.x == 0 && n2->node.coord.y == 0) {
            return true;
        }
    }
    return false;
}


bool GidWidget::neotr_node(CNode2* node, CFPoint point)
{

    if (!isEditGid()) return false;

    QString q;
    QString tn = "Список неотрисованных узлов";

    int n_ret = 0;

    if (node) {
        if (!isNeotr(node)) {
            QMessageBox::warning(this, "", tr("Все узлы отрисованы"));
            return false;
        }

        QString tn = "Список неотрисованных узлов связанных с выбранным";

        q = QString(
            "SELECT DISTINCT \n"
            "n.id,"
            "ec.name AS %1,"
            "n.externalNodeName AS %2,\n"
            "rc.name AS %3,\n"
            "hps.pipeSectLength AS %4,"
            "fr.name AS %5\n"


            "FROM net.v_nodes n\n"
            "LEFT JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
            "JOIN fragments fr ON fr.id=n.fileID\n"
            "LEFT JOIN net.v_realconsumers rc ON rc.nodeID =n.id\n"
            "JOIN net.v_linesobj l ON(l.nodeID1 = n.id OR l.nodeID2 = n.id) AND(l.nodeID1 = %6 OR l.nodeID2 = %6)\n"
            "LEFT JOIN heatPipeSections hps ON hps.lineID=l.id\n"
            "WHERE n.fileID in (%7) AND (n.x=0 OR n.x IS NULL) AND (n.y=0 OR n.y IS NULL) AND n.internalNodeID IS NULL\n--AND_NODE")
            .arg(quot_text("Код РС"), quot_text("Имя узла"), quot_text("Описание"), quot_text("Длина"), quot_text("Фрагмент"))
            .arg(node->id).arg(m_cxema.m_par);

//        TableWindow t(QSqlDatabase &db, const QString & tn, const QString & title, const QString & que, QWidget *parent, int targetId, bool find_str, bool ok_str, bool is_header, bool autofit);


//        CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_unknown);

        TableWindow tab(m_cxema.m_db, tn, tn, q, this, -1, true, true, true);

        if (tab.exec()) {
            int id = tab.value(0).toInt();

            CNode2* n = m_cxema.m_graph->find(id);
            if (n) {
                m_n1 = node;
                m_n2 = n;

//                m_nRegim = ID_LINE;
                m_nRegim = gidrAction.aLine;
                regim = R_BEGIN_DRAW_LINE;
                m_pt1 = m_pt2 = CFPoint(m_n1->node.coord.x, m_n1->node.coord.y);
                m_NP.clear();
                m_NP.push_back(m_pt1);

                return true;
            }
        }
        return true;
    }



    q = QString(
        "SELECT \n"
        "n.id,"
        "ec.name AS %1,"
        "n.externalNodeName AS %2,\n"
        "rc.name AS %3,\n"
        "fr.name AS %4\n"


        "FROM net.v_nodes n\n"
        "LEFT JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
        "JOIN fragments fr ON fr.id=n.fileID\n"
        "LEFT JOIN net.v_realconsumers rc ON rc.nodeID =n.id\n"
        "WHERE n.fileID in (%5) AND n.x=0 AND n.y=0 AND n.internalNodeID IS NULL\n--AND_NODE")
        .arg(quot_text("Код РС"), quot_text("Имя узла"), quot_text("Описание"), quot_text("Длина"))
        .arg(m_cxema.m_par);

//    CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_unknown);
//    if (dlg.DoModal() == IDOK) {

    TableWindow tab(m_cxema.m_db, tn, tn, q, this, -1, true, true, true);

    if (tab.exec()) {
        int id = tab.value(0).toInt();
        CNode2* n = m_cxema.m_graph->find(id);
        if (n) {
            setXY(n, point);
            repaint();
        }
    }


    return false;
}



double GidWidget::okr(double x, int dX_)
{
//    if (dX == 0) return x;

    if (!m_bIsAstr || m_parent_id == 0 || m_dX == 0) return x;


    return std::round(x / m_dX) * m_dX;
    /*
    if (x < 0) x -= m_dX;

    int


    long xx = (long)((x+m_dX/2)/m_dX*m_dX);

    return (double) xx;
*/
}

bool GidWidget::isMark()
{
    return m_cxema.graph()->isMark();
}


bool GidWidget::isPR()
{
    CNode2 *nc = m_cxema.m_graph->find(m_parent_id);
    if (nc && nc->node.typ == TIP_PR) return true;

    return false;
}

