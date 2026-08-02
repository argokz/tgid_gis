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


bool closeWord(const char* doc_name)
{
    HWND wnd = ::FindWindowEx(0, 0, "OpusApp", 0);

    if (wnd) {
        CString cap;
        CString name = GetTitle(doc_name);

        CWnd::FromHandle(wnd)->GetWindowText(cap);
        if (cap.Find(name) != -1) {
            ::ShowWindow(wnd, SW_MINIMIZE);
            ::ShowWindow(wnd, SW_RESTORE);
            return true;
        }
    }
    return false;
}

Range SetText(Table& oTbl, int row, int col, LPCTSTR str)
{
    Cell oCell;
    Range oRange;

    oCell = oTbl.Cell(row, col);
    //  oCell.SetBorders();
    oRange = oCell.GetRange();
    oRange.SetBold(0);
    oRange.SetText(str);

    return oRange;
}

Range SetTextBorder(Table& oTbl, int row, int col, LPCTSTR str)
{
    Cell oCell;
    Range oRange;

    oCell = oTbl.Cell(row, col);

    Borders oBorders = oCell.GetBorders();

    oBorders.SetEnable(1);

    oCell.SetBorders(oBorders);

    oRange = oCell.GetRange();
    oRange.SetBold(0);
    oRange.SetText(str);

    return oRange;
}


Range AppendText(Range &range, LPCTSTR str)
{
    long start = range.GetStart();
    long end = range.GetEnd();
    range.SetRange(end-1, end);
    range.SetText(str);
    long end2 = range.GetEnd();
    range.SetRange(end-1, end2);
    return range;
}


void SetAlign(Range& oRange, long align)
{
    Paragraphs paragraphs = oRange.GetParagraphs();
    Paragraph paragraph = paragraphs.GetFirst();
    
    int n = paragraphs.GetCount();
    paragraph.SetAlignment(3);

    for (int i = 0; i < n-1; i++) {
        COleVariant v = 1L;

        paragraph = paragraph.Next(&v);
        paragraph.SetAlignment(align);
    }
}




void SetText(Table& oTbl, int row, int col, int v)
{
    CString s;
    s.Format("%d", v);

    SetText(oTbl, row, col, s);
}

void SetText0(Table& oTbl, int row, int col, int v)
{
    if (v != 0) {
        SetText(oTbl, row, col, v);
    }
}


void SetText(Table& oTbl, int row, int col, double v)
{
    CString s;
    s.Format("%g", v);

    SetText(oTbl, row, col, s);
}



int CGidrView::WordKorroz(const char *doc_name, int y2, const char *path0) 
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

  int y1 = y2-5+1;
//  int y2 = y1+5-1;

  CString tn = transl("Индикатор_коррозии_по_годам");


  Klassif *kls = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_KORROZ));

  if (!kls) {
    return 0;
  }
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


//    CString name = transl("Индикатор_коррозии");

    CString fname, txt_name;
    
    fname.Format("%s\\excel2\\xls\\Индикатор_коррозии.docx", argpath());
    txt_name.Format("%s\\excel2\\xls\\Индикатор_коррозии.txt", argpath());

    CopyFile(fname, doc_name, 0);


