#include "stdafx.h"

#include "../resource.h"
#include "../gid6Doc.h"

#include "TgWnd.h"
#include "tempgraph.h"


#include "wm_user.h"


#define _basic_color RGB(0,0,0)
#define _setka_color RGB( 180, 180,180 )
#define _POD_color RGB(255,0,0)
#define _OBR_color RGB( 128,128,255 )
#define _SMESH_color RGB( 0, 213,106 )
#define _VETER_color RGB(200,200,0)

static bool is_smesh;

long round_to_dig(const double val)
{
    long lVal = (val);
    if (((lVal - val) >= 0.5) && (val < 0))lVal--;
    if (((val - lVal) >= 0.5) && (val >= 0))lVal++;
    return lVal;
}

IMPLEMENT_DYNCREATE(CTgWnd, CWnd)
struct TG {
    double tn, Q_otn, t1, t2, t3, tv, t_bn, tg;

    bool operator<(const TG& mc) const
    {
        return tn > mc.tn;
    };

};


BEGIN_MESSAGE_MAP(CTgWnd, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_PAINT()


  // Standard printing commands
//  ON_COMMAND(ID_FILE_PRINT, CWnd::OnFilePrint)
//  ON_COMMAND(ID_FILE_PRINT_DIRECT, CWnd::OnFilePrint)
//  ON_COMMAND(ID_FILE_PRINT_PREVIEW, CWnd::OnFilePrintPreview)

END_MESSAGE_MAP()

CTgWnd::CTgWnd()
{
    stepX = 20; // шаг между линиями сетки по X
    stepY = 14; // ... по Y

    nGridX = 27; // количество линий сетки по X
    nGridY = 17; // ... по Y
    llx = 30;
    lly = 10;


    llx = 0;
    lly = 0;


    lX = llx + 45,     // значение X левого верхнего угла
        lY = lly + 30,     // значение Y ...
        rX = lX + stepX * nGridX, // значение X проавого нижнего угла
        rY = lY + stepY * nGridY; // значение Y ...

    font = NULL;
    DotPen = NULL;
    drawdc = NULL;
    DotPen = new CPen(PS_DOT, 1, RGB(0, 0, 0));
    lButtonDown = 0;
    oldp.x = oldp.y = 0;

  m_ch1 = TRUE;
  m_ch2 = TRUE;
  m_ch3 = TRUE;
  m_ch4 = TRUE;
  m_ch5 = TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CEx21View printing

BOOL CTgWnd::OnPreparePrinting(CPrintInfo* pInfo)
{
  // default preparation
//  return DoPreparePrinting(pInfo);
    return 0;
}

void CTgWnd::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
  m_isPrinting = TRUE;
}

void CTgWnd::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add cleanup after printing
  m_isPrinting = FALSE;
}





void CTgWnd::init0(int id) 
{
//  CWnd::OnInitialUpdate();

  SetWindowText("Температурный график");

//  Cgid6Doc* pDoc = (Cgid6Doc*) GetDocument();

  m_id = id;

  InitArray();
  

  CString q;
  CAdoFile* ado = getAdo(getGidAdoName());
  q.Format("SELECT * FROM heatSources WHERE ID = %d", id);
  showGraphOT = false;
  if (ado->openTable0(q)) {
      BOOL type = ado->read_long("graphTypeID");
      ado->closeTable();
      if (!(type == 1 || type == 0)) {
          showGraphOT = true;
          InitArrayOT(ado);
      }
  }
  


  InitLine();

  is_smesh = false;

  for (long i = 1; i<array1[2].kol; i++)
  {
      if (array1[2].y[i] != 0) {
          is_smesh = true;
          break;
      }
  }

  if (!is_smesh)
  {
      array1[2].show = FALSE;
//      m_smesh.ShowWindow(SW_HIDE);
  }

  init();
  Invalidate();
}


void CTgWnd::init()
{
}


int CTgWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CWnd::OnCreate(lpCreateStruct) == -1)
    return -1;
  
  // Create the Grid control in the client rectangle
  RECT  rect;
  GetClientRect(&rect);
//  m_grid.CreateGrid(WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
//            rect,this,1234);

//  m_grid.Create(IDD_TEMP_GRAPH_GR_PAGE, this);

  
  return 0;
}


void CTgWnd::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
    // TODO: Add your specialized code here and/or call the base class

    m_drawRect = pInfo->m_rectDraw;
    m_printPage = pInfo->m_nCurPage;

