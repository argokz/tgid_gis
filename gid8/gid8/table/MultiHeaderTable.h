#ifndef MULTIHEADERTABLE_H
#define MULTIHEADERTABLE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTableView>
#include <QStandardItemModel>
#include <QJsonArray>
#include <QLabel>
#include <QPushButton>
#include <QSqlDatabase>
#include <QScrollArea>
#include "ResizableTableView.h"


class MultiHeaderTable : public QWidget {
    Q_OBJECT

public:
    QString tn;
    ResizableTableView *mainTable;
    QScrollArea *scrollArea;

public:
    explicit MultiHeaderTable(QWidget *parent = nullptr);
    void createToolBar(MyMain *main_window);
    void setGidWidget(GidWidget *_gid) { gid = _gid;};
    void init(QSqlDatabase &db, const QString & _tn, const QString & q0, const QString & _title);
    void checkCustomHeader();

    void setMoveTo(MOVE_TO _move_to) {move_to = _move_to;};
    void setAdd(bool yes) { m_add = yes;};
    void setEdit(bool yes) { m_edit = yes;};
    void setDelete(bool yes) { m_delete = yes;};
    void setOpres(const QString &opres) { m_opres = opres;};
    void setDop(const QString &dop) { m_dop = dop;};
    void setMarkColumn(const QString &markColumn) { m_mark_column = markColumn;};
    void setMarkAccessibility(bool yes) { m_mark_accessibility = yes;};


    void reset_q();
    void reset_q(const QString & q);

    void setGeo(const QString & _tn) {move_to = move_to_geo; this->tn = _tn; };

    void set_hide(std::set<int> &h);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void dataChanged();      // ← наш кастомный сигнал

public slots:
    void OnMoveTo();
    void OnExcel();
    void OnHide();
    void OnHide2();
    void OnAll();
    void OnAdd();
    void OnEdit();
    void OnDelete();
    void OnOpres();
    void OnFind();
    void OnFindAll();
    void onPropertyAccepted();
    void onPropertyRejected();
    void OnMarkAccessibility();
    void OnShowCellText();

    void findText(const QString & text);

private:
    QWidget *customHeader;
    QSqlDatabase *m_db = nullptr;

    bool m_add = false;
    bool m_edit = false;
    bool m_delete = false;
    bool m_mark_accessibility = false;
    int m_id = -1;
    QString m_title;
    QString m_opres = "";
    QString m_dop = "";
    QString m_mark_column = "";
    int m_find_col = 0;


    MOVE_TO move_to = move_to_unknown;
    std::vector<QString> v_col;
    GidWidget *gid = nullptr;
    std::vector<QString> old_names;
    bool closed = true;


private:
    void addCustomHeader(const QJsonObject &tableData);
    void syncHeaderWithTable(QScrollArea * qScrollArea);
    void resizeColumnDeal();
    void processHeaders(const QJsonArray &headers, QGridLayout *layout, int row, int &col, const QFont &font, int maxDepth);
    int calculateDepth(const QJsonArray &headers);


private slots:
    void closeTable();

protected:
    bool save_excel(const QString & filename);
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void saveSettings();



};
#endif // MULTIHEADERTABLE_H
