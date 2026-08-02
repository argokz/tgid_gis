#pragma once

#include <set>
#include <QDialog>
#include <QtWidgets>

#include <QSqlField>

#include <mystd.h>
#include "qvariant.h"

class QListWidget;
class QLineEdit;
class CCxema;

#include <dialog/SaveGeometry.h>
#include "PropertyTree.h"

struct TypeChecker;

#include <init/init_files.h>

struct PropertyStr
{
    QSqlDatabase *db = nullptr;

    QTreeWidgetItem *item_line = nullptr;
    QWidget *widget = nullptr;
    QMetaType metaType;
    QVariant value;
    QVariant old_value;
//---------
    QVariant text = "";
    int index = -1;
//---------
    bool changed = false;
    QString title1 = "";
    QString title2 = "";
    QString table = "";
    QString key = "";
    QString label = "";
    QString f1 = "";
    int id_col = 0;
    int id = -1;
    int fileID = -1;
    int row = 0;
    bool isFunc = false;
    bool readOnly = false;
    QString fun = "";
    QString validate = "";
    QString after = "";
    QString before = "";
    bool isLookup = false;
    bool isLookup2 = false;
    const ForeignTable *lookup = nullptr;
    std::map<int, QString> lookup2; //  = nullptr;

    CCxema *m_cxema = nullptr;

    QVariant getValue();
};

class PropertyDial : public QDialog
{
    Q_OBJECT

public:

//    PropertyDial(QWidget *parent, const QString & table, const QString & table_file, const QString & s_shape = "shape", const QString & s_id = "id");
    PropertyDial(QWidget *parent, const QString & table, const QString & table_file, const QString & s_shape, const QString & s_id, int id1, int id2);

    PropertyDial(QWidget *parent, QSqlDatabase &db, const QString & title, const QString & table, const QString & table_file, int id1, int id2, PrTypes pr_type, CCxema *c);
    void Add(const QString & txt, int id, bool on);

    void setReadOnly(const QString & col, bool on);
    void setVisual(const QString & col, bool on);

    bool init_table(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, PrTypes pr_type);
    bool init_gid(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, PrTypes pr_type, CCxema *cxema, bool pts);

    bool init_gid_with_line(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, int id_l, PrTypes pr_type, CCxema *cxema, bool pts);
    

    bool init_out(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, PrTypes pr_type);
    bool init_out(QSqlDatabase &db, const QString & title, const QString & table, const QString & out_table, int id1, int id2, PrTypes pr_type);

    bool init_fields_for_analiz(QSqlDatabase &db, const QString & table, PrTypes pr_type);
    bool init_fields_for_analiz(QSqlDatabase &db, const QString & table, PrTypes pr_type, const QStringList &allowed_fields);
    
    bool init_analiz(QSqlDatabase &db, const QString & title, const QString & table, PrTypes pr_type);
    bool initAdmissibilityAnalysis(QSqlDatabase &db, const QString & title, const QString & table_out, PrTypes pr_type);
    bool init_dialog(QSqlDatabase *db, const QString & title, const QString & table, std::map<QString, QVariant> &map_val);

    void addParam(const QString &key, const QVariant &val);
    QVariant getParam(const QString &key);


protected:
#if 0
    void addCustomSystemMenu();
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result);
#endif

protected slots:
    void contextMenu(QPoint point);
    void helpMenu(QPoint point);
    void onIndexNull();
    void onHelp();

private slots:
    void okClicked();
    void helpClicked();
    void fontClicked();
    void strClicked();
    void copy_buffer();

    void buttonClicked();
    void clicked();
    void rightClicked();
    void textChanged(const QString &);
    void dateChanged(QDate date);
    void currentTextChanged(const QString &);
    void currentIndexChanged(int index);
    bool init_fields(QSqlDatabase &db, const QString & table, const QString & table_field, int id1, int id2, PrTypes pr_type);
    bool _init_table(QTabWidget *tabWidget, QSqlDatabase &db, const QString & table, PrTypes pr_type, int tab_num0, int &tab_num);
    bool _init_out(QTabWidget *tabWidget, QSqlDatabase &db, const QString & title, const QString & table, PrTypes pr_type, int &tab_num);
    QTabWidget *init_widget();


