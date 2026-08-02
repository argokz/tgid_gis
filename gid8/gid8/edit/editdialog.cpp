#include <QtGui>
#include  <QtWidgets>
#include "scooterwindow.h"
#include "help.h"
#include "editdialog.h"

EditItem *get_item(const QString & name);

QValidator *sqlValidator(QMetaType type, QObject * parent = 0)
{
//  QSqlField & field,
//  QVariant::Type type = field.type();

  switch(type.id()) {
  case QMetaType::Int : return new QIntValidator(parent);
  case QMetaType::Double : return new QDoubleValidator(parent);
  default: return NULL;
  }
//  return NULL;
}

EditDialog::EditDialog(const QString & _tn, QVector<QPair<QString, QVector<QString> >  > & v,  QMap<QString, QVariant> &mapv, QWidget *parent)
    : QDialog(parent)
{
  tabWidget = new QTabWidget;

//  tabWidget->setDocumentMode(true);

//  tabWidget->setTabPosition(QTabWidget::West);
  curent = NULL;

  tn = _tn;
//  v = _v;

  for (int t = 0; t < v.size(); t++) {

    QVBoxLayout *layout = new QVBoxLayout;
    QWidget *w = new QWidget;
    w->setLayout(layout);
    QScrollArea *scrollView = new QScrollArea;
    scrollView->setWidget(w);
    scrollView->setWidgetResizable(true);

    QHBoxLayout *rowLayout = new QHBoxLayout;

    int row = 1;

    for (int i = 0; i < v[t].second.size(); i++) {
      EditItem *item = get_item(v[t].second[i]);
      EditItem *itemNext = NULL;
      if (i+1 < v[t].second.size()) {
        itemNext = get_item(v[t].second[i+1]);
      }
      if (!item->next) {
        rowLayout = new QHBoxLayout;
      }

      HelpStr hs1;
      HelpStr *hs = getHelp(tn, item->name);

      if (hs) {
        hs->item = item;
      }

      if (!hs) {
        hs1.Name = item->name;
        hs = &hs1;
      }

      if (hs) {

        QVariant value("");
        QMap<QString, QVariant>::const_iterator it;

        it = mapv.find(item->name);

        if (it != mapv.end()) {
          value = it.value();
        }

        QLabel *label = new QLabel(QString("%1. %2").arg(row).arg(hs->Help));
        label->setWordWrap (true);

        if (!item->next) {
          rowLayout->addWidget(label);
        }

        QWidget *lineEdit;

        QString typ = item->typ;

        hs->typ = typ;

        if (typ == "Combo" || typ == "ComboTable") {
          QComboBox *cb = new QComboBox;
          int cur = -1;
          for (int j = 0 ; j < item->combo.size(); j++) {
            cb->addItem (item->combo[j]);

            if (item->combo1[j] == value.toString()) {
              cur = j;              
            }
          }
          cb->setEditable (false);
          cb->setCurrentIndex(cur);
          lineEdit = (QWidget *) cb;

          connect(lineEdit, SIGNAL(currentIndexChanged(int)),
                this, SLOT(currentIndexChanged(int)));
        }
        else if (typ == "Kalendar") {
          QDateEdit *dt = new QDateEdit;
          lineEdit = (QWidget *) dt;
          dt->setCalendarPopup(true); 
          dt->setDate(value.toDate());

          connect(dt, SIGNAL(dateChanged( const QDate & )),
                this, SLOT(dateChanged( const QDate & )));
        }
        else if (typ == "EditCALL" || typ == "Dialog_Call" || typ == "Table" || typ == "EditTable") {
          QComboBox *cb = new QComboBox;
          lineEdit = (QWidget *) cb;
          cb->setEditText(value.toString());

          if (typ == "EditCALL" || typ == "EditTable") {
            cb->setEditable (true);
            cb->setValidator(sqlValidator(item->sql_type, cb));
            cb->setEditText(value.toString());
          }
          else {
            cb->setEditable (false);
          }
          connect(lineEdit, SIGNAL(textChanged(const QString &)),
                this, SLOT(textChanged(const QString &)));
        }
        else {
          QLineEdit *le = new QLineEdit;
          lineEdit = (QLineEdit *) le;

          if (typ == "Label") {
            le->setReadOnly (true);
          }

          le->setValidator(sqlValidator(item->sql_type, le));
          le->setText(value.toString());
//          le->setText(QString("type=%1").arg(item->sql_type));

          connect(lineEdit, SIGNAL(textChanged(const QString &)),
                this, SLOT(textChanged(const QString &)));
        }

        lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

         if (!item->next) {
          label->setBuddy(lineEdit);
          label->setFixedWidth (300);
        }

  //      rowLayout->addStretch();
  //      lineEdit->setFixedWidth (250);

        lineEdit->setObjectName(item->name);
        lineEdit->installEventFilter(this); 

        rowLayout->addWidget(lineEdit);


        if (hs->Stand_Razmer != "" && (!itemNext || !itemNext->next )) {
          label = new QLabel(hs->Stand_Razmer);
          rowLayout->addWidget(label);
        }

        if (!item->next) {
          layout->addLayout(rowLayout);
          row++;
        }
      }
    }
    layout->addStretch();
    tabWidget->addTab(scrollView, v[t].first);
  }

  okButton = new QPushButton(tr("&Ok"));
  okButton->setDefault(true);
  okButton->setEnabled(true);

  closeButton = new QPushButton(tr("Close"));
  QPushButton *helpButton = new QPushButton(tr("Help"));
  QPushButton *reportButton = new QPushButton(tr("Report"));
  QPushButton *tableButton = new QPushButton(tr("Table"));

  helpButton->setObjectName("Help!");

  connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));
  connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QHBoxLayout *buttonBox = new QHBoxLayout;

  buttonBox->addWidget(reportButton);
  buttonBox->addWidget(tableButton);
  
  buttonBox->addStretch();
  buttonBox->addWidget(helpButton);
  buttonBox->addWidget(okButton);
  buttonBox->addWidget(closeButton);

  QVBoxLayout *mainLayout = new QVBoxLayout;

  mainLayout->addWidget(tabWidget);
  mainLayout->addLayout(buttonBox);
  setLayout(mainLayout);

