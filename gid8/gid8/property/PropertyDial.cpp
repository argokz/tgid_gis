#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "PropertyDial.h"
#include <db/db.h>
#include <cxema/cxema1.h>
#include <mystd.h>
#include <mainwindow.h>

#include "qvariant.h"
#include "variantdelegate.h"
#include <dialog/TableWindow.h>

#include "CustomdateEdit.h"
#include "HelpDialog.h"

void setFontElement(QWidget *widget, QFont &font);

void read_tab(const QString & table, std::list<QString> &lst);
void read_fun(const QString & table, const QString & ext, std::map<QString, QString> &map_fun);
void read_disable(const QString & table, const QString & ext, std::map <QString, std::map<QString, std::list<QString> > > &map_disable);

QString create_select_all(QSqlDatabase & db, const QString & table, const QString & table_file, int id1, int id2, PrTypes pr_type, const QString & s_shape, const QString & s_id);

void setBold(QWidget *widget)
{
    QFont font = widget->font();
    font.setWeight(QFont::Bold);
    widget->setFont(font);
}

QVariant text_to_variant(const QString & text0, QMetaType type)
{
    QVariant value;
    QString text = text0;

    if (type == QMetaType(QMetaType::Double)) {
        text.replace(",", ".");
        return text.toDouble();
    }
    if (type == QMetaType(QMetaType::QDate)) {
        QVariant val = QDate::fromString(text, "dd.MM.yyyy");
        return val;
    }
    if (type == QMetaType(QMetaType::QDateTime)) {
        QVariant val = QDateTime::fromString(text, "dd.MV.yyyy");
        return val;
    }

    value = text;

    return value;
}



QWidget * makeLabelWidget(QWidget *dialog, int num, const QString & rus, const QString & f1)
{
    QWidget *labelWidget = new QWidget();
    QHBoxLayout *labelLayout = new QHBoxLayout();
    labelLayout->setContentsMargins(0,0,0,0);
    labelLayout->setSpacing(0);

    QLineEdit * numberEdit = new QLineEdit;
    QLineEdit * labelEdit = new QLineEdit;

    numberEdit->setProperty("class", "number");
    labelEdit->setProperty("class", "label");

    numberEdit->setText(QString("%1").arg(num++));
    numberEdit->setFixedWidth (40);
    numberEdit->setFocusPolicy(Qt::NoFocus);

    labelEdit->setText(rus);
    labelEdit->setFocusPolicy(Qt::NoFocus);
    labelEdit->setCursorPosition(0);

    if (f1 == "") {
        labelEdit->setProperty("f1", rus);
    }
    else {
        labelEdit->setProperty("f1", f1);
    }


    labelEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    labelEdit->connect(labelEdit, SIGNAL(customContextMenuRequested(QPoint)), dialog, SLOT(helpMenu(QPoint)));


//    labelLayout->addWidget(numberEdit);
    labelLayout->addWidget(labelEdit);

    labelWidget->setLayout(labelLayout);

    return labelWidget;
}


#include "date/CustomDateInput.h"

QWidget * makeDateWidget(
    QWidget *dialog,
    PropertyTree *tree,
    PropertyStr &pr
)
{
//    CustomDateEdit *dateEdit = new CustomDateEdit;
//    NullableDateEdit *dateEdit = new NullableDateEdit;

    CustomDateInput *dateEdit = new CustomDateInput;


    if (!pr.value.isNull()) {
        dateEdit->setDate(pr.value.toDate());
    }
//    else {
//        dateEdit->setSpecialValueText( ".." );
//        dateEdit->setDate( QDate::fromString( "01.01.0001", "dd.MM.yyyy" ) );
//    }
/*
    dateEdit->setCalendarPopup(true);
*/
    dateEdit->installEventFilter(dialog);


/*
    tree->connect(dateEdit, &NullableDateEdit::editingFinished, [dateEdit]() {
        QString txt = dateEdit->text();
        if (txt.isEmpty()) {
            dateEdit->setDate(dateEdit->minimumDate()); // Устанавливаем "пустое" значение
        }
    });
*/
/*
// Обработка начала редактирования (подстановка текущей даты)
    QObject::connect(dateEdit, &QDateEdit::focusIn, [dateEdit](QFocusEvent *) {
        if (dateEdit->date() == dateEdit->minimumDate()) {
            dateEdit->setDate(QDate::currentDate());
        }
    });
*/
#if 0

QObject::connect(dateEdit->lineEdit(), &QLineEdit::textChanged, [dateEdit](const QString &text) {
    if (text.isEmpty()) {
        dateEdit->setDate(dateEdit->minimumDate()); // Устанавливаем минимальную дату
    }
});

    tree->connect(dateEdit, &NullableDateEdit::editingFinished, [dateEdit]() {
// Проверяем, пустое ли поле ввода
        if (dateEdit->lineEdit()->text().isEmpty() || dateEdit->lineEdit()->text() == dateEdit->specialValueText()) {
            dateEdit->setDate(dateEdit->minimumDate()); // Устанавливаем минимальную дату
        }

/*
        QString txt = dateEdit->text();
        if (txt.isEmpty()) {
            dateEdit->setDate(dateEdit->minimumDate()); // Устанавливаем "пустое" значение
        }
*/
    });
#endif

    tree->connect(dateEdit, SIGNAL(dateChanged(QDate)),
                  dialog, SLOT(dateChanged(QDate)));


//    pr.dateEdit = dateEdit;
    pr.widget = dateEdit;

    return dateEdit;
}



QPixmap pixmap_from_text(const QString & str, int cx, int cy)
{
    QPixmap pixmap(cx,cy);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.drawText(0,0,cx,cy,Qt::AlignHCenter | Qt::AlignVCenter, str);

    return pixmap;
}

QIcon *icon_from_text(const QString & str, int cx, int cy)
{
    QIcon *actionIcon = new QIcon;
    QSize sz(cx, cy);

    QPixmap pixmap = pixmap_from_text(str, cx, cy);
//    actionIcon.addPixmap(dialog->style()->standardIcon(QStyle::SP_TitleBarUnshadeButton).pixmap(sz));
    actionIcon->addPixmap(pixmap);

    return actionIcon;
}



QWidget * makeEditWidget(
    QWidget *dialog,
    PropertyTree *tree,
    PropertyStr &pr,
    bool readOnly
)
{

    if (pr.metaType.id() == QMetaType::QDate || pr.metaType.id() == QMetaType::QDateTime) {
        QWidget *widget = makeDateWidget(dialog, tree, pr);
        widget->setProperty("col", QVariant::fromValue(pr));
        return widget;
    }

    if (pr.metaType.id() == QMetaType::QDate || pr.metaType.id() == QMetaType::QDateTime) {
        pr.isFunc = true;
        pr.fun = "Date";
    }

    if (pr.fun == "ReadOnly") {
        pr.readOnly = true;
        readOnly = true;
    }

    if (pr.lookup2.size() > 0) {
        if (readOnly) {
            QLineEdit *widget = new QLineEdit();
            widget->setProperty("class", "combo_readonly");


            widget->setReadOnly(true);
//            widget->setText(pr.value.toString());
            widget->setText(VariantDelegate::displayText(pr.value));
            pr.widget = widget;
            widget->setProperty("col", QVariant::fromValue(pr));
            widget->installEventFilter(dialog);
            return widget;
        }
        else {
            QComboBox *widget = new QComboBox;
            widget->setProperty("class", "combo");

            if (readOnly) {
                widget->setEnabled(false);
            }

            widget->setFocusPolicy( Qt::StrongFocus );
            widget->installEventFilter( dialog );

            int index = -1;
            int i = 0;

            for (auto &it : pr.lookup2) {
                widget->addItem(it.second, it.first);
                if (it.first == pr.index) {
                    index = i;
                }
                i++;
            }
//            widget->setCurrentText(pr.value.toString());

//            widget->setCurrentText(VariantDelegate::displayText(pr.value));
            widget->setCurrentIndex(index);

            pr.widget = widget;
            widget->setProperty("col", QVariant::fromValue(pr));

            tree->connect(widget, SIGNAL(currentTextChanged(const QString &)),
                dialog, SLOT(currentTextChanged(const QString &)));

            tree->connect(widget, SIGNAL(currentIndexChanged(int)),
                dialog, SLOT(currentIndexChanged(int)));

            widget->setContextMenuPolicy(Qt::CustomContextMenu);
            tree->connect(widget, SIGNAL(customContextMenuRequested(QPoint)), dialog, SLOT(contextMenu(QPoint)));

            widget->installEventFilter(dialog);
            return widget;
        }
    }

    TypeChecker typeChecker;
    QLineEdit * lineEdit = VariantDelegate::createLineEditor(tree, &typeChecker, pr.value, pr.metaType);

    pr.widget = lineEdit;
    lineEdit->setProperty("col", QVariant::fromValue(pr));

    tree->connect(lineEdit, SIGNAL(textChanged(const QString &)),
        dialog, SLOT(textChanged(const QString &)));

//    tree->connect(lineEdit, SIGNAL(rightClicked()), dialog, SLOT(rightClicked()));

    lineEdit->installEventFilter(dialog);

    if (readOnly) {
        lineEdit->setReadOnly(true);
        lineEdit->setProperty("class", "edit");
    }
    else {
        lineEdit->setProperty("class", "readonly");
    }

    if (pr.isFunc || pr.isLookup) {
        if (pr.isLookup) {
            lineEdit->setReadOnly(true);
            lineEdit->setFocusPolicy(Qt::StrongFocus);

            lineEdit->setContextMenuPolicy(Qt::CustomContextMenu);
            tree->connect(lineEdit, SIGNAL(customContextMenuRequested(QPoint)), dialog, SLOT(contextMenu(QPoint)));

//            tree->connect(lineEdit, SIGNAL(clicked()), dialog, SLOT(rightClicked()));
        }
        if (pr.fun == "ReadOnly") {
            lineEdit->setReadOnly(true);
            lineEdit->setFocusPolicy(Qt::StrongFocus);
        }
        else {
//            QIcon *actionIcon = icon_from_text("...", 16, 16);
            QIcon *actionIcon = new QIcon(":images4/three-dots-menu.svg");

    //        QAction *myAction = lineEdit->addAction(QIcon(":/images/gidr.ico"), QLineEdit::TrailingPosition);
            QAction *myAction = lineEdit->addAction(*actionIcon, QLineEdit::TrailingPosition);
            myAction->setProperty("col", QVariant::fromValue(pr));
            tree->connect(myAction, SIGNAL(triggered()), dialog, SLOT(clicked()));
        }
    }

    return lineEdit;
}

