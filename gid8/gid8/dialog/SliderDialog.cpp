#include <QApplication>
#include <QSlider>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>

#if 0

class SliderDialog : public QDialog {
public:
    SliderDialog(QWidget *parent, const QString & caption, int value) : QDialog(parent) {
        setWindowTitle(caption);

        // Создаем горизонтальный ползунок
        slider = new QSlider(Qt::Horizontal);
        slider->setRange(0, 100);
        slider->setValue(value);

        // Создаем метку для отображения значения ползунка
        label = new QLabel(QString("%1").arg(slider->value()));

        // Создаем макет для размещения ползунка и метки
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(slider);
        layout->addWidget(label);

        setLayout(layout);

        // Соединяем сигнал изменения значения с нашим слотом
        connect(slider, &QSlider::valueChanged, this, &SliderDialog::onValueChanged);
    }

    int getValue() const {
        return slider->value();
    }

private slots:
    void onValueChanged(int value) {
        // При изменении значения ползунка, обновляем метку
        label->setText(QString("%1").arg(value));
    }

private:
    QSlider *slider;
    QLabel *label;
};

int getSliderValue1(QWidget *parent, const QString & text, int value) {
//    QApplication app;
    SliderDialog dialog(parent, text, value);
    if (dialog.exec() == QDialog::Accepted) {
        return dialog.getValue();
    }
//    return value;
}


/*

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Вызываем функцию для отображения диалогового окна с ползунком и получения значения
    int selectedValue = getSliderValue();

    // Используем выбранное значение
    qDebug() << "Selected value: " << selectedValue;

    return 0;
}
*/

#endif

#include <QApplication>
#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

class SliderDialog : public QDialog {
    Q_OBJECT

public:
    SliderDialog(QWidget *parent = nullptr, int initialValue = 0) : QDialog(parent) {
        slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(0, 100);
        slider->setValue(initialValue);

        lineEdit = new QLineEdit(this);
        lineEdit->setValidator(new QIntValidator(0, 100, this));
        lineEdit->setText(QString::number(initialValue));

        auto *label = new QLabel("%", this);
        auto *okButton = new QPushButton("OK", this);

        // Layout for slider and input
        auto *inputLayout = new QHBoxLayout();
        inputLayout->addWidget(lineEdit);
        inputLayout->addWidget(label);

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(slider);
        mainLayout->addLayout(inputLayout);
        mainLayout->addWidget(okButton);
        setLayout(mainLayout);

        // Connect slider and line edit
        connect(slider, &QSlider::valueChanged, this, [this](int value) {
            lineEdit->setText(QString::number(value));
        });

        connect(lineEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
            slider->setValue(text.toInt());
        });

        // Close dialog on OK button click
        connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    }

    int getValue() const {
        return slider->value();
    }

private:
    QSlider *slider;
    QLineEdit *lineEdit;
};

//int main(int argc, char *argv[]) {
int getSliderValue(QWidget *parent, const QString & text, int value)
{
//    QApplication app(argc, argv);

    SliderDialog dlg(parent, value);
    dlg.setWindowTitle(text);
    dlg.resize(300, 150);

    if (dlg.exec() == QDialog::Accepted) {
        value = dlg.getValue();
    }

    return value;
}


#include "SliderDialog.moc"
