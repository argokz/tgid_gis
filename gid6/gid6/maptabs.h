#pragma once

struct MFIELDS 
{
  map<CString, CString> map1;
  map<CString, CString> map_q;
  list<pair<CString, CString> > list2;
  list<pair<CString, CString> > list3;
};


class MapTabs 
{
public:
  MapTabs();

  void Init(const char *mdb, const char *m_tn, bool isADO);
  void Init(const char *mdb, const char *m_tn, map<CString, CString> & map_val, bool isADO);
  bool getMap(const char *tn, const char *fn, MFIELDS &map1);

private:                                        
//  map < CString, map <CString, map<CString, CString> > > map_tabs;
  map < CString, map <CString, MFIELDS> > map_tabs;
};

void InitTab(const char *mdb, const char *m_tn, map<CString, CString> & map_val, bool isADO);
void InitTab(const char *mdb, const char *m_tn, bool isADO);
bool getMap(const char *tn, const char *fn, MFIELDS &map1);
CString getReal(const char *tn, const char *fn, const char *val);
CString getReal(const char *tn, const char *fn, int val);

