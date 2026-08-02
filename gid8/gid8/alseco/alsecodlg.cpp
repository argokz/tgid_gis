#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QSqlQueryModel>

#include <QtGui>
#include <QtWidgets>
#include <db/db.h>

#include "alsecodlg.h"


QString str_tr(const QString &s0)
{
    QString s = s0.toLower();
    s.replace(" ", "");
    s.replace("бульвар.", "");
    s.replace("бульв.", "");
    s.replace("бульвар", "");
    s.replace("ул.", "");
    s.replace("пер.", "");
    s.replace("пл.", "");
    s.replace("площадь", "");
    s.replace(".", "");
    s.replace("жирау", "жырау");

    return s;
}


bool almost_equal(const QString &s1, const QString &s2)
{
    QString ss1 = str_tr(s1);
    QString ss2 = str_tr(s2);

    return ss1 == ss2;
}


// Функция заполняет QListWidget из SQL-запроса.
// lst       - указатель на QListWidget
// queryText  - SQL-запрос
// column     - индекс колонки из результата (по умолчанию 0)

#if 0
void fillListWidgetFromSql(QSqlDatabase &db, QListWidget *lst, const QString &queryText, int column = 0)
{
    if (!lst) return;

    lst->clear();

    QSqlQuery query(db);
    if (!query.exec(queryText)) {
        qWarning() << "SQL error:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString value = query.value(column).toString();
        lst->addItem(value);
    }
}

#endif

bool readDbVals(QSqlDatabase &db,
                        const QString &queryText,
                        std::vector<QVariant> & vv,
                        const QVariantList &params = {}
                    )
{

    QSqlQuery query(db);
    if (!query.prepare(queryText)) {
        qWarning() << "Prepare error:" << query.lastError().text();
        return false;
    }

    qDebug() << queryText;

    // Привязываем параметры
    for (const QVariant &param : params) {
        qDebug() << param;
        query.addBindValue(param);
    }

    if (!query.exec()) {
        qWarning() << "Exec error:" << query.lastError().text();
        return false;
    }

    while (query.next()) {
        int nn = query.record().count();
        for (int i = 0; i < nn; i++) {
            QVariant v = query.value(i);
            qDebug() << v;
            vv.push_back(v);
        }
        return true;
    }
    return false;
}




void fillListWidgetFromSql(QSqlDatabase &db, 
                           QListWidget *lst,
                           const QString &queryText,
                           const QVariantList &params = {},
                           int column = 0)
{
    if (!lst) return;

    lst->clear();

    QSqlQuery query(db);
    if (!query.prepare(queryText)) {
        qWarning() << "Prepare error:" << query.lastError().text();
        return;
    }

    qDebug() << queryText;

    // Привязываем параметры
    for (const QVariant &param : params) {
        qDebug() << param;
        query.addBindValue(param);
    }

    if (!query.exec()) {
        qWarning() << "Exec error:" << query.lastError().text();
        return;
    }

    QString txt_old = "dfhfghjjfgjg";
    while (query.next()) {

        QString txt = query.value(column).toString();
        if (txt != txt_old) {
            QListWidgetItem *item = new QListWidgetItem(txt);
            if (column > 0) {
                int id = query.value(0).toInt();
                item->setData(Qt::UserRole, id);
            }

            txt_old  = txt;

            lst->addItem(item);
        }

    }
}


QString selectItemInList(QListWidget *lst, const QString &text0)
{
    if (!lst) return "";

    for (int i = 0; i < lst->count(); ++i) {
        QListWidgetItem *item = lst->item(i);
        QString text = item->text();
        if (almost_equal(text, text0)) {
            lst->setCurrentItem(item);   // выделить элемент
            lst->scrollToItem(item);     // прокрутить к нему
            return text;
        }
    }
    return "";

}



void BigDialog::init_q(bool all)
{
//    m_all = all;
//    m_no_all = !all;

/*
    if (m_str_rn == "") {
        add_q = "";
        add_q2 = "1=0";
        return;
    }
*/

    if (all) {
        add_q = "";
        add_q2 = "1=1";
 //       add_q2.Format("n.RAYON IN (%s)", m_str_rn);
    }
    else {
        add_q = QString("LEFT JOIN %1 z\n"
"ON\n"
"    (n.mkr=z.mkr2 OR n.mkr IS NULL AND z.mkr2 IS NULL)\n"
"    AND\n"
"    (n.street=z.street2 OR n.street IS NULL AND z.street2 IS NULL)\n"
"    AND\n"
"        LOWER(REPLACE(z.house2, ' ', '')) = LOWER(REPLACE(n.house, ' ', ''))\n").arg("zdaniya_2");

        add_q2 = "z.id IS NULL";
//        add_q2.Format("z.id IS NULL AND n.RAYON IN (%s)", m_str_rn);
    }
}



