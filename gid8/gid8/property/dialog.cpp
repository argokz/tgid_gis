#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include <QRegularExpression>

#include <map>
#include <list>
#include <mystd.h>

#include <db/db.h>

#include "PropertyDial.h"

QWidget * makeLabelWidget(QWidget *dialog, int num, const QString & rus, const QString & f1);

void read_fun(const QString & table, const QString & ext, std::map<QString, QString> &map_fun);


bool PropertyDial::init_dialog(QSqlDatabase *db, const QString & title, const QString & table, std::map<QString, QVariant> &map_val)
{
    setWindowTitle(title);

    read_fun(table, "fun", map_fun);

    QTabWidget *tabWidget = init_widget();

    QString fn = QString("%1dialog/%2.%3").arg(argpath(), table, "txt");

    QFile file(fn);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QTreeWidgetItem *parent = nullptr;
    QTreeWidgetItem *item_title = nullptr;
    QTreeWidgetItem *item_line = nullptr;

    PropertyTree *tree = new PropertyTree(1);
    tree->setColumnCount(2);
    tabWidget->addTab(tree, title);

    int num = 1;

    while (!file.atEnd()) {
        QString line = readCP1251Line(file);
        if (line.left(1) == "-") continue;
        if (line.left(1) != " ") {
            item_title = new QTreeWidgetItem(tree);
            item_title->setText(0, line);
            item_title->setFlags(item_title->flags() | Qt::ItemIsEditable);
            continue;
        }
        static QRegularExpression re("^\\s+([^ ]+)\\s+([^ ]+)\\s+(.+)\\s*$");
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString key = match.captured(1);
            QString type = match.captured(2);
            QString name = match.captured(3);

            std::map<int, QString> map_lookup;

            QMetaType metaType = QMetaType(QMetaType::QString);

            QVariant val = "";

            bool readonly = false;

            int index = -1;

            if (type == "D") {
                metaType = QMetaType(QMetaType::QDate);
            }
            else if (type == "N") {
                metaType = QMetaType(QMetaType::Int);
            }
            else if (type == "F") {
                metaType = QMetaType(QMetaType::Double);
            }
            else if (type == "M") {
                metaType = QMetaType(QMetaType::QString);
            }
            else if ((type == "L" || type == "I") && db) {
                metaType = QMetaType(QMetaType::Int);
                static QRegularExpression re("^\"(.+?)\",\"(.+?)\",\"(.+?)\",\"(.+?)\"\\s+(.+)$");
                QRegularExpressionMatch match = re.match(name);
                if (match.hasMatch()) {
                    QString ps = match.captured(1);
                    QString table1 = match.captured(2);
                    QString sid = match.captured(3);
                    QString sval = match.captured(4);
                    readLookup(*db, table1, sid, sval, map_lookup);
                    name = match.captured(5);
                }
            }


            bool listOnly = type == "L";

//            listOnly = false;

            auto it = map_val.find(key);
            if (it != map_val.end()) {
                val = it->second;
            }

            if (map_lookup.size() > 0) {
                index = val.toInt();
            }


            item_line = new QTreeWidgetItem(item_title);
            tree->setItemWidget(item_line, 0, makeLabelWidget(this, num++, name, ""));

            addRow(item_line, tree, db,
                   val,
                   0,
                   index,
                   -1,
                   metaType, table, 1, key, name, "", false, listOnly, map_lookup, "", "");
        }
    }

    tree->restore(table);

    return true;
}


