#include "ui/MainWindow.h"

#include "ui/MapView.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QComboBox>
#include <QCheckBox>
#include <QCompleter>
#include <QDate>
#include <QDateEdit>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QFont>
#include <QFileDialog>
#include <QHash>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSaveFile>
#include <QSplitter>
#include <QStatusBar>
#include <QStringConverter>
#include <QStyledItemDelegate>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QtConcurrentRun>

#include <utility>

namespace tgid::ui {
namespace {

enum ObjectItemRole {
    AttributeNameRole = Qt::UserRole,
    DatabaseTypeRole,
    OriginalValueRole,
    OriginalNullRole,
    EditableRole,
    EditorKindRole,
    OptionValuesRole,
    OptionLabelsRole,
    CurrentValueRole,
    CurrentNullRole,
};

class ObjectValueDelegate final : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QWidget* createEditor(
        QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override
    {
        const QString editorKind =
            index.data(EditorKindRole).toString();
        if (editorKind == QStringLiteral("lookup")
            || editorKind == QStringLiteral("boolean")) {
            auto* combo = new QComboBox(parent);
            combo->setMaxVisibleItems(20);
            combo->addItem(QStringLiteral("NULL"), QString());
            if (editorKind == QStringLiteral("boolean")) {
                combo->addItem(QStringLiteral("Да"), QStringLiteral("да"));
                combo->addItem(QStringLiteral("Нет"), QStringLiteral("нет"));
            } else {
                combo->setEditable(true);
                combo->setInsertPolicy(QComboBox::NoInsert);
                combo->lineEdit()->setPlaceholderText(
                    QStringLiteral("Введите часть названия"));
                combo->completer()->setCaseSensitivity(Qt::CaseInsensitive);
                combo->completer()->setFilterMode(Qt::MatchContains);
                combo->completer()->setCompletionMode(
                    QCompleter::PopupCompletion);
                const QStringList values =
                    index.data(OptionValuesRole).toStringList();
                const QStringList labels =
                    index.data(OptionLabelsRole).toStringList();
                for (qsizetype i = 0; i < values.size(); ++i) {
                    combo->addItem(
                        i < labels.size() ? labels.at(i) : values.at(i),
                        values.at(i));
                }
            }
            return combo;
        }
        if (editorKind == QStringLiteral("integer")
            || editorKind == QStringLiteral("decimal")) {
            auto* lineEdit = new QLineEdit(parent);
            const QString pattern =
                editorKind == QStringLiteral("integer")
                    ? QStringLiteral("^-?[0-9]*$")
                    : QStringLiteral(
                          "^-?[0-9]*([\\.,][0-9]*)?"
                          "([eE][+-]?[0-9]*)?$");
            lineEdit->setValidator(new QRegularExpressionValidator(
                QRegularExpression(pattern), lineEdit));
            lineEdit->setAlignment(Qt::AlignRight);
            lineEdit->setProperty("tgidEditorKind", editorKind);
            return lineEdit;
        }
        if (editorKind == QStringLiteral("date")) {
            auto* dateEdit = new QDateEdit(parent);
            dateEdit->setCalendarPopup(true);
            dateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
            dateEdit->setMinimumDate(QDate(1752, 1, 1));
            dateEdit->setMaximumDate(QDate(9999, 12, 31));
            dateEdit->setSpecialValueText(QStringLiteral("NULL"));
            return dateEdit;
        }
        if (editorKind == QStringLiteral("datetime")) {
            auto* dateTimeEdit = new QDateTimeEdit(parent);
            dateTimeEdit->setCalendarPopup(true);
            dateTimeEdit->setDisplayFormat(
                QStringLiteral("yyyy-MM-dd HH:mm:ss"));
            dateTimeEdit->setMinimumDateTime(
                QDateTime(QDate(1752, 1, 1), QTime(0, 0)));
            dateTimeEdit->setMaximumDateTime(
                QDateTime(QDate(9999, 12, 31), QTime(23, 59, 59)));
            dateTimeEdit->setSpecialValueText(QStringLiteral("NULL"));
            return dateTimeEdit;
        }
        if (editorKind == QStringLiteral("multiline")) {
            return new QPlainTextEdit(parent);
        }
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    void setEditorData(
        QWidget* editor,
        const QModelIndex& index) const override
    {
        if (auto* combo = qobject_cast<QComboBox*>(editor)) {
            if (index.data(CurrentNullRole).toBool()) {
                combo->setCurrentIndex(0);
                return;
            }
            const QString current =
                index.data(CurrentValueRole).toString();
            int optionIndex = combo->findData(current);
            if (optionIndex < 0) {
                combo->addItem(
                    QStringLiteral("%1 (нет в справочнике)").arg(current),
                    current);
                optionIndex = combo->count() - 1;
            }
            combo->setCurrentIndex(optionIndex);
            return;
        }
        if (auto* dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor)) {
            if (index.data(CurrentNullRole).toBool()) {
                dateTimeEdit->setDateTime(dateTimeEdit->minimumDateTime());
                return;
            }
            QDateTime dateTime = QDateTime::fromString(
                index.data(CurrentValueRole).toString(), Qt::ISODate);
            if (!dateTime.isValid()) {
                dateTime = QDateTime::fromString(
                    index.data(CurrentValueRole).toString(),
                    QStringLiteral("yyyy-MM-dd HH:mm:ss"));
            }
            dateTimeEdit->setDateTime(
                dateTime.isValid() ? dateTime
                                   : dateTimeEdit->minimumDateTime());
            return;
        }
        if (auto* dateEdit = qobject_cast<QDateEdit*>(editor)) {
            if (index.data(CurrentNullRole).toBool()) {
                dateEdit->setDate(dateEdit->minimumDate());
                return;
            }
            const QDate date = QDate::fromString(
                index.data(CurrentValueRole).toString().left(10),
                Qt::ISODate);
            dateEdit->setDate(date.isValid() ? date : dateEdit->minimumDate());
            return;
        }
        if (auto* lineEdit = qobject_cast<QLineEdit*>(editor);
            lineEdit != nullptr
            && lineEdit->property("tgidEditorKind").isValid()) {
            lineEdit->setText(index.data(Qt::EditRole).toString());
            lineEdit->selectAll();
            return;
        }
        if (auto* textEdit = qobject_cast<QPlainTextEdit*>(editor)) {
            textEdit->setPlainText(index.data(Qt::EditRole).toString());
            return;
        }
        QStyledItemDelegate::setEditorData(editor, index);
    }

    void setModelData(
        QWidget* editor,
        QAbstractItemModel* model,
        const QModelIndex& index) const override
    {
        if (auto* combo = qobject_cast<QComboBox*>(editor)) {
            if (index.data(EditorKindRole).toString()
                    == QStringLiteral("lookup")
                && combo->currentIndex() < 0) {
                const int matchingIndex = combo->findText(
                    combo->currentText(), Qt::MatchFixedString);
                if (matchingIndex < 0) {
                    QApplication::beep();
                    return;
                }
                combo->setCurrentIndex(matchingIndex);
            }
            const bool isNull = combo->currentIndex() == 0;
            const QString value =
                isNull ? QString() : combo->currentData().toString();
            model->setData(index, isNull, CurrentNullRole);
            model->setData(index, value, CurrentValueRole);
            model->setData(
                index,
                isNull ? QStringLiteral("NULL") : combo->currentText(),
                Qt::EditRole);
            return;
        }
        if (auto* dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor)) {
            const bool isNull =
                dateTimeEdit->dateTime() == dateTimeEdit->minimumDateTime();
            const QString value =
                isNull
                    ? QString()
                    : dateTimeEdit->dateTime().toString(Qt::ISODate);
            model->setData(index, isNull, CurrentNullRole);
            model->setData(index, value, CurrentValueRole);
            model->setData(
                index,
                isNull ? QStringLiteral("NULL") : value,
                Qt::EditRole);
            return;
        }
        if (auto* dateEdit = qobject_cast<QDateEdit*>(editor)) {
            const bool isNull = dateEdit->date() == dateEdit->minimumDate();
            model->setData(index, isNull, CurrentNullRole);
            model->setData(
                index,
                isNull ? QString() : dateEdit->date().toString(Qt::ISODate),
                CurrentValueRole);
            model->setData(
                index,
                isNull ? QStringLiteral("NULL")
                       : dateEdit->date().toString(Qt::ISODate),
                Qt::EditRole);
            return;
        }
        if (auto* lineEdit = qobject_cast<QLineEdit*>(editor);
            lineEdit != nullptr
            && lineEdit->property("tgidEditorKind").isValid()) {
            QString value = lineEdit->text().trimmed();
            if (lineEdit->property("tgidEditorKind").toString()
                == QStringLiteral("decimal")) {
                value.replace(',', '.');
            }
            model->setData(index, value, Qt::EditRole);
            return;
        }
        if (auto* textEdit = qobject_cast<QPlainTextEdit*>(editor)) {
            model->setData(index, textEdit->toPlainText(), Qt::EditRole);
            return;
        }
        QStyledItemDelegate::setModelData(editor, model, index);
    }
};

QTableWidgetItem* readOnlyItem(const QString& text)
{
    auto* item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

QString csvCell(QString value)
{
    value.replace('"', QStringLiteral("\"\""));
    return QStringLiteral("\"%1\"").arg(value);
}

}  // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , config_(db::DatabaseConfig::fromEnvironment())
{
    buildInterface();
    mapWatcher_ = new QFutureWatcher<repo::MapData>(this);
    connect(mapWatcher_, &QFutureWatcher<repo::MapData>::finished,
            this, &MainWindow::finishMapLoad);
    QTimer::singleShot(0, this, &MainWindow::connectAndRefresh);
}

void MainWindow::buildInterface()
{
    setWindowTitle(QStringLiteral("ТГИД — объектная GIS-БД"));
    resize(1280, 780);

    auto* centralWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(centralWidget);

    auto* title = new QLabel(
        QStringLiteral("<h2>ТГИД</h2>"
                       "<p>Новый клиент для схем net / ref / meta</p>"),
        centralWidget);
    layout->addWidget(title);

    connectionLabel_ = new QLabel(
        QStringLiteral("Подключение: %1").arg(config_.displayName()),
        centralWidget);
    connectionLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout->addWidget(connectionLabel_);

    statusLabel_ = new QLabel(QStringLiteral("Проверка БД…"), centralWidget);
    statusLabel_->setWordWrap(true);
    layout->addWidget(statusLabel_);

    refreshButton_ =
        new QPushButton(QStringLiteral("Подключиться / обновить"), centralWidget);
    connect(refreshButton_, &QPushButton::clicked,
            this, &MainWindow::connectAndRefresh);
    layout->addWidget(refreshButton_, 0, Qt::AlignLeft);

    auto* tabs = new QTabWidget(centralWidget);

    auto* mapTab = new QWidget(tabs);
    auto* mapTabLayout = new QVBoxLayout(mapTab);
    mapTabLayout->setContentsMargins(0, 0, 0, 0);
    auto* splitter = new QSplitter(Qt::Horizontal, mapTab);

    auto* fragmentPanel = new QWidget(splitter);
    auto* fragmentLayout = new QVBoxLayout(fragmentPanel);
    fragmentLayout->setContentsMargins(8, 8, 8, 8);
    fragmentLayout->addWidget(
        new QLabel(QStringLiteral("<b>Фрагменты сети</b>"), fragmentPanel));
    fragmentTree_ = new QTreeWidget(fragmentPanel);
    fragmentTree_->setHeaderHidden(true);
    fragmentTree_->setMinimumWidth(250);
    fragmentTree_->setUniformRowHeights(true);
    fragmentTree_->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(fragmentTree_, &QTreeWidget::itemSelectionChanged,
            this, &MainWindow::loadSelectedFragment);
    fragmentLayout->addWidget(fragmentTree_, 1);
    splitter->addWidget(fragmentPanel);

    auto* mapPanel = new QWidget(splitter);
    auto* mapLayout = new QVBoxLayout(mapPanel);
    mapLayout->setContentsMargins(8, 8, 8, 8);
    auto* mapToolbar = new QHBoxLayout();
    mapStatusLabel_ =
        new QLabel(QStringLiteral("Фрагмент не выбран"), mapPanel);
    mapToolbar->addWidget(mapStatusLabel_, 1);
    pointClassCombo_ = new QComboBox(mapPanel);
    pointClassCombo_->setMinimumWidth(145);
    pointClassCombo_->setToolTip(QStringLiteral("Класс нового узла"));
    pointClassCombo_->setEnabled(false);
    mapToolbar->addWidget(pointClassCombo_);
    createPointButton_ =
        new QPushButton(QStringLiteral("Создать точку"), mapPanel);
    createPointButton_->setCheckable(true);
    createPointButton_->setEnabled(false);
    connect(createPointButton_, &QPushButton::toggled,
            this, &MainWindow::togglePointCreation);
    mapToolbar->addWidget(createPointButton_);
    lineClassCombo_ = new QComboBox(mapPanel);
    lineClassCombo_->setMinimumWidth(145);
    lineClassCombo_->setToolTip(QStringLiteral("Класс новой линии"));
    lineClassCombo_->setEnabled(false);
    mapToolbar->addWidget(lineClassCombo_);
    createLineButton_ =
        new QPushButton(QStringLiteral("Создать линию"), mapPanel);
    createLineButton_->setCheckable(true);
    createLineButton_->setEnabled(false);
    connect(createLineButton_, &QPushButton::toggled,
            this, &MainWindow::toggleLineCreation);
    mapToolbar->addWidget(createLineButton_);
    splitLineButton_ =
        new QPushButton(QStringLiteral("Разрезать линию"), mapPanel);
    splitLineButton_->setCheckable(true);
    splitLineButton_->setEnabled(false);
    connect(splitLineButton_, &QPushButton::toggled,
            this, &MainWindow::toggleLineSplit);
    mapToolbar->addWidget(splitLineButton_);
    joinLinesButton_ =
        new QPushButton(QStringLiteral("Соединить линии"), mapPanel);
    joinLinesButton_->setCheckable(true);
    joinLinesButton_->setEnabled(false);
    connect(joinLinesButton_, &QPushButton::toggled,
            this, &MainWindow::toggleLineJoin);
    mapToolbar->addWidget(joinLinesButton_);
    batchEditButton_ =
        new QPushButton(QStringLiteral("Массово изменить"), mapPanel);
    batchEditButton_->setEnabled(false);
    batchEditButton_->setToolTip(
        QStringLiteral(
            "Выберите Ctrl+щелчком несколько объектов одного класса"));
    connect(batchEditButton_, &QPushButton::clicked,
            this, &MainWindow::batchEditSelectedObjects);
    mapToolbar->addWidget(batchEditButton_);
    fitMapButton_ = new QPushButton(QStringLiteral("Показать целиком"), mapPanel);
    fitMapButton_->setEnabled(false);
    mapToolbar->addWidget(fitMapButton_);
    mapLayout->addLayout(mapToolbar);
    mapView_ = new MapView(mapPanel);
    connect(fitMapButton_, &QPushButton::clicked,
            mapView_, &MapView::fitToData);
    connect(mapView_, &MapView::objectSelected,
            this, &MainWindow::showObjectDetails);
    connect(mapView_, &MapView::objectSelectionCleared,
            this, &MainWindow::clearObjectDetails);
    connect(mapView_, &MapView::multipleObjectsSelected,
            this, &MainWindow::showMultipleSelection);
    connect(mapView_, &MapView::selectionLimitReached, this, [this]() {
        statusBar()->showMessage(
            QStringLiteral("За один раз можно выбрать не более 500 объектов"));
    });
    connect(mapView_, &MapView::pointPlacementRequested,
            this, &MainWindow::createPointAt);
    connect(mapView_, &MapView::lineStartSelected,
            this, &MainWindow::showLineStart);
    connect(mapView_, &MapView::linePlacementRequested,
            this, &MainWindow::createLineBetween);
    connect(mapView_, &MapView::lineEndpointMissed, this, [this]() {
        statusBar()->showMessage(
            QStringLiteral(
                "Выберите другой узел: щелчок должен попадать по точке"));
    });
    connect(mapView_, &MapView::lineSplitRequested,
            this, &MainWindow::splitLineAt);
    connect(mapView_, &MapView::lineJoinRequested,
            this, &MainWindow::joinWithLine);
    connect(mapView_, &MapView::lineJoinMissed, this, [this]() {
        statusBar()->showMessage(
            QStringLiteral(
                "Щелчок должен попадать по второму линейному участку"));
    });
    mapLayout->addWidget(mapView_, 1);
    splitter->addWidget(mapPanel);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({280, 920});
    mapTabLayout->addWidget(splitter);
    tabs->addTab(mapTab, QStringLiteral("Карта"));

    auto* catalogTab = new QWidget(tabs);
    auto* catalogLayout = new QVBoxLayout(catalogTab);
    layerTable_ = new QTableWidget(catalogTab);
    layerTable_->setColumnCount(7);
    layerTable_->setHorizontalHeaderLabels({
        QStringLiteral("Название"),
        QStringLiteral("Схема"),
        QStringLiteral("Таблица"),
        QStringLiteral("Геометрия"),
        QStringLiteral("SRID"),
        QStringLiteral("Редактирование"),
        QStringLiteral("Тип"),
    });
    layerTable_->setAlternatingRowColors(true);
    layerTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    layerTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    layerTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layerTable_->horizontalHeader()->setStretchLastSection(true);
    layerTable_->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    catalogLayout->addWidget(layerTable_, 1);
    tabs->addTab(catalogTab, QStringLiteral("GIS-слои"));

    auto* searchTab = new QWidget(tabs);
    auto* searchLayout = new QVBoxLayout(searchTab);
    auto* searchClassRow = new QHBoxLayout();
    searchClassRow->addWidget(new QLabel(QStringLiteral("Класс:"), searchTab));
    searchClassCombo_ = new QComboBox(searchTab);
    searchClassCombo_->setMinimumWidth(220);
    searchClassCombo_->setEnabled(false);
    connect(searchClassCombo_, &QComboBox::currentIndexChanged,
            this, &MainWindow::refreshSearchFields);
    searchClassRow->addWidget(searchClassCombo_);
    searchClassRow->addWidget(new QLabel(QStringLiteral("Поле:"), searchTab));
    searchFieldCombo_ = new QComboBox(searchTab);
    searchFieldCombo_->setMinimumWidth(220);
    searchFieldCombo_->setEnabled(false);
    connect(searchFieldCombo_, &QComboBox::currentIndexChanged,
            this, &MainWindow::refreshSearchEditor);
    searchClassRow->addWidget(searchFieldCombo_, 1);
    searchArchivedCheck_ = new QCheckBox(
        QStringLiteral("Включая архив"), searchTab);
    searchClassRow->addWidget(searchArchivedCheck_);
    searchLayout->addLayout(searchClassRow);

    auto* searchConditionRow = new QHBoxLayout();
    searchOperatorCombo_ = new QComboBox(searchTab);
    searchOperatorCombo_->setMinimumWidth(145);
    searchOperatorCombo_->setEnabled(false);
    connect(searchOperatorCombo_, &QComboBox::currentIndexChanged,
            this, &MainWindow::refreshSearchValueInputs);
    searchConditionRow->addWidget(searchOperatorCombo_);
    searchValueEdit_ = new QLineEdit(searchTab);
    searchValueEdit_->setClearButtonEnabled(true);
    searchConditionRow->addWidget(searchValueEdit_, 1);
    searchValueCombo_ = new QComboBox(searchTab);
    searchValueCombo_->setMinimumWidth(240);
    searchValueCombo_->setMaxVisibleItems(25);
    searchValueCombo_->setVisible(false);
    searchConditionRow->addWidget(searchValueCombo_, 1);
    searchSecondValueEdit_ = new QLineEdit(searchTab);
    searchSecondValueEdit_->setClearButtonEnabled(true);
    searchSecondValueEdit_->setPlaceholderText(
        QStringLiteral("Верхняя граница"));
    searchSecondValueEdit_->setVisible(false);
    searchConditionRow->addWidget(searchSecondValueEdit_, 1);
    searchButton_ = new QPushButton(QStringLiteral("Найти"), searchTab);
    searchButton_->setEnabled(false);
    connect(searchButton_, &QPushButton::clicked,
            this, &MainWindow::executeSearch);
    connect(searchValueEdit_, &QLineEdit::returnPressed,
            searchButton_, &QPushButton::click);
    connect(searchSecondValueEdit_, &QLineEdit::returnPressed,
            searchButton_, &QPushButton::click);
    searchConditionRow->addWidget(searchButton_);
    searchLayout->addLayout(searchConditionRow);
    searchStatusLabel_ = new QLabel(
        QStringLiteral("Выберите класс и условие поиска"), searchTab);
    searchStatusLabel_->setWordWrap(true);
    searchLayout->addWidget(searchStatusLabel_);
    searchTable_ = new QTableWidget(searchTab);
    searchTable_->setColumnCount(6);
    searchTable_->setHorizontalHeaderLabels({
        QStringLiteral("ID"),
        QStringLiteral("Фрагмент"),
        QStringLiteral("Поле"),
        QStringLiteral("Значение"),
        QStringLiteral("Версия"),
        QStringLiteral("Состояние"),
    });
    searchTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    searchTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    searchTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    searchTable_->setAlternatingRowColors(true);
    searchTable_->setSortingEnabled(true);
    searchTable_->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    searchTable_->horizontalHeader()->setStretchLastSection(true);
    connect(searchTable_, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::openSearchResult);
    searchLayout->addWidget(searchTable_, 1);
    tabs->addTab(searchTab, QStringLiteral("Поиск"));

    auto* reportTab = new QWidget(tabs);
    auto* reportLayout = new QVBoxLayout(reportTab);
    auto* reportToolbar = new QHBoxLayout();
    reportToolbar->addWidget(
        new QLabel(QStringLiteral("Фрагмент:"), reportTab));
    reportFragmentCombo_ = new QComboBox(reportTab);
    reportFragmentCombo_->setMinimumWidth(240);
    reportFragmentCombo_->setEnabled(false);
    reportToolbar->addWidget(reportFragmentCombo_, 1);
    reportToolbar->addWidget(
        new QLabel(QStringLiteral("Группировка:"), reportTab));
    reportGroupingCombo_ = new QComboBox(reportTab);
    reportGroupingCombo_->addItem(
        QStringLiteral("Без группировки"), QStringLiteral("none"));
    reportGroupingCombo_->addItem(
        QStringLiteral("Условный диаметр"), QStringLiteral("diameter"));
    reportGroupingCombo_->addItem(
        QStringLiteral("Тип прокладки"), QStringLiteral("tubing_type"));
    reportGroupingCombo_->addItem(
        QStringLiteral("Материал изоляции"), QStringLiteral("insulation"));
    reportToolbar->addWidget(reportGroupingCombo_);
    reportArchivedCheck_ = new QCheckBox(
        QStringLiteral("Включая архив"), reportTab);
    reportToolbar->addWidget(reportArchivedCheck_);
    reportSelectedCheck_ = new QCheckBox(
        QStringLiteral("Только выбранные на карте"), reportTab);
    reportSelectedCheck_->setToolTip(
        QStringLiteral(
            "На карте должны быть выбраны участки pipe_section"));
    connect(reportSelectedCheck_, &QCheckBox::toggled,
            reportFragmentCombo_, &QComboBox::setDisabled);
    reportToolbar->addWidget(reportSelectedCheck_);
    runPipeLengthReportButton_ = new QPushButton(
        QStringLiteral("Сформировать"), reportTab);
    runPipeLengthReportButton_->setEnabled(false);
    connect(runPipeLengthReportButton_, &QPushButton::clicked,
            this, &MainWindow::executePipeLengthReport);
    reportToolbar->addWidget(runPipeLengthReportButton_);
    exportPipeLengthReportButton_ = new QPushButton(
        QStringLiteral("Экспорт CSV"), reportTab);
    exportPipeLengthReportButton_->setEnabled(false);
    connect(exportPipeLengthReportButton_, &QPushButton::clicked,
            this, &MainWindow::exportPipeLengthReport);
    reportToolbar->addWidget(exportPipeLengthReportButton_);
    reportLayout->addLayout(reportToolbar);
    pipeLengthReportStatusLabel_ = new QLabel(
        QStringLiteral("Сформируйте отчёт по активным pipe_section"),
        reportTab);
    pipeLengthReportStatusLabel_->setWordWrap(true);
    reportLayout->addWidget(pipeLengthReportStatusLabel_);
    pipeLengthReportTable_ = new QTableWidget(reportTab);
    pipeLengthReportTable_->setColumnCount(7);
    pipeLengthReportTable_->setHorizontalHeaderLabels({
        QStringLiteral("Группа"),
        QStringLiteral("Участков"),
        QStringLiteral("Паспортная, м"),
        QStringLiteral("По геометрии, м"),
        QStringLiteral("Рабочая, м"),
        QStringLiteral("Паспорт − геометрия, м"),
        QStringLiteral("Без паспортной длины"),
    });
    pipeLengthReportTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pipeLengthReportTable_->setSelectionBehavior(
        QAbstractItemView::SelectRows);
    pipeLengthReportTable_->setAlternatingRowColors(true);
    pipeLengthReportTable_->setSortingEnabled(true);
    pipeLengthReportTable_->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    pipeLengthReportTable_->horizontalHeader()->setStretchLastSection(true);
    reportLayout->addWidget(pipeLengthReportTable_, 1);
    tabs->addTab(reportTab, QStringLiteral("Протяжённость"));

    auto* archiveTab = new QWidget(tabs);
    auto* archiveLayout = new QVBoxLayout(archiveTab);
    auto* archiveToolbar = new QHBoxLayout();
    archiveToolbar->addWidget(
        new QLabel(
            QStringLiteral(
                "Дважды щёлкните объект, чтобы открыть карточку"),
            archiveTab),
        1);
    refreshArchiveButton_ =
        new QPushButton(QStringLiteral("Обновить архив"), archiveTab);
    connect(refreshArchiveButton_, &QPushButton::clicked,
            this, &MainWindow::refreshArchive);
    archiveToolbar->addWidget(refreshArchiveButton_);
    archiveLayout->addLayout(archiveToolbar);
    archiveTable_ = new QTableWidget(archiveTab);
    archiveTable_->setColumnCount(5);
    archiveTable_->setHorizontalHeaderLabels({
        QStringLiteral("Тип"),
        QStringLiteral("ID"),
        QStringLiteral("Фрагмент"),
        QStringLiteral("Название"),
        QStringLiteral("В архиве с"),
    });
    archiveTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    archiveTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    archiveTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    archiveTable_->setAlternatingRowColors(true);
    archiveTable_->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    archiveTable_->horizontalHeader()->setStretchLastSection(true);
    connect(archiveTable_, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::openArchivedObject);
    archiveLayout->addWidget(archiveTable_, 1);
    tabs->addTab(archiveTab, QStringLiteral("Архив"));
    layout->addWidget(tabs, 1);

    setCentralWidget(centralWidget);

    objectDock_ = new QDockWidget(QStringLiteral("Карточка объекта"), this);
    objectDock_->setAllowedAreas(
        Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    objectDock_->setMinimumWidth(330);
    auto* objectPanel = new QWidget(objectDock_);
    auto* objectLayout = new QVBoxLayout(objectPanel);
    objectTitleLabel_ =
        new QLabel(QStringLiteral("Выберите объект на карте"), objectPanel);
    objectTitleLabel_->setWordWrap(true);
    objectLayout->addWidget(objectTitleLabel_);
    objectTable_ = new QTableWidget(objectPanel);
    objectTable_->setColumnCount(2);
    objectTable_->setHorizontalHeaderLabels({
        QStringLiteral("Поле"),
        QStringLiteral("Значение"),
    });
    objectTable_->setEditTriggers(
        QAbstractItemView::DoubleClicked
        | QAbstractItemView::EditKeyPressed
        | QAbstractItemView::SelectedClicked);
    objectTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    objectTable_->setAlternatingRowColors(true);
    objectTable_->setItemDelegateForColumn(
        1, new ObjectValueDelegate(objectTable_));
    objectTable_->verticalHeader()->setVisible(false);
    objectTable_->horizontalHeader()->setSectionsClickable(false);
    objectTable_->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents);
    objectTable_->horizontalHeader()->setStretchLastSection(true);
    objectLayout->addWidget(objectTable_, 1);
    auto* objectButtons = new QHBoxLayout();
    reloadObjectButton_ =
        new QPushButton(QStringLiteral("Перезагрузить"), objectPanel);
    reloadObjectButton_->setEnabled(false);
    connect(reloadObjectButton_, &QPushButton::clicked,
            this, &MainWindow::reloadObjectDetails);
    objectButtons->addWidget(reloadObjectButton_);
    saveObjectButton_ =
        new QPushButton(QStringLiteral("Сохранить"), objectPanel);
    saveObjectButton_->setEnabled(false);
    connect(saveObjectButton_, &QPushButton::clicked,
            this, &MainWindow::saveObjectDetails);
    objectButtons->addWidget(saveObjectButton_);
    objectLayout->addLayout(objectButtons);
    auto* lifecycleButtons = new QHBoxLayout();
    historyObjectButton_ =
        new QPushButton(QStringLiteral("История"), objectPanel);
    historyObjectButton_->setEnabled(false);
    connect(historyObjectButton_, &QPushButton::clicked,
            this, &MainWindow::showObjectHistory);
    lifecycleButtons->addWidget(historyObjectButton_);
    archiveObjectButton_ =
        new QPushButton(QStringLiteral("В архив"), objectPanel);
    archiveObjectButton_->setEnabled(false);
    connect(archiveObjectButton_, &QPushButton::clicked,
            this, &MainWindow::toggleObjectArchive);
    lifecycleButtons->addWidget(archiveObjectButton_);
    objectLayout->addLayout(lifecycleButtons);
    objectDock_->setWidget(objectPanel);
    addDockWidget(Qt::RightDockWidgetArea, objectDock_);

    statusBar()->showMessage(QStringLiteral("Готово к подключению"));
}

void MainWindow::connectAndRefresh()
{
    refreshButton_->setEnabled(false);
    statusLabel_->setStyleSheet({});
    statusLabel_->setText(QStringLiteral("Подключение и проверка схемы…"));
    QApplication::processEvents();

    QString error;
    if (!connection_.open(config_, &error)) {
        showError(error);
        return;
    }

    const db::SchemaStatus schemaStatus = connection_.inspectSchema();
    if (!schemaStatus.valid) {
        showError(schemaStatus.error);
        return;
    }

    const QList<repo::LayerInfo> layers =
        layerRepository_.load(connection_.database(), &error);
    if (!error.isEmpty()) {
        showError(error);
        return;
    }

    const QList<repo::FragmentInfo> fragments =
        fragmentRepository_.loadActive(connection_.database(), &error);
    if (!error.isEmpty()) {
        showError(error);
        return;
    }

    showLayers(layers, schemaStatus);
    populatePointClasses(layers);
    populateLineClasses(layers);
    populateSearchClasses(layers);
    populateFragments(fragments);
    populateReportFragments(fragments);
    refreshArchive();
    statusLabel_->setText(
        statusLabel_->text()
        + QStringLiteral(" Фрагментов: %1.").arg(fragments.size()));
}

void MainWindow::showError(const QString& message)
{
    fragmentTree_->clear();
    layerTable_->setRowCount(0);
    archiveTable_->setRowCount(0);
    searchTable_->setRowCount(0);
    searchClassCombo_->clear();
    searchClassCombo_->setEnabled(false);
    searchFieldCombo_->clear();
    searchFieldCombo_->setEnabled(false);
    searchOperatorCombo_->clear();
    searchOperatorCombo_->setEnabled(false);
    searchButton_->setEnabled(false);
    searchStatusLabel_->setText(QStringLiteral("Поиск недоступен"));
    pipeLengthReportTable_->setRowCount(0);
    pipeLengthReportRows_.clear();
    reportFragmentCombo_->clear();
    reportFragmentCombo_->setEnabled(false);
    runPipeLengthReportButton_->setEnabled(false);
    exportPipeLengthReportButton_->setEnabled(false);
    pipeLengthReportStatusLabel_->setText(
        QStringLiteral("Отчёт недоступен"));
    mapView_->clearMap();
    clearObjectDetails();
    mapStatusLabel_->setText(QStringLiteral("Карта недоступна"));
    fitMapButton_->setEnabled(false);
    createPointButton_->setChecked(false);
    createPointButton_->setEnabled(false);
    pointClassCombo_->clear();
    pointClassCombo_->setEnabled(false);
    createLineButton_->setChecked(false);
    createLineButton_->setEnabled(false);
    splitLineButton_->setChecked(false);
    splitLineButton_->setEnabled(false);
    joinLinesButton_->setChecked(false);
    joinLinesButton_->setEnabled(false);
    lineClassCombo_->clear();
    lineClassCombo_->setEnabled(false);
    statusLabel_->setStyleSheet(QStringLiteral("color: #b42318;"));
    statusLabel_->setText(QStringLiteral("Ошибка: %1").arg(message));
    statusBar()->showMessage(QStringLiteral("Подключение не выполнено"));
    refreshButton_->setEnabled(true);
}

void MainWindow::populateFragments(
    const QList<repo::FragmentInfo>& fragments)
{
    fragmentTree_->clear();
    QHash<QString, QTreeWidgetItem*> settlementItems;

    for (const repo::FragmentInfo& fragment : fragments) {
        const QString settlement =
            fragment.settlement.trimmed().isEmpty()
                ? QStringLiteral("Без населённого пункта")
                : fragment.settlement.trimmed();
        QTreeWidgetItem* settlementItem = settlementItems.value(settlement);
        if (settlementItem == nullptr) {
            settlementItem = new QTreeWidgetItem(fragmentTree_, {settlement});
            settlementItem->setFlags(
                settlementItem->flags() & ~Qt::ItemIsSelectable);
            settlementItem->setExpanded(true);
            settlementItems.insert(settlement, settlementItem);
        }

        QString title = fragment.name;
        QStringList details;
        if (!fragment.season.isEmpty()) {
            details.append(fragment.season);
        }
        if (!fragment.year.isEmpty()) {
            details.append(fragment.year);
        }
        if (!details.isEmpty()) {
            title += QStringLiteral(" (%1)").arg(details.join(QStringLiteral(", ")));
        }

        auto* fragmentItem =
            new QTreeWidgetItem(settlementItem, {title});
        fragmentItem->setData(0, Qt::UserRole, fragment.id);
        fragmentItem->setToolTip(
            0, QStringLiteral("ID: %1\nСистемное имя: %2")
                   .arg(fragment.id)
                   .arg(fragment.systemName));
    }

    mapStatusLabel_->setText(
        QStringLiteral("Выберите фрагмент слева"));
}

void MainWindow::populatePointClasses(
    const QList<repo::LayerInfo>& layers)
{
    pointClassCombo_->clear();
    for (const repo::LayerInfo& layer : layers) {
        if (layer.schemaName != QStringLiteral("net")
            || layer.geometryType.toUpper() != QStringLiteral("POINT")
            || !layer.editable
            || !layer.supportsFragment) {
            continue;
        }
        pointClassCombo_->addItem(
            QStringLiteral("%1 (%2)")
                .arg(layer.displayName, layer.tableName),
            layer.tableName);
    }
    pointClassCombo_->setEnabled(pointClassCombo_->count() > 0);
}

void MainWindow::populateLineClasses(
    const QList<repo::LayerInfo>& layers)
{
    lineClassCombo_->clear();
    for (const repo::LayerInfo& layer : layers) {
        if (layer.schemaName != QStringLiteral("net")
            || layer.geometryType.toUpper() != QStringLiteral("LINESTRING")
            || !layer.editable
            || !layer.supportsFragment) {
            continue;
        }
        lineClassCombo_->addItem(
            QStringLiteral("%1 (%2)")
                .arg(layer.displayName, layer.tableName),
            layer.tableName);
    }
    lineClassCombo_->setEnabled(lineClassCombo_->count() > 0);
}

void MainWindow::populateSearchClasses(
    const QList<repo::LayerInfo>& layers)
{
    searchClassCombo_->blockSignals(true);
    searchClassCombo_->clear();
    for (const repo::LayerInfo& layer : layers) {
        if (layer.schemaName != QStringLiteral("net")) {
            continue;
        }
        searchClassCombo_->addItem(
            QStringLiteral("%1 (%2)")
                .arg(layer.displayName, layer.tableName),
            layer.tableName);
        searchClassCombo_->setItemData(
            searchClassCombo_->count() - 1,
            layer.geometryType.toUpper().contains(QStringLiteral("POINT")),
            Qt::UserRole + 1);
    }
    searchClassCombo_->blockSignals(false);
    searchClassCombo_->setEnabled(searchClassCombo_->count() > 0);
    refreshSearchFields();
}

void MainWindow::refreshSearchFields()
{
    searchFields_.clear();
    searchFieldCombo_->blockSignals(true);
    searchFieldCombo_->clear();
    searchFieldCombo_->blockSignals(false);
    searchOperatorCombo_->clear();
    searchTable_->setRowCount(0);
    searchButton_->setEnabled(false);
    const QString classTable = searchClassCombo_->currentData().toString();
    if (!connection_.isOpen() || classTable.isEmpty()) {
        searchFieldCombo_->setEnabled(false);
        return;
    }

    QString error;
    searchFields_ = searchRepository_.loadFields(
        connection_.database(), classTable, &error);
    if (!error.isEmpty()) {
        searchFieldCombo_->setEnabled(false);
        searchStatusLabel_->setStyleSheet(QStringLiteral("color: #b42318;"));
        searchStatusLabel_->setText(
            QStringLiteral("Ошибка полей поиска: %1").arg(error));
        return;
    }
    searchFieldCombo_->blockSignals(true);
    for (qsizetype index = 0; index < searchFields_.size(); ++index) {
        const repo::SearchField& field = searchFields_.at(index);
        searchFieldCombo_->addItem(
            field.unit.isEmpty()
                ? field.displayName
                : QStringLiteral("%1, %2").arg(field.displayName, field.unit),
            index);
    }
    searchFieldCombo_->blockSignals(false);
    searchFieldCombo_->setEnabled(!searchFields_.isEmpty());
    searchStatusLabel_->setStyleSheet({});
    searchStatusLabel_->setText(
        QStringLiteral("Доступно полей: %1").arg(searchFields_.size()));
    refreshSearchEditor();
}

void MainWindow::refreshSearchEditor()
{
    searchOperatorCombo_->blockSignals(true);
    searchOperatorCombo_->clear();
    searchValueCombo_->clear();
    searchValueCombo_->setEditable(false);
    searchValueEdit_->clear();
    searchSecondValueEdit_->clear();
    searchValueEdit_->setValidator(nullptr);
    searchSecondValueEdit_->setValidator(nullptr);
    const qsizetype fieldIndex =
        searchFieldCombo_->currentData().toLongLong();
    if (fieldIndex < 0 || fieldIndex >= searchFields_.size()) {
        searchOperatorCombo_->blockSignals(false);
        searchOperatorCombo_->setEnabled(false);
        searchButton_->setEnabled(false);
        return;
    }
    const repo::SearchField& field = searchFields_.at(fieldIndex);
    const bool textField =
        field.editorKind == QStringLiteral("text")
        || field.editorKind == QStringLiteral("multiline");
    const bool orderedField =
        field.editorKind == QStringLiteral("integer")
        || field.editorKind == QStringLiteral("decimal")
        || field.editorKind == QStringLiteral("date")
        || field.editorKind == QStringLiteral("datetime");
    if (textField) {
        searchOperatorCombo_->addItem(
            QStringLiteral("содержит"), QStringLiteral("contains"));
    }
    searchOperatorCombo_->addItem(
        QStringLiteral("равно"), QStringLiteral("equals"));
    if (orderedField) {
        searchOperatorCombo_->addItem(
            QStringLiteral("больше"), QStringLiteral("greater"));
        searchOperatorCombo_->addItem(
            QStringLiteral("меньше"), QStringLiteral("less"));
        searchOperatorCombo_->addItem(
            QStringLiteral("между"), QStringLiteral("between"));
    }
    searchOperatorCombo_->addItem(
        QStringLiteral("не заполнено"), QStringLiteral("is_null"));
    searchOperatorCombo_->addItem(
        QStringLiteral("заполнено"), QStringLiteral("not_null"));
    searchOperatorCombo_->blockSignals(false);
    searchOperatorCombo_->setEnabled(true);

    if (field.editorKind == QStringLiteral("boolean")) {
        searchValueCombo_->addItem(
            QStringLiteral("Да"), QStringLiteral("да"));
        searchValueCombo_->addItem(
            QStringLiteral("Нет"), QStringLiteral("нет"));
    } else if (field.editorKind == QStringLiteral("lookup")) {
        searchValueCombo_->setEditable(true);
        searchValueCombo_->setInsertPolicy(QComboBox::NoInsert);
        searchValueCombo_->completer()->setCaseSensitivity(
            Qt::CaseInsensitive);
        searchValueCombo_->completer()->setFilterMode(Qt::MatchContains);
        searchValueCombo_->completer()->setCompletionMode(
            QCompleter::PopupCompletion);
        for (const repo::ObjectFieldOption& option : field.options) {
            searchValueCombo_->addItem(option.label, option.value);
        }
    }
    if (field.editorKind == QStringLiteral("integer")
        || field.editorKind == QStringLiteral("decimal")) {
        const QString pattern =
            field.editorKind == QStringLiteral("integer")
                ? QStringLiteral("^-?[0-9]*$")
                : QStringLiteral(
                      "^-?[0-9]*([\\.,][0-9]*)?"
                      "([eE][+-]?[0-9]*)?$");
        searchValueEdit_->setValidator(new QRegularExpressionValidator(
            QRegularExpression(pattern), searchValueEdit_));
        searchSecondValueEdit_->setValidator(
            new QRegularExpressionValidator(
                QRegularExpression(pattern), searchSecondValueEdit_));
    }
    if (field.editorKind == QStringLiteral("date")) {
        searchValueEdit_->setPlaceholderText(QStringLiteral("ГГГГ-ММ-ДД"));
    } else if (field.editorKind == QStringLiteral("datetime")) {
        searchValueEdit_->setPlaceholderText(
            QStringLiteral("ГГГГ-ММ-ДД ЧЧ:ММ:СС"));
    } else {
        searchValueEdit_->setPlaceholderText(QStringLiteral("Значение"));
    }
    refreshSearchValueInputs();
}

void MainWindow::refreshSearchValueInputs()
{
    const qsizetype fieldIndex =
        searchFieldCombo_->currentData().toLongLong();
    if (fieldIndex < 0 || fieldIndex >= searchFields_.size()) {
        searchButton_->setEnabled(false);
        return;
    }
    const repo::SearchField& field = searchFields_.at(fieldIndex);
    const QString comparison =
        searchOperatorCombo_->currentData().toString();
    const bool noValue = comparison == QStringLiteral("is_null")
                         || comparison == QStringLiteral("not_null");
    const bool choiceValue = !noValue
        && comparison == QStringLiteral("equals")
        && (field.editorKind == QStringLiteral("lookup")
            || field.editorKind == QStringLiteral("boolean"));
    searchValueEdit_->setVisible(!noValue && !choiceValue);
    searchValueCombo_->setVisible(choiceValue);
    searchSecondValueEdit_->setVisible(
        comparison == QStringLiteral("between"));
    searchButton_->setEnabled(!comparison.isEmpty());
}

void MainWindow::executeSearch()
{
    const qsizetype fieldIndex =
        searchFieldCombo_->currentData().toLongLong();
    if (!connection_.isOpen()
        || fieldIndex < 0 || fieldIndex >= searchFields_.size()) {
        return;
    }
    const repo::SearchField& field = searchFields_.at(fieldIndex);
    repo::SearchCriteria criteria;
    criteria.classTable = searchClassCombo_->currentData().toString();
    criteria.fieldName = field.name;
    criteria.comparison = searchOperatorCombo_->currentData().toString();
    criteria.includeArchived = searchArchivedCheck_->isChecked();
    criteria.limit = 200;
    const bool noValue = criteria.comparison == QStringLiteral("is_null")
                         || criteria.comparison == QStringLiteral("not_null");
    const bool choiceValue = !noValue
        && criteria.comparison == QStringLiteral("equals")
        && (field.editorKind == QStringLiteral("lookup")
            || field.editorKind == QStringLiteral("boolean"));
    if (choiceValue) {
        if (searchValueCombo_->currentIndex() < 0) {
            QMessageBox::warning(
                this, QStringLiteral("Поиск"),
                QStringLiteral("Выберите значение из списка"));
            return;
        }
        criteria.value = searchValueCombo_->currentData().toString();
    } else if (!noValue) {
        criteria.value = searchValueEdit_->text().trimmed();
        if (criteria.value.isEmpty()) {
            QMessageBox::warning(
                this, QStringLiteral("Поиск"),
                QStringLiteral("Введите значение поиска"));
            return;
        }
    }
    if (criteria.comparison == QStringLiteral("between")) {
        criteria.secondValue = searchSecondValueEdit_->text().trimmed();
        if (criteria.secondValue.isEmpty()) {
            QMessageBox::warning(
                this, QStringLiteral("Поиск"),
                QStringLiteral("Введите обе границы диапазона"));
            return;
        }
    }

    searchButton_->setEnabled(false);
    searchStatusLabel_->setStyleSheet({});
    searchStatusLabel_->setText(QStringLiteral("Выполняется запрос…"));
    QApplication::processEvents();
    QString error;
    const QList<repo::SearchResult> results =
        searchRepository_.search(connection_.database(), criteria, &error);
    searchButton_->setEnabled(true);
    if (!error.isEmpty()) {
        searchTable_->setRowCount(0);
        searchStatusLabel_->setStyleSheet(QStringLiteral("color: #b42318;"));
        searchStatusLabel_->setText(QStringLiteral("Ошибка поиска: %1").arg(error));
        return;
    }

    searchTable_->setSortingEnabled(false);
    searchTable_->setRowCount(results.size());
    const bool isNode = searchClassCombo_->currentData(
        Qt::UserRole + 1).toBool();
    for (qsizetype row = 0; row < results.size(); ++row) {
        const repo::SearchResult& result = results.at(row);
        auto* idItem = readOnlyItem(QString::number(result.id));
        idItem->setData(Qt::UserRole, result.id);
        idItem->setData(Qt::UserRole + 1, criteria.classTable);
        idItem->setData(Qt::UserRole + 2, isNode);
        searchTable_->setItem(row, 0, idItem);
        searchTable_->setItem(row, 1, readOnlyItem(result.fragmentId));
        searchTable_->setItem(row, 2, readOnlyItem(field.displayName));
        searchTable_->setItem(row, 3, readOnlyItem(result.value));
        searchTable_->setItem(
            row, 4, readOnlyItem(QString::number(result.rowVersion)));
        searchTable_->setItem(
            row, 5,
            readOnlyItem(result.archived ? QStringLiteral("архив")
                                         : QStringLiteral("активен")));
    }
    searchTable_->setSortingEnabled(true);
    searchStatusLabel_->setStyleSheet(QStringLiteral("color: #067647;"));
    searchStatusLabel_->setText(
        results.size() == criteria.limit
            ? QStringLiteral(
                  "Показаны первые %1 результатов. Уточните условие поиска.")
                  .arg(criteria.limit)
            : QStringLiteral("Найдено объектов: %1").arg(results.size()));
}

void MainWindow::openSearchResult(int row, int column)
{
    Q_UNUSED(column);
    QTableWidgetItem* item = searchTable_->item(row, 0);
    if (item == nullptr) {
        return;
    }
    showObjectDetails(
        item->data(Qt::UserRole).toLongLong(),
        item->data(Qt::UserRole + 1).toString(),
        item->data(Qt::UserRole + 2).toBool());
}

void MainWindow::populateReportFragments(
    const QList<repo::FragmentInfo>& fragments)
{
    reportFragmentCombo_->clear();
    reportFragmentCombo_->addItem(
        QStringLiteral("Все фрагменты"), 0);
    for (const repo::FragmentInfo& fragment : fragments) {
        QString label = fragment.name;
        if (!fragment.settlement.trimmed().isEmpty()) {
            label = QStringLiteral("%1 — %2")
                        .arg(fragment.settlement.trimmed(), fragment.name);
        }
        reportFragmentCombo_->addItem(
            QStringLiteral("%1 (#%2)").arg(label).arg(fragment.id),
            fragment.id);
    }
    reportFragmentCombo_->setEnabled(
        reportFragmentCombo_->count() > 0
        && !reportSelectedCheck_->isChecked());
    runPipeLengthReportButton_->setEnabled(
        reportFragmentCombo_->count() > 0);
    exportPipeLengthReportButton_->setEnabled(false);
    pipeLengthReportRows_.clear();
    pipeLengthReportTable_->setRowCount(0);
    pipeLengthReportStatusLabel_->setStyleSheet({});
    pipeLengthReportStatusLabel_->setText(
        QStringLiteral("Выберите параметры и сформируйте отчёт"));
}

void MainWindow::executePipeLengthReport()
{
    if (!connection_.isOpen()) {
        return;
    }
    repo::PipeLengthReportCriteria criteria;
    criteria.fragmentId = reportSelectedCheck_->isChecked()
                              ? 0
                              : reportFragmentCombo_->currentData().toInt();
    criteria.grouping = reportGroupingCombo_->currentData().toString();
    criteria.includeArchived = reportArchivedCheck_->isChecked();
    if (reportSelectedCheck_->isChecked()) {
        const QList<SelectedMapObject> selected = mapView_->selectedObjects();
        for (const SelectedMapObject& object : selected) {
            if (object.isNode
                || object.classTable != QStringLiteral("pipe_section")) {
                QMessageBox::warning(
                    this,
                    QStringLiteral("Отчёт протяжённости"),
                    QStringLiteral(
                        "Для отчёта должны быть выбраны только pipe_section"));
                return;
            }
            criteria.pipeIds.append(object.id);
        }
        if (criteria.pipeIds.isEmpty()) {
            QMessageBox::warning(
                this,
                QStringLiteral("Отчёт протяжённости"),
                QStringLiteral(
                    "Сначала выберите участки pipe_section на карте"));
            return;
        }
    }
    runPipeLengthReportButton_->setEnabled(false);
    exportPipeLengthReportButton_->setEnabled(false);
    pipeLengthReportStatusLabel_->setStyleSheet({});
    pipeLengthReportStatusLabel_->setText(
        QStringLiteral("Расчёт протяжённости…"));
    QApplication::processEvents();

    QString error;
    pipeLengthReportRows_ = pipeLengthReportRepository_.load(
        connection_.database(), criteria, &error);
    runPipeLengthReportButton_->setEnabled(true);
    if (!error.isEmpty()) {
        pipeLengthReportRows_.clear();
        pipeLengthReportTable_->setRowCount(0);
        pipeLengthReportStatusLabel_->setStyleSheet(
            QStringLiteral("color: #b42318;"));
        pipeLengthReportStatusLabel_->setText(
            QStringLiteral("Ошибка отчёта: %1").arg(error));
        return;
    }

    qint64 totalCount = 0;
    qint64 totalMissing = 0;
    double totalPassport = 0.0;
    double totalGeometry = 0.0;
    double totalEffective = 0.0;
    for (const repo::PipeLengthReportRow& row : pipeLengthReportRows_) {
        totalCount += row.pipeCount;
        totalMissing += row.missingPassportCount;
        totalPassport += row.passportLength;
        totalGeometry += row.geometryLength;
        totalEffective += row.effectiveLength;
    }
    const bool addTotalRow = pipeLengthReportRows_.size() > 1;
    pipeLengthReportTable_->setSortingEnabled(false);
    pipeLengthReportTable_->setRowCount(
        pipeLengthReportRows_.size() + (addTotalRow ? 1 : 0));
    const auto putRow = [this](
                            qsizetype tableRow,
                            const QString& label,
                            qint64 count,
                            double passport,
                            double geometry,
                            double effective,
                            qint64 missing,
                            bool bold) {
        const QStringList values = {
            label,
            QString::number(count),
            QString::number(passport, 'f', 2),
            QString::number(geometry, 'f', 2),
            QString::number(effective, 'f', 2),
            QString::number(passport - geometry, 'f', 2),
            QString::number(missing),
        };
        for (qsizetype column = 0; column < values.size(); ++column) {
            QTableWidgetItem* item = readOnlyItem(values.at(column));
            if (bold) {
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
                item->setBackground(QColor(QStringLiteral("#e2e8f0")));
            }
            if (column > 0) {
                item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            }
            pipeLengthReportTable_->setItem(tableRow, column, item);
        }
    };
    for (qsizetype rowIndex = 0;
         rowIndex < pipeLengthReportRows_.size(); ++rowIndex) {
        const repo::PipeLengthReportRow& row =
            pipeLengthReportRows_.at(rowIndex);
        putRow(rowIndex, row.groupLabel, row.pipeCount,
               row.passportLength, row.geometryLength,
               row.effectiveLength, row.missingPassportCount, false);
    }
    if (addTotalRow) {
        putRow(pipeLengthReportRows_.size(), QStringLiteral("ИТОГО"),
               totalCount, totalPassport, totalGeometry,
               totalEffective, totalMissing, true);
    }
    exportPipeLengthReportButton_->setEnabled(
        !pipeLengthReportRows_.isEmpty());
    pipeLengthReportStatusLabel_->setStyleSheet(
        QStringLiteral("color: #067647;"));
    pipeLengthReportStatusLabel_->setText(
        QStringLiteral(
            "Участков: %1 · рабочая длина: %2 км · "
            "паспорт не заполнен: %3")
            .arg(totalCount)
            .arg(totalEffective / 1000.0, 0, 'f', 3)
            .arg(totalMissing));
}

void MainWindow::exportPipeLengthReport()
{
    if (pipeLengthReportRows_.isEmpty()) {
        return;
    }
    const QString fileName = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("Экспорт отчёта"),
        QStringLiteral("tgid_pipe_length.csv"),
        QStringLiteral("CSV (*.csv)"));
    if (fileName.isEmpty()) {
        return;
    }
    QSaveFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(
            this, QStringLiteral("Экспорт CSV"), file.errorString());
        return;
    }
    file.write("\xEF\xBB\xBF");
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << csvCell(QStringLiteral("Группа")) << ';'
           << csvCell(QStringLiteral("Участков")) << ';'
           << csvCell(QStringLiteral("Паспортная длина, м")) << ';'
           << csvCell(QStringLiteral("Геометрическая длина, м")) << ';'
           << csvCell(QStringLiteral("Рабочая длина, м")) << ';'
           << csvCell(QStringLiteral("Паспорт - геометрия, м")) << ';'
           << csvCell(QStringLiteral("Без паспортной длины")) << '\n';
    qint64 totalCount = 0;
    qint64 totalMissing = 0;
    double totalPassport = 0.0;
    double totalGeometry = 0.0;
    double totalEffective = 0.0;
    for (const repo::PipeLengthReportRow& row : pipeLengthReportRows_) {
        stream << csvCell(row.groupLabel) << ';'
               << row.pipeCount << ';'
               << QString::number(row.passportLength, 'f', 2) << ';'
               << QString::number(row.geometryLength, 'f', 2) << ';'
               << QString::number(row.effectiveLength, 'f', 2) << ';'
               << QString::number(
                      row.passportLength - row.geometryLength, 'f', 2)
               << ';' << row.missingPassportCount << '\n';
        totalCount += row.pipeCount;
        totalMissing += row.missingPassportCount;
        totalPassport += row.passportLength;
        totalGeometry += row.geometryLength;
        totalEffective += row.effectiveLength;
    }
    if (pipeLengthReportRows_.size() > 1) {
        stream << csvCell(QStringLiteral("ИТОГО")) << ';'
               << totalCount << ';'
               << QString::number(totalPassport, 'f', 2) << ';'
               << QString::number(totalGeometry, 'f', 2) << ';'
               << QString::number(totalEffective, 'f', 2) << ';'
               << QString::number(totalPassport - totalGeometry, 'f', 2)
               << ';' << totalMissing << '\n';
    }
    stream.flush();
    if (!file.commit()) {
        QMessageBox::critical(
            this, QStringLiteral("Экспорт CSV"), file.errorString());
        return;
    }
    statusBar()->showMessage(
        QStringLiteral("Отчёт сохранён: %1").arg(fileName));
}

