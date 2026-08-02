#pragma once


CString toUTF8(const CString &qq);

class JSON
{
public :
  JSON()
  {
    _str = "";
    zpt = 0;
  };

  void add_nobr(const char *key, const char *val1);
  void add(const char *key, const char *val);
  void add(const char *key, double val);
  void add(const char *key, int val);
  void add(const char *key, bool val);
  void add1(const char *val);
  void add1(double val);
  void add(const char *key);
  void error(bool result = true, const char *msg = "");
  CString str() {return toUTF8(_str);};

private :
  CString _str;
  int zpt;
};
