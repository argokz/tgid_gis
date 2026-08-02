#include <QCoreApplication>
#include <QSqlQuery>

#include <string.h>

#include <db/db.h>


struct CUser
{
  int id;
  QString password;
  int user_right;
};

static QString tn_password = "passwords";

int md5hash(const char *, size_t len, char *hash) ;

bool addUser(QSqlDatabase &db, const QString & user, const QString & password, int right)
{
    QSqlQuery query(db);
    char hash[265];
    md5hash(password.toLocal8Bit(),  password.toLocal8Bit().length(), hash);

    QString q = QString("INSERT INTO %1 (user_name,user_password, user_right) VALUES ('%2','%3',%4)").arg(tn_password, user).arg(hash).arg(right);
    query_exec(db, query, q);
    return true;
}

bool loadUsers(QSqlDatabase &db, std::map <QString, CUser>& list_user)
{
    CUser psw;
    QSqlQuery query(db);

    QString q = QString("SELECT * FROM %1").arg(tn_password);
    if (query_exec(db, query, q)) {
        while (query.next()) {
            psw.id = query.value("id").toInt();
            psw.user_right = query.value("user_right").toInt();

            QString user = query.value("user_name").toString();
            psw.password = query.value("user_password").toString();

    //        char hash[265];
    //        md5hash(psw.password, psw.password.GetLength(), hash);

            list_user[user] = psw;
          }
    }

    return true;
}

bool delUser(QSqlDatabase &db, const QString & user)
{
  QSqlQuery query(db);
  QString q = QString("Delete From %1 Where user_name=\'%2\'").arg(tn_password, user);
  query_exec(db, query, q);

  return true;
}

bool updateUserRight(QSqlDatabase &db, const QString & user, int right)
{
  QSqlQuery query(db);
  QString q = QString("UPDATE %1 SET user_right=%2 WHERE user_name='%3'").arg(tn_password).arg(right).arg(user);
  query_exec(db, query, q);

  return true;
}

bool updateUserPassword(QSqlDatabase &db, const QString & user, const QString & password)
{
    char hash[265];
    md5hash(password.toLocal8Bit(), password.toLocal8Bit().length(), hash);

    QString q = QString("UPDATE %1 SET user_password='%2' WHERE user_name='%3'").arg(tn_password, hash, user);
    QSqlQuery query(db);
    query_exec(db, query, q);

    return true;
}

bool updateUserPassword(QSqlDatabase &db, int id, const QString & password)
{
    char hash[265];
    md5hash(password.toLocal8Bit(), password.toLocal8Bit().length(), hash);

    QString q = QString("UPDATE %1 SET user_password='%2' WHERE id=%3").arg(tn_password, hash).arg(id);
    QSqlQuery query(db);
    query_exec(db, query, q);

    return true;
}