QVariant PropertyStr::getValue()
{
    if (this->isLookup) {
        if (this->index == -1) {
            return QVariant();
        }
        else {
            return QVariant(this->index);
        }
    }

    if (this->value.toString() == "" && this->value.typeId() != QMetaType::QString) {
        return QVariant();
    }

/*

    if (this->metaType == QMetaType(QMetaType::QDate)) {
        QDateEdit *dt = (QDateEdit *)this->widget;
        if (dt->date() == dt->minimumDate()) {
            return QVariant();
        }
    }
*/

    return this->value;
}



QString txt_to_html(const QString & txt0)
{
    QString txt = txt0;

    txt.replace("\n","</p>\n<p>");
    txt = "<html><p>"+txt+"</p></html>";
    return txt;
}

QString txt_to_html(const QString & txt0, const QString & head0)
{
    QString txt = txt0;

    txt.replace("\n","</p>\n<p>");
    txt = "<html><h2>" + head0 +  "</h2><p>" + txt + "</p></html>";
    return txt;
}



void PropertyDial::PropertyDial::addRow(
        QTreeWidgetItem *item_line,
        PropertyTree *tree,
        QSqlDatabase *db,
        QVariant value,
        int id_col,
        int index,
        int fileID,
        const QMetaType &metaType,
        QString table,
        int id,
        QString col,
        QString rus,
        QString f1,
        bool readOnly,
        bool listOnly,
        std::map<int, QString> &lookup2,
        const QString &title1,
        const QString &title2
)
{
    PropertyStr pr;
    pr.item_line = item_line;
    pr.value = value;
    pr.old_value = value;
    pr.index = index;
    pr.fileID = fileID;
    pr.metaType = metaType;
    pr.table = table;
    pr.key = col;
    pr.label = rus;
    pr.f1 = f1;
    pr.id = id;
    pr.id_col = id_col;
    pr.row = 0;
    pr.db = db;
    pr.m_cxema = m_cxema;

    pr.title1 = title1;
    pr.title2 = title2;

    if (!readOnly) {
        auto itfun = map_fun.find(col);
        if (itfun != map_fun.end()) {
            pr.isFunc = true;
            pr.fun = itfun->second;
        }

        {
            auto it = map_validate.find(col);
            if (it != map_validate.end()) {
                pr.validate = it->second;
            }
        }

        {
            auto it = map_after.find(col);
            if (it != map_after.end()) {
                pr.after = it->second;
            }
        }

        {
            auto it = map_before.find(col);
            if (it != map_before.end()) {
                pr.before = it->second;
            }
        }

        const ForeignTable *ft = findLookup(getDatabaseName(*db), table, col);
        if (ft) {
            pr.isLookup = true;
            pr.lookup = ft;
        }

        std::map<int, QString> l2; // = nullptr;

//        if (lookup2 && lookup2->size() > 0) {
        if (lookup2.size() > 0) {
            l2 = lookup2;
        }

        if (ft && l2.size() == 0) {
            auto *ll = findLookup2(getDatabaseName(*db), ft->table);
            if (ll) {
                l2 = *ll;
            }
        }

        if (l2.size() > 0) {
            pr.isLookup2 = true;
            pr.lookup2 = l2;
        }
    }

    QHBoxLayout *editLayout = new QHBoxLayout();
    editLayout->setContentsMargins(0,0,10,0);
    editLayout->setSpacing(0);

//    QWidget *lineEdit = makeEditWidget(this, tree, pr, readOnly || listOnly);
    QWidget *lineEdit = makeEditWidget(this, tree, pr, readOnly);

    QString ff1 = rus;

    if (f1 !=  "") {
        ff1 = txt_to_html(f1);
    }

    lineEdit->setToolTip(ff1);

    editLayout->addWidget(lineEdit);

    QWidget *itemWidget = new QWidget();
    itemWidget->setLayout(editLayout);

    itemWidget->setProperty("class", "item");

    tree->setItemWidget(item_line,id_col+1,itemWidget);
    list_widget.push_back(lineEdit);

//    map_prop[pr.key] = &pr;
}

void PropertyDial::PropertyDial::addButton(
        QTreeWidgetItem *item_line,
        PropertyTree *tree,
        QSqlDatabase *db,
        int id_col,
        int index,
        int fileID,
        QString table,
        int id,
        QString col,
        QString rus,
        QString f1
)
{
    QHBoxLayout *editLayout = new QHBoxLayout();
    editLayout->setContentsMargins(0,0,0,0);
    editLayout->setSpacing(0);

//    QWidget *lineEdit = makeEditWidget(this, tree, pr, readOnly);
    QPushButton *button = new QPushButton();
    button->setProperty("class", "ellipsis");

//    button->setText("...");

    QIcon icon(":images4/menu-svgrepo-com.svg");

    button->setIcon(icon);

    button->setToolTip(txt_to_html(f1));

    button->setProperty("col", col);
    button->setProperty("id", id_col == 0 ? m_id1 : m_id2);
    button->setProperty("db", QVariant::fromValue(db));
    button->setProperty("label", rus);

    tree->connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    editLayout->addWidget(button);

    QWidget *itemWidget = new QWidget();
    itemWidget->setLayout(editLayout);
    itemWidget->setProperty("class", "item");

    tree->setItemWidget(item_line,id_col+1,itemWidget);
    list_widget.push_back(button);
}

template<typename... Args>
QDebug operator<<(QDebug dbg, const std::tuple<Args...>& t)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "(";

    std::apply([&dbg](const auto&... args) {
        int n = 0;
        ((dbg.nospace() << args
                        << (++n < sizeof...(Args) ? ", " : "")), ...);
    }, t);

    dbg.nospace() << ")";
    return dbg;
}


void PropertyDial::fillValues(
        const QString &title1,
        std::list<QString> &tab_lst,
        PropertyTree *tree,
        QSqlDatabase *db,
        int tab_num,
        const QString &table,
        const QString &prefix,
        bool readOnly,
        bool noZero  // Не показывать нули. Для результатов расчета
        )
{
    QTreeWidgetItem *parent = nullptr;
    QTreeWidgetItem *item_title = nullptr;
    QTreeWidgetItem *item_line = nullptr;

    int tab = 0;
    int num = 1;

//    QString title1 = "";
    QString title2 = "";

/*
    qDebug() << "======================================";

    for (auto &[key, val] : map_val) {
        qDebug() <<  key << "--" << val;
    }

    qDebug() << "======================================";
*/

    TypeChecker typeChecker;

    for (auto & line : tab_lst) {
        if (line.left(2) == "-!") {
            tab ++;
            continue;
        }

        if (tab != tab_num && tab_num != -1) {
            continue;
        }

        if (line.left(1) == "-") continue;
        if (line.left(1) == " ") {
            QString col = "", rus = "", f1 = "";

            static QRegularExpression re("^\\s+([^ ]+)\\s+(.+)\\s*$");
            QRegularExpressionMatch match = re.match(line);
            if (match.hasMatch()) {
                col = match.captured(1);
                rus = match.captured(2);
            }
            else {
                rus = col = line.trimmed();
                const CColName *c = findColumnRusNameFull(getDatabaseName(*db), table, col);

                if (c) {
                    rus = c->name;
                    f1 = c->name_full;
                }
            }


            const CColName *c = findColumnRusNameFull(getDatabaseName(*db), table, col);

            if (c) {
//                rus = c->name;
                f1 = c->name_full;
            }


            if (col == s_id) continue;
            if (col == s_shape) continue;

            if (col == "shape_leng") continue;
            if (col == "shape_area") continue;

            if (noZero) {
                if (rus.toLower() == "calculationid") continue;
                if (rus.toLower() == "lineid") continue;
                if (rus.toLower() == "nodeid") continue;
            }

            if (rus == "") continue;

            bool is_button = false;

            if (col.left(1) == "$") {
                is_button = true;
            }

            std::tuple<QSqlField, int> vP, vO;
            std::tuple<QString, QString, QString, int, int> k1(table, col, prefix, 0, 0);
            std::tuple<QString, QString, QString, int, int> k2(table, col, prefix, 1, 0);

            std::tuple<QString, QString, QString, int, int> ki1(table, col, prefix, 0, 1);
            std::tuple<QString, QString, QString, int, int> ki2(table, col, prefix, 1, 1);

            int index1 = -1;
            auto iit1 = map_val.find(ki1);
            if (iit1 != map_val.end()) {
                auto v = iit1->second;
                index1 = std::get<0>(v).value().toInt();
            }
            int index2 = -1;
            auto iit2 = map_val.find(ki2);
            if (iit2 != map_val.end()) {
                auto v = iit2->second;
                index2 = std::get<0>(v).value().toInt();
            }

            QSqlField field1, field2;
            int id1 = -1, id2 = -1;
            int fileID1 = -1, fileID2 = -1;

            auto it1 = map_val.find(k1);
            if (it1 != map_val.end()) {
                auto v = it1->second;
                field1 = std::get<0>(v);
                id1 = std::get<1>(v);
                fileID1 = std::get<2>(v);
            }

            auto it2 = map_val.find(k2);
            if (it2 != map_val.end()) {
                auto v = it2->second;
                field2 = std::get<0>(v);
                id2 = std::get<1>(v);
                fileID2 = std::get<2>(v);
            }

            if (noZero && (field1.value().toString() == "0" || id1 == -1) && (field2.value().toString() == "0" || id2 == -1)) continue;

            item_line = new QTreeWidgetItem(item_title);
//title1 = item_title;

            QWidget * ww = makeLabelWidget(this, num, rus, f1);

            tree->setItemWidget(item_line,0, ww);

            int id_col = 0;

            std::map<int, QString> map_lookup;

            if (is_button) {
                addButton(item_line, tree, db,
                    id_col++,
                    index1,
                    fileID1,
                    table, id1, col, rus, f1);
            }
            else {
                bool readOnly2 = readOnly;

                if (id1 != id2 && id1 != -1 && id2 != -1) {
                    if (col.toLower() == "externalsignlineid") {
                        readOnly2 = true;
                    }
                }

                if (table.toLower() == "linesobj" && (col.toLower() == "externalcodeid" || col.toLower() == "externalnodename")) {
                    readOnly2 = true;
                }

                if (table.toLower() == "dr_out" && (col.toLower() == "comment")) {
                    readOnly2 = false;
                }



                if (id1 != -1) {
                    bool readOnly3 = readOnly2;
                    if (id1 != id2 && id1 != -1 && id2 != -1) {
                        if (col.toLower() == "pipesectstateidret") {
                            readOnly3 = true;
                        }
                    }

                    addRow(item_line, tree, db,
                        field1.value(),
                        id_col++,
                        index1,
                        fileID1,
                        field1.metaType(), table, id1, col, rus, f1, readOnly3, false, map_lookup, title1, title2);
                }

                if (id2 != -1) {
                    bool readOnly3 = readOnly2;

                    if (id1 != id2 && id1 != -1 && id2 != -1) {
                        if (col.toLower() == "pipesectstateidflow") {
                            readOnly3 = true;
                        }
                    }

                    addRow(item_line, tree, db,
                        field2.value(),
                        id_col++,
                        index2,
                        fileID2,
                        field1.metaType(), table, id2, col, rus, f1,  readOnly3, false, map_lookup, title1, title2);
                }
            }

            num ++;
        }
        else {
            item_title = new QTreeWidgetItem(tree);

            QLineEdit *widget = new QLineEdit();
            widget->setText(line);
            widget->setProperty("class", "title");
            widget->setReadOnly(true);
            tree->setItemWidget(item_title, 0,widget);
            title2 = line;
        }
    }
}

