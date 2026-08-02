#include "MultiHeaderTable.h"
#include "mystd.h"

MultiHeaderTable *getMyTableView(QSqlDatabase &db, const QString & tn, const QString & q, const QString & title)
{

    if (!&db)  return nullptr;

    MultiHeaderTable *table = new MultiHeaderTable();

    if (!table) return nullptr;

    QString style = readQSS("qss2/table.qss");
    table->mainTable->setStyleSheet(style);

    table->init(db, tn, q, title);

    return table;
}

