#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>

namespace tgid::repo {

struct FragmentInfo final {
    int id = 0;
    QString name;
    QString systemName;
    QString settlement;
    QString season;
    QString year;
};

class FragmentRepository final {
public:
    [[nodiscard]] QList<FragmentInfo> loadActive(
        const QSqlDatabase& database,
        QString* error) const;
};

}  // namespace tgid::repo
