#include <QApplication>
#include <QtGui>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>

#include <QInputDialog>
#include <any/rights.h>
#include <dialog/ChMenuDial.h>

#include <db/db.h>

#include <property/PropertyDial.h>
#include <any/MyMain.h>

#include "std.h"


void GidWidget::setToolbars()
{
    if (m_barCxema)  gidrAction.aViewToolbarCxema    ->setChecked(m_barCxema ->isVisible());
    if (m_barZapr )  gidrAction.aViewToolbarZ        ->setChecked(m_barZapr  ->isVisible());

    if (m_barDefect) gidrAction.aToolbarRemont2Defect->setChecked(m_barDefect->isVisible());
    if (m_barShurf)  gidrAction.aToolbarRemont2Shurf ->setChecked(m_barShurf ->isVisible());
    if (m_barOsmotr) gidrAction.aToolbarRemont2Osmotr->setChecked(m_barOsmotr->isVisible());
    if (m_barRemont) gidrAction.aToolbarRemont2Remont->setChecked(m_barRemont->isVisible());
    if (m_barOpres)  gidrAction.aToolbarRemont2Opres ->setChecked(m_barOpres ->isVisible());
}

/*
void GidWidget::barOnOff(bool on)
{
    if (m_barCxema)  m_barCxema ->setVisible(on);
    if (m_barZapr )  m_barZapr  ->setVisible(on);
    
    if (m_barDefect) m_barDefect->setVisible(on);
    if (m_barShurf)  m_barShurf->setVisible(on);
    if (m_barOsmotr) m_barOsmotr->setVisible(on);
    if (m_barRemont) m_barRemont->setVisible(on);
    if (m_barOpres)  m_barOpres->setVisible(on);
}
*/   

#if 0
void GidWidget::viewToolbar(bool on, QToolBar *m_bar)
{
    if (!m_bar) return;

    if (on) {
        m_bar->setVisible(true);

        if (m_barCxema  && m_bar != m_barCxema && m_bar != m_barZapr )  m_barCxema ->setVisible(false);
        if (m_barZapr   && m_bar != m_barZapr && m_bar != m_barCxema )  m_barZapr  ->setVisible(false);
                       
        if (m_barDefect && m_bar != m_barDefect)  m_barDefect->setVisible(false);
        if (m_barShurf  && m_bar != m_barShurf )  m_barShurf ->setVisible(false);
        if (m_barOsmotr && m_bar != m_barOsmotr)  m_barOsmotr->setVisible(false);
        if (m_barRemont && m_bar != m_barRemont)  m_barRemont->setVisible(false);
        if (m_barOpres  && m_bar != m_barOpres )  m_barOpres ->setVisible(false);
    }
    else {
        m_bar->setVisible(false);
    }
    setToolbars();
}


void GidWidget::onViewToolbarCxema(bool on) // Схема
{
    viewToolbar(on, m_barCxema);
}
   

void GidWidget::onViewToolbarZ(bool on) // Запросы
{
    viewToolbar(on, m_barZapr);
}
    

void GidWidget::onToolbarRemont2Defect(bool on) // Панель Нарушений
{
    viewToolbar(on, m_barDefect);
}
    

void GidWidget::onToolbarRemont2Shurf(bool on) // Панель Шурфовки
{
    viewToolbar(on, m_barShurf);
}
    

void GidWidget::onToolbarRemont2Osmotr(bool on) // Панель Осмотра
{
    viewToolbar(on, m_barOsmotr);
}
    

void GidWidget::onToolbarRemont2Remont(bool on) // Панель Ремонтов
{
    viewToolbar(on, m_barRemont);
}
    

void GidWidget::onToolbarRemont2Opres(bool on) // Панель Опрессовки
{
    viewToolbar(on, m_barOpres);
}

#endif
 

void GidWidget::onRemontPovrDefAdd() // Установить нарушение
{
    Klassif *kls = m_kl_list.findKlN("defect");

    if (kls) {
        addGeo(kls);
    }
    else {
        QMessageBox::warning(this, tr(""), QString(tr("В системе нет объекта %1").arg("defect")));
    }
}
    
#include <table/DbWindow.h>

//void GidWidget::remontZhur(int idd, const QString & fn, const QString & tn, const QString & date_name, const QString & title, const QString & state_cond, const map<QString, QString>* map_fn, VIRT_DATA typ)
void GidWidget::zhurnal(int idd, const QString & fn, const QString & tn, const QString & title, const map<QString, QString>* map_fn)
{
    QString q = readQ(fn);

    if (map_fn) {
        for (auto & it : *map_fn) {
            q.replace(it.first, it.second);
        }
    }

//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);
    DbWindow *view = getTableView(m_cxema.m_db, tn, q, title);

    if (view) {
        view->setGeo(tn);
        view->setGidWidget(this);
        main_window->addWidgetTab(view, title, Qt::BottomDockWidgetArea);
    }
}