private:
//    QListWidget lw;
    void hideEvent(QHideEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

    void setDisable(const QString & key, const QString & val);
    void setVis(const QString & key, const QString & val);
    void setVisAll();
    void setDisableAll();

    bool check();

    void addRow(
        QTreeWidgetItem *item_line,
        PropertyTree *tree, 
        QSqlDatabase *db,
        QVariant value1, 
        int id_col,
        int index1,
        int fileID,
        const QMetaType &metaTyp,
        QString table, 
        int id1, 
        QString col,
        QString rus,
        QString f1,
        bool readOnly,
        bool listOnly,
        std::map<int, QString> &lookup2, // = nullptr
        const QString &title1,
        const QString &title2
        );

    void addButton(
        QTreeWidgetItem *item_line,
        PropertyTree *tree, 
        QSqlDatabase *db,
        int id_col,
        int index1,
        int fileID,
        QString table, 
        int id1, 
        QString col,
        QString rus,
        QString f1
        );

    void fillValues(
        const QString &title1,
        std::list<QString> &tab_lst, 
        PropertyTree *tree, 
        QSqlDatabase *db, 
        int tab_num, 
        const QString & table, 
        const QString & prefix, 
        bool readOnly = false, 
        bool noZero = false
    );
    void prop_fun(PropertyStr &pr);
    void prop_fun2(const QString & cmd, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
    bool check_column(PropertyStr pr, const QString & v, QString &msg, QString &msg_notnull, QString &msg_sezon, int &moveID);

    void prop_after(PropertyStr &pr);


public:
    void setValue1(const QString & col, QVariant value);
    void setValue(const QString & col, int row, QVariant value);
    void setId(const QString & col, int id);
    void setIdValue(PropertyStr & pr, int index, const QString & text);
    void setId(PropertyStr & pr, int index);
    PropertyStr getPr1(const QString & col);
    PropertyStr getPr(const QString & col, int row);

    int getIndex(const QString & col);
    QVariant getValue(const QString & col);
    QString getValueString(const QString & col);
    bool getNewValue(const QString & tn, const QString & col, QVariant &val);
    void setPR(const QString & schemeNum);
//    void setCxema(CCxema *c) { m_cxema = c;};
    CCxema *getCxema() { return m_cxema;};

    void set_autodelete() {m_autodelete = true;};
    bool get_autodelete() {return m_autodelete;};

    void setUser(int user) {this->m_user = user;};
    int getUser() {return this->m_user;};


private:
    int getId(const QString & col);

//    QList<ChMenuItem> lst;

    CCxema *m_cxema = nullptr;
    SaveGeometry save;
    int N;

    bool m_autodelete = false;

    QFont *m_font = nullptr;

    QPlainTextEdit *helpWindow;

    // 

    PropertyTree *m_tree;

    std::map<
        std::tuple<QString, QString, QString, int, int>,  // Таблица, Поле, Префикс, номер колонки 0 или 1, Выпадашка
        std::tuple<QSqlField, int, int>                   // Данные из базы, id, fileID 
        > map_val;
    std::list<QString> tab_lst;

    std::map<QString, QString> map_fun;
    std::map<QString, QString> map_validate;
    std::map<QString, QString> map_after;   
    std::map<QString, QString> map_before;  

    std::map <QString, std::map<QString, std::list<QString> > > map_disable;
    std::map <QString, std::map<QString, std::list<QString> > > map_vis;

    std::list<QWidget*> list_widget;


    bool pts = false;

    QString m_table_file = "";

    QString s_shape = "shape";
    QString s_id = "id";

    int m_user = -1;  // Пользователь

//    std::map<QString, std::map<QString, PropertyStr*> > map_
public:
    std::map<QString, std::map<int, std::map<QString, PropertyStr> > > map_edited;
    std::map<QString, QVariant> m_params;

    QString m_table = "";
    int m_id1;
    int m_id2;

    PrTypes m_pr_type;

private:

   void showAboutMessage() {
        QMessageBox::information(this, "О программе", "Информация о программе...");
   }

signals:
    void triggerCustomMenu();  // Сигнал для обработки пользовательского элемента меню

};

bool update_db(std::map<QString, std::map<int, std::map<QString, PropertyStr> > > &map_edited, int m_user);
QString analiz_db(QSqlDatabase &db, std::map<QString, std::map<int, std::map<QString, PropertyStr> > > &map_edited);
