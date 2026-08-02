//#include "mainwindow.h"
#include "PicturesDialog.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QStandardItem>
#include <QLabel>
#include <QPainter>
#include <QMessageBox>

//void drawLinePic1(QPainter *painter, int thickness, double x1, double y1, double x2, double y2, QColor color, QColor colorF, QColor m_bk_color, const int *pics, double mas_otn, bool no_mas, double m_h);
void drawLinePic1(QPainter *painter, int thickness, double x1, double y1, double x2, double y2, QColor color, QColor colorT, QColor colorF, QColor m_bk_color, const int *pics, double mas_otn, bool no_mas, double m_h, double ring);

const int *get_pics(int typ, double coef);
const int *get_pics_map(int typ);

QIcon iconPic(int typ)
{
    int w = 64, h = 64;

    const int *pics = get_pics_map(typ);

    if (!pics) {
      pics = get_pics(typ, 1);
    }

    QColor color = Qt::black, colorF = Qt::blue, m_bk_color = Qt::white;

    QPixmap pixmap(w, h);
    QPainter painter(&pixmap);

    painter.fillRect(0, 0, w, h, m_bk_color);

    drawLinePic1(&painter, 1, w/2, h/2, w-1, h/2, color, color, colorF, m_bk_color, pics, 1, false, 1, 1);

    QIcon icon(pixmap);

    return icon;
}


PicturesDialog::PicturesDialog(QWidget *parent) : 
        QDialog(parent), 
        listView(new QListView(this)), 
        model(new QStandardItemModel(this)),
        save(this, "PicturesDialog")

        
        {

//    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout; //(centralWidget);

    listView->setModel(model);
    listView->setViewMode(QListView::IconMode);
    listView->setIconSize(QSize(64, 64));
    listView->setResizeMode(QListView::Adjust);
    listView->setSelectionMode(QAbstractItemView::SingleSelection);

QIcon *icon_from_text(const QString & str, int cx, int cy);

    // Загрузка картинок
    for (int i = 0; i < 100; ++i) {
//        QString imagePath = QString(":/images/image%1.png").arg(i%3+1);
//        QStandardItem *item = new QStandardItem(QIcon(imagePath), "");

//        QIcon *icon = icon_from_text(QString("%1").arg(i), 64, 64);
//        QStandardItem *item = new QStandardItem(*icon, "");

        int w = 64, h = 64;


//        const int *pics = get_pics(i+58, 1);

        int k = i+58;

        QIcon icon = iconPic(k);

        QStandardItem *item = new QStandardItem(icon, "");

        item->setEditable(false);
        item->setData(QVariant(k));

        model->appendRow(item);
    }

//    connect(listView, &QListView::clicked, this, &PicturesDialog::onImageClicked);
    connect(listView, &QListView::doubleClicked, this, &PicturesDialog::onImageClicked);
//    connect(listView, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, &PicturesDialog::onImageClicked);

    layout->addWidget(listView);

    setLayout(layout);

//    setCentralWidget(centralWidget);
}

void PicturesDialog::onImageClicked(const QModelIndex &index)
{
    m_id = model->itemFromIndex(index)->data().value<int>();
    done(QDialog::Accepted);

//    QString imagePath = model->itemFromIndex(index)->icon().name();
//    QMessageBox::information(this, "Image Selected", "You selected: " + imagePath);
//    QMessageBox::information(this, "Image Selected", QString("%1").arg(k));
}



PicturesDialog::~PicturesDialog() {}
