#include <QCoreApplication>

#include "json.h"

bool isF(QChar c)
{
  return c != '.' && c != '-' && (c < '0' || (c > '9' && c < 'A') || c == '\\');
}


void JSON::add_nobr(QString key, QString val)
{
  QString  s = QString("\"%1\": %2").arg(key, val);
 
  if (zpt) s = QString (",")+s;
  zpt = 1;

  _str += s;
}

void JSON::add(QString key, QString val1)
{
    QString  val = val1;
    QString  val2 = "";

    for (int i = 0; i < val.length(); i++) {
        QChar c = val[i];
        QString s = "";
        if (isF(c)) {
            s = QString("\\u%1").arg(c.digitValue(), 4, 16);
        }
        else {
            s += c;
        }
        val2 += s;
    }

    val = QString("\"%1\"").arg(val2);

    add_nobr(key, val);
}



void JSON::add(QString key1)
{
    QString  key = key1;
  QString  s = key;

  int zpt_old = zpt;

  if (key.indexOf("{") != -1) {
    zpt = 0;
  }
  else if (key.indexOf("}") != -1) {
    zpt = 1;
  }
  else if (key.indexOf("[") != -1) {
    zpt = 0;
  }
  else if (key.indexOf("]") != -1) {
    zpt = 1;
  }

  if (!zpt && zpt_old) s = QString (",")+s;
  _str += s;
}

void JSON::add1(double val)
{
  QString  s = QString("\"%1\"").arg(val);

  if (zpt) s = QString (",")+s;
  zpt = 1;
  _str += s;
}

void JSON::add1(QString val1)
{
  QString  s;
  QString  val = val1;
  QString  val2 = "";

  for (int i = 0; i < val.length(); i++) {
    QChar c = val[i];
    QString  s = "";
    if (isF(c)) {
        char16_t ii = c.unicode();
        s = QString("\\u%1").arg(static_cast<quint16>(ii), 4, 16, QChar('0')).toUpper();

//        s = QString("\\u%1").arg(ii, 4, 16, QChar(u'0'));
    }
    else {
        s += c;
    }
    val2 += s;
  }

  s = QString("\"%1\"").arg(val2);

  if (zpt) s = QString (",")+s;
  zpt = 1;
  _str += s;
}


void JSON::add(QString key, int val)
{
    QString  s = QString("%1").arg(val);
    add_nobr(key, s);
}

void JSON::add(QString key, double val)
{
    QString  s = QString("%1").arg(val);
    add_nobr(key, s);
}

void JSON::add(QString key, bool val)
{
  QString  s = "false";
  if (val) s = "true";
  add_nobr(key, s);
}

void JSON::error(bool result, QString msg)
{
  add("{");
  add((const QString &) "result", result);
  if (!result) add((const QString &) "message", (const QString &) msg);
  add("}");
}

/*
HRESULT __fastcall AnsiToUnicode(LPCSTR pszA, LPOLESTR* ppszW);
char *write_utf8(unsigned int code_point, char *buf);

*/
QString  toUTF8(const QString  &qq)
{
    return qq;
/*
  WCHAR *szStr;

  int len = qq.GetLength();

  try {

      char *buf = new char[len * 2 + 1];
      char *buf1 = buf;

      AnsiToUnicode(qq, &szStr);

      for (int i = 0; szStr[i]; i++) {
          buf = write_utf8(szStr[i], buf);
      }
      *buf = 0;

      CoTaskMemFree(szStr);

      QString  s = buf1;
      delete[] buf1;
      return s;
  }
  catch (...) {
      QString  str;
      str.Format("%d", len);
      AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
      return "";
  }
*/
}


