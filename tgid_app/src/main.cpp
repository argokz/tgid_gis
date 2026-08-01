#include "db/DatabaseConfig.h"
#include "db/DatabaseConnection.h"
#include "repo/LayerCatalogRepository.h"
#include "repo/MapRepository.h"
#include "repo/ObjectRepository.h"
#include "repo/PipeLengthReportRepository.h"
#include "repo/PipeVolumeReportRepository.h"
#include "repo/SearchRepository.h"
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
    const QCommandLineOption moveNodeOption(
        QStringLiteral("move-node"),
        QStringLiteral("Переместить узел: table:id:x:y"),
        QStringLiteral("table:id:x:y"));
    parser.addOption(moveNodeOption);
    const QCommandLineOption reclassObjectOption(
        QStringLiteral("reclass-object"),
        QStringLiteral("Изменить класс объекта: source:id:target"),
        QStringLiteral("source:id:target"));
    parser.addOption(reclassObjectOption);
    const QCommandLineOption batchSetOption(
        QStringLiteral("batch-set"),
        QStringLiteral("Изменить одно поле нескольких объектов: table:field:value"),
        QStringLiteral("table:field:value"));
    parser.addOption(batchSetOption);
    const QCommandLineOption batchObjectsOption(
        QStringLiteral("batch-objects"),
        QStringLiteral("ID и версии объектов: id:version,id:version"),
        QStringLiteral("id:version,..."));
    parser.addOption(batchObjectsOption);
    const QCommandLineOption searchObjectsOption(
        QStringLiteral("search-objects"),
        QStringLiteral(
            "Поиск объектов: table или table:field:comparison"),
        QStringLiteral("table[:field:comparison]"));
    parser.addOption(searchObjectsOption);
    const QCommandLineOption searchConditionOption(
        QStringLiteral("search-condition"),
        QStringLiteral(
            "Повторяемое AND-условие: field|comparison[|value[|second]]"),
        QStringLiteral("condition"));
    parser.addOption(searchConditionOption);
    const QCommandLineOption searchValueOption(
        QStringLiteral("search-value"),
        QStringLiteral("Первое значение поиска"),
        QStringLiteral("value"));
    parser.addOption(searchValueOption);
    const QCommandLineOption searchSecondValueOption(
        QStringLiteral("search-second-value"),
        QStringLiteral("Второе значение диапазона"),
        QStringLiteral("value"));
    parser.addOption(searchSecondValueOption);
    const QCommandLineOption searchArchivedOption(
        QStringLiteral("search-archived"),
        QStringLiteral("Включить архивные строки в поиск"));
    parser.addOption(searchArchivedOption);
    const QCommandLineOption pipeLengthReportOption(
        QStringLiteral("pipe-length-report"),
        QStringLiteral("Отчёт протяжённости: grouping[:fragment_id]"),
        QStringLiteral("grouping[:fragment_id]"));
    parser.addOption(pipeLengthReportOption);
    const QCommandLineOption pipeVolumeReportOption(
        QStringLiteral("pipe-volume-report"),
        QStringLiteral("Отчёт объёма сети: grouping[:fragment_id]"),
        QStringLiteral("grouping[:fragment_id]"));
    parser.addOption(pipeVolumeReportOption);
    const QCommandLineOption reportArchivedOption(
        QStringLiteral("report-archived"),
        QStringLiteral("Включить архивные участки в отчёт"));
    parser.addOption(reportArchivedOption);
    const QCommandLineOption reportPipeIdsOption(
        QStringLiteral("report-pipe-ids"),
        QStringLiteral("Ограничить отчёт ID участков через запятую"),
        QStringLiteral("id,id,..."));
    parser.addOption(reportPipeIdsOption);
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
        || parser.isSet(joinLinesOption)
        || parser.isSet(moveNodeOption)
        || parser.isSet(reclassObjectOption)
        || parser.isSet(batchSetOption)
        || parser.isSet(searchObjectsOption)
        || parser.isSet(searchConditionOption)
        || parser.isSet(pipeLengthReportOption)
        || parser.isSet(pipeVolumeReportOption)) {
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

        if (parser.isSet(moveNodeOption)) {
            const QStringList parts =
                parser.value(moveNodeOption).split(':');
            bool idValid = false;
            bool versionValid = false;
            bool xValid = false;
            bool yValid = false;
            if (parts.size() != 4 || parts.at(0).isEmpty()
                || !parser.isSet(expectedVersionOption)) {
                return 32;
            }
            const qint64 id = parts.at(1).toLongLong(&idValid);
            const double x = parts.at(2).toDouble(&xValid);
            const double y = parts.at(3).toDouble(&yValid);
            const qint64 version = parser.value(expectedVersionOption)
                                       .toLongLong(&versionValid);
            if (!idValid || !versionValid || !xValid || !yValid) {
                return 32;
            }
            const tgid::repo::MoveNodeResult moveResult =
                tgid::repo::ObjectRepository().moveNode(
                    connection.database(), parts.at(0), id, version,
                    QPointF(x, y));
            if (moveResult.conflict) {
                qCritical().noquote()
                    << QStringLiteral("CONFLICT:") << moveResult.error;
                return 33;
            }
            if (!moveResult.success) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка перемещения:")
                    << moveResult.error;
                return 34;
            }
            qInfo().noquote()
                << QStringLiteral(
                       "OK: узел=%1 версия=%2 перестроено_линий=%3")
                       .arg(id)
                       .arg(moveResult.rowVersion)
                       .arg(moveResult.connectedLines);
        }

        if (parser.isSet(reclassObjectOption)) {
            const QStringList parts =
                parser.value(reclassObjectOption).split(':');
            bool idValid = false;
            bool versionValid = false;
            if (parts.size() != 3 || parts.at(0).isEmpty()
                || parts.at(2).isEmpty()
                || !parser.isSet(expectedVersionOption)) {
                return 35;
            }
            const qint64 id = parts.at(1).toLongLong(&idValid);
            const qint64 version = parser.value(expectedVersionOption)
                                       .toLongLong(&versionValid);
            if (!idValid || !versionValid) {
                return 35;
            }
            const tgid::repo::ReclassResult reclassResult =
                tgid::repo::ObjectRepository().reclassObject(
                    connection.database(), parts.at(0), id, version,
                    parts.at(2));
            if (reclassResult.conflict) {
                qCritical().noquote()
                    << QStringLiteral("CONFLICT:") << reclassResult.error;
                return 36;
            }
            if (!reclassResult.success) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка смены класса:")
                    << reclassResult.error;
                return 37;
            }
            qInfo().noquote()
                << QStringLiteral(
                       "OK: объект=%1:%2 новый_класс=%3 версия=%4 полей=%5")
                       .arg(parts.at(0))
                       .arg(id)
                       .arg(reclassResult.targetTable)
                       .arg(reclassResult.rowVersion)
                       .arg(reclassResult.copiedFields);
        }

        if (parser.isSet(batchSetOption)) {
            const QString reference = parser.value(batchSetOption);
            const qsizetype first = reference.indexOf(':');
            const qsizetype second = reference.indexOf(':', first + 1);
            const QString classTable = reference.left(first);
            const QString fieldName =
                reference.mid(first + 1, second - first - 1);
            const QString value = reference.mid(second + 1);
            if (first <= 0 || second <= first + 1
                || classTable.isEmpty() || fieldName.isEmpty()
                || !parser.isSet(batchObjectsOption)) {
                qCritical().noquote()
                    << QStringLiteral(
                           "Ожидаются --batch-set table:field:value "
                           "--batch-objects id:version,id:version");
                return 25;
            }

            QList<tgid::repo::ObjectVersion> objects;
            const QStringList objectReferences =
                parser.value(batchObjectsOption).split(
                    ',', Qt::SkipEmptyParts);
            for (const QString& objectReference : objectReferences) {
                const qsizetype separator = objectReference.indexOf(':');
                bool idValid = false;
                bool versionValid = false;
                const qint64 id = objectReference.left(separator)
                                      .toLongLong(&idValid);
                const qint64 version = objectReference.mid(separator + 1)
                                           .toLongLong(&versionValid);
                if (separator <= 0 || !idValid || !versionValid
                    || id <= 0 || version <= 0) {
                    return 25;
                }
                objects.append({id, version});
            }
            if (objects.size() < 2) {
                return 25;
            }

            const tgid::repo::ObjectDetails details =
                tgid::repo::ObjectRepository().load(
                    connection.database(), classTable, objects.first().id);
            if (!details.isValid()) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка карточки объекта:")
                    << details.error;
                return 27;
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
                return 25;
            }

            const tgid::repo::BatchUpdateResult batchResult =
                tgid::repo::ObjectRepository().batchUpdate(
                    connection.database(), classTable, objects, change);
            if (batchResult.conflict) {
                qCritical().noquote()
                    << QStringLiteral("CONFLICT:") << batchResult.error;
                return 26;
            }
            if (!batchResult.success) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка массового изменения:")
                    << batchResult.error;
                return 27;
            }
            qInfo().noquote()
                << QStringLiteral("OK: массово обновлено=%1")
                       .arg(batchResult.updatedCount);
        }

        if (parser.isSet(searchObjectsOption)
            || parser.isSet(searchConditionOption)) {
            const QStringList parts =
                parser.value(searchObjectsOption).split(':');
            const bool legacyCondition = parts.size() == 3;
            if (!parser.isSet(searchObjectsOption)
                || (parts.size() != 1 && !legacyCondition)
                || parts.at(0).isEmpty()
                || (legacyCondition
                    && (parts.at(1).isEmpty() || parts.at(2).isEmpty()))) {
                qCritical().noquote()
                    << QStringLiteral(
                           "Ожидается --search-objects table[:field:comparison]");
                return 28;
            }
            tgid::repo::SearchCriteria criteria;
            criteria.classTable = parts.at(0);
            criteria.includeArchived = parser.isSet(searchArchivedOption);
            criteria.limit = 20;
            if (legacyCondition) {
                tgid::repo::SearchCondition condition;
                condition.fieldName = parts.at(1);
                condition.comparison = parts.at(2);
                condition.value = parser.value(searchValueOption);
                condition.secondValue = parser.value(searchSecondValueOption);
                if (condition.comparison != QStringLiteral("is_null")
                    && condition.comparison != QStringLiteral("not_null")
                    && !parser.isSet(searchValueOption)) {
                    return 28;
                }
                if (condition.comparison == QStringLiteral("between")
                    && !parser.isSet(searchSecondValueOption)) {
                    return 28;
                }
                criteria.conditions.append(condition);
            }
            for (const QString& encoded :
                 parser.values(searchConditionOption)) {
                const QStringList conditionParts =
                    encoded.split('|', Qt::KeepEmptyParts);
                if (conditionParts.size() < 2
                    || conditionParts.size() > 4
                    || conditionParts.at(0).isEmpty()
                    || conditionParts.at(1).isEmpty()) {
                    return 28;
                }
                tgid::repo::SearchCondition condition;
                condition.fieldName = conditionParts.at(0);
                condition.comparison = conditionParts.at(1);
                if (conditionParts.size() >= 3) {
                    condition.value = conditionParts.at(2);
                }
                if (conditionParts.size() == 4) {
                    condition.secondValue = conditionParts.at(3);
                }
                const bool noValue =
                    condition.comparison == QStringLiteral("is_null")
                    || condition.comparison == QStringLiteral("not_null");
                if ((!noValue && conditionParts.size() < 3)
                    || (condition.comparison == QStringLiteral("between")
                        && conditionParts.size() < 4)) {
                    return 28;
                }
                criteria.conditions.append(condition);
            }
            if (criteria.conditions.isEmpty()
                || criteria.conditions.size() > 8) {
                qCritical().noquote()
                    << QStringLiteral("Ожидается от 1 до 8 условий поиска");
                return 28;
            }
            QString searchError;
            const QList<tgid::repo::SearchResult> results =
                tgid::repo::SearchRepository().search(
                    connection.database(), criteria, &searchError);
            if (!searchError.isEmpty()) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка поиска:") << searchError;
                return 29;
            }
            for (const tgid::repo::SearchResult& result : results) {
                qInfo().noquote()
                    << QStringLiteral("FOUND: id=%1 fragment=%2 value=%3 version=%4 archive=%5")
                           .arg(result.id)
                           .arg(result.fragmentId, result.value)
                           .arg(result.rowVersion)
                           .arg(result.archived
                                    ? QStringLiteral("да")
                                    : QStringLiteral("нет"));
            }
            qInfo().noquote()
                << QStringLiteral("OK: найдено=%1 лимит=%2")
                       .arg(results.size())
                       .arg(criteria.limit);
        }

        if (parser.isSet(pipeLengthReportOption)) {
            const QStringList parts =
                parser.value(pipeLengthReportOption).split(':');
            bool fragmentValid = true;
            const int fragmentId =
                parts.size() == 2
                    ? parts.at(1).toInt(&fragmentValid)
                    : 0;
            if (parts.isEmpty() || parts.size() > 2
                || parts.at(0).isEmpty() || !fragmentValid
                || fragmentId < 0) {
                qCritical().noquote()
                    << QStringLiteral(
                           "Ожидается --pipe-length-report grouping[:fragment_id]");
                return 30;
            }
            tgid::repo::PipeLengthReportCriteria criteria;
            criteria.grouping = parts.at(0);
            criteria.fragmentId = fragmentId;
            criteria.includeArchived = parser.isSet(reportArchivedOption);
            if (parser.isSet(reportPipeIdsOption)) {
                const QStringList ids = parser.value(reportPipeIdsOption)
                                            .split(',', Qt::SkipEmptyParts);
                bool idsValid = !ids.isEmpty();
                for (const QString& idText : ids) {
                    bool idValid = false;
                    const qint64 id = idText.toLongLong(&idValid);
                    if (!idValid || id <= 0) {
                        idsValid = false;
                        break;
                    }
                    criteria.pipeIds.append(id);
                }
                if (!idsValid) {
                    return 30;
                }
            }
            QString reportError;
            const QList<tgid::repo::PipeLengthReportRow> rows =
                tgid::repo::PipeLengthReportRepository().load(
                    connection.database(), criteria, &reportError);
            if (!reportError.isEmpty()) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка отчёта:") << reportError;
                return 31;
            }
            qint64 totalCount = 0;
            qint64 totalMissing = 0;
            double totalPassport = 0.0;
            double totalGeometry = 0.0;
            double totalEffective = 0.0;
            for (const tgid::repo::PipeLengthReportRow& row : rows) {
                totalCount += row.pipeCount;
                totalMissing += row.missingPassportCount;
                totalPassport += row.passportLength;
                totalGeometry += row.geometryLength;
                totalEffective += row.effectiveLength;
                qInfo().noquote()
                    << QStringLiteral(
                           "ROW: %1 count=%2 passport=%3 geometry=%4 effective=%5 missing=%6")
                           .arg(row.groupLabel)
                           .arg(row.pipeCount)
                           .arg(row.passportLength, 0, 'f', 2)
                           .arg(row.geometryLength, 0, 'f', 2)
                           .arg(row.effectiveLength, 0, 'f', 2)
                           .arg(row.missingPassportCount);
            }
            qInfo().noquote()
                << QStringLiteral(
                       "OK: групп=%1 участков=%2 паспорт=%3 геометрия=%4 рабочая=%5 без_паспорта=%6")
                       .arg(rows.size())
                       .arg(totalCount)
                       .arg(totalPassport, 0, 'f', 2)
                       .arg(totalGeometry, 0, 'f', 2)
                       .arg(totalEffective, 0, 'f', 2)
                       .arg(totalMissing);
        }

        if (parser.isSet(pipeVolumeReportOption)) {
            const QStringList parts =
                parser.value(pipeVolumeReportOption).split(':');
            bool fragmentValid = true;
            const int fragmentId =
                parts.size() == 2
                    ? parts.at(1).toInt(&fragmentValid)
                    : 0;
            if (parts.isEmpty() || parts.size() > 2
                || parts.at(0).isEmpty() || !fragmentValid
                || fragmentId < 0) {
                qCritical().noquote()
                    << QStringLiteral(
                           "Ожидается --pipe-volume-report grouping[:fragment_id]");
                return 38;
            }
            tgid::repo::PipeVolumeReportCriteria criteria;
            criteria.grouping = parts.at(0);
            criteria.fragmentId = fragmentId;
            criteria.includeArchived = parser.isSet(reportArchivedOption);
            if (parser.isSet(reportPipeIdsOption)) {
                const QStringList ids = parser.value(reportPipeIdsOption)
                                            .split(',', Qt::SkipEmptyParts);
                bool idsValid = !ids.isEmpty();
                for (const QString& idText : ids) {
                    bool idValid = false;
                    const qint64 id = idText.toLongLong(&idValid);
                    if (!idValid || id <= 0) {
                        idsValid = false;
                        break;
                    }
                    criteria.pipeIds.append(id);
                }
                if (!idsValid) {
                    return 38;
                }
            }
            QString reportError;
            const QList<tgid::repo::PipeVolumeReportRow> rows =
                tgid::repo::PipeVolumeReportRepository().load(
                    connection.database(), criteria, &reportError);
            if (!reportError.isEmpty()) {
                qCritical().noquote()
                    << QStringLiteral("Ошибка отчёта объёма:")
                    << reportError;
                return 39;
            }
            qint64 totalCount = 0;
            qint64 totalCalculated = 0;
            qint64 totalMissingDiameter = 0;
            qint64 totalMissingLength = 0;
            double totalLength = 0.0;
            double totalVolume = 0.0;
            for (const tgid::repo::PipeVolumeReportRow& row : rows) {
                totalCount += row.pipeCount;
                totalCalculated += row.calculatedCount;
                totalMissingDiameter += row.missingDiameterCount;
                totalMissingLength += row.missingLengthCount;
                totalLength += row.passportLength;
                totalVolume += row.volume;
                qInfo().noquote()
                    << QStringLiteral(
                           "ROW_VOLUME: %1 count=%2 calculated=%3 length=%4 volume=%5 missing_diameter=%6 missing_length=%7")
                           .arg(row.groupLabel)
                           .arg(row.pipeCount)
                           .arg(row.calculatedCount)
                           .arg(row.passportLength, 0, 'f', 2)
                           .arg(row.volume, 0, 'f', 6)
                           .arg(row.missingDiameterCount)
                           .arg(row.missingLengthCount);
            }
            qInfo().noquote()
                << QStringLiteral(
                       "OK_VOLUME: групп=%1 участков=%2 рассчитано=%3 длина=%4 объём=%5 без_диаметра=%6 без_длины=%7")
                       .arg(rows.size())
                       .arg(totalCount)
                       .arg(totalCalculated)
                       .arg(totalLength, 0, 'f', 2)
                       .arg(totalVolume, 0, 'f', 6)
                       .arg(totalMissingDiameter)
                       .arg(totalMissingLength);
        }
        return 0;
    }

    tgid::ui::MainWindow window;
    window.show();
    return application.exec();
}