void MainWindow::loadSelectedFragment()
{
    if (mapWatcher_->isRunning()) {
        return;
    }

    QTreeWidgetItem* item = fragmentTree_->currentItem();
    if (item == nullptr) {
        return;
    }
    const QVariant fragmentValue = item->data(0, Qt::UserRole);
    if (!fragmentValue.isValid()) {
        return;
    }

    const int fragmentId = fragmentValue.toInt();
    if (createPointButton_->isChecked()) {
        createPointButton_->setChecked(false);
    }
    if (createLineButton_->isChecked()) {
        createLineButton_->setChecked(false);
    }
    if (splitLineButton_->isChecked()) {
        splitLineButton_->setChecked(false);
    }
    if (joinLinesButton_->isChecked()) {
        joinLinesButton_->setChecked(false);
    }
    batchEditButton_->setEnabled(false);
    fragmentTree_->setEnabled(false);
    refreshButton_->setEnabled(false);
    fitMapButton_->setEnabled(false);
    mapStatusLabel_->setText(
        QStringLiteral("Загрузка фрагмента %1…").arg(fragmentId));
    statusBar()->showMessage(QStringLiteral("Загрузка геометрии"));
    mapLoadTimer_.restart();

    const db::DatabaseConfig config = config_;
    mapWatcher_->setFuture(QtConcurrent::run([config, fragmentId]() {
        return repo::MapRepository().loadFragment(config, fragmentId);
    }));
}

