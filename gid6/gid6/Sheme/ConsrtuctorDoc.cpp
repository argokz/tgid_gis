// ConsrtuctorDoc.cpp : implementation of the CConsrtuctorDoc class
//

#include "stdafx.h"

#include "ConsrtuctorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConsrtuctorDoc
#include "CustomView.h" ///Addon

BEGIN_MESSAGE_MAP(CConsrtuctorDoc, CDocument)
  //{{AFX_MSG_MAP(CConsrtuctorDoc)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConsrtuctorDoc construction/destruction


CConsrtuctorDoc::~CConsrtuctorDoc()
{
}

BOOL CConsrtuctorDoc::OnNewDocument()
{
  if (!CDocument::OnNewDocument())
    return FALSE;
  CCustomView* m_pView=GetActiveView();
  m_pView->StopMovie();
  ClearAll();
  m_pView->OnKeyDown(VK_ESCAPE,0,0);
  m_pView->CompileList();
  m_pView->StartMovie();
//  m_pView->UpdateWindow();
     
    // TODO: add reinitialization code here
  // (SDI documents will reuse this document)

  return TRUE;
}
CCustomView * CConsrtuctorDoc::GetActiveView() 
{
  return mCustomView;
/*  CCustomView * ActiveView=NULL; 
  POSITION pos = GetFirstViewPosition();
  if (pos != NULL)
  {
      ActiveView=(CCustomView *)GetNextView(pos);       
  }
  return ActiveView;*/
}
/////////////////////////////////////////////////////////////////////////////
// CConsrtuctorDoc diagnostics

#ifdef _DEBUG
void CConsrtuctorDoc::AssertValid() const
{
  CDocument::AssertValid();
}

void CConsrtuctorDoc::Dump(CDumpContext& dc) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConsrtuctorDoc commands