void parse_col(const QString & col10, QString &col, QString &prefix, int &is_lookup)
{
    QString col1 = col10;

    col = col1;
    prefix = "";
    is_lookup = 0;

    if (col1.right(2) == "||") {
        is_lookup = 1;
        col1 = col1.left(col1.length()-2);
    }

    int n = col1.indexOf("||");
    if (n != -1) {
        col = col1.left(n);
        prefix = col1.mid(n+2);
    }
    else {
        col = col1;
    }
}

void init_fields_db(QSqlDatabase &db, const QString & table, std::list<QString> &tab_lst, const QString & s_id)
{
    std::list<AdoField> fields;
    get_table_columns(db, table, fields);

    tab_lst.push_back(QObject::tr("-!Основные"));
    tab_lst.push_back(QObject::tr("Основные"));
    for (auto & field : fields) {
        if (field.COLUMN_NAME != s_id) {
            if (field.DATA_TYPE != "geometry") {
                tab_lst.push_back("  "+field.COLUMN_NAME);
            }
        }
    }
}

bool PropertyDial::init_fields(QSqlDatabase &db, const QString & table, const QString & table_file, int id1, int id2, PrTypes pr_type)
{
    m_id1 = id1;
    m_id2 = id2;

//    table_file = this->m_table_file;
//    this->m_table_file = table;

    read_tab(table_file, this->tab_lst);

    if (this->tab_lst.size() == 0) {
        init_fields_db(db, table, this->tab_lst, s_id);
    }

    read_disable(table_file, "disable", map_disable);
    read_disable(table_file, "vis", map_vis);

    read_fun(table_file, "fun", map_fun);
    read_fun(table_file, "validate", map_validate);
    read_fun(table_file, "after",    map_after);
    read_fun(table_file, "before",   map_before);

    QString q2 = create_select_all(db, table, table_file, id1, id2, pr_type, s_shape, s_id);

    if (m_cxema) {
//        q.replace("$FragmentIds$", prop->getCxema()->m_par);
        q2.replace("AND n.fileID in ($fragments$)", "");
        q2.replace("$fragments$", m_cxema->m_par);
    }

    q2.replace("$id$", QString("%1").arg(id1));
    q2.replace("$table$", table);

//    QInputDialog::getMultiLineText(nullptr, "title", "label", q2);

    int idd = 0;

    QSqlQuery query(db);

    if (query_exec(db, query, q2)) {
        while (query.next()) {
            QSqlRecord record = query.record();
            for (int k = 0; k < record.count(); k++) {
                int id = record.field(0).value().toInt();

//                if (id == id1) idd = 0;
//                if (id == id2) idd = 1;

                QSqlField field = record.field(k);

                int mtyp = field.metaType().id();

                if (record.isNull(k)) {
                    field.clear();
                }

                QString col = field.name();
                QString prefix;
                int is_lookup = 0;
                parse_col(col, col, prefix, is_lookup);

                QString tab = table;

                if (prefix != "") {
                    id = record.field(QString("%1_ID").arg(prefix)).value().toInt();
                    if (prefix == "N1" || prefix == "N2" || prefix == "N") {
                        tab = "nodes";
                    }
                    if (prefix == "L") {
                        tab = "linesobj";
                    }
                }

                int fileID = record.field("fileID||N||").value().toInt();
                if (fileID == 0) {
                    fileID = record.field("fileID||N1||").value().toInt();
                }
                if (fileID == 0) {
                    fileID = record.field("fileID||").value().toInt();
                }

                std::tuple<QString, QString, QString, int, int> ky(tab, col, prefix, idd, is_lookup);
                std::tuple<QSqlField, int, int> v(field, id, fileID);
                map_val[ky] = v;


                qDebug() << "map_val "<< tab << " " << col << " "  << prefix  << " "  << idd << ""  << is_lookup << " >>> " << field;

            }
            idd += 1;
        }
    }
    else {
        void ErrorMessageText(QSqlQuery &query, QString &q, QString &text);

        QString q, text;

        ErrorMessageText(query, q, text);

        QInputDialog::getMultiLineText(nullptr, "Error", text, q);
//        ErrorMessage(query);
    }
    return true;
}

bool PropertyDial::_init_table(QTabWidget *tabWidget, QSqlDatabase &db, const QString & table, PrTypes pr_type, int tab_num0, int &tab_num)
{
//    int tab_num = 1;

    QString title1 = "";

    int n_cols = 2;

    if (m_id1 != m_id2 && m_id1 > 0 && m_id2 > 0) {
        n_cols = 3;
    }

    bool uuid = column_in_table(db, "linesobj", "globalid");

    for (auto & line: this->tab_lst) {
        if (line.left(2) == "-!") {
            QString txt = line.mid(2);
            title1 = txt;

            PropertyTree *tree = new PropertyTree(tab_num);
            m_tree = tree;

            tree->setColumnCount(n_cols);

            if (tab_num == tab_num0) {
                if (pr_type == pr_type_node) {
                    std::list<QString> tab_lst1;
                    tab_lst1.push_back("Узел");
                    tab_lst1.push_back(" externalCodeID");
                    tab_lst1.push_back(" externalNodeName");
                    tab_lst1.push_back(" externalSignID");
                    tab_lst1.push_back(" geoMarkTopTube");
                    tab_lst1.push_back(" geoMarkNodeArea");

/*
                    if (m_table != "nodes") {
                        tab_lst1.push_back("Производственная служба");
                        tab_lst1.push_back(" nodeTypeID");
                        tab_lst1.push_back(" organizationID");
                        tab_lst1.push_back(" inventNumber");
                        tab_lst1.push_back(" registNumber");

                        tab_lst1.push_back("Даты эксплуатации");
                        tab_lst1.push_back(" PICdate");
                        tab_lst1.push_back(" lastRepairDate");
                        tab_lst1.push_back("Архив");
                        tab_lst1.push_back(" archiveChangeDate");
                        tab_lst1.push_back(" operatorID");
                    }
*/
                    fillValues(title1, tab_lst1, tree, &db, 0, "nodes", "N");
                }
                if (pr_type == pr_type_line) {
                    std::list<QString> tab_lst1;
                    tab_lst1.push_back("Начальный узел");
                    tab_lst1.push_back(" externalCodeID");
                    tab_lst1.push_back(" externalNodeName");
                    fillValues(title1, tab_lst1, tree, &db, 0, "nodes", "N1", true);
                    std::list<QString> tab_lst2;
                    tab_lst2.push_back("Конечный узел");
                    tab_lst2.push_back(" externalCodeID");
                    tab_lst2.push_back(" externalNodeName");
                    fillValues(title1, tab_lst2, tree, &db, 0, "nodes", "N2", true);
                    std::list<QString> tab_lst3;
                    tab_lst3.push_back("Общая информация");
                    tab_lst3.push_back(" externalSignLineID");
                    tab_lst3.push_back(" organizationID");

                    if (uuid) {
//                        tab_lst3.push_back(" gistable");
//                        tab_lst3.push_back(" globalid");
                    }

                    if (!this->pts) {
                        tab_lst3.push_back(" hydroRes");
                    }
                    fillValues(title1, tab_lst3, tree, &db, 0, "linesobj", "L");
                }
            }

            fillValues(title1, this->tab_lst, tree, &db, tab_num - tab_num0 + 1, table, "");

            if (tab_num == tab_num0) {
                if (pr_type == pr_type_node) {
                    std::list<QString> tab_lst1;
                    if (m_table != "nodes" && m_table != "setPressNodes") {
                        tab_lst1.push_back("Производственная служба");
                        tab_lst1.push_back(" nodeTypeID");
                        tab_lst1.push_back(" organizationID");
                        tab_lst1.push_back(" inventNumber");
                        tab_lst1.push_back(" registNumber");

                        tab_lst1.push_back("Даты эксплуатации");
                        tab_lst1.push_back(" PICdate");
                        tab_lst1.push_back(" lastRepairDate");
                        tab_lst1.push_back("Архив");
                        tab_lst1.push_back(" archiveChangeDate");
                        tab_lst1.push_back(" operatorID");
                    }

                    fillValues(title1, tab_lst1, tree, &db, 0, "nodes", "N");
                }
            }


            tree->restore(table);

            tabWidget->addTab(tree, txt);
            tab_num += 1;
        }
    }
    if (tab_num == tab_num0) {
        PropertyTree *tree = new PropertyTree(tab_num);
        tree->setColumnCount(n_cols);
        fillValues("Данные", this->tab_lst, tree, &db, -1, table, "");
        tree->restore(table);
        tabWidget->addTab(tree, "Данные");
        tab_num += 1;
    }

    return true;
}

