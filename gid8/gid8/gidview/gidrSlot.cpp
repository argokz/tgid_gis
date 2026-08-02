#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <QProgressDialog>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>

#include <QInputDialog>
#include <any/rights.h>

#include <db/db.h>

#include "std.h"
#include <dialog/TableWindow.h>
#include <table/DbWindow.h>
#include <cxema/undo.h>

#include <dialog/ChMenuDial.h>
#include <ui/Ch_password_right_newDialog.h>


#define INDIKATOR_KORROZII "indikator_korrozii"
QString transl(const QString & s);
bool isEditGeo(const QString & tn, bool isMessage);


void GidWidget::onSqlSave() // Создание копии Базы данных
{
}


void GidWidget::onSqlCopyFragment() // Вставить Фрагмент схемы
{
}


int MMenuQ(QWidget *widget, const QString & title, QSqlDatabase &db, const QString & q, QString &out, bool *ok)
{
    MMenuDial md(widget, title);
//    md.AddButton("Удалить", -1);
    QSqlQuery query(db);
    out = "";

//    QString q = QString("Select id, name From directions WHERE fileID in (%1)").arg(m_cxema.m_par);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            md.Add(query.value(1).toString(), query.value(0).toInt());
        }
    }

    if (md.exec()) {
        *ok = true;
        out = md.curText;
        return md.value().toInt();
    }
    *ok = false;
    return -1;
}

void ErrorMessage(QSqlQuery &query);
bool delObject(QSqlDatabase &db, const QString & tn, int id);


void GidWidget::onSqlDelFragment() // Удалить Фрагмент схемы
{
  if (!UserRight::isAdmin()) {
      QMessageBox::warning(this, "", tr("Для выполнения этой операции у вас должны быть права администратора!"));
      return;
  }

  bool ok;
  QString str, name;

  int id = MMenuQ(this, tr("Список фрагментов"), m_cxema.m_db, QString("SELECT id, name FROM fragments WHERE id IN (%1) ORDER BY name").arg(m_cxema.m_par), name, &ok);

  if (ok) {
    str = QString(tr("Удалить фрагмент \"%1\" из текущей базы %2?\nВнимание! Восстановление будет невзможно!")).arg(name).arg(getDatabaseName(m_cxema.m_db));

    if (QMessageBox::question(this, "", str)  == QMessageBox::Yes) {
        int fileID = id;
        if (delObject(m_cxema.m_db, "fragments", fileID)) {
            closeFragment(fileID);
            QMessageBox::warning(this, "", "Готово");
        }
        else {
            QSqlQuery query(m_cxema.m_db);
            ErrorMessage(query);
        }
    }
  }
}

QString python_exe();


void GidWidget::onAddFragment() // Добавить Фрагмент схемы
{
  if (!UserRight::isAdmin()) {
      QMessageBox::warning(this, "", tr("Для выполнения этой операции у вас должны быть права администратора!"));
      return;
  }

  QString q = QString("SELECT id, name FROM fragments WHERE removed = 0 AND id IN (%1) ORDER BY name").arg(m_cxema.m_par);

  QSqlQuery query(m_cxema.m_db);
  query.setForwardOnly(true);
  query_exec(m_cxema.m_db, query, q);


  ChMenuDial dlg(this, tr("Выберите фрагменты для объединения"));

//  QStringList par_list = m_cxema.m_par.split(",");

  while (query.next()) {
      QString name = query.value("name").toString();
      int id = query.value("id").toInt();
      dlg.Add(name, id, false);
  }

  if (!dlg.exec()) return;


  int n = 0;

  QString par2 = "";

  for (auto & it : dlg.out) {
      if (par2 != "") par2 += " ";
      par2 += QString("%1").arg(it.toString());
      n += 1;
  }


  if (n < 2) {
      QMessageBox::warning(this, "", tr("Для выполнения этой операции вы должны выбрать два или более фрагментов"));
      return;
  }

  QString outName = QString("%1/out.txt").arg(QDir::tempPath());

  QString path = QString("%1 \"%2import_mdb/unite_tgid.py\"").arg(python_exe(), argpath());

  QString args = QString(" -out_file \"%1\" -files %2")
      .arg(outName, par2);

  QString str = str_python("unite/unite_tgid.py", args);

  main_window->start(str, this, SLOT(onFinishedImport(int, QProcess::ExitStatus)));
}



void GidWidget::onExportFragmentsMulty() // Экспорт всех фрагментов
{
}


/*
void GidWidget::onImportFragment() // Импорт фрагмента
{
}
*/

/*

void GidWidget::onImportFragmentMdb() // Конвертор ТГИД-05/ ТГИД-07
{
}
*/




QString menuFragment(QWidget *parent, QSqlDatabase &db, const QString & par1);

std::set<int> getList(const QString & str)
{
    QStringList list1 = str.split(QLatin1Char(','));

    std::set<int> lst;

    for (auto it : list1) {
        lst.insert(it.toInt());
    }
    return lst;
}

void GidWidget::onAddDop() // Отобразить дополнительную схему
{
    QString par = menuFragment(this, m_cxema.m_db, m_cxema.m_par);
    if (par == "") return;

    qDebug() << "onAddDop() begin";

    reopenFragments(m_cxema.m_par, par);
    qDebug() << "onAddDop() end";
}



void GidWidget::onBmpFind() // Найти графическую подложку
{
}


void Remont_docx1(QWidget* wnd, QSqlDatabase &db, const QString & q, int m_sezon_korrozia);
QString table_to_q_for_remont(const QString & fn);


void GidWidget::onCatalog() // Каталоги
{
/*
    QString fn = "sql/objects/defect/grafik_group.sql";
    QString q = table_to_q_for_remont(fn);
    std::map<QString, QString> map_fn;
    Remont_docx1(this, m_cxema.m_db, q, 2022);
*/
}


#include <ui/Print_frDialog.h>

void GidWidget::onPrintFr() // Фрагмент для печати
{
    CPrint_frDialog dlg(this);

    QSettings settings;

    dlg.ui.combo_Paper->addItem("A4");
    dlg.ui.combo_Paper->addItem("A3");
    dlg.ui.combo_Paper->addItem("A2");
    dlg.ui.combo_Paper->addItem("A1");
    dlg.ui.combo_Paper->addItem("A0");

    dlg.m_cmbPaper = settings.value("print_form/paper", 0).toInt();;

    dlg.ui.combo_Mas->addItem("500");
    dlg.ui.combo_Mas->addItem("1000");
    dlg.ui.combo_Mas->addItem("2000");
    dlg.ui.combo_Mas->addItem("5000");
    dlg.ui.combo_Mas->addItem("10000");

    dlg.m_cmbMas = settings.value("print_form/mas", 0).toInt();;

    dlg.m_radioPortret = settings.value("print_form/portrait", 0).toInt();;;

    dlg.write();

    if (dlg.exec() == QDialog::Accepted) {

        QSize l_size[] = {
          QSize(210, 297),  // A4
          QSize(297, 420),  // A3
          QSize(420, 594),  // A2
          QSize(594, 840),  // A1
          QSize(840, 1188), // A0
        };

        double l_mas[] = {500, 1000, 2000, 5000, 10000,};



        settings.setValue("print_form/paper",  dlg.m_cmbPaper);
        settings.setValue("print_form/mas",  dlg.m_cmbMas);
        settings.setValue("print_form/portrait",  dlg.m_radioPortret);

        m_nRegim = gidrAction.aPrintFr;
        regim = R_BEGIN_RECT_1;

        m_print_mas = l_mas[dlg.m_cmbMas];
        m_print_size = l_size[dlg.m_cmbPaper];

        m_print_size = QSize(
            m_print_size.width(),
            m_print_size.height()
            );


        if (dlg.m_radioPortret == 1) {
            m_print_size = QSize(m_print_size.height(), m_print_size.width());
        }


    }
}


void GidWidget::onFragment() // Выделить фрагмент
{
}


void GidWidget::onBdDel() // Удалить базу
{
#if 0
    CConnect cn = getCxema()->m_connect;
    QSqlDatabase db = getCxema()->m_db;
    bool ok;
    QString str, name;

    QString q = "SELECT datname FROM pg_database where not datistemplate order by datname";
    //    MMenuDial  m(this, "Выберите Базу данных для удаления");
    int id = MMenuQ(this, tr("Выберите Базу данных для удаления"), m_cxema.m_db, q, name, &ok);

    if (name == cn.baza) {
        QMessageBox::warning(this, "", );
    }



    if (QMessageBox::question(0, "", QString(tr("Удалить Базу Данных %1?")).arg(name)) != QMessageBox::Yes) return;
    if (QMessageBox::question(0, "", tr("Внимание!!! После выполнения комманды выбранная база данных будет удалена! Восстановить ее будет невозможно! Продолжить выполнение")) != QMessageBox::Yes) return;



    QString q1 = QString("SELECT pg_terminate_backend(pid) FROM pg_stat_activity WHERE datname = '%1';").arg(m);
    if (query_exec(db, q1)) {
    }

    QString q2 = QString("DROP DATABASE %1;").arg(cn.baza);

//    QMessageBox::warning(this, "", q2);
#endif
}


void GidWidget::onFilePrintSetup() // Параметры страницы...
{
}


void GidWidget::onAppExit() // Выход
{
}

void resetGid(CCxema * m_cxema, const QString & m_user_gid, int &n_nodes, int n_lines);


void GidWidget::onResetGid()
{
//    CString m_user_gid = pDoc->m_user_gid;
   if (QMessageBox::question(this, "", "Обновить изображение из базы данных") != QMessageBox::Yes) {
        return;
   }

    int n_nodes = 0, n_lines = 0;

    endDraw();

    resetGid(&m_cxema, "", n_nodes, n_lines);
    repaint();
    QMessageBox::warning(this, "", QString(tr("Синхронизация выполнена\n Узлов: %1, Участков: %2")).arg(n_nodes).arg(n_lines));
}

bool conv_tgid_qgis(QSqlDatabase &db1, const QString &name);

#include <config.h>
void init_cs(ConnectStr &cs);


#include <QApplication>
#include <QtWidgets>
#include <QtSql>

class DbConnectDialog : public QDialog {
    Q_OBJECT
public:
    DbConnectDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Выбор базы данных для синхронизации");

        ConnectStr cs;

        init_cs(cs);

        InitConfig *ini =  get_ini();


        auto *formLayout = new QFormLayout;

        hostEdit = new QLineEdit(cs.host);
        portEdit = new QLineEdit(QString("%1").arg(cs.port));
        dbEdit   = new QLineEdit(get_baza_gid9());
        userEdit = new QLineEdit(cs.user);
        passEdit = new QLineEdit(cs.password);
        passEdit->setEchoMode(QLineEdit::Password);

        hostEdit->setEnabled(false);
        portEdit->setEnabled(false);
//        dbEdit  ->setEnabled(false);
        userEdit->setEnabled(false);
        passEdit->setEnabled(false);

        formLayout->addRow("Имя хоста/IP:", hostEdit);
        formLayout->addRow("Порт:", portEdit);
        formLayout->addRow("База данных:", dbEdit);
        formLayout->addRow("Пользователь:", userEdit);
        formLayout->addRow("Пароль:", passEdit);

        auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(buttonBox, &QDialogButtonBox::accepted, this, &DbConnectDialog::tryConnect);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &DbConnectDialog::reject);

        auto *layout = new QVBoxLayout;
        layout->addLayout(formLayout);
        layout->addWidget(buttonBox);
        setLayout(layout);
    }

private slots:
    void tryConnect() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
        db.setHostName(hostEdit->text());
        db.setPort(portEdit->text().toInt());
        db.setDatabaseName(dbEdit->text());
        db.setUserName(userEdit->text());
        db.setPassword(passEdit->text());

        if (db.open()) {
            if (checkTableExists(db, "gid", "nodes")) {
                set_baza_gid9(dbEdit->text());

                QMessageBox::information(this, "", QString("База %1 используется для синхронизации!").arg(dbEdit->text()));
                accept();
            }
            else {
                QMessageBox::information(this, "", QString("База %1 не предназначена для синхронизации!").arg(dbEdit->text()));
            }
        }
        else {
            QMessageBox::critical(this, "Ошибка", "Подключение невозможно:\n" + db.lastError().text());
        }
    }

private:
    QLineEdit *hostEdit;
    QLineEdit *portEdit;
    QLineEdit *dbEdit;
    QLineEdit *userEdit;
    QLineEdit *passEdit;
};



void GidWidget::onSetupGid9 ()   // "Настройка",
{
    DbConnectDialog dlg(this);

    dlg.exec();

}


void GidWidget::onToGid9()
{
//    CString m_user_gid = pDoc->m_user_gid;
//    resetGid(&m_cxema, "");

   if (QMessageBox::question(this, "", "Синхронизировать базу данных с учётом внесённых изменений?") != QMessageBox::Yes) {
        return;
   }


    endDraw();

    bool ret = conv_tgid_qgis(m_cxema.m_db, get_baza_gid9());

    if (ret) {
        QMessageBox::warning(this, "", tr("Синхронизация выполнена"));
    }
    else {
        QMessageBox::warning(this, "", tr("Ошибка синхронизации"));
    }
    repaint();
}




void GidWidget::onEditUndoAccepted()
{
}

bool undo_gid(QSqlDatabase &db, int id, int &fileID);

bool rollbackGroup(QSqlDatabase &db, const QString &groupId) {
    QSqlQuery query(db);
    query.prepare("SELECT rollback_group(:group_id)");
    query.bindValue(":group_id", groupId);

    if (!query.exec()) {
        qDebug() << "Rollback failed for group_id" << groupId << ":" << query.lastError().text();
        return false;
    }
    qDebug() << "Rollback completed for group_id" << groupId;
    return true;
}

bool rollbackChange(QSqlDatabase &db, int logId) {
    QSqlQuery query(db);
    query.prepare("SELECT rollback_change(:log_id)");
    query.bindValue(":log_id", logId);

    if (!query.exec()) {
        qDebug() << "Rollback failed for log_id" << logId << ":" << query.lastError().text();
        return false;
    }
    qDebug() << "Rollback completed for log_id" << logId;
    return true;
}

static const QSet<QString> nS = {"realconsumers","generalizedconsumers","heatsources","pumpstations","connectnodes"};
static const QSet<QString> lS = {"heatpipesections","pumps","pressregulators","consumptregulators","dampers","regularmatures","bypass","diaphragms","elevators","systemradiators","heatexchangers","airheaters","pressdropregulators","reversevalves"};

CNode2* get_undo_node(CGraph2 *graph, const QString &table, int id)
{
    CNode2 *n = nullptr;
    CLINE2 *l = nullptr;

    if (table == "nodes") {
        n = graph->find(id);
    }

    if (nS.contains(table.toLower())) {
        graph->init_find_node_2();
        n = graph->find_node_2(id);
    }


    if (table == "linesobj") {
        graph->init_find_line_nom();
        l = graph->find_line_nom(id);
    }

    if (lS.contains(table.toLower())) {
        graph->init_find_line_nom_hps();
        l = graph->find_line_nom_hps(id);
    }



    if (l && !n) {
        n = where(l);
    }

    return n;
}

int get_undo_node_fileID(QSqlDatabase &db, const QString &table, int id)
{
    if (table == "nodes") {
        return readTableVariant(db, QString("select fileID from net.v_nodes where id=%1").arg(id)).toInt();
    }
    if (table == "linesobj") {
        return readTableVariant(db, QString("select n1.fileID from net.v_linesobj l join net.v_nodes n1 on n1.id=l.nodeid1 where l.id=%1").arg(id)).toInt();
    }

    if (nS.contains(table.toLower())) {
        return readTableVariant(db, QString("select fileID from net.v_nodes where nodeID=%1").arg(id)).toInt();
    }

    if (lS.contains(table.toLower())) {
        return readTableVariant(db, QString("select n1.fileID from net.v_linesobj l join net.v_nodes n1 on n1.id=l.nodeid1 where l.lineID=%1").arg(id)).toInt();
    }

    return 0;
}

void reset_geo(QSqlDatabase &db, QWidget *w);


void GidWidget::onEditUndoTU() // Отменить\tCtrl+Z
{
    QString tn = transl("Здания ТУ");
    if (!isEditGeo(tn, true)) {
        return;
    }

    QString q = readQ("undo/undo_tu.sql");


//    std::vector<QVariant> vv;

//    bool yes = readTableValues(m_cxema.m_db, q, int n, )


    QMap<QString, QVariant> vals;
    bool yes = fetchFirstRow(m_cxema.m_db, q, vals);


//    TableWindow * tab = new TableWindow(m_cxema.m_db, "changed_object", "Отмена", q, this, 0, false, true, true);

//    if (tab->exec() == QDialog::Accepted) {
    if (yes) {
        int logID = vals["log_id"].toInt();
        bool ret = false;

        QString groupId = vals["Группа"].toString();
        QString table = vals["Таблица"].toString();
        int id = vals["Номер строки"].toInt();
        QString operation = vals["operation"].toString();
        QString comment = vals["Комментарий"].toString();

        QString date = vals["Дата"].toString();


        QString msg = QString("Выполнить откат изменения выполненного %1?").arg(date);

        if (operation == "DELETE") {
            msg = QString("Отменить удаление выполненное %1").arg(date);
        }
        if (operation == "UPDATE") {
            msg = QString("Отменить удаление выполненное %1").arg(date);
        }
        if (operation == "INSERT") {
            msg = QString("Отменить добавление выполненное %1").arg(date);
        }

        if (comment != "") {
            msg = QString("Выполнить откат %1 выполненного %2?").arg(comment).arg(date);
        }

//        QMessageBox::warning(this, "", QString("%1 %2 ").arg(table).arg(id));

        if (QMessageBox::question(this, "", msg) == QMessageBox::Yes) {
            if (groupId  != "") {
                ret = rollbackGroup(m_cxema.m_db, groupId);
            }
            else {
                ret =  rollbackChange(m_cxema.m_db, logID);
            }
            if (!ret) {
                QMessageBox::warning(this, "", "Ошибка");
            }


            Klassif *kls = m_kl_list.findKlN(table);

            if (kls && table != "nodes" && table != "linesobj") {
                kls->open = false;
                reset_geo(*kls->m_db, this);
                repaint();
                return;
            }
        }
    }
}