void CConsrtuctorDoc::ClearAll()
{
  if (!tubeV.empty())tubeV.clear();
  if (!batV.empty())batV.clear();
  if (!turnV.empty())turnV.clear();
  if (!triV.empty())triV.clear();
  if (!ventV.empty())ventV.clear();
  if (!nasV.empty())nasV.clear();
  if (!drosV.empty())drosV.clear();
  if (!airbatV.empty())airbatV.clear();
  if (!vodovodoV.empty())vodovodoV.clear();
  if (!soploV.empty())soploV.clear();
  if (!connV.empty())connV.clear();
  if (!kranV.empty())kranV.clear(); 
  if (!labV.empty())labV.clear(); 
  if (!extermV.empty())extermV.clear(); 
//  if (!sopObjV.empty())sopObjV.clear();
  //vector of vectors
  if (!PairsV.empty())PairsV.clear();
}
void CConsrtuctorDoc::SetZeroFocusAll()
{
  SetZeroFocus(&batV);
  SetZeroFocus(&tubeV);
  SetZeroFocus(&batV);
  SetZeroFocus(&turnV);
  SetZeroFocus(&triV);
  SetZeroFocus(&ventV);
  SetZeroFocus(&nasV);
  SetZeroFocus(&drosV);
  SetZeroFocus(&airbatV);
  SetZeroFocus(&vodovodoV);
  SetZeroFocus(&soploV);
  SetZeroFocus(&connV);
  SetZeroFocus(&kranV);
  SetZeroFocus(&labV);
  SetZeroFocus(&extermV);
//  SetZeroFocus(&sopObjV);
}
//-----------------------------------------------------------------------------------
template <class V>
void CConsrtuctorDoc::SetZeroFocus(V * vect)
{
  V::iterator  itr;
  for (itr=vect->begin();itr!=vect->end();itr++)itr->SetFocus(0);
}
//----------------XML-------------------------------------------------------------------
void CConsrtuctorDoc::SaveXML(CArchive& ar)
{
  CCustomView* m_pView=GetActiveView();
  WriteStart(ar,"body");
  WriteScreen(ar,&m_pView->sc);
  WriteFontData(ar,&m_pView->font_data);
  SaveXMLAr(&tubeV,ar,TUBE);
  SaveXMLAr(&batV,ar,BATTERY);
  SaveXMLAr(&turnV,ar,TURN);
  SaveXMLAr(&triV,ar,TRINITY);
  SaveXMLAr(&ventV,ar,VENTIL);
  SaveXMLAr(&nasV,ar,NASOS);
  SaveXMLAr(&drosV,ar,DROSSEL);
  SaveXMLAr(&airbatV,ar,AIRBATTERY);
  SaveXMLAr(&vodovodoV,ar,VODOBATTERY);
  SaveXMLAr(&soploV,ar,SOPLO);
  SaveXMLAr(&connV,ar,CONNECTOR);   
  SaveXMLAr(&kranV,ar,KRAN);    
  SaveXMLAr(&labV,ar,LABEL);    
  SaveXMLAr(&extermV,ar,EXTERM);
//  SaveXMLAr(&sopObjV,ar,SOPLO_OBJECT);
  WriteEnd(ar,"body");
}
void CConsrtuctorDoc::LoadXML(CArchive& ar)
{
  CCustomView* m_pView=GetActiveView();
  m_pView->StopMovie();
  ClearAll();
  if (!ReadStart(ar,"body"))return;
  if (!ReadScreen(ar,&m_pView->sc))return;
  if (!ReadFontData(ar,&m_pView->font_data))return;
  while ( LoadXMLAr(ar));
  m_pView->InitDisplay();
  m_pView->CompileList(); 
  m_pView->StartMovie();    
}
//----WriteXML-------------------------------------------------------------------
void CConsrtuctorDoc::WriteScreen(CArchive& ar,SCREEN * sc)
{
  WriteStart(ar,"Screen");
  WriteElement(ar,"Width",(long)sc->CX);
  WriteElement(ar,"Height",(long)sc->CY);
  WriteElement(ar,"WorldWidth2",(long)sc->XL2);
  WriteElement(ar,"WorldHeight2",(long)sc->YL2);  
  WriteStart(ar,"Steps");
  WriteElement(ar,"CurrentStep",(long)sc->Step);  
  WriteElement(ar,"OriginalStep",(long)sc->OriginalStep); 
  WriteEnd(ar,"Steps");
  WriteStart(ar,"WorldRect");
  WriteElement(ar,"WR_Left",(double)sc->Wrect.left);  
  WriteElement(ar,"WR_Top",(double)sc->Wrect.top);  
  WriteElement(ar,"WR_Width",(double)sc->Wrect.width);    
  WriteElement(ar,"WR_Height",(double)sc->Wrect.height);    
  WriteEnd(ar,"WorldRect");
  WriteStart(ar,"Colors");
    WriteColor(ar,"Fore",&sc->ForeColor);
    WriteColor(ar,"Back",&sc->BackColor);
    WriteColor(ar,"Back2",&sc->BackColor2);
    WriteColor(ar,"Anim",&sc->AnimColor);
    WriteColor(ar,"Cold",&sc->ColdColor);
    WriteColor(ar,"Hot",&sc->HotColor);
  WriteEnd(ar,"Colors");
  WriteEnd(ar,"Screen");
}
void CConsrtuctorDoc::WriteFontData(CArchive& ar,FONT_DATA * font_data)
{
  WriteStart(ar,"Font_data");
  WriteColor(ar,"Color",&font_data->color);
    WriteStart(ar,"Scale");
    WriteElement(ar,"X",(double)font_data->scale.X);
    WriteElement(ar,"Y",(double)font_data->scale.Y);
    WriteElement(ar,"Z",(double)font_data->scale.Z);
    WriteEnd(ar,"Scale");
  WriteLF(ar,&font_data->lf);
  WriteEnd(ar,"Font_data"); 
}
void CConsrtuctorDoc::WriteStart(CArchive& ar,CString name)
{
  CString Mystr;
  Mystr="<";
  Mystr+=name;
  Mystr+=">";
  Mystr+="\r\n";
  ar.WriteString(_T(Mystr));  
}
void CConsrtuctorDoc::WriteEnd(CArchive& ar,CString name)
{
  CString Mystr;
  Mystr="</";
  Mystr+=name;
  Mystr+=">";
  Mystr+="\r\n";
  ar.WriteString(_T(Mystr));  
}
void CConsrtuctorDoc::WriteColor(CArchive& ar,CString name,TCOLOR * color)
{
  WriteStart(ar,name);
  WriteElement(ar,"R",(long)color->R);    
  WriteElement(ar,"G",(long)color->G);    
  WriteElement(ar,"B",(long)color->B);    
  WriteEnd(ar,name);
}
void CConsrtuctorDoc::WriteLF(CArchive& ar,LOGFONT * lf)
{
  WriteStart(ar,"LOGFONT");
  WriteElement(ar,"lfHeight",(long)lf->lfHeight);   
  WriteElement(ar,"lfWidth",(long)lf->lfWidth);   
  WriteElement(ar,"lfEscapement",(long)lf->lfEscapement);   
  WriteElement(ar,"lfOrientation",(long)lf->lfOrientation);   
  WriteElement(ar,"lfWeight",(long)lf->lfWeight);   
  WriteElement(ar,"lfItalic",(long)lf->lfItalic);
  WriteElement(ar,"lfUnderline",(long)lf->lfUnderline);
  WriteElement(ar,"lfStrikeOut",(long)lf->lfStrikeOut);
  WriteElement(ar,"lfCharSet",(long)lf->lfCharSet);
  WriteElement(ar,"lfOutPrecision",(long)lf->lfOutPrecision);
  WriteElement(ar,"lfClipPrecision",(long)lf->lfClipPrecision);
  WriteElement(ar,"lfQuality",(long)lf->lfQuality);
  WriteElement(ar,"lfPitchAndFamily",(long)lf->lfPitchAndFamily);
  WriteElement(ar,"lfFaceName",(CString)lf->lfFaceName);
  WriteEnd(ar,"LOGFONT");
}
void CConsrtuctorDoc::WriteObject(CArchive& ar,ELEMENT_DATA * object)
{
  WriteStart(ar,"Object");
    WriteStart(ar,"CommonData");
    WriteElement(ar,"Type",(long)object->cd.Type);
    WriteElement(ar,"TypeString",(CString)object->cd.TypeString);
    WriteElement(ar,"UserNumber",(long)object->cd.UserNumber);
    WriteElement(ar,"UserString",(CString)object->cd.UserString);
    WriteEnd(ar,"CommonData");  
  WriteElement(ar,"X0",(long)object->X0);
  WriteElement(ar,"Y0",(long)object->Y0);
  WriteElement(ar,"Radius",(long)object->Radius);
  WriteElement(ar,"Angle",(long)object->Angle);
  WriteElement(ar,"Height",(long)object->Height);
  WriteElement(ar,"animate",(long)object->animate);
  WriteElement(ar,"Pribor",(long)object->Pribor);
  WriteElement(ar,"Right",(long)object->Right);
  WriteElement(ar,"Text",(CString)object->Text);
  WriteElement(ar,"Kol",(long)object->Kol);
  WriteElement(ar,"Opening",(long)object->Opening);
  WriteElement(ar,"Radius1",(long)object->Radius1);
  WriteRectan(ar,"Bounds",&object->Bounds);
  WriteGroup(ar,"GroupData",&object->grdat);
  WriteEnd(ar,"Object");
}
void CConsrtuctorDoc::WriteRectan(CArchive& ar,CString name,RECTAN * bound)
{
  WriteStart(ar,name);
  WriteElement(ar,"left",(long)bound->left);
  WriteElement(ar,"top",(long)bound->top);
  WriteElement(ar,"width",(long)bound->width);
  WriteElement(ar,"height",(long)bound->height);
  WriteEnd(ar,name);
}
void CConsrtuctorDoc::WriteGroup(CArchive& ar,CString name,GROUP_DATA * grd)
{
  WriteStart(ar,name);
  WriteElement(ar,"group",(long)grd->group);
  WriteElement(ar,"groupOn",(long)grd->groupOn);  
  WriteEnd(ar,name);
}
void CConsrtuctorDoc::WriteElement(CArchive& ar,CString name,long value)
{
  char buffer[100];
  CString Mystr;
  Mystr="<";
  Mystr+=name;
  Mystr+=">";
  Mystr+=_ltoa(value,buffer,10);
  Mystr+="</";
  Mystr+=name;
  Mystr+=">";
  Mystr+="\r\n";
  ar.WriteString(_T(Mystr));  
}
void CConsrtuctorDoc::WriteElement(CArchive& ar,CString name,double value)
{
  char buffer[100];
  CString Mystr;
  Mystr="<";
  Mystr+=name;
  Mystr+=">";
  Mystr+=_gcvt(value,10,buffer);
  Mystr+="</";
  Mystr+=name;
  Mystr+=">";
  Mystr+="\r\n";
  ar.WriteString(_T(Mystr));
}
void CConsrtuctorDoc::WriteElement(CArchive& ar,CString name,CString value)
{
  CString Mystr;
  Mystr="<";
  Mystr+=name;
  Mystr+=">";
  Mystr+=value;
  Mystr+="</";
  Mystr+=name;
  Mystr+=">";
  Mystr+="\r\n";
  ar.WriteString(_T(Mystr));  
}
template <class V>
void CConsrtuctorDoc::SaveXMLAr(V * vect,CArchive& ar,long type)
{
  CString name;
  switch (type)
  {
    case TUBE : {name="TUBE";break;}
    case BATTERY : {name="BATTERY";break;}
    case TURN : {name="TURN";break;}
    case TRINITY : {name="TRINITY";break;}
    case VENTIL : {name="VENTIL";break;}
    case NASOS : {name="NASOS";break;}
    case DROSSEL : {name="DROSSEL";break;}
    case AIRBATTERY : {name="AIRBATTERY";break;}
    case VODOBATTERY : {name="VODOBATTERY";break;}
    case SOPLO : {name="SOPLO";break;}
    case CONNECTOR : {name="CONNECTOR";break;}
    case KRAN : {name="KRAN";break;}
    case LABEL : {name="LABEL";break;}
  }
  WriteStart(ar,name);
  UINT kol=vect->size();
  //ar<<kol;
  ELEMENT_DATA str;
  for (UINT it=0;it<kol;it++)
  {
    (*vect)[it].GetData(&str);
    WriteObject(ar,&str);
  }
  WriteEnd(ar,name);
}
////////////////////////////////////////////////////////////////////////////////////////
//----ReadXML-----------------------------------------------------------------------------
BOOL CConsrtuctorDoc::ReadScreen(CArchive& ar,SCREEN * sc)
{
  if (!ReadStart(ar,"Screen"))return FALSE;
  long buffer=(long)sc->CX;
  if (!ReadElement(ar,"Width",buffer))return FALSE;
  sc->CX=buffer;
  if (!ReadElement(ar,"Height",buffer))return FALSE;
  sc->CY=buffer;
  if (!ReadElement(ar,"WorldWidth2",buffer))return FALSE;
  sc->XL2=buffer;
  if (!ReadElement(ar,"WorldHeight2",buffer))return FALSE;
  sc->YL2=buffer;
  
  if (!ReadStart(ar,"Steps"))return FALSE;
  if (!ReadElement(ar,"CurrentStep",buffer))return FALSE;
  sc->Step=buffer;
  if (!ReadElement(ar,"OriginalStep",buffer))return FALSE;
  sc->OriginalStep=buffer;
  
  double Dbuf;
  if (!ReadStart(ar,"WorldRect"))return FALSE;
  if (!ReadElement(ar,"WR_Left",Dbuf))return FALSE;
  sc->Wrect.left=Dbuf;
  if (!ReadElement(ar,"WR_Top",Dbuf))return FALSE;
  sc->Wrect.top=Dbuf;
  if (!ReadElement(ar,"WR_Width",Dbuf))return FALSE;
  sc->Wrect.width=Dbuf;
  if (!ReadElement(ar,"WR_Height",Dbuf))return FALSE;
  sc->Wrect.height=Dbuf;

  if (!ReadStart(ar,"Colors"))return FALSE;
  if (!ReadColor(ar,"Fore",&sc->ForeColor))return FALSE;
  if (!ReadColor(ar,"Back",&sc->BackColor))return FALSE;
  if (!ReadColor(ar,"Back2",&sc->BackColor2))return FALSE;
  if (!ReadColor(ar,"Anim",&sc->AnimColor))return FALSE;
  if (!ReadColor(ar,"Cold",&sc->ColdColor))return FALSE;
  if (!ReadColor(ar,"Hot",&sc->HotColor))return FALSE;
  return  TRUE; 
}
BOOL CConsrtuctorDoc::ReadFontData(CArchive& ar,FONT_DATA * font_data)
{
  if (!ReadStart(ar,"Font_data"))return FALSE;
  if (!ReadColor(ar,"Color",&font_data->color))return FALSE;
  if (!ReadStart(ar,"Scale"))return FALSE;
  double Dbuf;
  if (!ReadElement(ar,"X",Dbuf))return FALSE;
  font_data->scale.X=Dbuf;
  if (!ReadElement(ar,"Y",Dbuf))return FALSE;
  font_data->scale.Y=Dbuf;
  if (!ReadElement(ar,"Z",Dbuf))return FALSE;
  font_data->scale.Z=Dbuf;
  if (!ReadLF(ar,&font_data->lf))return FALSE;
  return TRUE;
}
BOOL CConsrtuctorDoc::ReadLF(CArchive& ar,LOGFONT * lf)
{
  if (!ReadStart(ar,"LOGFONT"))return FALSE;
  long buffer;
  if (!ReadElement(ar,"lfHeight",buffer))return FALSE;
  lf->lfHeight=buffer;
  if (!ReadElement(ar,"lfWidth",buffer))return FALSE;
  lf->lfWidth=buffer;
  if (!ReadElement(ar,"lfEscapement",buffer))return FALSE;
  lf->lfEscapement=buffer;
  if (!ReadElement(ar,"lfOrientation",buffer))return FALSE;
  lf->lfOrientation=buffer;
  if (!ReadElement(ar,"lfWeight",buffer))return FALSE;
  lf->lfWeight=buffer;
  if (!ReadElement(ar,"lfItalic",buffer))return FALSE;
  lf->lfItalic=buffer;
  if (!ReadElement(ar,"lfUnderline",buffer))return FALSE;
  lf->lfUnderline=buffer;
  if (!ReadElement(ar,"lfStrikeOut",buffer))return FALSE;
  lf->lfStrikeOut=buffer;
  if (!ReadElement(ar,"lfCharSet",buffer))return FALSE;
  lf->lfCharSet=buffer;
  if (!ReadElement(ar,"lfOutPrecision",buffer))return FALSE;
  lf->lfOutPrecision=buffer;
  if (!ReadElement(ar,"lfClipPrecision",buffer))return FALSE;
  lf->lfClipPrecision=buffer;
  if (!ReadElement(ar,"lfQuality",buffer))return FALSE;
  lf->lfQuality=buffer;
  if (!ReadElement(ar,"lfPitchAndFamily",buffer))return FALSE;
  lf->lfPitchAndFamily=buffer;
  
  CString dest;
  if (!ReadElementString(ar,"lfFaceName",dest))return FALSE;
  strcpy(lf->lfFaceName,(LPCTSTR)dest);
  return TRUE;
}
BOOL CConsrtuctorDoc::ReadStart(CArchive& ar,CString name)
{
  CString Mystr;
  CString Source_str="<"+name+">";
  while (ar.ReadString(Mystr))
  {
    if (Mystr==Source_str)
    {
      return TRUE;
    }
  }
  return FALSE;
}
BOOL CConsrtuctorDoc::ReadElement(CArchive& ar,CString name,long &value)
{
  CString dest;
  if (!ReadElementString(ar,name,dest))return FALSE;
  value=atol(dest);
  return TRUE;
}
BOOL CConsrtuctorDoc::ReadElement(CArchive& ar,CString name,double &value)
{
  CString dest;
  if (!ReadElementString(ar,name,dest))return FALSE;
  value=atof(dest);
  return TRUE;
}
BOOL CConsrtuctorDoc::ReadColor(CArchive& ar,CString name,TCOLOR * color)
{
  if (!ReadStart(ar,name))return FALSE;
  long buffer=(long)color->R;
  if (!ReadElement(ar,"R",buffer))return FALSE;
  color->R=buffer;
  if (!ReadElement(ar,"G",buffer))return FALSE;
  color->G=buffer;
  if (!ReadElement(ar,"B",buffer))return FALSE;
  color->B=buffer;
  return TRUE;
}
BOOL CConsrtuctorDoc::ReadElementString(CArchive& ar,CString name,CString &dest)
{
  CString Mystr;
  CString Source_str="<"+name+">";
  while (ar.ReadString(Mystr))
  {
    int pos=Mystr.Find(Source_str);
    if ( pos>=0)
    {
      CString str="</"+name+">";
      long pos1;
      pos1=Mystr.Find(str);
      dest=Mystr.Left(pos1);      
      Mystr=dest.Right(dest.GetLength()-Source_str.GetLength());                  
      dest=Mystr;
      return TRUE;
    }
  }
  return FALSE;
}
BOOL CConsrtuctorDoc::ReadObject(CArchive& ar,ELEMENT_DATA * object)
{
  if (!ReadStart(ar,"Object"))return FALSE;
  
  if (!ReadStart(ar,"CommonData"))return FALSE;
  long buffer;
  if(!ReadElement(ar,"Type",buffer))return FALSE;
  object->cd.Type=buffer;
  CString dest; 
  if(!ReadElementString(ar,"TypeString",dest))return FALSE;
  strcpy(object->cd.TypeString,(LPCTSTR)dest);
  if(!ReadElement(ar,"UserNumber",buffer))return FALSE;
  object->cd.UserNumber=buffer;
  if(!ReadElementString(ar,"UserString",dest))return FALSE;
  strcpy(object->cd.UserString,(LPCTSTR)dest);
  if(!ReadElement(ar,"X0",buffer))return FALSE;
  object->X0=buffer;
  if(!ReadElement(ar,"Y0",buffer))return FALSE;
  object->Y0=buffer;
  if(!ReadElement(ar,"Radius",buffer))return FALSE;
  object->Radius=buffer;
  if(!ReadElement(ar,"Angle",buffer))return FALSE;
  object->Angle=buffer;
  if(!ReadElement(ar,"Height",buffer))return FALSE;
  object->Height=buffer;
  if(!ReadElement(ar,"animate",buffer))return FALSE;
  object->animate=buffer;
  if(!ReadElement(ar,"Pribor",buffer))return FALSE;
  object->Pribor=buffer;
  if(!ReadElement(ar,"Right",buffer))return FALSE;
  object->Right=buffer;
  if(!ReadElementString(ar,"Text",dest))return FALSE;
  strcpy(object->Text,(LPCTSTR)dest);
  if(!ReadElement(ar,"Kol",buffer))return FALSE;
  object->Kol=buffer;
  if(!ReadElement(ar,"Opening",buffer))return FALSE;
  object->Opening=buffer;
  if(!ReadElement(ar,"Radius1",buffer))return FALSE;
  object->Radius1=buffer;
  if(!ReadRectan(ar,"Bounds",&object->Bounds))return FALSE;
  if(!ReadGroup(ar,"GroupData",&object->grdat))return FALSE;
  return TRUE;
}
BOOL CConsrtuctorDoc::ReadRectan(CArchive& ar,CString name,RECTAN * bound)
{
  if (!ReadStart(ar,name))return FALSE;
  long buffer;
  if(!ReadElement(ar,"left",buffer))return FALSE;
  bound->left=buffer;
  if(!ReadElement(ar,"top",buffer))return FALSE;
  bound->top=buffer;
  if(!ReadElement(ar,"width",buffer))return FALSE;
  bound->width=buffer;
  if(!ReadElement(ar,"height",buffer))return FALSE;
  bound->height=buffer;
  return TRUE;
}
BOOL CConsrtuctorDoc::ReadGroup(CArchive& ar,CString name,GROUP_DATA * grd)
{
  if (!ReadStart(ar,name))return FALSE;
  long buffer;
  if(!ReadElement(ar,"group",buffer))return FALSE;
  grd->group=buffer;
  if(!ReadElement(ar,"groupOn",buffer))return FALSE;
  grd->groupOn=buffer;
  return TRUE;
}
BOOL CConsrtuctorDoc::LoadXMLAr(CArchive& ar)
{
  ELEMENT_DATA elem_dat;
  if ( !ReadObject(ar,&elem_dat))return FALSE;
  switch (elem_dat.cd.Type)
  {
    case TUBE : {AddElem(&elem_dat,(CTube *)NULL,&tubeV,false);break;}
    case BATTERY : {AddElem(&elem_dat,(CBattery *)NULL,&batV,false);break;}
    case TURN : {AddElem(&elem_dat,(CTurn *)NULL,&turnV,false);break;}
    case TRINITY : {AddElem(&elem_dat,(CTrinity *)NULL,&triV,false);break;}
    case VENTIL : {AddElem(&elem_dat,(CVentil *)NULL,&ventV,false);break;}
    case NASOS : {AddElem(&elem_dat,(CNasos *)NULL,&nasV,false);break;}
    case DROSSEL : {AddElem(&elem_dat,(CDrossel *)NULL,&drosV,false);break;}
    case AIRBATTERY : {AddElem(&elem_dat,(CAirBattery *)NULL,&airbatV,false);break;}
    case VODOBATTERY : {AddElem(&elem_dat,(CVodoVodo *)NULL,&vodovodoV,false);break;}
    case SOPLO : {AddElem(&elem_dat,(CSoplo *)NULL,&soploV,false);break;}
    case CONNECTOR : {AddElem(&elem_dat,(CConnector *)NULL,&connV,false);break;}
    case KRAN : {AddElem(&elem_dat,(CKran *)NULL,&kranV,false);break;}
    case LABEL : {AddElem(&elem_dat,(CLabel *)NULL,&labV,false);break;}
    case EXTERM : {AddElem(&elem_dat,(CExtTermo *)NULL,&extermV,false);break;}
//    case SOPLO_OBJECT : {AddElem(&elem_dat,(CSoploObject *)NULL,&sopObjV,false);break;}   
    default :
    {
      return FALSE;     
    }
  }
  return TRUE;
}
//----------------XML---------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CConsrtuctorDoc serialization
void CConsrtuctorDoc::Serialize(CArchive& ar)
{
  CCustomView* m_pView=GetActiveView();

  const CFile* fp = ar.GetFile();
  CString str=fp->GetFileName( );
  CString str1=str.Right(3);
  str1.MakeUpper();
  if (str1=="XML") 
  {
    if (ar.IsStoring())
    {
      SaveXML(ar);
    }
    else 
    {
      LoadXML(ar);
    }
  }
  else
  {
    if (ar.IsStoring())
    {
      ar.Write(&m_pView->sc,sizeof(m_pView->sc));
      ar.Write(&m_pView->font_data,sizeof(m_pView->font_data));   
      SaveAr(&tubeV,ar);
      SaveAr(&batV,ar);
      SaveAr(&turnV,ar);
      SaveAr(&triV,ar);
      SaveAr(&ventV,ar);
      SaveAr(&nasV,ar);
      SaveAr(&drosV,ar);
      SaveAr(&airbatV,ar);
      SaveAr(&vodovodoV,ar);
      SaveAr(&soploV,ar);
      SaveAr(&connV,ar);    
      SaveAr(&kranV,ar);    
      SaveAr(&labV,ar);   
      SaveAr(&extermV,ar);    
//      SaveAr(&sopObjV,ar);    

    }
    else
    {
      ClearAll();
      ar.Read(&m_pView->sc,sizeof(m_pView->sc));
      ar.Read(&m_pView->font_data,sizeof(m_pView->font_data));    
      LoadAr(&tubeV,(CTube*)NULL,ar);
      LoadAr(&batV,(CBattery*)NULL,ar);
      LoadAr(&turnV,(CTurn*)NULL,ar);
      LoadAr(&triV,(CTrinity*)NULL,ar);
      LoadAr(&ventV,(CVentil*)NULL,ar);
      LoadAr(&nasV,(CNasos*)NULL,ar);
      LoadAr(&drosV,(CDrossel*)NULL,ar);
      LoadAr(&airbatV,(CAirBattery*)NULL,ar);
      LoadAr(&vodovodoV,(CVodoVodo*)NULL,ar);
      LoadAr(&soploV,(CSoplo*)NULL,ar);
      LoadAr(&connV,(CConnector*)NULL,ar);
      LoadAr(&kranV,(CKran*)NULL,ar);
      LoadAr(&labV,(CLabel*)NULL,ar);
      LoadAr(&extermV,(CExtTermo*)NULL,ar);

//      LoadAr(&sopObjV,(CSoploObject*)NULL,ar);
    }
  }
}
template <class V> 
void CConsrtuctorDoc::SaveAr(V * vect,CArchive& ar)
{
  UINT kol=vect->size();
  ar<<kol;
  ELEMENT_DATA str;
  for (UINT it=0;it<kol;it++)
  {
    (*vect)[it].GetData(&str);
    ar.Write(&str,sizeof(str));
  }
}
template <class V,class Comp>
void CConsrtuctorDoc::LoadAr(V * vect,Comp * comp,CArchive& ar)
{
  UINT kol;
  ar>>kol;
  ELEMENT_DATA str;
  for (UINT it=0;it<kol;it++)
  {
    ar.Read(&str,sizeof(str));
    AddElem(&str,comp,vect,false);
  }
}
////////////////////////////////////////////////////////////////////////////////////////
//------------добавление/удаление---------------------------------------------------------
CDuo CConsrtuctorDoc::AddElement(int type,ELEMENT_DATA * el_dat)
{
  CCustomView* m_pView=GetActiveView();
  CDuo duo;
  switch (type)
  {
  case 0 : {break;}
  case TUBE : {duo=AddElem(el_dat,(CTube*)NULL,&tubeV,true);break;}
  case BATTERY : {duo=AddElem(el_dat,(CBattery*)NULL,&batV,true);break;}
  case TURN : {duo=AddElem(el_dat,(CTurn*)NULL,&turnV,true);break;}
  case TRINITY : {duo=AddElem(el_dat,(CTrinity*)NULL,&triV,true);break;}
  case VENTIL : {duo=AddElem(el_dat,(CVentil*)NULL,&ventV,true);break;}
  case NASOS : {duo=AddElem(el_dat,(CNasos*)NULL,&nasV,true);break;  }
  case DROSSEL : {duo=AddElem(el_dat,(CDrossel*)NULL,&drosV,true);break;   }
  case AIRBATTERY : {duo=AddElem(el_dat,(CAirBattery*)NULL,&airbatV,true);break; }
  case VODOBATTERY : {duo=AddElem(el_dat,(CVodoVodo*)NULL,&vodovodoV,true);break; }
  case SOPLO : {duo=AddElem(el_dat,(CSoplo*)NULL,&soploV,true);break; }
  case CONNECTOR : {duo=AddElem(el_dat,(CConnector*)NULL,&connV,true);break; }
  case KRAN : {duo=AddElem(el_dat,(CKran*)NULL,&kranV,true);break; }
  case LABEL : {duo=AddElem(el_dat,(CLabel*)NULL,&labV,true);break; }
  case EXTERM: {duo=AddElem(el_dat,(CExtTermo*)NULL,&extermV,true);break; }
    
    
//  case SOPLO_OBJECT : {duo=AddElem(el_dat,(CSoploObject*)NULL,&sopObjV,true);break; }
  } 
  return duo;
}