void MainWindow::finishMapLoad()
{
    fragmentTree_->setEnabled(true);
    refreshButton_->setEnabled(true);

    const bool preserveObject = preserveObjectAfterMapLoad_;
    preserveObjectAfterMapLoad_ = false;
    repo::MapData mapData = mapWatcher_->result();
    if (!mapData.isValid()) {
        mapView_->clearMap();
        mapStatusLabel_->setText(
            QStringLiteral("Ошибка загрузки: %1").arg(mapData.error));
        statusBar()->showMessage(QStringLiteral("Фрагмент не загружен"));
        return;
    }

    const qsizetype nodeCount = mapData.nodes.size();
    const qsizetype lineCount = mapData.lines.size();
    const int fragmentId = mapData.fragmentId;
    mapView_->setMapData(std::move(mapData));
    if (!preserveObject) {
        clearObjectDetails();
    }
    fitMapButton_->setEnabled(nodeCount > 0 || lineCount > 0);
    createPointButton_->setEnabled(
        (nodeCount > 0 || lineCount > 0)
        && pointClassCombo_->count() > 0);
    pointClassCombo_->setEnabled(pointClassCombo_->count() > 0);
    createLineButton_->setEnabled(
        nodeCount >= 2 && lineClassCombo_->count() > 0);
    lineClassCombo_->setEnabled(lineClassCombo_->count() > 0);
    mapStatusLabel_->setText(
        QStringLiteral("Фрагмент %1: %2 узлов, %3 линий, %4 мс")
            .arg(fragmentId)
            .arg(nodeCount)
            .arg(lineCount)
            .arg(mapLoadTimer_.elapsed()));
    statusBar()->showMessage(QStringLiteral("Фрагмент загружен"));
    if (pendingObjectId_ != 0) {
        const qint64 objectId = pendingObjectId_;
        const QString classTable = pendingObjectClassTable_;
        pendingObjectId_ = 0;
        pendingObjectClassTable_.clear();
        const bool isNode = pendingObjectIsNode_;
        pendingObjectIsNode_ = false;
        showObjectDetails(objectId, classTable, isNode);
        statusBar()->showMessage(
            QStringLiteral("Открыт объект %1 #%2")
                .arg(classTable)
                .arg(objectId));
    }
}

