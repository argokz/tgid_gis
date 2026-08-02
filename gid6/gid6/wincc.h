#ifndef __WINCC_H__
#define __WINCC_H__

class CWinCC {
public :
  CWinCC();
  ~CWinCC();

  bool open(const char *source, const char *catalogl);
  bool close();
  CString get_value(const char *val);
  CString get_value(time_t t, const char *val);
  bool get_value(time_t t, const list<CString> & listTag, list<CString> & listVal);
  bool is_error() { return m_error; };
  void no_error() { m_error = false; };


private:
  
  _ConnectionPtr m_pConn;
  bool isOpen;
  bool m_error;
  CString m_strError;

};

#endif