bool PropertyDial::_init_out(QTabWidget *tabWidget, QSqlDatabase &db, const QString & title, const QString & table_out, PrTypes pr_type, int &tab_num)
{
    PropertyTree *tree = new PropertyTree(tab_num);

    if (table_out.toLower() == "pt_out" || table_out.toLower() == "dr_out") {
        tree->setColumnCount(2);
    }
    else {
        tree->setColumnCount(3);
    }

    tabWidget->addTab(tree, title);
//    tabWidget->addTab(tree, tr("Результат расчета"));

//    if (table == ""
/*

    if (pr_type == pr_type_node_out || pr_type == pr_type_node0 || pr_type == pr_type_node) {
        std::list<QString> tab_lst1;
        tab_lst1.push_back("Узел");
        tab_lst1.push_back(" externalCodeID");
        tab_lst1.push_back(" externalNodeName");
        fillValues(tab_lst1, tree, &db, 0, "nodes", "N", true);
    }

    if (pr_type == pr_type_line_out || pr_type == pr_type_line) {
        std::list<QString> tab_lst1;
        tab_lst1.push_back("Начальный узел");
        tab_lst1.push_back(" externalCodeID");
        tab_lst1.push_back(" externalNodeName");
        fillValues(tab_lst1, tree, &db, 0, "nodes", "N1", true);
        std::list<QString> tab_lst2;
        tab_lst2.push_back("Конечный узел");
        tab_lst2.push_back(" externalCodeID");
        tab_lst2.push_back(" externalNodeName");
        fillValues(tab_lst2, tree, &db, 0, "nodes", "N2", true);
    }
*/

    fillValues("Результат расчета", this->tab_lst, tree, &db, -1, table_out, "", true, true);
    tree->restore(table_out);

    return true;
}

QTabWidget * PropertyDial::init_widget()
{

    QPushButton *okButton = new QPushButton(tr("Сохранить"));

    okButton->setProperty("dialog", "property");
    okButton->setProperty("class", "ok");

    okButton->setDefault(true);
    okButton->setEnabled(true);

    QPushButton *closeButton = new QPushButton(tr("Отменить"));
    closeButton->setProperty("dialog", "property");
    closeButton->setProperty("class", "cancel");

    QPushButton *helpButton = new QPushButton(tr("Помощь"));
    helpButton->setProperty("dialog", "property");

//    QPushButton *strButton = new QPushButton(tr("Структура"));
//    strButton->setProperty("dialog", "property");

    QPushButton *strButton = new QPushButton(tr("Структура"));
    strButton->setProperty("dialog", "property");

    QMenu *menu = new QMenu(strButton);

    QAction *action_str = menu->addAction("Копировать структуру таблицы в буфер");
    QAction *action_str2 = menu->addAction("Копировать данные в буфер");

    strButton->setMenu(menu);


    QPushButton *fontButton = new QPushButton(tr("Шрифт"));
    fontButton->setProperty("dialog", "property");

    QHBoxLayout *buttonBox = new QHBoxLayout;

    buttonBox->setSpacing(10);

    buttonBox->addWidget(okButton);
    buttonBox->addWidget(closeButton);
    buttonBox->addStretch();
    buttonBox->addWidget(strButton);
    buttonBox->addWidget(fontButton);
    buttonBox->addWidget(helpButton);

    QVBoxLayout *propertyBox = new QVBoxLayout;

    QVBoxLayout *mainLayout = new QVBoxLayout;
//    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    QTabWidget *tabWidget = new QTabWidget;

//    mainLayout->addWidget(tabWidget);

    propertyBox->addWidget(tabWidget);

/*
    this->helpWindow = new QPlainTextEdit;
    this->helpWindow->setFocusPolicy(Qt::NoFocus);
    this->helpWindow->setReadOnly(true);
    propertyBox->addWidget(this->helpWindow);
*/

//    mainLayout->addWidget(tabWidget);
    mainLayout->addLayout(propertyBox);

//    mainLayout->addWidget(&tree);

    mainLayout->addLayout(buttonBox);
    setLayout(mainLayout);
//    setWindowTitle(title);

    connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(fontButton, SIGNAL(clicked()), this, SLOT(fontClicked()));
//    connect(strButton, SIGNAL(clicked()), this, SLOT(strClicked()));
    connect(action_str, SIGNAL(triggered()), this, SLOT(strClicked()));
    connect(action_str2, SIGNAL(triggered()), this, SLOT(copy_buffer()));



    connect(this, &PropertyDial::triggerCustomMenu, this, &PropertyDial::showAboutMessage);


    QSettings settings;

    QString fontFamily = settings.value("property/fontFamily", "Arial").toString();
    int fontSize = settings.value("property/fontSize", 12).toInt();


//    QString style = readFileUTF8("qss2/property.qss");
//    this->setStyleSheet(style);

//    QTimer::singleShot(150, [=]() {
//  });

    QString style = readFileUTF8("qss2/property.qss");
    QString st = QString("\nCustomDateEdit, NullableDateEdit, QLineEdit, QComboBox {font-size: %1px; font-family: %2; height: %3px })").arg(fontSize).arg(fontFamily).arg(fontSize*2);

    style += st;

    this->setStyleSheet(style);

    return tabWidget;
}

bool PropertyDial::init_gid(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, PrTypes pr_type, CCxema *cxema, bool pts)
{
    setWindowTitle(title);

    this->pts = pts;

    m_pr_type = pr_type;

    QTabWidget *tabWidget = init_widget();

    int tab_num = 1;

    m_cxema = cxema;

    if (1) {
        init_fields(db, table, this->m_table_file, id1, id2, pr_type);
        _init_table(tabWidget, db, table, pr_type, 1, tab_num);

        if (table.toLower() == QString("realConsumers").toLower()) {
            QString schemeNum = getValue("schemeNum").toString();
            setPR(schemeNum);
        }
        setVisAll();
        setDisableAll();
    }

    if (!pts) {
        QString table_out = getOutTable(table);
        if (table_out != "") {
            init_fields(db, table_out, table_out, id1, id2, pr_type == pr_type_line ? pr_type_line_out : pr_type_node_out);
            _init_out(tabWidget, db, tr("Результат расчета"), table_out, pr_type, tab_num);
        }

        if (table.toLower() == "realconsumers") {
            init_fields(db, "DR_OUT", "DR_OUT", id1, id2, pr_type == pr_type_line ? pr_type_line_out : pr_type_node_out);
            _init_out(tabWidget, db, tr("Дроссельные органы"),  "DR_OUT", pr_type, tab_num);
        }
    }


    return true;
}

bool PropertyDial::init_gid_with_line(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, int id_l, PrTypes pr_type, CCxema *cxema, bool pts)
{
    setWindowTitle(title);

    this->pts = pts;

    m_pr_type = pr_type;

    QTabWidget *tabWidget = init_widget();

    int tab_num = 1;

    m_cxema = cxema;

    if (1) {
        init_fields(db, table, this->m_table_file, id1, id2, pr_type);
        _init_table(tabWidget, db, table, pr_type, 1, tab_num);

        setVisAll();
        setDisableAll();
    }

    QString table2 = "heatPipeSections";

    init_fields(db, table2, "heatPipeSections_2", id_l, -1, pr_type_line);
    _init_table(tabWidget, db, table2, pr_type_line, tab_num, tab_num);
//    _init_out(tabWidget, db, tr("Участок"), table_out, pr_type, tab_num);


    return true;
}

bool PropertyDial::init_fields_for_analiz(QSqlDatabase &db, const QString & table, PrTypes pr_type, const QStringList &allowed_fields)
{
    QTreeWidgetItem *item_title = nullptr;
    QTreeWidgetItem *item_line = nullptr;

    int num = 1;

    PropertyTree *tree = new PropertyTree(0);
    tree->setHeaderLabels({tr(""), tr("Минимальное значение"), tr("Максимальное значение")});
    tree->header()->setFixedHeight(24);

    QTabWidget *tabWidget = init_widget();
    tabWidget->addTab(tree, tr("Результат расчета"));

    std::list<AdoField> fields;
    get_table_columns(db, table, fields);

    QString title1 = "";
    QString title2 = "";

    item_title = new QTreeWidgetItem(tree);
    item_title->setText(0, tr("Параметры"));
    item_title->setFlags(item_title->flags() | Qt::ItemIsEditable);

    for (const QString &col : allowed_fields) {
        if (col == s_id || col == s_shape) continue;

        bool is_float = false;
        for (auto & it : fields) {
            if (col == it.COLUMN_NAME) {
                if (it.DATA_TYPE == "float" || it.DATA_TYPE == "double precision") {
                    is_float = true;
                    break;
                }
            }
        }
        if (!is_float) continue;

        QString rus = col;
        const CColName *c = findColumnRusNameFull(getDatabaseName(db), table, col);
        QString f1 = "";
        if (c) {
            rus = c->name;
            f1 = c->name_full;
        }

        item_line = new QTreeWidgetItem(item_title);
        QWidget * ww = makeLabelWidget(this, num, rus, f1);
        tree->setItemWidget(item_line, 0, ww);

        int index1 = -1, index2 = -1;
        int fileID1 = -1, fileID2 = -1;
        int id_col = 0;
        QVariant null_value;
        QMetaType metaType(QMetaType::Double);
        std::map<int, QString> map_lookup;

        addRow(item_line, tree, &db,
               null_value,
               id_col++,
               index1,
               fileID1,
               metaType, table, 0, col, rus, f1, false, false, map_lookup, title1, title2);
        addRow(item_line, tree, &db,
               null_value,
               id_col++,
               index2,
               fileID2,
               metaType, table, 1, col, rus, f1, false, false, map_lookup, title1, title2);

        num++;
    }

    tree->restore(table);

    return true;
}