void MainWindow::togglePointCreation(bool enabled)
{
    if (enabled) {
        if (createLineButton_->isChecked()) {
            createLineButton_->setChecked(false);
        }
        if (splitLineButton_->isChecked()) {
            splitLineButton_->setChecked(false);
        }
        if (joinLinesButton_->isChecked()) {
            joinLinesButton_->setChecked(false);
        }
        QTreeWidgetItem* item = fragmentTree_->currentItem();
        if (item == nullptr
            || !item->data(0, Qt::UserRole).isValid()
            || pointClassCombo_->currentData().toString().isEmpty()) {
            createPointButton_->setChecked(false);
            return;
        }
    }

    mapView_->setPointCreationMode(enabled);
    batchEditButton_->setEnabled(
        !enabled && mapView_->selectedObjects().size() >= 2);
    createPointButton_->setText(
        enabled ? QStringLiteral("Отмена")
                : QStringLiteral("Создать точку"));
    pointClassCombo_->setEnabled(
        !enabled && pointClassCombo_->count() > 0);
    lineClassCombo_->setEnabled(
        !enabled && !createLineButton_->isChecked()
        && lineClassCombo_->count() > 0);
    fragmentTree_->setEnabled(!enabled && !mapWatcher_->isRunning());
    statusBar()->showMessage(
        enabled
            ? QStringLiteral("Укажите положение нового объекта на карте")
            : QStringLiteral("Режим создания выключен"));
}