void BigDialog::init(QListWidget* lst, QSqlDatabase &db, int i)
{
    QString q;
    
    QList<QVariant> params;
    QString value = "";

    int column = 0;


    if (i == 0) {
        value = mk;
//        q = "select distinct id_adr_mas from zdaniya_2 z order by id_adr_mas";
        q = QString("select distinct COALESCE(n.mkr,'') as mkr from nagruzki n %1 where %2 order by COALESCE(mkr,'')").arg(add_q, add_q2);
    }
   else if (i == 1) {
        value = st;
        q = QString(R"(
            select distinct COALESCE(n.street,'') as street 
            from nagruzki n 
            %1
            where COALESCE(n.mkr, '') = ?
            and (%2)
            order by COALESCE(n.street,'')
            )").arg(add_q, add_q2);
        
        params.append(mk);

//, add_q, add_q2);    
    }
    else if (i == 2) {

        column = 0;
        
        value = dm;

        q = QString(R"(
select house 
from nagruzki n
%1
where 
    coalesce(n.mkr, '') = ?
and 
    coalesce(n.street, '') = ?
and (%2)

ORDER BY
    CASE
        WHEN n.house ~ '^[0-9]+' 
        THEN regexp_replace(n.house, '\D.*$', '')::int
        ELSE NULL
    END NULLS FIRST,

    CASE
        WHEN split_part(n.house, '/', 2) ~ '^[0-9]+$'
        THEN split_part(n.house, '/', 2)::int
        ELSE NULL
    END NULLS FIRST,

    regexp_replace(n.house, '^\d+/?\d*', '');

        )").arg(add_q, add_q2);

        params.append(mk);
        params.append(st);

    }

    fillListWidgetFromSql(db, lst, q, params, column);
    QString new_val = selectItemInList(lst, value);
    if (i == 0) {
        mk = new_val;
    }
    else if (i == 1) {
        st = new_val;
    }
    else if (i == 2) {
        dm = new_val;
    }

}

void execAndShow(QTableView *table, QSqlDatabase db,
                 const QString &sql, const QVariantMap &params = {})
{
    QSqlQuery query(db);
    query.prepare(sql);

    qDebug() << sql;

    // Привязываем параметры
    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
//        query.bindValue(":" + it.key(), it.value());
        qDebug() << it.key() << " " << it.value();
        query.addBindValue(it.value());
    }

    if (!query.exec()) {
        qDebug() << "SQL error:" << query.lastError().text();
        return;
    }

    // Создаём модель для отображения
    QStandardItemModel *model = new QStandardItemModel(table);

    // Заголовки колонок
    QSqlRecord rec = query.record();
    QStringList headers;
    for (int i = 0; i < rec.count(); ++i)
        headers << rec.fieldName(i);
    model->setHorizontalHeaderLabels(headers);

    // Переносим данные
    int row = 0;
    while (query.next()) {
        QList<QStandardItem *> items;
        for (int i = 0; i < rec.count(); ++i) {
            items << new QStandardItem(query.value(i).toString());
        }
        model->appendRow(items);
        row++;
    }

    // Показываем в таблице
    table->setModel(model);
    table->resizeColumnsToContents();
}