QString sezon_name(int y);

void GidWidget::ispr_remont_q(std::map<QString, QString> &map_fn)
{
//    int m_sezon_korrozia = 2022;

    map_fn["$state_cond$"] = "";
    map_fn["$and_utverdit$"] = "";

    map_fn["$and_n1_fileID$"] = QString(" AND n1.fileID in (%1) ").arg(m_cxema.m_par);

    QString season_condition = " IS NULL OR 1=1";

//    QDateTime m_sezon_date1(QDate(m_sezon_korrozia, 10, 15), QTime());
//    QDateTime m_sezon_date2(QDate(m_sezon_korrozia+1, 4, 15), QTime());

    Heating_seasons h = m_cxema.get_Heating_seasons(m_sezon_korrozia);

    if (h.y > 0) {
        QString ds1 = h.d0.toString("yyyyMMdd");
        QString ds2 = h.d2.toString("yyyyMMdd");

        if (m_sezon_korrozia > 0) {
            season_condition = QString(" BETWEEN '%1' AND '%2' ").arg(ds1, ds2);
        }
    }
    map_fn["$season_condition$"] = season_condition;
    map_fn["$and_condition2$"] = "";
}

void GidWidget::remontZhurnal(int idd, const QString & fn, const QString & tn, const QString & title, const QString & state_cond)
{
    std::map<QString, QString> map_fn;
    ispr_remont_q(map_fn);
    map_fn["$state_cond$"] = " " + state_cond + " ";

    zhurnal(idd, fn, tn, title, &map_fn);
}



void GidWidget::onDefectZhurnalExpluatacia() // Нарушения эксплуатации
{
    remontZhurnal(20, "sql/objects/defect/defects_journal.sql", "defect", QString("Нарушения эксплуатации на тепловой сети. %1").arg(sezon_name(m_sezon_korrozia)), " (remontTypeID = 1) AND ");
}


void GidWidget::onDefectZhurnalShurf() // Нарушения в шурфе
{
    remontZhurnal(21, "sql/objects/defect/shurf_defect_journal.sql", "defect", QString("Нарушения в шурфе на тепловой сети. %1").arg(sezon_name(m_sezon_korrozia)), " (remontTypeID = 3) AND ");
}
    

void GidWidget::onDefectZhurnalOsmotr() // Нарушения осмотра
{
    remontZhurnal(22, "sql/objects/defect/defects_journal.sql", "defect", QString("Нарушения осмотра на тепловой сети. %1").arg(sezon_name(m_sezon_korrozia)), " (remontTypeID = 6) AND ");
}
    

void GidWidget::onDefectZhurnalOpress() // Нарушения опрессовки
{
    remontZhurnal(23, "sql/objects/defect/defects_journal.sql", "defect", QString("Нарушения опрессовки на тепловой сети. %1").arg(sezon_name(m_sezon_korrozia)), " (remontTypeID = 2) AND ");
}
    

void GidWidget::onDefectZhurnalNarushenie() // Нарушения до ремонта
{
    remontZhurnal(24, "sql/objects/defect/defects_journal.sql", "defect", QString("Выявленные нарушения на тепловой сети. %1").arg(sezon_name(m_sezon_korrozia)), " (stateID = 1) AND ");
}
    

void GidWidget::onDefectZhurnalRemont() // Нарушения в процессе ремонта
{
    remontZhurnal(25, "sql/objects/defect/defects_journal.sql", "defect", QString("Нарушения в процессе ремонта на тепловой сети. %1").arg(sezon_name(m_sezon_korrozia)), " (stateID = 2) AND ");
}
    

void GidWidget::onDefectZhurnalPosleRemont() // Нарушения после ремонта
{
    remontZhurnal(26, "sql/objects/defect/defects_journal.sql", "defect", QString("Нарушения после ремонта на тепловой сети. %1").arg(sezon_name(m_sezon_korrozia)), " (stateID = 3) AND ");
}
    

void GidWidget::onDefectZhurnalAll() // Отобразить все нарушения
{
    remontZhurnal(27, "sql/objects/defect/defects_journal.sql", "defect", QString("Все нарушения на тепловой сети. %1").arg(sezon_name(m_sezon_korrozia)), "");
}
    

void GidWidget::onDefectWord1() // Журнал нарушений
{
}
    

