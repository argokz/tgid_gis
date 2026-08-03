#pragma once

#include <QByteArray>
#include <QHash>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>

// Доступ к полям строки по имени — без поиска имени на каждом обращении.
//
// QSqlQuery::value(const QString&) на КАЖДЫЙ вызов строит QSqlRecord и
// линейно ищет имя без учёта регистра. При чтении схемы это оказалось
// главной ценой: замер по журналу показал 12,3 с на 86 707 узлов и
// 21,3 с на 80 614 участков, тогда как сами запросы к БД занимают 1,6 с
// на всё. В чтении узлов 67 обращений по имени на строку при 76
// колонках в выборке, в чтении участков — 62 при 57.
//
// Здесь имена колонок разбираются один раз после выполнения запроса, и
// дальше обращение идёт по индексу.
//
// Имена приводятся к нижнему регистру: в коде они писались как придётся
// (nodeID1, fileID, externalCodeID), а PostgreSQL возвращает их
// строчными. Прежний value(QString) сравнивал без учёта регистра и это
// скрывал; при переходе на индексы разница стала бы ошибкой.
class ColIndex
{
public:
    ColIndex() = default;

    explicit ColIndex(const QSqlQuery &query)
    {
        init(query);
    }

    void init(const QSqlQuery &query)
    {
        m_index.clear();
        const QSqlRecord rec = query.record();
        const int n = rec.count();
        m_index.reserve(n);
        for (int i = 0; i < n; i++) {
            m_index.insert(rec.fieldName(i).toLower().toLatin1(), i);
        }
    }

    // -1, если колонки нет: QSqlQuery::value(-1) вернёт недействительный
    // QVariant, то есть поведение совпадёт с прежним обращением по
    // отсутствующему имени, а не приведёт к падению.
    //
    // Имя приводится к нижнему регистру в буфере на стеке. Через
    // QString это была бы аллокация на каждое обращение — 67 штук на
    // строку, почти шесть миллионов за чтение всех фрагментов, то есть
    // мы бы заменили одну лишнюю работу другой.
    int operator()(const char *name) const
    {
        char buf[64];
        int n = 0;
        for (; name[n] && n < int(sizeof(buf)) - 1; n++) {
            const char c = name[n];
            buf[n] = (c >= 'A' && c <= 'Z') ? char(c - 'A' + 'a') : c;
        }
        buf[n] = '\0';
        return m_index.value(QByteArray::fromRawData(buf, n), -1);
    }

private:
    QHash<QByteArray, int> m_index;
};