void BigDialog::init2(QSqlDatabase &db, int id)
{
    QString sql = QString(R"(

select 
sum(n.OTOP), sum(n.GVS), sum(n.VENT), sum(n.PAR), 
min(n.ADDR), min(n.ADM_RAYON), min(n.RAYON), min(n.IST), min(n.TG), min(n.UCHASTOK)
from nagruzki n 
where
    COALESCE(n.mkr, '') = ?
and 
    COALESCE(n.street , '') = ?
and 
    COALESCE(n.house , '')  = ?

)");


    QString s = QString("<p>%1 %2 %3</p>").arg(mk, st, dm);
    qDebug() << s;

    std::vector<QVariant> vv;

    QList<QVariant> params;
    params.append(mk);
    params.append(st);
    params.append(dm);

//    bool yes = readTableValues(db, sql, 10, vv);
    bool yes = readDbVals(db, sql, vv, params);

    double otop = vv[0].toDouble();
    double gvs = vv[1].toDouble();
    double vent = vv[2].toDouble();
    double par = vv[3].toDouble();

    double sum = otop + gvs + vent + par;

    _otop = otop;
    _gvs = gvs;
    _vent = vent;
    _par = par;    


    s = QString(R"(
<table border="1" cellspacing="0" cellpadding="2">
  <tr>
    <td colspan="2">Адрес: %6</td>
  </tr>
  <tr>
    <td>
      <table border="0" cellspacing="0" cellpadding="2">
        <tr><td>Административный район:</td><td>%7</td></tr>
        <tr><td>Район эксплуатации (АЛСЕКО):</td><td>%8</td></tr>
        <tr><td>Источник теплоснабжения:</td><td>%9</td></tr>
        <tr><td>Температурный график:</td><td>%10</td></tr>
        <tr><td>Участок эксплуатации:</td><td>%11</td></tr>
      </table>
    </td>
    <td>
      <table border="0" cellspacing="0" cellpadding="2">
        <tr><td>Отопление:</td><td>%1</td></tr>
        <tr><td>ГВС:</td><td>%2</td></tr>
        <tr><td>Вентиляция:</td><td>%3</td></tr>
        <tr><td>Пар:</td><td>%4</td></tr>
        <tr><td>Суммарная:</td><td>%5</td></tr>
      </table>
    </td>
  </tr>
</table>

    )")
    .arg(otop, 0, 'f', 0).arg(gvs, 0, 'f', 0).arg(vent, 0, 'f', 0).arg(par, 0, 'f', 0).arg(sum, 0, 'f', 0)
    
    .arg(vv[4].toString())
    .arg(vv[5].toString())
    .arg(vv[6].toString())
    .arg(vv[7].toString())
    .arg(vv[8].toString())
    .arg(vv[9].toString())
    
    ;


    textBrowser->setHtml(s);



/*
    sql = QString("select n.id, n.name AS \"Наименование\",n.dogovor AS \"Номер договора\","
        "n.otop AS \"Отопление, ккал/ч\",n.gvs AS \"ГВС, ккал/ч\",n.vent AS \"Вентиляция, ккал/ч\",n.par AS \"Пар, ккал/ч\" "
        "from nagruzki n %s "
        "where %s and house='%s' and %s ORDER BY numb").arg(add_q, wh, dom, add_q2);

*/

    sql = QString(R"(

select 
    n.id, n.name AS "Наименование",n.dogovor AS "Номер\nдоговора",
        n.otop AS "Отопление,\nккал/ч",n.gvs AS "ГВС,\nккал/ч",n.vent AS "Вентиляция,\nккал/ч",n.par AS "Пар,\nккал/ч" 
        from nagruzki n

where
    COALESCE(n.mkr, '') = '%1'
and 
    COALESCE(n.street , '') = '%2'
and 
    COALESCE(n.house , '')  = '%3'


ORDER BY n.numb

             )").arg(mk, st, dm);


    qDebug() << sql;


#if 0
     QSqlQuery query(db);
     query.prepare(sql);

     query.addBindValue(mk);
     query.addBindValue(st);
     query.addBindValue(dm);

     if (!query.exec()) {
         qDebug() << "SQL error:" << query.lastError().text();
     }

#endif

    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(sql, db);

    if (model->lastError().isValid()) {
        qDebug() << "SQL error:" << model->lastError().text();
    }

    int colCount = model->columnCount();
    for (int c = 0; c < colCount; ++c) {
        QVariant header = model->headerData(c, Qt::Horizontal, Qt::DisplayRole);
        if (header.isValid()) {
            QString text = header.toString();
            text.replace("\\n", "\n");  // заменяем текстовый \n на реальный перенос
            model->setHeaderData(c, Qt::Horizontal, text);
        }
    }

    table->horizontalHeader()->setMinimumHeight(40);       // высота заголовка



#if 0

    QStandardItemModel *model = new QStandardItemModel(this);

    // Заголовки
    model->setHorizontalHeaderLabels({"id", "name_zd"});

    // Чтение строк из результата
    int row = 0;
    while (query.next()) {
        model->setItem(row, 0, new QStandardItem(query.value(0).toString()));
        model->setItem(row, 1, new QStandardItem(query.value(1).toString()));
        row++;
    }

    table->setModel(model);
#endif
#if 0
    QVariantMap params;
    params["0"] = mk;
    params["1"] = st;
    params["2"] = dm;


    execAndShow(table, db, sql, params);
#endif

    table->setModel(model);

}


