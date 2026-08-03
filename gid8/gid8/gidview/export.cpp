#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include <QRegularExpression>
#include "GidWidget.h"
#include "mainwindow.h"

#include <db/db.h>

#if USE_ITWIN
#define TGID_FILE "itwin файлы (*.itwin)"
#else
#define TGID_FILE "tgid файлы (*.tgid)"
#endif


// Открытый фрагментов, если нескольео то -1

int nFragments(QSqlDatabase & db, CCxema *cxema, QString &fragment)
{
#if 0
  int n_open = 0;
  int fragmentID = -1;

  if (ado->openTable0("SELECT id, name  FROM fragments ORDER BY name")) {
    while (!ado->isEOF()) {
      long id = ado->read_long("id");
      QString s = ado->readStr("name");

      map<long, CGidFile *>::iterator it = cxema->m_file_map.find(id);

      if (it != cxema->m_file_map.end()) {
          n_open ++;
          fragmentID = id;
          fragment = s;
      }
      ado->MoveNext();
    }
    ado->closeTable();
  }

  if (n_open == 1)  return fragmentID;
  return -1;

#endif
    return 1;
}

bool export_tgid(QSqlDatabase & db, const QString & fn, int fileID, CGraph2 *graph);

#include <lib/DuckX/thirdparty/zip/zip.h>

void GidWidget::onExportFragmentVyd() // Экспорт выделенного фрагмента
{
//    QString fragment;
//    long fileID = nFragments(m_cxema.m_db, &m_cxema, fragment);

    int fileID = m_fileID;

    if (fileID <= 0) {
        QMessageBox::warning(nullptr, "", "Для экспорта должен быть выбран фрагмент");
        return;
    }

    QString fn = QFileDialog::getSaveFileName(this, tr(""), "", TGID_FILE);

    if (!fn.isEmpty()) {
        QString tmpn;
        tmpn = QString("%1/tempfileforexport.tmp").arg(QDir::tempPath());
        tmpn = QString("%1/tgid.txt").arg(QDir::tempPath());

        if ( export_tgid(m_cxema.m_db, tmpn, fileID, m_cxema.m_graph)) {
            QFile file(tmpn);
            if (!file.open(QIODevice::ReadOnly))
                return;

            const char *filenames[1];
            filenames[0] = tmpn.toLocal8Bit();

            zip_create(fn.toLocal8Bit(), filenames, 1);

            QMessageBox::warning(nullptr, "", "Выполнено");
        }
    }
}


void GidWidget::onExportFragmentsMulty() // Экспорт всех фрагментов
{
    if (m_cxema.map_fragments.empty()) {
        QMessageBox::information(this, "", tr("Нет открытых фрагментов для экспорта"));
        return;
    }

    QString dir = QFileDialog::getExistingDirectory(this, tr("Каталог для экспорта фрагментов"));
    if (dir.isEmpty()) return;

    int ok = 0, fail = 0;
    for (auto &it : m_cxema.map_fragments) {
        int fileID = it.first;
        QString name = it.second.m_name;
        name.replace(QRegularExpression("[\\\\/:*?\"<>|]"), "_");
        if (name.isEmpty()) name = QString("fragment_%1").arg(fileID);

        QString out = QString("%1/%2.tgid").arg(dir, name);
        QString tmpn = QString("%1/tgid.txt").arg(QDir::tempPath());
        if (export_tgid(m_cxema.m_db, tmpn, fileID, nullptr)) {
            QByteArray tmp_ba = tmpn.toLocal8Bit();
            QByteArray out_ba = out.toLocal8Bit();
            const char *filenames[1] = { tmp_ba.constData() };
            if (zip_create(out_ba.constData(), filenames, 1) == 0) ok++;
            else fail++;
        } else {
            fail++;
        }
    }

    QMessageBox::information(this, "", tr("Экспорт завершён: успешно %1, ошибок %2").arg(ok).arg(fail));
}

void GidWidget::onExportFragment() // Экспорт фрагмента
{
//    QString fragment;
//    long fileID = nFragments(m_cxema.m_db, &m_cxema, fragment);

    int fileID = m_fileID;

    if (fileID <= 0) {
        QMessageBox::warning(nullptr, "", "Для экспорта должен быть выбран фрагмент");
        return;
    }

    QString fn = QFileDialog::getSaveFileName(this, tr(""),
                               "",
                               tr(TGID_FILE));
    if (!fn.isEmpty()) {
        QString tmpn;
        tmpn = QString("%1/tempfileforexport.tmp").arg(QDir::tempPath());
        tmpn = QString("%1/tgid.txt").arg(QDir::tempPath());

        if ( export_tgid(m_cxema.m_db, tmpn, fileID, nullptr)) {
            QFile file(tmpn);
            if (!file.open(QIODevice::ReadOnly))
                return;

            const char *filenames[1];
            filenames[0] = tmpn.toLocal8Bit();

            zip_create(fn.toLocal8Bit(), filenames, 1);

            QMessageBox::warning(nullptr, "", "Выполнено");
        }
    }
}