void MainWindow::createPointAt(QPointF position)
{
    QTreeWidgetItem* item = fragmentTree_->currentItem();
    const QString classTable = pointClassCombo_->currentData().toString();
    if (item == nullptr || classTable.isEmpty()) {
        return;
    }
    const QVariant fragmentValue = item->data(0, Qt::UserRole);
    if (!fragmentValue.isValid()) {
        return;
    }
    const int fragmentId = fragmentValue.toInt();
    if (QMessageBox::question(
            this,
            QStringLiteral("Создание объекта"),
            QStringLiteral(
                "Создать %1 во фрагменте %2?\nX = %3\nY = %4")
                .arg(classTable)
                .arg(fragmentId)
                .arg(position.x(), 0, 'f', 3)
                .arg(position.y(), 0, 'f', 3))
        != QMessageBox::Yes) {
        return;
    }

    createPointButton_->setChecked(false);
    createPointButton_->setEnabled(false);
    const repo::CreateObjectResult result =
        objectRepository_.createPoint(
            connection_.database(),
            classTable,
            fragmentId,
            position);
    if (!result.success) {
        createPointButton_->setEnabled(true);
        QMessageBox::critical(
            this, QStringLiteral("Ошибка создания"), result.error);
        return;
    }

    pendingObjectId_ = result.id;
    pendingObjectClassTable_ = classTable;
    pendingObjectIsNode_ = true;
    loadSelectedFragment();
}

void MainWindow::toggleLineCreation(bool enabled)
{
    if (enabled) {
        if (createPointButton_->isChecked()) {
            createPointButton_->setChecked(false);
        }
        if (splitLineButton_->isChecked()) {
            splitLineButton_->setChecked(false);
        }
        if (joinLinesButton_->isChecked()) {
            joinLinesButton_->setChecked(false);
        }
        QTreeWidgetItem* item = fragmentTree_->currentItem();
        if (item == nullptr
            || !item->data(0, Qt::UserRole).isValid()
            || lineClassCombo_->currentData().toString().isEmpty()) {
            createLineButton_->setChecked(false);
            return;
        }
    }

    mapView_->setLineCreationMode(enabled);
    batchEditButton_->setEnabled(
        !enabled && mapView_->selectedObjects().size() >= 2);
    createLineButton_->setText(
        enabled ? QStringLiteral("Отмена")
                : QStringLiteral("Создать линию"));
    lineClassCombo_->setEnabled(
        !enabled && lineClassCombo_->count() > 0);
    pointClassCombo_->setEnabled(
        !enabled && !createPointButton_->isChecked()
        && pointClassCombo_->count() > 0);
    fragmentTree_->setEnabled(!enabled && !mapWatcher_->isRunning());
    statusBar()->showMessage(
        enabled
            ? QStringLiteral("Выберите начальный узел новой линии")
            : QStringLiteral("Режим создания линии выключен"));
}

