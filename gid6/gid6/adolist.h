#if !defined(__adolist_h)              // Sentry, use file only if it's not already included.
#define __adolist_h

class CGraph2;
class CAdoFile;
struct CAdoField;



class CAdoListData : public CVirtListData
{
public :
  CAdoListData();
  ~CAdoListData();

  bool initQ(CAdoFile *ado, const char *q);
  int initQpat(CAdoFile *ado, const char *q);
  bool initQsort(CAdoFile *ado, const char *q, int *cols,int num,int flags);
  bool initNode(CGraph2 *graph, CAdoFile *ado, const char *q, int typ);
  bool initLine(CGraph2 *graph, CAdoFile *ado, const char *q, int typ);

  bool initNodeOut(CGraph2 *graph, CAdoFile *ado, const char *tn, int typ);
  bool initLineOut(CGraph2 *graph, CAdoFile *ado, const char *tn, int typ);

  void setIdVisible(bool vis) {m_id_visible = vis;}
  void setAfterIdVisible(bool vis) { m_after_id_visible = vis; }

  void setGeo(const char *tn);

  virtual int GetNFlds();
  virtual CString GetFieldName(int i);
  virtual CString GetFieldTitle(int i);
  virtual CString GetFieldF1(int i);
  virtual int GetFieldWidth(int i);

  virtual int GetCount();
  virtual CString GetItemText(int nSubItem, int nIndex, int first, int last);
  virtual int GetFormat(int nSubItem);
  virtual int GetImage(int nIndex);
  virtual int GetStateImage(int nIndex);
  virtual void *getData(int row);

  virtual bool SetItem(int id);


  virtual void setType(VIRT_DATA typ) { m_typ = typ;};
  
  virtual VIRT_DATA getType() {return m_typ;};
  virtual CString getTable(int row) { return m_tn; };
  virtual int SortBy(int *cols,int num,int flags);
  virtual int GetId2(int id) {return get_id_obr(id);};
  virtual void setFindText(const char *text);


private:
  CAdoFile *m_ado;
  int m_nFlds;

  CAdoField *m_adoField;
  CString m_q0;
  CString m_q;
  CString m_q_pat;

  CString m_tn;

  bool m_id_visible;

  /*גטהטלמסעü ןונגמדמ סעמכבצא ןמסכו id*/
  bool m_after_id_visible;

  int get_id_obr(int i);
  int get_id(int i);
  bool initFlds(const char *q);

  map<long, vector<CString> > map_val;
  map<long, long> map_id;
  map<long, long> map_id_obr;
  VIRT_DATA m_typ;
};



#endif