std::set<int> getList(const QString & str);

QString get_fragment_name();
QString getPar(std::set<int>& v);


bool GidWidget::reopenFragments(const QString & par1, const QString & par2)
{
    endDraw();

    qDebug() << "reopenFragments " << par1 << " " << par2;
    
    std::set<int> set_1 = getList(par1);
    std::set<int> set_2 = getList(par2);
    std::set<int> set_3;    // set_3 = set_1 - set_2  то что убрали

    for (auto & i : set_1) {
        auto it = set_2.find(i);

        if (it == set_2.end()) {
            set_3.insert(i);
        }
    }

    std::set<int> set_4;    // set_4 = set_2 - set_1   то что добавили

    for (auto & i : set_2) {
        auto it = set_1.find(i);

        if (it == set_1.end()) {
            set_4.insert(i);
        }
    }

    if (set_3.size() > 0) {
        QString qq = "";
        m_cxema.graph()->remove_fragments(set_3);

        for (auto & i : set_3) {
            m_cxema.map_fragments.erase(i);
            qq += QString("%1\n").arg(i);
        }
//        QMessageBox::warning(this, "", qq);
    }

    if (set_4.size() > 0) {
        QProgressDialog progress("Операция выполняется, подождите пожалуйста...", "Прервать", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();

        QString par4 = getPar(set_4);
        if (m_cxema.open_cxema(this, par4, &progress)) {
        }
    }

    m_cxema.m_par = par2;

    QSettings settings;
    settings.setValue(m_fileName, par2);
    
    main_window->m_left_gid->init(this, &m_cxema);

    setFileID(-1, false);


    qDebug() << "reopenFragments закончено";

    return true;
}


bool GidWidget::closeFragment(int fileID)
{
    std::set<int> set_1 = getList(m_cxema.m_par);
    if (set_1.find(fileID) == set_1.end()) return true;
    set_1.erase(fileID);
    QString par = getPar(set_1);

    qDebug() << "closeFragment begin";

    bool ret = reopenFragments(m_cxema.m_par, par);

    qDebug() << "closeFragment end";

    return ret;

}

bool GidWidget::openFragment(int fileID)
{
    std::set<int> set_1 = getList(m_cxema.m_par);
    if (set_1.find(fileID) != set_1.end()) return true;
    set_1.insert(fileID);
    QString par = getPar(set_1);

    if (par == "") return true;

    qDebug() << "openFragment begin";

    bool ret = reopenFragments(m_cxema.m_par, par);

    qDebug() << "openFragment end";

    return ret;

}

int import_tgid(QSqlDatabase & db, const QString & fn, const QString & tgid_name);

#if 0

int GidWidget::import_tgid0( QString fn)
{
    int fileID = import_tgid(m_cxema.m_db, fn);
    if (fileID <= 0) {
        QMessageBox::warning(this, "", "Ошибка! Не могу записать данные в БД\n");
        return -1;
    }

    QSqlQuery query(m_cxema.m_db);

    QString q;
    q = QString("UPDATE  l SET shape = NULL FROM net.v_linesobj l JOIN net.v_nodes n1 ON n1.id=l.nodeID1 WHERE n1.fileID=%1").arg(fileID);
    query_exec(m_cxema.m_db, query, q);

    q = QString("UPDATE  l\n"
             "SET\n"
             "l.shape = geometry::STGeomFromText(CONCAT('LINESTRING(', n1.x/100, ' ',  -n1.y/100, ',',  dbo.str_coords(REPLACE(l.coords, ',', ' ')),n2.x/100, ' ', -n2.y/100, ')'), 0).MakeValid()\n"

             "FROM net.v_linesobj l\n"
             "JOIN net.v_nodes n1 ON n1.id=l.nodeID1\n"
             "JOIN net.v_nodes n2 ON n2.id=l.nodeID2\n"
             "WHERE n1.internalNodeID IS NULL \n"
             "  AND (n1.x <> n2.x OR n1.y <> n2.y) \n"
             "  AND (n1.x <> 0 OR n1.y <> 0) \n"
             "  AND (n2.x <> 0 OR n2.y <> 0) \n"
             "  AND n1.fileID=%1\n"
             "  AND l.removed=0\n").arg(fileID);

    query_exec(m_cxema.m_db, query, q);

    openFragment(fileID);

    return 0;
}

#endif


#include <lib/json.h>

void GidWidget::onFinishedImport(int exitCode, QProcess::ExitStatus exitStatus )
{
    QString outName = QString("%1/out.txt").arg(QDir::tempPath());

    QFile file(outName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString q = file.readAll().trimmed();


    QString par = m_cxema.m_par;

    par += QString(",%1").arg(q);

    qDebug() << "onFinishedImport begin";

    reopenFragments(m_cxema.m_par, par);

    qDebug() << "onFinishedImport end";

}

void GidWidget::onImportFragment() // Импорт фрагмента
{
    QString fileName = QFileDialog::getOpenFileName(this, tr(""), "", 
                               tr(TGID_FILE));

    if (!fileName.isEmpty()) {

        QString outName = QString("%1/out.txt").arg(QDir::tempPath());

        QFile::remove(outName);

        QString args = QString(" \"%1\" -out_file \"%2\"").arg(fileName, outName);
        QString str = str_python("unite/import_tgid.py", args);

//        QInputDialog::getMultiLineText(nullptr, "", "label", str);

        main_window->start(str, this, SLOT(onFinishedImport(int, QProcess::ExitStatus)));


/*
        QString dir = QDir::tempPath();
        if (zip_extract(fileName.toLocal8Bit(), dir.toLocal8Bit(), 0, 0) == 0) {
//            import_tgid0(dir+"/tgid.txt");
            int fileID = import_tgid(m_cxema.m_db, dir+"/tgid.txt", fileName);
        }
*/
    }
}

void json_add(QString &str, const QString & key, const QString & val)
{
    str += QString("\"%1\":\"%2\"").arg(key, val);
}

void json_add(QString &str, const QString & key, int val)
{
    str += QString("\"%1\":%2").arg(key, val);
}

void json_add(QString &str, const QString & key)
{
//    str += QString("\"%1\":%2").arg(key, val);
}


QString python_exe()
{
    QString p = "python";
    QString pp = QString("%1/python-3.12.7-embed-amd64/python.exe").arg(argpath_local());

#if !_WIN32

    pp = "/app/bin/python3.12";

    if (QFile::exists(pp)) {
        return pp;
    }

    pp = "/opt/ItWin/venv/bin/python";

    if (QFile::exists(pp)) {
        return pp;
    }
#else
    // embed-пакет рядом с данными
    if (QFile::exists(pp)) {
        return QString("\"%1\"").arg(pp);
    }

    // venv движка sety: рядом с exe и известный каталог разработчика
    const QString dir = QCoreApplication::applicationDirPath();
    const QStringList venvs = {
        dir + "/../venv/sety/Scripts/python.exe",
        "H:/venv/sety/Scripts/python.exe",
    };
    for (const QString &v : venvs) {
        if (QFile::exists(v)) {
            return QString("\"%1\"").arg(v);
        }
    }
#endif

    return p;
}


void GidWidget::onImportFragmentMdb() // Конвертор ТГИД-05/ ТГИД-07
{
    if (!isEditGid()) {
//        AfxMessageBox(_TR("Редактирование запрещено!"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    QStringList lst = QFileDialog::getOpenFileNames(this, tr(""), "", 
                               tr("*.mdb"));

    QString path_temp = QDir::tempPath();

    for (QString filename : lst) {
        QString fn = QString("%1/import_mdb/config.json").arg(path_temp);
        mkdir_for_file(fn);


//        void write_system_coord(JSON &json);

//        FILE* f = fopen(fn, "w");

        JSON json;

        json.add("{");
        json.add("\"mssql\": {");
        json.add("host", m_cxema.m_connect.host);
        json.add("user", m_cxema.m_connect.user);
        json.add("password", m_cxema.m_connect.password);
        json.add("port", m_cxema.m_connect.port);
        json.add("db", m_cxema.m_connect.baza);

        json.add("}");

        json.add("\"coords\": {");
//            write_system_coord(json);
        json.add("}");

        json.add("\"mdbs\": [");

        json.add1(filename);

        json.add("]");

        json.add("}");
//            fputs(json.str(), f);
//            fclose(f);

        QFile file(fn);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(nullptr, "", "Не могу открыть файл для записи");
            return;
        }

        QTextStream out(&file);

        out << json.str();

        file.close();

        QString path = QString("%1 \"%2import_mdb/import_mdb.py\"").arg(python_exe(), argpath());

        main_window->start(path, this, SLOT(onFinished(int, QProcess::ExitStatus)));

//        QProcess *process = new QProcess(this);
//        process->startCommand(path);
    }

/*
            if (ret < 0) {
                if (errno == ENOENT) {
                    QMessageBox::warning(this, "", "Для конвертирования mdb-файлов должен быть установлен python");
                }
                else {
                    QMessageBox::warning(this, "", "Ошибка запуска программы для конвертирования mdb-файлов");
                }
            }
            else {
                    QMessageBox::warning(this, "", "Конвертирование закончено");
//                    m_cxema->openFragments(m_cxema->m_ado);
//                    reopenFragments();
            }
        }
    }
*/
}