//    CWnd::OnPrint(pDC, pInfo);
}


void CTgWnd::OnPaint()
{
  CPaintDC dc(this); // device context for painting

  Draw(dc);
}



void CTgWnd::DrawEdge(CDC &dc)
{
    char buf[1000];
    CPen p5(PS_SOLID, 2, _basic_color);  //окантовка
    CPen * def_pen = dc.SelectObject(&p5);
    dc.MoveTo(lX, lY);  dc.LineTo(lX, rY);
    dc.MoveTo(lX, rY);  dc.LineTo(rX, rY);


    dc.SetTextColor(_basic_color); //подписи 

    CString Str;
//    CTempGraph * Mainn = (CTempGraph *)(GetParent()->GetParent());
    //  CTempGraph * Mainn=(CTempGraph *)(GetParent());

/**

    if (0 && Mainn != NULL)
    {
        Str = Mainn->TempStruct->name_str[2];
        Str += " : ";
        Str += Mainn->TempStruct->addon[2];
        dc.TextOut(lX - stepX + 50, (long)(lY - 1.5*stepY), Str);
        Str = Mainn->TempStruct->name_str[1];
        Str += " : ";
        Str += Mainn->TempStruct->addon[1];
        dc.TextOut(lX - stepX + 350, (long)(lY - 1.5*stepY), Str);
        Str = Mainn->TempStruct->name_str[0];
        Str += " : ";
        Str += Mainn->TempStruct->addon[0];
        dc.TextOut(lX - stepX + 500, (long)(lY - 1.5*stepY), Str);
    }
*/
    //dc.TextOut( lX+50,(long)(lY-1.5*stepY), DiamGR.Tip );
    minX = minY = masX = masY = 0;

    double tempminX, tempmaxX;
    FindMinMax(tempminX, tempmaxX, array1[0].x, array1[0].kol);
    if (minX > tempminX) minX = tempminX;
    if (maxX < tempmaxX) maxX = tempmaxX;

    MasShX(minX, maxX, nGridX, &masX);
    minY = 0;
    maxY = 200;
    MasShY(minY, maxY, nGridY, &masY);

    CRect space;
    int i, n = 0;
    CPen p1(PS_SOLID, 1, _setka_color);
    dc.SelectObject(&p1);
    for (i = 0; i<nGridX; i++) // сетка
    {
        n = (i % 2) * 5;
        if (i < nGridY)
        {
            dc.MoveTo(lX - n, rY - stepY - (stepY*i));  dc.LineTo(rX, rY - stepY - (stepY*i));
        }
        dc.MoveTo(lX + stepX + (stepX*i), lY);  dc.LineTo(lX + stepX + (stepX*i), rY + n);
    }

    dc.SetTextColor(_basic_color);
    for (i = 0; i <= nGridX / 2; i++) //подписи по оси X
    {
        sprintf(buf, "%4d", round_to_dig(maxX - (2 * stepX*i)*masX));
        dc.TextOut(lX - stepX / 2 + (2 * stepX*i), rY + stepY / 2, buf);
    }
    dc.TextOut(rX, (long)(rY + stepY / 2), "T возд., 'С");
    for (i = 0; i<nGridY; i++) //подписи по оси Y
    {
        sprintf(buf, "%4d", round_to_dig(minY + (stepY*i)*masY));
        dc.TextOut(lX - 1.5*stepX, rY - (stepY / 2) - stepY * i, buf);
    }
    dc.TextOut(lX - 1.5*stepX, lY - (stepY), "T воды, 'С");

    CPen p2(PS_SOLID, 2, _setka_color);
    dc.SelectObject(&p2);

    //Если showGraphOT тогда отрисовывать дополнительно отопительный температурный график
    if (m_ch5 && showGraphOT) {
        if (m_ch1) DrawLineDotted(dc, arrayOT[0]);
        if (m_ch2) DrawLineDotted(dc, arrayOT[1]);
        if (m_ch3) DrawLineDotted(dc, arrayOT[2]);
        if (m_ch4) DrawLineDotted(dc, arrayOT[3]);
    }
   

    if (m_ch1) DrawLine(dc, array1[0]);
    if (m_ch2) DrawLine(dc, array1[1]);
    if (m_ch3) if (is_smesh) DrawLine(dc, array1[2]);
    if (m_ch4) DrawLine(dc, array1[3]);

    
    dc.SelectObject(def_pen);
}
//----------------------------------------------------------------------------
void CTgWnd::DrawLine(CDC &dc, GR_PARAM2 &gr)
{
    double * _x = gr.x;
    double * _y = gr.y;
    long kol = gr.kol;
    CPen * Pen = gr.pen;
    if (gr.show)
    {
        CPen * pp = dc.SelectObject(Pen);
        for (long i = 1; i<kol; i++)
        {
            dc.MoveTo(rX - (_x[i - 1] - minX) / masX, rY - (_y[i - 1] - minY) / masY);
            dc.LineTo(rX - (_x[i] - minX) / masX, rY - (_y[i] - minY) / masY);
        }
        dc.SelectObject(pp);
    }
}

