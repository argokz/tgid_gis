#pragma once

#include <QDialog>
#include <QListView>
#include <QStandardItemModel>

#include <dialog/SaveGeometry.h>

class PicturesDialog : public QDialog {
    Q_OBJECT

public:
    PicturesDialog(QWidget *parent = nullptr);
    ~PicturesDialog();

    int m_id = -1;


private slots:
    void onImageClicked(const QModelIndex &index);

private:
    QListView *listView;
    QStandardItemModel *model;
    SaveGeometry save;

};