bool PropertyDial::init_fields_for_analiz(QSqlDatabase &db, const QString & table, PrTypes pr_type)
{
    QTreeWidgetItem *parent = nullptr;
    QTreeWidgetItem *item_title = nullptr;
    QTreeWidgetItem *item_line = nullptr;

    int tab = 0;
    int num = 1;

    PropertyTree *tree = new PropertyTree(0);

    tree->setHeaderLabels({tr(""), tr("Минимальное значение"), tr("Максимальное значение")});
    tree->header()->setFixedHeight(24);

    QTabWidget *tabWidget = init_widget();

    tabWidget->addTab(tree, tr("Результат расчета"));

    TypeChecker typeChecker;

//    QString table_file = table;

    QString title1 = "";
    QString title2 = "";

    read_tab(this->m_table_file, this->tab_lst);

    if (this->tab_lst.size() == 0) {
        init_fields_db(db, table, this->tab_lst, s_id);
    }

    std::list<AdoField> fields;
    get_table_columns(db, table, fields);

    for (auto & line : tab_lst) {

        if (line.left(1) == "-") continue;
        if (line.left(1) == " ") {
            QString col = "", rus = "", f1 = "";

            static QRegularExpression re("^\\s+([^ ]+)\\s+(.+)\\s*$");
            QRegularExpressionMatch match = re.match(line);
            if (match.hasMatch()) {
                col = match.captured(1);
                rus = match.captured(2);
            }
            else {
                rus = col = line.trimmed();
                const CColName *c = findColumnRusNameFull(getDatabaseName(db), table, col);

                if (c) {
                    rus = c->name;
                    f1 = c->name_full;
                }
                else {
                    continue;
                }
            }

            if (col == s_id) continue;
            if (col == s_shape) continue;

            if (col == "") continue;

            bool is_float = false;

            for (auto & it : fields) {
                if (col == it.COLUMN_NAME) {
                    if (it.DATA_TYPE == "float" || it.DATA_TYPE == "double precision") {
                        is_float = true;
                        break;
                    }
                }
            }

            if (!is_float) continue;

            QString prefix = "";

            std::tuple<QSqlField, int> vP, vO;
            std::tuple<QString, QString, QString, int, int> k1(table, col, prefix, 0, 0);
            std::tuple<QString, QString, QString, int, int> k2(table, col, prefix, 1, 0);

            std::tuple<QString, QString, QString, int, int> ki1(table, col, prefix, 0, 1);
            std::tuple<QString, QString, QString, int, int> ki2(table, col, prefix, 1, 1);

            int index1 = -1;
            int index2 = -1;

            int fileID1 = -1, fileID2 = -1;

            item_line = new QTreeWidgetItem(item_title);
//            item_line->setProperty("class", "title");

            QWidget * ww = makeLabelWidget(this, num, rus, f1);

            tree->setItemWidget(item_line,0, ww);

            int id_col = 0;

            QVariant null_value;
            QMetaType metaType(QMetaType::Double);
            std::map<int, QString> map_lookup;

            addRow(item_line, tree, &db,
                null_value,
                id_col++,
                index1,
                fileID1,
                metaType, table, 0, col, rus, f1, false, false, map_lookup, title1, title2);
            addRow(item_line, tree, &db,
                null_value,
                id_col++,
                index2,
                fileID2,
                metaType, table, 1, col, rus, f1, false, false, map_lookup, title1, title2);

            num ++;
        }
        else {
            item_title = new QTreeWidgetItem(tree);
            item_title->setText(0, line);
            title2 = line;
            item_title->setFlags(item_title->flags() | Qt::ItemIsEditable);
        }
    }

    tree->restore(table);

    return true;
}

bool PropertyDial::init_analiz(QSqlDatabase &db, const QString & title, const QString & table_out, PrTypes pr_type)
{
    setWindowTitle(title);

    int tab_num = 1;

    init_fields_for_analiz(db, table_out, pr_type);
//    _init_out(tabWidget, db, table_out, pr_type, tab_num);
    return true;
}

bool PropertyDial::initAdmissibilityAnalysis(QSqlDatabase &db, const QString & title, const QString & table_out, PrTypes pr_type)
{
    setWindowTitle(title);

    int tab_num = 1;

    QStringList myFields = {"pressure", "temperature", "velocity"};
    init_fields_for_analiz(db, table_out, pr_type, myFields);
    init_fields_for_analiz(db, table_out, pr_type);
    //    _init_out(tabWidget, db, table_out, pr_type, tab_num);
    return true;
}

bool PropertyDial::init_out(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, PrTypes pr_type)
{
    setWindowTitle(title);

    QTabWidget *tabWidget = init_widget();

    int tab_num = 1;

    QString table_out = getOutTable(table);

    m_table_file = table_out;

    if (table_out != "") {
        init_fields(db, table_out, table_out, id1, id2, pr_type == pr_type_line ? pr_type_line_out : pr_type_node_out);
        _init_out(tabWidget, db, tr("Результат расчета"), table_out, pr_type, tab_num);
    }
    return true;
}

bool PropertyDial::init_out(QSqlDatabase &db, const QString & title, const QString & table, const QString & table_out, int id1, int id2, PrTypes pr_type)
{
    setWindowTitle(title);

    QTabWidget *tabWidget = init_widget();

    int tab_num = 1;

//    QString table_out = getOutTable(table);

    m_table_file = table_out;

    if (table_out != "") {
        init_fields(db, table_out, table_out, id1, id2, pr_type == pr_type_line ? pr_type_line_out : pr_type_node_out);
        _init_out(tabWidget, db, tr("Результат расчета"), table_out, pr_type, tab_num);
    }
    return true;
}

PropertyDial::PropertyDial(QWidget *parent, const QString & table, const QString & table_file, const QString & _s_shape, const QString & _s_id, int id1, int id2)
    : QDialog(parent)
//    , save(this, "PropertyDial", table)
    , save(this, "PropertyDial",  QString("%1 %2").arg(table_file).arg((id1 == -1 || id2 == -1 || id1 == id2) ? 1 : 2))
{
    s_shape = _s_shape;
    s_id = _s_id;
    m_table = table;
    m_table_file = table_file;
    if (table_file == "") {
        m_table_file = table;
    }

#if 0
    addCustomSystemMenu();
#endif
}

PropertyDial::PropertyDial(QWidget *parent, QSqlDatabase &db, const QString & title, const QString & table, const QString & table_file, int id1, int id2, PrTypes pr_type, CCxema *cxema)
    : QDialog(parent)
    , save(this, "PropertyDial",  QString("%1 %2").arg(table_file).arg((id1 == -1 || id2 == -1 || id1 == id2) ? 1 : 2))
{
    setWindowTitle(title);
    m_cxema = cxema;
    m_table = table;
    m_table_file = table_file;
    if (table_file == "") {
        m_table_file = table;
    }

    init_gid(db, title, table, id1, id2, pr_type, m_cxema, true);
#if 0

    addCustomSystemMenu();
#endif
}

void PropertyDial::Add(const QString & txt, int id, bool on)
{

}


void PropertyDial::helpClicked()
{
    QString fn = QString("kls/help/%1.html").arg( m_table);

    QString text = readFileUTF8(fn);

    if (text != "") {
        QString title = findTableRusName("gid", m_table);

        HelpDialog hlp(title, text, this);
        hlp.exec();

//        QMessageBox::warning(this, title, text);
    }
}

#if 0
bool PropertyDial::prop_validate(PropertyStr &pr, const QString &val)
{
    if (pr.after == "NotNull") {
        if (val == "") {
            QMessageBox::warning(this, "", QString("Поле %1 должно быть заполнено!").arg(pr.rus));
            return false;
        }
    }

    return true;
}

#endif

bool PropertyDial::check()
{
    QStringList l1;

    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();

        if (pr.validate != "") {
            if (pr.validate == "NotNull") {
                if (pr.getValue().toString() == "") {
                    l1.append(pr.label);
                }
            }
        }
    }


    QString msg = "";


    if (l1.size() > 0) {
        if (msg != "") msg += "\n";
        msg +="Должны быть заполнены поля:";
        for (auto & s : l1) {
            if (msg != "") msg += "\n";
            msg += QString(s);
        }
    }

    if (msg != "") {
        QMessageBox::warning(this, "", msg);
        return false;
    }



    return true;
}

void PropertyDial::rightClicked()
{
    QMessageBox::warning(this, "", "!");
}


void PropertyDial::okClicked()
{
    if (check()) {
        done(QDialog::Accepted);
    }
}

void PropertyDial::hideEvent(QHideEvent *event)
{
    save.save();

    QDialog::hideEvent(event);
}

void PropertyDial::buttonClicked()
{
    int id = sender()->property("id").toInt();
    QSqlDatabase *db = sender()->property("db").value<QSqlDatabase *>();
    QString label = sender()->property("label").toString();

    QVariant v = sender()->property("col");
    if (!v.isValid()) {
        return;
    }

    QString s = v.toString().mid(1);

    QStringList lst = s.split('$');

    QString cmd = lst[0];
    QString table_view = "table_view";

    QString param1 = lst.size() > 1 ? lst[1] : "";
    QString param2 = lst.size() > 2 ? lst[2] : "";

    if (cmd.indexOf(table_view) == 0) {
        param1 = s.mid(table_view.length()+1);
        cmd = table_view;
    }

    prop_fun2(cmd, *db, param1, param2, id, label);
}


QString ispr_sql(const QString & q0)
{
    QString q = q0;

    if (!is_MSSQL()) {
        {
            static QRegularExpression re("^SELECT\\s+TOP\\s+([0-9]+)\\s+(.+)$");
            QRegularExpressionMatch match = re.match(q);
            if (match.hasMatch()) {
                int top = match.captured(1).toInt();
                QString s = match.captured(2);
                q = QString("SELECT %1 LIMIT %2").arg(s).arg(top);
            }
        }
        {

            static QRegularExpression re("^SELECT\\s+DISTINCT\\s+TOP\\s+([0-9]+)\\s+(.+)$");
            QRegularExpressionMatch match = re.match(q);
            if (match.hasMatch()) {
                int top = match.captured(1).toInt();
                QString s = match.captured(2);
                q = QString("SELECT DISTINCT %1 LIMIT %2").arg(s).arg(top);
            }
        }


    }

    return q;
}

void PropertyDial::setIdValue(PropertyStr & pr, int index, const QString & text)
{
    if (!pr.widget) return;

    if (pr.lookup2.size() > 0) {
        setId(pr.key, index);
    }
    else if (pr.isLookup && text != "") {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(pr.widget);
//        text = "111";
        lineEdit->setText(text);
    }
    else {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(pr.widget);

        lineEdit->setText(text);
    }

    pr.index = index;
    pr.text = text;

    setBold(pr.widget);

    pr.value = text_to_variant(text, pr.metaType);
    pr.widget->setProperty("col", QVariant::fromValue(pr));

    map_edited[pr.table][pr.id][pr.key] = pr;
}

QString getQLookup(PropertyStr &pr);


void PropertyDial::setId(PropertyStr & pr, int index)
{
    if (!pr.widget) return;

    QString text = "";

    QComboBox *combo = qobject_cast<QComboBox *>(pr.widget);
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(pr.widget);

    if (pr.isLookup) {
        QString q1 = getQLookup(pr);
        QString q = QString("SELECT * FROM (\n%1\n) __ \nWHERE __.id=%2").arg(q1).arg(index);
        text = readTableValue2(*pr.db, q);
    }

    if (lineEdit) {
        lineEdit->setText(text);
    }
    else if (combo) {
        int i = 0;
        for (auto &it : pr.lookup2) {
            if (index == it.first) {
                combo->setCurrentIndex(i);
                break;
            }
            i ++;
        }
    }

    pr.index = index;
    pr.text = text;

    setBold(pr.widget);

    pr.value = text_to_variant(text, pr.metaType);
    pr.widget->setProperty("col", QVariant::fromValue(pr));

    map_edited[pr.table][pr.id][pr.key] = pr;
}