void CConsrtuctorDoc::DeleteElement(const CDuo Duo, bool deleting_in_group)
{
  CCustomView* m_pView=GetActiveView();
  if (!deleting_in_group) m_pView->StopMovie();
  switch (Duo.place.Type)
    {
    case TUBE: {DeleteElem(Duo,&tubeV);break;}
    case BATTERY: {DeleteElem(Duo,&batV);break;}
    case TURN: {DeleteElem(Duo,&turnV);break;}
    case TRINITY: {DeleteElem(Duo,&triV);break;}
    case VENTIL: {DeleteElem(Duo,&ventV);break;}
    case NASOS: {DeleteElem(Duo,&nasV);break;}
    case DROSSEL: {DeleteElem(Duo,&drosV);break;}
    case AIRBATTERY: {DeleteElem(Duo,&airbatV);break;}
    case VODOBATTERY: {DeleteElem(Duo,&vodovodoV);break;}
    case SOPLO: {DeleteElem(Duo,&soploV);break;}
    case CONNECTOR: {DeleteElem(Duo,&connV);break;}
    case KRAN: {DeleteElem(Duo,&kranV);break;}
    case LABEL: {DeleteElem(Duo,&labV);break;}
    case EXTERM: {DeleteElem(Duo,&extermV);break;}
  
  //  case SOPLO_OBJECT : {DeleteElem(Duo,&sopObjV);break;}
    }     
  if (!deleting_in_group)
  {
    m_pView->type_selected=NONE;
    m_pView->CompileList();
    m_pView->OnKeyDown(VK_ESCAPE,0,0);
    m_pView->StartMovie();    
  }
}
//-----------------------------------------------------------------------------------
template <class V>
void CConsrtuctorDoc::DeleteElem(const CDuo Duo, V* vect)
{
	CCustomView* m_pView = GetActiveView();
	CTempl* itr;
	itr = GetTempl(Duo);

	DisconnectFromPair(itr, Duo);
	DecreaseFromPair(Duo);
	//  V::iterator iter = &vect->at(Duo.place.Number);

//	V::iterator iter(&vect->at(Duo.place.Number));

	V::iterator iter = vect->begin()+ Duo.place.Number;


	V v1;
	v1.insert(v1.begin(), vect->begin(), iter);
	v1.insert(v1.end(), ++iter, vect->end());
	if (vect->size() > 1)
	{
		vect->clear();
		vect->insert(vect->begin(), v1.begin(), v1.end());
	}
	else vect->clear();
}
//-----------------------------------------------------------------------------------
template <class Comp,class V>
CDuo CConsrtuctorDoc::AddElem(ELEMENT_DATA * str,Comp * comp,V * vect,bool FromBounds)
{
  CCustomView* m_pView=GetActiveView();
  CDuo duo;
  COMMON_DATA cd;
  comp= new Comp(str,FromBounds);
  comp->ConnectionCreate(m_pView);
  vect->push_back(*comp);
//  V::iterator last = &vect->back();

//  V::iterator last = &vect->back();

//  V::iterator last = vect->back(); 
  V::iterator last = vect->end(); last--;

  
  //  V *last = (&vect->back());
  if (FromBounds)
  {
    last->GetData(str);
    last->UpdateFromBounds(str);
  }
  delete comp;
  cd=last->GetCommonParam();
  duo.place.Type=cd.Type;
  duo.place.Number=vect->size()-1;
  Analyse(&vect->back()); 
  return duo;
}
//-----------------------------------------------------------------------------------
void CConsrtuctorDoc::PasteElement(CDuo &Duo, bool paste_in_group)
{
  CCustomView* m_pView=GetActiveView();
  if (!paste_in_group) m_pView->StopMovie();
  switch (Duo.place.Type)
    {
    case TUBE: {PasteElem(Duo,(CTube *)NULL,&tubeV);break;}
    case BATTERY: {PasteElem(Duo,(CBattery *)NULL,&batV);break;}
    case TURN: {PasteElem(Duo,(CTurn *)NULL,&turnV);break;}
    case TRINITY: {PasteElem(Duo,(CTrinity *)NULL,&triV);break;}
    case VENTIL: {PasteElem(Duo,(CVentil *)NULL,&ventV);break;}
    case NASOS: {PasteElem(Duo,(CNasos *)NULL,&nasV);break;}
    case DROSSEL: {PasteElem(Duo,(CDrossel *)NULL,&drosV);break;}
    case AIRBATTERY: {PasteElem(Duo,(CAirBattery *)NULL,&airbatV);break;}
    case VODOBATTERY: {PasteElem(Duo,(CVodoVodo *)NULL,&vodovodoV);break;}
    case SOPLO: {PasteElem(Duo,(CSoplo *)NULL,&soploV);break;}
    case CONNECTOR: {PasteElem(Duo,(CConnector *)NULL,&connV);break;}
    case KRAN: {PasteElem(Duo,(CKran *)NULL,&kranV);break;}
    case LABEL: {PasteElem(Duo,(CLabel *)NULL,&labV);break;}    
    case EXTERM: {PasteElem(Duo,(CExtTermo*)NULL,&extermV);break;}          
  //  case SOPLO_OBJECT : {PasteElem(Duo,(CSoploObject *)NULL,&sopObjV);break;}
    }   
  if (!paste_in_group)
  {
    m_pView->CompileList();
    m_pView->StartMovie();    
  }
}
//-----------------------------------------------------------------------------------
template <class V,class Comp>
void CConsrtuctorDoc::PasteElem(CDuo &Duo,Comp * comp,V * vect)
{
  CCustomView* m_pView=GetActiveView();
  CTempl * itr;
  itr=GetTempl(Duo);
  ELEMENT_DATA str;
  itr->GetData(&str);
  str.Bounds.left+=2*str.Radius;
  str.Bounds.top-=2*str.Radius;
  itr->SetFocus(0);
  AddElem(&str,comp,vect,true);
  vect->back().SetFocus(1);
  Duo=GetDuo(&(vect->back()));  
}
//-----------------------------------------------------------------------------------

