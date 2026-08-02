#pragma once


QString toUTF8(const QString &qq);

class JSON
{
public :
  JSON()
  {
    _str = "";
    zpt = 0;
  };

  void add_nobr(QString key, QString val1);
  void add(QString key, QString val);
  void add(QString key, double val);
  void add(QString key, int val);
  void add(QString key, bool val);
  void add1(QString val);
  void add1(double val);
  void add(QString key);
  void error(bool result = true, QString msg = "");
  QString str() {return toUTF8(_str);};

private :
  QString _str;
  int zpt;
};
