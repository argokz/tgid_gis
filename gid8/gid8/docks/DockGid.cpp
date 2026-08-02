#include <QtGui>
#include <QtWidgets>
#include <QPoint>

#include <QtSql>
#include <any/MyMain.h>

#include <geo/geofile.h>
#include <geo/Klassif.h>

#include <gidview/GidWidget.h>

#include "DockGid.h"
#include <cxema/cxema1.h>

void save_tree(QTreeWidget *tree, const QString & name);
void restore_tree(QTreeWidget *tree, const QString & name);

DockGid::DockGid(QWidget *parent)
    : QWidget(parent)
{
    QString style1 = "QTreeWidget::item:!selected "
      "{ "
        "border: 1px solid gainsboro; "
        "border-left: none; "
        "border-top: none; "
      "}"
      "QTreeWidget::item:selected {}";

//    tree->setStyleSheet(style1);


    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                        QIcon::Normal, QIcon::Off);
    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                        QIcon::Normal, QIcon::On);
    keyIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    tree = new QTreeWidget;
    tree->setHeaderHidden(true);


    tree->setHeaderLabels({ tr("") });
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(tree);
    setLayout(mainLayout);

    tree->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));

    connect(tree, &QTreeWidget::itemClicked, this, &DockGid::itemClicked);
}

QString transl(const QString & s);

QString findTableRusName(const QString & d, const QString & n);


bool DockGid::init(GidWidget *view, CCxema *cxema)
{
    m_view = view;
    tree->clear();

    for (auto &it: cxema->map_fragments) {
        QTreeWidgetItem *i = new QTreeWidgetItem(tree);
        i->setText(0, it.second.m_name);
        i->setData(0,Qt::UserRole,QVariant((int)it.second.m_id));
        i->setCheckState(0, Qt::Checked);
    }

/*
    QTreeWidgetItem *i00 = new QTreeWidgetItem(tree);
    i00->setText(0, baza);
    i00->setCheckState(0, Qt::Checked);

    
    QString fn = QString("%1kls/%2.lst").arg(argpath(), baza);
    QFile file(fn);

    QSet<QString> set_s;


    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);
        QString par;

        QTreeWidgetItem *i0 = nullptr;

        bool end_of_list = false;

        while (!file.atEnd()) {

//            QString s = in.readLine();
            QString s = readCP1251Line(file);

            if (s.left(1) == "-") continue;
            if (s.left(1) == "=") {
                end_of_list = true;
                continue; 
            }
            if (s.length() == 0) continue;

            if (s.left(1) != " ") {
//                par = in.readLine();

                i0 = new QTreeWidgetItem(i00);
                i0->setText(0, s);
//                i0->setIcon(0, groupIcon);
                i0->setCheckState(0, Qt::Checked);
                continue;
            }
            else {
                s = s.trimmed();
  
                Klassif *kls = m_kl_list.findKlN(s);

                if (kls) {
                    set_s.insert(s.toLower());
                    QString rus_name = findTableRusName(baza, s);

                    if (!end_of_list) {
                        QTreeWidgetItem *i1 = new QTreeWidgetItem(i0);
                        i1->setText(0, rus_name);
                        i1->setCheckState(0, kls->ins? Qt::Checked : Qt::Unchecked);
                        if (!kls->ins) {
                            i0->setCheckState(0, Qt::Unchecked);
                        }

                        i1->setData(0,Qt::UserRole,s);
                    }
                    else {
                        kls->ins = false;
                    }
                }

            }
        }
    }
    for (auto & kl : m_kl_list) {
        if (kl.baza == baza && !set_s.contains(kl.nazv.toLower())) {
                QString rus_name = findTableRusName(baza, kl.nazv);
                QTreeWidgetItem *i1 = new QTreeWidgetItem(i00);
                i1->setText(0, rus_name);
                i1->setCheckState(0, kl.ins? Qt::Checked : Qt::Unchecked);
                i1->setData(0,Qt::UserRole,kl.nazv);
         }
    }
*/   

    restore_tree(tree, "Tree/DockGid");

    return true;
}

void DockGid::itemClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *parent = item->parent();
    if (!parent) return;

    QString name = item->data(0, Qt::UserRole).toString();
    Klassif *kls = m_kl_list.findKlN(name);
    if (kls) {
        kls->ins = item->checkState(0) == Qt::Checked;
    }
}

void DockGid::contextMenu(QPoint point)
{
    QMenu *menu=new QMenu(this);

    QAction *aActive = new QAction("Активная", this);

    connect(aActive, SIGNAL(triggered()), this, SLOT(onActive()));

    menu->addAction(aActive);

//        menu->addAction(new QAction("Использовать в адресном поиске", this));
    menu->popup(tree->viewport()->mapToGlobal(point));
}

unsigned long rgb_bgr(unsigned long col);

void DockGid::onActive()
{
    QTreeWidgetItem *item = tree->currentItem();
    if (!item) return;

    int id = item->data(0, Qt::UserRole).toInt();

    if (id > 0) {
        m_view->setFileID(id);
    }

/*
    Klassif *kls = getKls();
    if (kls) {
        QString q = QString("SELECT * FROM %1").arg(kls->nazv);
        QTableView *view = getTableView(*kls->m_db, kls->nazv, q);

        main_window->addWidget(view, kls->nazv, Qt::BottomDockWidgetArea);
    }
*/
}

void DockGid::hideEvent(QHideEvent *event)
{    
    save_tree(tree, "Tree/DockGid");
    QWidget::hideEvent(event);
}

