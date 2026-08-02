#include "inputdialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFormLayout>

InputDialog::InputDialog(QStringList label, QStringList value, QWidget *parent)
    : QDialog(parent) , save(this, "InputDialog")
{
    QFormLayout *lytMain = new QFormLayout(this);

    for (int i = 0; i < label.length(); ++i)
    {
        QLabel *tLabel = new QLabel(label[i], this);
        QLineEdit *tLine = new QLineEdit(this);
        lytMain->addRow(tLabel, tLine);
        tLine->setText(value[i]);

        fields << tLine;
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox
            ( QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
              Qt::Horizontal, this );
    lytMain->addWidget(buttonBox);

    bool conn = connect(buttonBox, &QDialogButtonBox::accepted,
                   this, &InputDialog::accept);
    Q_ASSERT(conn);
    conn = connect(buttonBox, &QDialogButtonBox::rejected,
                   this, &InputDialog::reject);
    Q_ASSERT(conn);

    setLayout(lytMain);
}

QStringList InputDialog::getStrings(bool *ok)
{
    QStringList lst;

    foreach (auto field, fields) {
        lst << field->text();
    }

    return lst;
}
