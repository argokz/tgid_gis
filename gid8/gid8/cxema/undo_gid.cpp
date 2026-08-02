#include <QCoreApplication>
#include <QDateTime>

#include "cxema/cxema1.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"
#include "undo.h"
#include <QMessageBox>
#include <QRegularExpression>

#include "cxema/coordlis.h"


#include <db/db.h>


struct ChangedObject {
    long id;
    long changeTypeID;
    long changedID;
    long old_id;
    QString text;
    QDateTime dt;
};

static std::map<int, ChangedObject> map_lst;
static std::list<ChangedObject> lst;


QString getNameChT(int id)
{
  switch(id) {
  case CH_T_DELETE_LINE : return QObject::tr("Удален участок");
  case CH_T_DELETE_NODE : return QObject::tr("Удален узел");
  case CH_T_DELETE_CXEMA_BEGIN : return "";
  case CH_T_DELETE_CXEMA_END : return QObject::tr("Удалена схема");
  case CH_T_MOVE_NODE : return QObject::tr("Передвинут узел");
  case CH_T_MOVE_LINE : return QObject::tr("Передвинут участок");
  //case CH_T_MOVE_LINE_GEO : return QObject::tr("Передвинут участок");
  }
  return QObject::tr("Ошибка");
}

QString getRemovedNodeName(QSqlDatabase &db, int id)
{
    QString ret = "";
    QString q = QString("SELECT n.id, ec.name, n.externalNodeName FROM net.v_nodes n JOIN externalCodes ec ON ec.id=n.externalCodeID WHERE n.idRemoved=%1").arg(id);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        while (query.next()) {
    //        int id = query.value(0).toInt();
            QString kod = query.value(1).toString();
            QString name = query.value(2).toString();
            ret = kod + " " + name;
        }
    }

    return ret;
}


QString getMovedNodeName(QSqlDatabase &db, int id)
{
    QString ret = "";
    QString q = QString("SELECT n.id, ec.name, n.externalNodeName FROM net.v_nodes n JOIN externalCodes ec ON ec.id=n.externalCodeID WHERE n.id=%1").arg(id);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {

        while (query.next()) {
            //        int id = query.value(0).toInt();
            QString kod = query.value(1).toString();
            QString name = query.value(2).toString();
            ret = kod + " " + name;
        }
    }

    return ret;
}

QString getRemovedLineName(QSqlDatabase &db, int id)
{
    QString ret = "";
    QString q = QString("SELECT l.id, CONCAT(ec1.name, ' ', n1.externalNodeName, ' - ', ec2.name, ' ', n2.externalNodeName) FROM net.v_linesobj l JOIN net.v_nodes n1 ON n1.id=l.nodeID1 JOIN net.v_nodes n2 ON n2.id=l.nodeID2 JOIN externalCodes ec1 ON ec1.id=n1.externalCodeID JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID WHERE l.idRemoved=%1").arg(id);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    while (query.next()) {
//            long id = ado->read_long(0);
        ret = query.value(1).toString();
    }
    return ret;
}

bool reset_shape_node(QSqlDatabase & db, int id);
bool reset_shape_line(QSqlDatabase & db, int id);

void ErrorMessage(QSqlQuery &query);


bool undo_move_node(QSqlDatabase &db, ChangedObject &co, int &fileID)
{
//    long affected;
    QString q;
    double x, y;

    static QRegularExpression re("^(-?[0-9\\.eE\\+\\-]+)\\s+(-?[0-9\\.eE\\+\\-]+)\\s*$");
    QRegularExpressionMatch match = re.match(co.text);
    if (match.hasMatch()) {
        x = match.captured(1).toFloat();
        y = match.captured(2).toFloat();
    }
    else {
        QMessageBox::warning(nullptr, "", "Ошибка");
        return false;
    }

    q = QString("SELECT id, fileID FROM net.v_nodes WHERE id=%1").arg(co.changedID);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            fileID = query.value("fileID").toInt();
        }
    }

    q = QString("UPDATE nodes SET x=%1, y=%2 WHERE id=%3").arg(x).arg(y).arg(co.changedID);

    bool ret = query_exec(db, query, q);
    if (ret) {
        reset_shape_node(db, co.changedID);

        q = QString("DELETE FROM changed_object WHERE id=%1").arg(co.id);
        ret = query_exec(db, query, q);
    }

    if (!ret) {
        bool ret = query_exec(db, query, q);
        ErrorMessage(query);
        return false;
    }

    return true;
}