QString getQLookup(PropertyStr &pr)
{
    QString ilike = "LIKE";
    if (is_POSTGRESQL()) {
        ilike = "ILIKE";
    }

    QString q = QString("SELECT %1,%2 AS %3 FROM %4\n--AND_TXT WHERE %2 %5 '%$txt$%'  ")
        .arg(pr.lookup->id, pr.lookup->name, quot_text("Наименование"), pr.lookup->table, ilike);

    if (pr.lookup->q != "") {
        q = pr.lookup->q;

        if (pr.fileID <= 0) {
//            QInputDialog::getMultiLineText(nullptr, "", "", q);
            if (pr.m_cxema) {
                q.replace("$fragments$", QString("%1").arg(pr.m_cxema->m_par));
            }
        }
        else {
            q.replace("$fragments$", QString("%1").arg(pr.fileID));
        }

        q.replace("$id$", QString("%1").arg(pr.id));
        q.replace("$table$", pr.table);

        q = ispr_sql(q);
    }
    else {

        if (pr.fileID > 0 && column_in_table(*pr.db, pr.lookup->table, "fileID")) {
    //        q = QString("SELECT %1,%2 AS %3 FROM %4 WHERE fileID=%5").arg(pr.lookup->id, pr.lookup->name, quot_text("Наименование"), pr.lookup->table).arg(pr.fileID);

            q = QString("SELECT %1,%2 AS %3 FROM %4 WHERE fileID=%5\n--AND_TXT AND %2 %6 '%$txt%%'  ")
                .arg(pr.lookup->id, pr.lookup->name, quot_text("Наименование"), pr.lookup->table)
                .arg(pr.fileID).arg(ilike);
        }
        else {
    //        q = QString("SELECT * FROM (\n%1\n) __\n--AND_TXT WHERE %2 %3 '%$txt$%'").arg(q, pr.lookup->name, ilike);
        }
    }

    return q;
}



bool viewLookup(QWidget *widget, PropertyStr &pr, QString &text, int &index)
{
    if (pr.lookup->name == "nodes1") {
    }
    else {
        QString q = getQLookup(pr);

        if (pr.lookup->field.toLower() == "connectid") {
            q = QString("SELECT DISTINCT n2.id, CONCAT(ec.name, ' ', n2.externalNodeName) AS externalNodeName FROM net.v_linesobj l JOIN net.v_nodes n2 ON(n2.id = l.nodeID1 OR n2.id = l.nodeID2) LEFT JOIN externalCodes ec ON ec.id = n2.externalCodeID JOIN net.v_nodes n1 ON(n1.id = l.nodeID1 OR n1.id = l.nodeID2) AND n1.id <> n2.id JOIN net.v_nodes nc ON nc.internalNodeID = n1.id JOIN net.v_connectnodes cn ON cn.nodeID = nc.id WHERE cn.id = %1").arg(pr.id);
        }

    //    QInputDialog::getMultiLineText(nullptr, "", "", q);

        TableWindow tab(*pr.db, pr.label, pr.label, q, widget, pr.index, true, true, false);
        if (tab.exec()) {
            index = tab.value(0).toInt();
            text = tab.value(1).toString();
            return true;
        }
    }
    return false;
}


void PropertyDial::clicked()
{
    QVariant v = sender()->property("col");

    if (!v.isValid()) {
        return;
    }

    PropertyStr pr = v.value<PropertyStr>();
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(pr.widget);
    pr = lineEdit->property("col").value<PropertyStr>();

    if (pr.isFunc) {
        prop_fun(pr);
    }
    else if (pr.isLookup) {
        QString text;
        int index;
        if (viewLookup(this, pr, text, index)) {
            setIdValue(pr, index, text);
        }
    }
    else {
        if (lineEdit) {
            lineEdit->setText("!!!!");
        }
    }
}

bool PropertyDial::eventFilter(QObject *object, QEvent *event)
{

    if (object && event->type() == QEvent::Wheel)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(object);

        if (combo && !combo->hasFocus())
            return true;
    }

    if (object && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = (QKeyEvent *)event;
        QComboBox* combo = qobject_cast<QComboBox*>(object);

        if (combo && combo->hasFocus()) {

            QKeyCombination kc = ke->keyCombination();

            if (kc.key() == Qt::Key_Delete) {
                combo->setCurrentIndex(-1);
                return true;
            }
         }
    }

    if (object && event->type() == QEvent::ContextMenu)
    {
        QVariant v = object->property("col");

        if (v.isValid()) {
            PropertyStr pr = v.value<PropertyStr>();
            if (pr.isLookup || pr.isLookup2) {
                QContextMenuEvent *e = (QContextMenuEvent *) event;

//                QMessageBox::warning(this, "", "!");

                QMenu *menu = new QMenu(this);
                QAction *a = new QAction(tr("Убрать"), this);
                menu->addAction(a);
                menu->popup(e->pos());
            }
        }
    }



    if (object && event->type() == QEvent::FocusIn)
    {
        QVariant v = object->property("col");

        if (v.isValid()) {
            PropertyStr pr = v.value<PropertyStr>();
        }
    }
    return QObject::eventFilter(object  , event);
}


void PropertyDial::textChanged(const QString & txt)
{
    PropertyStr pr = sender()->property("col").value<PropertyStr>();

    if (false) {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(pr.widget);
        if (lineEdit) {
            lineEdit->setText(pr.value.toString());
        }
        return;
    }


//    if (pr.validate != "") {
//    }

    setBold(pr.widget);

    pr.value = text_to_variant(txt, pr.metaType);
    pr.text = txt;
//    pr.widget->setProperty("col", QVariant::fromValue(pr));
//
    pr.widget->setProperty("col", QVariant::fromValue(pr));

    map_edited[pr.table][pr.id][pr.key] = pr;

    if (pr.after != "") {
        prop_after(pr);
    }
}

void PropertyDial::currentIndexChanged(int index)
{
}

void PropertyDial::currentTextChanged(const QString & txt)
{
    PropertyStr pr = sender()->property("col").value<PropertyStr>();

    for (auto &it : pr.lookup2) {
        if (it.second == txt) {
            pr.index = it.first;
            pr.value = text_to_variant(txt, pr.metaType);
            pr.text = txt;

            setDisable(pr.key, QString("%1").arg(pr.index));
            setVis(pr.key, QString("%1").arg(pr.index));

            pr.widget->setProperty("col", QVariant::fromValue(pr));

            map_edited[pr.table][pr.id][pr.key] = pr;
            break;
        }
    }

    if (txt == "") {
        pr.index = -1;
        QVariant var;

        if (var.isNull()) {
            pr.value = var;
        }

        pr.text = txt;

        pr.widget->setProperty("col", QVariant::fromValue(pr));

        map_edited[pr.table][pr.id][pr.key] = pr;
    }

    setBold(pr.widget);
}

void PropertyDial::dateChanged(QDate date)
{
    PropertyStr pr = sender()->property("col").value<PropertyStr>();

    setBold(pr.widget);

    pr.value = date;
    pr.text = VariantDelegate::displayText(date);

    pr.widget->setProperty("col", QVariant::fromValue(pr));

    map_edited[pr.table][pr.id][pr.key] = pr;
}

#if 0
void PropertyDial::enter()
{
    PropertyStr pr = sender()->property("col").value<PropertyStr>();
    this->helpWindow->setPlainText(pr.key);
    QMessageBox::warning(this, tr("!"), pr.key);
}

#endif

void PropertyDial::setReadOnly(const QString & col, bool on)
{
    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
        if (pr.key.toLower() == col.toLower()) {
            widget->setEnabled(!on);
        }
    }
}

void PropertyDial::setVisual(const QString & col, bool on)
{
    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
        if (pr.key.toLower() == col.toLower()) {
//            widget->setEnabled(!on);
            pr.item_line->setHidden(on);
//            widget->parentWidget()->setEnabled(!on);
//            widget->setEnabled(!on);

//            QWidget *w = widget->parentWidget()->parentWidget();
//            w->setEnabled(!on);

//            widget->parentWidget()->setVisible(!on);
        }
    }
}


void PropertyDial::setValue1(const QString & col, QVariant value)
{
    setValue(col, -1, value);
}

void PropertyDial::setValue(const QString & col, int row, QVariant value)
{
    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
        if (pr.key == col && (pr.id_col == row || row == -1)) {
            QLineEdit *lineEdit = qobject_cast<QLineEdit *>(pr.widget);
            if (lineEdit) {
                lineEdit->setText(VariantDelegate::displayText(value));
            }
            pr.value = value;
            pr.widget->setProperty("col", QVariant::fromValue(pr));
        }
    }
}

QString PropertyDial::getValueString(const QString & col)
{
    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
        if (pr.key == col) {
            return VariantDelegate::displayText(pr.value);
        }
    }
    return "";
}


QVariant PropertyDial::getValue(const QString & col)
{
    QVariant v;
    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
        if (pr.key == col) {
            return pr.getValue();
//            return pr.value;
        }
    }
    return v;
}

int PropertyDial::getIndex(const QString & col)
{
    QVariant v;
    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
        if (pr.key == col) {
            return pr.index;
        }
    }
    return -1;
}


PropertyStr PropertyDial::getPr1(const QString & col)
{
    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
        if (pr.key.toLower() == col.toLower()) {
            return pr;
        }
    }

    PropertyStr pr;
    return pr;
}


PropertyStr PropertyDial::getPr(const QString & col, int row)
{
    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
//        if (pr.key.toLower() == col.toLower()) {

        if (pr.key == col && (pr.id_col == row || row == -1)) {
            return pr;
        }
    }

    PropertyStr pr;
    return pr;
}




void PropertyDial::setId(const QString & col, int id)
{
    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
        if (pr.key.toLower() == col.toLower()) {
            if (pr.lookup2.size() > 0) {
                QComboBox *comboBox = new QComboBox;
                int index = 0;
                for (auto &it : pr.lookup2) {
                    if (id == it.first) {
                        comboBox->setCurrentIndex(index);
                        break;
                    }
                    index ++;
                }
            }
        }
    }
}


int PropertyDial::getId(const QString & col)
{
    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
        if (pr.key == col) {
            return 1;
        }
    }
    return 1;
}

bool PropertyDial::getNewValue(const QString & tn, const QString & fn, QVariant &val)
{
    for (auto &it1: map_edited) {
        QString table = it1.first;
        if (QString::compare(tn, table, Qt::CaseInsensitive) != 0) return false;
        for (auto &it2: it1.second) {
            for (auto &it3: it2.second) {
                QString field = it3.first;
                if (QString::compare(field, fn, Qt::CaseInsensitive) != 0) return false;
                PropertyStr pr = it3.second;
                val = pr.value;
                return true;
            }
        }
    }
    return false;
}