void GidWidget::onEditUndo() // Отменить\tCtrl+Z
{
#if 1
    if (!isEditGid()) return;


    QString q = readQ("undo/undo.sql");

    QMap<QString, QVariant> vals;
    bool yes = fetchFirstRow(m_cxema.m_db, q, vals);


//    TableWindow * tab = new TableWindow(m_cxema.m_db, "changed_object", "Отмена", q, this, 0, false, true, true);


//    if (tab->exec() == QDialog::Accepted) {
    if (yes) {
        int logID = vals["log_id"].toInt();
        bool ret = false;

        QString groupId = vals["Группа"].toString();
        QString table = vals["Таблица"].toString();
        int id = vals["Номер строки"].toInt();
        QString operation = vals["operation"].toString();
        QString comment = vals["Комментарий"].toString();

        QString date = vals["Дата"].toString();


        QString msg = QString("Выполнить откат изменения выполненного %1?").arg(date);

        if (operation == "DELETE") {
            msg = QString("Отменить удаление выполненное %1").arg(date);
        }
        if (operation == "UPDATE") {
            msg = QString("Отменить удаление выполненное %1").arg(date);
        }
        if (operation == "INSERT") {
            msg = QString("Отменить добавление выполненное %1").arg(date);
        }

        if (comment != "") {
            msg = QString("Выполнить откат %1 выполненного %2?").arg(comment).arg(date);
        }

//        QMessageBox::warning(this, "", QString("%1 %2 ").arg(table).arg(id));

        if (QMessageBox::question(this, "", msg) == QMessageBox::Yes) {

            if (groupId  != "") {
                ret = rollbackGroup(m_cxema.m_db, groupId);
            }
            else {
                ret =  rollbackChange(m_cxema.m_db, logID);
            }
            if (!ret) {
                QMessageBox::warning(this, "", "Ошибка");
            }

            Klassif *kls = m_kl_list.findKlN(table);


            if (kls && table != "nodes" && table != "linesobj") {
                kls->open = false;
                reset_geo(*kls->m_db, this);
                return;
            }


            CNode2 *n = get_undo_node(m_cxema.graph(), table, id);
            int fileID = -1;

            if (n) {
                fileID = n->node.fileID;
            }
            else {
    //            QMessageBox::warning(this, "", QString("Не могу найти %1 %2 ").arg(table).arg(id));
                fileID = get_undo_node_fileID(m_cxema.m_db, table, id);
            }

            if (fileID >= 0) {
                endDraw();

                QString par = QString("%1").arg(fileID);
                //            CWaitCursor wait;

                std::set<int>  v;
                v.insert(fileID);

                m_cxema.graph()->clearFile(v);
                reopen(v);
            }
        }

//            QMessageBox::warning(this, "", QString("%1").arg(ret));
    }

#endif

//    connect(tab, SIGNAL(accepted()), this, SLOT(onEditUndoAccepted()));
}





#if 0

void GidWidget::onEditUndo() // Отменить\tCtrl+Z
{
#if 1

    QString q = readQ("undo/undo.sql");

    TableWindow * tab = new TableWindow(m_cxema.m_db, "changed_object", "Отмена", q, this, 0, false, true, true);


    if (tab->exec() == QDialog::Accepted) {
        int logID = tab->value();
        bool ret = false;

        QString groupId = tab->value(1).toString();
        QString table  = tab->value(3).toString();
        int id  = tab->value(4).toInt();

//        QMessageBox::warning(this, "", QString("%1 %2 ").arg(table).arg(id));

        if (groupId  != "") {
            QString groupId = tab->value(1).toString();
            ret = rollbackGroup(m_cxema.m_db, groupId);
        }
        else {
            ret =  rollbackChange(m_cxema.m_db, logID);
        }
        if (!ret) {
            QMessageBox::warning(this, "", "Ошибка");
        }


        Klassif *kls = m_kl_list.findKlN(table);


        if (kls && table != "nodes" && table != "linesobj") {
            kls->open = false;
            reset_geo(*kls->m_db, this);
            return;
        }


        CNode2 *n = get_undo_node(m_cxema.graph(), table, id);
        int fileID = -1;

        if (n) {
            fileID = n->node.fileID;
        }
        else {
//            QMessageBox::warning(this, "", QString("Не могу найти %1 %2 ").arg(table).arg(id));
            fileID = get_undo_node_fileID(m_cxema.m_db, table, id);
        }

        if (fileID >= 0) {
            endDraw();

            QString par = QString("%1").arg(fileID);
            //            CWaitCursor wait;

            std::set<int>  v;
            v.insert(fileID);

            m_cxema.graph()->clearFile(v);
            reopen(v);
        }

//            QMessageBox::warning(this, "", QString("%1").arg(ret));
    }

#else

//    QString q = QString("SELECT * FROM changed_object ORDER BY id DESC");

    QString q = readQ("sql/history/history.sql");

//    TableWindow dlg();
    TableWindow * tab = new TableWindow(m_cxema.m_db, "changed_object", "Отмена", q, this, 0, false, true, true);
//    tab->show();
    if (tab->exec() == QDialog::Accepted) {
//        QMessageBox::warning(this, "", QString("%1").arg(tab->value()));

        int id = tab->value();

        int fileID;

        endDraw();

        if (undo_gid(m_cxema.m_db, id, fileID)) {
            QString par = QString("%1").arg(fileID);
//            CWaitCursor wait;

            std::set<int>  v;
            v.insert(fileID);

            m_cxema.graph()->clearFile(v);
            reopen(v);

        }
    }
#endif

//    connect(tab, SIGNAL(accepted()), this, SLOT(onEditUndoAccepted()));
}


#endif


void GidWidget::onEditUndoDel() // Отменить удаления
{
}


void GidWidget::onZapVnCx() // Узлы с внутренней схемой
{
    QString q = readQ("sql4/find_node_vn.sql");
    q.replace(QString("$fileID$"), m_cxema.m_par);

    TableWindow * tab = new TableWindow(m_cxema.m_db, "node", "Выберите узел с внутренней схемой", q, this);
    tab->show();

    connect(tab, SIGNAL(accepted()), this, SLOT(onEditFindNodeAccepted()));
}


void GidWidget::onZapNullGeo() // Узлы с нулевой геодезической отметкой
{
    QString q = readQ("sql4/find_node_zero.sql");
    q.replace(QString("$fileID$"), m_cxema.m_par);

    TableWindow * tab = new TableWindow(m_cxema.m_db, "node", "Выберите узел с с нулевой геодезической отметкой", q, this);
    tab->show();

    connect(tab, SIGNAL(accepted()), this, SLOT(onEditFindNodeAccepted()));
}


void GidWidget::onOdnolin() // Однолинейные участки
{
    m_cxema.m_graph->reset();

    auto p = m_cxema.m_graph->map_node.begin();

    while (p != m_cxema.m_graph->map_node.end()) {
        CNode2* n = p->second;
        for (CLINE2* l = n->lines; l; l = nextl(l)) {
            if (bline(l)->line.nomP == -1 || bline(l)->line.nomO == -1) {
                bline(l)->line.isPjezo = true;
                where(l)->node.isPjezo = true;
                other(l)->node.isPjezo = true;
            }
        }
        p++;
    }
    repaint();
}


void GidWidget::onRaznolin() // Участки с разными подачей и обраткой
{
    m_cxema.m_graph->reset();

    auto p = m_cxema.m_graph->map_node.begin();

    while (p != m_cxema.m_graph->map_node.end()) {
        CNode2* n = p->second;
        for (CLINE2* l = n->lines; l; l = nextl(l)) {
            if (bline(l)->line.nomP != -1 && bline(l)->line.nomO != -1 && bline(l)->line.nomP != bline(l)->line.nomO) {
                bline(l)->line.isPjezo = true;
                where(l)->node.isPjezo = true;
                other(l)->node.isPjezo = true;
            }
        }
        p++;
    }
    repaint();
}


void GidWidget::onRasprMag() // Расчетные схемы
{
    QString tn = "externalCodes";
    QString q;
    //  q.Format("SELECT id, name FROM %s WHERE fileID=%d AND removed = 0", tn, m_cxema.m_cur_file->id);

    q = QString(
        " SELECT %1"
        " ec.id, fr.name AS %2, "
        " ec.name AS %3, "
        " hs.name AS %4, "
        " ot.name AS %5"
        " FROM externalCodes ec"
        " JOIN fragments fr ON fr.id=ec.fileID"
        " LEFT JOIN net.v_heatsources hs ON hs.id=ec.heatSourceID"
        " LEFT JOIN objectTypes ot ON ot.id=ec.objectID"
        " WHERE ec.removed=0 AND ec.fileID IN (%6)"

         " AND  EXISTS ( "
         "    SELECT 1  "
         "    FROM net.v_nodes n "
         "    WHERE n.externalcodeid = ec.id "
         " ) "

        " ORDER BY fr.name, ec.name")
            .arg(top100000(),
                br_text("Фрагмент"),
                br_text("Расчетная схема"),
                br_text("Источник"),
                br_text("Объект РС"),
                m_cxema.m_par
        );

    QString out;

//    int externalCodeID = MMenuQ(this, tr("Выберите Расчетную схему для выделения"), m_cxema.m_db, q, out, &ok);

    TableWindow tab(m_cxema.m_db, tn, tr("Выберите Расчетную схему для выделения"), q, this);
    if (tab.exec()) {
        int externalCodeID = tab.value("id").toInt();
        m_cxema.m_graph->reset();
        auto p = m_cxema.m_graph->map_node.begin();

        while (p != m_cxema.m_graph->map_node.end()) {
            CNode2* n = p->second;
            if (n->node.coord.x != 0 && n->node.coord.x != 0 && n->node.id_kod == externalCodeID && n->node.internalNodeID == 0) {
                CLINE2* l;
                for (l = n->lines; l; l = nextl(l)) {
                    bline(l)->line.isPjezo = 1;
                    where(l)->node.isPjezo = 1;
                    other(l)->node.isPjezo = 1;
                }
            }
            p++;
        }
        moveVydIf();
        repaint();
    }
}


void GidWidget::onRasprMag2() // Объекты системы теплоснабжения
{
    QString tn = "b10_vidy_obektov_raschetnoy_shemy";
    QString q =  "SELECT * FROM [vsp].[dbo].[b10_vidy_obektov_raschetnoy_shemy]";
    q = R"SQL(

select * from (
values
(1,'Магистраль'),
(2,'Распредсеть'),
(3,'Источник тепла'),
(4,'Насосная станция'),
(5,'ЦТРП'),
(6,'КРП'),
(7,'Камера'),
(8,'Котельная'),
(9,'Участок магистрали')

) t (id,name_object)
        )SQL";



    TableWindow tab(m_cxema.m_db, tn, tr("Выберите Расчетную схему для выделения"), q, this, -1, false, true, false);

    if (!tab.exec()) return;

    int id = tab.value("id").toInt();


    m_cxema.m_graph->reset();

    auto pp = m_cxema.m_graph->map_node.begin();

    while (pp != m_cxema.m_graph->map_node.end()) {
        CNode2* p = pp->second;
        if (p->node.coord.x != 0 && p->node.coord.x != 0) {
            RasCx * cx = m_cxema.getRasCx(p->node.id_kod);
            if (cx) {
                if (id == cx->objectID) {
                    CLINE2* l;
                    for (l = p->lines; l; l = nextl(l)) {
                        bline(l)->line.isPjezo = 1;
                        where(l)->node.isPjezo = 1;
                        other(l)->node.isPjezo = 1;
                    }
                }
            }
/*
            map<long, RasCx>::const_iterator it = m_cxema.map_rs.find(p->node.id_kod);
            if (it != m_cxema.map_rs.end()) {
                RasCx cx = it->second;

                if (id == cx.objectID) {
                    CLINE2* l;
                    for (l = p->lines; l; l = next(l)) {
                        bline(l)->line.isPjezo = 1;
                        where(l)->node.isPjezo = 1;
                        other(l)->node.isPjezo = 1;
                    }
                }
            }
*/
        }
        pp++;
    }
    moveVydIf();
    repaint();
}

void GidWidget::onIstSety() // Источник
{
    QString tn = "heatSources";

    QString q = QString(
        "SELECT %1\n"
        "hs.id,\n"
        "fr.name AS %3,\n"
        "hs.name AS %4,\n"
        "n.externalNodeName AS %5\n"
        "FROM net.v_heatsources hs\n"
        "JOIN net.v_nodes n ON n.id=hs.nodeID\n"
        "JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
        "JOIN fragments fr ON fr.id=n.fileID\n"
        " WHERE fr.id IN (%2) and n.removed=0\n"

        "ORDER BY fr.name, hs.name,n.externalNodeName").arg(top100000(), m_cxema.m_par)

//        "fr.name AS fr_name,\n"
//        "hs.name AS hs_name,\n"
//        "n.externalNodeName AS n_name\n"
            
            .arg(quot_text("Фрагмент"))
            .arg(quot_text("Источник"))
            .arg(quot_text("Узел"))
        
        ;

    TableWindow tab(m_cxema.m_db, tn, tr("Выберите Источник для выделения его участков"), q, this);
    if (tab.exec()) {
        int id = tab.value("id").toInt();

        m_cxema.m_graph->reset();

        auto pp = m_cxema.m_graph->map_node.begin();

        while (pp != m_cxema.m_graph->map_node.end()) {
            CNode2* p = pp->second;
            if (p->node.coord.x != 0 && p->node.coord.x != 0 && p->node.internalNodeID == 0) {
                int kod_id = -1;

                RasCx *cx = m_cxema.getRasCx(p->node.id_kod);

                if (cx) {
                    if (cx->objectID == 2) {
                        RasCx *cx2 = m_cxema.getRasCx(cx->belongMagistral);

                        if (cx2) {
                            kod_id = cx2->heatSourceID;
                        }
                    }

                    if (kod_id == -1 /*cx.objectID == 1 || cx.objectID == 8*/) {
                        kod_id = cx->heatSourceID;
                    }
                }

                if (id == kod_id) {
                    CLINE2* l;
                    for (l = p->lines; l; l = nextl(l)) {
                        bline(l)->line.isPjezo = 1;
                        where(l)->node.isPjezo = 1;
                        other(l)->node.isPjezo = 1;
                    }
                }
            }
            pp++;
        }

        moveVydIf();
        repaint();
    }
}


void GidWidget::onIstSetyNew() // Источник
{
    QString tn = "heatSources";

    QString q = QString(
        "SELECT %1\n"
        "hs.id,\n"
        "fr.name AS %3,\n"
        "hs.name AS %4,\n"
        "n.externalNodeName AS %5\n"
        "FROM net.v_heatsources hs\n"
        "JOIN net.v_nodes n ON n.id=hs.nodeID\n"
        "JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
        "JOIN fragments fr ON fr.id=n.fileID\n"
//        " WHERE fr.id IN (%2) and n.removed=0\n"

        " where n.removed=0 and hs.id in (\n"
        " select distinct ist from us_out\n"
        " join net.v_nodes n on n.id=us_out.nodeid\n" 

        " LEFT JOIN ( \n"
        "    SELECT \n"
        "        c.fileid,\n"
        "        max(c.id) AS cid\n"
        "    FROM calculation c\n"
        "    LEFT JOIN fragments fr ON fr.id = c.fileid\n"
        "    GROUP BY c.fileid) calc ON calc.fileid = n.fileid\n"
        " where n.removed=0 and n.fileid in (%2)\n"
        ")\n"
        "ORDER BY fr.name, hs.name,n.externalNodeName")
            .arg(top100000(), m_cxema.m_par)
//            .arg(quot_text("fr_name"))
//            .arg(quot_text("hs_name"))
// n_name
            .arg(quot_text("Фрагмент"))
            .arg(quot_text("Источник"))
            .arg(quot_text("Узел"))
            ;

    TableWindow tab(m_cxema.m_db, tn, tr("Выберите Источник для выделения его участков"), q, this);
    if (tab.exec()) {
        int id = tab.value("id").toInt();

        QString q = QString("select id from ut_out uo where uo.ist=%1").arg(id);


        std::set<int> set_id;

        QSqlQuery query(m_cxema.m_db);
        query.setForwardOnly(true);

        if (query_exec(m_cxema.m_db, query, q)) {
            while (query.next()) {
                int id = query.value("id").toInt();
                set_id.insert(id);
            }
        }

        m_cxema.m_graph->vyd_lines_out(set_id);

        moveVydIf();
        repaint();
    }
}




void GidWidget::onVyd() // Фрагмент сети
{
}


void GidWidget::onBalans() // Балансовая принадлежность
{
    QString tn = "organizations";
    QString q = QString("SELECT %1 id,name FROM %2 \n--AND_TXT WHERE name LIKE '%%$txt$%%'").arg(top100000(), tn);

//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);

    TableWindow tab(m_cxema.m_db, tn, tr("Выберите Балансовую принадлежность для выделения"), q, this);
    if (tab.exec()) {
        int ID = tab.value("id").toInt();

        m_cxema.m_graph->reset();

        auto p = m_cxema.m_graph->map_node.begin();

        while (p != m_cxema.m_graph->map_node.end()) {
            CNode2* n = p->second;
            if (n->node.coord.x != 0 && n->node.coord.x != 0 && n->node.internalNodeID == 0) {
                CLINE2* l;
                for (l = n->lines; l; l = nextl(l)) {
                    if (bline(l)->line.owner == ID) {
                        bline(l)->line.isPjezo = 1;
                        where(l)->node.isPjezo = 1;
                    }
                }
            }
            p++;
        }
        moveVydIf();
        repaint();
    }
}