void MainWindow::showLineStart(qint64 nodeId)
{
    statusBar()->showMessage(
        QStringLiteral(
            "Начальный узел #%1 выбран. Выберите конечный узел.")
            .arg(nodeId));
}

void MainWindow::createLineBetween(qint64 nodeFrom, qint64 nodeTo)
{
    QTreeWidgetItem* item = fragmentTree_->currentItem();
    const QString classTable = lineClassCombo_->currentData().toString();
    if (item == nullptr || classTable.isEmpty()) {
        return;
    }
    const QVariant fragmentValue = item->data(0, Qt::UserRole);
    if (!fragmentValue.isValid()) {
        return;
    }
    const int fragmentId = fragmentValue.toInt();
    if (QMessageBox::question(
            this,
            QStringLiteral("Создание линии"),
            QStringLiteral(
                "Создать %1 во фрагменте %2?\n"
                "Начальный узел: #%3\nКонечный узел: #%4")
                .arg(classTable)
                .arg(fragmentId)
                .arg(nodeFrom)
                .arg(nodeTo))
        != QMessageBox::Yes) {
        statusBar()->showMessage(
            QStringLiteral("Выберите начальный узел новой линии"));
        return;
    }

    createLineButton_->setChecked(false);
    createLineButton_->setEnabled(false);
    const repo::CreateObjectResult result =
        objectRepository_.createLine(
            connection_.database(),
            classTable,
            fragmentId,
            nodeFrom,
            nodeTo);
    if (!result.success) {
        createLineButton_->setEnabled(true);
        QMessageBox::critical(
            this, QStringLiteral("Ошибка создания линии"), result.error);
        return;
    }

    pendingObjectId_ = result.id;
    pendingObjectClassTable_ = classTable;
    pendingObjectIsNode_ = false;
    loadSelectedFragment();
}

void MainWindow::toggleLineSplit(bool enabled)
{
    if (enabled) {
        if (currentObjectDetails_.classTable.isEmpty()
            || currentObjectIsNode_ || currentObjectDetails_.archived
            || !currentObjectDetails_.canSplit
            || !mapView_->hasSelectedLine(
                currentObjectDetails_.id,
                currentObjectDetails_.classTable)) {
            splitLineButton_->setChecked(false);
            return;
        }
        if (createPointButton_->isChecked()) {
            createPointButton_->setChecked(false);
        }
        if (createLineButton_->isChecked()) {
            createLineButton_->setChecked(false);
        }
        if (joinLinesButton_->isChecked()) {
            joinLinesButton_->setChecked(false);
        }
    }

    mapView_->setLineSplitMode(enabled);
    batchEditButton_->setEnabled(false);
    splitLineButton_->setText(
        enabled ? QStringLiteral("Отмена разрезания")
                : QStringLiteral("Разрезать линию"));
    pointClassCombo_->setEnabled(
        !enabled && !createPointButton_->isChecked()
        && pointClassCombo_->count() > 0);
    lineClassCombo_->setEnabled(
        !enabled && !createLineButton_->isChecked()
        && lineClassCombo_->count() > 0);
    fragmentTree_->setEnabled(!enabled && !mapWatcher_->isRunning());
    statusBar()->showMessage(
        enabled
            ? QStringLiteral("Укажите место разрезания выбранной линии")
            : QStringLiteral("Режим разрезания выключен"));
}

void MainWindow::splitLineAt(QPointF position)
{
    if (!connection_.isOpen()
        || currentObjectDetails_.classTable.isEmpty()
        || currentObjectIsNode_ || currentObjectDetails_.archived) {
        return;
    }

    if (QMessageBox::question(
            this,
            QStringLiteral("Разрезание линии"),
            QStringLiteral(
                "Разрезать %1 #%2 в ближайшей к указанной точке позиции?\n"
                "X = %3\nY = %4\n\n"
                "Исходная линия будет помещена в архив, а её атрибуты "
                "перенесены в два новых участка.")
                .arg(currentObjectDetails_.classTable)
                .arg(currentObjectDetails_.id)
                .arg(position.x(), 0, 'f', 3)
                .arg(position.y(), 0, 'f', 3))
        != QMessageBox::Yes) {
        statusBar()->showMessage(
            QStringLiteral("Укажите другое место разрезания"));
        return;
    }

    splitLineButton_->setChecked(false);
    splitLineButton_->setEnabled(false);
    const QString classTable = currentObjectDetails_.classTable;
    const repo::SplitLineResult result = objectRepository_.splitLine(
        connection_.database(),
        classTable,
        currentObjectDetails_.id,
        currentObjectDetails_.rowVersion,
        position);
    if (result.conflict) {
        reloadObjectDetails();
        QMessageBox::warning(
            this,
            QStringLiteral("Конфликт редактирования"),
            QStringLiteral(
                "Линия уже изменена другим пользователем. "
                "Карточка перезагружена."));
        return;
    }
    if (!result.success) {
        splitLineButton_->setEnabled(true);
        QMessageBox::critical(
            this, QStringLiteral("Ошибка разрезания"), result.error);
        return;
    }

    QMessageBox::information(
        this,
        QStringLiteral("Линия разрезана"),
        QStringLiteral(
            "%1 узел #%2 и участки #%3, #%4.")
            .arg(result.nodeCreated
                     ? QStringLiteral("Создан")
                     : QStringLiteral("Использован существующий"))
            .arg(result.nodeId)
            .arg(result.firstLineId)
            .arg(result.secondLineId));
    pendingObjectId_ = result.firstLineId;
    pendingObjectClassTable_ = classTable;
    pendingObjectIsNode_ = false;
    loadSelectedFragment();
}

void MainWindow::toggleLineJoin(bool enabled)
{
    if (enabled) {
        if (currentObjectDetails_.classTable.isEmpty()
            || currentObjectIsNode_ || currentObjectDetails_.archived
            || !currentObjectDetails_.canJoin
            || !mapView_->hasSelectedLine(
                currentObjectDetails_.id,
                currentObjectDetails_.classTable)) {
            joinLinesButton_->setChecked(false);
            return;
        }
        if (createPointButton_->isChecked()) {
            createPointButton_->setChecked(false);
        }
        if (createLineButton_->isChecked()) {
            createLineButton_->setChecked(false);
        }
        if (splitLineButton_->isChecked()) {
            splitLineButton_->setChecked(false);
        }
    }

    mapView_->setLineJoinMode(
        enabled,
        currentObjectDetails_.id,
        currentObjectDetails_.classTable);
    batchEditButton_->setEnabled(false);
    joinLinesButton_->setText(
        enabled ? QStringLiteral("Отмена соединения")
                : QStringLiteral("Соединить линии"));
    pointClassCombo_->setEnabled(
        !enabled && !createPointButton_->isChecked()
        && pointClassCombo_->count() > 0);
    lineClassCombo_->setEnabled(
        !enabled && !createLineButton_->isChecked()
        && lineClassCombo_->count() > 0);
    fragmentTree_->setEnabled(!enabled && !mapWatcher_->isRunning());
    statusBar()->showMessage(
        enabled
            ? QStringLiteral("Выберите второй участок на карте")
            : QStringLiteral("Режим соединения выключен"));
}

void MainWindow::joinWithLine(qint64 secondId, QString classTable)
{
    if (!connection_.isOpen()
        || currentObjectDetails_.classTable.isEmpty()
        || currentObjectIsNode_ || currentObjectDetails_.archived) {
        return;
    }
    if (classTable != currentObjectDetails_.classTable) {
        QMessageBox::warning(
            this,
            QStringLiteral("Соединение линий"),
            QStringLiteral(
                "Участки должны принадлежать одному классу. "
                "Выбран %1, ожидается %2.")
                .arg(classTable, currentObjectDetails_.classTable));
        return;
    }

    const repo::ObjectDetails secondDetails =
        objectRepository_.load(
            connection_.database(), classTable, secondId);
    if (!secondDetails.isValid() || secondDetails.archived
        || !secondDetails.canJoin) {
        QMessageBox::critical(
            this,
            QStringLiteral("Ошибка соединения"),
            secondDetails.isValid()
                ? QStringLiteral("Выбранный участок нельзя соединить")
                : secondDetails.error);
        return;
    }
    if (QMessageBox::question(
            this,
            QStringLiteral("Соединение линий"),
            QStringLiteral(
                "Соединить %1 #%2 и #%3 через их общий узел?\n\n"
                "Оба исходных участка и общий соединительный узел "
                "будут помещены в архив. Будет создан один новый "
                "участок. Операция разрешена только при одинаковых "
                "атрибутах и отсутствии других линий в общем узле.")
                .arg(classTable)
                .arg(currentObjectDetails_.id)
                .arg(secondId))
        != QMessageBox::Yes) {
        statusBar()->showMessage(
            QStringLiteral("Выберите другой второй участок"));
        return;
    }

    joinLinesButton_->setChecked(false);
    joinLinesButton_->setEnabled(false);
    const qint64 firstId = currentObjectDetails_.id;
    const qint64 firstVersion = currentObjectDetails_.rowVersion;
    const repo::JoinLinesResult result = objectRepository_.joinLines(
        connection_.database(),
        classTable,
        firstId,
        firstVersion,
        secondId,
        secondDetails.rowVersion);
    if (result.conflict) {
        reloadObjectDetails();
        QMessageBox::warning(
            this,
            QStringLiteral("Конфликт редактирования"),
            QStringLiteral(
                "Один из участков уже изменён другим пользователем. "
                "Карточка перезагружена."));
        return;
    }
    if (!result.success) {
        joinLinesButton_->setEnabled(
            currentObjectDetails_.canJoin
            && mapView_->hasSelectedLine(firstId, classTable));
        QMessageBox::critical(
            this, QStringLiteral("Ошибка соединения"), result.error);
        return;
    }

    QMessageBox::information(
        this,
        QStringLiteral("Линии соединены"),
        QStringLiteral(
            "Создан участок #%1. Общий узел #%2 помещён в архив.")
            .arg(result.joinedLineId)
            .arg(result.archivedNodeId));
    pendingObjectId_ = result.joinedLineId;
    pendingObjectClassTable_ = classTable;
    pendingObjectIsNode_ = false;
    loadSelectedFragment();
}

void MainWindow::showMultipleSelection(
    int count, QString classTable, bool isNode)
{
    clearObjectDetails();
    if (count < 2) {
        return;
    }
    batchEditButton_->setEnabled(true);
    objectTitleLabel_->setText(
        QStringLiteral(
            "<b>Выбрано объектов: %1</b><br>%2 · %3<br>"
            "Ctrl+щелчок добавляет или исключает объект")
            .arg(count)
            .arg(classTable)
            .arg(isNode ? QStringLiteral("точечный класс")
                        : QStringLiteral("линейный класс")));
    objectDock_->show();
    objectDock_->raise();
    statusBar()->showMessage(
        QStringLiteral("Выбрано %1 объектов %2")
            .arg(count)
            .arg(classTable));
}

