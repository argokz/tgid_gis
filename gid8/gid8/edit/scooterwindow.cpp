#include <QtGui>
#include <QtSql>
#include  <QtWidgets>

#include "scooterwindow.h"
#include "help.h"

ScooterWindow::ScooterWindow(const QString & dbn, const QString & tn, const QString & que, QWidget *parent)
    : QDialog(parent)
{
//    model = new QSqlTableModel(this);
    model = new QSqlQueryModel(this);
//    model->setTable("Help_BP_OUT");

    QSqlDatabase db = QSqlDatabase::database(dbn); 

    if (que != "") {
      model->setQuery(que, db);
    }
    else {
      model->setQuery("SELECT * FROM [" + tn + "]", db);
    }

    QSqlRecord record = model->query().record();

    for (int i = 0; i < record.count(); i++) {
      QSqlField field = record.field(i);
      HelpStr *hs = getHelp(tn, record.fieldName(i));

      if (hs) {
//      QMessageBox::warning(this, tn, record.fieldName(i));
        model->setHeaderData(i, Qt::Horizontal, hs->Russu);
      }
    }    

    view = new QTableView;
    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
//    view->setColumnHidden(Scooter_Id, true);
    view->resizeColumnsToContents();
//    view->setEditTriggers(QAbstractItemView::AllEditTriggers);

//    QHeaderView *header = view->horizontalHeader();
//    header->setStretchLastSection(true);
//    header->setSortIndicatorShown(true);

    QPushButton *okButton = new QPushButton(tr("&Ok"));
    okButton->setDefault(true);
    okButton->setEnabled(true);

    QPushButton *closeButton = new QPushButton(tr("Close"));
    QPushButton *helpButton = new QPushButton(tr("Help"));

    QHBoxLayout *buttonBox = new QHBoxLayout;

    buttonBox->addStretch();
    buttonBox->addWidget(helpButton);
    buttonBox->addWidget(okButton);
    buttonBox->addWidget(closeButton);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(view);

    mainLayout->addLayout(buttonBox);

    setLayout(mainLayout);

    setWindowTitle(tn);

    connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}


void ScooterWindow::helpClicked()
{
/*
  if (curent && curent->objectName () != "") {
    HelpStr *hs = getHelp(tn, curent->objectName ());
    if (hs) {
      QMessageBox::information(this, tr("Help"), hs->HelpShiftF1);
    }
    curent->setFocus(Qt::MouseFocusReason);
  }
*/
  QMessageBox::warning(this, tr("!"), "!");
}

void ScooterWindow::okClicked()
{
  QMessageBox::warning(this, tr("!"), "!");
  close();
}