void GidWidget::vyd_tubing(int tubingID)
{
    CLINE2* l;
    auto pp = m_cxema.m_graph->map_node.begin();

    while (pp != m_cxema.m_graph->map_node.end()) {
        CNode2* p = pp->second;

        for (l = p->lines; l; l = nextl(l)) {
            bline(l)->line.isPjezo = (bline(l)->line.tubingTypeID == tubingID) ? 1 : 0;
            where(l)->node.isPjezo = bline(l)->line.isPjezo;
            other(l)->node.isPjezo = bline(l)->line.isPjezo;
        }
        pp++;
    }
    repaint();
}




void GidWidget::onTubing1() // Канальная
{
    vyd_tubing(1);
}


void GidWidget::onTubing2() // Бесканальная
{
    vyd_tubing(2);
}


void GidWidget::onTubing4() // Надземная
{
    vyd_tubing(4);
}


void GidWidget::onTubing3() // Подвальная
{
    vyd_tubing(3);
}


void GidWidget::onTubing5() // Обвязка узлов и насосных станций
{
    vyd_tubing(5);
}


void GidWidget::onFindKti() // По коэффициенту тепловых испытаний
{
    QString q = "SELECT heattestscoeff FROM heatpipesections"
                " WHERE heattestscoeff IS NOT NULL"
                " GROUP BY heattestscoeff ORDER BY heattestscoeff";

    QStringList items;
    QSqlQuery query(m_cxema.m_db);
    query.setForwardOnly(true);
    if (query_exec(m_cxema.m_db, query, q)) {
        while (query.next()) {
            items << QString::number(query.value(0).toDouble(), 'g', 12);
        }
    }

    bool ok = false;
    QString s = QInputDialog::getItem(this, tr("По коэффициенту тепловых испытаний"),
                                      tr("Выберите коэффициент"), items, 0, false, &ok);
    if (!ok || s.isEmpty()) return;

    std::set<int> set_id;
    q = QString(
        "SELECT l.id FROM heatpipesections hps"
        " JOIN linesobj l ON l.id=hps.lineid"
        " JOIN nodes n ON l.nodeid1=n.id"
        " WHERE heattestscoeff=%1 AND n.fileid IN (%2)").arg(s, m_cxema.m_par);

    if (query_exec(m_cxema.m_db, query, q)) {
        while (query.next()) {
            set_id.insert(query.value(0).toInt());
        }
    }

    m_cxema.m_graph->vyd_lines(set_id);
    repaint();
}


void GidWidget::onSetOtv() // Установить ФИО техников
{
    QVariant val;
    setSomething(true, TIP_PR, "responsibleID", QVariant(), val);
}


void GidWidget::onSetTr() // Установить Код расчетных температур
{
    QVariant val;
    if (setSomething(true, TIP_PR, "calcTemperatureID", QVariant(), val)) {
        setValue(TIP_PO, "calcTemperatureID", val);
    }
//    setSomething(true, TIP_PO, "calcTemperatureID", val);
}


void GidWidget::onSetUr() // Установить Код удельных расходов
{
    QVariant val;
    if (setSomething(true, TIP_PR, "specExpendID", QVariant(), val)) {
        setValue(TIP_PO, "specExpendID", val);
    }
//    setSomething(true, TIP_PO, "specExpendID", val);
}


void GidWidget::onSetKvPt() // Установить Коэффициенты вариации по потребителям
{
    QVariant val;
    if (setSomething(true, TIP_PR, "varCoeffID", QVariant(), val)) {
        setValue(TIP_PO, "varCoeffID", val);
    }
//    setSomething(true, TIP_PO, "varCoeffID", val);
}


void GidWidget::onSetUf() // Установить Коэффициенты смешения элеватора
{
    QVariant val;
    setSomething(true, TIP_PR, "mixFactCoeff", 2.2, val);
}


void GidWidget::onSetTp() // Установить Тепловые пункты по потребителям
{
}


void GidWidget::onSetUdobVent() // Установить Удельный объем системы вентиляции
{
    QVariant val;
    if (setSomething(true, TIP_PR, "volWaterVS", 1., val)) {
        setValue(TIP_PO, "volWaterVS", val);
    }
}


void GidWidget::onSetUdobOt() // Установить Удельный объем системы отопления
{
    QVariant val;
    if (setSomething(true, TIP_PR, "volWaterHS", 1., val)) {
        setValue(TIP_PO, "volWaterHS", val);
    }
}


void GidWidget::onSetOpenKoef() // Коэф. часовой неравномерности
{
    QVariant val;
    if (setSomething(true, TIP_PR, "hourIrregCoeff", 1.2, val)) {
        setValue(TIP_PO, "hourIrregCoeffOpen", val);
    }
}


void GidWidget::onSetOpenRez() // Расчетные тепловые потери в рециркуляц. контуре ГВС
{
    QVariant val;
    if (setSomething(true, TIP_PR, "circHLosOpen", 30., val)) {
        setValue(TIP_PO, "avgHLcompOpen", val);
    }
}


void GidWidget::onSetOpenRezT() // Температура в рециркуляционном трубопроводе ГВС
{
    QVariant val;
    if (setSomething(true, TIP_PR, "tempRecircPipe", 40., val)) {
        setValue(TIP_PO, "tempRecircPipeOpen", val);
    }
}


void GidWidget::onSetOpenGvsT() // Расчетная температура горячей воды
{
    QVariant val;
    if (setSomething(true, TIP_PR, "calcTempHWDO", 60., val)) {
        setValue(TIP_PO, "calcTempHWDOOpen", val);
    }
}

bool diameter_std(QWidget *widget, QSqlDatabase &db, const QString & stand, double & d1, double & d2, double & d3, double & th);

void GidWidget::onSetDiams() // Установить Диаметр
{
    QVariant val;
    double d1, d2, d3, th;

    if (!isEditGid()) return;
    if (m_parent_id) return;

    if (!m_cxema.m_graph->isMark()) {
        QMessageBox::warning(this, "", "Для выполнения операции необходимо выделить участки");
        return;
    }

    if (diameter_std(this, m_cxema.m_db, "ГОСТ", d1, d2, d3, th)) {

        QString str = QString(tr(
            "Внимание!\n"
            "При выполнении этой операции диаметры всех выделенных участков будут изменены\n"
            "Выполнить операцию?"));

        if (QMessageBox::question(0, "", str) == QMessageBox::Yes) {
            change_group_start(m_cxema.m_db, "Операция по установке диаметров");

            setValue(TIP_UT, "diameterCondit", QVariant(d1));
            setValue(TIP_UT, "diameterExternal", QVariant(d2));
            setValue(TIP_UT, "diameterInternal", QVariant(d3));
            setValue(TIP_UT, "wallThickness", QVariant(th));

            change_group_end(m_cxema.m_db);


            QMessageBox::warning(this, "", tr("Выполнено"));
        }
    }
}


void GidWidget::onSetLosesShare() // Установить Долю местных потерь
{
    QVariant val;
    setSomething(false, TIP_UT, "localLosesShare", 0., val);
}


void GidWidget::onSetKolChas() // Установить Количество часов работы
{
    QVariant val;
    setSomething(false, TIP_UT, "signNumWork", QVariant(), val);
}


void GidWidget::onSetKvUt() // Установить Коэффициенты вариации по участкам
{
    QVariant val;
    if (setSomething(false, TIP_UT, "varCoeffIDflow", QVariant(), val)) {
        setValue(TIP_UT, "varCoeffIDret", val);
    }
}


void GidWidget::onSetKti() // Установить Коэффициенты тепловых испытаний
{
    QVariant val;
    setSomething(false, TIP_UT, "heatTestsCoeff", 1., val);
}


void GidWidget::onSetOrg() // Установить Организации
{
    QVariant val;
    setSomething(false, TIP_UT, "organizationID", QVariant(), val);
}


void GidWidget::onSetPipeRemontType() // Установить Признак ремонта
{
    QVariant val;
    setSomething(false, TIP_UT, "pipeRemontTypeID", QVariant(), val);
}


void GidWidget::onSetTubingType() // Установить Тип прокладки
{
    QVariant val;
    setSomething(false, TIP_UT, "tubingTypeID", QVariant(), val);
}


void GidWidget::onSetSher() // Установить Эквивалентную шероховатость
{
    QVariant val;
    setSomething(false, TIP_UT, "tubeRoughness", 0.5, val);
}


void GidWidget::onSetDate2() // Установить Дату первичного ввода в эксплуатацию
{
}


void GidWidget::onSetDate1() // Установить Дату последней перекладки
{
}


void GidWidget::onSetDate3() // Установить Дату планируемого ремонта
{
}


void GidWidget::onSetKodRs() // Установить Код расчетной схемы
{
    QVariant val;
    if (setSomething(true, TIP_US, "externalCodeID", QVariant(), val)) {

        QVariant null;
        auto p = m_cxema.m_graph->map_node.begin();
        while (p != m_cxema.m_graph->map_node.end()) {
            CNode2 *pp = p->second;
            if (pp->node.isPjezo) {
                pp->node.id_kod = val.toInt();
            }

            p++;
        }
    }
}


void GidWidget::onGeodz() // Установить Отметки узлов горизонтали
{
}


void GidWidget::onSetKorrozia() // Установить Индикаторы коррозии
{
}

bool setMarkNode1Value(QSqlDatabase &db, CGraph2* graph, const QString & tn, const QString & fn, const QVariant & val, bool all);
bool setMarkLine1Value(QSqlDatabase &db, CGraph2* graph, const QString & fn, const QVariant & val, bool all);


void GidWidget::onSetPodpOn() // Показать Надписи
{
    if (!isEditGid()) return;
    if (m_parent_id) return;
    if (!m_cxema.m_graph->isMark()) {
        QMessageBox::warning(this, "", "Для выполнения операции необходимо выделить участки");
        return;
    }

    QString dt = "1";

    QString  str = tr("Показывать надписи по всем выделенным объектам");
    if (QMessageBox::question(0, "", str) == QMessageBox::Yes) {
        setMarkNode1Value(m_cxema.m_db, m_cxema.graph(), "nodes", "displaySign", "0", false);
        setMarkLine1Value(m_cxema.m_db, m_cxema.graph(), "displaySign", "0", false);

        m_cxema.graph()->setOnOff(true);
        repaint();
    }
}


void GidWidget::onSetPodpOff() // Не отображать Надписи
{
    if (!isEditGid()) return;
    if (m_parent_id) return;
    if (!m_cxema.m_graph->isMark()) {
        QMessageBox::warning(this, "", "Для выполнения операции необходимо выделить участки");
        return;
    }

    QString dt = "1";

    QString  str = tr("Не показывать надписи по всем выделенным объектам?");
    if (QMessageBox::question(0, "", str) == QMessageBox::Yes) {
        setMarkNode1Value(m_cxema.m_db, m_cxema.graph(), "nodes", "displaySign", "1", false);
        setMarkLine1Value(m_cxema.m_db, m_cxema.graph(), "displaySign", "1", false);

        m_cxema.graph()->setOnOff(false);
        repaint();
    }
}


void GidWidget::onFragmentPoly() // Выделить многоугольный фрагмент
{
}


void GidWidget::onMoveVyd() // Передвинуть фрагмент
{
}




void GidWidget::onEditCopy() // Копировать\tCtrl+C
{
    if (m_bIsFragment) {
        copyBuffer(CFRect(m_pt1_1, m_pt1_2));
        m_bIsFragment = false;
    }
    else {
        QMessageBox::information(this, "", tr("Выделите фрагмент для копирования"));
    }
}


void GidWidget::onEditCopyPng() // Копировать в PNG
{
}


void GidWidget::onViewToolbarMain(bool on) // Главная
{
    if (m_barGid) m_barGid->setVisible(on);
}

/*
void GidWidget::onViewToolbar2PtsNew() // Паспортизация
{
}


void GidWidget::onTuOnOff() // Технические условия
{
}


void GidWidget::onViewToolbarElectro() // Электрические сети
{
}


void GidWidget::onViewToolbarControlTu() // Контроль технического состояния
{
}


void GidWidget::onViewToolbar2Remont() // Ремонт
{
}


void GidWidget::onViewToolbarOpressovka() // Опрессовки
{
}


void GidWidget::onViewToolbarKorrozia() // Индикаторы коррозии
{
}
  */

void GidWidget::onNavigator(bool on) // Навигатор
{
}


void GidWidget::onLeftBar(bool on) // Проводник карты
{
    main_window->showDocks(Qt::LeftDockWidgetArea, on);
}


void GidWidget::onMagAlma(bool on) // Проводник структуры сети
{
    main_window->showDocks(Qt::RightDockWidgetArea, on);
}


void GidWidget::onBottomRemont(bool on) // Проводник мониторинга
{
    main_window->showDocks(Qt::BottomDockWidgetArea, on);
}

void GidWidget::onProtocol(bool on) // Протокол
{
    main_window->shellDoc->setVisible(on);
//    main_window->shellDoc->hide();
}


void GidWidget::onMovew() // Перемещение\tM
{
}


void GidWidget::onPovorot() // Повернуть
{
}


void GidWidget::onPovorot2() // Возврат
{
}



void GidWidget::onZoom() // Масштабировать
{
}


void GidWidget::onPicture() // Графическая подложка
{
}


void GidWidget::onGeobaza(bool on) // Показать геобазу
{
    m_bIsGeo = on;
    repaint();
}


void GidWidget::onFindGeo() // Поиск в геобазе...
{
}


void GidWidget::onGeoFindNext() // Продолжение поиска
{
}


void GidWidget::onFindGeoAddr() // Поиск по адресу
{
}


void GidWidget::onGeoInfo() // Информация из геобазы
{
}


void GidWidget::onSetLength() // Установить длины
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    if (!m_cxema.m_graph->isMark()) {
        QMessageBox::warning(this, "", "Для выполнения операции необходимо выделить участки");
        return;
    }

    int cnt = 0;

    {
        auto p = m_cxema.m_graph->map_node.begin();
        while (p != m_cxema.m_graph->map_node.end()) {
            CNode2* n = p->second;
            if (n->node.internalNodeID == m_parent_id) {

                for (CLINE2* l = n->lines; l; l = nextl(l)) {
                    if (bline(l)->line.isPjezo) {
                        cnt += 1;
                    }
                }
            }
            p++;
        }
    }



    if (QMessageBox::question(this, "", "Установить длины всех выделенных участков?") == QMessageBox::Yes) {
        QString str = tr("Внимание!\nПри выполнении этой операции длины всех выделенных участков будут изменены\nВыполнить операцию?");

        if (QMessageBox::question(this, "", str) == QMessageBox::Yes) {

            QProgressDialog percent;


            int i = 0;

            auto p = m_cxema.m_graph->map_node.begin();

            while (p != m_cxema.m_graph->map_node.end()) {
                CNode2* n = p->second;
                if (n->node.internalNodeID == m_parent_id) {
                    for (CLINE2* l = n->lines; l; l = nextl(l)) {
                        CLine2 *ll = bline(l);
                        if (ll->line.isPjezo) {

                            if (ll->line.nomP != -1 || ll->line.nomO != -1) {
                                m_cxema.setLength(l, true, false);
        //                        m_cxema.setLength(l, is_google_map & GOOGLE_LEN, false);
                                percent.setValue(i * 100. / cnt);
                                i += 1;
                            }
                        }
                    }

                }
                p++;
            }
        }
    }
}


void GidWidget::on1000() // Создать файл полей
{
}


void GidWidget::on1001() // Переименовать поля
{
}


void GidWidget::on1002() // Сохранить переименованные поля
{
}


#if 0

void GidWidget::onNode() // Установить узел\tN
{
}


void GidWidget::onLine() // Линейный объект\tL
{
}


void GidWidget::onRename() // Изменить тип объекта
{
}


void GidWidget::onZn() // Узел с заданным напором
{
}


void GidWidget::onKam() // Внутренняя схема
{
}


void GidWidget::onNodeNeotr() // Установка конвертированных узлов\tC
{
}

#endif


void GidWidget::onZapNezak() // Незаконченные узлы
{
    QString q = QString(

"select \n"
"n1.id, \n"
"ec.name AS %1,\n"
"n1.externalNodeName as %2,\n"
"rc.name as %3,\n"
"fr.name as %4\n"


"from net.v_linesobj l \n"
"join net.v_nodes n1 on n1.id=l.nodeID1 and n1.removed=0 and n1.internalNodeID is null\n"
"join net.v_nodes n2 on n2.id=l.nodeID2 and n2.removed=0 and n2.internalNodeID is null\n"
"JOIN fragments fr ON fr.id=n1.fileID\n"
"LEFT JOIN externalCodes ec ON ec.id=n1.externalCodeID\n"
"LEFT JOIN net.v_realconsumers rc ON rc.nodeID =n1.id\n"

"where l.removed=0\n"
"and n1.fileID in (%5) \n"
"and (n2.x=0 and n2.y=0 and (n1.x <> 0 or n1.y <> 0))\n"

"union all\n"

"select \n"
"n2.id, \n"
"ec.name AS %1,\n"
"n2.externalNodeName as %2,\n"
"rc.name as %3,\n"
"fr.name as %4\n"

"from net.v_linesobj l \n"
"join net.v_nodes n1 on n1.id=l.nodeID1 and n1.removed=0 and n1.internalNodeID is null\n"
"join net.v_nodes n2 on n2.id=l.nodeID2 and n2.removed=0 and n2.internalNodeID is null\n"
"JOIN fragments fr ON fr.id=n1.fileID\n"
"LEFT JOIN externalCodes ec ON ec.id=n1.externalCodeID\n"
"LEFT JOIN net.v_realconsumers rc ON rc.nodeID =n1.id\n"

"where l.removed=0\n"
"and n1.fileID in (%5) \n"
"and (n1.x=0 and n1.y=0 and (n2.x <> 0 or n2.y <> 0))\n"

)
.arg(quot_text("Код РС"), quot_text("Имя узла"), quot_text("Описание"), quot_text("Фрагмент"), m_cxema.m_par);

    QString tn = "Список незаконченных узлов";

//    TableWindow t(QSqlDatabase &db, const QString & tn, const QString & title, const QString & que, QWidget *parent, int targetId, bool find_str, bool ok_str, bool is_header, bool autofit);

    TableWindow tab(m_cxema.m_db, tn, tn, q, this, -1, true, true, true);

    if (tab.exec()) {
        int id = tab.value(0).toInt();

        CNode2* n = m_cxema.m_graph->find(id);
        if (n) {
            moveNode(n);
        }
    }
}


