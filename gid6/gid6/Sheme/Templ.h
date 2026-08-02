#pragma once

#include "math.h"

#include "Common.h"
#include "temlpwnd.h"

void SetColorUb(TCOLOR Color);
//Поворот на целое число градусов
GLvoid RotateTo0(double x0,double y0);
GLvoid RotateTo90(double x0,double y0);
GLvoid RotateTo180(double x0,double y0);
GLvoid RotateTo270(double x0,double y0);
GLvoid RotateOn10Y();
GLvoid RotateOnM10Y();
GLvoid RotateOnM10X();
GLvoid RotateOn90Y();
GLvoid RotateOnM90Y();
GLvoid RotateOn180Z();
GLvoid Rotate90();//Надо ли?


void DrawBound(const RECTAN *bound);
void DrawPoint(const TDist x0,const TDist y0,double z);
void DrawPointBound(const TDist x0,const TDist y0,const TDist xt,const TDist yt,const TDist x,const TDist y,const TDist x1,const TDist y1,double z);

void DrawNode(const CShemaNode node);
void auxTube(double x0, double y0, double &x, double &y, double radius,int angle,double height);
void auxHalfTube(double x0, double y0, double &x, double &y, double radius,int angle,double height);
void auxCylinder(double radius,double height);
void auxCone(double radius0,double radius,double height);
void auxTurn(double x0, double y0, double &x, double &y,double radius,int &angle_return,bool right=true,double height=4);
//void auxTrinity(double x0, double y0, double &x, double &y,double radius,int angle,double height,double &x1, double &y1,int &ang,bool right,double part=1);
void auxTrinity(double x0, double y0,double radius,int angle,double height,bool right);
void auxVentil(double x0, double y0,double radius,int angle);
void VentSalnik(double X1,double Y1,double X2,double Y2,double radius,bool right);
void VentRog(double X3,double Y3,double height,int angle);
void VentShturval(double X2,double Y2,double radius,double height,double part,int degree,int angle,int kol=4);
	void auxConnector(double x0, double y0,double radius,double height,int angle);
	//void auxKran(double x0, double y0, double &x, double &y,double radius,int angle,double height,int &i,byte &opening);
	void auxBattery(double x0, double y0,double radius,int kol,double height,double r1);
	void BatRebro(double x0,double y0,double radius,double height,double altitude);
	void BatGayka(double x0,double y0,int angle,double radius,double alt);
	void auxAirBattery(double x0, double y0,double radius,int quantity,double altitude);
	void AirTubes(int quantity,double radius,double altitude,GLUquadricObj *quadObj);
	void auxSoplo(TCOLOR ColdWater,TCOLOR HotWater,double x0, double y0,double radius);
	void auxWaterBattery(double x0, double y0,double radius,double altitude,double radius1);
	void auxWaterTurn(double radius);
	void auxDrossel(double x0, double y0, double radius,int ang,double height,double part);
	void auxNasos(TCOLOR ColdWater,TCOLOR HotWater,double x0, double y0,double radius,int angle,double height, unsigned char anim=1);
	void auxTermometer(double x, double y,double rad,double width,double radius1);
	void auxDrawDistrict(const double x0,const double y0,const double xt,const double yt,const double x,const double y,const double x1,const double y1,double rad);
// Строим векторный шрифт
GLvoid BuildFont(HDC hDC,FONT_DATA &font_data,GLuint &base,GLYPHMETRICSFLOAT * gmf,LOGFONT * lf=NULL,COLORREF * color=NULL);           // Строим растровый шрифт
GLvoid glPrint(GLYPHMETRICSFLOAT * gmf,FONT_DATA &font_data,GLuint &base,TCOLOR * cl_text,TCOLOR * cl_back,bool show_back,long x,long y,double z,const char *fmt, ...);     // Функция вывода текста в OpenGL
GLvoid glPrintFormatted(GLYPHMETRICSFLOAT * gmf,FONT_DATA &font_data,GLuint &base,TCOLOR * cl_text,TCOLOR * cl_back,const double x,const double y,const double z,const long x1,const long y1,const char *fmt, ...);     // Функция вывода форматированного  текста в OpenGL
RECTAN GetPrintRect(GLYPHMETRICSFLOAT * gmf,FONT_DATA &font_data,const long x,const long y,const char *fmt, ... );     // Функция вывода текста в OpenGL
RECTAN GetFontRect(GLYPHMETRICSFLOAT * gmf,FONT_DATA &font_data,double lenth,int angle, double &x,double &y,const char *fmt, ... );     // Функция вывода текста в OpenGL
// Удаление шрифта
GLvoid KillFont(GLuint &base);                   // Удаление шрифта	
//векторная графика
void Get2Points(VECTORXY &first,VECTORXY &last,const VECTORXY lenth);
//
long Min(const long one,const long two);
long round_to_dig(const double val);

class CTempl
{
public:
	CTempl();
	~CTempl();
	virtual void Draw(BOOL ShowNode);
	void DrawNodes(BOOL ShowNode);
	virtual void Animate();
	virtual void Update(ELEMENT_DATA *update);
	virtual void UpdateFromBounds(ELEMENT_DATA *update);
	virtual bool PointInRgn(MOUSEXY *m);
	virtual void SetFocus(BOOL focus);
	virtual void GetData(ELEMENT_DATA *data) const;
	virtual int GetNodeKol() const;
	int GetType() const;
	GROUP_DATA GetGroup() const;
	void SetGroupON(bool on);
	bool GetGroupON() const;
	void InitConnect();
	void CopyConnect(const CTempl * prev);
	bool IsConnected(int number) const;
	bool Connect(int number) const;
	bool DisConnect(int number) const;
	void SetUserParam(ELEMENT_DATA *update);
	void SetTypeParam(ELEMENT_DATA *update,int Type);
	COMMON_DATA GetCommonParam() const;
	virtual CShemaNode GetNode(int number) const;	
	virtual void ConnectionCreate(CTemplWnd * newConnectWnd);
	void ShowText(bool show=true);
	bool SwitchShowText();
	bool GetShowText();
	virtual bool PointInTxtRgn(MOUSEXY *m);
	virtual bool HasSubDevice();	
	virtual bool HasText();	
	virtual void MoveText(RECTAN * newPos);
	virtual RECTAN GetTxtBounds();
	virtual bool PointInClickRgn(MOUSEXY *m);
	virtual bool OnClickInClickRgn(MOUSEXY *m);
protected:
	bool groupOn_mirror;
	bool showtext;
	TDist x,y,x0,y0,height,radius;
	RECTAN bound;
	COMMON_DATA cd;
	BOOL Focused;
	GROUP_DATA group;
	int angle;
	bool * connected;
	CTemplWnd * ConnectWnd;
};
class CPribor : public CTempl
{
public:
	TDist Xt;
	TDist Yt;
	virtual RECTAN GetBound();
	void MoveText(RECTAN * newPos);	
	virtual void ResetLocation();		
	char text[256]; //чтобы иметь доступ
protected:
	virtual void Update();
	virtual void Draw();	
};
