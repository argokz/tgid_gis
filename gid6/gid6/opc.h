#pragma once

#include "opc/opcda.h"
#include "opc/opcdx.h"
#include "opc/opchda.h"
#include "opc/OpcComn.h"
#include "opc/OpcEnum.h"

void LocalInit(void);
void LocalInit2(void);
void LocalCleanup(void);
void SetComputer(const char *computer, const char *domain, const char *name, const char *pwd);
bool GetServer(CLSID &clsid, CString &str);
void MachineIP(char* result);

class OPCItem;
class OPCGroup;


class OPCError
{
  public:
  OPCError(const char *s, int code);

  static char error_txt[256];
  static int error_code;
};


class OPCServer
{
friend OPCGroup;  
public:
  OPCServer(const CLSID & clsid);
  ~OPCServer();
  void Menu2(void *tree);
  CArray<CString,CString> & getList() {return lst;}
  CArray<CString,CString> & getList2() {return lst2;};
  map <HTREEITEM, int> &get_map() {return map_hn;};

private:
  void write_tree(CTreeCtrl *tree, HTREEITEM hParent);
  IOPCServer *gpOPCS;
  IOPCBrowseServerAddressSpace *gpOPCBA;
  map <HTREEITEM, int> map_hn;
  CArray<CString,CString> lst;
  CArray<CString,CString> lst2;
};

  
class OPCGroup 
{
friend OPCItem;  
public:
  OPCGroup(OPCServer *server);
  ~OPCGroup();
  void AddItem();
  void RemoveItem();

private :
  IOPCServer * pOPC;

  IUnknown *pGRP1U;
  IOPCGroupStateMgt * pGRP1GSM;
  IOPCPublicGroupStateMgt * pGRP1PGSM;
  IOPCSyncIO        * pGRP1SIO;
  IOPCAsyncIO       * pGRP1ASIO;
  IOPCItemMgt       * pGRP1IM;
  IDataObject       * pGRP1DO;
  DWORD hServerGroup1;
};

class OPCItem
{
public:
  OPCItem(OPCGroup *group, const char *fn, double a);
  OPCItem(OPCGroup *group, const CArray<CString,CString> & fn);
  OPCItem(OPCGroup *group, int n, const char **fn);
  ~OPCItem();
  CString ReadVar(const char *fn);
  void Read(char *buf);
  COleVariant Read();
  void Read(CArray<CString,CString> & fn); 
  void Read(int n, int *nn, CArray<CString,CString> & fn);
  void Write(COleVariant &var);
private:
  IOPCItemMgt * pIM;
  OPCGroup *gr;
  OPCHANDLE *g_sh;
  int *otn;
  int n_item;
  int n_item0;
  double _a;
};
