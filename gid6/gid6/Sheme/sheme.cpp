// sheme.cpp : implementation file
//

#include "stdafx.h"
#include "sheme.h"
//#include "../TeploCalc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSheme

//IMPLEMENT_DYNCREATE(CSheme, CCustomView)

/////////////////////////////////////////////////////////////////////////////
// CSheme drawing
CSheme::CSheme()
{
  dx=0;dy=0;
  origin=0;
  tempOrigin=0;
  listik=501;
}
//-----------------------------------------------------------------------------------
void CSheme::OnDraw(CDC* pDC)
{
//  wglMakeCurrent(pdc->m_hDC, hGLRC);
  display();
}
// CSheme при создании отдельно от проекта
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSheme, CCustomView)
  //{{AFX_MSG_MAP(CSheme)
  ON_WM_CREATE()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_MESSAGE(WM_MY_SHEME_ACTION,OnAction) 
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSheme message handlers
int CSheme::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CCustomView::OnCreate(lpCreateStruct) == -1)return -1;
/*  if (!NodesConnected())
  {
    if (AfxMessageBox("Ошибка при попытке построения схемы!\nЕсть неприсоединенный конец!\nЭта схема может работать некорректно!",MB_OKCANCEL)==IDCANCEL) return -1;
  }
*/  return 0; 
}
//-----------------------------------------------------------------------------------
void CSheme::CompileList()
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  glDeleteLists(listik,1);
  glClearColor(sc.BackColor.R/255.0,sc.BackColor.G/255.0,sc.BackColor.B/255.0,1);
  glColor3ub(sc.ForeColor.R,sc.ForeColor.G,sc.ForeColor.B);
  glNewList(listik ,GL_COMPILE);
  glNormal3d(0,0,1);
  GLdouble params[4],clparams[4];
  glGetDoublev(GL_CURRENT_COLOR,params);  
  GLfloat amb[4],ambient[4] = {0.0,0.0, 0.0, 1};
  glGetFloatv(GL_LIGHT_MODEL_AMBIENT,amb);    
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  glColor3ub(sc.BackColor2.R,sc.BackColor2.G,sc.BackColor2.B);  
  glNormal3d(0,0,1);
  glBegin(GL_QUADS);
  //вертикальный вылет
  glVertex3d(sc.XL2+dx,sc.YL2-dy,-0.1);
  glVertex3d(-sc.XL2+sc.Wrect.width+dx,sc.YL2-dy,-0.1);
  glVertex3d(-sc.XL2+sc.Wrect.width+dx,sc.YL2-sc.Wrect.height-dy,-0.1);
  glVertex3d(sc.XL2+dx,sc.YL2-sc.Wrect.height-dy,-0.1);
  //горизонтальный вылет
  glVertex3d(-sc.XL2,-sc.YL2,-0.1);
  glVertex3d(-sc.XL2+sc.Wrect.width+dx,-sc.YL2,-0.1);
  glVertex3d(-sc.XL2+sc.Wrect.width+dx,sc.YL2-sc.Wrect.height-dy,-0.1);
  glVertex3d(-sc.XL2,sc.YL2-sc.Wrect.height-dy,-0.1);
  glEnd();
  glColor3dv(params);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
  DrawWhenCompileElem(&m_pDoc->tubeV);
  DrawWhenCompileElem(&m_pDoc->batV);
  DrawWhenCompileElem(&m_pDoc->turnV);
  DrawWhenCompileElem(&m_pDoc->triV);
  DrawWhenCompileElem(&m_pDoc->ventV);
  DrawWhenCompileElem(&m_pDoc->nasV);
  DrawWhenCompileElem(&m_pDoc->drosV);
  DrawWhenCompileElem(&m_pDoc->airbatV);
  DrawWhenCompileElem(&m_pDoc->vodovodoV);
  DrawWhenCompileElem(&m_pDoc->soploV);
  DrawWhenCompileElem(&m_pDoc->connV);
  DrawWhenCompileElem(&m_pDoc->kranV);
  DrawWhenCompileElem(&m_pDoc->labV);
  DrawWhenCompileElem(&m_pDoc->extermV);  
