#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "MyDialog.h"

QLineEdit *MyDialog::addLineEdit(int id, QVariant value, int width)
{
    auto lineEdit = new QLineEdit;

    lineEdit->setFixedWidth(width);

    if (value != QVariant()) {
        lineEdit->setText(value.toString());
    }

    if (value.metaType().id() == QMetaType::UShort) {
        lineEdit->setValidator(new QIntValidator(1, 65535, this));
    }

    EditValue ev;

    ev.w = lineEdit;
    ev.type = value.metaType();
    map_v[id] = ev;

    return lineEdit;
}

QComboBox *MyDialog::addComboBox(int id, int width)
{
    auto comboBox = new QComboBox;

    comboBox->setFixedWidth(width);

    EditValue ev;

    ev.w = comboBox;
//    ev.type = ;
    map_v[id] = ev;

    return comboBox;
}

QWidget *MyDialog::addRow(QBoxLayout *layout, int id, const QString & text, int width, QWidget *w)
{
    QHBoxLayout *rowLayout = new QHBoxLayout;

    auto label = new QLabel(text);

    label->setFixedWidth(width);

    label->setBuddy(w);
    rowLayout->addWidget(label);
    rowLayout->addWidget(w);
    rowLayout->addStretch();

    layout->addLayout(rowLayout);

    return w;
}

QLineEdit *MyDialog::addRowLineEdit(QBoxLayout *layout, int id, const QString & text, QVariant value, int width1, int width2, bool enable)
{
    QLineEdit *lineEdit = addLineEdit(id, value, width2);

    addRow(layout, id, text, width1, (QWidget *)lineEdit);

    if (!enable) {
        lineEdit->setEnabled(false);
    }

    return lineEdit;
}

QIcon *icon_from_text(const QString & str, int cx, int cy);


MyDialog::MyDialog(QWidget *parent, const QString & title)
    : QDialog(parent)
//    , save(this, "MyDialog", title)
{
//    actionIcon = new QIcon;
//    QSize sz(16, 16);
//    actionIcon->addPixmap(this->style()->standardIcon(QStyle::SP_TitleBarUnshadeButton).pixmap(sz));

    actionIcon = icon_from_text("...", 16, 16);
    setWindowTitle(title);

}

QWidget *MyDialog::getWidget(int id)
{
    auto it = map_v.find(id);
    if (it == map_v.end()) return nullptr;

    return it->second.w;
}

QString MyDialog::getValueStr(int id)
{
    QLineEdit *le = (QLineEdit*) getWidget(id);

    if (!le) {
        return "";
    }

    QString str = le->text();

    return str;
}

int MyDialog::getValueInt(int id)
{
    QString str = getValueStr(id);

    return str.toInt();
}

int MyDialog::getCurentCombo(int id)
{
    QComboBox *cb = (QComboBox *) getWidget(id);

    if (!cb) return -1;

    return cb->currentIndex();
}

