#include <QtGui>
#include <QDockWidget>
#include <QProcess>
#include <QTextEdit>
#include <QMessageBox>
#include "shell.h"


void Shell::hideEvent ( QHideEvent * event )
{
//    QMessageBox::information(this, "", "Спрятал");
    m_process->kill();
}

void Shell::onFinished(int a, QProcess::ExitStatus b)
{
//    QMessageBox::information(this, "", QString("%1 %2").arg(a).arg(b));
}


Shell::Shell(QMainWindow *pwgt)
    : QDockWidget(tr("Протокол"), (QWidget*)pwgt)
{
    m_ptxtDisplay = new ShellEdit(this);

    this->setObjectName(tr("Протокол"));

    m_ptxtDisplay->setReadOnly(true);
//    m_ptxtDisplay->setFontFamily("");
    QFont f("unexistent");
    f.setStyleHint(QFont::Monospace);
    m_ptxtDisplay->setFont(f);

    m_process = new QProcess(this);

    connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(slotDataOnStdout()));
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onFinished(int, QProcess::ExitStatus)));

    setWidget(m_ptxtDisplay);
}

QString argpath();

//void Shell::start(QString strCommand, QWidget *widget)
void Shell::start(QString strCommand, QWidget *widget, const char *slot_name)
{
#ifdef Q_WS_WIN
    strCommand = "cmd /C "+strCommand;
#else
//    strCommand = "sh -c "+strCommand;
#endif

//    strCommand.replace("\"", "\"\"\"");
//    strCommand = "cmd /C dir";
//   strCommand = "cmd /C "+strCommand;
//    m_process->setWorkingDirectory(argpath());

    m_ptxtDisplay->setGid(widget);

    m_ptxtDisplay->clear();

    disconnect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), nullptr, nullptr);

    if (slot_name) {
        connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), widget, slot_name);
    }

    qDebug() << strCommand;

    m_process->startCommand(strCommand);
}

void cp866_to_win1251(char *s)
{
    for (signed char *c = (signed char *)s; *c; c++) {
        if( *c>-129 && *c<-80 )
            *c += 64;
        else if( *c>-33 && *c<-16 )
            *c += 16;
    }
}

/*
QString escapeHtmlText(const QString& html) {
    QString result = html;

    // Регулярка: ищем < и >, которые НЕ внутри тегов (<...>)
    // Используем негативный lookbehind и lookahead для исключения тегов
    QRegularExpression re("(?<!<[^>]*)<(?![^>]*>)|(?<!<[^>]*)>(?![^>]*>)");
    result.replace(re, [](const QRegularExpressionMatch &match) {
        return match.captured() == "<" ? "&lt;" : "&gt;";
    });

    return result;
}

*/



#if 0

QString escapeHtmlText(const QString& html) {
    QString result = html;

#if 0
    // Регулярки для <, > и пробела вне тегов
    QRegularExpression reLt("(?<!<[^>]*)<(?![^>]*>)");   // Для <
    QRegularExpression reGt("(?<!<[^>]*)>(?![^>]*>)");   // Для >
    QRegularExpression reSpace("(?<!<[^>]*) (?![^>]*>)"); // Для пробела

    // Последовательные замены
    result = result.replace(reLt, "&lt;");
    result = result.replace(reGt, "&gt;");
    result = result.replace(reSpace, "&nbsp;");
#endif

    // Регулярки для <, > и пробела вне тегов
    QRegularExpression reLt("(?<!<[^>]*)<(?![^>]*>)");   // Для <
    QRegularExpression reGt("(?<!<[^>]*)>(?![^>]*>)");   // Для >
    // Улучшенная регулярка для пробелов: любой пробел в текстовом содержимом
    QRegularExpression reSpace("(?<=<[^>]+>)\\s+(?=(?:[^<]*</[^>]+>)?)");

    // Последовательные замены
    result = result.replace(reLt, "&lt;");
    result = result.replace(reGt, "&gt;");
    result = result.replace(reSpace, "&nbsp;");

    return result;
}

#endif


QString escapeHtmlText0(const QString &html)
{
    QString result = html;

    // Регулярка: всё, что между ">" и "<" (т.е. текст узлов)
    QRegularExpression re(">([^<]+)<");

    QRegularExpressionMatchIterator it = re.globalMatch(result);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        QString text = m.captured(1);

        // Экранируем спецсимволы
        QString escaped = text;
        escaped.replace("&", "&amp;");   // сначала & !
        escaped.replace("<", "&lt;");
        escaped.replace(">", "&gt;");
        escaped.replace(" ", "&nbsp;");

        // Заменяем в строке
        result.replace(m.capturedStart(1), text.length(), escaped);
    }

    return result;
}

