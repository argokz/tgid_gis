#include <QApplication>
#include <QtSql>

    #include <db/db.h>

bool createBackup(QSqlDatabase &db, const QString & name, const QString & save_dir)
{
    if (save_dir != "") {
        QString q = QString(
            "BACKUP DATABASE [%1]\n"
            "TO DISK = N'%2' WITH\n"
            "    NOFORMAT\n"
            ",     NOINIT\n"
            ",     NAME = N'%3'\n"
            ",     SKIP\n"
            ",     REWIND\n"
            ",     NOUNLOAD\n"
            ",     STATS = 10").arg(name, save_dir, name);

//        CInputDialog inD(AfxGetMainWnd(), "", _TR("Запрос"), q);
//        if (inD.DoModal() != IDOK) return false;

//        AfxMessageBox(q, MB_OK|MB_ICONINFORMATION);

        QSqlQuery query(db);
        return query_exec(db, query, q);
    }

    return false;
}

bool restoreBackup(QSqlDatabase &db, const QString & name_old, const QString & name_new, const QString & save_dir)
{
    QString dir = "";

    static QRegularExpression re("(.+)\\\\MSSQL\\\\Backup\\\\.+");
    QRegularExpressionMatch match = re.match(save_dir);
    if (match.hasMatch()) {
        dir = match.captured(1);
    }

    QString sqlCommand = QString(
        "USE [master]\n"
        "RESTORE DATABASE [%1]\n"
        "FROM DISK = N'%2' WITH FILE = 1\n"
        ", MOVE N'%3' TO N'%4\\MSSQL\\DATA\\%5.mdf'\n"
        ", MOVE N'%6_log' TO N'%7\\MSSQL\\DATA\\%8_log.ldf'\n"
        ", NOUNLOAD\n"
        ", STATS = 5").arg(name_new, 
        save_dir,
        name_old, dir, name_new,
        name_old, dir, name_new
        );

//    CInputDialog inD(AfxGetMainWnd(), "", _TR("Не удалось восстановить базу данны их файла. Запрос :"), sqlCommand);
//    if (inD.DoModal() != IDOK) return false;

    QSqlQuery query(db);
    return query_exec(db, query, sqlCommand);
}