// Обновление базы после PropertyDial для синхронизации


#if 0
bool update_db_2(QSqlDatabase & db, const QString table0, int id, std::map<QString, std::map<int, std::map<QString, PropertyStr> > > &map_edited)
{
    bool ret = false;
    QString s_set = "";

    for (auto &it1: map_edited) {
        QString table = it1.first;
        for (auto &it2: it1.second) {
            for (auto &it3: it2.second) {
                QString col = it3.first;
                if (s_set != "") s_set += ",";
                s_set += QString("%1_%2=?").arg(table, col);
            }
        }
    }

    QSqlQuery query(db);

    QString table = "gid." + table0;

    QString node_line = "linesobj";

    QString q = QString("UPDATE %1 SET %2 WHERE %3_id=%4").arg(br_text(table), s_set).arg(node_line).arg(id);
    query.prepare(q);

    for (auto &it1: map_edited) {
        QString table = it1.first;
        for (auto &it2: it1.second) {
            int id = it2.first;
            for (auto &it3: it2.second) {
                QString col = it3.first;
                PropertyStr pr = it3.second;

                bool isNull = pr.getValue().isNull();

                if (pr.metaType != QMetaType(QMetaType::QString) && pr.getValue() == "") {
                    isNull = true;
                }

                if (isNull) {
                    query.addBindValue(QVariant());
                }
                else {
                    query.addBindValue(pr.getValue());
                }
            }
        }
    }

    if (query_exec(db, query)) {
        ret = true;
    }
    else {
        ErrorMessage(query);
    }
    return ret;
}

#endif


// Обновление базы после PropertyDial

QString change_group_start(QSqlDatabase &db, const QString &comment);
bool change_group_end(QSqlDatabase & db);


bool update_db(std::map<QString, std::map<int, std::map<QString, PropertyStr> > > &map_edited, int m_user)
{
    bool ret = false;

    bool uchastok_ms = false;
    bool uchastok_rs = false;


    QSqlDatabase *db = nullptr;


    bool edit_start = false;


    for (auto &it1: map_edited) {
        QString table = it1.first;
        for (auto &it2: it1.second) {

            auto db_old = db;

            db = nullptr;

            for (auto &it3: it2.second) {
                PropertyStr pr = it3.second;
                db = pr.db;
            }
            if (!db) continue;

            if (!edit_start) {
                change_group_start(*db, "Редактирование");
                edit_start = true;
            }

            QSqlQuery query(*db);
            int id = it2.first;
            QString s_set = "";
            for (auto &it3: it2.second) {
                QString col = it3.first;
                PropertyStr pr = it3.second;
                if (s_set != "") s_set += ",";
//                s_set += QString("%1=:%1").arg(col);
                s_set += QString("%1=?").arg(col);
            }

//    map_v["operatorID"] = m_user;
//    map_v["archiveChangeDate"] = "$$$_NOW_$$$";

            if (table == "nodes" || table == "linesobj") {
                if (s_set != "") s_set += ",";
                s_set += QString("%1=%2,%3=%4,sync_tgid=true")
                    .arg("operatorID")
                    .arg(m_user)
                    .arg("archiveChangeDate")
                    .arg(get_now());
            }


            QString q = QString("UPDATE %1 SET %2 WHERE id=%3").arg(br_text(table), s_set).arg(id);
            query.prepare(q);
            for (auto &it3: it2.second) {
                QString col = it3.first;
                PropertyStr pr = it3.second;

                bool isNull = pr.getValue().isNull();

                if (pr.metaType != QMetaType(QMetaType::QString) && pr.getValue() == "") {
                    isNull = true;
                }

                if (isNull) {
//                    query.b1indValue(QString(":%1").arg(col), QVariant());
                    query.addBindValue(QVariant());
                }
                else {
//                    query.b1indValue(QString(":%1").arg(col), pr.getValue());
                    query.addBindValue(pr.getValue());
                }
            }




            if (query_exec(*db, query)) {
                ret = true;

                if (table.compare("uchastok_ms", Qt::CaseInsensitive) == 0) {
                    main_window->m_ms->init(nullptr, *db, true);
                }
                if (table.compare("uchastok_rs", Qt::CaseInsensitive) == 0) {
                    main_window->m_rs->init(nullptr, *db, false);
                }
            }
            else {
                ErrorMessage(query);
            }
        }
    }

    if (edit_start) {
        change_group_end(*db);
    }


    return ret;
}

// Анализ

QString analiz_db(QSqlDatabase &db, std::map<QString, std::map<int, std::map<QString, PropertyStr> > > &map_edited)
{
    bool ret = false;

    for (auto &it1: map_edited) {
        QString table = it1.first;

        QString s_set = "";

//        QSqlDatabase *db = nullptr;

        for (auto &it2: it1.second) {

            for (auto &it3: it2.second) {
                PropertyStr pr = it3.second;
//                db = pr.db;
            }

            int id = it2.first;
            for (auto &it3: it2.second) {
                QString col = it3.first;
                PropertyStr pr = it3.second;

                if (pr.id == 0) {
                    if (!pr.value.isNull()) {
                        if (s_set != "") s_set += " AND ";
                        s_set += QString("%1 >= %2").arg(col).arg(pr.value.toDouble());
                    }
                }
                else {
                    if (!pr.value.isNull()) {
                        if (s_set != "") s_set += " AND ";
                        s_set += QString("%1 <= %2").arg(col).arg(pr.value.toDouble());
                    }
                }
            }
        }
        QString q = QString("SELECT * FROM %1 WHERE %2").arg(table, s_set);
        return s_set;
//        return q;
    }
    return "";
}

void PropertyDial::onHelp()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QLineEdit * w = action->data().value<QLineEdit*>();

        if (w) {
            QString f1 = txt_to_html(w->property("f1").toString(), w->text());
            if (f1 != "") {
                HelpDialog hlp(w->text(), f1, this);
                hlp.exec();
            }
        }
    }
}

void PropertyDial::onIndexNull()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
//        QWidget *widget = action->data().value<QWidget *>();
        QComboBox* combo = action->data().value<QComboBox*>();

        if (combo) {
            QVariant v = combo->property("col");
            PropertyStr pr = v.value<PropertyStr>();

            pr.index = -1;
            QVariant var;

            if (var.isNull()) {
                pr.value = var;
            }

            pr.text = "";

            pr.widget->setProperty("col", QVariant::fromValue(pr));

            map_edited[pr.table][pr.id][pr.key] = pr;

            combo->setCurrentIndex(-1);

            setBold(pr.widget);
        }


        QLineEdit* lineEdit = action->data().value<QLineEdit*>();

        if (lineEdit) {
            QVariant v = lineEdit->property("col");
            PropertyStr pr = v.value<PropertyStr>();

            pr.index = -1;
            QVariant var;

            if (var.isNull()) {
                pr.value = var;
            }

            pr.text = "";

            pr.widget->setProperty("col", QVariant::fromValue(pr));

            map_edited[pr.table][pr.id][pr.key] = pr;

            lineEdit->setText("");
//            combo->setCurrentIndex(-1);

            setBold(pr.widget);
        }
    }
}


void PropertyDial::contextMenu(QPoint point)
{
    QMenu *menu = new QMenu(this);


    QWidget *widget = qobject_cast<QWidget *>(sender());
    if (widget) {

        QAction *action = new QAction("Удалить значение", this);
        action->setData(QVariant::fromValue(widget));

        connect(action, SIGNAL(triggered()), this, SLOT(onIndexNull()));

        menu->addAction(action);
        menu->popup(widget->mapToGlobal(point));
    }
}


void PropertyDial::helpMenu(QPoint point)
{
    QMenu *menu = new QMenu(this);


    QWidget *widget = qobject_cast<QWidget *>(sender());
    if (widget) {

        QAction *action = new QAction("Помощь", this);
        action->setData(QVariant::fromValue(widget));

        connect(action, SIGNAL(triggered()), this, SLOT(onHelp()));

        menu->addAction(action);
        menu->popup(widget->mapToGlobal(point));
    }
}