void GidWidget::onSetCoordNull() // Удалить координаты выделенных узлов
{

#if 1

    if (!isEditGid()) return;
    if (m_parent_id) return;

    QString str = QString(tr(
        "Внимание!\nПри выполнении этой операции координаты всех выделенных узлов будут обнулены\nВыполнить операцию?"
        ));

    if (QMessageBox::question(0, "", str) == QMessageBox::Yes) {


        bool setMarkNodeLine2Value(QSqlDatabase &db, CGraph2* graph, const QString & tn, const QString & fn, const QVariant & val, bool all, bool isNode, const QString & node_line_id);
        bool setMarkLine1Value(QSqlDatabase &db, CGraph2* graph, const QString & fn, const QVariant & val, bool all);


        setMarkNodeLine2Value(m_cxema.m_db, m_cxema.m_graph, "nodes", "x", "0.", false, true, "id");
        setMarkNodeLine2Value(m_cxema.m_db, m_cxema.m_graph, "nodes", "y", "0.", false, true, "id");

        setMarkLine1Value(m_cxema.m_db, m_cxema.m_graph, "coords", "''", false);


        auto p = m_cxema.m_graph->map_node.begin();

        while (p != m_cxema.m_graph->map_node.end()) {
            CNode2* n = p->second;
            if (n->node.isPjezo) {
                n->node.coord.x = 0;
                n->node.coord.y = 0;

                for (CLINE2* l = n->lines; l; l = nextl(l)) {
                    bline(l)->line.m_NP.clear();
                }
            }
            p++;
        }

        repaint();
    }
#endif
}


void GidWidget::onNodeMove() // Перенести\tV
{
}


void GidWidget::onDel() // Удалить объект\tD
{
}


void GidWidget::onOpenClose() // Закрыть/открыть объект
{
}


void GidWidget::onNodeMove1() // Разъединить линии
{
}


void GidWidget::onSwap() // Изменить направление потока
{
}


void GidWidget::onMoveLineEnd() // Перенести линию
{
}


void GidWidget::onText() // Установить текст
{
/*
    if (!isEditGid()) return;

    bool ok;
    QString text = QInputDialog::getText(this,"", tr("Введите текст"), QLineEdit::Normal, text, &ok);
*/
}


void GidWidget::onMagFragment() // Магистральный фрагмент
{
    bool ok;
    QString str, name;

    int id = MMenuQ(this, tr("Выберите магистральный фрагмент присоединения"), m_cxema.m_db, QString("SELECT id, name FROM fragments ORDER BY name").arg(m_cxema.m_par), name, &ok);

    if (ok) {
        str = QString(tr("Установить магистральный фрагмент присоединения во всех фрагментах распределительных сетей \"%1\"?")).arg(name);
        if (QMessageBox::question(this, "", str)  == QMessageBox::Yes) {

            QString q = QString("UPDATE setPressNodes SET fragment_resultID=%1").arg(id);
            QSqlQuery query(m_cxema.m_db);
            query.exec(q);
        }
    }
}


void GidWidget::onNapr() // Выбpать напpавление
{
}


void GidWidget::savePjezo()
{
    int fileID = 0;

    CGraph2 *graph = m_cxema.graph();

    std::deque<long>::const_iterator it = graph->list_pjezo_min.begin();
    for (; it != graph->list_pjezo_min.end(); it++) {
        long id = *it;
        CNode2* node = graph->find(id);
        if (node) {
            fileID = node->node.fileID;
            break;
        }
    }

    if (fileID == 0) return;

    QString text = "";
    bool ok;

    QString name = QInputDialog::getText(nullptr, "Название направления", tr("Введите название направления"), QLineEdit::Normal, "", &ok);

    if (ok) {
/*
        ado->AddNew("directions");
        ado->write("name", inD.m_strInput);
        ado->write("fileID", (long)fileID);
        long directionID = ado->Update();
*/

//        QString q = QString("INSERT INTO directions (name, fileID) VALUES (:name, :fileID)").arg(name).arg(fileID);

//        QString q = QString("INSERT INTO directions (name, fileID) VALUES (:name, :fileID)");

        std::map<QString, QVariant> data;

        QString q = QString("INSERT INTO directions (name, fileID) VALUES (?, ?)");

/*
        QSqlQuery query(m_cxema.m_db);


        query.prepare(q);
//        query.b1indValue(":name", name);
//        query.b1indValue(":fileID", fileID);
        query.addBindValue(name);
        query.addBindValue(fileID);
*/
        data["name"] = name;
        data["fileID"] = fileID;

//        int directionID = execInsertQ(m_cxema.m_db, query, "");

        int directionID = insertIntoDatabase(m_cxema.m_db, "directions", data);


        QString qq;
        bool first = true;

        q = QString("INSERT INTO deployedDirections (directionID,nodeID) VALUES ");

        std::map<QString, QVariant> data2;


        deque<long>::const_iterator it = graph->list_pjezo_min.begin();
        for (; it != graph->list_pjezo_min.end(); it++) {
            long id = *it;
            if (!first) q += ",";  first = false;
            qq = QString("(%1,%2)").arg(directionID).arg(id);
            q += qq;

            data2["directionID"] = directionID;
            data2["nodeID"] = (int)id;
            int n = insertIntoDatabase(m_cxema.m_db, "deployedDirections", data2);
        }

        if (!first) {
//            int n = execInsertQ(m_cxema.m_db, q);

//            int n = insertIntoDatabase(m_cxema.m_db, "deployedDirections", data2);

        }
    }
}


void GidWidget::onSavePjezo() // Сохранить направление...
{
    savePjezo();
}



void GidWidget::onClearOut() // Удалить старые расчеты
{
    if (!UserRight::isAdmin()) {
        QMessageBox::warning(this, "", tr("Для выполнения этой операции у вас должны быть права администратора!"));
        return;
    }

    if (QMessageBox::question(this, "", tr("Удалить старые гидравлические расчеты?")) == QMessageBox::Yes) {
        QString q = "DELETE FROM calculation WHERE NOT id IN ("
                    " SELECT max(c.id) AS cid FROM calculation c"
                    " LEFT JOIN fragments fr ON fr.id=c.fileid"
                    " GROUP BY c.fileid, c.user_gid)";

        if (query_exec(m_cxema.m_db, q)) {
            QMessageBox::information(this, "", tr("Готово"));
        }
    }
}



void GidWidget::onInfo() // Свойства объекта\tI
{
}



#include <dialog/NoVisual.h>


void GidWidget::onNovisual() // Системные параметры...
{
    NoVisual dlg(this, "", m_fileID, &m_cxema, m_cxema.m_db, m_cxema.m_graph, this);
    dlg.exec();
}


void GidWidget::onMoveTo() // Таблица объекта
{
}


void GidWidget::onInfoGid() // Результат расчета
{
}

bool listOut(QWidget *widget, const QString & title, QString &table_out, QString &title_out, int &node_or_line);

void GidWidget::onOut2() // Отчеты...
{
    QString table, title;
    int node_or_line;

#if 0
    QString title = tr("Таблицы");
    if (mark) {
        title = QString(tr("Таблицы в выделенном фрагменте"));
    }
    else {
        title = QString(tr("Таблицы во фрагменте %1")).arg(m_cxema.getFileName(m_fileID));
    }
#endif

    if (!listOut(this, tr("Выберите таблицу объекта с результатами расчета"), table, title, node_or_line)) return;

    outView(node_or_line, table, title, "", true);
}


void view_db2(DbWindow *view, const QString & title, QWidget *parent); // table/table_part.cpp

void GidWidget::onTableDan() // Редактор таблиц...
{
    QString q =
        "SELECT table_schema || '.' || table_name "
        "FROM information_schema.tables "
        "WHERE table_schema IN ('compat','net','ref','calc','meta','addr','doc','el','ops','org') "
        "AND table_type IN ('BASE TABLE','VIEW') "
        "ORDER BY 1";

    MMenuDial menu(this, tr("Редактор таблиц"));

    QSqlQuery query(m_cxema.m_db);
    query.setForwardOnly(true);

    if (query_exec(m_cxema.m_db, query, q)) {
        while (query.next()) {
            QString tn = query.value(0).toString();
            menu.Add(tn, tn);
        }
    }

    if (menu.exec() != QDialog::Accepted) return;

    QString tn = menu.value().toString();
    QString qs = QString("SELECT * FROM %1").arg(tn);

    DbWindow *table = getTableView(m_cxema.m_db, tn, qs, tn);
    if (!table) {
        QMessageBox::warning(this, "", tr("Ошибка открытия таблицы %1").arg(tn));
        return;
    }
    table->setEdit(true);
    view_db2(table, tn, this);
}

void GidWidget::onGidinf(bool on) // Отображать информацию
{
    m_bIsGidrInf = on;
    repaint();
}


void GidWidget::onPodp(bool on) // Отображать имена узлов
{
    m_bIsPodp = on;
    repaint();
}


void GidWidget::onPodpPo() // Отображать схему ПТС
{
}


void GidWidget::onPodpRes(bool on) // Отображать имена узлов с #
{
    m_bIsPodpRes = on;
    repaint();
}


void GidWidget::onPribor() // Отображать Приборы учета
{
}


void GidWidget::onRezhim(bool on) // Отображать вспомогательные участки
{
    m_bIsRezhim = !on;

//  if (m_idMenu == 4) {
//      m_bIsRezhim = true;
//  }

    repaint();
}


void GidWidget::onPopupPodpOn() // Показать скрытые имена
{
}


void GidWidget::onPopupPodpOff() // Не показывать скрытые имена
{
}


void GidWidget::onPodpYesNo(bool on) // Отображать/Не отображать надпись
{
}


void GidWidget::onNaprGid() // Распределение потоков
{
}

//void GidWidget::onAnal() // Анализ...




void GidWidget::onZap6() // Закрытые потребители
{
    QString q = QString(R"(

select n.id, ec.name, n.externalnodename, pt.name
from net.v_nodes n
join externalcodes ec on ec.id=n.externalcodeid
join (
select gc.nodeID, gc.consumerStateID, gc.name from net.v_generalizedconsumers gc where gc.consumerStateID=2
union
select rc.nodeID, rc.consumerStateID, rc.name from net.v_realconsumers rc where rc.consumerStateID=2
) pt on pt.nodeID=n.id
where n.removed=0 and n.fileID in (%1)
--AND_NODE

)").arg(m_cxema.m_par);

    QString label = "Закрытые потребители";
    TableWindow *tab = new TableWindow(m_cxema.m_db, label, label, q, this, -1, true, true, false);
    tab->show();

    connect(tab, SIGNAL(accepted()), this, SLOT(onEditFindNodeAccepted()));
}


void GidWidget::onPotNagr0() // С нулевой нагрузкой
{
    QString q = QString(R"(

select n.id, ec.name, n.externalnodename, pt.name
from net.v_nodes n
join externalcodes ec on ec.id=n.externalcodeid
join (
select gc.nodeID, gc.consumerStateID, gc.name from net.v_generalizedconsumers gc where

(calcHLdep     is Null or calcHLdep    = 0) and
(calcHLindep   is Null or calcHLindep  = 0) and
(calcHLparall  is Null or calcHLparall = 0) and
(calcHLmix     is Null or calcHLmix    = 0) and
(calcHLconseq  is Null or calcHLconseq = 0) and
(calcHLpreON   is Null or calcHLpreON  = 0)

union
select rc.nodeID, rc.consumerStateID, rc.name from net.v_realconsumers rc where
(calcHLdep is Null or calcHLdep=0)
and (calcHLindep is Null or calcHLindep=0)

) pt on pt.nodeID=n.id
where n.removed=0 and n.fileID in (%1)

--AND_NODE


)").arg(m_cxema.m_par);



    QString label = "Закрытые потребители";
    TableWindow *tab = new TableWindow(m_cxema.m_db, label, label, q, this, -1, true, true, false);
    tab->show();

    connect(tab, SIGNAL(accepted()), this, SLOT(onEditFindNodeAccepted()));
}


void GidWidget::onPotrOtkl() // Отключенные потребители
{
    QString q = readQ("sql/any/potrotkl.sql");
    q.replace("$fragments$", m_cxema.m_par);

    QString label = "Отключенные потребители";
    TableWindow *tab = new TableWindow(m_cxema.m_db, label, label, q, this, -1, true, true, false);
    tab->show();

    connect(tab, SIGNAL(accepted()), this, SLOT(onEditFindNodeAccepted()));

}


void GidWidget::onUtZakr() // Закрытые участки
{

    QString q = readQ("sql/any/UtZakr.sql");
    q.replace("$fragments$", m_cxema.m_par);

    QString label = "Закрытые участки";
    TableWindow *tab = new TableWindow(m_cxema.m_db, label, label, q, this, -1, true, true, false);
    tab->show();

    connect(tab, SIGNAL(accepted()), this, SLOT(onEditFindLineAccepted()));
}


void GidWidget::onUtZakrAll() // Отключенные участки
{

    QString q = readQ("sql/any/UtZakrAll.sql");
    q.replace("$fragments$", m_cxema.m_par);


    QString label = "Отключенные участки";
    TableWindow *tab = new TableWindow(m_cxema.m_db, label, label, q, this, -1, true, true, false);
    tab->show();

    connect(tab, SIGNAL(accepted()), this, SLOT(onEditFindLineAccepted()));
}


void GidWidget::onZapOtr() // Отрицательные перепады
{
    CLINE2* l;
    auto pp = m_cxema.m_graph->map_node.begin();

    while (pp != m_cxema.m_graph->map_node.end()) {
        CNode2* p = pp->second;
        if (p->node.data.pihP < p->node.data.pihO && p->node.data.pihP != 0 && p->node.data.pihO != 0) {
            p->node.isPjezo = 1;
            for (l = p->lines; l; l = nextl(l)) {
                bline(l)->line.isPjezo = 1;
            }
        }
        pp++;
    }
    repaint();
}

bool getTG(long hSourceID, double tn, double &t1, double &t2, double &t3, double &tv);
bool readTG(QSqlDatabase &db, QSqlQuery &query);

void GidWidget::onPtTempMin() // Низкие температуры
{

//    QString str = AfxGetApp()->GetProfileString(szSection, "Tn", "-32");
//    double Tn = atof(str);

    QSettings settings;

    double Tn = settings.value("param/Tn", -25).toDouble();
    double t1, t2 = 0, t3, tv;

//    bool readTG(CAdoFile * ado);

//    CAdoFile* ado = m_cxema->m_ado;

    QSqlQuery query(m_cxema.m_db);
    query.setForwardOnly(true);

    readTG(m_cxema.m_db, query);

    CLINE2* l;
    auto pp = m_cxema.m_graph->map_node.begin();

    MMenuDial menu(this, tr("Выберите потребитель с низкими температурами"));

    while (pp != m_cxema.m_graph->map_node.end()) {
        CNode2* p = pp->second;

        const RasCx* cx = m_cxema.getRasCx(p->node.id_kod);

        if (cx) {
            getTG(cx->heatSourceID, Tn, t1, t2, t3, tv);
        }

        double t1_ = p->node.data.tempP;

        if ((p->node.typ == TIP_PR || p->node.typ == TIP_PO) && t1_ < t2) {

            double Q = p->node.Qot + p->node.Qgvs + p->node.Qvent;

            if (!(p->node.qz == 0 && p->node.qP == 0 && p->node.qO == 0 && Q != 0)) {
                p->node.isPjezo = 1;
                QString str = p->getName();

                menu.Add(str, (int)p->id);

                for (l = p->lines; l; l = nextl(l)) {
                    bline(l)->line.isPjezo = 1;
                }
            }
        }
        pp++;
    }
    if (menu.cnt() == 0) {
        QMessageBox::warning(this, "", tr("Нет потребителей с низкими температурами"));
    }
    else {
        if (menu.exec() == QDialog::Accepted) {
            int id = menu.value().value<int>();
            CNode2* n = m_cxema.m_graph->find(id);
            if (n) {
                moveNode(n);
//                BeginDrawNode(n);
            }
        }
    }



    repaint();
}


void GidWidget::onZavozd() // Завоздушивание
{

    QString q = readQ("sql/any/Zavozd.sql");
    q.replace("$fragments$", m_cxema.m_par);

    QString label = "Потребители с завоздушиванием";
    TableWindow *tab = new TableWindow(m_cxema.m_db, label, label, q, this, -1, true, true, false);
    tab->show();

    connect(tab, SIGNAL(accepted()), this, SLOT(onEditFindNodeAccepted()));
}


void view_db2(DbWindow *view, const QString & title, QWidget *parent);

void GidWidget::onIsprLast() // Изменённые объекты
{
    QString q = readQ("sql/history/history.sql");
    QString tn = "История изменений";

    DbWindow *table = getTableView(m_cxema.m_db, tn, q, tn);

    if (!table) {
        QMessageBox::warning(this, "", QString("Ошибка"));
        return;
    }

    view_db2(table, tn, this);
}




void GidWidget::onZona() // Гидростатические зоны
{
}



void GidWidget::onNagrOrg() // Юридические лица
{
}


void GidWidget::onTuTable() // Технические условия
{
}


void GidWidget::onTuYear() // ТУ по годам
{
}


//void GidWidget::onTuSost() // ТУ состояние


#include "tu/ReportInputDialog.h"

void print_tu_itog(QSqlDatabase &db, int y1, int y2, int y, double coef);


void GidWidget::onTuExcel() // Итоговый тепловой баланс
{
    ReportInputDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        int y1 = dialog.startYear();
        int y2 = dialog.endYear();
        int y = dialog.sourceYear();
        double coef = dialog.coefficient();
        print_tu_itog(m_cxema.m_db, y1, y2, y, coef);
    }
}