void CTgWnd::DrawLineDotted(CDC& dc, GR_PARAM2& gr)
{
    double* _x = gr.x;
    double* _y = gr.y;
    long kol = gr.kol;
    CPen* Pen = gr.pen;
    if (gr.show)
    {
        CPen* pp = dc.SelectObject(Pen);
        for (long i = 1; i < kol; i++)
        {
            double xM = (_x[i - 1] + _x[i])/2;
            double yM = (_y[i - 1] + _y[i]) / 2;
            dc.MoveTo(rX - (_x[i - 1] - minX) / masX, rY - (_y[i - 1] - minY) / masY);
            dc.LineTo(rX - (xM - minX) / masX, rY - (yM - minY) / masY);
        }
        dc.SelectObject(pp);
    }
}

//----------------------------------------------------------------------------
void CTgWnd::Draw(CDC &dc)
{
        SetMyFont(dc);
        bkgrnd.left = (long)(lX - 3 * stepX);
        bkgrnd.top = (long)(lY - 2 * stepY);
        bkgrnd.right = (long)(rX + 3 * stepX);
        bkgrnd.bottom = (long)(rY + 1.6*stepY);


        bkgrnd.left = (long)(lX);
        bkgrnd.top = (long)(lY);
        bkgrnd.right = (long)(rX);
        bkgrnd.bottom = (long)(rY);


        GetClientRect(bkgrnd);



        dc.FillRect(&bkgrnd, &CBrush(RGB(255, 255, 255)));
        dc.SetTextColor(RGB(0, 0, 0));
        DrawEdge(dc);
    }
//----------------------------------------------------------------------------
void CTgWnd::SetMyFont(CDC &dc)
{
    if (font == NULL)
    {
        font = new CFont();
        LOGFONT lf;
        memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
        lf.lfHeight = 14;
        lf.lfWidth = 0;
        lf.lfEscapement = 0;
        lf.lfOrientation = 0;
        lf.lfWeight = FW_NORMAL;
        lf.lfItalic = 0;
        lf.lfUnderline = 0;
        lf.lfStrikeOut = 0;
        lf.lfCharSet = ANSI_CHARSET;
        lf.lfCharSet = RUSSIAN_CHARSET;
        lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
        lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
        lf.lfQuality = PROOF_QUALITY;
        lf.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
        lstrcpy(lf.lfFaceName, "Times New Roman");
        VERIFY(font->CreateFontIndirect(&lf));  // create the font          
    }
    dc.SelectObject(font);
}
//----------------------------------------------------------------------------
double CTgWnd::FindMax(double *arr, long kol)
{
    double _maxY_ = arr[0];
    for (int i = 1; i<kol; i++) if (arr[i] > _maxY_) _maxY_ = arr[i];
    return _maxY_;
}
//----------------------------------------------------------------------------
void CTgWnd::FindMinMax(double &min, double &max, double *arr, long kol)
{
    min = max = arr[0];
    for (int i = 1; i<kol; i++)
    {
        if (arr[i] > max) max = arr[i];
        if (arr[i] < min) min = arr[i];
    }

}
//----------------------------------------------------------------------------

void CTgWnd::MasSh(double  min, double max, short kol, double *mas)
{
    double p = (double)(max - min) / kol;
    int masd = (int)p + 1;
    if (masd > 10)
    {
        int j = 1;
        while (j != 0)  j = masd - ((int)masd++ / 10) * 10;
        masd -= 1;
    }
    *mas = masd;
}


//----------------------------------------------------------------------------
void CTgWnd::MasShX(double  min, double max, short kol, double *mas)
{
    double masd = (max - min) / ((rX - lX)*1.0);
    *mas = masd;
}
//----------------------------------------------------------------------------
void CTgWnd::MasShY(double  min, double max, short kol, double *mas)
{
    double masd = (max - min) / ((rY - lY)*1.0);
    *mas = masd;
}


