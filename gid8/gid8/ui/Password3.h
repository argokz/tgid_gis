/********************************************************************************
** Form generated from reading UI file 'Password3.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef PASSWORD3_H
#define PASSWORD3_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Password3
{
public:
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox1;
    QVBoxLayout *vboxLayout;
    QListWidget *listWidget;
    QGroupBox *groupBox2;
    QVBoxLayout *vboxLayout1;
    QPushButton *push_Ok;
    QPushButton *push_Cancel;
    QSpacerItem *verticalSpacer;
    QPushButton *pushButton_Delete;
    QPushButton *pushButton_Edit;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(480, 623);
        horizontalLayout = new QHBoxLayout(Dialog);
        horizontalLayout->setObjectName("horizontalLayout");
        groupBox1 = new QGroupBox(Dialog);
        groupBox1->setObjectName("groupBox1");
        vboxLayout = new QVBoxLayout(groupBox1);
        vboxLayout->setObjectName("vboxLayout");
        listWidget = new QListWidget(groupBox1);
        listWidget->setObjectName("listWidget");

        vboxLayout->addWidget(listWidget);


        horizontalLayout->addWidget(groupBox1);

        groupBox2 = new QGroupBox(Dialog);
        groupBox2->setObjectName("groupBox2");
        vboxLayout1 = new QVBoxLayout(groupBox2);
        vboxLayout1->setObjectName("vboxLayout1");
        push_Ok = new QPushButton(groupBox2);
        push_Ok->setObjectName("push_Ok");

        vboxLayout1->addWidget(push_Ok);

        push_Cancel = new QPushButton(groupBox2);
        push_Cancel->setObjectName("push_Cancel");

        vboxLayout1->addWidget(push_Cancel);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vboxLayout1->addItem(verticalSpacer);

        pushButton_Delete = new QPushButton(groupBox2);
        pushButton_Delete->setObjectName("pushButton_Delete");

        vboxLayout1->addWidget(pushButton_Delete);

        pushButton_Edit = new QPushButton(groupBox2);
        pushButton_Edit->setObjectName("pushButton_Edit");

        vboxLayout1->addWidget(pushButton_Edit);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vboxLayout1->addItem(verticalSpacer_2);


        horizontalLayout->addWidget(groupBox2);


        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Password3", "\320\222\321\213\320\261\320\265\321\200\320\270\321\202\320\265 \320\277\320\276\320\273\321\214\320\267\320\276\320\262\320\260\321\202\320\265\320\273\321\217 \321\201\320\270\321\201\321\202\320\265\320\274\321\213", nullptr));
        push_Ok->setText(QCoreApplication::translate("Password3", "OK", nullptr));
        push_Cancel->setText(QCoreApplication::translate("Password3", "\320\236\321\202\320\274\320\265\320\275\320\270\321\202\321\214", nullptr));
        pushButton_Delete->setText(QCoreApplication::translate("Password3", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214", nullptr));
        pushButton_Edit->setText(QCoreApplication::translate("Password3", "\320\224\320\276\321\201\321\202\321\203\320\277", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Password3: public Ui_Password3 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // PASSWORD3_H
