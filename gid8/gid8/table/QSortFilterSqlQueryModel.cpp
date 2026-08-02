#include "QSortFilterSqlQueryModel.h"
#include <QtSql>
#include <QString>

QSortFilterSqlQueryModel::QSortFilterSqlQueryModel(QObject* parent)
    : QSqlQueryModel(parent) {}

void QSortFilterSqlQueryModel::setQuery0(const QString& query) {
    queryClause = query;
}

#include <QStandardItemModel>
#include <QPushButton>

void QSortFilterSqlQueryModel::setDelete() 
{
    int columns = columnCount();
/*

    for (int row = 0; row < rowCount(); ++row) {
        QStandardItem *deleteButtonItem = new QStandardItem();
        QPushButton *deleteButton = new QPushButton();
        deleteButton->setIcon(QIcon(":/images/db2/remove_item.svg"));  // Пример с иконкой удаления
        deleteButtonItem->setData(QVariant::fromValue(deleteButton), Qt::UserRole);
        setItem(row, columns - 1, deleteButtonItem);
    }
*/
}


void QSortFilterSqlQueryModel::setQuery(const QString& query,
                                        const QSqlDatabase& db) {

    originalQuery = query;

//    queryClause = query;
    queryDB = db;

    filterString.clear();
    // filterColumn.clear();
    filterFlags = Qt::MatchStartsWith;
    sortKeyColumn = -1;
    sortOrder = Qt::AscendingOrder;

    QSqlQueryModel::setQuery(query, db);

    QSqlError err = this->lastError();
    if (err.isValid()) {
        qDebug() << err.text();
//        QMessageBox::critical(this, "Database Error", err.text());
        // Дополнительная обработка ошибки
    }

}

bool is_POSTGRESQL();


void QSortFilterSqlQueryModel::select() {
    if (queryClause.isEmpty() || (!queryDB.isValid()))
        return;

    QString whereClause = "";
    QString orderClause = "";
    QString query = queryClause;


    if (!filterString.isEmpty() && !filterColumn.isEmpty()) {
        QString esFilterString = filterString;
        QString esFilterColumn = filterColumn;

        if (filterFlags ==  Qt::MatchExactly) {  // totaly wrong :) Qt::MatchExacly = 0 & 0
            whereClause = "%1 = %2";
        } 
        else if (filterFlags == Qt::MatchStartsWith) {
            whereClause = "%1 LIKE %2";
            esFilterString.append("%");
        } 
        else if (filterFlags == Qt::MatchEndsWith) {
            whereClause = "%1 LIKE %2";
            esFilterString.prepend("%");
//        } else if (filterFlags & Qt::MatchRegExp) {
//            whereClause = "WHERE %1 REGEXP %2";
        } 
        else if (filterFlags == Qt::MatchWildcard) {
            whereClause = "%1 LIKE %2";
        } 
        else if (filterFlags == Qt::MatchContains) {
            whereClause = "%1 LIKE %2";
            esFilterString.append("%");
            esFilterString.prepend("%");
        } 
        else {
            return;
        }  // unhandled filterflag

        if (is_POSTGRESQL()) {
            whereClause.replace(" LIKE ", "::text ILIKE ");
        }

        QSqlDriver* driver = queryDB.driver();
        esFilterColumn =
            driver->escapeIdentifier(filterColumn, QSqlDriver::FieldName);
        QSqlField field;
//        field.setType(QVariant::String);
        field.setMetaType(QMetaType(QMetaType::QString));
        field.setValue(esFilterString);
        esFilterString = driver->formatValue(field);

        whereClause =
            whereClause.replace("%1", esFilterColumn).replace("%2", esFilterString);
    }

    if (sortKeyColumn >= 0) {
        orderClause = "ORDER BY " + QString::number(sortKeyColumn + 1) + " " +
                      ((sortOrder == Qt::AscendingOrder) ? "ASC" : "DESC");
    }

    if (m_all) {
        query = query.replace("AND n1.internalNodeID IS NULL", "");
        query = query.replace("AND n.internalNodeID IS NULL", "");
    }

    query = QString("select * from (%1) _").arg(query);
    if (whereClause != "") {
        query += QString("\nwhere %1").arg(whereClause);
    }
    if ( orderClause != "") {
        query += QString("\n%1").arg(orderClause);
    }

    originalQuery = query;

    qDebug() << query;

    QSqlQueryModel::setQuery(query, queryDB);
}

void QSortFilterSqlQueryModel::setSort(int column, Qt::SortOrder order) {
    sortKeyColumn = column;
    sortOrder = order;
}

void QSortFilterSqlQueryModel::sort(int column, Qt::SortOrder order) {
    if ((sortKeyColumn != column) || (sortOrder != order)) {
        setSort(column, order);
        select();
    }
}

void QSortFilterSqlQueryModel::setFilterColumn(const QString& column) {
    filterColumn = column;
}

void QSortFilterSqlQueryModel::setFilter(const QString& filter) {
    filterString = filter;
}

void QSortFilterSqlQueryModel::setFilterFlags(const Qt::MatchFlag flags) {
    filterFlags = flags;
}

void QSortFilterSqlQueryModel::filter(const QString& filter) {
    if (filterString != filter) {
        setFilter(filter);
        select();
    }
}

void QSortFilterSqlQueryModel::setAll( bool all) 
{
    m_all = all;
    select();
}


QVariant QSortFilterSqlQueryModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::BackgroundRole) {
        // Проверяем четность строки и задаем разные цвета
        if (index.row() % 2 == 0) {
            return QBrush(QColor("#f7f7f7"));  // Цвет для четных строк
        } 
        else {
            return QBrush(QColor("#ffffff"));  // Цвет для нечетных строк
        }
    }
    // Для всех остальных ролей используем стандартное поведение
    return QSqlQueryModel::data(index, role);
}
