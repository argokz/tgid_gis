#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "ShellEdit.h"

#include <gidview/GidWidget.h>

ShellEdit::ShellEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{
}


void ShellEdit::OnMoveTo()
{
}


#include <QRegularExpression>


QString getBrText(QString txt)
{
    static QRegularExpression re(".*?\\[([^\\[]+?)\\]");
    
    QRegularExpressionMatch match = re.match(txt);
    if (match.hasMatch()) {
        QString s = match.captured(1);
        return s;
    }
    return "";
}

void split_name(QString txt, QString &txt1, QString &txt2)
{
    txt1 = "";
    txt2 = txt;


    static QRegularExpression re("^\\((.+?)\\)\\s+(.+)");
    
    QRegularExpressionMatch match = re.match(txt);
    if (match.hasMatch()) {
        txt1 = match.captured(1);
        txt2 = match.captured(2);
    }
}


void ShellEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QTextCursor cursor = cursorForPosition(event->pos());

        int nn = cursor.columnNumber();

        cursor.select(QTextCursor::LineUnderCursor);

        QString txt = getBrText(cursor.selectedText());


        qDebug() << txt;

//        txt.replace("\\u0160", " " );
        txt.replace(QChar(0x00A0), QChar(' '));

        txt.replace("&lt;",   "<" );
        txt.replace("&gt;",   ">" );
        txt.replace("&nbsp;", " " );
        txt.replace("&amp;",  "&" ); 

        qDebug() << txt;


//        txt = getBrText(" [(М2 НС2) М2 НС2 П - М2 НС2_рег_вых П] не м");

        QString txt1 = "";
        QString txt2 = "";

        if (gid) {
//            txt = "ТЭЦ-21 (т/м Ново-Девяткино) ТЭЦ-21 (т/м Ново-Девяткино) П";

            split_name(txt, txt1, txt2);


            bool ret = gid->moveText(txt1, txt2);
            if (!ret) {
                ret = gid->moveText(txt1, txt2 + " " + S_POD);
            }
            if (!ret) {
                ret = gid->moveText(txt1, txt2 + " " + S_OBR);
            }
        }
    }

//    QPlainTextEdit::mouseDoubleClickEvent(event);
}


