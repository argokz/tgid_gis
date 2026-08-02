#pragma once

class CGraph2;

#include "virtualdata.h"

class CNodeListData : public CVirtListData
{
public :
  CNodeListData(CGraph2 *graph, bool otris, bool vyd_potr = false);
  ~CNodeListData();

  
  
  virtual int GetNFlds();
  virtual CString GetFieldName(int i);
  virtual int GetFieldWidth(int i);

  virtual int GetCount();
  virtual CString GetItemText(int nSubItem, int nIndex, int first, int last);
  virtual int GetImage(int nIndex);
  virtual int GetStateImage(int nIndex);
  virtual void *getData(int row);
  virtual VIRT_DATA getType() {return virt_data_node;};
  virtual void setFindText(const char *text);

protected:
  void init(const char *text);


private:
  CGraph2 *m_graph;
  vector<CNode2*> m_v;
  bool m_otris;
  bool m_vyd_potr;
    
};

