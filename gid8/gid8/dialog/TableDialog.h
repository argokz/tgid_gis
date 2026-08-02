#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include <dialog/SaveGeometry.h>

class TableDialog : public QDialog {
    Q_OBJECT

public:
    TableDialog(const QString & title, QWidget *parent = nullptr);
    void init();
    void addColumn(const QString & col);
    int addRow();
    void setValue(int row, int col, const QString & val);
    void setId(int row, QVariant id);
    void setCurrentRow(int row);

    QVariant m_id;


protected:
    void hideEvent(QHideEvent *event);
    void resizeEvent(QResizeEvent *event) override;
//    void mouseDoubleClickEvent(QMouseEvent *event);


protected:
    void adjustColumnWidths();


private slots:
    void selectRow();
    void tableItemClicked(int row, int col);

private:
    QTableWidget *tableWidget;

    std::map<int, QVariant> mapId;

    QString m_title = "";
    SaveGeometry save;
};