//  DrawWhenCompileElem(&m_pDoc->sopObjV);
  glEndList(); 
}
//-----------------------------------------------------------------------------------
template <class V>
void CSheme::DrawWhenCompileElem(V * vect)
{
  V::iterator itr;
  for(itr=vect->begin();itr!=vect->end();itr++)itr->Draw(false);
}
//-----------------------------------------------------------------------------------
void CSheme::OnMouseMove(UINT nFlags, CPoint point) 
{
  m.X=getGLX(point.x);
  m.Y=getGLY(point.y);
  switch (DebugMode)
  {
  case 0:
    {
      MoveDrag(point);
      if (!MouseOnSwitch())MouseOnObject(); 
      break;
    }
  case 1:
    {OnZoomInMouseMove(point);break;}
  }
  CCustomView::OnMouseMove(nFlags, point);
}
//-----------------------------------------------------------------------------------
bool CSheme::MouseOnSwitch()
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return false;
  type_selected=NONE;
  if (OnswObject(VENTIL,&m_pDoc->ventV))return 1;
  if (OnswObject(NASOS,&m_pDoc->nasV))return 1;
  if (OnswObject(AIRBATTERY,&m_pDoc->airbatV))return 1;
  if (OnswObject(KRAN,&m_pDoc->kranV))return 1;
  ::SetClassLong(this->m_hWnd,GCL_HCURSOR,(long)old);
  return 0;
}
//-----------------------------------------------------------------------------------
template <class V>
bool CSheme::OnswObject(long type,V * vect)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();

  V::iterator loop;
  long i=0;
  for (loop=vect->begin();loop!=vect->end();loop++)
  {
    COMMON_DATA cp=loop->GetCommonParam();
    if (cp.UserNumber!=0)
    {
      if (loop->PointInClickRgn(&m))
      {
        type_selected=type;
        duo_selected.place.Type=type;
        duo_selected.place.Number=i;
        ::SetClassLong(this->m_hWnd,GCL_HCURSOR,(long)cr);
        return 1;
      }
    }
    i++;
  }
  return 0;
}
//-----------------------------------------------------------------------------------

bool CSheme::MouseOnObject(bool zero_focus_before)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return false;
  type_selected=NONE;
  if (zero_focus_before) m_pDoc->SetZeroFocusAll(); 
  if (OnObject(TUBE,&m_pDoc->tubeV))return 1;
  if (OnObject(BATTERY,&m_pDoc->batV))return 1;
  if (OnObject(TURN,&m_pDoc->turnV))return 1;
  if (OnObject(TRINITY,&m_pDoc->triV))return 1;
  if (OnObject(VENTIL,&m_pDoc->ventV))return 1;
  if (OnObject(NASOS,&m_pDoc->nasV))return 1;
  if (OnObject(DROSSEL,&m_pDoc->drosV))return 1;
  if (OnObject(AIRBATTERY,&m_pDoc->airbatV))return 1;
  if (OnObject(VODOBATTERY,&m_pDoc->vodovodoV))return 1;
  if (OnObject(SOPLO,&m_pDoc->soploV))return 1;
  if (OnObject(CONNECTOR,&m_pDoc->connV))return 1;
  if (OnObject(KRAN,&m_pDoc->kranV))return 1;
  if (OnObject(LABEL,&m_pDoc->labV))return 1;
  if (OnObject(EXTERM,&m_pDoc->extermV))return 1;