void GidWidget::onTuSvod() // Итоговые по годам
{
}


void GidWidget::onTuZhurnal() // Журнал регистрации ТУ
{
}


void GidWidget::onFindTuIst() // ТУ по источнику
{
    Klassif* kls = m_kl_list.findKlN("zdaniya_tu");
    if (!kls) {
        QMessageBox::information(this, "", tr("Не подключён файл Технических условий"));
        return;
    }

    QString q = "SELECT DISTINCT istochnik FROM tehnicheskie_usloviya"
                " WHERE istochnik IS NOT NULL AND istochnik <> '' ORDER BY istochnik";

    QStringList items;
    QSqlQuery query(*kls->m_db);
    query.setForwardOnly(true);
    if (query_exec(*kls->m_db, query, q)) {
        while (query.next()) {
            items << query.value(0).toString();
        }
    }

    bool ok = false;
    QString ist = QInputDialog::getItem(this, tr("ТУ по источнику"),
                                        tr("Выберите источник тепла"), items, 0, true, &ok);
    if (!ok || ist.isEmpty()) return;

    std::set<int> set_id;
    q = QString("SELECT DISTINCT z.id FROM tehnicheskie_usloviya t"
                " JOIN zdaniya_tu z ON t.zdanie=z.id"
                " WHERE t.istochnik='%1'").arg(QString(ist).replace("'", "''"));

    if (query_exec(*kls->m_db, query, q)) {
        while (query.next()) {
            set_id.insert(query.value(0).toInt());
        }
    }

    double x1 = 1e10, x2 = -1e10, y1 = 1e10, y2 = -1e10;
    for (auto gl: kls->geo4) {
        gl->mark = set_id.find(gl->nom) != set_id.end();
        if (gl->mark) {
            const CFRect &r = gl->rect;
            if (r.left != 0 || r.right != 0 || r.top != 0 || r.bottom != 0) {
                x1 = qMin(x1, r.left);
                x2 = qMax(x2, r.right);
                y1 = qMin(y1, qMin(r.top, r.bottom));
                y2 = qMax(y2, qMax(r.top, r.bottom));
            }
        }
    }

    repaint();
    if (x1 != 1e10) {
        moveRect(CFRect(x1, y1, x2, y2));
    }
}

/*
void GidWidget::onTuFind() // ТУ, Договора
{
}
*/


void GidWidget::onPrisNagrEdit() // Присоединенная нагрузка по источникам
{
}


void GidWidget::onQGvs() // Коэффициент перевода k=Qгвmax/Qгвср
{
}


void GidWidget::onNagrZd() // Физические лица
{
}


void GidWidget::onNagrZdNeiz() // Физические лица
{
}


void GidWidget::onNagrOrgNeiz() // Юридические лица
{
}


void GidWidget::onTuTableNeiz() // Ненайденные ТУ на карте
{
    QString q = QString(
        "SELECT t.id, "
        "t.nomer_tu AS %1, "
        "t.data_vydachi_tu AS %2, "
        "t.naimenovanie_organizatsii__zaprashivayuschey_tu AS %3, "
        "t.naimenovanie_obekta AS %4, "
        "t.adres_obekta AS %5, "
        "t.istochnik AS %6 "
        "FROM tehnicheskie_usloviya t "
        "WHERE t.zdanie IS NULL "
        "OR NOT EXISTS (SELECT 1 FROM zdaniya_tu z WHERE z.id = t.zdanie) "
        "ORDER BY t.data_vydachi_tu NULLS LAST, t.nomer_tu")
        .arg(quot_text("Номер ТУ"), quot_text("Дата выдачи ТУ"),
             quot_text("Организация"), quot_text("Наименование объекта"),
             quot_text("Адрес объекта"), quot_text("Источник"));

    QString title = tr("Ненайденные ТУ на карте");

    DbWindow *table = getTableView(m_cxema.m_db, "tehnicheskie_usloviya", q, title);
    if (!table) {
        QMessageBox::warning(this, "", tr("Нет данных"));
        return;
    }
    table->setEdit(false);
    view_db2(table, title, this);
}


void GidWidget::onTuIst() // Отчет по источникам
{
    QString q = QString(
        "SELECT "
        "ist.naimenovanie AS %1, "
        "_nagr.nagr AS %2, "
        "prirost AS %3, "
        "god AS %4, "
        "raspolagaemaya_moschnost_ov AS %5, "
        "raspolagaemaya_moschnost_gvs_srednyaya AS %6, "
        "raspolagaemaya_moschnost_summarnaya AS %7, "
        "normativnye_teplovye_poteri AS %8, "
        "prisoedinennaya_moschnost_otoplenie_1 AS %9, "
        "prisoedinennaya_moschnost_ventilyatsiya_1 AS %10, "
        "prisoedinennaya_moschnost_gvs_maksimalnaya_1 AS %11, "
        "prisoedinennaya_moschnost_par_1 AS %12, "
        "prisoedinennaya_moschnost_otoplenie AS %13, "
        "prisoedinennaya_moschnost_ventilyatsiya AS %14, "
        "prisoedinennaya_moschnost_gvs_maksimalnaya AS %15, "
        "prisoedinennaya_moschnost_par AS %16 "
        "FROM ("
        "    SELECT istochnik_tepla, sum(nagr) AS nagr FROM ("
        "       SELECT istochnik_tepla, nagruzka_gvs+nagruzka_otoplenie AS nagr FROM zhile"
        "    UNION"
        "       SELECT istochnik_tepla, nagruzka__otoplenie_+nagruzka__par_+nagruzka__ventilyatsiya_+nagruzka__gvs_ AS nagr"
        "       FROM organizatsii"
        "    ) _T GROUP BY istochnik_tepla"
        ") _nagr "
        "RIGHT JOIN istochniki_tepla ist ON _nagr.istochnik_tepla=ist.naimenovanie "
        "   OR (_nagr.istochnik_tepla IS NULL AND ist.naimenovanie='АО \"АлЭС\" (ТЭЦ-1, ТЭЦ-2, ЗТК)') "
        "LEFT JOIN (SELECT istochnik, sum(prirost_nagruzki) AS prirost FROM tehnicheskie_usloviya GROUP BY istochnik) tu "
        "   ON tu.istochnik = ist.naimenovanie "
        "LEFT JOIN prisoedinennaya_nagruzka_istochnikov pr ON pr.id2=ist.id "
        "WHERE god = (SELECT max(god) FROM prisoedinennaya_nagruzka_istochnikov) "
        "ORDER BY pr.id2")
        .arg(quot_text("Наименование"), quot_text("Нагрузка"), quot_text("Прирост нагрузки"),
             quot_text("Год"), quot_text("Располагаемая мощность ОВ"),
             quot_text("Располагаемая мощность ГВС средняя"), quot_text("Располагаемая мощность суммарная"),
             quot_text("Нормативные тепловые потери"),
             quot_text("Присоединенная мощность Отопление 1"), quot_text("Присоединенная мощность Вентиляция 1"),
             quot_text("Присоединенная мощность ГВС максимальная 1"), quot_text("Присоединенная мощность Пар 1"),
             quot_text("Присоединенная мощность Отопление"), quot_text("Присоединенная мощность Вентиляция"),
             quot_text("Присоединенная мощность ГВС максимальная"), quot_text("Присоединенная мощность Пар"));

    QString title = tr("Отчет по источникам");

    DbWindow *table = getTableView(m_cxema.m_db, "istochniki_tepla", q, title);
    if (!table) {
        QMessageBox::warning(this, "", tr("Нет данных"));
        return;
    }
    table->setEdit(false);
    view_db2(table, title, this);
}


void GidWidget::onPtsInfo() // Информация по объекту
{
}

#include <table/DbWindow.h>

const map<QString, Klassif*> *init_pts_list_file(const QString & fn1);
QString getGeoSelect(Klassif *kls, CGraph2 *graph);
QString getLineMarkTable(CGraph2 *graph);

QString getQForMarkLine(CGraph2 * graph, bool br);
QString getQForMarkNode(CGraph2 * graph, bool br);

QString getQForMarkLineSort(CGraph2 * graph, bool br);
QString getQForMarkNodeSort(CGraph2 * graph, bool br);



void isprQForMark2(CGraph2 * graph, bool is_line, int m_fileID, QString &q)
{
    QString pp = "";
    QString where = "";
    if (is_line) {
        pp = getQForMarkLine(graph, true);
        if (pp != "") {
            q = q.replace("-- ++ --", QString("\njoin (values %1) t(id) on t.id=l.id\n").arg(pp));
        }
    }
    else {
        pp = getQForMarkNode(graph, true);
        if (pp != "") {
            q = q.replace("-- ++ --", QString("\njoin (values %1) t(id) on t.id=n.id\n").arg(pp));
        }
    }
}


bool isprQForMark2Sort(CGraph2 * graph, bool is_line, int m_fileID, QString &q)
{
    QString pp = "";
    QString where = "";
    if (is_line) {
        pp = getQForMarkLineSort(graph, true);
        if (pp != "") {
            q = q.replace("-- ++ --", QString("\njoin (values %1) t(id, id_ord) on t.id=l.id\n").arg(pp));
            return true;
        }
    }
    else {
        pp = getQForMarkNodeSort(graph, true);
        if (pp != "") {
            q = q.replace("-- ++ --", QString("\njoin (values %1) t(id, id_ord) on t.id=n.id\n").arg(pp));
            return true;
        }
    }
    return false;
}



void view_db2(DbWindow *view, const QString & title, QWidget *parent);


void GidWidget::onPtsTable() // Таблица
{
    const map<QString, Klassif*> *map_pts = init_pts_list_file("pts");

    if (!map_pts) return;

    MMenuDial menu(this, tr("Таблицы"));

    menu.Add(tr("Узлы ПТС"), -5);
    menu.Add(tr("Участки ПТС"), -6);


    for (auto & [tn, kls] : *map_pts) {
        menu.Add(kls->RusName(), QVariant::fromValue(kls));
    }
    if (menu.exec() == QDialog::Accepted) {
        int id = menu.value().value<int>();
        QString q = "";

        Klassif * kls = nullptr;
        QSqlDatabase *db = &m_cxema.m_db;
        QString nazv = "";
        QString title = "";
        bool is_node = false;
        bool is_line = false;


        if (id == -5) {
            is_node = true;
            nazv = "nodes";
            title = "Узлы ПТС";
            q = readQ("sql/us_big8.sql");
            isprQForMark2(getCxema()->m_graph, false, -1, q);
        }
        else if (id == -6) {
            is_line = true;
            q = readQ("sql/ut_big8.sql");
            nazv = "heatPipeSections";
            nazv = "linesobj";
            title = "Участки ПТС";
            isprQForMark2(getCxema()->m_graph, true, -1, q);
        }
        else {
            kls = menu.value().value<Klassif *>();
            nazv = kls->nazv;
            title = kls->RusName();
            db = kls->m_db;
            q = getGeoSelect(kls, getCxema()->m_graph);
        }

        DbWindow *table = getTableView(*db, nazv, q, title);
        if (table) {
            table->setGidWidget(this);

            if (kls) table->setGeo(nazv);
            if (is_line) table->setMoveTo(move_to_line);
            if (is_node) table->setMoveTo(move_to_node);

            if (is_node || is_line) {
                table->setDop("PTS");
            }

            table->setEdit(true);
            view_db2(table, title, this);


            //                      main_window->addWidgetTab(table, title, Qt::BottomDockWidgetArea);
        }
        else {
            QMessageBox::warning(nullptr, "", QString("Нет данных в таблице %1").arg(title));
        }
    }
}


void GidWidget::onPtsDel() // Удалить объект
{
}


void GidWidget::onProtPass() // Протяженность
{
}


void GidWidget::onProtNew() // Протяженность выделенных трубопроводов
{
}


void GidWidget::onProtMag() // Протяженность магистралей
{
}


void GidWidget::onProtRs() // Протяженность распредсетей
{
}


void GidWidget::onProtKot() // Протяженность котельных
{
}


void GidWidget::onPaspNew() // Паспорт сети
{
}


void GidWidget::onViewToolbarRemont2() // Панель Контуры ремонта
{
}


void GidWidget::onNaprRemont2() // Создать Контур ремонта...
{
}


void GidWidget::onNaprTableRemont2() // Таблица участков Контура ремонта...
{
}


void GidWidget::onSaveRemont2() // Сохранить текущий Контур ремонта...
{
}


void GidWidget::onSaveRemont2New() // Сохранить новый Контур ремонта...
{
}



void GidWidget::onRemontsInfo() // Информация по объекту
{
}


void GidWidget::onRemontsTable() // Таблица
{
}


void GidWidget::onRemontsDel() // Удалить объект
{
}


void GidWidget::onZhurnalDefect() // Повреждения
{
}


void GidWidget::onZhurnalDiag() // Диагностика
{
}


void GidWidget::onZhurnalRemont() // Ремонты
{
}


void GidWidget::onZhurnalExcel() // Excel
{
}




void GidWidget::onNaprOpres() // Создать Контур опрессовок...
{
}


void GidWidget::onRemontsAddOpres() // Создать Повреждение опрессовок...
{
}


void GidWidget::onZhurnalDefectOpres() // Журнал Повреждений опрессовок...
{
}


void GidWidget::onNaprTableOpres() // Таблица участков Контура опрессовок...
{
}


void GidWidget::onSaveOpres() // Сохранить текущий Контур опрессовок...
{
}


void GidWidget::onSaveOpresNew() // Сохранить новый Контур опрессовок...
{
}




void GidWidget::onKorrozAdd() // Установить индикатор коррозии
{
    Klassif *kls = m_kl_list.findKlN(INDIKATOR_KORROZII);

    if (kls) {
        addGeo(kls);
    }
    else {
        QMessageBox::warning(this, tr(""), QString(tr("В системе нет объекта %1").arg(INDIKATOR_KORROZII)));
    }
}


void GidWidget::onKorrozDel() // Удалить индикатор коррозии
{
}


void GidWidget::onFragmentPolyKorroziaAr() // Удалить выбранные индикаторы
{
}


void GidWidget::onKorrozInfo() // Информация по индикатору
{
}


void GidWidget::onKorroziaUpdate() // Выбор шаблона плана
{
}


void GidWidget::onKorrozDoc3() // План установки
{
}


void GidWidget::onKorrozTable() // Таблица индикаторов коррозии
{
}


void GidWidget::onKorroziaZhurnal1() // План индикаторов
{
}


void GidWidget::onKorroziaZhurnal2() // Индикаторы установленные
{
}


void GidWidget::onKorroziaZhurnal3() // Индикаторы извлеченные
{
}


void GidWidget::onKorroziaCurrent() // Все индикаторы сезона
{
}


void GidWidget::onKorrozDoc2() // Оценка за отопительный сезон
{
}


void GidWidget::onKorrozDoc1() // Оценка за выбранные сезоны
{
}


void GidWidget::onPassportOnOff() // Включить/отключить отображение объектов ПТС
{
    geoOnOffFile("pts", tr("Выберите объекты ПТС"));
}

void GidWidget::onKorroziaOnoff() // Включить/отключить отображение
{
//    geoOnOffFile("remont", tr("Выберите "));
}


void GidWidget::onKorroziaSezon() // Выбор отопительного сезона
{
}


void GidWidget::onIznos() // Износ оборудования
{
}


void GidWidget::onElectroAdd() // Установить объект
{
}


void GidWidget::onElectroInfo() // Информация по объекту
{
}


void GidWidget::onElectroTable() // Таблица
{
}


void GidWidget::onElectroDel() // Удалить объект
{
}


void GidWidget::onZhurnalElectro() // Журнал
{
}

#include <ui/Choose_loses_typeDialog.h>

static QSqlDatabase *m_db = nullptr;
static QString m_city = "Неизвестный город";


int readTableInt(QSqlDatabase &db, QSqlQuery &query, const QString & q);


void mk_config_json(const CConnect &cn)
{

    QString q = QString(
"       {\n"
"        \"postgresql\": {\n"
"            \"host\": \"%1\",\n"
"            \"user\": \"%2\",\n"
"            \"password\": \"%3\",\n"
"            \"port\": %4,\n"
"            \"db\": \"%5\"\n"
"    }\n}").arg(cn.host, cn.user, cn.password).arg(cn.port).arg(cn.baza);


     QString path_temp = QDir::tempPath();
     QFile file2(path_temp + "/config.json");
     if (file2.open(QIODevice::WriteOnly)) {
         QTextStream out(&file2);
         out << q;

         file2.close();
     }
}


void init(CChoose_loses_typeDialog *dlg, QSqlDatabase &db)
{
    QString heatLosesMain = "heatLosesMain";

//    QString q = QString("SELECT d1, d2, CONCAT(CONVERT(VARCHAR(10), d1, 104), ' - ', CONVERT(VARCHAR(10), d2, 104)) AS d1d2 FROM %1").arg(heatLosesMain);
    QString q = QString("SELECT d1, d2, CONCAT(to_char(d1, 'dd.mm.yyyy'), ' - ', to_char(d2, 'dd.mm.yyyy')) AS d1d2 FROM %1").arg(heatLosesMain);

            QSqlQuery query(db);
    query.setForwardOnly(true);

    dlg->ui.combo_Combo1->clear();

    if (query_exec(db, query, q)) {
        while (query.next()) {
            QDate d1 = query.value("d1").toDate();
            QDate d2 = query.value("d2").toDate();
            QString d1d2 = query.value("d1d2").toString();

            dlg->ui.combo_Combo1->addItem(d1d2);
        }
    }
}


