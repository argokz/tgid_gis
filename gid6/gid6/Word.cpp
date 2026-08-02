// geoView.cpp : implementation of the CGidrView class
//

#include "stdafx.h"
#include "gid6.h"

#include "gid6Doc.h"
#include "gidrView.h"

#include "win.h"
#include "geofile.h"

#include "maptabs.h"

#include "math.h"


#include "ado.h"


#include "grid/msword8.h"

static COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
static COleVariant covTrue((short)TRUE), covFalse((short)FALSE);


bool closeWord(const char* doc_name);

Range SetText(Table& oTbl, int row, int col, LPCTSTR str);
Range SetText(Table& oTbl, int row, int col, int v);
Range SetText0(Table& oTbl, int row, int col, int v);
Range SetText(Table& oTbl, int row, int col, double v);


//int CGidrView::WordKorroz(const char *doc_name) 
int IsprDoc(CString doc_name)
{
  Selection oSel;
  Paragraphs oPars;
  Paragraph oPar;
  PageSetup oPgStp;
  Range oCntnt;
  Frames oFrms;
  Frame oFrm;
  Range oRange;
  Find oFind;

  CString q;


//  CString tn = transl("Индикатор_коррозии_по_годам");

  if (closeWord(doc_name)) return 0;

//  if (closeWord(doc_name)) return 0;

  CWaitCursor cur;

//   COleException *e = new COleException;

  _Application app;  // app - это объект _Application, т.е. Word 9

   try {
    if(!app.CreateDispatch("Word.Application")) //запустить сервер
    {
      AfxMessageBox(_TR("Ошибка при старте Wordа!"), MB_OK|MB_ICONINFORMATION);
      return 0;
    }

//    app.SetVisible(TRUE); //и сделать его видимым

    Documents oDocs;
    _Document oDoc;
    //наша коллекция документов
    oDocs = app.GetDocuments();
    //добавить к ней новый документ
    //Внимание! Если у вас Word 97 - то строчка будет такая:
//    oDocs.Add(covOptional,covOptional);       //97
  //  oDocs.Add(covOptional,covOptional,covOptional,covOptional); //2000
      //и получить его как экзепляр коллекции с номером 1
//    oDoc = oDocs.Item(COleVariant(long(1)));
    //активизировать документ
//    oDoc.Activate();

//    oSel = app.GetSelection();


    oDocs.Open(COleVariant(doc_name), covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional);
    oDoc = oDocs.Item(COleVariant(long(1)));
    oDoc.Activate();
    oPgStp = oDoc.GetPageSetup();

    oSel = app.GetSelection();

    oFind = oSel.GetFind();

    Replacement repl = oFind.GetReplacement();
    
    oFind.ClearFormatting();


    Tables oTbls;
    Table oTbl1, oTbl2;
    Range oRange;

    oTbls = oDoc.GetTables();



    COleVariant f(0L, VT_BOOL);
    COleVariant t(1L, VT_BOOL);
    COleVariant nul(1L, VT_BOOL);


    oDoc.SaveAs(COleVariant(doc_name), covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional);


    app.SetVisible(TRUE); //и сделать его видимым
  }
  catch (COleDispatchException * e) {
    app.SetVisible(TRUE); //и сделать его видимым
    AfxMessageBox(e->m_strDescription, MB_OK|MB_ICONINFORMATION);
    e->Delete();
  }

  HWND wnd = ::FindWindowEx(0, 0, "OpusApp", 0);
  
  if (wnd) {
    ::ShowWindow(wnd, SW_MINIMIZE);
    ::ShowWindow(wnd, SW_RESTORE);
  }
  return 1;
}