void GidWidget::onDefectWord2() // Нарушения/ремонты трубопроводов тепловой сети(документ)
{
}
    

void GidWidget::onDefectDocTable() // Нарушения/ремонты трубопроводов тепловой сети
{
}
    

void GidWidget::onDefectAnaliz() // Анализ нарушений
{
}

const map<QString, Klassif*> *init_pts_list_file(const QString & fn1);


void GidWidget::geoOnOff(const map<QString, Klassif*> *map_pts, const QString & title)
{
    ChMenuDial dlg(this, title);

    for (auto & it : *map_pts) {
        Klassif* kls = it.second;
        dlg.Add(kls->RusName(), QVariant::fromValue(kls), kls->ins);
    }

    Klassif *kls0 = nullptr;

    if (dlg.exec()) {
        for (auto & it : *map_pts) {
            Klassif* kls = it.second;
            kls->ins = dlg.inOut(QVariant::fromValue(kls));
            kls0 = kls;
        }
        if (kls0) {
            QString klfn = QString("%1/%2.kls2").arg(argpath_2(), kls0->baza);
            m_kl_list.save2(klfn, kls0->baza);
            main_window->m_left_geo->reset();
            repaint();
        }
    }
}


void GidWidget::geoOnOffFile(const QString & file_name, const QString & title)
{
    const map<QString, Klassif*> *map_pts = init_pts_list_file(file_name);

    if (map_pts) {
        geoOnOff(map_pts, title);
    }
}


void GidWidget::onRemontPovrDefOtobr() // Включить/отключить отображение объектов
{
    geoOnOffFile("remont", tr("Выберите объекты ремонтов"));
}



int MMenuQ(QWidget *widget, const QString & title, QSqlDatabase &db, const QString & q, QString &out, bool *ok);

#include <dialog/TableDialog.h>
    

void GidWidget::onRemontPovrOtop() // Выбор отопительного сезона
{
    TableDialog dlg(tr("Выберите сезон"), this);
    dlg.addColumn(tr("Сезон"));
    dlg.addColumn(tr("Дата\nначала сезона"));
    dlg.addColumn(tr("Дата\nокончания сезона"));

     int row = dlg.addRow();
     dlg.setId(row, -1);
     dlg.setValue(row, 0, "Все сезоны");
     dlg.setCurrentRow(row);

    for (auto & [y, h] : m_cxema.map_Heating_seasons) {
        int row = dlg.addRow();
        dlg.setId(row, y-1);
        dlg.setValue(row, 0, QString("%1-%2").arg(y-1).arg(y));
        dlg.setValue(row, 1, h.d0.toString("dd.MM.yyyy"));
        dlg.setValue(row, 2, h.d1.toString("dd.MM.yyyy"));

        if (m_sezon_korrozia == y-1) {
            dlg.setCurrentRow(row);
        }
    }

    dlg.init();

    if (dlg.exec() != QDialog::Accepted) return;

//    QMessageBox::information(this, "", QString("%1").arg(dlg.m_id.toInt()));

    m_sezon_korrozia = dlg.m_id.toInt();
    QSettings settings;
    settings.setValue("flags/season_korrozia", m_sezon_korrozia);
    setTitle();


#if 0

//    QString q = "SELECT id, YEAR(date1) AS dt, CONCAT(YEAR(date1), '-', YEAR(date2)) AS name, date1, date2 FROM heating_seasons ORDER BY date1";
    QString q = QString(
        "SELECT -1 AS id, '%1' AS name UNION SELECT YEAR(date1) AS id, CONCAT(YEAR(date1), '-', YEAR(date2)) AS name\n"
        "FROM heating_seasons\n"
        "ORDER BY id"
        ).arg(tr("Все сезоны"));
    QString out;
    bool ok;
    int y = MMenuQ(this, "Выберите сезон", m_cxema.m_db, q, out, &ok);
    if (ok) {
        m_sezon_korrozia = y;
        QSettings settings;
        settings.setValue("flags/season_korrozia", m_sezon_korrozia);
        setTitle();
    }
#endif


}
    

void GidWidget::onRemontOsmotrTeploSet() // Выбор фрагмента сети по Начальнику участка
{
}
    

void GidWidget::onDefectBezUch() // Нарушения без участков
{
}
    

void GidWidget::onRemontPovrShurfAdd() // Установить плановый шурф
{
    Klassif *kls = m_kl_list.findKlN("shurfy");

    if (kls) {
        std::map<QString, QVariant> map_value;

        map_value["naznachenie_vskrID"] = 1;
        map_value["sostoyanie_shurfaID"] = 1;
        map_value["utverdit"] = 0;

        addGeo(kls, &map_value);
    }
    else {
        QMessageBox::warning(this, tr(""), QString(tr("В системе нет объекта %1").arg("shurfy")));
    }
}
    