BigDialog::BigDialog(QSqlDatabase &db, int id, QWidget *parent) 
    : QDialog(parent) {

    setWindowTitle("Большой диалог");
    resize(1100, 700);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ======= Радиокнопки =======
    QHBoxLayout *radioLayout = new QHBoxLayout();
    QRadioButton *radio1 = new QRadioButton("Все");
    QRadioButton *radio2 = new QRadioButton("Ненайденные");
    radio1->setChecked(true);
    radioLayout->addWidget(radio1);
    radioLayout->addWidget(radio2);
    mainLayout->addLayout(radioLayout);

    // ======= Верхний блок (списки + чекбоксы + текст) =======
    QHBoxLayout *topBlockLayout = new QHBoxLayout();


    QString q = QString("SELECT id_adr_mas, street_nam, number_1 from zdaniya_2 where id=%1").arg(id);
    std::vector<QVariant> vv;

    bool yes = readTableValues(db, q, 4, vv);


//    QString mk = "", st = "", dm = "";

    if (yes) {
        mk =   vv[0].toString().trimmed();
        st =   vv[1].toString().trimmed();
        dm =   vv[2].toString().trimmed();
    }



    // --- три списка с поиском ---
    for (int i = 0; i < 3; ++i) {
        QVBoxLayout *block = new QVBoxLayout();
        QLineEdit *search = new QLineEdit();
        search->setPlaceholderText("Поиск...");
        QListWidget *lst = new QListWidget();

        

//        for (int j = 0; j < 10; ++j) {
//            lst->addItem(QString("Элемент %1.%2").arg(i+1).arg(j+1));
//        }


        block->addWidget(search);
        block->addWidget(lst);
        topBlockLayout->addLayout(block, 1);

        searchEdits.append(search);
        lists.append(lst);
    }

    for (int i = 0; i < 3; ++i) {
        init(lists[i], db, i);
    }





    // --- список с чекбоксами ---
    QVBoxLayout *checkBlock = new QVBoxLayout();
    checkBlock->addWidget(new QLabel("Список с чекбоксами:"));
    checkList = new QListWidget();


    q = "SELECT DISTINCT rayon FROM nagruzki ORDER BY rayon";

    QSqlQuery query(db);

    if (!query.exec(q)) {
        qWarning() << "Exec error:" << query.lastError().text();
    }
    else {
        QListWidgetItem *item = new QListWidgetItem("Все районы");
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
        checkList->addItem(item);

        while (query.next()) {


            QString rn = query.value(0).toString();
            QListWidgetItem *item = new QListWidgetItem(rn);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Checked);
            checkList->addItem(item);
        }
    }



    checkBlock->addWidget(checkList);
    topBlockLayout->addLayout(checkBlock, 1);

    // --- текстовое поле с HTML ---
    QVBoxLayout *textBlock = new QVBoxLayout();
    textBlock->addWidget(new QLabel("Описание:"));
    textBrowser = new QTextBrowser();
    textBrowser->setHtml("<p></p>");
    textBlock->addWidget(textBrowser);
    topBlockLayout->addLayout(textBlock, 3);

    mainLayout->addLayout(topBlockLayout);

#if 1
    // ======= Таблица =======
    mainLayout->addWidget(new QLabel("Таблица:"));
    table = new QTableView();
    model = new QStandardItemModel(5, 3, this);
    model->setHorizontalHeaderLabels({"Колонка 1", "Колонка 2", "Колонка 3"});
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 3; ++c) {
            model->setItem(r, c, new QStandardItem(QString("R%1C%2").arg(r+1).arg(c+1)));
        }
    }

    table->setModel(model);
    mainLayout->addWidget(table);

#endif

    init_q(id < 0);

    init2(db, id);

    // ======= Кнопки OK / Cancel =======
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    // ======= Привязка поиска к спискам =======
    for (int i = 0; i < searchEdits.size(); ++i) {
        connect(searchEdits[i], &QLineEdit::textChanged, this, [=](const QString &text){
            filterList(lists[i], text);
        });
    }

    // ======= Пример реакции на выбор =======
    connect(lists[0], &QListWidget::itemClicked, this, [&](QListWidgetItem *item){
        mk = item->text();
        dm = "";
        
        init(lists[1], db, 1);
        init(lists[2], db, 2);

        textBrowser->setHtml("");

/*
        textBrowser->setHtml(QString("<p>Вы выбрали: <b>%1</b></p>").arg(item->text()));
        model->setItem(0, 0, new QStandardItem(item->text()));
*/
    });

    connect(lists[1], &QListWidget::itemClicked, this, [&](QListWidgetItem *item){
        st = item->text();
        dm = "";
        
        init(lists[2], db, 2);
        textBrowser->setHtml("");
    });

    connect(lists[2], &QListWidget::itemClicked, this, [&](QListWidgetItem *item){
        dm = item->text();
        int id = item->data(Qt::UserRole).toInt();
        init2(db, id);
//        QString s = QString("<p>%1 %2 %3</p>").arg(mk, st, dm);
//        textBrowser->setHtml(s);

    });


}
