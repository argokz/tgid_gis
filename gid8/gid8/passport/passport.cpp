#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <gidview/GidWidget.h>

#include "db/db.h"
//#include "xlsxdocument.h"
//#include "xlsxworkbook.h"

#include "mainwindow.h"

QString python_exe();
bool is_POSTGRESQL();


QString str_python2(const QString prefix0, const QString command)
{
    QString dir = QCoreApplication::applicationDirPath();
    QString prefix = prefix0;
    QString path = "";
    
    for (int i = 0; i < 4; i++) {
        path = dir + prefix + command;

//        qDebug() << '?' << path;

        if (QFile::exists(path)) break;

        path = dir + prefix + command + "c";

        if (QFile::exists(path)) break;
        path = dir + prefix + command;

        if (QFile::exists(path)) break;

        prefix = "/.." + prefix;
    }
    
    qDebug() << prefix;
    qDebug() << path;
    return path;
}


QString GidWidget::str_python(const QString & command, const QString & args, bool conn) 
{
    QString rdbms = "MsSql";

    if (is_POSTGRESQL()) {
        rdbms = "postgreSQL";
    }

    CConnect cn = m_cxema.m_connect;

    QString connect_str = "";

    if (conn) {
        connect_str =  QString(" -rdbms %1 -server \"%2\" -database \"%3\" -user \"%4\" -port %5 ")
            .arg(rdbms, cn.host, cn.baza, cn.user).arg(cn.port);

        if (cn.password != "") {
            connect_str += QString(" -password \"%1\"").arg(cn.password);
        }
    }

    QString path = argpath() + command;
    if (!QFile::exists(path)) {
        
        if (!QFile::exists(path)) {
            path = str_python2("/python/", command);
        }
        if (!QFile::exists(path)) {
            path = str_python2("/python/docs/", command);
        }
    }

#if _WIN32
    return QString("%1 -u \"%2\" %3 %4").arg(python_exe(), path, connect_str, args);
#else
    return QString("%1 -u %2 %3 %4").arg(python_exe(), path, connect_str, args);
#endif
}

void GidWidget::onFinishedPassport(int exitCode, QProcess::ExitStatus exitStatus )
{
    if (exitStatus == QProcess::NormalExit) {
        QMessageBox::information(this, "", QString(tr("OK!")));
        QString path = QString("file:///%1").arg(passport_file_name);
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    }
    else {
        QMessageBox::information(this, "", QString(tr("При проведении расчета произошел сбой")));
    }
}


void GidWidget::Passport(CNode2* nl, CNode2* n1, int ms_rs, int id)
{
//    cmd = python_str("passport\\p.py", args);
//    QString path = QString("\"%1\" \"%2passport/p.py\"").arg(python_exe(), argpath());

    passport_file_name = QString("%1/tgid.xlsx").arg(QDir::tempPath());

    QString path = str_python("passport_ps/p.py",
        QString("-fragments %1 -out_file \"%2\" -type %3 -id %4")
                              .arg(m_cxema.m_par)
                              .arg(passport_file_name)
                              .arg(ms_rs == 1 ? "ms" : "rs")
                              .arg(id));

//    QInputDialog::getMultiLineText(this, "", "", path);

    main_window->start(path, this, SLOT(onFinishedPassport(int, QProcess::ExitStatus)));
}