//  setWindowTitle(tr("Edit"));
  setFixedHeight(sizeHint().height());
}

void EditDialog::helpClicked()
{
  if (curent && curent->objectName () != "") {
    HelpStr *hs = getHelp(tn, curent->objectName ());
    if (hs) {
      QMessageBox::information(this, tr("Help"), hs->HelpShiftF1);
    }
    curent->setFocus(Qt::MouseFocusReason);
  }
}

void EditDialog::okClicked()
{
  done(QDialog::Accepted);
}

bool EditDialog::eventFilter ( QObject * o, QEvent * e )
{
  if (e->type() == QEvent::FocusIn) {
    curent = static_cast<QWidget*> (o);
  }
  else if (e->type() == QEvent::MouseButtonPress/* && o == comboBox*/) {
//    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
//    HelpStr *hs = getHelp(tn, o->objectName ());
//    if (hs) {
    EditItem *item = get_item(o->objectName());
    if (item) {

      if (item->typ == "EditCALL" || item->typ == "Dialog_Call") {
        QMessageBox::warning(this, ("!"), o->objectName());
        return true;
      }
      else if (item->typ == "Table" || item->typ == "EditTable") {
        ScooterWindow *window = new ScooterWindow(item->dbn, item->table, item->query, this);
        window->resize(600, 400);
        window->exec();
        return true;
      }
    }
  }
  else if (e->type() == QEvent::Wheel) {
    return true;
  }
  return QDialog::eventFilter ( o, e );
}

void EditDialog::runDialog(int n)
{
  if (curent) {
    EditItem *item = get_item(sender()->objectName());
    if (item) {
      QMessageBox::information(0, tr("Dialog"), sender()->objectName());
    }
    curent->setFocus(Qt::MouseFocusReason);
  }
}


void EditDialog::textChanged(const QString & txt)
{
  mapv_out[sender()->objectName()] = QVariant(txt);
}

void EditDialog::dateChanged( const QDate & date)
{
//  QMessageBox::information(0, tr("Dialog"), date.toString());
  mapv_out[sender()->objectName()] = QVariant(date);
}

void EditDialog::currentIndexChanged(int index)
{
  if (index >= 0) {
    EditItem *item = get_item(sender()->objectName());
    if (item) {
      mapv_out[sender()->objectName()] = item->combo1[index];
    }
  }
}

