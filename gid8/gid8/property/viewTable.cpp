#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "PropertyDial.h"
#include "qmessagebox.h"
#include <db/db.h>
#include <mystd.h>

#include <QMessageBox>

//#include "variantdelegate.h"

#include <dialog/TableWindow.h>


QString transl(const QString & s);
QString create_select_rus(QSqlDatabase & db, const QString & tn, const QString & s_shape, const QString & s_id);

// Список по запросу


QString viewTableQ(QWidget *widget, QSqlDatabase &db, const QString & label, const QString & fun, bool *ok)
{
     *ok = false;
    static QRegularExpression re("TableQ \"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9]+)\\s*$");
    QRegularExpressionMatch match = re.match(fun);
    if (match.hasMatch()) {
        QString db1 = match.captured(1);
        QString tn = match.captured(2);
        QString q = match.captured(3);
        int n_ret = match.captured(4).toInt();

        tn = transl(tn);

        TableWindow tab(db, tn, label, q, widget, -1, false, true, false, true);

        if (tab.exec()) {
            *ok = true;

            QVariant v = tab.value(n_ret);
            return v.toString();
        }
    }
    return "";
}

// Список из таблицы

#include <ui2/OpenServer.h>

void init_cs(ConnectStr &cs);


QString viewTable(QWidget *widget, QSqlDatabase &db, const QString & label, const QString & fun, bool *ok)
{
     *ok = false;
    static QRegularExpression re("Table \"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9]+)\\s*$");
    QRegularExpressionMatch match = re.match(fun);
    if (match.hasMatch()) {
        QString db1 = match.captured(1);
        QString tn = match.captured(2);
        int n_ret = match.captured(3).toInt();

        QSqlDatabase db0 = db;

        if (db1 != "$main$" && db1 != "$ps$") {
            ConnectStr cs;
            init_cs(cs);

            bool ok = connectSQL0(cs.rdbms, cs.host, cs.port, db1, cs.user, cs.password, db0);
            if (!ok) {
                QMessageBox::warning(widget, "", "Error");
            }
        }

        tn = transl(tn);
        QString q = create_select_rus(db0, tn, "shape", "id");

        TableWindow tab(db0, "", label, q, widget, false, true, true, true);

        if (tab.exec()) {
            *ok = true;
            return tab.value(n_ret).toString();
        }
    }
    return "";
}