void CChoose_loses_typeDialog::onAdd_heating_date() {
    read();

    QDate d1 = this->m_dtDatetimepicker1;
    QDate d2 = this->m_dtDatetimepicker2;

    QString heatLosesMain = "heatLosesMain";

    /*
    if (m_norm_fact == 1) {
        heatLosesMain = "heatLosesMainFact";
    }
    */
    QSqlQuery query(*m_db);

    QString sd1 = d1.toString("yyyy-MM-dd");
    QString sd2 = d2.toString("yyyy-MM-dd");

    QString q = QString("SELECT id FROM %1 WHERE d1='%2' AND d2='%3'").arg(heatLosesMain, sd1, sd2);

    int id = readTableInt(*m_db, query, q);

    if (id > 0) {
        QMessageBox::warning(this, "", "Отопительный сезон уже задан!");
        return;
    }

    q = QString("INSERT into %1 (city, d1, d2) values ('%2', '%3', '%4')").arg(heatLosesMain, m_city, sd1, sd2);
    query_exec(*m_db, query, q);

    init(this, *m_db);

    write();
}

void CChoose_loses_typeDialog::onDel() {
    read();

    if (this->m_cmbCombo1 < 0) return;

    QString text = this->ui.combo_Combo1->currentText();

    QString s = QString("Удалить отопительный сезон %1?").arg(text);
    if (QMessageBox::question(0, "", s) != QMessageBox::Yes) return;


//    int l = regex_match("([0-9]+)\\.([0-9]+)\\.([0-9]+) \\- ([0-9]+)\\.([0-9]+)\\.([0-9]+)", m_str_date, match);

    QString heatLosesMain = "heatLosesMain";
    QString q = QString("delete from %1 where CONCAT(to_char(d1, 'dd.mm.yyyy'), ' - ', to_char(d2, 'dd.mm.yyyy')) = '%2'").arg(heatLosesMain, text);
    QSqlQuery query(*m_db);

    query_exec(*m_db, query, q);

    init(this, *m_db);

    write();
}

void GidWidget::onTeplopoteri() // Теплопотери
{
    CChoose_loses_typeDialog dlg;
    m_db = &m_cxema.m_db;
    m_city = m_cxema.m_city;

    dlg.m_dtDatetimepicker1 = QDate(2024, 10, 15);
    dlg.m_dtDatetimepicker2 = QDate(2025, 4, 15);
    dlg.ui.date_Datetimepicker1->setCalendarPopup(true);
    dlg.ui.date_Datetimepicker2->setCalendarPopup(true);

    dlg.ui.group_Static3->setVisible(false);
    dlg.ui.radio_Radio1->setVisible(false);
    dlg.ui.radio_Radio2->setVisible(false);



    init(&dlg, m_cxema.m_db);

    dlg.m_bFragment = false;

    dlg.write();

    if (dlg.exec() == QDialog::Accepted) {
        if (dlg.m_cmbCombo1 < 0) {
            QMessageBox::warning(this, "", "Выберите сезон");
            return;
        }
        if (dlg.m_cmbCombo1 >= 0) {
            QString text = dlg.ui.combo_Combo1->currentText();

            QRegularExpression regex("([0-9]+\\.[0-9]+\\.[0-9]+) \\- ([0-9]+\\.[0-9]+\\.[0-9]+)");
            QRegularExpressionMatch match = regex.match(text);

            if (match.hasMatch()) {
                QString sd1 = match.captured(1);
                QString sd2 = match.captured(2);

                sd1.replace(".", "/");
                sd2.replace(".", "/");


bool create_vyd_node_table0(QSqlDatabase &db, CGraph2* graph, const QString & tn);
bool create_vyd_line_table0(QSqlDatabase &db, CGraph2* graph, const QString & tn);

                QString yesno = "no";

                if (dlg.m_bFragment) {
                    create_vyd_node_table0(m_cxema.m_db, m_cxema.graph(), "TEMP_NODE");
                    create_vyd_line_table0(m_cxema.m_db, m_cxema.graph(), "TEMP_LINE");
                    yesno = "yes";
                }


                QString par = m_cxema.m_par;
//                par.replace(",", "");

                QString args = QString("%1 %2 %3 %4 %5 %6")
                    .arg("norm")
                    .arg(sd1)
                    .arg(sd2)
                    .arg(m_cxema.m_city)
                    .arg(yesno)
                    .arg(par);

                QString q_city = QString("SELECT id FROM cities where name='%1'").arg(m_cxema.m_city);

                int city_id = readTableInt(m_cxema.m_db, q_city);
                if (city_id == 0) {
                    QString q = QString("INSERT INTO cities (name) VALUES ('%1')").arg(m_cxema.m_city);
                    query_exec(m_cxema.m_db, q);
                }


//                    city, yesno, m_cxema->m_par;

                mk_config_json(m_cxema.m_connect);

                QString str = str_python("poteriNewPg/tp_main.py", args, false);

                qDebug() << str;

                QProcess *process = new QProcess();
                process->startCommand(str);

                // Проверяем, успешно ли программа запустилась
                if (!process->waitForStarted(5000)) { // 5000 мс на проверку запуска
                    QMessageBox::warning(this, "", "Не удалось запустить программу");
                    delete process;
                } else {
                    qDebug() << "Программа успешно запущена.";
                }

//                HINSTANCE hRun = ShellExecute (AfxGetMainWnd()->GetSafeHwnd (), "open", pythonw_exe(), str, path, SW_SHOW);

            }
        }
    }
}

#if 0

#include <ui/Colors2Dialog.h>


void CColors2Dialog::onPod() {
    read();
    write();
}

void CColors2Dialog::onObr() {
    read();
    write();
}

void CColors2Dialog::onDefault() {
    read();
    write();
}

void CColors2Dialog::onPodf() {
    read();
    write();
}

void CColors2Dialog::onObrf() {
    read();
    write();
}

void CColors2Dialog::onColor_uzel() {
    read();
    write();
}

void CColors2Dialog::onDefault2() {
    read();
    write();
}



void GidWidget::onColors() // Стиль линии
{
    CColors2Dialog dlg(this);

    dlg.ui.combo_Open->addItem("Нагруженная");
    dlg.ui.combo_Open->addItem("Не нагруженная");
    dlg.ui.combo_Open->addItem("Закрытая");
    dlg.ui.combo_Mag_ras->addItem("Магистраль");
    dlg.ui.combo_Mag_ras->addItem("Распредсеть");
    dlg.ui.combo_Mag_ras->addItem("Паропровод");
    dlg.ui.combo_Typ->addItem("Надземные");
    dlg.ui.combo_Typ->addItem("Бесканальные");
    dlg.ui.combo_Typ->addItem("Канальные");
    dlg.ui.combo_Typ->addItem("Подвальные");

//    dlg.m_bPunkt;
//    dlg.m_cmbOpen;
//    dlg.m_cmbMag_ras;
//    dlg.m_cmbTyp;
//    dlg.m_iLine_thickness;

    dlg.write();

    dlg.exec();
}

#endif

#include <QColorDialog>
#include <QFontDialog>

void GidWidget::onFon() // Цвет фона
{
    QColorDialog dlg(m_bk_color, this);

    if (!dlg.exec()) return;

    m_bk_color = dlg.currentColor();
    repaint();
}


/*
void GidWidget::onFont2() // Шрифт текста
{
    QFontDialog dlg(this);

    if (!dlg.exec()) return;

    QFont font = dlg.currentFont();
    m_font2 = font;
    repaint();
}
*/


// Здесь

void GidWidget::onFont() // Шрифт надписей
{
    QFontDialog dlg(this);
    dlg.setCurrentFont(m_font1);

    if (!dlg.exec()) return;

    m_font1 = dlg.currentFont();
    repaint();
}


void GidWidget::onFontMag() // Шрифт названия магистрали
{
}


void GidWidget::onFontPanel() // Шрифт в проводнике
{
}

int getSliderValue(QWidget *parent, const QString & text, int value);


void GidWidget::onTransp() // Прозрачность
{
    m_nTransp = getSliderValue(this, "Прозрачность", m_nTransp);
}


void GidWidget::onAstr(bool on) // Сетка
{
//    setAstr(on);
    repaint();
}


void GidWidget::onRamka(bool on) // Рамка схемы
{
    m_bIsRamka = on;
    repaint();
}


void GidWidget::onIsMas(bool on) // Сохранять пропорции
{
    m_bIsMas = on;
    repaint();
}

void GidWidget::onMasPic() // Номинальный масштаб
{
    bool ok;
    double MasPic = QInputDialog::getDouble(this, tr("Номинальный масштаб"),
                                 tr("Введите номинальный масштаб"), getStdMas(), 1, 100000, 0, &ok);

    if (ok) {
        setStdMas(MasPic);
        repaint();
    }
}


void GidWidget::onMasPodpis() // Масштаб откл.надписей
{
    bool ok;
    double MasPodp = QInputDialog::getDouble(this, tr("Масштаб отключения надписей"),
                                 tr("Введите масштаб отключения надписей"), getMasPodp(), 1, 100000, 0, &ok);

    if (ok) {
        setMasPodp(MasPodp);
        repaint();
    }
}


void GidWidget::onTrio() // Передвинуть
{
}

void GidWidget::onIndexes() // Обновление индексов
{
    QString q = readQ("sql3/update_index.sql");
    q.replace("$BAZA$", getDatabaseName(m_cxema.m_db));

//    CWaitCursor wait;
    QSqlQuery query(m_cxema.m_db);
    if (query_exec(m_cxema.m_db, query, q)) {
        QMessageBox::warning(this, "", tr("Готово"));
    }
    else {
        QMessageBox::warning(this, "", tr("Ошибка"));
    }
}

void GidWidget::onUpdateSetup() // Настройка обновлений
{
}

#if 0

#include <ui2/Webserver_dialog2Dialog.h>

void GidWidget::onHttp2() // Веб-сервер
{
    CWebserver_dialog2Dialog dlg(this);
    dlg.exec();
}

#endif


#include <table/MultiHeaderTable.h>

MultiHeaderTable* getMyTableView(QSqlDatabase &db, const QString & tn, const QString & q, const QString & title);
void view_my_table(MultiHeaderTable *view, const QString & title, QWidget *parent);

void GidWidget::onQuery() // Выполнить запрос
{
    QString q0 = "";
    bool ok;

    QString q =  QInputDialog::getMultiLineText(this, "", "Введите запрос", q0, &ok);

    if (ok && !q.isEmpty()) {

        QSqlQuery query(m_cxema.m_db);

        if (!query.exec(q)) {
            QMessageBox::critical(nullptr, "Ошибка SQL", query.lastError().text());
            return;
        }

        QSqlRecord rec = query.record();
        if (rec.isEmpty()) {
            // Нет набора данных (например UPDATE/DELETE/INSERT)
            int affected = query.numRowsAffected();
            QMessageBox::information(nullptr, "Результат",
                                     QString("Запрос выполнен. Затронуто строк: %1").arg(affected));
        }
        else {
            MultiHeaderTable* table = getMyTableView(m_cxema.m_db, "Запрос", q, "");

            if (table) {
                table->setGidWidget(this);
                table->setGeo("");
                table->setEdit(false);

                view_my_table(table, "Запрос", this);
            }
        }
    }
}




void GidWidget::onExportPts() // Экспорт фрагмента ПТС
{
}


void GidWidget::onImportPts() // Импорт фрагмента ПТС
{
}


void GidWidget::onSetAvtoOn() // Сделать потребители автоматическими
{
}


void GidWidget::onSetAvtoOff() // Сделать потребители неавтоматическими
{
}


void GidWidget::onCreateSortNode() // Создание таблицы sortNodesForUchastok
{
}


void view_db2(DbWindow *view, const QString & title, QWidget *parent); // table/table_part.cpp

void GidWidget::onRasList() // Список расчетов
{
    QString title = tr("Список расчетов");
    QString q = QString(
        "SELECT c.id, f.name AS fragment, c.date1, c.user_gid, c.name, c.tn"
        " FROM calculation c"
        " LEFT JOIN fragments f ON f.id=c.fileid"
        " WHERE c.fileid IN (%1)"
        " ORDER BY f.name NULLS LAST, c.date1 DESC NULLS LAST").arg(m_cxema.m_par);

    DbWindow *view = getTableView(m_cxema.m_db, "calculation", q, title);

    if (view) {
        view->setGidWidget(this);
        view_db2(view, title, this);
    }
}


void GidWidget::onSetPsMap() // Установить объекты ПС
{
}


void GidWidget::onSavePng() // Сохранить как PNG
{
}


void GidWidget::onUtNapr2() // Сортировка участков
{
}


void GidWidget::onInvert() // Инвертировать выделение
{
}


void GidWidget::onSetAddr() // Адреса потребителей
{
}


void GidWidget::onAlma() // Паспортизация
{
}


void GidWidget::onTuZav() // Установить статус Завершен для ТУ
{
}


static bool write_out_table(QTextStream &ts, QSqlDatabase &db, const QString &out_table, int calcid, const QString &key_col)
{
    QString q = QString("SELECT * FROM %1 WHERE calculationid=%2 ORDER BY %3").arg(out_table).arg(calcid).arg(key_col);

    QSqlQuery query(db);
    query.setForwardOnly(true);
    if (!query_exec(db, query, q)) {
        return false;
    }

    QSqlRecord rec = query.record();
    QStringList head;
    for (int c = 0; c < rec.count(); c++) {
        head << rec.fieldName(c);
    }
    ts << head.join('\t') << "\n";

    while (query.next()) {
        QStringList row;
        for (int c = 0; c < rec.count(); c++) {
            row << query.value(c).toString();
        }
        ts << row.join('\t') << "\n";
    }

    return true;
}

void GidWidget::onExport() // Экспорт гидравлики в TXT
{
    if (m_fileID == -1) {
        onMainCxema();
        if (m_fileID == -1) {
            return;
        }
    }

    int calcid = -1;
    QString q = QString("SELECT max(id) FROM calculation WHERE fileid=%1").arg(m_fileID);
    QSqlQuery query(m_cxema.m_db);
    if (query_exec(m_cxema.m_db, query, q) && query.next()) {
        calcid = query.value(0).toInt();
    }

    if (calcid <= 0) {
        QMessageBox::information(this, "", tr("Нет расчетов для текущего фрагмента"));
        return;
    }

    QString fn = QFileDialog::getSaveFileName(this, tr("Экспорт гидравлики в TXT"),
                                              QString("gidr_%1.txt").arg(calcid), "TXT (*.txt)");
    if (fn.isEmpty()) return;

    QFile file(fn);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "", tr("Не могу открыть на запись файл %1").arg(fn));
        return;
    }

    QTextStream ts(&file);
    ts << tr("Расчёт") << " " << calcid << "\n\n" << "US_OUT" << "\n";
    bool ok = write_out_table(ts, m_cxema.m_db, "us_out", calcid, "nodeid");
    ts << "\n" << "UT_OUT" << "\n";
    ok = write_out_table(ts, m_cxema.m_db, "ut_out", calcid, "lineid") && ok;
    file.close();

    if (ok) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fn));
    }
}


void GidWidget::onSetLineid() // !
{
}


void GidWidget::onSetIst() // Расстановка источников
{
}


void GidWidget::onUchList() // Список участков ПТС
{
}



void GidWidget::onAsyncCheck() // Проверка Async Task
{
}


void GidWidget::onCheckPo() // Проверка
{
}


void GidWidget::onResetGeo() // Обновить надписи
{
}


void GidWidget::onPsAstanaName() // Отображение имен узлов
{
}


extern bool m_isOnlyPTSColor;

void GidWidget::onColorOnlyPts() // Цвета только для участков ПТС
{
    m_isOnlyPTSColor = !m_isOnlyPTSColor;
    repaint();
}


void GidWidget::onExportShape() // ESRI shape-файлы
{
}


void GidWidget::onPrPo() // Реальные -> обобщенные
{
}


void GidWidget::onUtNapr() // Сортировка участков
{
}


void GidWidget::onC3Del() // Удалить внутренние схемы
{
}


void GidWidget::onC3Add() // Создать внутренние схемы
{
}


void GidWidget::onIspravl() // Восстановить файл
{
}


void GidWidget::onOpc() // Настройка OPC сервера
{
}

#include <any/json.hpp>

nlohmann::json get_j(nlohmann::json& j, const string& key);
double get_j_float(nlohmann::json& j, const string& key);
string get_j_string(nlohmann::json& j, const string& key);


nlohmann::json get_j(nlohmann::json &j, const string &key)
{
    nlohmann::json::iterator it = j.find(key);

    if (it != j.end()) {
        return it.value();
    }
    nlohmann::json j0;
    return j0;
}

string get_j_string(nlohmann::json &j, const string &key)
{
    nlohmann::json::iterator it = j.find(key);

    if (it != j.end()) {
        if (it->is_string()) {
            return it.value();
        }
    }
    return "";
}

double get_j_float(nlohmann::json &j, const string &key)
{
    nlohmann::json::iterator it = j.find(key);

    if (it != j.end()) {
        if (it->is_number()) {
            return it.value();
        }
    }
    return 0;
}



bool GidWidget::readAddress(const QString & str)
{
    try {
        nlohmann::json j0;

        QByteArray  arr = str.toUtf8();

        const char* buf = (const char*) arr.constData();

        j0 = nlohmann::json::parse(buf);

        nlohmann::json j1 = get_j(j0, "features");

        int n = j1.size();

        MMenuDial menu(this, tr("Выбрать"));

        int ii = 0;

        if (n != 1) {

            for (int i = 0; i < n; i++) {
                string j2 = j1[i]["properties"]["display_name"];

                QString ss = j2.c_str();

                menu.Add(ss, i);
            }

            if (menu.exec() != QDialog::Accepted) return 0L;
            ii = menu.value().toInt();
        }

        nlohmann::json jj = j1[ii]["bbox"];

        double x1 = jj[0];
        double y1 = jj[1];
        double x2 = jj[2];
        double y2 = jj[3];

        CFPoint get_google_to_coord(double x, double y);

        CFPoint pt1 = get_google_to_coord(x1, y1);
        CFPoint pt2 = get_google_to_coord(x2, y2);

        moveRect(CFRect(pt1, pt2));

        return 0L;
    }
    catch (...) {
    }
    return true;
}

#include <any/download2.h>

static GidWidget *this_geo = nullptr;

void setView(GidWidget *view)
{
    this_geo = view;
}



GidWidget *getView()
{
    return this_geo;
}


bool fund(const QString & txt)
{
    return getView()->readAddress(txt);
}