//  if (OnObject(SOPLO_OBJECT,&m_pDoc->sopObjV))return 1;
  ::SetClassLong(this->m_hWnd,GCL_HCURSOR,(long)old);
  return 0; 
}
//-----------------------------------------------------------------------------------
template <class V>
bool CSheme::OnObject(long type,V * vect)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();

  V::iterator loop;
  long i=0;
  for (loop=vect->begin();loop!=vect->end();loop++)
  {
    COMMON_DATA cp=loop->GetCommonParam();
    if (cp.UserNumber!=0)
    {
      if (loop->PointInRgn(&m))
      {
          type_selected=type;
          duo_selected.place.Type=type;
          duo_selected.place.Number=i;
          loop->SetFocus(TRUE);
          ::SetClassLong(this->m_hWnd,GCL_HCURSOR,(long)cr);
        return 1;
      }
    }
    i++;
  }
  return 0;
}
//-----------------------------------------------------------------------------------
void CSheme::OnLButtonDown(UINT nFlags, CPoint point) 
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  SetCapture();
  switch (DebugMode)
  {
  case 0://простой режим(расстановка объектов)
    {
      int el=element;
      element=-1;       
      m.X=getGLX(point.x);
      m.Y=getGLY(point.y);
      if (!MouseOnSub(false))
      {
        if (MouseOnSwitch())
        {
          SwitchObject(point);
          CompileList();  
        }       
        else 
        {
          if (MouseOnObject())
          {
            OnRunLClick(ONLY_ONE_CLICK);            
            CompileList();
          }     
        }
      }
      element=el;
      break;
    }
  case 1:
    {ZoomIn(point);break;}//режим Масштабироавния ZOOM_IN   
  }
  CCustomView::OnLButtonDown(nFlags, point);
}
//-----------------------------------------------------------------------------------
void CSheme::OnLButtonUp(UINT nFlags, CPoint point) 
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  ReleaseCapture();
  switch (DebugMode)
  {
  case 0:
    {
      EndDragText();      
      break;
    }
  case 1://режим Масштабироавния ZOOM_IN
    {OnZoomInMouseUP(); break;}
  case 2://режим Масштабироавния ZOOM_OUT
    {ZoomOut(point);break;}
  }
  CCustomView::OnLButtonUp(nFlags, point);
} 
//-----------------------------------------------------------------------------------
void CSheme::SetTextToElem(PLACE place,CString newText)
{ 
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  CDuo dd;
  dd.first=dd.second=-1;
  dd.place=place;
  CTempl* el=m_pDoc->GetTempl(dd);
  ELEMENT_DATA el_data;
  el->GetData(&el_data);
  strncpy(el_data.Text,newText,256);
  el->Update(&el_data);
}
//-----------------------------------------------------------------------------------
void CSheme::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
  if (MouseOnObject())
  {
    CConsrtuctorDoc * m_pDoc=GetDocument(); 
    SendData(ONLY_TWO_CLICK);   
  }
  CCustomView::OnLButtonDblClk(nFlags, point);
}
//-----------------------------------------------------------------------------------
bool CSheme::SendData(long param)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return false;
  CTempl * ActiveItr=m_pDoc->GetTempl(duo_selected);
  DATA_TRANSFER Transfer;
  Transfer.place=m_pDoc->GetDuo(ActiveItr).place;
  ELEMENT_DATA data;
  ActiveItr->GetData(&data);
  Transfer.group=(ActiveItr->GetGroup()).group;
  strcpy(Transfer.cd.TypeString,data.cd.TypeString);
  Transfer.cd.UserNumber=data.cd.UserNumber;
  strcpy(Transfer.cd.UserString,data.cd.UserString);
  COPYDATASTRUCT cd;
  cd.lpData=&Transfer;
  cd.cbData=sizeof(Transfer);
  HWND hwnd;
  Transfer.PARAM=param;
/*
  hwnd=::GetParent(this->m_hWnd); 
  ///
  hwnd=::GetParent(hwnd); 
  ///
  if (hwnd != NULL)
  {
    return ::SendMessage(hwnd,WM_COPYDATA,0,(LPARAM) &cd);
  }
*/
#if 0
  CRealPotrebMain *rm = (CRealPotrebMain *) GetParent()->GetParent();
  if ( rm ) {
    rm->CopyData(&cd);
  }
