#pragma once

#include "Templ.h"
/////////////////////////////////////////////////
// Необходимые структуры ////////////////////////
/////////////////////////////////////////////////
struct SWITCH_DATA
{
  TDist X0;
  TDist Y0;
  TDist Z0;
  TDist Xt;
  TDist Yt;   
  TDist Height;
};
struct DEVICE_DATA
{
  TDist X0;
  TDist Y0;
  TDist Radius;
  int Angle;
  TDist Height;
  TDist Rad;
  char  Text[256];
  TDist Xt;
  TDist Yt; 
  RECTAN Bounds;
};
struct FLOWMETER_DATA
{
  TDist X0;
  TDist Y0;
  TDist Radius;
  int Angle;
  TDist Rad;
  char  Text[256];
  TDist Xt;
  TDist Yt; 
  RECTAN Bounds;
};
struct TERMO_DATA
{
  TDist X0;
  TDist Y0;
  TDist Radius;
  int Angle;
  TDist Height;
  TDist Width;
  TDist Rad;  
  char  Text[256];
  TDist Xt;
  TDist Yt; 
  RECTAN Bounds;
};
/////////////////////////////////////////////////
// Необходимые классы ////////////////////////
/////////////////////////////////////////////////
class CBaloonTube
{
public:
  CBaloonTube(TDist Radius,TDist Height,double Phase);
  ~CBaloonTube();
  void Animate();
  void Update();
protected:
  double xp,yp;
  bool ypos;
  TDist radius,height;
  double storona,step;
};
//------------------------------------------------------------------
class CBattery : public CTempl
{
public:
  CBattery(ELEMENT_DATA *bat_create,BOOL FromBounds=0);
  CBattery(const CBattery &prev_bat); //конструктор копий
  ~CBattery();
  void GetData(ELEMENT_DATA *bat_data)const;
  int GetNodeKol() const;
  CShemaNode GetNode(int number) const;
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA *bat_update);
  void UpdateFromBounds(ELEMENT_DATA *bat_update);
protected:
  int kol;
  double r1;
};
//------------------------------------------------------------------
class CTurn : public CTempl
{
public:
  CTurn(ELEMENT_DATA *turn_create,BOOL FromBounds=0);
  CTurn(const CTurn &prev_turn); //конструктор копий
  ~CTurn();
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA *turn_update);
  void UpdateFromBounds(ELEMENT_DATA *turn_update);
  void GetData(ELEMENT_DATA *turn_data)const;
  int GetNodeKol() const;
  CShemaNode GetNode(int number) const;
protected:
  int angle1;
  bool right;
};
//------------------------------------------------------------------
class CTrinity : public CTempl
{
public:
  CTrinity(ELEMENT_DATA * tri_create,BOOL FromBounds=0);
  CTrinity(const CTrinity &prev_tri); //конструктор копий
  ~CTrinity();
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA *tri_update);
  void UpdateFromBounds(ELEMENT_DATA *tri_update);
  void GetData(ELEMENT_DATA *tri_data)const;
  int GetNodeKol() const;
  CShemaNode GetNode(int number) const;