bool undo_move_line(QSqlDatabase &db, ChangedObject &co, int &fileID)
{
    long id = -1;
    QString q;

    q = QString("SELECT n1.fileID FROM net.v_linesobj l "
        " JOIN net.v_nodes n1 ON l.nodeID1=n1.id "
        " WHERE l.id=%1").arg(co.changedID);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            fileID = query.value("fileID").toInt();
        }
    }

    q = QString("UPDATE linesobj SET coords='%1' WHERE id=%2").arg(co.text).arg(co.changedID);

    bool ret = query_exec(db, query, q);

    if (ret) {
        reset_shape_line(db, co.changedID);
        q = QString("DELETE FROM changed_object WHERE id=%1").arg(co.id);
        bool ret = query_exec(db, query, q);
    }

    if (!ret) {
        ErrorMessage(query);
        return false;
    }

    return true;
}

bool undo_move_line_geo(QSqlDatabase &db, ChangedObject &co, int &fileID)
{
    long id = -1;
    QString q;

    q = QString("SELECT n1.fileID FROM net.v_linesobj l "
        " JOIN net.v_nodes n1 ON l.nodeID1=n1.id "
        " WHERE l.id=%1").arg(co.changedID);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            fileID = query.value("fileID").toInt();
        }
    }

    QString text = co.text;
    
    CCoordList cl;
    
    cl.loadStr(text.toLatin1().data());

    CFPoint pt1 = cl.first();
    CFPoint pt2 = cl.last();
    cl.pop_front();
    cl.pop_back();

    text = cl.saveStr();

    q = QString("UPDATE linesobj SET coords='%1' WHERE id=%2").arg(co.text).arg(co.changedID);

    bool ret = query_exec(db, query, q);

    if (ret) {
        reset_shape_line(db, co.changedID);
        q = QString("DELETE FROM changed_object WHERE id=%1").arg(co.id);
        bool ret = query_exec(db, query, q);
    }

    if (!ret) {
        ErrorMessage(query);
        return false;
    }


    
    return true;
}


bool undo_node(QSqlDatabase &db, ChangedObject &co, int &fileID)
{
    long id = -1;
    QString q = QString("SELECT id, fileID FROM net.v_nodes WHERE idRemoved=%1").arg(co.id);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            id = query.value("id").toInt();
            fileID = query.value("fileID").toInt();
        }
    }
    if (id <= 0) {
        QMessageBox::warning(nullptr, "", "Ошибка");
        return false;
    }

    q = QString("UPDATE nodes SET removed=0, idRemoved=NULL WHERE idRemoved=%1").arg(co.id);

    bool ret = query_exec(db, query, q);
    if (ret) {
        q = QString("DELETE FROM changed_object WHERE id=%1").arg(co.id);
        ret = query_exec(db, query, q);
    }

    if (!ret) {
        ErrorMessage(query);
        return false;
    }

    return true;
}


bool undo_line(QSqlDatabase &db, ChangedObject &co, int &fileID)
{
    bool quit = false;
    QString q = QString("SELECT n1.fileID, CONCAT(ec1.name, ' ', n1.externalNodeName) AS name1, n1.removed AS n1r, "
                             "  CONCAT(ec2.name, ' ', n2.externalNodeName) AS name2, n2.removed AS n2r FROM net.v_linesobj l "
        " JOIN net.v_nodes n1 ON l.nodeID1=n1.id "
        " JOIN net.v_nodes n2 ON l.nodeID2=n2.id "
        " JOIN externalCodes ec1 on ec1.id = n1.externalCodeID "
        " JOIN externalCodes ec2 on ec2.id = n2.externalCodeID "
        " WHERE l.idRemoved=%1").arg(co.id);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            QString s1 = query.value("name1").toString();
            QString s2 = query.value("name2").toString();

            int l1 = query.value("nlr").toInt();
            int l2 = query.value("nlr").toInt();

            fileID = query.value("fileID").toInt();

            if (l1 || l2) {
                q = QString(QObject::tr("Отмена невозможна, нужно откатить удаление узла %1 ")).arg(l1 ? s1 : s2);
                QMessageBox::warning(nullptr, "", q);
                quit = true;
            }
        }
    }
    if (quit) return false;

    q = QString("UPDATE linesobj SET removed=0, idRemoved=NULL WHERE idRemoved=%1").arg(co.id);

    bool ret = query_exec(db, query, q);
    if (ret) {
        q = QString("DELETE FROM changed_object WHERE id=%1").arg(co.id);
        ret = query_exec(db, query, q);
    }

    if (!ret) {
        ErrorMessage(query);
        return false;
    }
    return true;
}

