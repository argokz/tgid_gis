#pragma once

class CVirtListData;

class CVirtData: public CUGDataSource {

protected:
  int FetchNewRows( long row );
  
  long    m_nFetchedStarting; // indicates relation between fetched
                  // rows and the grid rows.
  long    m_nMaxFetchRows;  // maximum rows allowed for fetching, calculated value
  long    m_nMinFetchRows;  // Long value used to define minimum number of records to
                  // fetch from the database in one fetch operation.
                  // This value can be changed with UG_ADO_DS_SET_FETCHSIZE
                  // option.
  long    m_pcRowsObtained; // number of rows fetched with the last fetch operation


public:
  CVirtData();
  ~CVirtData();

  bool init(CVirtListData *data);

  //cell info
  virtual int GetCell(int col,long row,CUGCell *cell);
  virtual int SetCell(int col,long row,CUGCell *cell);

  //options
//  virtual int SetOption(int option,long param1,long param2);
//  virtual int GetOption(int option,long param1,long param2);

  virtual long GetNumRows();
  virtual int GetNumCols();
  virtual int GetColName(int col,CString * string);
  virtual int GetColType(int col,int *type);  //0-string 1-bool 2-short 3-long 4-float 
                          //5-double 6-currency 7-data 8-time
                          //8-blob 9-graphic 10-ole
  virtual int OnHitBottom(long numrows,long rowspast,long *rowsfound);

  virtual int FindFirst(CString *string,int *col,long *row,long flags);
  virtual int FindNext(CString *string,int *col,long *row,int flags);

  virtual int SortBy(int col,int flags = UG_SORT_ASCENDING);
  virtual int SortBy(int *cols,int num,int flags);

  virtual int AppendRow();
  virtual int DeleteRow(long row);

  int AppendRow(CUGCell *cellList,int numCells);

  int StartTransaction();
  int CancelTransaction();
  int FinishTransaction();

  // editing notifications...
  virtual int OnEditStart(int col, long row,CWnd **edit);
  virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
  virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);
  virtual int OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow);

  // grid movement...
  virtual int OnCanMove(int oldcol,long oldrow,int newcol,long newrow);

  //new functions - not derived from the base class (CUGDatasource)
  long GetNumRowsComplete();
  BOOL IsOpen();
  BOOL CanEditCell(int col, long row);

  int GotoRecord(long row);
  int SetFieldValueFromCell(int col, CUGCell *cell);
  int UpdateCurrentRecord();


  void AllocateGhostRow();
  int  PopulateGhostRow();
  void ClearGhostRow();

  void AllocateEditRow();
  void ReadEditRow(long row);
  void ClearEditRow();
  int  UpdateFromEditRow();

  // temporary - override this to supply default vals when
  // using GhostRowMode
  virtual int AddDefaultRow();

  void CancelEdit();

  int Reset();

  // debug ...
  int CheckPosition();
  void VerifyPosition();

  CVirtListData *m_data;

};