void GidWidget::onRemontPovrShurfAddNeplan() // Установить НЕплановый шурф
{
    Klassif *kls = m_kl_list.findKlN("shurfy");

    if (kls) {
        std::map<QString, QVariant> map_value;

        map_value["naznachenie_vskrID"] = 2;
        map_value["sostoyanie_shurfaID"] = 2;
        map_value["utverdit"] = 3;

        addGeo(kls, &map_value);
    }
    else {
        QMessageBox::warning(this, tr(""), QString(tr("В системе нет объекта %1").arg("shurfy")));
    }
}
    
bool init_pts_table(const QString &  tab);

   
//-----------------------------------------------------------------------

void GidWidget::remontZhurnalShurf(int idd, const QString & fn, const QString & tn, const QString & title, const QString & state_cond)
{
    std::map<QString, QString> map_fn;
    ispr_remont_q(map_fn);
    map_fn["$and_naznachenie_vskrID$"] = " " + state_cond + " ";

    zhurnal(idd, fn, tn, title, &map_fn);
}

void GidWidget::onRemontShurfPlan() // Плановые шурфы
{
    QString s = QString("План шурфов. %1").arg(sezon_name(m_sezon_korrozia));
    remontZhurnalShurf(1, "sql/objects/defect/shurf_journal.sql", "shurfy", s, " AND d.naznachenie_vskrID = 1 ");
}
   

void GidWidget::onRemontShurfPredpis() // Шурфы предписания
{
    QString s = QString("Шурфы по предписанию. %1").arg(sezon_name(m_sezon_korrozia));
    remontZhurnalShurf(2, "sql/objects/defect/shurf_journal.sql", "shurfy", s, " AND d.naznachenie_vskrID = 2 ");
}
   

void GidWidget::onRemontShurfNarush() // Шурфы нарушения
{
    QString s = QString("Шурфы по нарушению. %1").arg(sezon_name(m_sezon_korrozia));
    remontZhurnalShurf(3, "sql/objects/defect/shurf_journal.sql", "shurfy", s, " AND d.naznachenie_vskrID = 3 ");
}

void GidWidget::onShurfProcess() // Шурфы в процессе выполнения
{
    QString s = QString("Шурфы в процессе. %1").arg(sezon_name(m_sezon_korrozia));
    remontZhurnalShurf(11, "sql/objects/defect/shurf_journal.sql", "shurfy", s, " AND d.sostoyanie_shurfaID = 2 ");
}

void GidWidget::onShurfVypolneno() // Шурфы выполненные
{
    QString s = QString("Шурфы выполненные. %1").arg(sezon_name(m_sezon_korrozia));
    remontZhurnalShurf(12, "sql/objects/defect/shurf_journal.sql", "shurfy", s, " AND d.sostoyanie_shurfaID = 3 ");
}
    

void GidWidget::onShurfAll() // Отобразить все Шурфы
{
}
    

void GidWidget::onRemontShurfPlanUtverdit() // Утвердить План шурфов Отопительного сезона
{
    auto *dlg = new PropertyDial(this, "shurf_utverdit", "", "shape", "id", -1, -1);

    std::map<QString, QVariant> map_val;

    dlg->init_dialog(&m_cxema.m_db, "Утвердить План шурфов Отопительного сезона", "shurf_utverdit", map_val);
    dlg->show();

    connect(dlg, &QDialog::accepted, [&]()
        {
//            if (update_db(&m_cxema, dlg->map_edited)) {
//                repaint();
//            }

            QMessageBox::warning(this, "", "!");
        }
    );
}


    

void GidWidget::onRemontShurfPlanExcel() // План шурфов (документ)
{
}
    

void GidWidget::onRemontShurfPlanExcelMonth() // План шурфов по месяцам (документ)
{
}
    

void GidWidget::onShurfTable() // Анализ Плана шурфов
{
}
    

void GidWidget::onRemontShurfOtobr() // Включить/отключить отображение объектов
{
}


    

void GidWidget::onShurfBezUch() // Шурфовки без участков
{
}
    

void GidWidget::onOsmotrAdd() // Создать контур осмотра
{
}
    

void GidWidget::onOsmotrSave() // Обновить сохранение контура
{
}
    

void GidWidget::onListOsmotr2() // Выполнить осмотр контура
{
    int m_id_osmotr2;

    ListOpres("osmotr", tr("Выберите контур для Осмотра"), "name", "data_osmotra", m_id_osmotr2);
}
    