#endif
  return false;
}
//-----------------------------------------------------------------------------------
//----------------XML-----------
/*void CSheme::LoadXML(CArchive& ar)
{
  //StopMovie();
  ClearAll();
  if (!ReadStart(ar,"body"))return;
  if (!ReadScreen(ar,&sc))return;
  if (!ReadFontData(ar,&font_data))return;
  while ( LoadXMLAr(ar));
  //InitDisplay();
  //CompileList();  
  //StartMovie();
} 
*/
//-----------------------------------------------------------------------------------

void CSheme::OnRunLClick(long param)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  long par=param;
  CTempl * ActiveItr=m_pDoc->GetTempl(duo_selected);
  if (ActiveItr->HasText())if (ActiveItr->SwitchShowText())par|=ONLY_SHOW_TEXT;   
  if (SendData(par)) CompileList();
}
void CSheme::SetFocusToElement(const long UserNumber,const char * UserName)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  if (SetFocusTo(&m_pDoc->batV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->tubeV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->batV,UserNumber,UserName))return;;
  if (SetFocusTo(&m_pDoc->turnV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->triV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->ventV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->nasV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->drosV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->airbatV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->vodovodoV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->soploV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->connV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->kranV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->labV,UserNumber,UserName))return;
  if (SetFocusTo(&m_pDoc->extermV,UserNumber,UserName))return;
//  if (SetFocusTo(&m_pDoc->sopObjV,UserNumber,UserName))return;
}
//-----------------------------------------------------------------------------------
template <class V>
BOOL CSheme::SetFocusTo(V * vect,const long UserNumber,const char * UserName)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return FALSE;
  V::iterator  itr;
  for (itr=vect->begin();itr!=vect->end();itr++)
  {
    COMMON_DATA cd=itr->GetCommonParam();
//    if (cd.UserNumber==UserNumber)
    if (cd.UserNumber == UserNumber)
    {
//      if (UserName[0]) if (cd.UserString!=UserName) continue;
      if (UserName[0]) if (strcmp(cd.UserString, UserName)) continue;
      m_pDoc->SetZeroFocusAll();
      itr->SetFocus(TRUE);
      return TRUE;
    }
  }
  return FALSE;
}
//-----------------------------------------------------------------------------------
void CSheme::OpenObject(const bool open,const long UserNumber,const char * UserName)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  if (OpenObjectTo(&m_pDoc->ventV,open,UserNumber,UserName))return;
  if (OpenObjectTo(&m_pDoc->nasV,open,UserNumber,UserName))return;
  if (OpenObjectTo(&m_pDoc->airbatV,open,UserNumber,UserName))return;
  if (OpenObjectTo(&m_pDoc->kranV,open,UserNumber,UserName))return;
}
//-----------------------------------------------------------------------------------
template <class V>
BOOL CSheme::OpenObjectTo(V * vect,const bool open,const long UserNumber,const char * UserName)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return FALSE;
  V::iterator  itr;
  for (itr=vect->begin();itr!=vect->end();itr++)
  {
    COMMON_DATA cd=itr->GetCommonParam();
    if (cd.UserNumber==UserNumber)
    {
//      if (UserName[0]) if (cd.UserString!=UserName) continue;
      if (UserName[0]) if (strcmp(cd.UserString, UserName)) continue;
      if ( (open&&(itr->sw->ON()!=true))||
         ((!open)&&(itr->sw->ON()!=false)) )
      {
        itr->sw->Click();
        m_pDoc->SwitchGroup(itr->GetGroup().group,itr->sw->ON());
      }
      return TRUE;
    }
  }
  return FALSE;
}
//-----------------------------------------------------------------------------------
void CSheme::ShowDiafrafma(const bool show,const long UserNumber,const char * UserName)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  DROSSEL_VECTOR::iterator  itr;
  for (itr=m_pDoc->drosV.begin();itr!=m_pDoc->drosV.end();itr++)
  {
    COMMON_DATA cd=itr->GetCommonParam();
    if (cd.UserNumber==UserNumber)
    {
//      if (UserName[0]) if (cd.UserString != UserName) continue;
      if (UserName[0]) if (strcmp(cd.UserString, UserName)) continue;
      itr->ShowText(show);
      CompileList();
      return ;
    }
  }
}
//-----------------------------------------------------------------------------------
void CSheme::ShowZadvizhka(const bool open,const long UserNumber,const char * UserName)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  VENTIL_VECTOR::iterator  itr;
  for (itr=m_pDoc->ventV.begin();itr!=m_pDoc->ventV.end();itr++)
  {
    COMMON_DATA cd=itr->GetCommonParam();
    if (cd.UserNumber==UserNumber)
    {
//      if (UserName[0]) if (cd.UserString!=UserName) continue;
      if (UserName[0]) if (strcmp(cd.UserString, UserName)) continue;
      CVentil *v = (CVentil*)&*itr;
//      if ( v->sw->ON() != open ) {
//        m_pDoc->SwitchGroup(v->GetGroup().group, open);
        v->sw->Open(open); 
//        v->Open(open ? 1: 0); 
//      }
      return ;
    }
  }
}
//-----------------------------------------------------------------------------------
void CSheme::ShowDiafrafma(const bool show)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  DROSSEL_VECTOR::iterator  itr;
  for (itr=m_pDoc->drosV.begin();itr!=m_pDoc->drosV.end();itr++)
  {
    COMMON_DATA cd=itr->GetCommonParam();
    itr->ShowText(show);    
  }
  CompileList();
}
//-----------------------------------------------------------------------------------

