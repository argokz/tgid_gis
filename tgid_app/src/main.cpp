#include "db/DatabaseConfig.h"
#include "db/DatabaseConnection.h"
#include "repo/LayerCatalogRepository.h"
#include "repo/MapRepository.h"
#include "repo/ObjectRepository.h"
#include "ui/MainWindow.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("TGID"));
    QCoreApplication::setApplicationName(QStringLiteral("tgid_app"));
    QCoreApplication::setApplicationVersion(
        QStringLiteral(TGID_APP_VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Новый клиент ТГИД для объектной PostGIS-БД"));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption checkDatabaseOption(
        QStringLiteral("check-db"),
        QStringLiteral("Проверить подключение и контракт БД без запуска окна"));
    parser.addOption(checkDatabaseOption);
    const QCommandLineOption checkFragmentOption(
        QStringLiteral("check-fragment"),
        QStringLiteral(
            "Проверить загрузку геометрии указанного фрагмента без запуска окна"),
        QStringLiteral("id"));
    parser.addOption(checkFragmentOption);
    const QCommandLineOption checkObjectOption(
        QStringLiteral("check-object"),
        QStringLiteral(
            "Проверить карточку объекта в формате table:id без запуска окна"),
        QStringLiteral("table:id"));
    parser.addOption(checkObjectOption);
    const QCommandLineOption setObjectOption(
        QStringLiteral("set-object"),
        QStringLiteral(
            "Изменить одно поле объекта: table:id:field:value"),
        QStringLiteral("table:id:field:value"));
    parser.addOption(setObjectOption);
    const QCommandLineOption expectedVersionOption(
        QStringLiteral("expect-version"),
        QStringLiteral(
            "Ожидаемая row_version изменяемого объекта"),
        QStringLiteral("version"));
    parser.addOption(expectedVersionOption);
    const QCommandLineOption archiveObjectOption(
        QStringLiteral("archive-object"),
        QStringLiteral("Переместить объект table:id в архив"),
        QStringLiteral("table:id"));
    parser.addOption(archiveObjectOption);
    const QCommandLineOption restoreObjectOption(
        QStringLiteral("restore-object"),
        QStringLiteral("Восстановить объект table:id из архива"),
        QStringLiteral("table:id"));
    parser.addOption(restoreObjectOption);
    const QCommandLineOption checkHistoryOption(
        QStringLiteral("check-history"),
        QStringLiteral("Проверить журнал объекта table:id"),
        QStringLiteral("table:id"));
    parser.addOption(checkHistoryOption);
    const QCommandLineOption checkArchiveOption(
        QStringLiteral("check-archive"),
        QStringLiteral("Проверить загрузку списка архивных объектов"));
    parser.addOption(checkArchiveOption);
    const QCommandLineOption createPointOption(
        QStringLiteral("create-point"),
        QStringLiteral(
            "Создать точку: table:fragment_id:x:y"),
        QStringLiteral("table:fragment_id:x:y"));
    parser.addOption(createPointOption);
    const QCommandLineOption createLineOption(
        QStringLiteral("create-line"),
        QStringLiteral(
            "Создать линию: table:fragment_id:node_from:node_to"),
        QStringLiteral("table:fragment_id:node_from:node_to"));
    parser.addOption(createLineOption);
    const QCommandLineOption splitLineOption(
        QStringLiteral("split-line"),
        QStringLiteral(
            "Разрезать линию: table:id:x:y"),
        QStringLiteral("table:id:x:y"));
    parser.addOption(splitLineOption);
    const QCommandLineOption joinLinesOption(
        QStringLiteral("join-lines"),
        QStringLiteral(
            "Соединить участки: table:first_id:first_version:"
            "second_id:second_version"),
        QStringLiteral(
            "table:first_id:first_version:second_id:second_version"));
    parser.addOption(joinLinesOption);
    parser.process(application);

    if (parser.isSet(checkDatabaseOption)
        || parser.isSet(checkFragmentOption)
        || parser.isSet(checkObjectOption)
        || parser.isSet(setObjectOption)
        || parser.isSet(archiveObjectOption)
        || parser.isSet(restoreObjectOption)
        || parser.isSet(checkHistoryOption)
        || parser.isSet(checkArchiveOption)
        || parser.isSet(createPointOption)
        || parser.isSet(createLineOption)
        || parser.isSet(splitLineOption)
        || parser.isSet(joinLinesOption)) {
        const tgid::db::DatabaseConfig config =
            tgid::db::DatabaseConfig::fromEnvironment();
        tgid::db::DatabaseConnection connection;
        QString error;
        if (!connection.open(config, &error)) {
            qCritical().noquote() << QStringLiteral("Ошибка подключения:")
                                  << error;
            return 2;
        }

        const tgid::db::SchemaStatus status = connection.inspectSchema();
        if (!status.valid) {
            qCritical().noquote() << QStringLiteral("Ошибка контракта БД:")
                                  << status.error;
            return 3;
        }

        const tgid::repo::LayerCatalogRepository repository;
        const QList<tgid::repo::LayerInfo> layers =
            repository.load(connection.database(), &error);
        if (!error.isEmpty()) {
            qCritical().noquote() << QStringLiteral("Ошибка каталога слоёв:")
                                  << error;
            return 4;
        }

        qInfo().noquote()
            << QStringLiteral(
                   "OK: БД=%1 пользователь=%2 схема=%3 слоёв=%4")
                   .arg(status.database, status.user)
                   .arg(status.version)
                   .arg(layers.size());

        if (parser.isSet(checkFragmentOption)) {
            bool fragmentIdIsValid = false;
            const int fragmentId =
                parser.value(checkFragmentOption).toInt(&fragmentIdIsValid);
            if (!fragmentIdIsValid) {
                qCritical().noquote()
                    << QStringLiteral("Некорректный ID фрагмента");
                return 5;
            }

            const tgid::repo::MapData mapData =
                tgid::repo::MapRepository().loadFragment(config, fragmentId);
            if (!mapData.isValid()) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка загрузки фрагмента:")
                    << mapData.error;
                return 6;
            }
            qInfo().noquote()
                << QStringLiteral("OK: фрагмент=%1 узлов=%2 линий=%3")
                       .arg(fragmentId)
                       .arg(mapData.nodes.size())
                       .arg(mapData.lines.size());
        }

        if (parser.isSet(checkObjectOption)) {
            const QString objectReference = parser.value(checkObjectOption);
            const qsizetype separator = objectReference.lastIndexOf(':');
            bool objectIdIsValid = false;
            const qint64 objectId =
                objectReference.mid(separator + 1).toLongLong(&objectIdIsValid);
            const QString classTable = objectReference.left(separator);
            if (separator <= 0 || classTable.isEmpty() || !objectIdIsValid) {
                qCritical().noquote()
                    << QStringLiteral(
                           "Некорректная ссылка на объект, ожидается table:id");
                return 7;
            }

            const tgid::repo::ObjectDetails details =
                tgid::repo::ObjectRepository().load(
                    connection.database(), classTable, objectId);
            if (!details.isValid()) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка карточки объекта:")
                    << details.error;
                return 8;
            }
            qsizetype lookupFields = 0;
            qsizetype lookupOptions = 0;
            for (const tgid::repo::ObjectAttribute& attribute
                 : details.attributes) {
                if (attribute.editorKind == QStringLiteral("lookup")) {
                    ++lookupFields;
                    lookupOptions += attribute.options.size();
                }
            }
            qInfo().noquote()
                << QStringLiteral(
                       "OK: объект=%1:%2 атрибутов=%3 "
                       "справочных_полей=%4 вариантов=%5")
                       .arg(details.classTable)
                       .arg(details.id)
                       .arg(details.attributes.size())
                       .arg(lookupFields)
                       .arg(lookupOptions);
        }

        if (parser.isSet(setObjectOption)) {
            const QString reference = parser.value(setObjectOption);
            const qsizetype first = reference.indexOf(':');
            const qsizetype second = reference.indexOf(':', first + 1);
            const qsizetype third = reference.indexOf(':', second + 1);
            bool objectIdIsValid = false;
            bool versionIsValid = false;
            const QString classTable = reference.left(first);
            const qint64 objectId =
                reference.mid(first + 1, second - first - 1)
                    .toLongLong(&objectIdIsValid);
            const QString fieldName =
                reference.mid(second + 1, third - second - 1);
            const QString value = reference.mid(third + 1);
            const qint64 expectedVersion =
                parser.value(expectedVersionOption)
                    .toLongLong(&versionIsValid);
            if (first <= 0 || second <= first + 1 || third <= second + 1
                || classTable.isEmpty() || fieldName.isEmpty()
                || !objectIdIsValid || !parser.isSet(expectedVersionOption)
                || !versionIsValid || expectedVersion < 1) {
                qCritical().noquote()
                    << QStringLiteral(
                           "Ожидаются --set-object table:id:field:value "
                           "--expect-version N");
                return 9;
            }

            const tgid::repo::ObjectRepository objectRepository;
            const tgid::repo::ObjectDetails details =
                objectRepository.load(
                    connection.database(), classTable, objectId);
            if (!details.isValid()) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка карточки объекта:")
                    << details.error;
                return 8;
            }

            tgid::repo::AttributeChange change;
            bool editableFieldFound = false;
            for (const tgid::repo::ObjectAttribute& attribute
                 : details.attributes) {
                if (attribute.name == fieldName && attribute.editable) {
                    change.name = attribute.name;
                    change.databaseType = attribute.databaseType;
                    change.value = value;
                    change.setNull = value == QStringLiteral("NULL");
                    editableFieldFound = true;
                    break;
                }
            }
            if (!editableFieldFound) {
                qCritical().noquote()
                    << QStringLiteral("Поле нельзя редактировать:")
                    << fieldName;
                return 9;
            }

            const tgid::repo::UpdateResult updateResult =
                objectRepository.update(
                    connection.database(),
                    classTable,
                    objectId,
                    expectedVersion,
                    {change});
            if (updateResult.conflict) {
                qCritical().noquote()
                    << QStringLiteral("CONFLICT:") << updateResult.error;
                return 10;
            }
            if (!updateResult.success) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка изменения:")
                    << updateResult.error;
                return 11;
            }
            qInfo().noquote()
                << QStringLiteral("OK: объект=%1:%2 версия=%3")
                       .arg(classTable)
                       .arg(objectId)
                       .arg(updateResult.rowVersion);
        }

        if (parser.isSet(archiveObjectOption)
            || parser.isSet(restoreObjectOption)) {
            if (parser.isSet(archiveObjectOption)
                && parser.isSet(restoreObjectOption)) {
                qCritical().noquote()
                    << QStringLiteral(
                           "Нельзя одновременно архивировать и восстанавливать");
                return 12;
            }
            const bool archive = parser.isSet(archiveObjectOption);
            const QString reference = parser.value(
                archive ? archiveObjectOption : restoreObjectOption);
            const qsizetype separator = reference.lastIndexOf(':');
            bool objectIdIsValid = false;
            bool versionIsValid = false;
            const QString classTable = reference.left(separator);
            const qint64 objectId =
                reference.mid(separator + 1).toLongLong(&objectIdIsValid);
            const qint64 expectedVersion =
                parser.value(expectedVersionOption)
                    .toLongLong(&versionIsValid);
            if (separator <= 0 || classTable.isEmpty()
                || !objectIdIsValid
                || !parser.isSet(expectedVersionOption)
                || !versionIsValid || expectedVersion < 1) {
                qCritical().noquote()
                    << QStringLiteral(
                           "Ожидаются table:id и --expect-version N");
                return 12;
            }

            const tgid::repo::UpdateResult updateResult =
                tgid::repo::ObjectRepository().setArchived(
                    connection.database(),
                    classTable,
                    objectId,
                    expectedVersion,
                    archive);
            if (updateResult.conflict) {
                return 10;
            }
            if (!updateResult.success) {
                return 11;
            }
        }

        if (parser.isSet(checkHistoryOption)) {
            const QString reference = parser.value(checkHistoryOption);
            const qsizetype separator = reference.lastIndexOf(':');
            bool objectIdIsValid = false;
            const QString classTable = reference.left(separator);
            const qint64 objectId =
                reference.mid(separator + 1).toLongLong(&objectIdIsValid);
            if (separator <= 0 || classTable.isEmpty()
                || !objectIdIsValid) {
                return 12;
            }
            const QList<tgid::repo::ObjectHistoryEntry> entries =
                tgid::repo::ObjectRepository().loadHistory(
                    connection.database(), classTable, objectId, &error);
            if (!error.isEmpty()) {
                return 13;
            }
            qInfo().noquote()
                << QStringLiteral("OK: записей истории=%1")
                       .arg(entries.size());
        }

        if (parser.isSet(checkArchiveOption)) {
            const QList<tgid::repo::ArchivedObjectInfo> objects =
                tgid::repo::ObjectRepository().loadArchived(
                    connection.database(), &error);
            if (!error.isEmpty()) {
                return 14;
            }
            qInfo().noquote()
                << QStringLiteral("OK: объектов в архиве=%1")
                       .arg(objects.size());
        }

        if (parser.isSet(createPointOption)) {
            const QString reference = parser.value(createPointOption);
            const QStringList parts = reference.split(':');
            bool fragmentIsValid = false;
            bool xIsValid = false;
            bool yIsValid = false;
            if (parts.size() != 4) {
                return 15;
            }
            const int fragmentId =
                parts.at(1).toInt(&fragmentIsValid);
            const double x = parts.at(2).toDouble(&xIsValid);
            const double y = parts.at(3).toDouble(&yIsValid);
            if (parts.at(0).isEmpty() || !fragmentIsValid
                || !xIsValid || !yIsValid) {
                return 15;
            }
            const tgid::repo::CreateObjectResult createResult =
                tgid::repo::ObjectRepository().createPoint(
                    connection.database(),
                    parts.at(0),
                    fragmentId,
                    QPointF(x, y));
            if (!createResult.success) {
                return 16;
            }
            qInfo().noquote()
                << QStringLiteral("OK: создан объект=%1:%2 версия=%3")
                       .arg(parts.at(0))
                       .arg(createResult.id)
                       .arg(createResult.rowVersion);
        }

        if (parser.isSet(createLineOption)) {
            const QString reference = parser.value(createLineOption);
            const QStringList parts = reference.split(':');
            bool fragmentIsValid = false;
            bool fromIsValid = false;
            bool toIsValid = false;
            if (parts.size() != 4) {
                return 17;
            }
            const int fragmentId =
                parts.at(1).toInt(&fragmentIsValid);
            const qint64 nodeFrom =
                parts.at(2).toLongLong(&fromIsValid);
            const qint64 nodeTo =
                parts.at(3).toLongLong(&toIsValid);
            if (parts.at(0).isEmpty() || !fragmentIsValid
                || !fromIsValid || !toIsValid) {
                return 17;
            }
            const tgid::repo::CreateObjectResult createResult =
                tgid::repo::ObjectRepository().createLine(
                    connection.database(),
                    parts.at(0),
                    fragmentId,
                    nodeFrom,
                    nodeTo);
            if (!createResult.success) {
                return 18;
            }
            qInfo().noquote()
                << QStringLiteral("OK: создана линия=%1:%2 версия=%3")
                       .arg(parts.at(0))
                       .arg(createResult.id)
                       .arg(createResult.rowVersion);
        }

        if (parser.isSet(splitLineOption)) {
            const QStringList parts =
                parser.value(splitLineOption).split(':');
            bool idIsValid = false;
            bool xIsValid = false;
            bool yIsValid = false;
            bool versionIsValid = false;
            if (parts.size() != 4 || !parser.isSet(expectedVersionOption)) {
                return 19;
            }
            const qint64 id = parts.at(1).toLongLong(&idIsValid);
            const double x = parts.at(2).toDouble(&xIsValid);
            const double y = parts.at(3).toDouble(&yIsValid);
            const qint64 expectedVersion =
                parser.value(expectedVersionOption)
                    .toLongLong(&versionIsValid);
            if (parts.at(0).isEmpty() || !idIsValid
                || !xIsValid || !yIsValid || !versionIsValid) {
                return 19;
            }
            const tgid::repo::SplitLineResult splitResult =
                tgid::repo::ObjectRepository().splitLine(
                    connection.database(),
                    parts.at(0),
                    id,
                    expectedVersion,
                    QPointF(x, y));
            if (splitResult.conflict) {
                qCritical().noquote()
                    << QStringLiteral("CONFLICT:") << splitResult.error;
                return 20;
            }
            if (!splitResult.success) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка разрезания:")
                    << splitResult.error;
                return 21;
            }
            qInfo().noquote()
                << QStringLiteral(
                       "OK: узел=%1 новый_узел=%2 линии=%3,%4 доля=%5")
                       .arg(splitResult.nodeId)
                       .arg(splitResult.nodeCreated
                                ? QStringLiteral("да")
                                : QStringLiteral("нет"))
                       .arg(splitResult.firstLineId)
                       .arg(splitResult.secondLineId)
                       .arg(splitResult.splitFraction, 0, 'f', 6);
        }

        if (parser.isSet(joinLinesOption)) {
            const QStringList parts =
                parser.value(joinLinesOption).split(':');
            bool firstIdValid = false;
            bool firstVersionValid = false;
            bool secondIdValid = false;
            bool secondVersionValid = false;
            if (parts.size() != 5 || parts.at(0).isEmpty()) {
                return 22;
            }
            const qint64 firstId =
                parts.at(1).toLongLong(&firstIdValid);
            const qint64 firstVersion =
                parts.at(2).toLongLong(&firstVersionValid);
            const qint64 secondId =
                parts.at(3).toLongLong(&secondIdValid);
            const qint64 secondVersion =
                parts.at(4).toLongLong(&secondVersionValid);
            if (!firstIdValid || !firstVersionValid
                || !secondIdValid || !secondVersionValid) {
                return 22;
            }
            const tgid::repo::JoinLinesResult joinResult =
                tgid::repo::ObjectRepository().joinLines(
                    connection.database(),
                    parts.at(0),
                    firstId,
                    firstVersion,
                    secondId,
                    secondVersion);
            if (joinResult.conflict) {
                qCritical().noquote()
                    << QStringLiteral("CONFLICT:") << joinResult.error;
                return 23;
            }
            if (!joinResult.success) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка соединения:")
                    << joinResult.error;
                return 24;
            }
            qInfo().noquote()
                << QStringLiteral("OK: линия=%1 архивный_узел=%2")
                       .arg(joinResult.joinedLineId)
                       .arg(joinResult.archivedNodeId);
        }
        return 0;
    }

    tgid::ui::MainWindow window;
    window.show();
    return application.exec();
}