#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>

bool GidWidget::readElevation(const QString & j)
{
    try {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(j.toUtf8(), &error);
        QJsonObject json = doc.object();
//        QJsonValue v = json["elevationProfile"][0];
//        double f = v["height"].toDouble();

        QJsonValue v = json["results"][0];
        double f = v["elevation"].toDouble();

        QInputDialog::getDouble(this, tr("Высота"),  tr("Высота, м"), f, -10000, 10000, 1, nullptr,  Qt::WindowFlags(), 1);
    }
    catch(...) {
        QMessageBox::warning(this, tr(""), "Ошибка!");
        return false;
    }

    return true;
}



bool fund2(const QString & txt)
{
    return getView()->readElevation(txt);
}


void get_google_coord(CFPoint pt, double& x, double& y);


#include <config.h>


void GidWidget::googleElevation(CFPoint pt)
{
//    QPoint point = mapFromGlobal(QCursor::pos());
//    CFPoint pt = ScreenToCoord(CFPoint(point.x(), point.y()));

    double x, y;
    get_google_coord(pt, x, y);
    QString  _url;

#if 0
    _url = QString("http://maps.googleapis.com/maps/api/elevation/json?locations=%1,%2").arg(y).arg(x);
    _url = QString("http://api.geonames.org/srtm3JSON?lat=%1&lng=%2&username=demo").arg(y).arg(x);
    _url = QString("https://elevation-api-test.azurewebsites.net/api/elevation/%1,%2").arg(y).arg(x);
    _url = QString("http://open.mapquestapi.com/elevation/v1/profile?key=MNtuOSulyjpOWnWvl4sAWCuBYexHUUzG&shapeFormat=raw&latLngCollection=%1,%2").arg(y).arg(x);

    _url = QString("http://open.mapquestapi.com/elevation/v1/profile?key=MNtuOSulyjpOWnWvl4sAWCuBYexHUUzG&shapeFormat=raw&latLngCollection=%1,%2").arg(y).arg(x);

    _url = QString("http://45.132.85.23:8084/api/v1/lookup?locations=%1,%2").arg(y).arg(x);

    QString site = "https://api.open-elevation.com";
#endif

    QString site = get_ini()->elevation_url;
    QSettings settings;
    site = settings.value("config/elevation_url", site).toString();

    _url = QString("%1/api/v1/lookup?locations=%2,%3").arg(site).arg(y).arg(x);

    QUrl url = QUrl::fromPercentEncoding(QUrl::toPercentEncoding(_url));

    this_geo = this;
    download_manager2.doDownload(url, fund2);
}

void GidWidget::onGoogleElevation() // Высота по Google
{
}


void GidWidget::onGoogleElevationSet() // Высота по Google
{
    bool ok;
    QSettings settings;
//    QString site = "https://api.open-elevation.com";
    QString site = get_ini()->elevation_url;
    site = settings.value("config/elevation_url", site).toString();

    site = QInputDialog::getText(this,"", tr("Введите сервер геодезических отметок"), QLineEdit::Normal, site, &ok);

    if (ok) {
        settings.setValue("config/elevation_url", site);
    }
}


#include <ui/inputdialog.h>



void GidWidget::onMapSearch() // Поиск
{
    QString text;

    QStringList lst_1, lst_2;

    lst_1 << "Город";
    lst_1 << "Адрес";

    QSettings settings;
    lst_2 << settings.value("mapsearch/city", "").toString();
    lst_2 << settings.value("mapsearch/addr", "").toString();

    InputDialog dial(lst_1, lst_2, this);

    if (dial.exec()) {
        QStringList lst = dial.getStrings();

        if (!lst.isEmpty()) {
            settings.setValue("mapsearch/city",  lst[0]);
            settings.setValue("mapsearch/addr",  lst[1]);

            QString text = lst[0] + " " + lst[1];
            QString _url;

//            _url = QString("http://lan.avto-glass.kz/?action=osm&q=%1").arg(text);
            _url = QString("http://nominatim.openstreetmap.org/search?q=%1&format=geojson&polygon=1&addressdetails=1").arg(text);

            QByteArray arr = QUrl::toPercentEncoding(_url);
            QUrl url = QUrl::fromPercentEncoding(arr);

            this_geo = this;
            download_manager2.doDownload(url, fund);
        }
    }
}


void GidWidget::onMapPath() // Каталог
{
}


void GidWidget::onMapYes(bool on) // Восстанавливать подключение
{
    m_bIsMapYes = on;
    repaint();
}


void GidWidget::onFileOpenOld() // Импорт
{
}


static CRight lst_right[] = {
  {R_admin,     "Администратор"},
  {R_regim,     "Группа режимов"},
  {R_proiz,     "Производственная служба"},
  {R_neud,      "Запрещено добавлять и удалять объекты гидравлики"},
  {R_akt,       "Акты раздела"},
  {R_geo,       "Геобаза"},
//  {R_neud,      "Не разрешено удалять объекты гидравлики"},
  {R_indikator, "Индикаторы коррозии"},
  {R_web_read,  "Веб приложение"},
//  {R_web_read,  "Веб приложение для чтения"},
//  {R_web_write, "Веб приложение для записи"},
  {R_remont,    "Ремонты"},
};


int rightCode(int *r)
{
    int right = 0;

    int n = sizeof(lst_right) / sizeof(lst_right[0]);
    for (int i = 0; i < n; i++) {
        int rs = lst_right[i].ID;
        if (rs < 0 || rs > 255) return 0;

        int rr = r[rs];

        if (rs == R_admin || rs == R_akt || rs == R_geo) rr = !rr;

        if (rr) right += (2 << rs);
    }

    return right;
}


void rightDecode(int right, int *r)
{
    int n = sizeof(lst_right) / sizeof(lst_right[0]);

    for (int i = 0; i < n; i++) {
        int rs = lst_right[i].ID;

        if (rs < 0 || rs > 255) return;

        r[rs] = (right & (2 << rs)) != 0;

        if (rs == R_admin || rs == R_akt || rs == R_geo) r[rs] = !r[rs];
    }
}


bool addUser(QSqlDatabase &db, const QString & user, const QString & password, int right);

void GidWidget::onAddUser() // Регистрация пользователей
{
    if (!UserRight::isAdmin()) {
        QMessageBox::warning(this, "", tr("Для выполнения этой операции у вас должны быть права администратора!"));
        return;
    }


    CCh_password_right_newDialog dlg(this);

    int n = sizeof(lst_right)/sizeof(lst_right[0]);

    for ( int i = 0; i < n; i++) {
        QListWidgetItem *item = new QListWidgetItem(lst_right[i].r, dlg.ui.lstb_Filtr);
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, lst_right[i].ID);
        dlg.ui.lstb_Filtr->addItem(item);
    }

    if (dlg.exec() == QDialog::Accepted) {
        if (dlg.m_strUser == "") {
            QMessageBox::warning(this, "", "Имя пользователя должно быть введено");
        }
        else if (dlg.m_strPassword != dlg.m_strPassword2) {
            QMessageBox::warning(this, "", "Пароли должны быть одинаковыми");
        }
        else {
            unsigned int user_right = 0;
            QList<int> out;

            for (int i = 0; i < dlg.ui.lstb_Filtr->count(); ++i) {
                QListWidgetItem* item = dlg.ui.lstb_Filtr->item(i);
                // Проверяем, отмечен ли элемент галочкой
                if (item->checkState() == Qt::Checked) {
                    // Извлекаем данные из Qt::UserRole и преобразуем в int
                    bool ok;
                    int value = item->data(Qt::UserRole).toInt(&ok);
                    if (ok) { // Проверяем, что данные корректны
                        out.append(value);
                    } else {
                        qDebug() << "Ошибка: данные в элементе" << i << "не являются int!";
                    }
                }
            }

            int n = sizeof(lst_right) / sizeof(lst_right[0]);
            for (int i = 0; i < n; i++) {
                unsigned int br = (2 <<lst_right[i].ID);
                bool right = out.contains(lst_right[i].ID);

                if (lst_right[i].ID == R_admin || lst_right[i].ID == R_akt || lst_right[i].ID == R_geo) right = !right;

                if (right) {
                    user_right |= br;
                }
            }
            addUser(m_cxema.m_db, dlg.m_strUser, dlg.m_strPassword, user_right);
            QMessageBox::information(this, "", QString("Пользователь %1 создан").arg(dlg.m_strUser));
        }
    }
}

#include <ui/Ch_passwordDialog.h>
#include <ui/Password3.h>

bool updateUserRight(QSqlDatabase &db, const QString & user, int right);


struct CUser
{
    int id;
    QString password;
    int user_right;
};

class Password3 : public QDialog {
     Q_OBJECT
public:
    Password3(QWidget *parent = nullptr) : QDialog(parent) {
        ui.setupUi(this);  // ui - это объект, сгенерированный из .ui файла

        connect(ui.push_Ok, &QPushButton::clicked, this, &Password3::close);
        connect(ui.push_Cancel, &QPushButton::clicked, this, &Password3::close);
        connect(ui.pushButton_Edit, &QPushButton::clicked, this, &Password3::edit);
        connect(ui.pushButton_Delete, &QPushButton::clicked, this, &Password3::del);
    }

public slots:
    void edit() {
        QListWidgetItem *item = ui.listWidget->currentItem();
        if (item) {
            CUser psw = item->data(Qt::UserRole).value<CUser>();
            QString curText = item->text();

            ChMenuDial dlg(this, QString(QObject::tr("Ввод прав доступа для пользователя %1")).arg(curText));

            int n = sizeof(lst_right) / sizeof(lst_right[0]);
            for (int i = 0; i < n; i++) {
                int br = (2 <<lst_right[i].ID);
                int right = psw.user_right;

                if (lst_right[i].ID == R_admin || lst_right[i].ID == R_akt || lst_right[i].ID == R_geo) right = ~right;

                dlg.Add(lst_right[i].r, QVariant::fromValue(lst_right[i].ID), right & br ? Qt::Checked : Qt::Unchecked);
            }


            if (dlg.exec()) {
                unsigned int user_right = 0;

                for (int i = 0; i < n; i++) {
                    unsigned int br = (2 <<lst_right[i].ID);
                    bool right = dlg.out.contains(lst_right[i].ID);

                    if (lst_right[i].ID == R_admin || lst_right[i].ID == R_akt || lst_right[i].ID == R_geo) right = !right;

                    if (right) {
                        user_right |= br;
                    }
                }
                if (updateUserRight(*m_db, curText, user_right)) {
                    psw.user_right = user_right;
                    item->setData(Qt::UserRole, QVariant::fromValue(psw));
                    QMessageBox::information(this, "", QString("Права пользователя %1 успешно обновлены").arg(curText));
                }
            }
        }
    }

    void del() {
        QListWidgetItem *item = ui.listWidget->currentItem();
        if (item) {
            CUser psw = item->data(Qt::UserRole).value<CUser>();

            QString curText = item->text();
            if (QMessageBox::question(0, "", QString("Удалить пользователя %1?").arg(curText)) != QMessageBox::Yes) return;
            QString q = QString("delete from passwords where id=%1").arg(psw.id);
            bool ret = query_exec(*m_db, q);
            if (ret) {
                delete ui.listWidget->takeItem(ui.listWidget->row(item));
            }
        }
    }

public:
    void init(QSqlDatabase & db) {
        CUser psw;

        m_db = &db;

        QSqlQuery query(db);

        QString q = QString("SELECT * FROM passwords order by user_name");
        if (query_exec(db, query, q)) {
            while (query.next()) {
                psw.id = query.value("id").toInt();
                psw.user_right = query.value("user_right").toInt();

                QString user = query.value("user_name").toString();
                psw.password = query.value("user_password").toString();

                QListWidgetItem *item = new QListWidgetItem(user, ui.listWidget);

                QVariant variant;
                variant.setValue(psw);
                item->setData(Qt::UserRole, variant);
                ui.listWidget->addItem(item);
            }
        }
    }

private:
    QSqlDatabase * m_db;
    Ui_Password3 ui;  // Имя класса, сгенерированного в .ui
};



void GidWidget::onPassword() // Изменить доступ пользователя
{
    if (!UserRight::isAdmin()) {
        QMessageBox::warning(this, "", tr("Для выполнения этой операции у вас должны быть права администратора!"));
        return;
    }

    Password3 dlg(this);

    dlg.init(m_cxema.m_db);

    if (dlg.exec()) {
    }
}

#include <ui/Ch_password2Dialog.h>

bool updateUserPassword(QSqlDatabase &db, const QString & user, const QString & password);
bool updateUserPassword(QSqlDatabase &db, int id, const QString & password);


void GidWidget::onPassword2() // Изменить пароль
{
    CCh_password2Dialog dlg(this);

    if (dlg.exec()) {
        if (dlg.m_strPassword != dlg.m_strPassword2) {
            QMessageBox::warning(this, "", "Пароли не совпадают");
        }
        else if (dlg.m_strOldpassword != this->m_password) {
            QMessageBox::warning(this, "", "Неверный пароль!");
        }
        else {
            if (updateUserPassword(m_cxema.m_db, this->m_user, dlg.m_strPassword2)) {
                QMessageBox::information(this, "", QString("Пароль пользователя %1 успешно изменен").arg(m_strUser));
            }
        }
    }
}


void GidWidget::onViewStatusBar() // Строка статуса
{
}

void GidWidget::onHelpFinder() // Содержание\tF1
{
    const QString dir = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
        dir + "/help/index.html",
        dir + "/gid8.chm",
        dir + "/gid8.pdf",
    };

    for (const QString &path: candidates) {
        if (QFile::exists(path)) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            return;
        }
    }

    QMessageBox::information(this, "", tr("Файл справки не найден"));
}


void GidWidget::onUpdate() // Последнее обновление
{
}

/*

void GidWidget::onAppAbout() // О программе
{
}
*/


Klassif *menu_list_file(QWidget *widget, const QString & title, const QString & fn1);


void GidWidget::onPtsAdd()  // Установить объект ПТС
{
    Klassif *kls = menu_list_file(this, "Объекты ПТС", "pts");

    if (kls) {
        addGeo(kls);
    }
}


void GidWidget::onEditFindPs() //
{
    QString q = readQ("sql4/find_node_ps.sql");
    q.replace(QString("$fileID$"), m_cxema.m_par);

    TableWindow * tab = new TableWindow(m_cxema.m_db, "node", "Выберите узел", q, this);
    tab->show();

    connect(tab, SIGNAL(accepted()), this, SLOT(onEditFindNodeAccepted()));
}



void GidWidget::onProtocol()
{
}

void GidWidget::onElectroIst()
{
}

void GidWidget::onElectroPriem()
{
}

void GidWidget::onElectroLine()
{
}

void GidWidget::onElectroMufta()
{
}

void GidWidget::onElectroOpora()
{
}

void GidWidget::onElectroGil()
{
}

void GidWidget::onElectroKanal()
{
}

void GidWidget::onElectroZhurnal()
{
}

void GidWidget::onElectroOnoff()
{
}

void GidWidget::onSetMsNew()
{
}

void GidWidget::onSetRsNew()
{
}

void GidWidget::onRasprMagNew()
{
}

void GidWidget::onMagNode()
{
}

void GidWidget::onNapr2()
{
}

void GidWidget::onVyd4()
{
}

void GidWidget::onMarkDel()
{
}

void GidWidget::onMsNode1()
{
}

void GidWidget::onMsNode2()
{
}

void GidWidget::onSaveMAG()
{
    QVariant val;
    setSomething(false, TIP_UT, "magistral", QVariant(), val);
}

void GidWidget::onSaveMS()
{
    QVariant val;
    if (setSomething(false, TIP_UT, "magistralSite", QVariant(), val)) {
        QVariant null;
        qDebug() << "null " << null.isNull(); // true
        setValue(TIP_UT, "distSite", null);

        main_window->m_ms->init(this, m_cxema.m_db, true);
        main_window->m_rs->init(this, m_cxema.m_db, false);

        CGraph2 *graph = m_cxema.graph();

        auto p = graph->map_node.begin();
        while (p != graph->map_node.end()) {
            CNode2 *pp = p->second;

            if (pp->node.internalNodeID == 0) {
                for (CLINE2 *l = pp->lines; l; l = nextl(l)) {
                    CLine2 *ll = bline(l);
                    if (IsBegin(l) && bline(l)->line.isPjezo) {
                        ll->line.ms = 0;
                        ll->line.ms = val.toInt();
                    }
                }
            }
            p++;
        }
    }
}

void GidWidget::onSaveRS()
{
    QVariant val;
    if (setSomething(false, TIP_UT, "distSite", QVariant(), val)) {
        QVariant null;
        qDebug() << "null " << null.isNull(); // true
        setValue(TIP_UT, "magistralSite", null);

        main_window->m_ms->init(this, m_cxema.m_db, true);
        main_window->m_rs->init(this, m_cxema.m_db, false);

        CGraph2 *graph = m_cxema.graph();

        auto p = graph->map_node.begin();
        while (p != graph->map_node.end()) {
            CNode2 *pp = p->second;

            if (pp->node.internalNodeID == 0) {
                for (CLINE2 *l = pp->lines; l; l = nextl(l)) {
                    CLine2 *ll = bline(l);
                    if (IsBegin(l) && bline(l)->line.isPjezo) {
                        ll->line.rs = 0;
                        ll->line.rs = val.toInt();
                    }
                }
            }
            p++;
        }
    }
}

void GidWidget::onMsPassport()
{
}


void GidWidget::onTuNew()
{
    QString tn = transl("Здания ТУ");
    Klassif *kls = m_kl_list.findKlN(tn);

    if (kls) {
        addGeo(kls);
    }
    else {
        QMessageBox::warning(this, tr(""), QString(tr("В системе нет объекта %1").arg(tn)));
    }
}


void GidWidget::onAktCreate()
{
    QString tn = "act";
    Klassif *kls = m_kl_list.findKlN(tn);

    if (kls) {
        addGeo(kls);
    }
    else {
        QMessageBox::warning(this, tr(""), QString(tr("В системе нет объекта %1").arg(tn)));
    }
}