bool undo_block(QSqlDatabase &db, ChangedObject &co0, int &fileID)
{
    bool ret;
    QString q = QString(
                    "WITH next_24 AS (\n"
                    "  SELECT id AS end_id\n"
                    "  FROM changed_object\n"
                    "  WHERE changetypeid = 24 AND id > %1\n"
                    "  ORDER BY id\n"
                    "  LIMIT 1\n"
                    ")\n"
                    "SELECT id,changetypeid,old_id,changedid,changedtext,dateremoved,userremoved\n"
                    "FROM changed_object\n"
                    "WHERE id > %1\n"
                    "  AND id < (SELECT end_id FROM next_24)\n"
                    "ORDER BY id desc;\n").arg(co0.id);


    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        while (query.next()) {

            ChangedObject co;

            co.changeTypeID = query.value(1).toInt();
            co.dt = query.value(5).toDateTime();
            co.id = query.value(0).toInt();
            co.changedID = query.value(3).toInt();
            co.old_id = query.value(2).toInt();
            co.text = query.value(4).toString();

            bool ret;

            if (co.changeTypeID == CH_T_DELETE_NODE) {
                ret = undo_node(db, co, fileID);
            }
            else if (co.changeTypeID == CH_T_DELETE_LINE) {
                ret = undo_line(db, co, fileID);
            }
            else if (co.changeTypeID == CH_T_MOVE_NODE) {
                ret = undo_move_node(db, co, fileID);
            }
            else if (co.changeTypeID == CH_T_MOVE_LINE) {
                ret = undo_move_line(db, co, fileID);
            }
        }
    }


    q = QString(
            "WITH next_24 AS (\n"
            "  SELECT id AS end_id\n"
            "  FROM changed_object\n"
            "  WHERE changetypeid = 24 AND id > %1\n"
            "  ORDER BY id\n"
            "  LIMIT 1\n"
            ")\n"
            "DELETE FROM changed_object\n"
            "WHERE id >= %1\n"
            "  AND id <= (SELECT end_id FROM next_24);\n").arg(co0.id);

    ret = query_exec(db, q);

    return true;
}

#if 0

