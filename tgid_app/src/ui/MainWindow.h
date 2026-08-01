#pragma once

#include "db/DatabaseConfig.h"
#include "db/DatabaseConnection.h"
#include "repo/FragmentRepository.h"
#include "repo/LayerCatalogRepository.h"
#include "repo/MapRepository.h"
#include "repo/ObjectRepository.h"
#include "repo/SearchRepository.h"

#include <QElapsedTimer>
#include <QFutureWatcher>
#include <QMainWindow>

class QLabel;
class QDockWidget;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QPushButton;
class QTableWidget;
class QTreeWidget;

namespace tgid::ui {

class MapView;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void connectAndRefresh();
    void loadSelectedFragment();
    void finishMapLoad();
    void showObjectDetails(qint64 id, QString classTable, bool isNode);
    void clearObjectDetails();
    void saveObjectDetails();
    void reloadObjectDetails();
    void toggleObjectArchive();
    void showObjectHistory();
    void refreshArchive();
    void openArchivedObject(int row, int column);
    void togglePointCreation(bool enabled);
    void createPointAt(QPointF position);
    void toggleLineCreation(bool enabled);
    void createLineBetween(qint64 nodeFrom, qint64 nodeTo);
    void showLineStart(qint64 nodeId);
    void toggleLineSplit(bool enabled);
    void splitLineAt(QPointF position);
    void toggleLineJoin(bool enabled);
    void joinWithLine(qint64 secondId, QString classTable);
    void showMultipleSelection(int count, QString classTable, bool isNode);
    void batchEditSelectedObjects();
    void refreshSearchFields();
    void refreshSearchEditor();
    void refreshSearchValueInputs();
    void executeSearch();
    void openSearchResult(int row, int column);

private:
    void buildInterface();
    void displayObjectDetails(
        const repo::ObjectDetails& details,
        bool isNode);
    void populatePointClasses(const QList<repo::LayerInfo>& layers);
    void populateLineClasses(const QList<repo::LayerInfo>& layers);
    void populateFragments(const QList<repo::FragmentInfo>& fragments);
    void populateSearchClasses(const QList<repo::LayerInfo>& layers);
    void showError(const QString& message);
    void showLayers(
        const QList<repo::LayerInfo>& layers,
        const db::SchemaStatus& status);

    db::DatabaseConfig config_;
    db::DatabaseConnection connection_;
    repo::FragmentRepository fragmentRepository_;
    repo::LayerCatalogRepository layerRepository_;
    repo::ObjectRepository objectRepository_;
    repo::SearchRepository searchRepository_;

    QLabel* connectionLabel_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QLabel* mapStatusLabel_ = nullptr;
    QPushButton* refreshButton_ = nullptr;
    QPushButton* fitMapButton_ = nullptr;
    QComboBox* pointClassCombo_ = nullptr;
    QPushButton* createPointButton_ = nullptr;
    QComboBox* lineClassCombo_ = nullptr;
    QPushButton* createLineButton_ = nullptr;
    QPushButton* splitLineButton_ = nullptr;
    QPushButton* joinLinesButton_ = nullptr;
    QPushButton* batchEditButton_ = nullptr;
    QTreeWidget* fragmentTree_ = nullptr;
    QTableWidget* layerTable_ = nullptr;
    QTableWidget* archiveTable_ = nullptr;
    QPushButton* refreshArchiveButton_ = nullptr;
    QComboBox* searchClassCombo_ = nullptr;
    QComboBox* searchFieldCombo_ = nullptr;
    QComboBox* searchOperatorCombo_ = nullptr;
    QLineEdit* searchValueEdit_ = nullptr;
    QLineEdit* searchSecondValueEdit_ = nullptr;
    QComboBox* searchValueCombo_ = nullptr;
    QCheckBox* searchArchivedCheck_ = nullptr;
    QPushButton* searchButton_ = nullptr;
    QLabel* searchStatusLabel_ = nullptr;
    QTableWidget* searchTable_ = nullptr;
    QDockWidget* objectDock_ = nullptr;
    QLabel* objectTitleLabel_ = nullptr;
    QTableWidget* objectTable_ = nullptr;
    QPushButton* saveObjectButton_ = nullptr;
    QPushButton* reloadObjectButton_ = nullptr;
    QPushButton* archiveObjectButton_ = nullptr;
    QPushButton* historyObjectButton_ = nullptr;
    MapView* mapView_ = nullptr;
    QFutureWatcher<repo::MapData>* mapWatcher_ = nullptr;
    QElapsedTimer mapLoadTimer_;
    repo::ObjectDetails currentObjectDetails_;
    QList<repo::SearchField> searchFields_;
    bool currentObjectIsNode_ = false;
    bool preserveObjectAfterMapLoad_ = false;
    qint64 pendingObjectId_ = 0;
    QString pendingObjectClassTable_;
    bool pendingObjectIsNode_ = false;
};

}  // namespace tgid::ui