void GidWidget::onLastOsmotr2() // Последний контур осмотра
{
    int m_id_osmotr2, m_id_osmotr2_old;

    LastOpres("osmotr", tr("Контур осмотра"), m_id_osmotr2, m_id_osmotr2_old, true);
}
    

void GidWidget::onLastFaktory() // Выбор трубопроводов для осмотра
{
}
    

void GidWidget::onOsmotrZapros1() // Осмотренные фрагменты сети
{
}
    

void GidWidget::onOsmotrZapros2() // Осмотренные трубопроводы за период
{
}
    

void GidWidget::onOsmotrZapros3() // Неосмотренные трубопроводы за период
{
}
    

void GidWidget::onOsmotrAnaliz2() // Анализ осмотра контуров
{
}
    

void GidWidget::onOsmotrAnaliz() // Анализ осмотра трубопроводов контура
{
}


void GidWidget::onRemontAddPlan() // Сохранить контур плана ремонта
{
}
    

void GidWidget::onRemontAddCurrent() // Сохранить контур текущего ремонта
{
}
    

void GidWidget::onRemontSave() // Обновить сохранение контура
{
}
    

void GidWidget::onListRemont2() // Список контуров ремонтов
{
    int m_id_remont2;

    ListOpres("remont2", tr("Контуры ремонта"), "otchet_po_defektu", "data_osmotra", m_id_remont2);
}
    

void GidWidget::onLastRemont2() // Последний контур ремонта
{
    LastOpres("remont2", tr("Контуры ремонта"), m_id_remont2, m_id_remont2_old, false);
}
    

void GidWidget::onRemontTrub() // Выбор трубопроводов контура ремонта
{
}
    

void GidWidget::onRemontPlan() // Контуры капитального/инвестиционного ремонтов
{
}
    

void GidWidget::onRemontCurrent() // Контуры текущих ремонтов
{
}
    

void GidWidget::onRemontProcess() // Ремонт в процессе выполнения
{
}
    

void GidWidget::onRemontVypolneno() // Законченные ремонты
{
}
    

void GidWidget::onRemontAll2() // Отобразить все контура
{
}
    

void GidWidget::onRemontWord1() // График выполнения ремонтов (документ)
{
}
    

void GidWidget::onRemontWord2() // План капитальных/инвестиционных ремонтов (документ)
{
}
    

void GidWidget::onRemontWord3() // План ремонтов по месяцам (документ)
{
}
    

void GidWidget::onRemontAnaliz2() // Анализ контуров ремонта
{
}
    

void GidWidget::onRemontAnaliz() // График выполнения ремонтов
{
}


    

void GidWidget::onRemontVyborNach() // Создать контур ремонта
{
}
    

void GidWidget::onMarkRev() // Добавить/удалить трубопровод к контуру
{
}
    

void GidWidget::onOpresAddPlan() // Сохранить контур плана опрессовки
{
}
    

void GidWidget::onOpresSave() // Обновить сохранение контура
{
}
    

void GidWidget::onOpresGranitsaRazdela() // Границы раздела
{
}

QString set_to_text(std::set<int>& set_i);


void GidWidget::onListOpres() // Список контуров опрессовок
{
    int m_id_opr;

    ListOpres("opres", tr("Контуры опрессовок"), "name", "date_opres", m_id_opr);
}
    

void GidWidget::onLastOpres() // Последний контур опрессовки
{
    LastOpres("opres", tr("Контуры опрессовок"), m_id_opr, m_id_opr_old, false);
}
    

void GidWidget::onOpresPlan() // Контуры планов опрессовок сезона
{
}
    

void GidWidget::onOpresProcess() // Опрессовка в процессе выполнения
{
}
    

void GidWidget::onOpresVypolneno() // Завершенные опрессовки
{
}
    

void GidWidget::onOpresAll2() // Отобразить все контура
{
}
    

void GidWidget::onOpresWord1() // План опрессовок (документ)
{
}
    

void GidWidget::onOpresWord2() // График опрессовок по месяцам (документ)
{
}
    

void GidWidget::onOpresAnaliz2() // Анализ контуров опрессовок
{
}
    

void GidWidget::onOpresAnaliz() // Анализ нарушений контура
{
}
    


void GidWidget::onOpresVyborNach() // Создать контур опрессовки
{
}
    


void GidWidget::onRemontPovrKarta() // Карта нарушений
{
}
    

void GidWidget::onRemontPovrKarta2() // Карта осмотра шурфа
{
}
    