//    CString fname = argpath()+"excel\\xls\\qq.doc";
//    fname = "D:\\gid\\0\\qq.doc";
//    oSel.InsertFile(fname, covOptional, covOptional, covOptional, covOptional);
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



    long l = oTbls.GetCount();

    if (l < 2) return 0;


    oTbl1 = oTbls.Item(1L);
    oTbl2 = oTbls.Item(2L);

    CAdoFile *ado = kls->m_ado;

    q.Format(

" SELECT "
"   IK.ID, Year(IKG.Data_izvlecheniya) AS Y, Kod_RS_nachalnoy_kamery,Nachalnaya_kamera,Kod_RS_konechnoy_kamery,Konechnaya_kamera, "
"   IKG.Srednyaya_skorost_korrozii__mm_god AS V, "
"   OZ.name AS O, "
"   es.name AS podacha, "
"   IK.Nomer_indikatora_korrozii "

" FROM Indikator_korrozii IK "
" LEFT JOIN Indikator_korrozii_po_godam IKG ON IK.ID = IKG.ID_I "
" LEFT JOIN netWaterAggressivenesses OZ ON OZ.ID=IK.otsenka_korrozionnogo_protsessa"
" LEFT JOIN externalSigns es ON es.id=IK.truboprovod"
" WHERE Year(IKG.Data_izvlecheniya) >= %d AND Year(IKG.Data_izvlecheniya) <= %d "
" ORDER BY IK.ID, Year(IKG.Data_izvlecheniya)"
    , y1, y2);



    
    CString s;

    {
      int r0 = 2, c = 1;
      int r = r0;

//      InitTab(kls->m_ado->m_strFileName, transl("Индикатор_коррозии"), false);

      Rows oRows = oTbl2.GetRows();
      Row oRowEnd = oRows.GetLast();


      for (int i = 0; i < 5; i++) {
        CString s1;
        s.Format("%d", y1+i);
        SetText(oTbl2, 2, 4+i, s);
      }

      if (ado->openTable0(q)) {

          int ID_old = -1;

          double sum = 0;

          while ( !ado->isEOF() ) {
            int ID = ado->read_long("ID");
            int year = ado->read_long("Y");
            double V = ado->read_double("V");

            CString nomer = ado->readStr(transl("Номер индикатора коррозии"));

            CString kod1  = ado->readStr(transl("Код РС начальной камеры"));
            CString name1 = ado->readStr(transl("Начальная камера"));
            CString kod2  = ado->readStr(transl("Код РС конечной камеры"));
            CString name2 = ado->readStr(transl("Конечная камера"));
            CString tr = ado->readStr(transl("podacha"));

//            int O = ado->read_long("O");
//            CString OO = getReal(transl("Индикатор_коррозии"), transl("Оценка коррозионного процесса"), O);
            CString OO = ado->readStr("O");

            if (ID > ID_old) {
              s.Format("%d", ID);
              ID_old = ID;

              COleVariant f(0L, VT_BOOL);

              long rc = oRows.GetCount();
  
              if (r > r0) {
                oRows.Add(&covOptional);
              }

              r++;

              SetText(oTbl2, r, 1, nomer);

              SetText(oTbl2, r, 2, kod1);
              SetText(oTbl2, r, 3, name1);
              SetText(oTbl2, r, 4, kod2);
              SetText(oTbl2, r, 5, name2);
              SetText(oTbl2, r, 6, tr);
            
              sum = 0;
            }

            sum += V;


            CString s;

            s.Format("%.4f %s", V, OO);
            SetText(oTbl2, r, year-y1+7, s);


            s.Format("%.4f", sum);

            SetText(oTbl2, r, 12, s);


            ado->MoveNext();
          }
        }
        ado->closeTable();
    }


    {
      Rows oRows = oTbl1.GetRows();

//      q.Format("SELECT *, [Дата извлечения]-[Дата установки] AS DT FROM Индикатор_коррозии ");

      q.Format("SELECT *, DATEDIFF(day, %s, %s) AS DT FROM  %s",  
        transl("Дата установки"), transl("Дата извлечения"), transl("Индикатор_коррозии")
      );

      q.Format(
" SELECT "
" ag.name AS agres,"
" ots.name AS otsen,"
" es.name AS podacha, "
" DATEDIFF(day, data_ustanovki, data_izvlecheniya) AS DT, * "
" FROM indikator_korrozii IK"
" LEFT JOIN externalSigns es ON es.id=IK.truboprovod"
" LEFT JOIN netWaterAggressivenesses ag ON ag.id=IK.agressivnost_setevoy_vody"
" LEFT JOIN corrosionProcessMarks ots ON ots.id=IK.otsenka_korrozionnogo_protsessa");



      int r0 = 2;
      int r = r0;

      if (ado->openTable0(q)) {


          while ( !ado->isEOF() ) {
            int ID = ado->read_long("ID");

            CString nomer  = ado->readStr(transl("Номер индикатора коррозии"));


            CString kod1  = ado->readStr(transl("Код РС начальной камеры"));
            CString name1 = ado->readStr(transl("Начальная камера"));
            CString kod2  = ado->readStr(transl("Код РС конечной камеры"));
            CString name2 = ado->readStr(transl("Конечная камера"));
            CString tr = ado->readStr(transl("podacha"));
            CString d1 = ado->readStr(transl("Дата извлечения"));
            CString dt = ado->readStr("DT");
            CString V = ado->readStr(transl("Средняя скорость коррозии, мм/год"));


//            CString n1 = ado->readStr(transl("Оценка коррозионного процесса"));
//            CString n2 = ado->readStr(transl("Агрессивность сетевой воды"));
            CString N1 = ado->readStr(transl("otsen"));
            CString N2 = ado->readStr(transl("agres"));
  

            
            CString prim = ado->readStr(transl("Внешний вид пластин"));

//            CString N1 = getReal(transl("Индикатор_коррозии"), transl("Оценка коррозионного процесса"), n1);  if (n1 == N1) N1 = "";
//            CString N2 = getReal(transl("Индикатор_коррозии"), transl("Агрессивность сетевой воды"), n2);     if (n2 == N2) N2 = "";

//            int O = ado->read_long("O");
//            CString OO = getReal(transl("Индикатор_коррозии"), transl("Оценка коррозионного процесса"), O);

            s.Format("%d", ID);

            COleVariant f(0L, VT_BOOL);


            long rc = oRows.GetCount();

            if (r > r0) {
              oRows.Add(&covOptional);
            }

            SetText(oTbl1, r, 1, nomer);

            SetText(oTbl1, r, 2, kod1);
            SetText(oTbl1, r, 3, name1);
            SetText(oTbl1, r, 4, kod2);
            SetText(oTbl1, r, 5, name2);
            SetText(oTbl1, r, 6, tr);
            SetText(oTbl1, r, 7, d1);
            SetText(oTbl1, r, 8, dt);
            SetText(oTbl1, r, 9, V);
            SetText(oTbl1, r, 10, N1);
            SetText(oTbl1, r, 11, N2);
            SetText(oTbl1, r, 12, prim);

            CString s;
  //          s.Format("%g %s", V, OO);
  //          SetText(oTbl1, r, year-y1+7, s);
            r ++;

            ado->MoveNext();
        }
        ado->closeTable();
      }
    }


    COleVariant f(0L, VT_BOOL);
    COleVariant t(1L, VT_BOOL);
    COleVariant nul(1L, VT_BOOL);

    COleVariant wdReplaceAll(2L);
//    oFind.Execute(COleVariant("$A01$"), &f, &f, &f, &f, &f, &t, &f, &f, COleVariant("qqqq"), &wdReplaceAll);

//    char s[1024], a1[1024];

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

