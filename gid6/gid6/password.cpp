#include "stdafx.h"
#include "gid6.h"

#include "ado.h"

#include "LoginDialog.h"

static CString tn = "passwords";

int md5hash(const char *msg, size_t len, char *hash) ;


bool addUser(const char *user, const char *password, int right)
{
  CAdoFile *ado = getAdo(getGidAdoName());
  if (ado) {
    CString q;
    long affected;

    char hash[265];
    md5hash(password, strlen(password), hash);

    q.Format("INSERT INTO %s (user_name,user_password, user_right) VALUES ('%s','%s',%d)", tn, user, hash, right);
    ado->ExecuteInsert(q, &affected );
  }

  return true;
}


bool loadUsers(map <CString, CUser>& list_user)
{
  CUser psw;

  CAdoFile *ado = getAdo(getGidAdoName());

  if (ado) {

    CString q;
    q.Format("SELECT * FROM %s", tn);

    if (ado->openTable0(q)) {

      while ( !ado->isEOF() ) {
        CString user = ado->readStr("user_name");
        psw.password = ado->readStr("user_password");
        psw.user_right = ado->read_long("user_right");
        psw.id = ado->read_long("id");

//        char hash[265];
//        md5hash(psw.password, psw.password.GetLength(), hash);

        list_user[user] = psw;
        ado->MoveNext();
      }
      ado->closeTable();
    }
  }

  return true;
}

bool delUser(const char *user)
{
  CAdoFile *ado = getAdo(getGidAdoName());
  if (ado) {
    CString q;
    long affected;

    q.Format("Delete From %s Where user_name=\'%s\'", tn, user);
    ado->Execute(q, &affected );
  }

  return true;
}


bool updateUserRight(const char *user, int right)
{
  CAdoFile *ado = getAdo(getGidAdoName());
  if (ado) {
    CString q;
    long affected;

    q.Format("UPDATE %s SET user_right=%d WHERE user_name='%s'", tn, right, user);
    ado->Execute(q, &affected );
  }

  return true;
}

bool updateUserPassword(const char *user, const char *password)
{
  CAdoFile *ado = getAdo(getGidAdoName());
  if (ado) {
    CString q;
    long affected;

    char hash[265];
    md5hash(password, strlen(password), hash);

//    q.Format("UPDATE %s SET password='%s' WHERE user='%s'", tn, password, user);
    q.Format("UPDATE %s SET user_password='%s' WHERE user_name='%s'", tn, hash, user);
    ado->Execute(q, &affected );
  }

  return true;
}

