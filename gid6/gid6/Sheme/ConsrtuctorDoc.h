#pragma once


#include "Tube.h"
#include "vector"

using namespace std;

typedef vector < CTube > VECTOR;
typedef vector < CBattery > BAT_VECTOR;
typedef vector < CTurn > TURN_VECTOR;
typedef vector < CTrinity > TRINITY_VECTOR;
typedef vector < CVentil > VENTIL_VECTOR;
typedef vector < CNasos > NASOS_VECTOR;
typedef vector < CDrossel > DROSSEL_VECTOR;
typedef vector < CAirBattery > AIRBAT_VECTOR;
typedef vector < CVodoVodo > VODOVODO_VECTOR;
typedef vector < CSoplo > SOPLO_VECTOR;
typedef vector < CConnector > CONNECTOR_VECTOR;
typedef vector < CKran > KRAN_VECTOR;
typedef vector < CLabel > LABEL_VECTOR;
typedef vector < CExtTermo > EXTERM_VECTOR;
//typedef vector < CSoploObject > SOPLO_OBJ_VECTOR;

typedef vector <int> STACK;
typedef vector <CDuo> PAIR;
typedef vector <PAIR> VECTOR_OF_PAIR;
//typedef multimap < int,CShemaNode,less<int> > Vertex_map;


class CConsrtuctorDoc :public CDocument
{
	friend class CCustomView;	
	friend class CSheme;	
protected: // create from serialization only
	

// Attributes
public:
// Operations
public:
	void ClearAll();
	void SetZeroFocusAll();
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConsrtuctorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CConsrtuctorDoc();
	CString Datastring;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	
	
	template <class V> void SetZeroFocus(V * vect);
	
	

////////////////////////////////////////////////////////////////////////////////////////
//------------добавление/удаление------------------------------------------------------------
	CDuo AddElement(int type,ELEMENT_DATA * el_dat);

	void DeleteElement(const CDuo Duo, bool deleting_in_group);
	void PasteElement(CDuo &Duo, bool paste_in_group);
	template <class Comp,class V> CDuo AddElem(ELEMENT_DATA * str,Comp * comp,V * vect,bool FromBounds=true);	
	template <class V> void DeleteElem(const CDuo Duo,V * vect);
	template <class V,class Comp> void PasteElem(CDuo &Duo,Comp * comp,V * vect);

//------------добавление/удаление------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//------------изменение родительского окна------------------------------------------------------------
	void RemakeConnection(CTemplWnd * wnd);
	template <class V> void RemakeConnectionIn(CTemplWnd * wnd,V * vect);

//------------изменение родительского окна------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////