void MainWindow::batchEditSelectedObjects()
{
    const QList<SelectedMapObject> selected = mapView_->selectedObjects();
    if (!connection_.isOpen() || selected.size() < 2) {
        batchEditButton_->setEnabled(false);
        return;
    }
    const QString classTable = selected.first().classTable;
    const repo::ObjectDetails templateDetails = objectRepository_.load(
        connection_.database(), classTable, selected.first().id);
    if (!templateDetails.isValid() || templateDetails.archived) {
        QMessageBox::critical(
            this, QStringLiteral("Массовое изменение"),
            templateDetails.isValid()
                ? QStringLiteral("Выбран архивный объект")
                : templateDetails.error);
        return;
    }

    QList<repo::ObjectAttribute> editableAttributes;
    for (const repo::ObjectAttribute& attribute
         : templateDetails.attributes) {
        if (attribute.editable) {
            editableAttributes.append(attribute);
        }
    }
    if (editableAttributes.isEmpty()) {
        QMessageBox::information(
            this, QStringLiteral("Массовое изменение"),
            QStringLiteral("У этого класса нет редактируемых полей"));
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Массовое изменение"));
    dialog.setMinimumWidth(470);
    auto* layout = new QVBoxLayout(&dialog);
    auto* explanation = new QLabel(
        QStringLiteral(
            "Класс: <b>%1</b><br>Объектов: <b>%2</b><br>"
            "Одно значение будет записано во все выбранные строки.")
            .arg(classTable)
            .arg(selected.size()),
        &dialog);
    explanation->setWordWrap(true);
    layout->addWidget(explanation);
    layout->addWidget(new QLabel(QStringLiteral("Поле:"), &dialog));
    auto* fieldCombo = new QComboBox(&dialog);
    for (qsizetype index = 0; index < editableAttributes.size(); ++index) {
        const repo::ObjectAttribute& attribute =
            editableAttributes.at(index);
        fieldCombo->addItem(
            attribute.unit.isEmpty()
                ? attribute.displayName
                : QStringLiteral("%1, %2")
                      .arg(attribute.displayName, attribute.unit),
            index);
    }
    layout->addWidget(fieldCombo);
    layout->addWidget(new QLabel(QStringLiteral("Новое значение:"), &dialog));
    auto* valueEdit = new QLineEdit(&dialog);
    auto* valueText = new QPlainTextEdit(&dialog);
    valueText->setMaximumHeight(100);
    auto* valueCombo = new QComboBox(&dialog);
    valueCombo->setEditable(false);
    layout->addWidget(valueEdit);
    layout->addWidget(valueText);
    layout->addWidget(valueCombo);
    auto* nullCheck = new QCheckBox(
        QStringLiteral("Установить NULL (очистить поле)"), &dialog);
    layout->addWidget(nullCheck);
    auto* fieldHint = new QLabel(&dialog);
    fieldHint->setWordWrap(true);
    layout->addWidget(fieldHint);

    const auto updateEditor = [=, &editableAttributes](int comboIndex) {
        if (comboIndex < 0) {
            return;
        }
        const qsizetype attributeIndex =
            fieldCombo->itemData(comboIndex).toLongLong();
        const repo::ObjectAttribute& attribute =
            editableAttributes.at(attributeIndex);
        const bool choiceEditor =
            attribute.editorKind == QStringLiteral("lookup")
            || attribute.editorKind == QStringLiteral("boolean");
        const bool multiline =
            attribute.editorKind == QStringLiteral("multiline");
        valueEdit->setVisible(!choiceEditor && !multiline);
        valueText->setVisible(multiline);
        valueCombo->setVisible(choiceEditor);
        valueEdit->clear();
        valueText->clear();
        valueCombo->clear();
        valueEdit->setValidator(nullptr);
        if (attribute.editorKind == QStringLiteral("integer")
            || attribute.editorKind == QStringLiteral("decimal")) {
            const QString pattern =
                attribute.editorKind == QStringLiteral("integer")
                    ? QStringLiteral("^-?[0-9]*$")
                    : QStringLiteral(
                          "^-?[0-9]*([\\.,][0-9]*)?"
                          "([eE][+-]?[0-9]*)?$");
            valueEdit->setValidator(new QRegularExpressionValidator(
                QRegularExpression(pattern), valueEdit));
            valueEdit->setAlignment(Qt::AlignRight);
        } else {
            valueEdit->setAlignment(Qt::AlignLeft);
        }
        if (attribute.editorKind == QStringLiteral("boolean")) {
            valueCombo->addItem(
                QStringLiteral("Да"), QStringLiteral("да"));
            valueCombo->addItem(
                QStringLiteral("Нет"), QStringLiteral("нет"));
        } else if (attribute.editorKind == QStringLiteral("lookup")) {
            for (const repo::ObjectFieldOption& option : attribute.options) {
                valueCombo->addItem(option.label, option.value);
            }
        }
        fieldHint->setText(
            QStringLiteral("Системное имя: %1 · тип: %2")
                .arg(attribute.name, attribute.dataType));
    };
    connect(fieldCombo, &QComboBox::currentIndexChanged,
            &dialog, updateEditor);
    connect(nullCheck, &QCheckBox::toggled, &dialog,
            [=](bool checked) {
                valueEdit->setEnabled(!checked);
                valueText->setEnabled(!checked);
                valueCombo->setEnabled(!checked);
            });
    updateEditor(fieldCombo->currentIndex());

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttons, &QDialogButtonBox::accepted,
            &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected,
            &dialog, &QDialog::reject);
    layout->addWidget(buttons);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    const qsizetype attributeIndex =
        fieldCombo->currentData().toLongLong();
    const repo::ObjectAttribute& attribute =
        editableAttributes.at(attributeIndex);
    repo::AttributeChange change;
    change.name = attribute.name;
    change.databaseType = attribute.databaseType;
    change.setNull = nullCheck->isChecked();
    QString displayValue = QStringLiteral("NULL");
    if (!change.setNull) {
        if (attribute.editorKind == QStringLiteral("lookup")
            || attribute.editorKind == QStringLiteral("boolean")) {
            if (valueCombo->currentIndex() < 0) {
                QMessageBox::warning(
                    this, QStringLiteral("Массовое изменение"),
                    QStringLiteral("Выберите значение из списка"));
                return;
            }
            change.value = valueCombo->currentData().toString();
            displayValue = valueCombo->currentText();
        } else if (attribute.editorKind == QStringLiteral("multiline")) {
            change.value = valueText->toPlainText();
            displayValue = change.value;
        } else {
            change.value = valueEdit->text();
            if (attribute.editorKind == QStringLiteral("integer")
                || attribute.editorKind == QStringLiteral("decimal")) {
                change.value = change.value.trimmed();
                change.value.replace(',', '.');
                const QRegularExpression completeNumber(
                    attribute.editorKind == QStringLiteral("integer")
                        ? QStringLiteral("^-?[0-9]+$")
                        : QStringLiteral(
                              "^-?([0-9]+(\\.[0-9]*)?|\\.[0-9]+)"
                              "([eE][+-]?[0-9]+)?$"));
                if (!completeNumber.match(change.value).hasMatch()) {
                    QMessageBox::warning(
                        this, QStringLiteral("Массовое изменение"),
                        QStringLiteral("Введите корректное числовое значение"));
                    return;
                }
            } else if (attribute.editorKind == QStringLiteral("date")
                       && !QDate::fromString(
                               change.value, Qt::ISODate).isValid()) {
                QMessageBox::warning(
                    this, QStringLiteral("Массовое изменение"),
                    QStringLiteral("Введите дату в формате ГГГГ-ММ-ДД"));
                return;
            } else if (attribute.editorKind == QStringLiteral("datetime")
                       && !QDateTime::fromString(
                               change.value, Qt::ISODate).isValid()) {
                QMessageBox::warning(
                    this, QStringLiteral("Массовое изменение"),
                    QStringLiteral(
                        "Введите дату и время в формате ГГГГ-ММ-ДД ЧЧ:ММ:СС"));
                return;
            }
            displayValue = change.value;
        }
    }

    QList<qint64> ids;
    ids.reserve(selected.size());
    for (const SelectedMapObject& object : selected) {
        ids.append(object.id);
    }
    QString versionError;
    const QList<repo::ObjectVersion> versions =
        objectRepository_.loadVersions(
            connection_.database(), classTable, ids, &versionError);
    if (!versionError.isEmpty()) {
        QMessageBox::warning(
            this, QStringLiteral("Массовое изменение"), versionError);
        loadSelectedFragment();
        return;
    }
    if (displayValue.size() > 120) {
        displayValue = displayValue.left(117) + QStringLiteral("…");
    }
    if (QMessageBox::question(
            this,
            QStringLiteral("Подтверждение массового изменения"),
            QStringLiteral(
                "Изменить поле «%1» у %2 объектов?\n\nНовое значение: %3\n\n"
                "При конфликте хотя бы одной версии не изменится ни одна строка.")
                .arg(attribute.displayName)
                .arg(selected.size())
                .arg(displayValue))
        != QMessageBox::Yes) {
        return;
    }

    batchEditButton_->setEnabled(false);
    const repo::BatchUpdateResult result = objectRepository_.batchUpdate(
        connection_.database(), classTable, versions, change);
    if (result.conflict) {
        QMessageBox::warning(
            this,
            QStringLiteral("Конфликт массового изменения"),
            QStringLiteral(
                "Один из объектов был изменён другим пользователем. "
                "Вся операция отменена, карта будет перезагружена."));
        loadSelectedFragment();
        return;
    }
    if (!result.success) {
        batchEditButton_->setEnabled(true);
        QMessageBox::critical(
            this, QStringLiteral("Ошибка массового изменения"), result.error);
        return;
    }
    QMessageBox::information(
        this,
        QStringLiteral("Массовое изменение завершено"),
        QStringLiteral("Обновлено объектов: %1").arg(result.updatedCount));
    loadSelectedFragment();
}

void MainWindow::showObjectDetails(
    qint64 id,
    QString classTable,
    bool isNode)
{
    if (!connection_.isOpen()) {
        return;
    }

    const repo::ObjectDetails details =
        objectRepository_.load(connection_.database(), classTable, id);
    if (!details.isValid()) {
        objectTitleLabel_->setStyleSheet(
            QStringLiteral("color: #b42318;"));
        objectTitleLabel_->setText(
            QStringLiteral("Ошибка карточки: %1").arg(details.error));
        objectTable_->setRowCount(0);
        saveObjectButton_->setEnabled(false);
        reloadObjectButton_->setEnabled(false);
        archiveObjectButton_->setEnabled(false);
        historyObjectButton_->setEnabled(false);
        splitLineButton_->setChecked(false);
        splitLineButton_->setEnabled(false);
        joinLinesButton_->setChecked(false);
        joinLinesButton_->setEnabled(false);
        currentObjectDetails_ = {};
        return;
    }

    displayObjectDetails(details, isNode);
}

void MainWindow::displayObjectDetails(
    const repo::ObjectDetails& details,
    bool isNode)
{
    currentObjectDetails_ = details;
    currentObjectIsNode_ = isNode;
    batchEditButton_->setEnabled(false);

    objectTitleLabel_->setStyleSheet({});
    objectTitleLabel_->setText(
        QStringLiteral("<b>%1 #%2</b><br>%3 · версия %4%5")
            .arg(details.classTable)
            .arg(details.id)
            .arg(isNode ? QStringLiteral("Узел")
                        : QStringLiteral("Линейный объект"))
            .arg(details.rowVersion)
            .arg(details.archived ? QStringLiteral(" · В АРХИВЕ")
                                  : QString()));
    objectTitleLabel_->setStyleSheet(
        details.archived ? QStringLiteral("color: #b54708;") : QString());

    objectTable_->setSortingEnabled(false);
    objectTable_->clearSpans();
    objectTable_->setRowCount(0);
    bool hasEditableAttributes = false;
    QString currentGroup;
    for (const repo::ObjectAttribute& attribute : details.attributes) {
        const QString attributeGroup =
            attribute.groupName.isEmpty()
                ? QStringLiteral("Параметры")
                : attribute.groupName;
        if (attributeGroup != currentGroup) {
            currentGroup = attributeGroup;
            const int groupRow = objectTable_->rowCount();
            objectTable_->insertRow(groupRow);
            auto* groupItem = readOnlyItem(currentGroup);
            groupItem->setFlags(Qt::ItemIsEnabled);
            groupItem->setBackground(QColor(QStringLiteral("#e2e8f0")));
            QFont groupFont = groupItem->font();
            groupFont.setBold(true);
            groupItem->setFont(groupFont);
            objectTable_->setItem(groupRow, 0, groupItem);
            objectTable_->setSpan(groupRow, 0, 1, 2);
        }

        const int row = objectTable_->rowCount();
        objectTable_->insertRow(row);
        const QString fieldLabel =
            attribute.unit.isEmpty()
                ? attribute.displayName
                : QStringLiteral("%1, %2")
                      .arg(attribute.displayName, attribute.unit);
        auto* nameItem = readOnlyItem(fieldLabel);
        nameItem->setToolTip(
            QStringLiteral("%1\nТип: %2%3")
                .arg(
                    attribute.name,
                    attribute.dataType,
                    attribute.groupName.isEmpty()
                        ? QString()
                        : QStringLiteral("\nГруппа: %1")
                              .arg(attribute.groupName)));
        QString displayValue =
            attribute.isNull ? QStringLiteral("NULL") : attribute.value;
        if (!attribute.isNull
            && attribute.editorKind == QStringLiteral("lookup")) {
            for (const repo::ObjectFieldOption& option : attribute.options) {
                if (option.value == attribute.value) {
                    displayValue = option.label;
                    break;
                }
            }
        }
        auto* valueItem = new QTableWidgetItem(displayValue);
        QStringList optionValues;
        QStringList optionLabels;
        optionValues.reserve(attribute.options.size());
        optionLabels.reserve(attribute.options.size());
        for (const repo::ObjectFieldOption& option : attribute.options) {
            optionValues.append(option.value);
            optionLabels.append(option.label);
        }
        valueItem->setData(AttributeNameRole, attribute.name);
        valueItem->setData(DatabaseTypeRole, attribute.databaseType);
        valueItem->setData(OriginalValueRole, attribute.value);
        valueItem->setData(OriginalNullRole, attribute.isNull);
        valueItem->setData(EditableRole, attribute.editable);
        valueItem->setData(EditorKindRole, attribute.editorKind);
        valueItem->setData(OptionValuesRole, optionValues);
        valueItem->setData(OptionLabelsRole, optionLabels);
        valueItem->setData(CurrentValueRole, attribute.value);
        valueItem->setData(CurrentNullRole, attribute.isNull);
        if (!attribute.editable || details.archived) {
            valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
        } else {
            hasEditableAttributes = true;
            valueItem->setToolTip(
                QStringLiteral(
                    "Дважды щёлкните для редактирования. "
                    "Введите NULL, чтобы очистить поле. Тип: %1")
                    .arg(attribute.dataType));
        }
        if (attribute.isNull) {
            valueItem->setForeground(QColor(QStringLiteral("#94a3b8")));
        }
        objectTable_->setItem(row, 0, nameItem);
        objectTable_->setItem(row, 1, valueItem);
    }
    saveObjectButton_->setEnabled(hasEditableAttributes);
    reloadObjectButton_->setEnabled(true);
    archiveObjectButton_->setEnabled(details.canArchive);
    archiveObjectButton_->setText(
        details.archived ? QStringLiteral("Восстановить")
                         : QStringLiteral("В архив"));
    historyObjectButton_->setEnabled(true);
    splitLineButton_->setEnabled(
        !isNode && !details.archived && details.canSplit
        && mapView_->hasSelectedLine(details.id, details.classTable));
    joinLinesButton_->setEnabled(
        !isNode && !details.archived && details.canJoin
        && mapView_->hasSelectedLine(details.id, details.classTable));
    objectDock_->show();
    objectDock_->raise();
    statusBar()->showMessage(
        QStringLiteral("Выбран %1 #%2")
            .arg(details.classTable)
            .arg(details.id));
}

