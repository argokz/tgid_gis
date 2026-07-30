#include "repo/FragmentRepository.h"

#include <QSqlError>
#include <QSqlQuery>

namespace tgid::repo {

QList<FragmentInfo> FragmentRepository::loadActive(
    const QSqlDatabase& database,
    QString* error) const
{
    QList<FragmentInfo> fragments;
    QSqlQuery query(database);
    query.setForwardOnly(true);

    if (!query.exec(QStringLiteral(
            "SELECT id, name, coalesce(name_sys, ''),"
            "       coalesce(nasel_point, ''), coalesce(sezon, ''),"
            "       coalesce(\"year\", '')"
            "  FROM net.fragment"
            " WHERE removed_at IS NULL"
            " ORDER BY coalesce(nasel_point, ''), name, id"))) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return fragments;
    }

    while (query.next()) {
        FragmentInfo fragment;
        fragment.id = query.value(0).toInt();
        fragment.name = query.value(1).toString();
        fragment.systemName = query.value(2).toString();
        fragment.settlement = query.value(3).toString();
        fragment.season = query.value(4).toString();
        fragment.year = query.value(5).toString();
        fragments.append(std::move(fragment));
    }
    return fragments;
}

}  // namespace tgid::repo