bool undo_gid_old(QSqlDatabase &db, int id, int &fileID)
{
/*
  std::map<int, ChangedObject>::const_iterator it = map_lst.find(id);
  if (it == map_lst.end()) {
    QMessageBox::warning(nullptr, "", "Ошибка");
    return false;
  }
*/

    QString q = QString("select id,changetypeid,old_id,changedid,changedtext,dateremoved,userremoved from changed_object where id=%1 and not changetypeid in (10, 11)").arg(id);

    std::vector<QVariant> vv;

    bool ret = readTableRowValues(db, q, vv);

    if (!ret) return false;

//    bool ret = false;

    QString s;
    ChangedObject co;

    co.changeTypeID = vv[1].toInt();
    co.dt = vv[5].toDateTime();
    co.id = vv[0].toInt();
    co.changedID = vv[3].toInt();
    co.old_id = vv[2].toInt();
    co.text = vv[4].toString();


    //  QString dts = co.dt = QString("%Y-%m-%d %H:%M:%S");

      QString dts = co.dt.toString("dd MM yyyy hh:mm:ss");

      if (co.changeTypeID == CH_T_DELETE_NODE) {
          s = QString(QObject::tr("Восстановить узел %1 удаленный %2?")).arg(getRemovedNodeName(db, co.id), dts);
          if (QMessageBox::question(0, "", s) == QMessageBox::Yes) {
              ret = undo_node(db, co, fileID);
          }
      }
      else if (co.changeTypeID == CH_T_DELETE_LINE) {
          s = QString(QObject::tr("Восстановить линию %1 удаленную %2?")).arg(getRemovedLineName(db, co.id), dts);
          if (QMessageBox::question(0, "", s) == QMessageBox::Yes) {
              ret = undo_line(db, co, fileID);
          }
      }
      else if (co.changeTypeID == CH_T_MOVE_NODE) {
          s = QString(QObject::tr("Вернуть узел %1 передвинутый %2 на место?")).arg(getMovedNodeName(db, co.id), dts);
          if (QMessageBox::question(0, "", s) == QMessageBox::Yes) {
              ret = undo_move_node(db, co, fileID);
          }
      }
      else if (co.changeTypeID == CH_T_MOVE_LINE) {
          s = QString(QObject::tr("Вернуть линию %1 передвинутую %2 на место?")).arg(getMovedNodeName(db, co.id), dts);
          if (QMessageBox::question(0, "", s) == QMessageBox::Yes) {
              ret = undo_move_line(db, co, fileID);
          }
      }

      else if (co.changeTypeID == CH_T_BEGIN) {
//          s = QString(QObject::tr("Вернуть линию %1 передвинутую %2 на место?")).arg(getMovedNodeName(db, co.id), dts);
          s = QString(QObject::tr("Вернуть линию передвинутую %1 на место?"))
//            .arg(getMovedNodeName(db, co.id), dts);
            .arg(dts);

          if (QMessageBox::question(0, "", s) == QMessageBox::Yes) {
              ret = undo_block(db, co, fileID);
          }
      }


/*
      else if (co.changeTypeID == CH_T_MOVE_LINE_GEO) {
          s = QString(QObject::tr("Вернуть линию %1 передвинутую %2 на место?")).arg(getMovedNodeName(db, co.id), dts);
          if (QMessageBox::question(0, "", s) == QMessageBox::Yes) {
              ret = undo_move_line_geo(db, co, fileID);
          }
      }
*/
      return ret;
}

#endif


#if 0
void CGidrView::Undo(bool only_del)
{
    QSqlDatabase &db = m_cxema->m_ado;
    if (!ado) return;

    QString q;

    map_lst.clear();
    lst.clear();

    QString qq = "";

    if (only_del) {
        qq = QString(" WHERE changeTypeID IN (1, 2) ");
    }


    q = QString("SELECT * FROM changed_object ORDER BY id DESC LIMIT 20");
    if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
        q = QString("SELECT TOP 2000 * FROM changed_object %s ORDER BY id DESC", qq);
    }


    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            ChangedObject co;
            co.id = ado->read_long("id");
            co.changeTypeID = ado->read_long("changeTypeID");
            co.changedID = ado->read_long("changedID");

            co.old_id = ado->read_long("old_id");
            co.dt = ado->read_date("dateRemoved");
            co.text = ado->readStr("changedText");
            lst.push_back(co);

            ado->MoveNext();
        }
        ado->closeTable();
    }

    if (lst.size() <= 0) {
      AfxMessageBox(QObject::tr("Нет допустимых отмен"), MB_OK|MB_ICONINFORMATION);
      return;
    }


    CMMenu2 menu(AfxGetMainWnd(), IDD_MENU_MULTI, QObject::tr("Отмена изменений"));

    menu.AddColumn(QObject::tr("Время"));
    menu.AddColumn(QObject::tr("Объект"));
    menu.AddColumn(QObject::tr("Тип"));

/*
    menu.setWidth(1, 150);
    menu.setWidth(2, 150);
    menu.setWidth(3, 200);
*/

