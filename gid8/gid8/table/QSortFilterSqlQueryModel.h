#pragma once

#include <QSqlQueryModel>
#include <QSqlRecord>

class QSortFilterSqlQueryModel : public QSqlQueryModel {

    Q_OBJECT

public:

    explicit QSortFilterSqlQueryModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

//void setQuery(const QSqlQuery &query);
    
    void setQuery(const QString & query, const QSqlDatabase & db = QSqlDatabase::database() );
    void setQuery0(const QString & query);

    QString getOriginalQuery() const {
        return originalQuery;
    }

public slots:

    void setFilterColumn (const QString & column);

    void setFilterFlags (const Qt::MatchFlag flags);
    void setFilter( const QString & filter );
    void filter( const QString & filter );
    void setAll( bool all);
    bool getAll() { 
        return m_all;
    };
    
    void setDelete();

    void select();

    QString filter() { return filterString; };

    virtual void setSort (int column, Qt::SortOrder order);

    virtual void sort (int column, Qt::SortOrder order);
private:

    QSqlDatabase queryDB;

    QString queryClause;
    Qt::MatchFlag filterFlags;
    QString filterString;
    QString filterColumn;
    int sortKeyColumn;
    Qt::SortOrder sortOrder;
    bool m_all = false;
    QString originalQuery;

signals:

};