bool getTG_n(long hSourceID, int n, double &tn, double &t1, double &t2, double &t3, double &tv);
int getTG_n_col(long hSourceID);


void CTgWnd::InitArray()
{
    CString q;
    long Kol, pen_width = 2;
    int j;
    set <TG> tgl;

    bool getTG_tn1_tn2(long hSourceID, double &tn1, double &tn2);

    double tn1, tn2;

    TG tg;

    Kol = getTG_n_col(m_id);

    array1[0].pen = new CPen(PS_SOLID, pen_width, _POD_color);
    array1[1].pen = new CPen(PS_SOLID, pen_width, _OBR_color);
    array1[2].pen = new CPen(PS_SOLID, pen_width, _SMESH_color);
    array1[3].pen = new CPen(PS_SOLID, pen_width, _VETER_color);

    for (j = 0; j < 4; j++) {
        array1[j].x = new double[Kol + 1];
        array1[j].y = new double[Kol + 1];
        array1[j].kol = Kol;
        array1[j].show = TRUE;
    }


    for (int i = 0; i < Kol; i++) {
        double tn, t1, t2, t3, tv;
        getTG_n(m_id, i, tn, t1, t2, t3, tv);

        tg.tn = tn;
        tg.t1 = t1;
        tg.t2 = t2;
        tg.t3 = t3;
        tg.tv = tv;
        for (j = 0; j < 4; j++) {
            array1[j].x[i] = tg.tn;
        }

        array1[0].y[i] = tg.t1;
        array1[1].y[i] = tg.t2;
        array1[2].y[i] = tg.t3;
        array1[3].y[i] = tg.tv;
    }
    //----------------------------------------------------------------------------
}

void CTgWnd::InitArrayOT(CAdoFile* ado)
{
    long Kol, pen_width = 2;
    int j;

    arrayOT[0].pen = new CPen(PS_SOLID, pen_width, _POD_color);
    arrayOT[1].pen = new CPen(PS_SOLID, pen_width, _OBR_color);
    arrayOT[2].pen = new CPen(PS_SOLID, pen_width, _SMESH_color);
    arrayOT[3].pen = new CPen(PS_SOLID, pen_width, _VETER_color);
    
    CTempGraph* tempGraph = new CTempGraph(ado, m_id, false);
    COMMON_TEMP_STRUCTURE * tempStruct = tempGraph->TempStruct;
    Kol = tempStruct->ot_data->n;
    

    for (j = 0; j < 4; j++) {
        arrayOT[j].x = new double[Kol + 1];
        arrayOT[j].y = new double[Kol + 1];
        arrayOT[j].kol = Kol;
        arrayOT[j].show = TRUE;
    }


    for (int i = 0; i < Kol; i++) {
        double tn, t1, t2, t3, tv;
       
        for (j = 0; j < 4; j++) {
            arrayOT[j].x[i] = tempStruct->array_ptr[0][i]; //tn
        }

        arrayOT[0].y[i] = tempStruct->array_ptr[2][i]; //t1
        arrayOT[1].y[i] = tempStruct->array_ptr[3][i]; //t2
        arrayOT[2].y[i] = tempStruct->array_ptr[4][i]; //t3
        arrayOT[3].y[i] = tempStruct->array_ptr[5][i];  //tv
    }
    delete tempGraph->TempStruct;
    delete tempGraph->TempStruct_norm;
    delete tempGraph;
    
}

//----------------------------------------------------------------------------
void CTgWnd::InitLine()
{
//    m_pod.SetCheck(array1[0].show);
//    m_obr.SetCheck(array1[1].show);
//    m_smesh.SetCheck(array1[2].show);
//    m_wind.SetCheck(array1[3].show);
}
//----------------------------------------------------------------------------
void CTgWnd::OnLine()
{
//    array1[0].show = m_pod.GetCheck();
//    array1[1].show = m_obr.GetCheck();
//    array1[2].show = m_smesh.GetCheck();
//    array1[3].show = m_wind.GetCheck();

//    this->Invalidate();
}


void CTgWnd::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    CRect sz;
    
    GetClientRect(sz);


    stepX = (cx - lX*2)/nGridX; // шаг между линиями сетки по X
    stepY = (cy - lY*2)/nGridY; // ... по Y

