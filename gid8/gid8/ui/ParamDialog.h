#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QDebug>

class ParamDialog : public QDialog {
    Q_OBJECT

public:

    virtual ~ParamDialog() {};
    
    ParamDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Введите дополнитеьные параметры для расчета");
        QVBoxLayout *layout = new QVBoxLayout(this);


        {
            QHBoxLayout *param1Layout = new QHBoxLayout;
            QLabel *label1 = new QLabel("Плотность воды в подаче, т/м3");
            edit_roP = new QLineEdit;
            edit_roP->setValidator(new QDoubleValidator(0.0, 2.0, 3));

            param1Layout->addWidget(label1);
            param1Layout->addWidget(edit_roP);
            layout->addLayout(param1Layout);
        }
        {
            QHBoxLayout *param1Layout = new QHBoxLayout;
            QLabel *label1 = new QLabel("Плотность воды в обратке, т/м3");
            edit_roO = new QLineEdit;
            edit_roO->setValidator(new QDoubleValidator(0.0, 2.0, 3));
            param1Layout->addWidget(label1);
            param1Layout->addWidget(edit_roO);
            layout->addLayout(param1Layout);
        }

        {
            QHBoxLayout *param1Layout = new QHBoxLayout;

            check1 = new QCheckBox(tr("Плотность по температуре"));

//            QLabel *label1 = new QLabel("Плотность воды в подаче, т/м3");

//            param1Layout->addWidget(label1);
            param1Layout->addWidget(check1);
            layout->addLayout(param1Layout);
        }



        QHBoxLayout *param2Layout = new QHBoxLayout;
        QLabel *label2 = new QLabel("Формула для расчета гидравлического сопротивления");
        comboBox = new QComboBox;

#if USE_ITWIN
const QString ItWin = "ItWin";
#else
const QString ItWin = "ТГид";
#endif

        comboBox->addItems({ItWin, "Альтшуля", "Никурадзе", "Шифринсона", "Колбрука-Уайта"});
        param2Layout->addWidget(label2);
        param2Layout->addWidget(comboBox);
        layout->addLayout(param2Layout);

        QPushButton *okButton = new QPushButton("OK");
        QPushButton *cancelButton = new QPushButton("Отмена");
        connect(okButton, &QPushButton::clicked, this, &ParamDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &ParamDialog::reject);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);
        layout->addLayout(buttonLayout);
    }

    double getRoP() const { return edit_roP->text().toDouble(); }
    double getRoO() const { return edit_roO->text().toDouble(); }
    bool getCheck1() const { return check1->checkState() == Qt::Checked; }

    void setRoP(double ro) { edit_roP->setText(QString("%1").arg(ro)); }
    void setRoO(double ro) { edit_roO->setText(QString("%1").arg(ro)); }

    void setCheck1(bool yes) {
       check1->setCheckState(yes ? Qt::Checked : Qt::Unchecked);
    }


    int getParam2Index() const { return comboBox->currentIndex(); }
    void setParam2Index(int index) { comboBox->setCurrentIndex(index); }

private:
    QCheckBox *check1;
    QLineEdit *edit_roP;
    QLineEdit *edit_roO;
    QComboBox *comboBox;
};