	//Сохранение вектора
	template <class V> void SaveAr(V * vect,CArchive& ar);
	template <class V,class Comp> void LoadAr(V * vect,Comp * comp,CArchive& ar);

////////////////////////////////////////////////////////////////////////////////////////
//------------работа с группой------------------------------------------------------------
	template <class V> void SwitchGroupInVector(V * vect,long group,bool ON);
	void SwitchGroup(long group,bool ON);
	void OpenGroup(const long group_num);
	void CloseGroup(const long group_num);
	template <class V> BOOL IsInGroup(V * vect,const long group_num);
	BOOL IsVentOrKranInGroup(const long group_num);	
	template <class V> void ActionGroupIn(V * vect,const long group_num,const long Param);
	void SendActionData(CTempl * vect,const long Param);	
	LRESULT OnAction(WPARAM wParam, LPARAM lParam);
//------------работа с группой------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////

			
	//---------------XML-------------------------------------------------
	void SaveXML(CArchive& ar);
	void LoadXML(CArchive& ar);
	//---WriteXML------------------------------------------------------
	void WriteStart(CArchive& ar,CString name);
	void WriteEnd(CArchive& ar,CString name);
	void WriteScreen(CArchive& ar,SCREEN * sc);
	void WriteFontData(CArchive& ar,FONT_DATA * font_data);
	void WriteColor(CArchive& ar,CString name,TCOLOR * color);
	void WriteLF(CArchive& ar,LOGFONT * lf);
	void WriteRectan(CArchive& ar,CString name,RECTAN * bound);
	void WriteGroup(CArchive& ar,CString name,GROUP_DATA * grd);
	void WriteObject(CArchive& ar,ELEMENT_DATA * object);
	void WriteElement(CArchive& ar,CString name,long value);
	void WriteElement(CArchive& ar,CString name,double value);
	void WriteElement(CArchive& ar,CString name,CString value);
	template <class V> void SaveXMLAr(V * vect,CArchive& ar,long type);
	//---ReadXML------------------------------------------------------
	BOOL ReadScreen(CArchive& ar,SCREEN * sc);
	BOOL ReadFontData(CArchive& ar,FONT_DATA * font_data);
	BOOL ReadLF(CArchive& ar,LOGFONT * lf);
	BOOL ReadStart(CArchive& ar,CString name);
	BOOL ReadColor(CArchive& ar,CString name,TCOLOR * color);
	BOOL ReadRectan(CArchive& ar,CString name,RECTAN * bound);
	BOOL ReadGroup(CArchive& ar,CString name,GROUP_DATA * grd);
	BOOL ReadObject(CArchive& ar,ELEMENT_DATA * object);	
	BOOL ReadElement(CArchive& ar,CString name,long &value);
	BOOL ReadElement(CArchive& ar,CString name,double &value);
	BOOL ReadElementString(CArchive& ar,CString name,CString &dest);
	BOOL LoadXMLObject(CArchive& ar);
	BOOL LoadXMLAr(CArchive& ar);
	//---------------XML-------------------------------------------------
///////////////	
	VECTOR tubeV;
	BAT_VECTOR batV;
	TURN_VECTOR turnV;
	TRINITY_VECTOR triV;
	VENTIL_VECTOR ventV;
	NASOS_VECTOR nasV;
	DROSSEL_VECTOR drosV;
	AIRBAT_VECTOR airbatV;
	VODOVODO_VECTOR  vodovodoV;
	SOPLO_VECTOR soploV;
	CONNECTOR_VECTOR connV;
	KRAN_VECTOR kranV;
	LABEL_VECTOR labV;
	EXTERM_VECTOR extermV;
//	SOPLO_OBJ_VECTOR sopObjV;
//////////////////
//////////////////
//	Vertex_map Nodes;
	VECTOR_OF_PAIR PairsV;	
//////////////////
	//////////////////
	//---------------Analyse-------------------------------------------------
	void Analyse(CTempl* check_elem);
	unsigned char InVector(CDuo duo,CPoint &coord);
	void DisconnectFromPair(CTempl* check_elem,CDuo duo);
	void DecreaseFromPair(CDuo duoDel);
	void DisconnectElem(CDuo * duo_del,bool first=1,bool second=1);	
	CDuo GetDuo(CTempl* check_elem);
	CTempl * GetTempl(const CDuo duo);
	//поиск ближайших узлов к узлам элемента check_elem из типа vect и соединение их(если возможно)
	template <class V> void SearchNear(CTempl* check_elem,CDuo duo,V * vect);
	//отсоединение от vect элемента типа vect и со стуктурой duo_del
	template <class V> void DisconnectFromType(CDuo * duo_del,bool first,bool second,V * vect);
	//GetNumber - возвращает номер check_elem в последовательности vect
	template <class V> int GetNumber(CTempl* check_elem,V * vect);
	int GetSecondPin(CTempl* check_elem,int i);
public:
	bool NodesConnected();
protected:
	template <class V> bool CheckElemConnected(V * vect);	
	//---------------Analyse-------------------------------------------------
	//////////////////
	//---------------Add-------------------------------------------------
	//---------------Add-------------------------------------------------
	CCustomView * mCustomView;
	CCustomView* GetActiveView();
	CConsrtuctorDoc(CCustomView* _mCustomView);	
	//---------------Add-------------------------------------------------
	//---------------Add-------------------------------------------------
	//{{AFX_MSG(CConsrtuctorDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