//    lY = lly + 30,     // значение Y ...
    rX = lX + stepX * nGridX, // значение X проавого нижнего угла
    rY = lY + stepY * nGridY; // значение Y ...




}


void CTgWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
  CString s;
  double x = minX + (rX - point.x)*masX;
  double y = minY + (rY - point.y)*masY;

  s.Format("Т возд.: %.0f°C,   Т воды,: %.0lf°C", x, y);  

  GetParent()->SendMessage(WM_SET_OUT, (WPARAM)0, (LPARAM)(LPCSTR)s);


/*
    drawdc = new CClientDC(this);
    drawdc->SelectObject(font);
    if ((point.x<rX) && (point.x>lX) && (point.y<rY) && (point.y>lY))
    {
        drawdc->TextOut(620, 190, "Т возд.,'C:");
        drawdc->TextOut(620, 230, "Т воды, 'C:");
        char buf[18] = "                 ";
        double x = minX + (rX - point.x)*masX;
        double y = minY + (rY - point.y)*masY;
        sprintf(buf, "%6.0lf", x);  drawdc->TextOut(620, 210, buf);
        sprintf(buf, "%6.0lf", y);  drawdc->TextOut(620, 250, buf);
    }
    lButtonDown = 1;
    SetCapture();
    delete drawdc;
*/

    CWnd::OnLButtonDown(nFlags, point);
}

//----------------------------------------------------------------------------
void CTgWnd::OnLButtonUp(UINT nFlags, CPoint point)
{

  GetParent()->SendMessage(WM_SET_OUT, (WPARAM)0, (LPARAM)(LPCSTR)"");

/*
    lButtonDown = 0;
    char buf[18] = "                 ";
    RubberBand(oldp);
    drawdc = new CClientDC(this);
    drawdc->SelectObject(font);
    if ((point.x<rX) && (point.x>lX) && (point.y<rY) && (point.y>lY))
    {
        drawdc->TextOut(620, 190, buf);
        drawdc->TextOut(620, 210, buf);
        drawdc->TextOut(620, 230, buf);
        drawdc->TextOut(620, 250, buf);
    }
    delete drawdc;
    ReleaseCapture();
  
    point.x = point.y = oldp.x = oldp.y = -1;
*/

    CWnd::OnLButtonUp(nFlags, point);
}



//----------------------------------------------------------------------------
void CTgWnd::RubberBand(CPoint pos)
{
    drawdc = new CClientDC(this);
    oldMode = drawdc->SetROP2(R2_NOTXORPEN);
    DotPen = NULL;
    DotPen = new CPen(PS_DOT, 1, RGB(0, 0, 0));
    CPen * pen = (CPen *)drawdc->SelectObject(DotPen);
    if (pos.x >= lX && pos.x <= rX)
    {
        drawdc->MoveTo(pos.x, lY);
        drawdc->LineTo(pos.x, rY);
    }
    if (pos.y >= lY && pos.y <= rY)
    {
        drawdc->MoveTo(lX, pos.y);
        drawdc->LineTo(rX, pos.y);
    }
    drawdc->SelectObject(pen);
    delete DotPen;
    DotPen = NULL;
    drawdc->SetROP2(oldMode);
    delete drawdc;
}



void CTgWnd::Copy()
{
    if (!OpenClipboard())
    {
        AfxMessageBox("Невозможно открыть буфер обмена", MB_OK | MB_ICONSTOP);
        return;
    }
    if (!EmptyClipboard())
    {
        AfxMessageBox("Невозможно очистить буфер обмена для записи в него", MB_OK | MB_ICONSTOP);
        return;
    }
    CMetaFileDC mfDC;
    mfDC.CreateEnhanced(NULL, NULL, NULL, NULL);
    Draw(mfDC);
    mf = mfDC.CloseEnhanced();
    if (::SetClipboardData(CF_ENHMETAFILE, mf) == NULL)
    {
        AfxMessageBox("Невозможно скопировать данные в буфер обмена", MB_OK | MB_ICONSTOP);
        CloseClipboard();
        return;
    }
    CloseClipboard();
}

void CTgWnd::SetLines(BOOL c1, BOOL c2, BOOL c3, BOOL c4, BOOL c5)
{
  m_ch1 = c1;
  m_ch2 = c2;
  m_ch3 = c3;
  m_ch4 = c4;
  m_ch5 = c5;
}