void CSheme::CallOpenGroup(const long group_num,bool open)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  CallOpenGroupIn(&m_pDoc->tubeV,group_num,open); 
  CallOpenGroupIn(&m_pDoc->ventV,group_num,open); 
  CallOpenGroupIn(&m_pDoc->nasV,group_num,open);  
  CallOpenGroupIn(&m_pDoc->drosV,group_num,open); 
  CallOpenGroupIn(&m_pDoc->soploV,group_num,open);  
  CallOpenGroupIn(&m_pDoc->kranV,group_num,open); 
  //CallOpenGroupIn(&m_pDoc->sopObjV,group_num,open); ?????   
}
//-----------------------------------------------------------------------------------
template <class V>
void CSheme::CallOpenGroupIn(V * vect,const long group_num,const bool open)
{
  V::iterator  itr;
  for (itr=vect->begin();itr!=vect->end();itr++)
  {
    if (itr->GetGroup().group==group_num)
    {
      COMMON_DATA cp=itr->GetCommonParam();
      OpenObject(open,cp.UserNumber,cp.UserString);
    }
  } 
}
//-----------------------------------------------------------------------------------
void CSheme::SetTextToElem(const bool show,const long UserNumber,const char * UserName,const char * newText)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  if (SetTextTo(&m_pDoc->tubeV,show,UserNumber,UserName,newText))return;
  if (SetTextTo(&m_pDoc->batV,show,UserNumber,UserName,newText))return;
  if (SetTextTo(&m_pDoc->turnV,show,UserNumber,UserName,newText))return;
  if (SetTextTo(&m_pDoc->triV,show,UserNumber,UserName,newText))return; 
  if (SetTextTo(&m_pDoc->ventV,show,UserNumber,UserName,newText))return;  
  if (SetTextTo(&m_pDoc->nasV,show,UserNumber,UserName,newText))return; 
  if (SetTextTo(&m_pDoc->drosV,show,UserNumber,UserName,newText))return;  
  if (SetTextTo(&m_pDoc->airbatV,show,UserNumber,UserName,newText))return;  
  if (SetTextTo(&m_pDoc->vodovodoV,show,UserNumber,UserName,newText))return;  
  if (SetTextTo(&m_pDoc->soploV,show,UserNumber,UserName,newText))return; 
  if (SetTextTo(&m_pDoc->connV,show,UserNumber,UserName,newText))return;  
  if (SetTextTo(&m_pDoc->kranV,show,UserNumber,UserName,newText))return;  
  if (SetTextTo(&m_pDoc->labV,show,UserNumber,UserName,newText))return; 
  if (SetTextTo(&m_pDoc->extermV,show,UserNumber,UserName,newText))return;  