double getScreenMas();

    menu.setWidth(1, 120*getScreenMas());
    menu.setWidth(2, 120*getScreenMas());
    menu.setWidth(3, 120*getScreenMas());

    for (list<ChangedObject>::const_iterator it = lst.begin(); it != lst.end(); it++) {
        ChangedObject co = *it;
        QString s;
        QString tn = "";

        if (it->changeTypeID == CH_T_DELETE_NODE) {
            tn = getRemovedNodeName(ado, it->id);
        }
        else if (it->changeTypeID == CH_T_DELETE_LINE) {
            tn = getRemovedLineName(ado, it->id);
        }
        else if (it->changeTypeID == CH_T_MOVE_NODE) {
            tn = getMovedNodeName(ado, it->id);
        }

        menu.Add(1, it->dt = QString("%Y-%m-%d %H:%M:%S"));

        if (tn == "") {
          menu.Add(2, getNameChT(it->changeTypeID));
          menu.Add(3, tn);
        }
        else {
          menu.Add(2, getNameChT(it->changeTypeID));
          menu.Add(3, tn);
        }
        menu.Add((void*)it->id);

        map_lst[co.id] = co;
    }


    if (menu.DoModal() == IDOK) {
      long id = (long) menu.getV();
      int fileID;
      if (undo_gid(ado, id, fileID)) {
          QString par;
          par = QString("%d", fileID);
          CWaitCursor wait;

          set<int>  v;
          v.insert(fileID);
          m_cxema->m_graph->clearFile(v);
          Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
          m_cxema->openGid(ado, par, pDoc->m_user_gid);
          Invalidate();
      }
    }

    InvalidateG();
}

#endif

#include <geo/Klassif.h>




#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>
#include <QDebug>

#if 0

// Начало группы изменений
bool change_group_start_old(QSqlDatabase &db) {
    // Генерируем UUID в Qt (альтернатива uuid_generate_v4, если расширение недоступно)
    QString session = QUuid::createUuid().toString(QUuid::WithoutBraces); // Формат: 123e4567-e89b-12d3-a456-426614174000

    // Используем параметризованный запрос для безопасности
    QSqlQuery query(db);
    
    if (!query.exec(QString("SET tgid.current_group_id = '%1'").arg(session))) {
        qDebug() << "Failed to set group_id:" << query.lastError().text();
        return false;
    }

    // Начинаем транзакцию (если требуется атомарность)
    if (!db.transaction()) {
        qDebug() << "Failed to start transaction:" << db.lastError().text();
        return false;
    }

    return true;
}

#endif

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>
#include <QRegularExpression>
#include <QDebug>

#if 0
// Предполагаемая реализация execQ (для примера)
bool execQ(QSqlDatabase &db, const QString &query, bool transactional = false) {
    QSqlQuery q(db);
    if (transactional && !db.transaction()) {
        qDebug() << "Failed to start transaction:" << db.lastError().text();
        return false;
    }
    if (!q.exec(query)) {
        qDebug() << "Query failed:" << query << "\nError:" << q.lastError().text();
        if (transactional) db.rollback();
        return false;
    }
    if (transactional && !db.commit()) {
        qDebug() << "Failed to commit transaction:" << db.lastError().text();
        return false;
    }
    return true;
}

#endif


static int m_group_id = 0;

// Начало группы изменений с комментарием
QString change_group_start(QSqlDatabase &db, const QString &comment) {

    m_group_id += 1;

    if (m_group_id > 1) {
        return 0;
    }

    // Генерируем UUID в Qt
    QString session = QUuid::createUuid().toString(QUuid::WithoutBraces); // Формат: 123e4567-e89b-12d3-a456-426614174000

    // Проверяем, что UUID валиден
    QRegularExpression uuidRegex("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$");
    if (!uuidRegex.match(session).hasMatch()) {
        qDebug() << "Invalid UUID generated:" << session;
        return QString();
    }

    // Проверяем, что комментарий не пустой (или устанавливаем значение по умолчанию)
    QString safeComment = comment.isEmpty() ? "No comment provided" : comment;

    // Начинаем транзакцию
    if (!db.transaction()) {
        qDebug() << "Failed to start transaction:" << db.lastError().text();
        return QString();
    }

    // Устанавливаем tgid.current_group_id
    QString setQuery = QString("SET tgid.current_group_id = '%1'").arg(session);
    QSqlQuery query(db);
    if (!query.exec(setQuery)) {
        qDebug() << "Failed to set group_id:" << query.lastError().text();
        db.rollback();
        return QString();
    }

    // Сохраняем комментарий в audit_group_comments
    query.prepare("INSERT INTO audit_group_comments (change_group_id, comment) VALUES (:group_id, :comment)");
    query.bindValue(":group_id", session);
    query.bindValue(":comment", safeComment);
    if (!query.exec()) {
        qDebug() << "Failed to save comment:" << query.lastError().text();
        db.rollback();
        return QString();
    }

    return session; // Возвращаем UUID для последующего использования
}