QString escapeHtmlText(const QString &src, bool convertSpaces = false)
{
    QString out;
    out.reserve(src.size() * 2);

    const int n = src.size();
    for (int i = 0; i < n; ) {
        QChar c = src.at(i);

        // --- Tag: copy and optionally transform <b>/<i>/<u> to <span style=...>
        if (c == '<') {
            int j = src.indexOf('>', i + 1);
            if (j == -1) {
                // несбалансированный '<' — экранируем
                out += "&lt;";
                ++i;
                continue;
            }

            QString tag = src.mid(i, j - i + 1);           // включая < и >
            QString inner = tag.mid(1, tag.size() - 2);    // внутри тега
            inner = inner.trimmed();

            bool closing = inner.startsWith('/');
            QString name;
            QString attrs;

            if (closing) {
                QString rest = inner.mid(1).trimmed();
                name = rest.section(QRegularExpression("\\s+"), 0, 0).toLower();
            } else {
                QRegularExpression re("^([A-Za-z0-9]+)\\b(.*)$");
                auto m = re.match(inner);
                if (m.hasMatch()) {
                    name = m.captured(1).toLower();
                    attrs = m.captured(2); // включает ведущие пробелы, если есть
                }
            }

            if (!name.isEmpty() && (name == "b" || name == "i" || name == "u")) {
                if (closing) {
                    out += "</span>";
                } else {
                    QString style;
                    if (name == "b") style = "font-weight:bold;";
                    else if (name == "i") style = "font-style:italic;";
                    else /* u */       style = "text-decoration:underline;";

                    out += "<span style=\"" + style + "\"";
                    if (!attrs.isEmpty()) out += attrs; // оставляем другие атрибуты
                    out += ">";
                }
            } else {
                // обычный тег — копируем как есть
                out += tag;
            }

            i = j + 1;
            continue;
        }

        // --- Сущности: если встретили '&', попробуем скопировать валидную сущность &name;
        if (c == '&') {
            int sem = src.indexOf(';', i + 1);
            bool copiedEntity = false;
            if (sem != -1 && sem - i - 1 <= 16) { // разумная длина сущности
                QString ent = src.mid(i + 1, sem - i - 1);
                // простая проверка: только буквы/цифры/# — значит это сущность
                if (QRegularExpression("^[A-Za-z0-9#]+$").match(ent).hasMatch()) {
                    out += src.mid(i, sem - i + 1);
                    i = sem + 1;
                    copiedEntity = true;
                }
            }
            if (!copiedEntity) {
                out += "&amp;";
                ++i;
            }
            continue;
        }

        // --- Обычный текстовый символ — экранируем специальные символы
        if (c == '<') { out += "&lt;"; ++i; }
        else if (c == '>') { out += "&gt;"; ++i; }
        else if (c == '"') { out += "&quot;"; ++i; }
        else if (c == '\'') { out += "&#39;"; ++i; }
        else if (c == ' ') {
            if (convertSpaces) out += "&nbsp;";
            else out += ' ';
            ++i;
        } else {
            out += c;
            ++i;
        }
    } // for

    return out;
}




void Shell::slotDataOnStdout()
{
//    QTextCodec::codecForName("CP-866");
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("cp-866"));

//    auto toUtf16 = QStringDecoder(QStringDecoder:: Utf8);
    QByteArray a = m_process->readAllStandardOutput();

//    cp866_to_win1251(a.data());

    QString str = QString::fromLocal8Bit(a);
 //   QMessageBox::information(this, "", str);

//    m_ptxtDisplay->appendPlainText(str);
//    qDebug() << "===================================";
//    qDebug() << "===================================";
//    qDebug() << "===================================";

    str.replace("\r", "");
    str.replace("\r\n", "</pre><pre>");

    str = "<pre>"+ str + "</pre>";

//    qDebug() << str;

//    qDebug() << "===================================";

    
    str.replace("< ", "&lt; ");
    str.replace(" >", " &gt;");
    str.replace("<>", "&lt;&gt;");
    str.replace("<=", "&lt;=");
    str.replace(">=", "&gt;=");

//    qDebug() << str;

//    qDebug() << "===================================";



//    qDebug() << "===================================";

//    qDebug() << str;


//    str.replace("\r\n", "</pre><pre>");
    str.replace("\n", "</pre><pre>");

    str = escapeHtmlText(str, false);


    qDebug() << str;


//    str = "<p>"+str;
//    str.replace("</p>", "</p>\n");
//    m_ptxtDisplay->appendPlainText(str);
    m_ptxtDisplay->appendHtml(str);
}

void Shell::mouseDoubleClickEvent(QMouseEvent *event)
{
    QMessageBox::warning(this, "", "!");
    
    QDockWidget::mouseDoubleClickEvent(event);
}
