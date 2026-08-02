#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "ChMenuDial.h"

ChMenuDial::ChMenuDial(QWidget *parent, const QString & title, bool _up_down)
    : QDialog(parent)
    , save(this, "ChMenuDial", title)
{
    up_down = _up_down;

    QPushButton *okButton = new QPushButton(tr("&Ok"));
    okButton->setDefault(true);
    okButton->setEnabled(true);

    QPushButton *closeButton = new QPushButton(tr("Отменить"));
    QPushButton *helpButton = new QPushButton(tr("Помощь"));
    QPushButton *allButton = new QPushButton(tr("Выделить"));
    QPushButton *ubrButton = new QPushButton(tr("Убрать"));


//    QVBoxLayout *buttonBox = new QVBoxLayout;
    buttonBox = new QVBoxLayout;

//    buttonBox->addWidget(helpButton);
    buttonBox->addWidget(okButton);
    buttonBox->addWidget(closeButton);
    buttonBox->addStretch();

/*
    if (up_down) {
        QPushButton *upButton = new QPushButton("↑");
        QPushButton *downButton = new QPushButton("↓");
        buttonBox->addWidget(upButton);
        buttonBox->addWidget(downButton);
        buttonBox->addStretch();
        connect(upButton, SIGNAL(clicked()), this, SLOT(upClicked()));
        connect(downButton, SIGNAL(clicked()), this, SLOT(downClicked()));
    }
*/
    buttonBox->addWidget(allButton);
    buttonBox->addWidget(ubrButton);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    
    mainLayout->addWidget(&lw);

    mainLayout->addLayout(buttonBox);

    setLayout(mainLayout);

    setWindowTitle(title);


    lw.setDragDropMode(QAbstractItemView::InternalMove);


//    connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
 

    connect(allButton, SIGNAL(clicked()), this, SLOT(all()));
    connect(ubrButton, SIGNAL(clicked()), this, SLOT(ubr()));
}


void ChMenuDial::Add(const QString & txt, QVariant id, bool on)
{
    QListWidgetItem *item = new QListWidgetItem(txt, &lw);

    if (on)
        item->setCheckState(Qt::Checked);
    else
        item->setCheckState(Qt::Unchecked);

    ChMenuItem chitem;
    chitem.id = id;
    chitem.txt = txt;
    chitem.on = on;

    QVariant variant;
    variant.setValue(chitem);

    item->setData(Qt::UserRole, variant);

    lw.addItem(item);



//    lst.push_back(chitem);
}

void ChMenuDial::helpClicked()
{
  QMessageBox::warning(this, tr("!"), "!");
}

void ChMenuDial::okClicked()
{
//    int N = lst.size();

    int N = lw.count();

    for (int i = 0; i < N; i++) {
        QListWidgetItem *item = lw.item (i);
        if (item) {
            if (item->checkState () == Qt::Checked) {
//                out.push_back(lst[i]);
//                out.insert(lst[i]);
                ChMenuItem it = item->data(Qt::UserRole).value<ChMenuItem>();
                out.push_back(it.id);
            }
        }
    }
    done(QDialog::Accepted);
}


void ChMenuDial::all()
{
//    int N = lst.size();
    int N = lw.count();

    for (int i = 0; i < N; i++) {
        QListWidgetItem *item = lw.item (i);
        if (item) {
            item->setCheckState(Qt::Checked);
        }
    }
}

void ChMenuDial::ubr()
{
//    int N = lst.size();
    int N = lw.count();

    for (int i = 0; i < N; i++) {
        QListWidgetItem *item = lw.item (i);
        if (item) {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

bool ChMenuDial::inOut(QVariant id)
{
//    return out.find(id) != out.end();

    for (auto &it : out) {
        if (it == id) return true;
    }
    return false;
}

void ChMenuDial::addButton(const QString & txt, QWidget *widget, const char *slot_name)
{
    QPushButton *button = new QPushButton(txt);
    buttonBox->addWidget(button);

    connect(button, SIGNAL(clicked()), widget, slot_name);
}



void ChMenuDial::upClicked()
{
}

void ChMenuDial::downClicked()
{
}
