#include "pumpcomputedialog.h"
#include "ui_pumpcomputedialog.h"


PumpComputeDialog::PumpComputeDialog(QWidget *parent, QSqlDatabase &db, int pumpId, bool approximateMode)
    : QDialog(parent)
    , ui(new Ui::PumpComputeDialog)
    , save(this, "PumpComputeDialog")
{
    m_db = &db;
    ui->setupUi(this);
    mApproximateMode = approximateMode;
    if (!mApproximateMode)
        ui->groupBox_3->setVisible(false);
    else
        ui->label->setVisible(false);

    connect(ui->checkChar1, SIGNAL(stateChanged(int)), this, SLOT(stateCharN(int)));

    connect(ui->checkChar2, SIGNAL(stateChanged(int)), this, SLOT(stateCharP(int)));

    connect(ui->push_Report, SIGNAL(clicked()), this, SLOT(clickReport()));

    connect(ui->push_Cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->push_Ok, SIGNAL(clicked()), this, SLOT(okClicked()));
}


void PumpComputeDialog::stateCharN(int b){

    if (paintWidget != nullptr){
        paintWidget->pumpGraph.Show_H_chart = b;
        paintWidget->pumpGraph.Show_KPD_chart = b;
        paintWidget->pumpGraph.Show_N_chart = b;
        paintWidget->repaint();
    }
}

void PumpComputeDialog::stateCharP(int b){

    if (paintWidget != nullptr){
        paintWidget->pumpGraph.Show_Hr_chart = b;
        paintWidget->pumpGraph.Show_KPDr_chart = b;
        paintWidget->pumpGraph.Show_Nr_chart = b;
        paintWidget->repaint();
    }
}

void PumpComputeDialog::clickReport(){
    paintWidget->isReportMode = !paintWidget->isReportMode;
    if (paintWidget->isReportMode)
        ui->push_Report->setText("Графики");
    else
        ui->push_Report->setText("Отчет");
    paintWidget->repaint();
}

void PumpComputeDialog::addGraph(int id, GRAPHR_PARAM grApr, int k_kol) {
    QSqlQuery query(*m_db);
    // Создаем виджет для рисования
    paintWidget = new PumpGraphWidget(ui->staticWidget,query,id, grApr, k_kol, mApproximateMode);


    ui->verticalLayout->addWidget(paintWidget);
}

void PumpComputeDialog::okClicked()
{
    if (mApproximateMode){
        pReport.apH = paintWidget->pumpGraph.apH;
        pReport.apHr = paintWidget->pumpGraph.apHr;
        pReport.apKPD = paintWidget->pumpGraph.apKPD;
        pReport.apKPDr = paintWidget->pumpGraph.apKPDr;
        pReport.apN = paintWidget->pumpGraph.apN;
        pReport.apNr = paintWidget->pumpGraph.apNr;

        pReport.k_kol= paintWidget->pumpGraph.DiamGR.k_kol;
        pReport.d= paintWidget->pumpGraph.GrApr->diam;
        pReport.f= paintWidget->pumpGraph.GrApr->c_vr;
        pReport.typePump = paintWidget->pumpGraph.DiamGR.Tip;
        pReport.calc = true;
    }
    else{
        pReport.calc = false;
        pReport.d_obt = paintWidget->pumpGraph.d_obt;
    }
    emit dialogAccepted();

    this->accept();
}


PumpComputeDialog::~PumpComputeDialog()
{
    delete ui;
}