// Завершение группы изменений
bool change_group_end(QSqlDatabase &db) {

    m_group_id -= 1;

    if (m_group_id > 0) {
        return true;
    }


    // Завершаем транзакцию
    if (!db.commit()) {
        qDebug() << "Failed to commit transaction:" << db.lastError().text();
        db.rollback();
        return false;
    }

    // Сбрасываем переменную с помощью RESET
    QSqlQuery query(db);
    if (!query.exec("RESET tgid.current_group_id")) {
        qDebug() << "Failed to reset group_id:" << query.lastError().text();
        return false;
    }

    return true;
}



int addRemoved(QSqlDatabase & db, int m_userRemoved, int typeID, int id, const QString &text)
{

//  CH_T_BEGIN = 23,  
//  CH_T_END = 24,

    if (typeID == CH_T_BEGIN) {
        change_group_start(db, text);
    }
    else if (typeID == CH_T_END) {
        change_group_end(db);
    }

    return 0;

    std::map<QString, QVariant> map_v;

    map_v["dateRemoved"] = "$$$_NOW_$$$";
    map_v["userRemoved"] = m_userRemoved;
    map_v["changeTypeID"] = typeID;
    map_v["changedID"] = id;

    if (text != "" && id > 0) {
        map_v["changedText"] = text;
    }

//    int id1 = execInsertQ(db, "changed_object", map_v, true);
    int id1 = insertIntoDatabase(db, "changed_object", map_v);


    return id1;
}


int get_max_changed_object();
QString getPar(std::set<int>& v);
void reset_max_changed_object(QSqlDatabase &db);
void set_cxema_updated(bool yes);


bool read_undo_edit(QSqlDatabase &db, const QString &text, std::map<QString, int> &map_table, std::map<QString, std::map<QString, QString> > &map_table2)
{
#if 0
    QString q;
    QString qq = "";

    try {
        std::string utf8_text = ansi_to_utf8((const char*)text);
        nlohmann::json json_data = nlohmann::json::parse(utf8_text.c_str());

        for (const auto& item : json_data) {
            for (auto it = item.begin(); it != item.end(); ++it) {
                std::map<QString, QString> map_val;

                std::string topKey = it.key();  // Имя верхнеуровневого ключа
                QString tn = it.key().c_str();
                auto value = it.value();        // Значение, которое может быть объектом

                if (value.is_object()) {
                    int id = value["id"];

                    for (auto inner_it = value.begin(); inner_it != value.end(); ++inner_it) {
                        std::string innerKey = inner_it.key();
                        auto innerValue = inner_it.value();

                        QString col = utf8_to_ansi(innerKey).c_str();

                        QString s = utf8_to_ansi(innerValue.dump()).c_str();

                        if (innerValue.is_string()) {
                            s = utf8_to_ansi(innerValue).c_str();
                            if (s.Left(2) == "N'") {
                            }
                            else {
                                s = "'" + s + "'";
                            }
                        }

                        if (col != "id") {
                            map_val[col] = s;
                        }
                    }
                }
                else {
                    // Если значение не объект, выводим его напрямую
                    //                std::cout << "Значение: " << value << std::endl;
                }
            }
        }
    }
    catch (nlohmann::json::exception &ex) {
        log1((const char*)text);
        AfxMessageBox(ex.what());
    }
#endif
    return true;
}




