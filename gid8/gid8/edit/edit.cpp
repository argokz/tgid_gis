#include <QApplication>
#include <QtWidgets>
#include <QRegularExpression>

#include <QDir>
#include <QMessageBox>
#include <QMap>

#include "editdialog.h"
#include "help.h"

#include <db/db.h>


static QMap<QString, EditItem> map_item;
//static QMap<QString, QMap<QString, EditItem > > map_item2;

EditItem *get_item(const QString & name)
{
  QMap<QString, EditItem>::iterator it = map_item.find(name);
  if (it != map_item.end()) {
    return &it.value();
  }
  return NULL;
}


bool open_list(const QString & tn, QVector<QPair<QString, QVector<QString> > > &v)
{
  QString fn = QString("D:/Qt/txt/%1.txt").arg(tn);
  QString tabn = "";

  QFile file(fn);

  map_item.clear();

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return false;
  }

  while (!file.atEnd()) {
    QByteArray line = file.readLine();
    if (line[0] == ' ') {
      if (!tabn.isEmpty()) {
        EditItem item;
  
        QString str = line.trimmed();
        QRegularExpression sp("\\s\\s+");
        QStringList list = str.split(sp);

        int first = 2;

        if (line.size() >= 2) {
          item.name = list[0];
          item.typ = list[1];

          if (item.typ == "Next") {
            item.next = true;
            item.typ = list[2];
            first++;
          }

          if (item.typ == "Combo") {
            for (int i = first; i+1 < list.size(); i += 2) {
              if (list[i] == "@") {
                item.combo1.push_back(" ");
              }
              else {
                item.combo1.push_back(list[i]);
              }
              item.combo.push_back(list[i+1]);
            }
          }
          else if (item.typ == "Table" || item.typ == "EditTable") {
            item.dbn = list[first];
            item.table = list[first+1];
            if (list.size() > first+2) item.query = list[first+2];
          }
          else if (item.typ == "ComboTable") {
            if (list.size() >= 6 ) {
              QString tn = list[first+1];
              QString f1 = list[first+2];
              QString f2 = list[first+3];

              QString q;
              QTextStream out(&q);
              out << "SELECT DISTINCT [" << f1 << "], [" << f2 << "] FROM [" << tn << "] ORDER BY " << f1;

              QSqlDatabase db = QSqlDatabase::database(list[first]); 
              QSqlQuery query(db);
              query.setForwardOnly(true);

              query_exec(db, query, q);

//              QMessageBox::information(0, "", q);

              while (query.next()) { 
//              QMessageBox::information(0, q, query.value(0).toString());
                item.combo.push_back(query.value(0).toString());
                item.combo1.push_back(query.value(1).toString());
              }
            }
          }
          v.back().second.push_back(item.name);
          map_item[item.name] = item;
        }
        else {
          item.name = str;
          item.typ = "";
          v.back().second.push_back(item.name);
          map_item[item.name] = item;
        }
      }
    }
    else {
      tabn = line.simplified ();
//      QMessageBox::warning(0, "", QString("v.size() = %1").arg(tabn));
      v.push_back(QPair<QString, QVector<QString> > (tabn, QVector<QString> ()));
    }
  }
  return true;
}



bool edit_q(QSqlDatabase &db, QWidget *window, const QString & tn, const QString & q)
{
  QSqlQuery query(db);

//
//  QMessageBox::warning(0, QObject::tr("!"), QString("SELECT * FROM [%1] WHERE %2").arg(tn, q));

  if (!query_exec(db, query, QString("SELECT * FROM %1 WHERE %2").arg(tbl_sql(tn), q))) {
    QMessageBox::warning(0, QObject::tr("!"), q);
    return false;
  }

  QSqlRecord record = query.record();

  QMap<QString, QVariant> mapv;

  if (query.next()) { 
    QVector<QPair<QString, QVector<QString> >  > v;

    if (!open_list(tn, v)) {
//      QFile file(QDir::currentPath()+"/1/"+tn+".txt");
      QFile file(QString("D:/Qt/txt/%1.txt").arg(tn));
      if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
      QTextStream out(&file);
//      out.setCodec("UTF8");
      
      QString name = QObject::tr("Основные");
      out << name << Qt::endl;

      v.push_back(QPair<QString, QVector<QString> > (name, QVector<QString> ()));
      
      for (int k = 0; k < record.count(); k++) {
        EditItem item;
        QSqlField field = record.field(k);
        item.name = field.name();

        if (field.metaType().id() == QMetaType::QDateTime) {
          item.typ = "Kalendar";
        }
        else {
          item.typ = "Edit";
        }
        out << "  " << item.name << "  " << item.typ << Qt::endl;
        map_item[item.name] = item;
        v[0].second.push_back(item.name);
      }
    }

    for (int k = 0; k < record.count(); k++) {
      QSqlField field = record.field(k);

      mapv[field.name()] = query.value(k).toString();

      EditItem *item = get_item(field.name());
      if (item) {
        item->field = field;
        item->sql_type = field.metaType();
        item->value = query.value(k).toString();

        if (field.metaType().id() == QMetaType::QDateTime) {
          item->typ = "Kalendar";
        }
      }
    }

    EditDialog *dialog = new EditDialog(tn, v, mapv, window);

    dialog->setWindowTitle(tn);

    dialog->resize(600, 400);
    while (dialog->exec() == QDialog::Accepted) {
      QString str, q0, q1 = "";
      QMap<QString, QVariant>::const_iterator it;

      for (it = dialog->get_out().begin(); it != dialog->get_out().end(); it++) {
        EditItem *item = get_item(it.key());
        if (item && item->value != it.value().toString()) {
          if (item->sql_type.id() == QMetaType::Double) {
            q1 += QString("%1 = %2,").arg(it.key()).arg(it.value().toString());
          }
          else if (item->sql_type.id() == QMetaType::Int) {
            q1 += QString("%1 = %2,").arg(it.key()).arg(it.value().toString());
          }
          else {
            q1 += QString("%1 = '%2',").arg(it.key()).arg(it.value().toString());
          }
        }
      }
      if (q1.isEmpty()) {
        break;
      }
      else {
        q1 = q1.left(q1.size()-1);

        q0 = QString("UPDATE %1 SET %2 WHERE %3")
         .arg(tbl_sql(tn)).arg(q1).arg(q);

        if (query_exec(db, query, q0)) {
          return true;
        }
        QMessageBox::warning(0, "", q0 + "\n" + query.lastError().text());
      }
    }
  }
  return false;
}

bool edit(QSqlDatabase &db, QWidget *window, const QString & tn, int n)
{
  QString q = QString("[id]=%1").arg(n);

  return edit_q(db, window, tn, q);
}