protected:
  TDist x1,y1;
  int ang;
  bool right;
};
//------------------------------------------------------------------
class CSwitcher : public CPribor
{
public:
  CSwitcher(SWITCH_DATA *switch_create);
  ~CSwitcher();
  void Update(SWITCH_DATA *switch_update);
  void GetData(SWITCH_DATA *switch_data)const;
  void Draw();
  void Animate();
  bool Click();
  bool ON();
  void Open(bool open) {on = open;};
  RECTAN GetBound();
  void MoveText(RECTAN * newPos);
  void ResetLocation(); 
protected:
  TDist z0;
  bool on;
};
//------------------------------------------------------------------
class CDevice : public CPribor
{
public:
  CDevice(DEVICE_DATA *dev_create);
  ~CDevice();
  void Update(DEVICE_DATA *dev_update);
  void GetData(DEVICE_DATA *dev_data)const;
  void Draw();
  void Show(CTemplWnd * t_wnd,bool show=1); 
  //void Animate();
  bool Click();
  bool ON();    
protected:
  TDist rad;
  //TDist z0;
  bool on;  
};
//------------------------------------------------------------------
class CFlowmeter : public CPribor
{
public:
  CFlowmeter(FLOWMETER_DATA *flow_create);
  ~CFlowmeter ();
  void Update(FLOWMETER_DATA *flow_update);
  void GetData(FLOWMETER_DATA *flow_data)const;
  void Draw();
  void Show(CTemplWnd * t_wnd,bool show=1); 
  //void Animate();
  bool Click();
  bool ON();
  void ResetLocation();
protected:
  TDist rad;
  bool on;  
};
//------------------------------------------------------------------
class CHeatCounter: public CFlowmeter 
{
public:
  CHeatCounter(FLOWMETER_DATA *flow_create);
  void Draw();    
};
//------------------------------------------------------------------
class CTermo : public CPribor
{
public:
  CTermo(TERMO_DATA *termo_create);
  ~CTermo();
  void Update(TERMO_DATA *termo_create);
  void GetData(TERMO_DATA *termo_data)const;
  void Draw();
  void Show(CTemplWnd * t_wnd,bool show=1); 
  //void Animate();
  bool Click();
  bool ON();  
protected:
  double rad;
  double radius1; 
  TDist width;  
  bool on;  
};
//------------------------------------------------------------------
class CDescreetDevice : public CTempl
{
public:
  CSwitcher * sw;
  bool PointInTxtRgn(MOUSEXY *m);
  bool HasSubDevice();  
  void MoveText(RECTAN * newPos);
  RECTAN GetTxtBounds();  
  bool PointInClickRgn(MOUSEXY *m);
  bool OnClickInClickRgn(MOUSEXY *m);
  void SetGroupON(bool on);
};
//------------------------------------------------------------------
class CVentil : public CDescreetDevice
{
public:
  CVentil(ELEMENT_DATA *vent_create,BOOL FromBounds=0);
  CVentil(const CVentil &prev_vent); //конструктор копий
  ~CVentil();
  virtual void Draw(BOOL ShowNode);
  virtual void Animate();
  virtual void Update(ELEMENT_DATA * vent_update);
  virtual void UpdateFromBounds(ELEMENT_DATA *vent_update);
  virtual void GetData(ELEMENT_DATA * vent_data)const;
  int GetNodeKol() const;
  void Open(int op) {opening = op;}
  CShemaNode GetNode(int number) const;
  virtual void ConnectionCreate(CTemplWnd * newConnectWnd);
protected:
  int kol;
  int position;
  BOOL Infocus;
  int opening;
};
//------------------------------------------------------------------
class CBaloon
{
public:
  CBaloon(TDist X0,TDist Y0,TDist X,TDist Y,TDist Radius,TDist Height,double *Sin1,double *Cos1,double Phase);
  ~CBaloon();
  void Animate();
  void Update();
protected:
  double sin1[10];
  double cos1[10];
  int cnt;
  double xp,yp;
  TDist x0,y0,x,y,radius,height;
  unsigned char pos;
  double storona,step,xx0,yy0;
};
//------------------------------------------------------------------
class CNasos : public CDescreetDevice
{
public:
  CNasos(ELEMENT_DATA * nas_create,BOOL FromBounds=0);
  CNasos(const CNasos &prev_nas); //конструктор копий
  ~CNasos();
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA * nas_update);
  void UpdateFromBounds(ELEMENT_DATA * nas_update);
  void GetData(ELEMENT_DATA * nas_data) const;
  int GetNodeKol() const;
  CShemaNode GetNode(int number) const;  
  virtual void ConnectionCreate(CTemplWnd * newConnectWnd);
protected:
  CBaloon **ball;
  TDist height;
  unsigned char anim;
  int position;
  double sin1[10];
  double cos1[10];
  int ball_kol; 
};
//------------------------------------------------------------------
class CTube : public CTempl
{
public:
  CTube(ELEMENT_DATA *tube_create,BOOL FromBounds=0);
  CTube(const CTube &prev_tube); //конструктор копий
  ~CTube();
  void Draw(BOOL ShowNode);
  virtual void Animate();
  virtual void Update(ELEMENT_DATA  *tube_update);
  virtual void UpdateFromBounds(ELEMENT_DATA  *tube_update);
  void GetData(ELEMENT_DATA  *tube_data) const;
  int GetNodeKol() const;
  CShemaNode GetNode(int number) const;
  virtual bool HasText();
  bool PointInTxtRgn(MOUSEXY *m);
  void MoveText(RECTAN * newPos);
  RECTAN GetTxtBounds();
  virtual void ConnectionCreate(CTemplWnd * newConnectWnd);
protected:
  int ang;  
  unsigned char anim;
  unsigned char pribor;
  bool right;
  char text[256];
  CBaloonTube ** ballT;
  int ball_kol;
  CDevice * dev;
  CTermo * termo;
  CFlowmeter * flmeter;
  CHeatCounter * heatcntr;  
};
//------------------------------------------------------------------
class CDrossel : public CTube
{
public:
  CDrossel(ELEMENT_DATA *dros_create,BOOL FromBounds=0);
  CDrossel(const CDrossel &prev_dros); //конструктор копий
  bool HasText();
//  ~CDrossel();
  void Draw(BOOL ShowNode);
  void Update(ELEMENT_DATA *dros_update);
  void UpdateFromBounds(ELEMENT_DATA *dros_update);
//protected:
};
//------------------------------------------------------------------
class CAirBattery : public CDescreetDevice
{
public:
  CAirBattery(ELEMENT_DATA *airbat_create,BOOL FromBounds=0);
  CAirBattery(const CAirBattery &prev_airbat); //конструктор копий
  ~CAirBattery();
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA *airbat_update);
  void UpdateFromBounds(ELEMENT_DATA * airbat_update);
  void GetData(ELEMENT_DATA * airbat_data)const;
  int GetNodeKol() const;
  CShemaNode GetNode(int number) const;  
  virtual void ConnectionCreate(CTemplWnd * newConnectWnd);