void GidWidget::onPtsZhurnal()
{
}

void GidWidget::onSelect6()   // "Выделить",
{
    QMenu *menu1 = new QMenu( this);
    QMenu *menu2;
    QMenu *menu3;
    QMenu *menu4;

    menu1->setToolTipsVisible(true);
    menu1->addAction(gidrAction.aRasprMag); // Расчетные схемы
    menu1->addAction(gidrAction.aRasprMag2); // Объекты системы теплоснабжения
    menu1->addAction(gidrAction.aIstSety); // Источник
    menu1->addAction(gidrAction.aIstSetyNew); // Источник Новый
    menu1->addAction(gidrAction.aVyd); // Фрагмент сети
    menu1->addAction(gidrAction.aBalans); // Балансовая принадлежность
    menu2 = menu1->addMenu(tr("По виду прокладки"));
        menu2->setToolTipsVisible(true);
        menu2->addAction(gidrAction.aTubing1); // Канальная
        menu2->addAction(gidrAction.aTubing2); // Бесканальная
        menu2->addAction(gidrAction.aTubing4); // Надземная
        menu2->addAction(gidrAction.aTubing3); // Подвальная
        menu2->addAction(gidrAction.aTubing5); // Обвязка узлов и насосных станций
//    menu1->addAction(gidrAction.aFindKti); // По коэффициенту тепловых испытаний




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
//                menu3->addAction(gidrAction.aSetTp); // Установить Тепловые пункты по потребителям
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
//                menu3->addSeparator();
//                menu3->addAction(gidrAction.aSetDate2); // Установить Дату первичного ввода в эксплуатацию
//                menu3->addAction(gidrAction.aSetDate1); // Установить Дату последней перекладки
//                menu3->addAction(gidrAction.aSetDate3); // Установить Дату планируемого ремонта
            menu2->addAction(gidrAction.aSetKodRs); // Установить Код расчетной схемы
//            menu2->addAction(gidrAction.aGeodz); // Установить Отметки узлов горизонтали
//            menu2->addAction(gidrAction.aSetKorrozia); // Установить Индикаторы коррозии
            menu2->addSeparator();
            menu2->addAction(gidrAction.aSetPodpOn); // Показать Надписи
            menu2->addAction(gidrAction.aSetPodpOff); // Не отображать Надписи


/*
    menu1->addSeparator();

    menu1->addAction(gidrAction.aZapVnCx); // Узлы с внутренней схемой
    menu1->addAction(gidrAction.aZapNullGeo); // Узлы с нулевой геодезической отметкой
    menu1->addAction(gidrAction.aOdnolin); // Однолинейные участки
    menu1->addAction(gidrAction.aRaznolin); // Участки с разными подачей и обраткой
*/

    menu1->exec( QCursor::pos() );

}

void GidWidget::onExport6()   // "Экспорт",
{
    QMenu *menu2 = new QMenu( this);

    menu2->setToolTipsVisible(true);
    menu2->addAction(gidrAction.aExportDxf); // Автокад DXF
//    menu2->addAction(gidrAction.aExportShape); // ESRI shape-файлы

    menu2->exec( QCursor::pos() );
}


void GidWidget::onRezhim6()   // "Режим",     );
{
    QMenu menu(this);

    menu.setToolTipsVisible(true);
    menu.addAction(gidrAction.aEditor1); // Режимы
//    menu.addAction(gidrAction.aEditor2); // Просмотр схемы
    menu.addAction(gidrAction.aEditor4); // АРМ

//    menu.setDefaultAction(gidrAction.aEditor1);

    QAction *a =  menu.exec( QCursor::pos() );

}

void GidWidget::onAnalVyd ()   // "Настройка",
{
    QMenu *menu2 = new QMenu( this);

    menu2->setToolTipsVisible(true);
    menu2->addAction(gidrAction.aColorLine); // Участки
    menu2->addAction(gidrAction.aColorLine1); // Потребители
    menu2->exec( QCursor::pos() );
}

void GidWidget::onFile6 ()   // "Файл",
{
    QMenu *menu1 = new QMenu( this);
    QMenu *menu2;

        menu1->setToolTipsVisible(true);

//        menu1->addAction(main_window->mainAction.aFileNew); // Подключить
//        menu1->addAction(main_window->mainAction.aFileSqlite); // Подключить

        menu1->addAction(gidrAction.aFileClose); // Закрыть
        menu1->addSeparator();
/*
        menu2 = menu1->addMenu(tr("Режим"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aEditor1); // Режимы
            menu2->addAction(gidrAction.aEditor2); // Просмотр схемы
            menu2->addAction(gidrAction.aEditor4); // АРМ
        menu1->addAction(gidrAction.aSqlSave); // Создание копии Базы данных
*/
        menu2 = menu1->addMenu(tr("Работа с фрагментами"));
//            menu2->addAction(gidrAction.aSqlCopyFragment); // Вставить Фрагмент схемы
            menu2->addAction(gidrAction.aSqlDelFragment); // Удалить Фрагмент схемы
            menu2->addAction(gidrAction.aAddFragment); // Добавить Фрагмент схемы
            menu2->addAction(gidrAction.aExportFragment); // Экспорт фрагмента
            menu2->addAction(gidrAction.aExportFragmentVyd); // Экспорт выделенного фрагмента
//            menu2->addAction(gidrAction.aExportFragmentsMulty); // Экспорт всех фрагментов
            menu2->addAction(gidrAction.aImportFragment); // Импорт фрагмента

//        menu1->addAction(gidrAction.aImportFragmentMdb); // Конвертор ТГИД-05/ ТГИД-07
        menu1->addSeparator();
/*
        menu1->addAction(gidrAction.aMainCxema); // Активная схема
        menu1->addAction(gidrAction.aAddDop); // Отобразить дополнительную схему
        menu1->addSeparator();
        menu1->addAction(gidrAction.aBmp); // Файл графической подложки...
        menu1->addAction(gidrAction.aBmpFind); // Найти графическую подложку
        menu1->addSeparator();
*/
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

        menu1->addAction(main_window->mainAction.aAppAbout); // О программе
        menu1->addSeparator();

        menu1->addAction(main_window->mainAction.aAppExit); // Выход


    menu1->exec( QCursor::pos() );

}

void GidWidget::onSetup6 ()   // "Настройка",
{
    QMenu *menu2 = new QMenu( this);
    QMenu *menu3;

    menu2->setToolTipsVisible(true);

    menu2->addAction(gidrAction.aLeftBar); // Проводник карты
    menu2->addAction(gidrAction.aMagAlma); // Проводник структуры сети
    menu2->addAction(gidrAction.aBottomRemont); // Проводник мониторинга
//    menu2->addAction(gidrAction.aProtocol); // Протокол
    menu2->addSeparator();

    menu2->addAction(gidrAction.aColors); // Стиль линии
    menu2->addAction(gidrAction.aFon); // Цвет фона
    menu3 = menu2->addMenu(tr("Шрифты"));
        menu3->setToolTipsVisible(true);
//        menu3->addAction(gidrAction.aFont2); // Шрифт текста
        menu3->addAction(gidrAction.aFont); // Шрифт надписей
//        menu3->addAction(gidrAction.aFontMag); // Шрифт названия магистрали
        menu3->addAction(gidrAction.aFontPanel); // Шрифт в проводнике
//    menu2->addAction(gidrAction.aTransp); // Прозрачность
    menu2->addSeparator();
    menu2->addAction(gidrAction.aAstr); // Сетка
    menu2->addAction(gidrAction.aRamka); // Рамка схемы
    menu2->addSeparator();
    menu2->addAction(gidrAction.aIsMas); // Сохранять пропорции
    menu2->addAction(gidrAction.aMasPic); // Номинальный масштаб
    menu2->addAction(gidrAction.aMasPodpis); // Масштаб откл.надписей

#if USE_GID9
    menu2->addSeparator();
    menu2->addAction(gidrAction.aSetupGid9); //
#endif


/*
    menu2->addAction(gidrAction.aTrio); // Передвинуть
    menu2->addAction(gidrAction.aIndexes); // Обновление индексов
    menu2->addAction(gidrAction.aHttp2); // Веб-сервер
    menu3 = menu2->addMenu(tr("Дополнительная обработка"));
        menu3->setToolTipsVisible(true);
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
    menu2->addAction(gidrAction.aPsAstanaName); // Отображение имен узлов
    menu2->addAction(gidrAction.aColorOnlyPts); // Цвета только для участков ПТС
*/
    menu2->exec( QCursor::pos() );
}

void GidWidget::onFind6 ()   // "Настройка",
{
    QMenu *menu1 = new QMenu( this);

    menu1->setToolTipsVisible(true);

    menu1->addAction(gidrAction.aZapVnCx); // Узлы с внутренней схемой
    menu1->addAction(gidrAction.aZapNullGeo); // Узлы с нулевой геодезической отметкой
    menu1->addAction(gidrAction.aOdnolin); // Однолинейные участки
    menu1->addAction(gidrAction.aRaznolin); // Участки с разными подачей и обраткой

    menu1->exec( QCursor::pos() );
}

void GidWidget::onGeobaza6 ()   // "Настройка",
{
    QMenu *menu1 = new QMenu( this);
    QMenu *menu2;
    QMenu *menu3;

    menu1->setToolTipsVisible(true);
    menu1->addAction(gidrAction.aGeobaza); // Показать геобазу
//    menu1->addAction(gidrAction.aFindGeo); // Поиск в геобазе...
//    menu1->addAction(gidrAction.aGeoFindNext); // Продолжение поиска
//    menu1->addAction(gidrAction.aFindGeoAddr); // Поиск по адресу
    menu1->addAction(gidrAction.aGeoInfo); // Информация из геобазы
    menu1->addSeparator();
    menu1->addAction(gidrAction.aSetLength); // Установить длины
//    menu1->addSeparator();
//    menu2 = menu1->addMenu(tr("Переименование полей"));
//        menu2->setToolTipsVisible(true);
//        menu2->addAction(gidrAction.a1000); // Создать файл полей
//        menu2->addAction(gidrAction.a1001); // Переименовать поля
//        menu2->addAction(gidrAction.a1002); // Сохранить переименованные поля



    menu2 = menu1->addMenu(tr("Интернет карты"));
        menu2->setToolTipsVisible(true);
        menu2->addAction(gidrAction.aNomap); // Нет карты
        menu3 = menu2->addMenu(tr("Google"));
            menu3->setToolTipsVisible(true);
            menu3->addAction(gidrAction.aGoogleMap); // Карта
            menu3->addAction(gidrAction.aGoogleSat); // Спутник
            menu3->addAction(gidrAction.aGoogleHybrid); // Гибридная
//        menu2->addAction(gidrAction.aVisicomMap); // Карта VISICOM
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
        menu2->addAction(gidrAction.aGoogleElevation); // Высота по Google
        menu2->addAction(gidrAction.aGoogleElevationSet); // Высота по Google
        menu2->addAction(gidrAction.aMapSearch); // Поиск
//        menu2->addAction(gidrAction.aMapPath); // Каталог
        menu2->addAction(gidrAction.aMapYes); // Восстанавливать подключение

    menu1->exec( QCursor::pos() );
}



void GidWidget::onZaprosy() // Запросы
{
    QMenu *menu2 = new QMenu( this);

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

    menu2->exec( QCursor::pos() );
}


void RunProgram(const QString & progr, bool message)
{
    if (QFile::exists(progr)) {
       QString path = QString("file:///%1").arg(progr);
       qDebug() << path;
       QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    }
    else {
        if (message) {
            QMessageBox::warning(nullptr, "", QString("Файл %1 не существует").arg(progr));
        }
    }
}


void GidWidget::onRunProgram() //
{
    QString fn = "C:/Program Files/ITwin/opt.exe";
    RunProgram(fn, true);
//    QString s = readFileUTF8("program/program.txt");
//    RunProgram("C:/Program Files/DBeaver/dbeaver.exe");
//    QMessageBox::warning(this, "", "!");
}

void GidWidget::onZaprosy2() // Запросы
{
    QMenu *menu1 = new QMenu( this);
    QMenu *menu2;

//    menu1 = menu->addMenu(tr("Анализ"));
        menu1->setToolTipsVisible(true);
        menu2 = menu1->addMenu(tr("Отключенные потребители"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aZap6); // Закрытые потребители
            menu2->addAction(gidrAction.aPotNagr0); // С нулевой нагрузкой
            menu2->addAction(gidrAction.aPotrOtkl); // Отключенные потребители
        menu2 = menu1->addMenu(tr("Отключенные участки"));
            menu2->setToolTipsVisible(true);
            menu2->addAction(gidrAction.aUtZakr); // Закрытые участки
            menu2->addAction(gidrAction.aUtZakrAll); // Отключенные участки
//        menu1->addAction(gidrAction.aZapOtr); // Отрицательные перепады
//        menu1->addAction(gidrAction.aPtTempMin); // Низкие температуры
//        menu1->addAction(gidrAction.aZavozd); // Завоздушивание

        menu1->addAction(gidrAction.aIsprLast); // Изменённые объекты
        menu1->addSeparator();
//        menu1->addAction(gidrAction.aPjezo); // Пьезометр
        menu1->addAction(gidrAction.aZona); // Гидростатические зоны



    menu1->exec( QCursor::pos() );
}



void GidWidget::onNagrAlmatyTable() // Объекты АЛСЕКО
{
    QString q = readQ("sql/alseco/alseco.sql");

    Klassif* kls = m_kl_list.findKlN("ZDANIYA_2");

    if (kls) {
        MultiHeaderTable* table = getMyTableView(*kls->m_db, "nagruzki", q, "");

        if (table) {
            table->setGidWidget(this);
    //        table->setGeo("");
            table->setAdd(true);
            table->setEdit(true);
            table->setDelete(true);

            view_my_table(table, "Объекты АЛСЕКО", this);
        }
    }
}
#if 0

void GidWidget::onNagrAlmatyAdd1() // Добавить Объект (МЖД) АЛСЕКО
{
    QMessageBox::information(this, "", "2");
}

void GidWidget::onNagrAlmatyAdd2() // Добавить Объект (Прочие) АЛСЕКО
{
    QMessageBox::information(this, "", "3");
}

#endif

#include <alseco/alsecodlg.h>

void GidWidget::onNagrAlmaty() // Объекты АЛСЕКО по адресам
{
//    QMessageBox::information(this, "", "4");

    Klassif* kls = m_kl_list.findKlN("ZDANIYA_2");

    if (kls) {
        BigDialog dlg(*kls->m_db, -1, this);
        dlg.exec();
    }
}

void GidWidget::onNagrAlmatyOnOff() // Вывести нагрузки АЛСЕКО
{
//    QMessageBox::information(this, "", "5");

    QString tn = "ZDANIYA_2";

    Klassif *kls = m_kl_list.findKlN(tn);
    if (kls) {
        if (kls->codlabel == "txt") {
            kls->codlabel = "number_1";
            kls->lf.lfItalic = FALSE;
            kls->label = 1;
        }
        else {
            kls->codlabel = "txt";
            kls->lf.lfItalic = TRUE;
            kls->label = 1;
        }


        QString klfn = QString("%1/%2.kls2").arg(argpath_2(), kls->baza);
        m_kl_list.save2(klfn, kls->baza);

        kls->open = false;

        void reset_geo(QSqlDatabase &db, QWidget *w);


        reset_geo(*kls->m_db, this);

        repaint();
    }
}

void GidWidget::onNagrAlmaXlsx() // Реестр объектов АЛСЕКО
{
    QSettings settings;
    QString alseco_xls = settings.value("alseco_xls", "").toString();

    // Проверяем, существует ли файл
    if (alseco_xls.isEmpty() || !QFileInfo::exists(alseco_xls)) {
        QString fileName = QFileDialog::getOpenFileName(
            nullptr,
            QObject::tr("Выберите файл Excel"),
            QString(),
            QObject::tr("Файлы Excel (*.xlsx)"));

        if (!fileName.isEmpty()) {
            alseco_xls = fileName;
            // Сохраняем путь в настройки
            settings.setValue("alseco_xls", alseco_xls);
        }
    }

    // Если файл существует — открываем Excel
    if (!alseco_xls.isEmpty() && QFileInfo::exists(alseco_xls)) {
        // Можно использовать QProcess::startDetached
        QProcess::startDetached("excel", QStringList() << alseco_xls);
    }
}

void GidWidget::onNagrAlmaNenaid1() // Реестр ненайденных объектов (МЖД) АЛСЕКО
{
    QString q = readQ("sql/alseco/nenaid1.sql");

    Klassif* kls = m_kl_list.findKlN("ZDANIYA_2");

    if (kls) {

        MultiHeaderTable* table = getMyTableView(*kls->m_db, "nagruzki", q, "");

        if (table) {
            table->setGidWidget(this);
            view_my_table(table, "Ненайденные объекты МЖД", this);
        }
    }
}

void GidWidget::onNagrAlmaNenaid2() // Реестр ненайденных объектов (Прочие) АЛСЕКО
{
    QString q = readQ("sql/alseco/nenaid2.sql");

    Klassif* kls = m_kl_list.findKlN("ZDANIYA_2");

    if (kls) {
        MultiHeaderTable* table = getMyTableView(*kls->m_db, "nagruzki", q, "");

        if (table) {
            table->setGidWidget(this);
            view_my_table(table, "Ненайденные объекты прочие", this);
        }
    }
}

void GidWidget::onNagrAlmaNenaid3() // Здания АЛСЕКО без потребителя
{
    QString q = readQ("sql/alseco/nenaid3.sql");

    QString tn = "zdaniya_2";
    Klassif* kls = m_kl_list.findKlN("ZDANIYA_2");

    if (kls) {
        MultiHeaderTable* table = getMyTableView(*kls->m_db, tn, q, "");

        if (table) {
            table->setGidWidget(this);
            table->setGeo(tn);
            view_my_table(table, "Здания АЛСЕКО без потребителя", this);
        }
    }
}





#include "gidrSlot.moc"



