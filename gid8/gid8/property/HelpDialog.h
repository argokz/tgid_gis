#pragma once

#include <QDialog>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <dialog/SaveGeometry.h>

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(const QString & title, const QString & text, QWidget *parent = nullptr) :
        QDialog(parent)
        , save(this, "HelpDialog", title)

    {
        QSettings settings;
        QString fontFamily = settings.value("property/fontFamily", "Arial").toString();
        int fontSize = settings.value("property/fontSize", 12).toInt();

        QString head = QString("<html><head><style>body { font-family: %1; font-size: %2px; }</style></head><body>\n").arg(fontFamily).arg(fontSize);


        auto *textBrowser = new QTextBrowser(this);
        textBrowser->setHtml(head + text + "</body></html>");

        textBrowser->setOpenExternalLinks(true); // Позволяет открывать внешние ссылки

        auto *layout = new QVBoxLayout(this);
        layout->addWidget(textBrowser);
        setLayout(layout);

        setWindowTitle(title);
//        resize(600, 400);
    }

private:
    SaveGeometry save;
};
