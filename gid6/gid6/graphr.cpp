#include "stdafx.h"
#include "gid6.h"
#include "graphr.h"

#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
/////////////////////////////////////////////////////////////////////////////
// DEFINE SECTION
#define _HQ_color RGB(255,0,0)
#define _setka_color RGB( 180, 180,180 )
#define _rab_zone_color RGB( 255, 133, 64 )
#define _basic_color RGB(0,0,0)
*/
#define _NOTerror_color RGB(0,255,0)
#define _error_color RGB(255,0,0)
#define _diam_char_color RGB(215,0,215)

/////////////////////////////////////////////////////////////////////////////
// CGraphR dialog
CGraphR::CGraphR(GRAPHR_PARAM * Param, CWnd* pParent /*=NULL*/)
	: CGraphTemplate(&Param->gr, CGraphR::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGraphR)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	if (Param == NULL) return;

	GRP = (*Param);
	//  GRP.k_kol=1;

	_rr2 = -1.0;
	p.x = oldp.x = pmode.x = -1;
	p.y = oldp.y = pmode.y = -1;
	qq2 = hh2 = kb = del_p = del_d = d_obt = 0.0;
	fl = 0;
	oldMode = 0;
	rButtonDown = lButtonDown = 0;
	frst = 0;


	masX = 0, masY = 0;

	dot_pen = NULL;
	dot_pen = new CPen(PS_DOT, 1, _basic_color);

	//анализ
	minX = minY = maxX = maxY = 0;
	inihq(DiamGR.Q, DiamGR.Q, DiamGR.k_kol); //масштабирование к кол-ву насосов   
	DiamGR.Q_min = DiamGR.Q_min*DiamGR.k_kol;//масштабирование к кол-ву насосов   
	DiamGR.Q_max = DiamGR.Q_max*DiamGR.k_kol;//масштабирование к кол-ву насосов   
	maxX = FindMax(DiamGR.Q);
	maxY = FindMax(DiamGR.H);
	maxX += maxX * 0.2;
	maxY += maxY * 0.2;
	double pog;
	Appr(r0, r1, r2, DiamGR.H, DiamGR.Q, &pog);
}
//----------------------------------------------------------------------------
void CGraphR::DoDataExchange(CDataExchange* pDX)
{
	CGraphTemplate::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphR)
	DDX_Control(pDX, IDCANCEL, m_CANCEL);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_REP, m_c_rep_but);
	DDX_Control(pDX, IDC_COPY_TO, m_copy_to);
	//}}AFX_DATA_MAP
}
//----------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CGraphR, CGraphTemplate)
	//{{AFX_MSG_MAP(CGraphR)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_REP, OnRep)
	ON_BN_DOUBLECLICKED(IDC_REP, OnRep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphR message handlers
void CGraphR::OnRep()
{
	CGraphTemplate::OnRep();
	char *buttonName[2] = { "Отчет", "График" };
	m_c_rep_but.SetWindowText(buttonName[gr_rap]);
}
//----------------------------------------------------------------------------
void CGraphR::Draw(CDC &dc)
{
	CGraphTemplate::Draw(dc);

	DrawDiam(dc);

}
//----------------------------------------------------------------------------

void CGraphR::DrawDiam(CDC &dc)
{
	CPen p1(PS_SOLID, 1, _setka_color);
	CPen * def;
	def = dc.SelectObject(&p1);
	char buf[80];
	int i;

	dc.TextOut(llx + 270, lly + 5, _TR("Расчет диаметра обточки"));

	if (!gr_rap) //График
	{
		int n = 0;
		char *legend[4] = { "H, м  ", "Q, т/ч", "КПД, %", "N, Квт" };
		char *legend1[4] = { "H  :", "Q  :", "КПД:", "N  :" };
		dc.SetTextColor(_basic_color);
		dc.TextOut(rX, rY + stepY / 2, legend[1]);

		dc.SetTextColor(_HQ_color);
		dc.TextOut(llx + 0, lly + stepY, legend[0]);

		strcpy(legX, legend1[1]);
		strcpy(legY, legend1[0]);

		MasSh(minX, maxX, nGridX, &masX);
		MasSh(minY, maxY, nGridY, &masY);
		CRect space;
		space.left = lX + (double)DiamGR.Q_min / ((double)masX / (stepX));
		space.top = lY;
		space.right = lX + (double)DiamGR.Q_max / ((double)masX / (stepX));
		space.bottom = rY;
		dc.FillRect(&space, &CBrush(_rab_zone_color));
		for (int i = 0; i<nGridY; i++)   // сетка
		{
			n = (i % 2) * 5;
			if (i < nGridX)
			{
				dc.MoveTo(lX + stepX + (stepX*i), lY); dc.LineTo(lX + stepX + (stepX*i), rY + n);
			}
			dc.MoveTo(lX - n, rY - stepY - (stepY*i)); dc.LineTo(rX + n, rY - stepY - (stepY*i));
		}

		CPen p2(PS_SOLID, 2, _basic_color);
		dc.SelectObject(&p2);
		dc.MoveTo(lX, lY);  dc.LineTo(lX, rY); // ось Y
		dc.MoveTo(lX, rY);  dc.LineTo(rX, rY); // ось X

		dc.SetTextColor(_basic_color);
		for (i = 0; i<10; i++)             // цифры по X
		{
			sprintf(buf, "%6.0lf", i*masX * 2);
			if (i) dc.TextOut(lX - stepX / 2 + (stepX * 2 * i) - 10, rY + stepY / 2, buf);
		}
		for (i = 0; i<nGridY / 2; i++)       // цифры по Y
		{
			sprintf(buf, "%6.0lf", i*masY * 2);
			dc.TextOut(lX - 35, rY - stepY / 2 - (stepY * 2 * i), buf);
		}

		CPen p3(PS_SOLID, 2, _HQ_color);
		dc.SelectObject(&p3);
		DrawGraph(r0, r1, r2, &dc, lX, rY);

		if (frst == 1)
		{
//			m_OK.EnableWindow(TRUE);
			static CPoint oldP = pmode;
			oldP = p;
			p.x = (double)(p.x - lX) * (double)masX / stepX;
			p.y = (double)(rY - p.y) * (double)masY / stepY;
			pmode = p;
			if (pmode.x>0 && pmode.y>0) prdim(pmode, &dc);
			p = oldP;
		}
//		else m_OK.EnableWindow(FALSE);

	}
	else    //Отчет
	{
		EnableWindow(TRUE);
		dc.TextOut(llx + 15, lly + 60, _TR("Номинальный диаметр рабочего колеса, мм"));
		sprintf(buf, "%4d", (int)GRP.diam1); dc.TextOut(llx + 440, lly + 60, buf);
		dc.TextOut(llx + 15, lly + 80, _TR("Номинальная частота вращения, 1/мин"));
		sprintf(buf, "%4d", (int)GRP.c_vr1); dc.TextOut(llx + 440, lly + 80, buf);

		dc.TextOut(llx + 15, lly + 100, _TR("Диаметр обточенного рабочего колеса,  мм"));
		sprintf(buf, "%4.02f", d_obt); dc.TextOut(llx + 440, lly + 100, buf);
		sprintf(buf, _TR("Режимная точка :  Q = %5d т/ч ;    H = %5d м"), pmode.x, pmode.y);
		dc.TextOut(llx + 15, lly + 140, buf);
		sprintf(buf, _TR("Точка на номинальной характеристике : Q = %5.0lf т/ч ;    H = %5.0lf м"), qq2, hh2);
		dc.TextOut(llx + 15, lly + 160, buf);
		dc.TextOut(llx + 15, lly + 200, _TR("Коэффициент быстроходности, об/мин"));
		sprintf(buf, "%5.0lf", kb); dc.TextOut(440, lly + 200, buf);
		dc.TextOut(llx + 15, lly + 220, _TR("Полученная относительна величина обточки :"));
		sprintf(buf, "%8.6lf", del_p); dc.TextOut(440, lly + 220, buf);
		dc.TextOut(llx + 15, lly + 240, _TR("Допустимая относительна величина обточки :"));
		sprintf(buf, "%8.4lf", del_d); dc.TextOut(440, lly + 240, buf);
		if (del_p <= del_d)
		{
			dc.SetTextColor(_NOTerror_color);
			dc.TextOut(llx + 150, lly + 280, _TR("Обточка допустима"));
		}
		else
		{
			dc.SetTextColor(_error_color);
			dc.TextOut(llx + 150, lly + 280, _TR("Обточка недопустима"));
		}

	}
	dc.SelectObject(def);

}
//----------------------------------------------------------------------------
void CGraphR::prdim(CPoint p, CDC * drawdc)
{
	double pp = 0.0, pp3 = 0.0, pp4 = 0.0;

	// double _rr0 = 0.0, _rr1 = 0.0; // Сразу подставляем нули (в DrawGraph)
	_rr2 = p.y / pow(p.x, 2);
	CPen p5(PS_SOLID, 2, _diam_char_color);
	drawdc->SelectObject(&p5);
	DrawGraph(0.0, 0.0, _rr2, drawdc, lX, rY);

	pp = r1 / (r2 - _rr2);
	pp4 = r0 / (r2 - _rr2);
	pp3 = pow(pp, 2) / 4.0 - pp4;
	qq2 = (-pp / 2.0) + pow(pp3, 0.5); // QQ
	hh2 = r0 + r1 * qq2 + r2 * pow(qq2, 2);  // HH

	pp4 = pow(p.x, 0.5) / 60.0;
	pp = pow(p.y, 0.75);

	kb = 3.65 * GRP.c_vr1 * pp4 / pp;
	if (kb < 150.0)  d_obt = (double)GRP.diam1 * p.x / qq2;
	else              d_obt = (double)GRP.diam1 * pow(p.x / qq2, 0.5);
	del_p = (GRP.diam1 - d_obt) / GRP.diam1;
	if (kb <= 120.0) del_d = 0.20;
	else if (kb > 350.0) del_d = 0.0;
	else if (kb < 200.0) del_d = 0.15;
	else if (kb < 300.0) del_d = 0.11;
	else if (kb < 350.0) del_d = 0.09;
	else if (kb == 350.0) del_d = 0.07;
}
//----------------------------------------------------------------------------
void CGraphR::RubberBand(CPoint pos, CDC* drawdc)
{
	drawdc->MoveTo(pos.x, lY);
	drawdc->LineTo(pos.x, rY);
	drawdc->MoveTo(lX, pos.y);
	drawdc->LineTo(rX, pos.y);
}
//----------------------------------------------------------------------------
void CGraphR::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (!gr_rap) {
		fl = 1;
		drawdc = new CClientDC(this);
		drawdc->SelectObject(font);
		oldMode = drawdc->SetROP2(R2_NOTXORPEN);
		drawdc->TextOut(llx + 450, lly + 290, legX);
		drawdc->TextOut(llx + 450, lly + 310, legY);
		CString buf;
		double x = (double)(point.x - lX) * (double)masX / stepX;
		double y = (double)(rY - point.y) * (double)masY / stepY;
		buf.Format("%6.0lf", x);  drawdc->TextOut(llx + 470, lly + 290, buf);
		buf.Format("%6.0lf", y);  drawdc->TextOut(llx + 470, lly + 310, buf);
		pen = drawdc->SelectObject(dot_pen);
		lButtonDown = 1;
		SetCapture();
	}
	CGraphTemplate::OnLButtonDown(nFlags, point);
}
//----------------------------------------------------------------------------
void CGraphR::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (!gr_rap && lButtonDown) {
		lButtonDown = 0;
		CString buf = "               ";
		RubberBand(oldp, drawdc);
		drawdc->SetROP2(oldMode);
		drawdc->SelectObject(pen);
		drawdc->TextOut(llx + 450, lly + 290, buf);
		drawdc->TextOut(llx + 450, lly + 310, buf);
		drawdc->TextOut(llx + 470, lly + 290, buf);
		drawdc->TextOut(llx + 470, lly + 310, buf);
		ReleaseCapture();
		Draw(*drawdc);
		delete drawdc;
		point.x = point.y = oldp.x = oldp.y = -1;
		fl = 0;
	}
	CGraphTemplate::OnLButtonUp(nFlags, point);
}
//----------------------------------------------------------------------------
void CGraphR::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (!gr_rap) {
		if ((fl) && (point.x<rX) && (point.x>lX) && (point.y<rY) && (point.y>lY))
		{
			CString buf = "             ";// memset( buf, ' ', 12 ); buf[12] = 0;
			drawdc->TextOut(llx + 470, lly + 290, buf);
			drawdc->TextOut(llx + 470, lly + 310, buf);
			buf.Format("%6.0lf", (double)(point.x - lX) * (double)masX / stepX);
			drawdc->TextOut(llx + 470, lly + 290, buf);
			buf.Format("%6.0lf", (double)(rY - point.y) * (double)masY / stepY);
			drawdc->TextOut(llx + 470, lly + 310, buf);
			RubberBand(oldp, drawdc);
			RubberBand(point, drawdc);
			oldp = point;
		}
	}
	CGraphTemplate::OnMouseMove(nFlags, point);
}
//----------------------------------------------------------------------------
void CGraphR::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	CGraphTemplate::PostNcDestroy();
	if (dot_pen != NULL) delete dot_pen;
}
//----------------------------------------------------------------------------
void CGraphR::OnRButtonDown(UINT nFlags, CPoint point)
{
	if ((point.x<rX) && (point.x>lX) && (point.y<rY) && (point.y>lY))
	{
		p = point;
		rButtonDown = 1;
		frst = 1;
		CClientDC * cdc;
		cdc = new CClientDC(this); // device context for painting
		Draw(*cdc);
		delete cdc;

	}
	CGraphTemplate::OnRButtonDown(nFlags, point);
}
//----------------------------------------------------------------------------
CString CGraphR::GetCaptionStrt()
{
	CString Str;
	Str.LoadString(IDS_STRING_ST_NAS_CHARG);
	return Str;
}
//----------------------------------------------------------------------------
void CGraphR::CopyRaport(char *cbuf)
{
	char buf[1000];
	CString Str;
	Str.LoadString(IDS_GRAPHR_M2);  strcpy(cbuf, Str);
	sprintf(buf, "\n"); strcat(cbuf, buf);
	Str.LoadString(IDS_GRAPHR_TYPE);      strcat(cbuf, Str);
	sprintf(buf, ":%s\n", DiamGR.Tip);     strcat(cbuf, buf);
	Str.LoadString(IDS_GRAPHR_DNOM);      strcat(cbuf, Str);
	sprintf(buf, "\t%4.3f\n", GRP.diam1);     strcat(cbuf, buf);
	Str.LoadString(IDS_GRAPHR_FNOM);      strcat(cbuf, Str);
	sprintf(buf, "\t%4.3f\n", GRP.c_vr1);     strcat(cbuf, buf);
	Str.LoadString(IDS_GRAPHR_DOBT);      strcat(cbuf, Str);
	sprintf(buf, "\t%4.2f\n", d_obt);     strcat(cbuf, buf);

	Str.LoadString(IDS_GRAPHR_R_POINT);     strcat(cbuf, Str);
	sprintf(buf, "  Q =%5d", pmode.x);   strcat(cbuf, buf);
	Str.LoadString(IDS_GRAPHR_TCH);       strcat(cbuf, Str);
	sprintf(buf, ";    H = %5d", pmode.y);   strcat(cbuf, buf);
	Str.LoadString(IDS_GRAPHR_METER);     strcat(cbuf, Str);
	sprintf(buf, "\n");           strcat(cbuf, buf);

	Str.LoadString(IDS_GRAPHR_POINT);     strcat(cbuf, Str);
	sprintf(buf, "  Q = %5.0lf", qq2);     strcat(cbuf, buf);
	Str.LoadString(IDS_GRAPHR_TCH);       strcat(cbuf, Str);
	sprintf(buf, ";    H = %5.0lf", hh2);    strcat(cbuf, buf);
	Str.LoadString(IDS_GRAPHR_METER);     strcat(cbuf, Str);
	sprintf(buf, "\n");           strcat(cbuf, buf);
	Str.LoadString(IDS_GRAPHR_KB);        strcat(cbuf, Str);
	sprintf(buf, " %5.0lf\n", kb);          strcat(cbuf, buf);
	Str.LoadString(IDS_GRAPHR_DOTN);      strcat(cbuf, Str);
	sprintf(buf, "%8.6lf\n", del_p);      strcat(cbuf, buf);
	Str.LoadString(IDS_GRAPHR_DDOTN);     strcat(cbuf, Str);
	sprintf(buf, "%8.4lf\n", del_d);     strcat(cbuf, buf);
	if (del_p <= del_d)
	{
		Str.LoadString(IDS_GRAPHR_OLET);      strcat(cbuf, Str);
	}
	else
	{
		Str.LoadString(IDS_GRAPHR_ODONTLET);      strcat(cbuf, Str);
	}
}
//----------------------------------------------------------------------------