protected:
  int kol;
  TDist altitude;
  int position;
};
//------------------------------------------------------------------
class CVodoVodo : public CTempl
{
public:
  CVodoVodo(ELEMENT_DATA * vodobat_create,BOOL FromBounds=0);
  CVodoVodo(const CVodoVodo &prev_vodobat); //конструктор копий
  ~CVodoVodo();
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA * airbat_update);
  void UpdateFromBounds(ELEMENT_DATA * airbat_update);
  void GetData(ELEMENT_DATA *  airbat_data)const;
  int GetNodeKol() const;
  CShemaNode GetNode(int number) const;  
protected:
  TDist altitude;
  TDist radius1;
  TDist x1,y1,x2,y2;
};
//------------------------------------------------------------------
class CSoplo : public CTempl
{
public:
  CSoplo(ELEMENT_DATA *soplo_create,BOOL FromBounds=0);
  CSoplo(const CSoplo &prev_soplo); //конструктор копий
  ~CSoplo();
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA *soplo_update);
  void UpdateFromBounds(ELEMENT_DATA *soplo_update);
  void GetData(ELEMENT_DATA *soplo_data)const;
  int GetNodeKol() const;
  CShemaNode GetNode(int number) const;
protected:
  TDist x1,y1;
  CBaloonTube ** ballT;
  CBaloonTube ** ballT90;
  int ball_kol;
  int ball_kol90;
};
//------------------------------------------------------------------
class CConnector : public CTempl
{
public:
  CConnector(ELEMENT_DATA *conn_create,BOOL FromBounds=0);
  CConnector(const CConnector &prev_conn); //конструктор копий
  ~CConnector ();
  void GetData(ELEMENT_DATA *conn_data)const;
  int GetNodeKol() const;
  CShemaNode GetNode(int number) const;
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA *conn_update);
  void UpdateFromBounds(ELEMENT_DATA *conn_update);
  bool HasText();
  bool PointInTxtRgn(MOUSEXY *m);
  RECTAN GetTxtBounds();
  void MoveText(RECTAN * newPos);
  virtual void ConnectionCreate(CTemplWnd * newConnectWnd);
protected:
  CDevice * dev;
  char text[256];
  unsigned char anim;
};
//------------------------------------------------------------------
class CKran : public CVentil
{
public:
  CKran(ELEMENT_DATA* kran_create,BOOL FromBounds=0);
  CKran(const CKran &prev_kran); //конструктор копий
  ~CKran();
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA* kran_update);
  void UpdateFromBounds(ELEMENT_DATA* kran_update);
  int GetNodeKol() const;
  CShemaNode GetNode(int number) const;
protected:  
  CBaloonTube ** ballT;
  int ball_kol;
};
//------------------------------------------------------------------
class CLabel : public CTempl
{
public:
  CLabel(ELEMENT_DATA *label_create,BOOL FromBounds=0);
  CLabel(const CLabel &prev_label); //конструктор копий
  ~CLabel();
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA *label_update);
  void UpdateFromBounds(ELEMENT_DATA *label_update);
  void GetData(ELEMENT_DATA *label_data)const;
protected:
  char text[256];
  TCOLOR front,back;
  bool anim;
};
//------------------------------------------------------------------
class CExtTermo : public CTempl
{
public:
  CExtTermo(ELEMENT_DATA *exterm_create,BOOL FromBounds=0);
  CExtTermo(const CExtTermo &prev_exterm); //конструктор копий
  ~CExtTermo();
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA *exterm_update);
  void UpdateFromBounds(ELEMENT_DATA *exterm_update);
  void GetData(ELEMENT_DATA *exterm_data)const;
  int GetNodeKol() const;
  CShemaNode GetNode(int number) const;
  bool HasText();
  bool PointInTxtRgn(MOUSEXY *m);
  void MoveText(RECTAN * newPos);
  RECTAN GetTxtBounds();
  virtual void ConnectionCreate(CTemplWnd * newConnectWnd);
protected:  
  CTermo * termo;
};
//------------------------------------------------------------------
class CConnection : public CTempl
{
public:
  CConnection(ELEMENT_DATA *cotion_create,BOOL FromBounds=0);
  CConnection(const CConnection &prev_cotion); //конструктор копий
  ~CConnection();
  void Draw(BOOL ShowNode);
  void Animate();
  void Update(ELEMENT_DATA *cotion_update);
  void UpdateFromBounds(ELEMENT_DATA *cotion_update);
  void GetData(ELEMENT_DATA *cotion_data)const;
protected:
  TDist xt;
  TDist yt;
  TDist x1;
  TDist y1;
  double rad;
};
//------------------------------------------------------------------
