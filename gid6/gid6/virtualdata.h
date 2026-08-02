#pragma once

#include "Ultimate Grid/virt_data.h"


class CVirtListData
{
public :
  virtual int GetNFlds();
  virtual CString GetFieldName(int i);
  virtual CString GetFieldTitle(int i) { return GetFieldName(i); };
  virtual CString GetFieldF1(int i) { return GetFieldName(i); };
  virtual int GetFieldWidth(int i);
  virtual int GetId2(int id) {return -1;};

  virtual int GetCount();
  virtual CString GetItemText(int nSubItem, int nIndex, int first, int last);
  virtual int GetFormat(int nSubItem) { return LVCFMT_LEFT; };
  virtual int GetImage(int nIndex);
  virtual int GetStateImage(int nIndex);
  virtual void *getData(int row);
  virtual CString getTable(int row) { return ""; };
  virtual VIRT_DATA getType() {return virt_data_unknown;};
  virtual void setFindText(const char *text) {};
  virtual int SortBy(int *cols,int num,int flags) { return 0; };
};