bool PropertyDial::check_column(PropertyStr pr, const QString & v, QString &msg, QString &msg_notnull, QString &msg_sezon, int &moveID)
{

    if (pr.validate == "") return true;

#if 0
    QString strV = pr.validate;

    bool is_null = false;

    if (v == "") is_null = true;

    if (strV.Find("Exists") >= 0 && !is_null) {
         vector<QString> match;
         int l = regex_match(".*Exists\\s+([^ ]+)", strV, match);

         if (l > 1) {
             QString fn = match[1];
             CItem* item2 = FindItemName(fn);
             if (item2) {
                  QString ss;
                  bool is = GetNameValue(fn, ss);
                  if (!is || ss == "") {
                      msg_notnull = item2->m_label;
//                      msg.Format("Заполните поле \"\"%s\"\"", item2->m_label);
//                      msg.Format("Поле \"%s\" должно быть заполнено перед заполнением поля \"%s\"",  item2->m_label, item->m_label);
//                      AfxMessageBox(msg);
//                      if (mov) FocusItem(item2->m_id);
                      moveID = item2->m_id;
                      return false;
                  }
             }
         }
    }


#endif
#if 0
    msg = "";
    msg_notnull = "";
    msg_sezon = "";

//    if (item->m_is_validate && !item->m_is_hidden) {
        QString s;

        QString strV = item->m_strValidate;

        bool is_null = false;

        if (v == "") is_null = true;
        if (item->m_is_lookup && item->m_index < 0) is_null = true;

        if (item->m_type == IT_DATE && (item->m_dtValue.GetStatus() != 0 || item->m_dtValue == 0)) is_null = true;


        if (is_null) {
            if (strV.Find("NotNIf") >= 0) {

                QString s = item->m_strValidate;

                vector<QString> match;
                int l = regex_match(".*NotNIf\\s+([^ ]+)\\s+([0-9]+)\\s*$", s, match);

                if (l > 1) {
                    QString fn2 = match[1];
                    int id2 = atoi(match[2]);
                    int id3;

                    GetNameValue(QString(fn2), id3);
                    if (id2 == id3) {
//                        msg.Format("Поле \"%s\" должно быть заполнено", item->m_label);
                        msg_notnull = item->m_label;
//                        AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
//                        if (mov) FocusItem(item->m_id);
                        moveID = item->m_id;
                        return false;
                    }
                }
            }
            else if (strV.Find("NotNull") >= 0) {
                msg_notnull = item->m_label;
//                msg.Format("Поле \"%s\" должно быть заполнено", item->m_label);
//                AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
//                if (mov) FocusItem(item->m_id);
                moveID = item->m_id;
                return false;
            }
        }
        if (strV.Find("After") >= 0 && !is_null) {
             vector<QString> match;
             int l = regex_match(".*After\\s+([^ ]+)", strV, match);

             if (l > 1) {
                 QString fn = match[1];
                 CItem* item2 = FindItemName(fn);
                 if (item2) {
                      COleDateTime dt1, dt2;
//                      bool is_d1 = GetNameValue(item->m_name, dt1);

                      bool is_d1 = StringToDate(v, dt1);

                      bool is_d2 = GetNameValue(item2->m_name, dt2);
                      if (dt2 > dt1) {
                          msg.Format("\"%s\" должно быть после \"%s\"", item->m_label , item2->m_label);
//                          AfxMessageBox(msg);
//                          if (mov) FocusItem(item->m_id);
                          moveID = item->m_id;
                          return false;
                      }
                 }
             }
        }

        if (strV.Find("Exists") >= 0 && !is_null) {
             vector<QString> match;
             int l = regex_match(".*Exists\\s+([^ ]+)", strV, match);

             if (l > 1) {
                 QString fn = match[1];
                 CItem* item2 = FindItemName(fn);
                 if (item2) {
                      QString ss;
                      bool is = GetNameValue(fn, ss);
                      if (!is || ss == "") {
                          msg_notnull = item2->m_label;
//                          msg.Format("Заполните поле \"\"%s\"\"", item2->m_label);
//                          msg.Format("Поле \"%s\" должно быть заполнено перед заполнением поля \"%s\"",  item2->m_label, item->m_label);
//                          AfxMessageBox(msg);
//                          if (mov) FocusItem(item2->m_id);
                          moveID = item2->m_id;
                          return false;
                      }
                 }
             }
        }

        if (strV.Find("NotNIfExists") >= 0 && !is_null) {
             vector<QString> match;
             int l = regex_match(".*NotNIfExists\\s+([^ ]+)", strV, match);

             if (l > 1) {
                 QString fn = match[1];
                 CItem* item2 = FindItemName(fn);
                 if (item2) {
                      QString ss;
                      bool is = GetNameValue(fn, ss);
                      if (!is || ss == "") {
                          msg_notnull = item2->m_label;
                          moveID = item2->m_id;
                          return false;
                      }
                 }
             }
        }


        if (!is_null && (strV.Find("CurrentSeason") >= 0 || strV.Find("CurrentAndNextSeason") >= 0)) {
            COleDateTime dt1;
            bool is_d1 = GetNameValue(item->m_name, dt1);

            int y1, y2;
            COleDateTime date1, date2;
            get_season(y1, y2, date1, date2);

            if (y1 < 0) {
                msg = "Необходимо выбрать отопительный сезон";
                moveID = item->m_id;
                return false;
            }

            if (!check_season(dt1) && strV.Find("CurrentSeason") >= 0) {
                msg_sezon = item->m_label;
                moveID = item->m_id;
                return false;
            }

        }

        bool CheckFaktoryRiskaForRemont(int id);

        if (!is_null && strV.Find("CheckFaktoryRiskaForRemont") >= 0) {
            if (!CheckFaktoryRiskaForRemont(item->m_id_bd)) {
                msg = "Заполните все обязательные данные \"Список участков для ремонта\"";
//                AfxMessageBox("Заполните все обязательные данные \"Список участков для ремонта\"");
                CItem* item2 = FindItemName("Список участков для ремонта");
                if (item2) {
//                    if (mov) FocusItem(item->m_id);
                    moveID = item->m_id;
                }
                return false;
            }
        }

        if (!is_null && strV.Find("Unique") >= 0) {
            if (!check_unique(item->m_table, item->m_name, item->m_id_bd, v)) {
                msg.Format("Поле \"%s\" должно быть уникальным", item->m_label);
//                AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
//                if (mov) FocusItem(item->m_id);
                moveID = item->m_id;
                return false;
            }
        }

        QString sp = "Pattern ";

        if (v != "" && strV.Find(sp) == 0) {
            QString s = item->m_strValidate;

            vector<QString> match;
            int l = regex_match(".*Pattern \"(.+)\" \"(.+)\".*", s, match);

            if (l > 1) {
                QString pat = match[1];
                QString txt = match[2];

                int l = regex_match(pat, v, match);
                if (l < 2) {
                    msg = txt;
    //                AfxMessageBox(txt, MB_OK | MB_ICONINFORMATION);
    //                if (mov) FocusItem(item->m_id);
                    moveID = item->m_id;
                    return false;
                }
            }
        }
//    }
#endif
    return true;
}

#if 0

void PropertyDial::addCustomSystemMenu()
{
      // Получаем хендл окна
     HWND hWnd = reinterpret_cast<HWND>(winId());

      // Получаем системное меню окна
     HMENU hSysMenu = GetSystemMenu(hWnd, FALSE);
     if (hSysMenu) {
          // Добавляем разделитель и новый элемент в системное меню
          AppendMenu(hSysMenu, MF_SEPARATOR, 0, nullptr);  // Разделитель
          AppendMenu(hSysMenu, MF_STRING, 1, L"О программе");  // Новый пункт

          // Обработка нажатия на элемент меню
          connect(this, &PropertyDial::triggerCustomMenu, this, &PropertyDial::showAboutMessage);
     }
}

bool PropertyDial::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    // Перехватываем событие выбора элемента меню
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG*>(message);
        if (msg->message == WM_SYSCOMMAND) {
            if (msg->wParam == 1) {  // ID элемента "О программе"
                emit triggerCustomMenu();  // Генерируем сигнал
                *result = 0;
                return true;
            }
        }
    }
    return QDialog::nativeEvent(eventType, message, result);
}

#endif


void setFontElement(QWidget *widget, QFont &font) {

    if (widget->inherits("QLineEdit") || widget->inherits("QComboBox")) {
        qDebug() << "Widget:" << widget->metaObject()->className();  // Выводим имя класса виджета

        QFont currentFont = widget->font();
        currentFont.setFamily(font.family());
        currentFont.setPointSize(font.pointSize());
        widget->setFont(currentFont);
    }

    // Рекурсивно обходим всех детей
    for (QWidget *child : widget->findChildren<QWidget*>()) {
        setFontElement(child, font);
    }
}

void PropertyDial::strClicked()
{
    QString txt = "";
    QString title1_old = "";
    QString title2_old = "";

    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();

        if (pr.id_col == 0) {

            if (pr.title1 != title1_old) {
                if (txt != "") {
                    txt += "\n";
                }
                txt += pr.title1;
                title1_old = pr.title1;
            }

            if (pr.title2 != title2_old) {
                if (txt != "") {
                    txt += "\n";
                }
                txt += "    " + pr.title2;
                title2_old = pr.title2;
            }

            if (txt != "") {
                txt += "\n";
            }

            txt += QString("        \"%1\",\"%2\"").arg(pr.key, pr.label);
        }
    }


    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(txt);
}


void PropertyDial::copy_buffer()
{
    QString txt = "";
    QString title1_old = "";
    QString title2_old = "";


//    for (auto *widget : list_widget) {
    for (auto it = list_widget.begin(); it != list_widget.end(); ++it) {
        QWidget *widget = *it;

        PropertyStr pr = widget->property("col").value<PropertyStr>();

        if (pr.id_col == 0) {

            QString v1 = VariantDelegate::displayText(pr.value);
            QString v2 = "";

            bool d2 = false;

            auto it2 = std::next(it);

            if (it2 != list_widget.end()) {
                QWidget *widget2 = *it2;
                PropertyStr pr2 = widget2->property("col").value<PropertyStr>();
                if (pr2.id_col == 1) {
                    v2 = VariantDelegate::displayText(pr2.value);
                    d2 = true;
                }
            }

            if (pr.title1 != title1_old) {
                if (txt != "") {
                    txt += "\n";
                }
                txt += pr.title1;
                title1_old = pr.title1;
            }

            if (pr.title2 != title2_old) {
                if (txt != "") {
                    txt += "\n";
                }
                txt += "    " + pr.title2;
                title2_old = pr.title2;
            }

            if (txt != "") {
                txt += "\n";
            }

            if (d2) {
                txt += QString("        \"%1\",\"%2\",\"%3\",\"%4\"").arg(pr.key, pr.label).arg(v1, v2);
            }
            else {
                txt += QString("        \"%1\",\"%2\",\"%3\"").arg(pr.key, pr.label).arg(v1);
            }
        }
    }


    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(txt);
}



void PropertyDial::fontClicked()
{
    bool ok;
    QSettings settings;

    QString fontFamily = settings.value("property/fontFamily", "Arial").toString();
    int fontSize = settings.value("property/fontSize", 12).toInt();
    QFont font0(fontFamily, fontSize);

    QFont font = QFontDialog::getFont(&ok, font0, this);

    if (ok) {

//        QString style = readFileUTF8("qss2/property.qss");
//        QString st = QString("\nCustomDateEdit, QLineEdit, QComboBox {font-size: %1px; font-family: %2})").arg(font.pointSize()).arg(font.family());
//        QString st = QString("\nCustomDateEdit, NullableDateEdit, QLineEdit, QComboBox {font-size: %1px; font-family: %2; height: %3px })").arg(fontSize).arg(fontFamily).arg(fontSize*2);

        QString style = readFileUTF8("qss2/property.qss");
        QString st = QString("\nCustomDateEdit, NullableDateEdit, QLineEdit, QComboBox {font-size: %1px; font-family: %2; height: %3px })").arg(fontSize).arg(fontFamily).arg(fontSize*2);

        style += st;

        settings.setValue("property/fontFamily", font.family());
        settings.setValue("property/fontSize", font.pointSize());

        QTimer::singleShot(250, [=]() {
            this->setStyleSheet(style);
        });

/*
        qDebug() << "---------------------------------------------------------------------------------";
        this->setUpdatesEnabled(false);
        ::setFontElement(this, font);
        this->setUpdatesEnabled(true);
        update();

        settings.setValue("property/fontFamily", font.family());
        settings.setValue("property/fontSize", font.pointSize());
*/
    }
}


void PropertyDial::addParam(const QString &key, const QVariant &val)
{
    this->m_params[key] = val;
}

QVariant PropertyDial::getParam(const QString &key)
{
    auto it = this->m_params.find(key);
    if (it != this->m_params.end()) {
        return it->second;
    }
    return QVariant();
}


