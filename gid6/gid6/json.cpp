#include "stdafx.h"

#include "json.h"

bool isF(unsigned char c)
{
  return c != '.' && c != '-' && (c < '0' || (c > '9' && c < 'A') || c == '\\');
}


void JSON::add_nobr(const char *key, const char *val)
{
  CString s;

  s.Format("\"%s\": %s", key, val);
 
  if (zpt) s = CString(",")+s;
  zpt = 1;

  _str += s;
}

void JSON::add(const char *key, const char *val1)
{
    CString val = val1;
    CString val2 = "";

    for (int i = 0; i < val.GetLength(); i++) {
        unsigned char c = val[i];
        CString s = "";
        if (isF(c)) {
            s.Format("\\u%04x", c);
        }
        else {
            s += c;
        }
        val2 += s;
    }

    val.Format("\"%s\"", val2);

    add_nobr(key, (const char*)val);
}



void JSON::add(const char *key1)
{
    CString key = key1;
  CString s = key;

  int zpt_old = zpt;

  if (key.Find("{") != -1) {
    zpt = 0;
  }
  else if (key.Find("}") != -1) {
    zpt = 1;
  }
  else if (key.Find("[") != -1) {
    zpt = 0;
  }
  else if (key.Find("]") != -1) {
    zpt = 1;
  }

  if (!zpt && zpt_old) s = CString(",")+s;
  _str += s;
}

void JSON::add1(double val)
{
  CString s;

  s.Format("\"%f\"", val);

  if (zpt) s = CString(",")+s;
  zpt = 1;
  _str += s;
}

void JSON::add1(const char *val1)
{
  CString s;
  CString val = val1;
  CString val2 = "";

  for (int i = 0; i < val.GetLength(); i++) {
    unsigned char c = val[i];
    CString s = "";
    if (isF(c)) {
      s.Format("\\u%04x", c);
    }
    else {
      s += c;
    }
    val2 += s;
  }

  s.Format("\"%s\"",val2);

  if (zpt) s = CString(",")+s;
  zpt = 1;
  _str += s;
}


void JSON::add(const char *key, int val)
{
  CString s;
  s.Format("%d", val);
  add_nobr(key, s);
}

void JSON::add(const char *key, double val)
{
  CString s;
  s.Format("%f", val);
  add_nobr(key, s);
}

void JSON::add(const char *key, bool val)
{
  CString s = "false";
  if (val) s = "true";
  add_nobr(key, s);
}

void JSON::error(bool result, const char *msg)
{
  add("{");
  add((CString) "result", result);
  if (!result) add((CString) "message", (CString) msg);
  add("}");
}

HRESULT __fastcall AnsiToUnicode(LPCSTR pszA, LPOLESTR* ppszW);
char *write_utf8(unsigned int code_point, char *buf);

CString toUTF8(const CString &qq)
{
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

      CString s = buf1;
      delete[] buf1;
      return s;
  }
  catch (...) {
      CString str;
      str.Format("%d", len);
      AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
      return "";
  }

}