//------------добавление/удаление---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//------------изменение родительского окна------------------------------------------------------------
void CConsrtuctorDoc::RemakeConnection(CTemplWnd * wnd)
{
  RemakeConnectionIn(wnd,&tubeV);
  RemakeConnectionIn(wnd,&batV);
  RemakeConnectionIn(wnd,&turnV);
  RemakeConnectionIn(wnd,&triV);
  RemakeConnectionIn(wnd,&ventV);
  RemakeConnectionIn(wnd,&nasV);
  RemakeConnectionIn(wnd,&drosV);
  RemakeConnectionIn(wnd,&airbatV);
  RemakeConnectionIn(wnd,&vodovodoV);
  RemakeConnectionIn(wnd,&soploV);
  RemakeConnectionIn(wnd,&connV);
  RemakeConnectionIn(wnd,&kranV); 
  RemakeConnectionIn(wnd,&labV);  
  RemakeConnectionIn(wnd,&extermV); 
//  RemakeConnectionIn(wnd,&sopObjV);
}
//-----------------------------------------------------------------------------------
template <class V>
void CConsrtuctorDoc::RemakeConnectionIn(CTemplWnd * wnd,V * vect)
{
  V::iterator  itr;
  for (itr=vect->begin();itr!=vect->end();itr++)itr->ConnectionCreate(wnd);
}
//------------изменение родительского окна------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////

