/********************************************************************************
** Form generated from reading UI file 'param_pt.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef PARAM_PT_H
#define PARAM_PT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ParamPT
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox_2;
    QHBoxLayout *hboxLayout;
    QGroupBox *groupBox_4;
    QVBoxLayout *vboxLayout1;
    QComboBox *comboBox;
    QGroupBox *groupBox_PO;
    QHBoxLayout *hboxLayout1;
    QGroupBox *groupBox_P;
    QGridLayout *gridLayout;
    QLabel *label_P_1;
    QLineEdit *lineEdit_P_1;
    QPushButton *pushButton_P_1;
    QLabel *label_P_1_;
    QLineEdit *lineEdit_P_1_;
    QLabel *label_P_2;
    QLineEdit *lineEdit_P_2;
    QPushButton *pushButton_P_2;
    QLabel *label_P_3;
    QLineEdit *lineEdit_P_3;
    QPushButton *pushButton_P_3;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(322, 227);
        vboxLayout = new QVBoxLayout(Dialog);
        vboxLayout->setObjectName("vboxLayout");
        groupBox_2 = new QGroupBox(Dialog);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setFlat(true);
        hboxLayout = new QHBoxLayout(groupBox_2);
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName("hboxLayout");
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        groupBox_4 = new QGroupBox(groupBox_2);
        groupBox_4->setObjectName("groupBox_4");
        groupBox_4->setFlat(true);
        vboxLayout1 = new QVBoxLayout(groupBox_4);
        vboxLayout1->setObjectName("vboxLayout1");
        comboBox = new QComboBox(groupBox_4);
        comboBox->setObjectName("comboBox");

        vboxLayout1->addWidget(comboBox);

        groupBox_PO = new QGroupBox(groupBox_4);
        groupBox_PO->setObjectName("groupBox_PO");
        groupBox_PO->setFlat(true);
        hboxLayout1 = new QHBoxLayout(groupBox_PO);
        hboxLayout1->setObjectName("hboxLayout1");
        hboxLayout1->setContentsMargins(1, 1, 1, 1);
        groupBox_P = new QGroupBox(groupBox_PO);
        groupBox_P->setObjectName("groupBox_P");
        gridLayout = new QGridLayout(groupBox_P);
        gridLayout->setObjectName("gridLayout");
        label_P_1 = new QLabel(groupBox_P);
        label_P_1->setObjectName("label_P_1");

        gridLayout->addWidget(label_P_1, 0, 2, 1, 1);

        lineEdit_P_1 = new QLineEdit(groupBox_P);
        lineEdit_P_1->setObjectName("lineEdit_P_1");

        gridLayout->addWidget(lineEdit_P_1, 0, 3, 1, 1);

        pushButton_P_1 = new QPushButton(groupBox_P);
        pushButton_P_1->setObjectName("pushButton_P_1");

        gridLayout->addWidget(pushButton_P_1, 0, 4, 1, 1);

        label_P_1_ = new QLabel(groupBox_P);
        label_P_1_->setObjectName("label_P_1_");

        gridLayout->addWidget(label_P_1_, 1, 0, 1, 1);

        lineEdit_P_1_ = new QLineEdit(groupBox_P);
        lineEdit_P_1_->setObjectName("lineEdit_P_1_");

        gridLayout->addWidget(lineEdit_P_1_, 1, 1, 1, 1);

        label_P_2 = new QLabel(groupBox_P);
        label_P_2->setObjectName("label_P_2");

        gridLayout->addWidget(label_P_2, 1, 2, 1, 1);

        lineEdit_P_2 = new QLineEdit(groupBox_P);
        lineEdit_P_2->setObjectName("lineEdit_P_2");

        gridLayout->addWidget(lineEdit_P_2, 1, 3, 1, 1);

        pushButton_P_2 = new QPushButton(groupBox_P);
        pushButton_P_2->setObjectName("pushButton_P_2");

        gridLayout->addWidget(pushButton_P_2, 1, 4, 1, 1);

        label_P_3 = new QLabel(groupBox_P);
        label_P_3->setObjectName("label_P_3");

        gridLayout->addWidget(label_P_3, 2, 2, 1, 1);

        lineEdit_P_3 = new QLineEdit(groupBox_P);
        lineEdit_P_3->setObjectName("lineEdit_P_3");

        gridLayout->addWidget(lineEdit_P_3, 2, 3, 1, 1);

        pushButton_P_3 = new QPushButton(groupBox_P);
        pushButton_P_3->setObjectName("pushButton_P_3");

        gridLayout->addWidget(pushButton_P_3, 2, 4, 1, 1);


        hboxLayout1->addWidget(groupBox_P);


        vboxLayout1->addWidget(groupBox_PO);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vboxLayout1->addItem(verticalSpacer);


        hboxLayout->addWidget(groupBox_4);


        vboxLayout->addWidget(groupBox_2);

        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox, 0, Qt::AlignLeft);


        retranslateUi(Dialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, Dialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, Dialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("ParamPT", "\320\243\320\272\320\260\320\266\320\270\321\202\320\265 \320\277\321\200\320\265\320\264\320\265\320\273\321\213 \320\277\320\260\321\200\320\260\320\274\320\265\321\202\321\200\320\276\320\262 \320\264\320\273\321\217 \320\262\321\213\320\264\320\265\320\273\320\265\320\275\320\270\321\217", nullptr));
        groupBox_2->setTitle(QString());
        groupBox_4->setTitle(QString());
        groupBox_P->setTitle(QCoreApplication::translate("ParamPT", "\320\237\320\276\320\264\320\260\321\207\320\260", nullptr));
        label_P_1->setText(QCoreApplication::translate("ParamPT", "\342\251\275", nullptr));
        pushButton_P_1->setText(QString());
        label_P_1_->setText(QCoreApplication::translate("ParamPT", ">", nullptr));
        label_P_2->setText(QCoreApplication::translate("ParamPT", "\342\251\275", nullptr));
        pushButton_P_2->setText(QString());
        label_P_3->setText(QCoreApplication::translate("ParamPT", ">", nullptr));
        pushButton_P_3->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ParamPT: public Ui_ParamPT {};
} // namespace Ui

QT_END_NAMESPACE

#endif // PARAM_PT_H
