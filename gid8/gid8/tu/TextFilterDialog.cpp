#include "TextFilterDialog.h"
#include <QVBoxLayout>


// Функция для замены указанных букв в строке
QString replaceLetters(const QString& input, const QMap<QChar, QChar>& replacements) {
    QString result = input;
    
    // Проходим по каждому символу в строке
    for (int i = 0; i < result.length(); ++i) {
        // Если символ есть в словаре замен, заменяем его
        if (replacements.contains(result[i])) {
            result[i] = replacements[result[i]];
        }
    }
    
    return result;
}


QString cyr(const QString& input)
{
    QMap<QChar, QChar> replacements;
    
    replacements[';'] = QStringLiteral("ж").at(0);
    replacements[','] = QStringLiteral("б").at(0);
    replacements['.'] = QStringLiteral("ю").at(0);
    replacements['q'] = QStringLiteral("й").at(0);
    replacements['w'] = QStringLiteral("ц").at(0);
    replacements['e'] = QStringLiteral("у").at(0);
    replacements['r'] = QStringLiteral("к").at(0);
    replacements['t'] = QStringLiteral("е").at(0);
    replacements['y'] = QStringLiteral("н").at(0);
    replacements['u'] = QStringLiteral("г").at(0);
    replacements['i'] = QStringLiteral("ш").at(0);
    replacements['o'] = QStringLiteral("щ").at(0);
    replacements['p'] = QStringLiteral("з").at(0);
    replacements['a'] = QStringLiteral("ф").at(0);
    replacements['s'] = QStringLiteral("ы").at(0);
    replacements['d'] = QStringLiteral("в").at(0);
    replacements['f'] = QStringLiteral("а").at(0);
    replacements['g'] = QStringLiteral("п").at(0);
    replacements['h'] = QStringLiteral("р").at(0);
    replacements['j'] = QStringLiteral("о").at(0);
    replacements['k'] = QStringLiteral("л").at(0);
    replacements['l'] = QStringLiteral("д").at(0);
    replacements['z'] = QStringLiteral("я").at(0);
    replacements['x'] = QStringLiteral("ч").at(0);
    replacements['c'] = QStringLiteral("с").at(0);
    replacements['v'] = QStringLiteral("м").at(0);
    replacements['b'] = QStringLiteral("и").at(0);
    replacements['n'] = QStringLiteral("т").at(0);
    replacements['m'] = QStringLiteral("ь").at(0);

    return replaceLetters(input, replacements);
}



TextFilterDialog::TextFilterDialog(QWidget *parent)
    : QDialog(parent)
    , save(this, "TextFilterDialog")
{
//    setWindowTitle("Фильтрация по тексту");
    setWindowTitle("Поиск Технических условий");

    QVBoxLayout *layout = new QVBoxLayout(this);

    lineEdit = new QLineEdit();
//    lineEdit->setPlaceholderText("Введите текст...");
    lineEdit->setPlaceholderText("Введите одну из величин : Номер ТУ, наименование объекта ТУ, наименование организации ТУ, адрес здания ТУ");
    

    listWidget = new QListWidget();

    layout->addWidget(lineEdit);
    layout->addWidget(listWidget);

    connect(lineEdit, &QLineEdit::textChanged,
            this, &TextFilterDialog::onTextChanged);


    connect(listWidget, &QListWidget::itemClicked, this, [=](QListWidgetItem *item) {
        int id = item->data(Qt::UserRole).toInt();
        if (id > 0) {
//            QMessageBox::warning(nullptr, "", QString("%1").arg(id));
            result = id;
            accept();
        }
        else {
            lineEdit->setText(item->text());
        }
    });
}



//static std::map<QString, int, less_dom> map_dom;
//static std::map<QString, QString> map_str1;
//static std::map<QString, int> map_nazv;
//static std::set<QString> set_str;

bool init_find(const QString &p, QListWidget *listWidget);

void TextFilterDialog::onTextChanged(const QString &text)
{
    listWidget->clear();
    if (text == "") return;
    listWidget->clear();
    init_find(text, listWidget);
}

QStringList TextFilterDialog::getMatchingItems(const QString &text)
{
    // Пример данных, замените на свою логику
    QStringList allItems = {"apple", "apricot", "banana", "blueberry", "avocado", "grape"};
    QStringList result;

    if (text == "") {
        return result;
    }


    for (const QString &item : allItems) {
        if (item.contains(text, Qt::CaseInsensitive)) {
            result << item;
        }
    }
    return result;
}