// CConsrtuctorDoc serialization
/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//----------------Analyse-----------------------------------------------------------------
void CConsrtuctorDoc::Analyse(CTempl* check_elem)
{
//  return;
  
  CDuo duo;
  duo=GetDuo(check_elem);
  //рассоединение элемента check_elem от остальных
  DisconnectFromPair(check_elem,duo);
  //поиск подсоединения
  SearchNear(check_elem,duo,&tubeV);
  SearchNear(check_elem,duo,&batV);
  SearchNear(check_elem,duo,&turnV);
  SearchNear(check_elem,duo,&triV);
  SearchNear(check_elem,duo,&ventV);
  SearchNear(check_elem,duo,&nasV);
  SearchNear(check_elem,duo,&drosV);
  SearchNear(check_elem,duo,&airbatV);
  SearchNear(check_elem,duo,&vodovodoV);
  SearchNear(check_elem,duo,&soploV);
  SearchNear(check_elem,duo,&connV);
  SearchNear(check_elem,duo,&kranV);
  SearchNear(check_elem,duo,&extermV);
  //SearchNear(check_elem,duo,&labV); нет смысла искать в метке
  
//  SearchNear(check_elem,duo,&sopObjV);
  
  //ShowAboutWithVectors();///Vector Show
}
/*void CConsrtuctorDoc::ShowAboutWithVectors()
{
  if (AboutDialg==NULL)
  {
    AboutDialg=new CAboutDlg;
    AboutDialg->Create(IDD_ABOUTBOX);
  }
  else
  {
    
    AboutDialg->ShowWindow(SW_SHOW);
    AboutDialg->m_ListBox.ResetContent( );
    VECTOR_OF_PAIR::iterator PairsV_itr;
    int chain=0;
    for (PairsV_itr=PairsV.begin();PairsV_itr!=PairsV.end();++PairsV_itr)
    {
      char buffer[100];
      CString str=itoa(chain,buffer,10);
      str+=" Цепочка  ";      
      PAIR::iterator p_itr;
      for (p_itr=PairsV_itr->begin();p_itr!=PairsV_itr->end();++p_itr)
      {
        CString str1=str;
        str1+="Тип:";
        str1+=itoa(p_itr->place.Type,buffer,10);
        str1+="  Номер:";
        str1+=itoa(p_itr->place.Number,buffer,10);
        str1+="  First:";
        str1+=itoa(p_itr->first,buffer,10);
        str1+="  Second:";
        str1+=itoa(p_itr->second,buffer,10);
        AboutDialg->m_ListBox.AddString(str1);
        
      }
      chain++;
    }
  }
}
*/
//-----------------------------------------------------------------------------------
CDuo CConsrtuctorDoc::GetDuo(CTempl* check_elem)
{
  CDuo duo;
  duo.place.Type=check_elem->GetType();
  duo.place.Number=0;
  switch (duo.place.Type)
  {
  case TUBE:  {duo.place.Number=GetNumber(check_elem,&tubeV); break;}
  case BATTERY: {duo.place.Number=GetNumber(check_elem,&batV); break;}
  case TURN:  {duo.place.Number=GetNumber(check_elem,&turnV); break;}
  case TRINITY: {duo.place.Number=GetNumber(check_elem,&triV); break;}
  case VENTIL:  {duo.place.Number=GetNumber(check_elem,&ventV); break;}
  case NASOS: {duo.place.Number=GetNumber(check_elem,&nasV); break;}
  case DROSSEL: {duo.place.Number=GetNumber(check_elem,&drosV); break;}
  case AIRBATTERY:  {duo.place.Number=GetNumber(check_elem,&airbatV); break;}
  case VODOBATTERY: {duo.place.Number=GetNumber(check_elem,&vodovodoV); break;}
  case SOPLO:{duo.place.Number=GetNumber(check_elem,&soploV); break;}
  case CONNECTOR:{duo.place.Number=GetNumber(check_elem,&connV); break;}
  case KRAN:{duo.place.Number=GetNumber(check_elem,&kranV); break;}
  case LABEL:{duo.place.Number=GetNumber(check_elem,&labV); break;}   
  case EXTERM:{duo.place.Number=GetNumber(check_elem,&extermV); break;}       
//  case SOPLO_OBJECT:{duo.place.Number=GetNumber(check_elem,&sopObjV); break;}
  }
  return duo;
}
//-----------------------------------------------------------------------------------
template <class V>
int CConsrtuctorDoc::GetNumber(CTempl* check_elem,V * vect)
{
  int number=-1;
//  return number;

  V::const_iterator itr; 
  int y=0;
  for(itr=vect->begin();itr!=vect->end();++itr)
    {
      if (check_elem==(CTempl*)&*itr) number=y;
      y++;
    }
  return number;
}
//-----------------------------------------------------------------------------------
CTempl * CConsrtuctorDoc::GetTempl(const CDuo duo)
{
  CTempl * el=NULL;
  switch (duo.place.Type)
  {
  case TUBE:  {if ( (duo.place.Number)<(tubeV.size()) )el=(CTempl*)&tubeV.at(duo.place.Number);break;}
  case BATTERY: {if ( (duo.place.Number)<(batV.size()) ) el=(CTempl*)&batV.at(duo.place.Number);break;}
  case TURN:  {if ( (duo.place.Number)<(turnV.size()) )el=(CTempl*)&turnV.at(duo.place.Number);break;}    
  case TRINITY: { if (  (duo.place.Number)<(triV.size())  )el=(CTempl*)&triV.at(duo.place.Number);break;}       
  case VENTIL:  {if ( (duo.place.Number)<(ventV.size()) )el=(CTempl*)&ventV.at(duo.place.Number);break;}  
  case NASOS: {if ( (duo.place.Number)<(nasV.size()) )el=(CTempl*)&nasV.at(duo.place.Number);break;}  
  case DROSSEL: {if ( (duo.place.Number)<(drosV.size()) )el=(CTempl*)&drosV.at(duo.place.Number);break;}    
  case AIRBATTERY:  {if ( (duo.place.Number)<(airbatV.size()) )el=(CTempl*)&airbatV.at(duo.place.Number);break;}    
  case VODOBATTERY: {if ( (duo.place.Number)<(vodovodoV.size()) )el=(CTempl*)&vodovodoV.at(duo.place.Number);break;}
  case SOPLO:{if ( (duo.place.Number)<(soploV.size()) )el=(CTempl*)&soploV.at(duo.place.Number);break;}   
  case CONNECTOR:{if ( (duo.place.Number)<(connV.size()) )el=(CTempl*)&connV.at(duo.place.Number);break;}   
  case KRAN:{if ( (duo.place.Number)<(kranV.size()) )el=(CTempl*)&kranV.at(duo.place.Number);break;}    
  case LABEL:{if ( (duo.place.Number)<(labV.size()) )el=(CTempl*)&labV.at(duo.place.Number);break;}   
  case EXTERM:{if ( (duo.place.Number)<(extermV.size()) )el=(CTempl*)&extermV.at(duo.place.Number);break;}        
//  case SOPLO_OBJECT:{if ( (duo.place.Number)<(sopObjV.size()) )el=(CTempl*)&sopObjV.at(duo.place.Number);break;}    
  }
  return el;
}
//-----------------------------------------------------------------------------------
template <class V>
void CConsrtuctorDoc::SearchNear(CTempl* check_elem,CDuo duo,V * vect)
{
  for (int i=0;i<check_elem->GetNodeKol();i++)
  { 
    CShemaNode etalon=check_elem->GetNode(i);     
    V::const_iterator itr;
    int num=0;
    for (itr=vect->begin();itr!=vect->end();++itr)
    {
      for (int u=0;u<itr->GetNodeKol();u++)
      {
        if (!itr->IsConnected(u))
        {
          CShemaNode consider=itr->GetNode(u);
          if ((consider.Radius==etalon.Radius)&&(abs(consider.Angle-etalon.Angle)==180))
          {
            if ((consider.X==etalon.X)&&(consider.Y==etalon.Y))
            {                         
              check_elem->Connect(i);
              itr->Connect(u);
              //if (check_elem->GetNodeKol()!=3)
              {
                //эл-т, к которому подсоединяют
                CDuo duosec;
                duosec.place.Type=itr->GetType();
                duosec.place.Number=num;
                duosec.first=u;
                duosec.second=GetSecondPin((CTempl*)&*itr,u);
                //эл-т, который подсоединяют
                duo.second=i;
                duo.first=GetSecondPin(check_elem,i);
                if (!check_elem->IsConnected(duo.first))//если другой конец подсоединяемого эл-та не подсоединен
                {
                  CPoint coord;
                  switch (InVector(duosec,coord))
                  {
                  case 1:{//в начале вектора
                        (PairsV.at(coord.x)).insert((PairsV.at(coord.x)).begin(),duo);
                        break;
                       }
                  case 2:{//в конце вектора
                        //поменять местами концы duo для корректной вставки
                        int fir=duo.second;
                        duo.second=duo.first;
                        duo.first=fir;
                        (PairsV.at(coord.x)).push_back(duo);  
                        break;
                       }
                  default:{
                        PAIR p;
                        p.push_back(duo);
                        p.push_back(duosec);
                        PairsV.push_back(p);
                        p.clear();
                        break;
                      }               
                  }
                }
                else //если другой конец подсоединяемого эл-та уже подсоединен
                {
                  CPoint coord_conne;//
                  int fir=duo.second;//поменять second и first
                  duo.second=duo.first;//
                  duo.first=fir;
                  unsigned char conne=InVector(duo,coord_conne);
                  CPoint coord_sec;

                  VECTOR_OF_PAIR::iterator conne_itr = PairsV.begin()+coord_conne.x;

                  unsigned char sec=InVector(duosec,coord_sec);
//                  VECTOR_OF_PAIR::iterator sec_itr=&(PairsV.at(coord_sec.x));
				  VECTOR_OF_PAIR::iterator sec_itr = PairsV.begin()+coord_sec.x;

                  if ((conne!=0)&&(sec!=0)&&(coord_sec.x==coord_conne.x))//если соединяемые элементы - два конца одного массива
                  {
                    if ((sec==1)&&(conne==2))//|sec+++conne|
                    {
                      (sec_itr->begin())->first=-2;
                      (conne_itr->end()-1)->second=-2;
                    }
                    else //|conne+++sec|
                    {
                      (conne_itr->begin())->first=-2;
                      (sec_itr->end()-1)->second=-2;
                    }
                    break;
                  }
                  if ((sec==1)&&(conne==1))//|+++sec*||conne----|
                  {
                    PAIR p;
                    PAIR::reverse_iterator p_itr;
                    if ( conne_itr->size()<sec_itr->size() )
                    {
                      for (p_itr=conne_itr->rbegin();p_itr!=conne_itr->rend();p_itr++)
                      {
                        //поменять second и first
                        CDuo rev_duo=*(&*p_itr);
                        int tmp=rev_duo.first;
                        rev_duo.first=rev_duo.second;
                        rev_duo.second=tmp;
                        p.push_back(rev_duo);                       
                      }
                      sec_itr->insert( sec_itr->begin(),p.begin(),p.end() );
                      PairsV.erase(conne_itr);
                    }
                    else 
                    {
                      for (p_itr=sec_itr->rbegin();p_itr!=sec_itr->rend();p_itr++)
                      {
                        //поменять second и first
                        CDuo rev_duo=*(&*p_itr);
                        int tmp=rev_duo.first;
                        rev_duo.first=rev_duo.second;
                        rev_duo.second=tmp;
                        p.push_back(rev_duo);
                      }
                      conne_itr->insert( conne_itr->begin(),p.begin(),p.end() );
                      PairsV.erase(sec_itr);
                    }
                    p.clear();
                    break;
                  }
                  if ((sec==2)&&(conne==2))//|+++sec||*conne----|
                  {
                    PAIR p;
                    PAIR::reverse_iterator p_itr;
                    if ( conne_itr->size()<sec_itr->size() )
                    {
                      for (p_itr=conne_itr->rbegin();p_itr!=conne_itr->rend();p_itr++)
                      {
                        //поменять second и first
                        CDuo rev_duo=*(&*p_itr);
                        int tmp=rev_duo.first;
                        rev_duo.first=rev_duo.second;
                        rev_duo.second=tmp;
                        p.push_back(rev_duo);
                      }
                      sec_itr->insert( sec_itr->end(),p.begin(),p.end() );
                      PairsV.erase(conne_itr);
                    }
                    else 
                    {
                      for (p_itr=sec_itr->rbegin();p_itr!=sec_itr->rend();p_itr++)
                      {
                        //поменять second и first
                        CDuo rev_duo=*(&*p_itr);
                        int tmp=rev_duo.first;
                        rev_duo.first=rev_duo.second;
                        rev_duo.second=tmp;
                        p.push_back(rev_duo);                       
                      }
                      conne_itr->insert( conne_itr->end(),p.begin(),p.end() );
                      PairsV.erase(sec_itr);
                    }
                    p.clear();
                    break;
                  }
                  if ((sec==2)&&(conne==1))//|+++sec||conne----|
                  {
                    if ( conne_itr->size()<sec_itr->size() )
                    {
                      sec_itr->insert( sec_itr->end(),conne_itr->begin(),conne_itr->end() );
                      PairsV.erase(conne_itr);
                    }
                    else 
                    {
                      conne_itr->insert( conne_itr->begin(),sec_itr->begin(),sec_itr->end() );
                      PairsV.erase(sec_itr);
                    }
                    break;
                  }
                  if ((sec==1)&&(conne==2))//|----conne||sec+++|
                  {
                    if ( conne_itr->size()<sec_itr->size() )                          
                    {
                      sec_itr->insert( sec_itr->begin(),conne_itr->begin(),conne_itr->end() );
                      PairsV.erase(conne_itr);                                                                    
                    }
                    else 
                    {
                      conne_itr->insert( conne_itr->end(),sec_itr->begin(),sec_itr->end() );
                      PairsV.erase(sec_itr);
                    }
                    break;
                  }
                  if(sec==0)//подсоединение идет одного эл-та sec
                  {
                    if (conne==1)//|sec||conne----|
                    {
                      //поменять second и first
                      int tmp=duosec.first;
                      duosec.first=duosec.second;
                      duosec.second=tmp;
                      conne_itr->insert( conne_itr->begin(),duosec);
                    }
                    else //|----conne||sec|
                      conne_itr->insert( conne_itr->end(),duosec);
                  }
                  
                }
              }
              break;
            }
          }
        }
      }
      num++;
    }
  }
}
//-----------------------------------------------------------------------------------
int CConsrtuctorDoc::GetSecondPin(CTempl* check_elem,int i)
{
  int sec=-1;
  switch (i)
  {
    case 0: {if(check_elem->GetNodeKol()>1)sec=1; break;}
    case 1: {sec=0; break;}
    case 2: {if(check_elem->GetNodeKol()>3)sec=3; break;}
    case 3: {sec=2; break;}
  }
  if (check_elem->GetNodeKol()==3)sec=-1;
  return sec;
}
//-----------------------------------------------------------------------------------
unsigned char CConsrtuctorDoc::InVector(CDuo duo,CPoint &coord)
{
  long ext=0;
  VECTOR_OF_PAIR::iterator PairsV_itr;
  for (PairsV_itr=PairsV.begin();PairsV_itr!=PairsV.end();++PairsV_itr)
  {
    PAIR::iterator p_itr;
    //проверить конец массива
    p_itr=PairsV_itr->end()-1;
    long inter=PairsV_itr->size()-1;
    if ((p_itr->place.Type==duo.place.Type)&&
      (p_itr->place.Number==duo.place.Number)&&
      (p_itr->second==duo.first)&&        
      (p_itr->first==duo.second))
    {
      coord.x=ext;
      coord.y=inter;
      return 2;
    }
    //проверить начало массива
    p_itr=PairsV_itr->begin();
    inter=0;
    if ((p_itr->place.Type==duo.place.Type)&&
      (p_itr->place.Number==duo.place.Number)&&
      (p_itr->second==duo.second)&&       
      (p_itr->first==duo.first))
    {
      coord.x=ext;
      coord.y=inter;
      return 1;
    }
    ext++;
  }
  coord.x=0;
  coord.y=0;
  return 0;
}
//-----------------------------------------------------------------------------------
void CConsrtuctorDoc::DisconnectElem(CDuo * duo_del,bool first,bool second)
{
  switch (duo_del->place.Type)
  {
    case TUBE:{DisconnectFromType(duo_del,first,second,&tubeV);break;}
    case BATTERY:{DisconnectFromType(duo_del,first,second,&batV);break;}
    case TURN:{DisconnectFromType(duo_del,first,second,&turnV);break;}
    case TRINITY:{DisconnectFromType(duo_del,first,second,&triV);break;}
    case VENTIL:{DisconnectFromType(duo_del,first,second,&ventV);break;}
    case NASOS:{DisconnectFromType(duo_del,first,second,&nasV);break;}
    case DROSSEL:{DisconnectFromType(duo_del,first,second,&drosV);break;}
    case AIRBATTERY:{DisconnectFromType(duo_del,first,second,&airbatV);break;}
    case VODOBATTERY:{DisconnectFromType(duo_del,first,second,&vodovodoV);break;}
    case SOPLO:{DisconnectFromType(duo_del,first,second,&soploV);break;}
    case CONNECTOR:{DisconnectFromType(duo_del,first,second,&connV);break;}
    case KRAN:{DisconnectFromType(duo_del,first,second,&kranV);break;}
    //case LABEL:{DisconnectFromType(duo_del,first,second,&labV);break;} Нет в анализе метки
    case EXTERM:{DisconnectFromType(duo_del,first,second,&extermV);break;}

//    case SOPLO_OBJECT:{DisconnectFromType(duo_del,first,second,&sopObjV);break;}
  }
}
//-----------------------------------------------------------------------------------
template <class V>
void CConsrtuctorDoc::DisconnectFromType(CDuo * duo_del,bool first,bool second,V * vect)
{
  if (first)  (vect->at(duo_del->place.Number)).DisConnect(duo_del->first);
  if (second) (vect->at(duo_del->place.Number)).DisConnect(duo_del->second);
}
//-----------------------------------------------------------------------------------
void CConsrtuctorDoc::DisconnectFromPair(CTempl* check_elem,CDuo duo)
{
  if (check_elem->GetNodeKol()==3)//элемент с тремя выводами
  {
again:
    VECTOR_OF_PAIR::iterator PairsV_itr;
    for (PairsV_itr=PairsV.begin();PairsV_itr!=PairsV.end();++PairsV_itr)
      {
        PAIR::iterator p_itr;
        for (p_itr=PairsV_itr->begin();p_itr!=PairsV_itr->end();++p_itr)
        {
          if (p_itr->place.Type==duo.place.Type)
          {
            if (p_itr->place.Number==duo.place.Number)
            {
              if (p_itr->first==-1)//|p_itr++++++|
              {
                p_itr++;//указывает на следующий элемент
                DisconnectElem(&*p_itr,1,0);
                p_itr--;//указывает на искомый элемент
              }
              if (p_itr->second==-1)//|++++++p_itr|
              {
                p_itr--;//указывает на предыдущий элемент
                DisconnectElem(&*p_itr,0,1);
                p_itr++;//указывает на искомый элемент
              }             
              if (PairsV_itr->size()<=2)
              {
                PairsV.erase(PairsV_itr);
                goto again;
              }
              else
              {
                PairsV_itr->erase(p_itr);
                break;
              }
            }
          }
        }
      }   
    for (int i=0;i<check_elem->GetNodeKol();i++) check_elem->DisConnect(i);
    return;
  } 
  for (int i=0;i<check_elem->GetNodeKol();i++)
  {
    if (check_elem->IsConnected(i))
    {
      VECTOR_OF_PAIR::iterator PairsV_itr;
      for (PairsV_itr=PairsV.begin();PairsV_itr!=PairsV.end();++PairsV_itr)
      {
        PAIR::iterator p_itr;
        for (p_itr=PairsV_itr->begin();p_itr!=PairsV_itr->end();++p_itr)
        {
          if (p_itr->place.Type==duo.place.Type)
          {
            if (p_itr->place.Number==duo.place.Number)
            {
              duo.first=i;
              duo.second=GetSecondPin(check_elem,i);
              if (  ( ((p_itr->first==duo.second)&&(p_itr->second==duo.first))||
                    ((p_itr->first==duo.first)&&(p_itr->second==duo.second)) )||
                  
                  ( ((p_itr->second==-2)&&((p_itr->first==duo.second)||(p_itr->first==duo.first)))||
                    ((p_itr->first==-2)&&((p_itr->second==duo.second)||(p_itr->second==duo.first))) )  )
              {
                PAIR p; //буфер для сохранения остаточной части массива и затем вставки ее в конец PairsV
                p.clear();
                if ( (PairsV_itr->begin()->first==-2)||(PairsV_itr->back().second==-2) )//если последовательность была замкнута
                {
                    unsigned char flag=0;
                  if (p_itr->first==-2) flag=1;
                  if (p_itr->second==-2) flag=2;
                  PairsV_itr->begin()->first=GetSecondPin(GetTempl(*PairsV_itr->begin()),PairsV_itr->begin()->second);
                  PairsV_itr->back().second=GetSecondPin(GetTempl(PairsV_itr->back()),PairsV_itr->back().first);                  
                  if (flag==1)DisconnectElem(&PairsV_itr->back(),0,1);//искомый элемент находится в начале цикла
                  if (flag==2)DisconnectElem(&*PairsV_itr->begin(),1,0);//искомый элемент находится в конце цикла
                  if ( (p_itr!=PairsV_itr->begin())&&(&*p_itr!=(&PairsV_itr->back())) )//искомый элемент находится посередине цикла
                  {
                    p_itr++; //указывает на последующий элемент
                    DisconnectElem(&*p_itr,1,0);
                    p_itr-=2; //указывает на предыдущий элемент
                    DisconnectElem(&*p_itr,0,1);
                    p_itr++; //указывает на искомый элемент
                    if ( (p_itr-PairsV_itr->begin())>((&PairsV_itr->back())-&*p_itr) )//искомый элемент ближе к концу цикла
                    {
                      p.insert(p.begin(),(p_itr+1),PairsV_itr->end());
                      PairsV_itr->erase(p_itr,PairsV_itr->end());
                      PairsV_itr->insert(PairsV_itr->begin(),p.begin(),p.end());                      
                    }
                    else 
                    {
                      p.insert(p.begin(),PairsV_itr->begin(),p_itr);
                      PairsV_itr->erase(PairsV_itr->begin(),p_itr+1);
                      PairsV_itr->insert(PairsV_itr->end(),p.begin(),p.end());
                    }
                    check_elem->DisConnect(i);
                    break;
                  }
                    
                }
                p_itr++;//указывает на последующий элемент
                if (p_itr!=PairsV_itr->end())//нужный эл-т не последний
                {
                  p_itr++;
                  if (p_itr!=PairsV_itr->end())//нужный эл-т совсем не последний (надо перекидать массив)
                  {
                    p_itr--;
                    p.insert(p.begin(),p_itr,PairsV_itr->end());
                    p_itr++;
                  }
                  p_itr-=2;//p_itr указывает на искомомый эл-т
                  PAIR::iterator p_begin;
                  p_begin=PairsV_itr->begin();
                  p_begin++;//второй эл-т массива
                  if( p_itr>p_begin) //нужный эл-т как минимум третий в массиве, причем есть эл-ты дальше
                  {
                    p_itr--;//p_itr указывает на предыдущий эл-т
                    DisconnectElem(&*p_itr,0,1);
                    p_itr+=2;//p_itr указывает на следующий эл-т
                    DisconnectElem(&*p_itr,1,0);
                    p_itr--;//p_itr указывает на искомомый эл-т
                    DisconnectElem(&*p_itr);
                    PairsV_itr->erase(p_itr,PairsV_itr->end());
                  }
                  else //нужный эл-т первый или второй в массиве, причем есть эл-ты дальше
                  {
                    p_itr--;//p_itr указывает на предыдущий эл-т                    
                    if (p_itr>=PairsV_itr->begin()) DisconnectElem(&*p_itr);
                    p_itr++;//p_itr указывает на искомомый эл-т
                    DisconnectElem(&*p_itr);
                    p_itr++;//p_itr указывает на следующий эл-т
                    DisconnectElem(&*p_itr,1,0);
                    PairsV.erase(PairsV_itr);                   
                  }
                  if (!p.empty()) PairsV.push_back(p);
                  return;
                }
                else //нужный эл-т последний
                {
                  p_itr--;//p_itr указывает на искомый эл-т
                  PAIR::iterator p_begin;
                  p_begin=PairsV_itr->begin();
                  p_begin++;//второй эл-т массива
                  if( p_itr>p_begin)//нужный эл-т как минимум третий в массиве
                  {
                    DisconnectElem(&*p_itr);                    
                    p_itr--;//p_itr указывает на предыдущий эл-т
                    DisconnectElem(&*p_itr,0,1);
                    PairsV_itr->pop_back();

                  }
                  else //нужный эл-т первый или второй в массиве, причем последний
                  {
                    PAIR::iterator del_itr;
                    for (del_itr=PairsV_itr->begin();del_itr!=PairsV_itr->end();++del_itr)
                    {
                      DisconnectElem(&*del_itr);
                    }
                    PairsV.erase(PairsV_itr);                   
                  }
                  return;
                }
              }
            }
          }
        }
        check_elem->DisConnect(i);
      }
    }
  }
}
//-----------------------------------------------------------------------------------
void CConsrtuctorDoc::DecreaseFromPair(CDuo duoDel)
{
  VECTOR_OF_PAIR::iterator PairsV_itr;
  int chain=0;
  for (PairsV_itr=PairsV.begin();PairsV_itr!=PairsV.end();++PairsV_itr)
  {
    PAIR::iterator p_itr;
    for (p_itr=PairsV_itr->begin();p_itr!=PairsV_itr->end();++p_itr)
    {
      if (p_itr->place.Type==duoDel.place.Type)
      {
        if (p_itr->place.Number>duoDel.place.Number)
        {
          p_itr->place.Number--;    
        }
      }
    }   
  }
}
//-----------------------------------------------------------------------------------
bool CConsrtuctorDoc::NodesConnected()
{
  if (!CheckElemConnected(&tubeV))return false;
  if (!CheckElemConnected(&batV))return false;
  if (!CheckElemConnected(&turnV))return false;
  if (!CheckElemConnected(&triV))return false;
  if (!CheckElemConnected(&ventV))return false;
  if (!CheckElemConnected(&nasV))return false;
  if (!CheckElemConnected(&drosV))return false;
  if (!CheckElemConnected(&airbatV))return false;
  if (!CheckElemConnected(&vodovodoV))return false;
  if (!CheckElemConnected(&soploV))return false;
  if (!CheckElemConnected(&connV))return false;
  if (!CheckElemConnected(&kranV))return false;
  //if (!CheckElemConnected(&labV))return false; НЕТ там Метки
  if (!CheckElemConnected(&extermV))return false;

//  if (!CheckElemConnected(&sopObjV))return false;
  return true;
}
//-----------------------------------------------------------------------------------
template <class V>
bool CConsrtuctorDoc::CheckElemConnected(V * vect)
{
  if (vect->empty())return true;
  V::const_iterator itr;
  for (itr=vect->begin();itr!=vect->end();itr++)
  {
    for (int i=0;i<=itr->GetNodeKol();i++)
    {
      if (!itr->IsConnected(i))return false; 
    }
  }
  return true;
}
//----------------Analyse-----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//------------работа с группой------------------------------------------------------------
void CConsrtuctorDoc::SwitchGroup(long group,bool ON)
{
  SwitchGroupInVector(&ventV,group,ON);
  SwitchGroupInVector(&nasV,group,ON);
  SwitchGroupInVector(&airbatV,group,ON);
  SwitchGroupInVector(&kranV,group,ON); 
}
//-----------------------------------------------------------------------------------
template <class V> 
void CConsrtuctorDoc::SwitchGroupInVector(V * vect,long group,bool ON)
{
  V::iterator itr;
  for (itr=vect->begin();itr!=vect->end();itr++)
  {
    long grp=itr->GetGroup().group;
    if (grp==group) 
    {
      if (itr->sw->ON()!=ON) itr->sw->Click();
    }
  }
}
//-----------------------------------------------------------------------------------
LRESULT CConsrtuctorDoc::OnAction(WPARAM wParam, LPARAM lParam)
{
  switch (lParam)
  {
  case NASOS_OPENED :
  case AIRBAT_OPENED:
    {
      if (!IsVentOrKranInGroup((long)wParam)) 
      {
        OpenGroup((long)wParam);
      }
      break;
    }
  case NASOS_CLOSED  :
  case AIRBAT_CLOSED :
    {
      if (!IsVentOrKranInGroup((long)wParam)) 
      {
          CloseGroup((long)wParam);
      }
      break;
    }
  case VENTIL_OPENED :
    {
      OpenGroup((long)wParam);
      break;
    }
  case VENTIL_CLOSED :
    {
      CloseGroup((long)wParam);
      break;
    }
  }
  return TRUE;
}
//-----------------------------------------------------------------------------------
void CConsrtuctorDoc::OpenGroup(const long group_num)
{
  ActionGroupIn(&tubeV,group_num,ONLY_OPEN_OBJECT); 
  ActionGroupIn(&airbatV,group_num,ONLY_OPEN_OBJECT); 
  ActionGroupIn(&ventV,group_num,ONLY_OPEN_OBJECT); 
  ActionGroupIn(&nasV,group_num,ONLY_OPEN_OBJECT);  
  ActionGroupIn(&drosV,group_num,ONLY_OPEN_OBJECT); 
  ActionGroupIn(&soploV,group_num,ONLY_OPEN_OBJECT);  
  ActionGroupIn(&kranV,group_num,ONLY_OPEN_OBJECT); 
  //ActionGroupIn(&sopObjV,group_num,ONLY_OPEN_OBJECT); ????? 
}
//-----------------------------------------------------------------------------------
void CConsrtuctorDoc::CloseGroup(const long group_num)
{
  ActionGroupIn(&tubeV,group_num,ONLY_CLOSE_OBJECT);  
  ActionGroupIn(&airbatV,group_num,ONLY_CLOSE_OBJECT);  
  ActionGroupIn(&ventV,group_num,ONLY_CLOSE_OBJECT);  
  ActionGroupIn(&nasV,group_num,ONLY_CLOSE_OBJECT); 
  ActionGroupIn(&drosV,group_num,ONLY_CLOSE_OBJECT);  
  ActionGroupIn(&soploV,group_num,ONLY_CLOSE_OBJECT); 
  ActionGroupIn(&kranV,group_num,ONLY_CLOSE_OBJECT);  
  //ActionGroupIn(&sopObjV,group_num,ONLY_CLOSE_OBJECT);  ?????   
}
//-----------------------------------------------------------------------------------
BOOL CConsrtuctorDoc::IsVentOrKranInGroup(const long group_num)
{
  if (IsInGroup(&ventV,group_num))return TRUE;
  if (IsInGroup(&kranV,group_num))return TRUE;
  return FALSE;
}
//-----------------------------------------------------------------------------------
template <class V>
BOOL CConsrtuctorDoc::IsInGroup(V * vect,const long group_num)
{
  V::iterator  itr;
  for (itr=vect->begin();itr!=vect->end();itr++)
  {
    if (itr->GetGroup().group==group_num)return TRUE;
  }
  return FALSE;
}
//-----------------------------------------------------------------------------------
template <class V>
void CConsrtuctorDoc::ActionGroupIn(V * vect,const long group_num,const long Param)
{
  V::iterator  itr;
  for (itr=vect->begin();itr!=vect->end();itr++)
  {
    if (itr->GetGroup().group==group_num)
    {
      if (Param==ONLY_CLOSE_OBJECT) 
      {
//        if (itr->GetGroupON())
        {
          itr->SetGroupON(false);
          SendActionData((CTempl * )&*itr,Param);
        }
      }
      else 
      {
//        if (!itr->GetGroupON())
        {
          itr->SetGroupON(true);
          SendActionData((CTempl * )&*itr,Param);
        }
      }

    }
  } 
}
//-----------------------------------------------------------------------------------
void CConsrtuctorDoc::SendActionData(CTempl * vect,const long Param)
{
  CCustomView* m_pView=GetActiveView();
  COMMON_DATA cd=vect->GetCommonParam();
  switch (cd.Type)
  {
  case VENTIL : 
  case NASOS : 
  case AIRBATTERY : 
  case KRAN : 
    {
      m_pView->duo_selected=GetDuo(vect);
      m_pView->SendData(Param);
      break;
    }
  }
}
//------------работа с группой------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//------------загрузка данных при выполнении отдельно от проекта -------------------------
/*
bool CSheme::Open(CString FileName)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  
  CFile fl;
  if (!fl.Open(FileName,CFile::modeRead)) return false;
  CArchive ar(&fl,CArchive::load);
  const CFile* fp = ar.GetFile();
  CString str=fp->GetFileName( );
  CString str1=str.Right(3);
  str1.MakeUpper();
  if (str1=="XML") 
  {
    m_pDoc->LoadXML(ar);
    sc.CY=sc.OriginalStep*2*sc.YL2;
    sc.CX=sc.OriginalStep*2*sc.XL2;
    return true;
  }
  m_pDoc->ClearAll();
  UINT size;
  size=ar.Read(&sc,sizeof(sc));
  if ( size!=sizeof(sc) )return false;
  sc.CY=sc.OriginalStep*2*sc.YL2;
  sc.CX=sc.OriginalStep*2*sc.XL2;
  ///
  sc.BackColor2=sc.BackColor;
  ///
  size=ar.Read(&font_data,sizeof(font_data));   
  if ( size!=sizeof(font_data) )return false;
  if (!m_pDoc->LoadAr(&m_pDoc->tubeV,&objects[1],(CTube*)NULL,ar))return false;
  if (!m_pDoc->LoadAr(&m_pDoc->batV,&objects[2],(CBattery*)NULL,ar))return false;
  if (!m_pDoc->LoadAr(&m_pDoc->turnV,&objects[3],(CTurn*)NULL,ar))return false;
  if (!m_pDoc->LoadAr(&m_pDoc->triV,&objects[4],(CTrinity*)NULL,ar))return false;
  if (!m_pDoc->LoadAr(&m_pDoc->ventV,&objects[5],(CVentil*)NULL,ar))return false;
  if (!m_pDoc->LoadAr(&m_pDoc->nasV,&objects[6],(CNasos*)NULL,ar))return false;
  if (!m_pDoc->LoadAr(&m_pDoc->drosV,&objects[7],(CDrossel*)NULL,ar))return false;
  if (!m_pDoc->LoadAr(&m_pDoc->airbatV,&objects[8],(CAirBattery*)NULL,ar))return false;
  if (!m_pDoc->LoadAr(&m_pDoc->vodovodoV,&objects[9],(CVodoVodo*)NULL,ar))return false;
  if (!m_pDoc->LoadAr(&m_pDoc->soploV,&objects[10],(CSoplo*)NULL,ar))return false;
  if (!m_pDoc->LoadAr(&m_pDoc->connV,&objects[11],(CConnector*)NULL,ar))return false;
  if (!m_pDoc->LoadAr(&m_pDoc->kranV,&objects[12],(CKran*)NULL,ar))return false;
//  if (!m_pDoc->LoadAr(&m_pDoc->sopObjV,&objects[13],(CSoploObject*)NULL,ar))return false;
  ar.Close();
  fl.Close();
  CompileList();  
  return true;
}
template <class V,class Comp>
bool CSheme::LoadAr(V * vect,ELEMENT_DATA *str,Comp * comp,CArchive &ar)
{
  UINT kol;
  UINT size;
  size=ar.Read(&kol,sizeof(kol));
  if ( size!=sizeof(kol) )return false;
  //ar>>kol;
  for (UINT it=0;it<kol;it++)
  {
    size=ar.Read(str,sizeof(*str));
    if ( size!=sizeof(*str) )return false;
      //  AddElem(str,comp,vect);
    comp= new Comp(str,1);
    comp->ConnectionCreate(this);
    vect->push_back(*comp);
    delete comp;
    Analyse(&vect->back()); 
      //  AddElem(str,comp,vect);
    
  }
  return true;
}*/
//------------загрузка данных при выполнении отдельно от проекта -------------------------
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
///ADD//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
CConsrtuctorDoc::CConsrtuctorDoc(CCustomView* _mCustomView) 
{
  mCustomView=_mCustomView;
  ClearAll();
  PairsV.clear();
}