void resetGid_old(CCxema * m_cxema, const QString & m_user_gid)
{
    QSqlDatabase db = m_cxema->m_db;

//    CAdoFile* ado = m_cxema->m_ado;
//    if (!ado) return;

    CGraph2* graph = m_cxema->m_graph;

    QString ss;

        int max_changed_object = get_max_changed_object();

//    ss.Format("%d", );
//    AfxMessageBox(ss);


//    max_changed_object = 169694;

    QString q = QString("select id, changeTypeID, changedID, changedText from changed_object where id > %1 order by id").arg(max_changed_object);

    std::set<int> set_nodes;
    std::set<int> set_lines;
    std::map<QString, std::set<int> > set_geo;

    QSqlQuery query(m_cxema->m_db);
    query.setForwardOnly(true);

    query_exec(m_cxema->m_db, query, q);

    while (query.next()) {
        int changedID = query.value("changedID").toInt();
        int changeTypeID = query.value("changeTypeID").toInt();
        QString changedText = query.value("changedText").toString();


        switch (changeTypeID) {
        case CH_T_DELETE_LINE:
        case CH_T_MOVE_LINE:
        case CH_T_ADD_LINE:
        case CH_T_RENAME_LINE:
            set_lines.insert(changedID);
            break;
        case CH_T_DELETE_NODE:
        case CH_T_MOVE_NODE:
        case CH_T_ADD_NODE:
        case CH_T_RENAME_NODE:
            set_nodes.insert(changedID);
            break;
//            case CH_T_DELETE_CXEMA_BEGIN: return "";
//            case CH_T_DELETE_CXEMA_END: return _TR("Удалена схема");


        case CH_T_DELETE_GEO:
        case CH_T_ADD_GEO:
        case CH_T_MOVE_GEO:
            {
                QString tn = changedText;
                int id = changedID;
                Klassif* kls = m_kl_list.findKlN(tn);
                if (kls) {
                    set_geo[tn].insert(id);
                }
            }
            break;

        case CH_T_EDIT:
            {
                std::map<QString, int> map_table;
                std::map<QString, std::map<QString, QString> > map_table2;
                read_undo_edit(m_cxema->m_db, changedText, map_table, map_table2);

                for (auto & [tn, id] : map_table) {
                    if (tn == "linesobj") {
                        set_lines.insert(id);
                    }
                    else if (tn == "nodes") {
                        set_nodes.insert(id);
                    }
                    else {
                        Klassif* kls = m_kl_list.findKlN(tn);
                        if (kls) {
                            set_geo[tn].insert(id);
                        }
                    }
                }
            }
            break;

/**

        case CH_T_DELETE_GEO: return _TR("Удален объект геобазы");
        case CH_T_ADD_GEO: return _TR("Добавлен объект геобазы");
        case CH_T_EDIT_GEO: return _TR("Редактировался объект геобазы");
*/

            }

    }

//    CGraph2* graph = m_cxema->m_graph;


    for (const int& id : set_nodes) {
        CNode2 * n = graph->find(id);
        if (n) {
            for (CLINE2* l = n->lines; l; l = nextl(l)) {
                set_lines.insert(bline(l)->line.nomP);
                set_lines.insert(bline(l)->line.nomO);
            }
        }
    }


    graph->init_find_line_nom();

    for (const int& id : set_lines) {
        CLINE2 *l = graph->find_line_nom(id);
        if (l) {
            graph->remove_line(l);
        }
    }

    for (const int& id : set_nodes) {
        CNode2 *n = graph->find(id);
        if (n) {
            graph->remove_node(*n);
        }
    }

    for (auto & [tn, set_id] : set_geo) {
        Klassif* kls = m_kl_list.findKlN(tn);
        if (kls) {
            for (auto& id : set_id) {
                int n = kls->GetNom2(id);
                if (n > 0) {
                    kls->geo4.erase(kls->geo4.begin() + (n - 1));
                }
            }
        }

/**

        CNode2* n = graph->find(id);
        if (n) {

            graph->remove_node(*n);
        }
*/
    }


    QString par2;
    QProgressDialog *percent = nullptr;

    if (set_nodes.size() > 0) {
        par2 = QString("(n.id in (%1)) ").arg(getPar(set_nodes));
        m_cxema->read_nodes(m_cxema->m_par, percent, false);
    }

    if (set_lines.size() > 0) {
        par2 = QString("(l.id in (%1)) ").arg(getPar(set_lines));
        m_cxema->read_lines(m_cxema->m_par, percent, false);
    }

    reset_max_changed_object(m_cxema->m_db);


    set_cxema_updated(false);
}




