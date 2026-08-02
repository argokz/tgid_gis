#pragma once

/////////////////////////////////////////////////////////////////////////////
// CSheme view

#include "CustomView.h"

class CSheme : public CCustomView
{ 
friend class CConsrtuctorDoc; //Дружественный класс документа -- для доступа к защищенным переменным
public:
  CSheme();           //Конструктор при создании в проекте
  virtual ~CSheme(); //Деструктор
  
  void CompileList();
  
  void SetTextToElem(PLACE place,CString newText);
  void SetTextToElem(const bool show,const long UserNumber,const char * UserName,const char * newText);

  
  ///Action //////////////////////////////////////////////////////////////
  void CallOpenGroup(const long group_num,bool open);
  void OpenObject(const bool open,const long UserNumber,const char * UserName);
  void ShowDiafrafma(const bool show,const long UserNumber,const char * UserName);
  void ShowDiafrafma(const bool show);
  void ShowZadvizhka(const bool show,const long UserNumber,const char * UserName);
  void SetFocusToElement(const long UserNumber,const char * UserName);
  
  BOOL Action(WPARAM wParam, LPARAM lParam);


// Attributes
public:

// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CSheme)
  protected:
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  //}}AFX_VIRTUAL

// Implementation
public:
  
  // Generated message map functions
protected:

  bool SendData(long param);
  void OnRunLClick(long param);
  

  template <class V> bool SetTextTo(V * vect,const bool show,const long UserNumber,const char * UserName,const char * newText);

    

/////////////////////////////////////////////////////////////////////////////
// Движение мыши
  bool MouseOnSwitch();
  template <class V> bool OnswObject(long type,V * vect);
  bool MouseOnObject(bool zero_focus_before=true);
  template <class V> bool OnObject(long type,V * vect);
  void SwitchObject(CPoint point);
// Движение мыши
/////////////////////////////////////////////////////////////////////////////



  

  
  template <class V> BOOL SetFocusTo(V * vect,const long UserNumber,const char * UserName);
  template <class V> BOOL OpenObjectTo(V * vect,const bool open,const long UserNumber,const char * UserName);
  template <class V> void CallOpenGroupIn(V * vect,const long group_num,const bool open);
  
  




  
  
  //////////////////////////////////////////
  //////////////////Анализ схемы////////////
  //////////////////////////////////////////
  //////////////////////////////////////////
  //CArchive * ar;
  template <class V> void DrawWhenCompileElem(V * vect);  
  /////////////// 
  //---------------XML-------------------------------------------------
//  void LoadXML(CArchive& ar);
//////////////////
  ELEMENT_DATA objects[ELEM_KOL]; 
//////////////////
  //////////////////////////////////////////
  ////ADD///////////////////////////////////
public :
  CSheme(const char *FileName,RECT &Clrect); //Конструктор при создании отдельно от проекта
  void setSize(RECT &Clrect);


protected : 
  bool Open(const char *FileName);
  bool Close(const char *FileName);
  CString fileN;
  //////////////////////////////////////////
  
  //{{AFX_MSG(CSheme)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg LRESULT OnAction(WPARAM wParam, LPARAM lParam);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