void MainWindow::saveObjectDetails()
{
    if (!connection_.isOpen()
        || currentObjectDetails_.classTable.isEmpty()) {
        return;
    }

    QList<repo::AttributeChange> changes;
    for (int row = 0; row < objectTable_->rowCount(); ++row) {
        QTableWidgetItem* valueItem = objectTable_->item(row, 1);
        if (valueItem == nullptr
            || !valueItem->data(EditableRole).toBool()) {
            continue;
        }

        const bool originalNull =
            valueItem->data(OriginalNullRole).toBool();
        const QString originalValue =
            valueItem->data(OriginalValueRole).toString();
        const QString editorKind =
            valueItem->data(EditorKindRole).toString();
        const bool currentNull =
            editorKind == QStringLiteral("lookup")
                || editorKind == QStringLiteral("boolean")
                ? valueItem->data(CurrentNullRole).toBool()
                : valueItem->text() == QStringLiteral("NULL");
        const QString value =
            editorKind == QStringLiteral("lookup")
                || editorKind == QStringLiteral("boolean")
                ? valueItem->data(CurrentValueRole).toString()
                : valueItem->text();
        if ((originalNull && currentNull)
            || (!originalNull && value == originalValue)) {
            continue;
        }

        repo::AttributeChange change;
        change.name = valueItem->data(AttributeNameRole).toString();
        change.databaseType =
            valueItem->data(DatabaseTypeRole).toString();
        change.value = value;
        change.setNull = currentNull;
        changes.append(std::move(change));
    }

    if (changes.isEmpty()) {
        statusBar()->showMessage(QStringLiteral("Изменений нет"));
        return;
    }

    saveObjectButton_->setEnabled(false);
    const repo::UpdateResult result = objectRepository_.update(
        connection_.database(),
        currentObjectDetails_.classTable,
        currentObjectDetails_.id,
        currentObjectDetails_.rowVersion,
        changes);

    if (result.conflict) {
        reloadObjectDetails();
        QMessageBox::warning(
            this,
            QStringLiteral("Конфликт редактирования"),
            QStringLiteral(
                "Объект был изменён другим пользователем. "
                "Карточка перезагружена; повторите правку на новой версии."));
        return;
    }
    if (!result.success) {
        saveObjectButton_->setEnabled(true);
        QMessageBox::critical(
            this,
            QStringLiteral("Ошибка сохранения"),
            result.error);
        return;
    }

    reloadObjectDetails();
    statusBar()->showMessage(
        QStringLiteral("Изменения сохранены, версия %1")
            .arg(result.rowVersion));
}

void MainWindow::reloadObjectDetails()
{
    if (currentObjectDetails_.classTable.isEmpty()) {
        return;
    }
    showObjectDetails(
        currentObjectDetails_.id,
        currentObjectDetails_.classTable,
        currentObjectIsNode_);
}

void MainWindow::toggleObjectArchive()
{
    if (!connection_.isOpen()
        || currentObjectDetails_.classTable.isEmpty()
        || !currentObjectDetails_.canArchive) {
        return;
    }

    const bool archive = !currentObjectDetails_.archived;
    const QString action =
        archive ? QStringLiteral("переместить в архив")
                : QStringLiteral("восстановить");
    if (QMessageBox::question(
            this,
            archive ? QStringLiteral("Архивирование объекта")
                    : QStringLiteral("Восстановление объекта"),
            QStringLiteral("Действительно %1 объект %2 #%3?")
                .arg(action, currentObjectDetails_.classTable)
                .arg(currentObjectDetails_.id))
        != QMessageBox::Yes) {
        return;
    }

    archiveObjectButton_->setEnabled(false);
    const repo::UpdateResult result = objectRepository_.setArchived(
        connection_.database(),
        currentObjectDetails_.classTable,
        currentObjectDetails_.id,
        currentObjectDetails_.rowVersion,
        archive);
    if (result.conflict) {
        reloadObjectDetails();
        QMessageBox::warning(
            this,
            QStringLiteral("Конфликт редактирования"),
            QStringLiteral(
                "Состояние объекта уже изменено другим пользователем. "
                "Карточка перезагружена."));
        return;
    }
    if (!result.success) {
        archiveObjectButton_->setEnabled(true);
        QMessageBox::critical(
            this,
            QStringLiteral("Ошибка"),
            result.error);
        return;
    }

    reloadObjectDetails();
    refreshArchive();
    QTreeWidgetItem* selectedFragment = fragmentTree_->currentItem();
    if (selectedFragment != nullptr
        && selectedFragment->data(0, Qt::UserRole).isValid()
        && !mapWatcher_->isRunning()) {
        preserveObjectAfterMapLoad_ = true;
        loadSelectedFragment();
    }
    statusBar()->showMessage(
        archive ? QStringLiteral("Объект перемещён в архив")
                : QStringLiteral("Объект восстановлен"));
}

void MainWindow::showObjectHistory()
{
    if (!connection_.isOpen()
        || currentObjectDetails_.classTable.isEmpty()) {
        return;
    }

    QString error;
    const QList<repo::ObjectHistoryEntry> entries =
        objectRepository_.loadHistory(
            connection_.database(),
            currentObjectDetails_.classTable,
            currentObjectDetails_.id,
            &error);
    if (!error.isEmpty()) {
        QMessageBox::critical(
            this, QStringLiteral("Ошибка истории"), error);
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(
        QStringLiteral("История %1 #%2")
            .arg(currentObjectDetails_.classTable)
            .arg(currentObjectDetails_.id));
    dialog.resize(980, 440);
    auto* layout = new QVBoxLayout(&dialog);
    auto* table = new QTableWidget(&dialog);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({
        QStringLiteral("Версия"),
        QStringLiteral("Операция"),
        QStringLiteral("Время"),
        QStringLiteral("Пользователь"),
        QStringLiteral("Приложение"),
        QStringLiteral("Изменённые поля"),
    });
    table->setRowCount(entries.size());
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    for (qsizetype row = 0; row < entries.size(); ++row) {
        const repo::ObjectHistoryEntry& entry = entries.at(row);
        QString operation = entry.operation;
        if (operation == QStringLiteral("update")) {
            operation = QStringLiteral("изменение");
        } else if (operation == QStringLiteral("archive")) {
            operation = QStringLiteral("архив");
        } else if (operation == QStringLiteral("restore")) {
            operation = QStringLiteral("восстановление");
        }
        table->setItem(
            row, 0, readOnlyItem(QString::number(entry.rowVersion)));
        table->setItem(row, 1, readOnlyItem(operation));
        table->setItem(row, 2, readOnlyItem(entry.changedAt));
        table->setItem(row, 3, readOnlyItem(entry.changedBy));
        table->setItem(row, 4, readOnlyItem(entry.applicationName));
        table->setItem(row, 5, readOnlyItem(entry.changedFields));
    }
    table->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    table->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(table);
    auto* buttons =
        new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    connect(buttons, &QDialogButtonBox::rejected,
            &dialog, &QDialog::reject);
    layout->addWidget(buttons);
    dialog.exec();
}

void MainWindow::refreshArchive()
{
    if (!connection_.isOpen()) {
        archiveTable_->setRowCount(0);
        return;
    }

    refreshArchiveButton_->setEnabled(false);
    QString error;
    const QList<repo::ArchivedObjectInfo> objects =
        objectRepository_.loadArchived(connection_.database(), &error);
    if (!error.isEmpty()) {
        archiveTable_->setRowCount(0);
        refreshArchiveButton_->setEnabled(true);
        statusBar()->showMessage(
            QStringLiteral("Ошибка архива: %1").arg(error));
        return;
    }

    archiveTable_->setSortingEnabled(false);
    archiveTable_->setRowCount(objects.size());
    for (qsizetype row = 0; row < objects.size(); ++row) {
        const repo::ArchivedObjectInfo& object = objects.at(row);
        auto* classItem = readOnlyItem(object.classTable);
        classItem->setData(Qt::UserRole, object.id);
        classItem->setData(Qt::UserRole + 1, object.isNode);
        archiveTable_->setItem(row, 0, classItem);
        archiveTable_->setItem(
            row, 1, readOnlyItem(QString::number(object.id)));
        archiveTable_->setItem(
            row, 2,
            readOnlyItem(
                object.fragmentId == 0
                    ? QString()
                    : QString::number(object.fragmentId)));
        archiveTable_->setItem(row, 3, readOnlyItem(object.label));
        archiveTable_->setItem(row, 4, readOnlyItem(object.archivedAt));
    }
    archiveTable_->setSortingEnabled(true);
    refreshArchiveButton_->setEnabled(true);
}

void MainWindow::openArchivedObject(int row, int column)
{
    Q_UNUSED(column);
    QTableWidgetItem* item = archiveTable_->item(row, 0);
    if (item == nullptr) {
        return;
    }
    showObjectDetails(
        item->data(Qt::UserRole).toLongLong(),
        item->text(),
        item->data(Qt::UserRole + 1).toBool());
}

void MainWindow::clearObjectDetails()
{
    objectTitleLabel_->setStyleSheet({});
    objectTitleLabel_->setText(
        QStringLiteral("Выберите объект на карте"));
    objectTable_->setRowCount(0);
    saveObjectButton_->setEnabled(false);
    reloadObjectButton_->setEnabled(false);
    archiveObjectButton_->setEnabled(false);
    historyObjectButton_->setEnabled(false);
    splitLineButton_->setChecked(false);
    splitLineButton_->setEnabled(false);
    joinLinesButton_->setChecked(false);
    joinLinesButton_->setEnabled(false);
    batchEditButton_->setEnabled(false);
    currentObjectDetails_ = {};
    currentObjectIsNode_ = false;
}

void MainWindow::showLayers(
    const QList<repo::LayerInfo>& layers,
    const db::SchemaStatus& status)
{
    layerTable_->setSortingEnabled(false);
    layerTable_->setRowCount(layers.size());

    for (qsizetype row = 0; row < layers.size(); ++row) {
        const repo::LayerInfo& layer = layers.at(row);
        layerTable_->setItem(
            row, 0, readOnlyItem(layer.displayName));
        layerTable_->setItem(
            row, 1, readOnlyItem(layer.schemaName));
        layerTable_->setItem(
            row, 2, readOnlyItem(layer.tableName));
        layerTable_->setItem(
            row, 3, readOnlyItem(layer.geometryType));
        layerTable_->setItem(
            row, 4, readOnlyItem(QString::number(layer.srid)));
        layerTable_->setItem(
            row, 5,
            readOnlyItem(layer.editable ? QStringLiteral("да")
                                        : QStringLiteral("нет")));
        layerTable_->setItem(
            row, 6, readOnlyItem(layer.kind));
    }

    layerTable_->setSortingEnabled(true);
    statusLabel_->setStyleSheet(QStringLiteral("color: #067647;"));
    statusLabel_->setText(
        QStringLiteral(
            "Подключено к %1 от имени %2. Версия схемы: %3. Слоёв: %4.")
            .arg(status.database, status.user)
            .arg(status.version)
            .arg(layers.size()));
    statusBar()->showMessage(QStringLiteral("Контракт БД проверен"));
    refreshButton_->setEnabled(true);
}

}  // namespace tgid::ui