bool undo_gid(QSqlDatabase &db, int id, int &fileID)
{
//    QSqlDatabase db = m_cxema->m_db;
//    QString q = readQ("undo/undo.sql");


    return true;
}

CNode2* get_undo_node(CGraph2 *graph, const QString &table, int id);


void resetGid(CCxema * m_cxema, const QString & m_user_gid, int &n_nodes, int n_lines)
{
    int max_changed_object = get_max_changed_object();

//    max_changed_object = 0;

//    QString q = QString("select log_id,comment,operation,table_name,record_id,old_data,new_data,changed_at,changed_by,change_group_id,is_rolled_back from audit_log where id > %1 order by log_d").arg(max_changed_object);
    QString q = QString("select log_id,comment,operation,table_name,record_id,changed_at,changed_by,change_group_id,is_rolled_back from audit_log where log_id > %1 order by log_id").arg(max_changed_object);

    q = QString(R"(
select 
log_id,al.comment as comment_al,ag.comment as comment_ag,operation,table_name,record_id,node_id,changed_at,changed_by,is_rolled_back

from audit_log al
left join audit_group_comments ag on ag.change_group_id = al.change_group_id
where log_id > %1 
order by log_id
)").arg(max_changed_object);


static const QSet<QString> nS = {"realconsumers","generalizedconsumers","heatsources","pumpstations","connectnodes"};
static const QSet<QString> lS = {"heatpipesections","pumps","pressregulators","consumptregulators","dampers","regularmatures","bypass","diaphragms","elevators","systemradiators","heatexchangers","airheaters","pressdropregulators","reversevalves"};


    std::set<int> set_nodes;
    std::set<int> set_lines;


    QSqlQuery query(m_cxema->m_db);

    query.setForwardOnly(true);

    query_exec(m_cxema->m_db, query, q);

    while (query.next()) {
        QString table_name = query.value("table_name").toString();
        QString operation = query.value("operation").toString();
        int record_id = query.value("record_id").toInt();
        int node_id = query.value("node_id").toInt();
        QString comment_al = query.value("comment_al").toString();
        QString comment_ag = query.value("comment_ag").toString();

//        CNode2 * n =  get_undo_node(m_cxema->graph(), table_name, record_id);
//        if (n) {
            if (table_name == "nodes" || nS.contains(table_name.toLower())) {
                set_nodes.insert(node_id);
            }
            if (table_name == "linesobj" || lS.contains(table_name.toLower())) {
                set_lines.insert(node_id);
            }


/*
            if (operation == "UPDATE") {

            }
            else if (operation == "INSERT") {

            }
*/
//        }
    }

    CGraph2* graph = m_cxema->m_graph;

    QString par2;
    QProgressDialog *percent = nullptr;


    graph->init_find_line_nom();

    for (const int& id : set_lines) {
        CLINE2 *l = graph->find_line_nom(id);
        if (l) {
            graph->remove_line(l);
        }
    }

    for (const int& id : set_nodes) {
        CNode2 *n = graph->find(id);
        if (n) {
            graph->remove_node(*n);
        }
    }



    if (set_nodes.size() > 0) {
        par2 = QString("(n.id in (%1)) ").arg(getPar(set_nodes));
//        m_cxema->read_nodes(m_cxema->m_par, percent, false);
        m_cxema->read_nodes(par2, percent, false);
    }

    if (set_lines.size() > 0) {
        par2 = QString("(l.id in (%1)) ").arg(getPar(set_lines));
//        m_cxema->read_lines(m_cxema->m_par, percent, false);
        m_cxema->read_lines(par2, percent, false);
    }

    reset_max_changed_object(m_cxema->m_db);

    n_nodes = set_nodes.size();
    n_lines = set_lines.size();

    set_cxema_updated(false);
}