/*
if( GRP.men != 4 ) {
sprintf(buf, _TR("Характеристика : %s\n"), title[GRP.pr - 1] );
strcat(cbuf, buf);
sprintf(buf, _TR("Количество насосов - %4d\n"), GRP.k_kol );
strcat(cbuf, buf);
sprintf(buf, _TR("Максимальная ошибка аппроксимации, %%\t%1.2lf\n"), pog1 );
strcat(cbuf, buf);

if( GRP.men == 2 ) { // пересчет по частоте
sprintf( buf, _TR("Новое значение частоты вращения, 1/мин\t%4d\n"), GRP.c_vr );
strcat(cbuf, buf);
}
if( GRP.men == 3 ) { // пересчет по диаметру
sprintf(buf, _TR("Новое значение диаметра рабочего колеса, мм\t%4d\n"), GRP.diam );
strcat(cbuf, buf);
}
sprintf( buf, _TR("Рабочая зона :\n") );
strcat(cbuf, buf);
if( GRP.pr != 2 ) {
int a, b, a1, b1;
a = GRP.Nmin * GRP.k_kol;
b = r0 + r1*a + r2*pow( a, 2 );
a1 = GRP.Nmax*GRP.k_kol;
b1 = r0+r1*a1+r2*pow(a1,2);
sprintf(buf, _TR("Левая граница: напор - %5d м,   расход - %5d т/ч\n") , b, a);
strcat(cbuf, buf);
sprintf( buf,  _TR("Правая граница: напор - %5d м,   расход - %5d т/ч\n") , b1, a1);
strcat(cbuf, buf);
}
char *par1[4] = { "H - напор, м ;", "H - напор, м ;",  "N - мощность, кВт ;",
_TR("КПД - коэффициент полезного действия, % ;") };
char *par2[4] = { "H", "H", "N", _TR("КПД") };

sprintf(buf, _TR("Уравнение характеристики : %s = r0 + r1*Q + r2*Q*Q\n"), par2[GRP.pr-1] );
strcat(cbuf, buf);
sprintf(buf, _TR("где   Q - расход,  т/ч ; %s\n"), par1[GRP.pr-1] );
strcat(cbuf, buf);

char *buf1="r0 = %10.7lf,м   r1 = %10.7lf, м/(т/ч)   r2 = %10.7lf, м/(т/ч)^2\n";

sprintf(buf, buf1, r0, r1, r2);
strcat(cbuf, buf);

if( GRP.men == 2 || GRP.men == 3 ) {
sprintf(buf, _TR("Для пересчета :\n") );
strcat(cbuf, buf);
sprintf(buf, buf1, rr0, rr1, rr2);
strcat(cbuf, buf);
}
}
else {


if( !gr_rap )
{
dc.TextOut(llx+270,lly+5, _TR("Характеристика :") );
dc.TextOut(llx+ 390,lly+5, title[GRP.pr - 1] );
COLORREF me[2]={RGB(0,0,0),RGB(255,0,0)};
CPen p1(PS_SOLID,1,RGB( 180, 180,180 ));
CPen * def;
def=dc.SelectObject(&p1);

COLORREF col[4] = { RGB(255,0,0),RGB(255,0,0),RGB(128,128,255),RGB(0,213,106)};
char *legend[4] = { "H, м  ", "Q, т/ч", _TR("КПД, %"), "N, Квт" };
char *legend1[4] = { "H  :", "Q  :", _TR("КПД:"), "N  :" };
int par[4] = { 0, 3, 1, 2 };

minX = minY = maxX = maxY = 0;
maxX = (double)GRP.gr.Q[9] * GRP.k_kol;
mashq( GRP.H, GRP.Q, par[GRP.pr-1] );
switch( GRP.pr )
{
case 1:
{
dc.SetTextColor( RGB(0,0,0));
dc.TextOut( rX, rY+stepY/2, legend[1] );
dc.SetTextColor( RGB(255,0,0) );
dc.TextOut( llx+0,lly+stepY, legend[0] );
strcpy( legX, legend1[1] );
strcpy( legY, legend1[0] );
break;
}
case 2:
{
maxX = (double)GRP.gr.H[0]; // переопределение maxX
dc.SetTextColor( RGB(0,0,0) );
dc.TextOut( llx+0,lly+stepY, legend[1] );
dc.SetTextColor( RGB(255,0,0) );
dc.TextOut( rX, rY+stepY/2, legend[0] );
strcpy( legX, legend1[1] );
strcpy( legY, legend1[0] );
break;
}
case 3:
{
dc.SetTextColor( RGB(0,0,0) );
dc.TextOut( rX, rY+stepY/2, legend[1] );
dc.SetTextColor( RGB(128,128,255) );
dc.TextOut(llx+0,lly+stepY, legend[3] );
strcpy( legX, legend1[1] );
strcpy( legY, legend1[3] );
break;
}
case 4:
{
dc.SetTextColor( RGB(0,0,0) );
dc.TextOut( rX, rY+stepY/2, legend[1] );
dc.SetTextColor( RGB(0,213,106));//TColor(TColor::LtCyan) );
dc.TextOut( llx+0,lly+stepY, legend[2] );
strcpy( legX, legend1[1] );
strcpy( legY, legend1[2] );
break;
}
}
FindMax( GRP.H );
maxX += maxX * 0.2;
maxY += maxY * 0.2;
MasSh( minX, maxX, nGridX, &masX );
MasSh( minY, maxY, nGridY, &masY );
if( GRP.pr == 4 ) masY = 4;
if( GRP.pr == 2 ) minX = GRP.gr.H[9];//globnas.a11;

if( GRP.pr != 2 )
{
CRect space;
space.left=lX+(double)GRP.gr.Q_min*GRP.k_kol/((double)masX/(stepX) );
space.top=lY;
space.right=lX+(double)GRP.gr.Q_max*GRP.k_kol/((double)masX/(stepX) );
space.bottom=rY;
dc.FillRect(&space,&CBrush( RGB( 255, 133, 64 ) ) );


}
/*
else {
dc.FillRect( lX, rY-(double)globnas.a42*nasprop.k_kol/((double)masY/(stepY) ),
rX, rY-(double)globnas.a44*nasprop.k_kol/((double)masY/(stepY) ),
TBrush( TColor( 255, 133, 64 ) ) );
}
*/
/*  for( i = 0; i<nGridY; i++ )   // сетка
{
n = ( i % 2 ) * 5;
if( i < nGridX )
{
dc.MoveTo( lX+stepX+(stepX*i), lY ); dc.LineTo( lX+stepX+(stepX*i), rY+n );
}
dc.MoveTo( lX-n, rY-stepY-(stepY*i) ); dc.LineTo( rX+n, rY-stepY-(stepY*i) );
}

CPen p2(PS_SOLID,2,RGB(0,0,0 ));
dc.SelectObject(&p2);
dc.MoveTo( lX, lY );  dc.LineTo( lX, rY ); // ось Y
dc.MoveTo( lX, rY );  dc.LineTo( rX, rY ); // ось X

dc.SetTextColor( RGB(0,0,0 ) );
for( i = 0; i<10; i++ )             // цифры по X
{
sprintf( buf, "%5lu", i*masX*2 );
if ( i ) dc.TextOut( lX-stepX/2+(stepX*2*i)-10, rY+stepY/2, buf );
//    dc.TextOut( lX-stepX/2+(stepX*2*i), rY+stepY/2, buf );
}
for( i = 0; i<nGridY/2; i++ )       // цифры по Y
{
sprintf( buf, "%5u", i*masY*2 );
//    dc.TextOut( lX-35, rY-stepY/2-(stepY*2*i), buf );
dc.TextOut( lX-35, rY-stepY/2-(stepY*2*i), buf );
}
CPen p3(PS_SOLID,2,col[GRP.pr-1]);
dc.SelectObject( &p3);
Appr( GRP.H, GRP.Q, &pog );  DrawGraph( r0, r1, r2, &dc, lX, rY );
CPen p4(PS_SOLID,1,RGB(215,0,215));
dc.SelectObject( &p4);//Green
if( ( GRP.men != 4 ) && ( GRP.prop1 || GRP.prop2 ) )  // пересчет
{
double p1 = 1.0, p2 = 1.0;
if( GRP.prop1 == 1 ) p2 = (double)GRP.c_vr / (double)GRP.c_vr1;
if( GRP.prop1 == 2 ) p1 = (double)GRP.diam / (double)GRP.diam1;
if( p1 && p2 )
{
switch( GRP.pr )
{
case 1:
rr0 = r0 * pow( p1, 2 ) * pow( p2, 2 );
rr1 = r1 * p1 * p2;
rr2 = r2 - GRP.S;
break;
case 2:
rr0 = r0 * p1 * p2;
rr1 = r1 / ( p1 * p2 );
rr2 = r2 / ( pow( p1, 3 ) * pow( p2, 3 ) );
break;
case 3:
rr0 = r0 * pow( p2, 3 ) * pow( p1, 5 );
rr1 = r1 * pow( p2, 2 ) * pow( p1, 4 );
rr2 = r2 * p2 * pow( p1, 3 );
break;
case 4:
rr0 = r0;
rr1 = r1 / ( p1 * p2 );
rr2 = r2 / ( pow( p1, 2 ) * pow( p2, 2 ) );
break;
}
DrawGraph( rr0, rr1, rr2, &dc, lX, rY );
}
}
/*if( men == 4 ) {
dc.SelectObject( &CPen(PS_DOT,1,RGB(215,0,215)));//Green
DrawGraph( 0.0, 0.0, _rr2, &dc, lX, rY );
}
*/
/*   if ((frst == 1)&& (GRP.men == 4))//((rButtonDown)&& (men == 4))
{
static CPoint oldP = pmode;
//oldMode = dc.SetROP2( R2_NOTXORPEN );
oldP=p;
p.x = (double)( p.x - lX ) * (double)masX / stepX;
p.y = (double)( rY - p.y ) * (double)masY / stepY;
pmode = p;
//if( frst ) prdim( oldP,&dc );
if( pmode.x>0 && pmode.y>0 ) prdim( pmode,&dc);
p=oldP;
//oldP = pmode;
//frst = 1;
//rButtonDown = 0;
//RubberBand( oldp,&dc );
//dc.SetROP2( oldMode );
}

dc.SelectObject(def);
}
else
{
dc.TextOut(llx+ 15,lly+ 60, _TR("Номинальный диаметр рабочего колеса, мм") );
sprintf( buf, "%4d", (int)GRP.diam1 ); dc.TextOut(llx+ 470,lly+ 60, buf );
dc.TextOut(llx+ 15,lly+ 80, _TR("Номинальная частота вращения, 1/мин") );
sprintf( buf, "%4d", (int)GRP.c_vr1 ); dc.TextOut( llx+470,lly+ 80, buf );
if( GRP.men != 4 ) {
dc.TextOut(llx+270,lly+ 5, _TR("Характеристика :") );
dc.TextOut(llx+ 390,lly+ 5, title[GRP.pr - 1] );
dc.SetTextColor( RGB(0,0,0) );
dc.TextOut(llx+15,lly+ 40, _TR("Количество насосов - ") );
sprintf( buf, "%4d", GRP.k_kol ); dc.TextOut(llx+ 470,lly+ 40, buf );
dc.TextOut(llx+ 15, lly+100, _TR("Максимальная ошибка аппроксимации, %") );
sprintf( buf, "%1.2lf", pog );      dc.TextOut(llx+ 470, lly+100, buf );
if( GRP.men == 2 ) { // пересчет по частоте
dc.TextOut( llx+15,lly+ 120, _TR("Новое значение частоты вращения, 1/мин") );
sprintf( buf, "%4d", GRP.c_vr ); dc.TextOut(llx+ 470,lly+ 120, buf );
}
if( GRP.men == 3 ) { // пересчет по диаметру
dc.TextOut(llx+ 15,lly+ 120, _TR("Новое значение диаметра рабочего колеса, мм") );
sprintf( buf, "%4d", GRP.diam ); dc.TextOut(llx+ 470,lly+ 120, buf );
}
dc.TextOut(llx+ 200,lly+ 140, _TR("Рабочая зона :") );
dc.TextOut(llx+ 15,lly+ 180,  _TR("Левая  граница: ") );
dc.TextOut(llx+ 15,lly+ 200,  _TR("Правая граница: ") );
dc.TextOut(llx+ 130,lly+ 180,  _TR("напор -              м,") );
dc.TextOut(llx+ 130,lly+ 200,  _TR("напор -              м,") );
dc.TextOut(llx+ 250,lly+ 180,  _TR("расход -              т/ч") );
dc.TextOut(llx+ 250,lly+ 200,  _TR("расход -              т/ч") );
//      dc.TextOut( 15, 180,  _TR("Левая  граница: напор -      м,   расход -      т/ч") );
//      dc.TextOut( 15, 200,  _TR("Правая граница: напор -      м,   расход -      т/ч") );
if( GRP.pr != 2 ) {
int a = GRP.Nmin * GRP.k_kol;
int b = r0 + r1*a + r2*pow( a, 2 );
for( i = 0; i<2; i++ ) {
sprintf( buf, "%5d", a ); dc.TextOut(llx+ 300,lly+ 180+20*i, buf );
sprintf( buf, "%5d", b ); dc.TextOut(llx+ 170,lly+ 180+20*i, buf );
a = GRP.Nmax*GRP.k_kol;
b = r0+r1*a+r2*pow(a,2);
}
}
char *par1[4] = { "H - напор, м ;", "H - напор, м ;",  "N - мощность, кВт ;",
_TR("КПД - коэффициент полезного действия, % ;") };
char *par2[4] = { "H", "H", "N", _TR("КПД") };
dc.TextOut(llx+  60,lly+ 230, _TR("Уравнение характеристики :") );
dc.TextOut(llx+ 295,lly+ 230, "= r0 + r1*Q + r2*Q*Q " );
dc.TextOut(llx+ 265,lly+ 230, par2[GRP.pr-1] );
dc.TextOut(llx+  15,lly+ 270, _TR("где   Q - расход,  т/ч ; ") );
dc.TextOut(llx+ 170,lly+ 270, par1[GRP.pr-1] );

char *buf1="r0 = %10.7lf,м   r1 = %10.7lf, м/(т/ч)   r2 = %10.7lf, м/(т/ч)^2";

sprintf(s, buf1, r0, r1, r2);
dc.TextOut(llx+15,lly+ 290, s );

if( GRP.men == 2 || GRP.men == 3 ) {
dc.TextOut(llx+180,lly+ 330, _TR("Для пересчета :") );
sprintf(s, buf1, rr0, rr1, rr2);
dc.TextOut(llx+15,lly+ 350, s );
}
}
else {
dc.TextOut(llx+ 270,lly+ 5, _TR("Расчет диаметра обточки") );
dc.TextOut(llx+ 15,lly+ 100, _TR("Диаметр обточенного рабочего колеса,  мм") );
sprintf( buf, "%4.0lf", d_obt ); dc.TextOut(llx+ 470,lly+ 100, buf );
sprintf( buf, _TR("Режимная точка :  Q = %5d, т/ч ;    H = %5d, м"), pmode.x, pmode.y);
dc.TextOut(llx+ 15,lly+ 140, buf );
sprintf( buf,  _TR("Точка на номинальной характеристике : Q = %5.0lf, т/ч ;    H = %5.0lf, м"), qq2, hh2);
dc.TextOut(llx+ 15,lly+ 160, buf );
dc.TextOut(llx+ 15,lly+ 200, _TR("Коэффициент быстроходности, об/мин") );
sprintf( buf, "%5.0lf", kb ); dc.TextOut( 470, 200, buf );
dc.TextOut(llx+ 15,lly+ 220, _TR("Полученная относительна величина обточки :") );
sprintf( buf, "%8.6lf", del_p ); dc.TextOut( 440, 220, buf );
dc.TextOut(llx+ 15,lly+ 240, _TR("Допустимая относительна величина обточки :") );
sprintf( buf, "%8.4lf", del_d ); dc.TextOut( 440, 240, buf );
if( del_p <= del_d ) {
dc.SetTextColor( RGB(0,255,0) );
dc.TextOut(llx+ 150,lly+ 280, _TR("Обточка допустима") );
}
else {
dc.SetTextColor( RGB(255,0,0) );
dc.TextOut(llx+ 150,lly+ 280, _TR("Обточка недопустима") );
}
}
}

void mashq( double *arr1, double *arr2, short i );
void CGraphR::mashq( double *arr1, double *arr2, short i )
{
if( i != 3 ) inihq( GRP.gr.Q, arr2, GRP.k_kol );
else         inihq( GRP.gr.H,  arr2, GRP.k_kol );
switch( i )
{
case 0:{ inihq( GRP.gr.H,  arr1, 1 );     break;}
case 1:{ inihq( GRP.gr.N, arr1, GRP.k_kol ); break;}
case 2:{ inihq( GRP.gr.KPD, arr1, 1 );     break;}
case 3:{ inihq( GRP.gr.Q, arr1, GRP.k_kol ); break;}
}
}
//----------------------------------------------------------------------------

/*  int n = 0, i;
char buf[80], s[80];

char *title[4] = { _TR("Напор-расход"),    _TR("Расход-напор"),
_TR("Мощность-расход"), _TR("КПД-расход")    };
*/



void CGraphR::OnCancel()
{
	if (!gr_rap) //График
	{
		CGraphTemplate::OnCancel();
	}
	else //Отчет
	{
		CGraphR::OnRep();
	}
}