//  if (SetTextTo(&m_pDoc->sopObjV,show,UserNumber,UserName,newText))return;  
}
//-----------------------------------------------------------------------------------
template <class V>
bool CSheme::SetTextTo(V * vect,const bool show,const long UserNumber,const char * UserName,const char * newText)
{
  V::iterator  itr;
  for (itr=vect->begin();itr!=vect->end();itr++)
  {
    if (itr->HasText())
    {
      COMMON_DATA cd=itr->GetCommonParam();
      if (cd.UserNumber==UserNumber)
      {
        if (UserName[0]) {
//          if (cd.UserString!=UserName) continue;
          if (strcmp(cd.UserString, UserName)) continue;
        }
        itr->ShowText(show);    
        ELEMENT_DATA el_data;
        itr->GetData(&el_data);
        strncpy(el_data.Text,newText,256);
        itr->Update(&el_data);
        CompileList();
        return true;
      }
    }
  }
  return false;
}
//-----------------------------------------------------------------------------------
LRESULT CSheme::OnAction(WPARAM wParam, LPARAM lParam)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return FALSE;
  return m_pDoc->OnAction(wParam,lParam);
}
//-----------------------------------------------------------------------------------
BOOL CSheme::Action(WPARAM wParam, LPARAM lParam)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return FALSE;
  return m_pDoc->OnAction(wParam,lParam);
}
//-----------------------------------------------------------------------------------
void CSheme::SwitchObject(CPoint point)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  m.X=getGLX(point.x);
  m.Y=getGLY(point.y);
  CTempl * active_elem=m_pDoc->GetTempl(duo_selected);
  bool onoff=active_elem->OnClickInClickRgn(&m);
  m_pDoc->SwitchGroup(active_elem->GetGroup().group,onoff);
}
/////////////////////////////////////////////////////////////////////////////
///ADD///////////////////////////////////////////////////////////////////////
// CSheme при создании отдельно от проекта
CSheme::CSheme(const char *FileName,RECT &Clrect)
{
  dx=0;dy=0;
  origin=0;
  tempOrigin=0;
  listik=501;
  TxtElemPlace.Type=-1;
  TxtElemPlace.Number=-1;
  TxtShift[0]=0;
  TxtShift[1]=0;
  DraggingText=FALSE;
  dx=0;dy=0;
  origin=0;
  tempOrigin=0;
  fileN=FileName;
  if (!Open(FileName)) return;
  else 
  {
    Clrect.left=Clrect.top=0;
    Clrect.bottom=sc.CY;
    Clrect.right=sc.CX;
  } 
}

void CSheme::setSize(RECT &Clrect)
{
    Clrect.left=Clrect.top=0;
    Clrect.bottom=sc.CY;
    Clrect.right=sc.CX;
}


bool CSheme::Open(const char *FileName)
{
// return false;
  
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return false;
  CFile fl;
  if (!fl.Open(FileName,CFile::modeRead)) return false;
  CArchive ar(&fl,CArchive::load);
  const CFile* fp = ar.GetFile();
  m_pDoc->Serialize(ar);
  ///
  sc.BackColor2=sc.BackColor;
  ///
  ar.Close();
  fl.Close();
//  CompileList();   

//  return false;
}
bool CSheme::Close(const char *FileName)
{
//  CConsrtuctorDoc * m_pDoc=GetDocument();
//  if (m_pDoc==NULL) return false;
//  CFile fl;

//  if (!fl.Open(FileName,CFile::modeWrite)) return false;
//  CArchive ar(&fl,CArchive::store);
//  const CFile* fp = ar.GetFile();
//  m_pDoc->Serialize(ar);
//  ar.Close();
//  fl.Close();
    return false;
}
CSheme::~CSheme()
{
//  Close(fileN);
}
