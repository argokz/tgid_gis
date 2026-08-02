#include "stdafx.h"
#include "Templ.h"
void SetColorUb(TCOLOR Color){glColor3ub(Color.R,Color.G,Color.B);}

/*void ColdWater(TCOLOR ColdWtr) {glColor3ub(ColdWtr.R,ColdWtr.G,ColdWtr.B);}
void HotWater(TCOLOR HotWtr) {glColor3ub(HotWtr.R,HotWtr.G,HotWtr.B);}
void NodeColor(TCOLOR ColdWtr){glColor3d(1,0,0);}
void AnimColor(TCOLOR ColdWtr){glColor3d(1,1,0);}
void DisplayColor(TCOLOR ColdWtr){glColor3d(1,1,1);}
void ForeGroundColor(TCOLOR ColdWtr){glColor3d(0,0,1);}
void BackGroundColor(TCOLOR ColdWtr){glColor3d(0,0,0);} 
*/
long Min(const long one,const long two)
{
  int result=one;
  if (two<result)result=two;
  return result;
}
GLvoid Rotate90()
{
  //glRotated(90,0,1,0);
  //вОКРУГ ОСИ y
  const GLfloat mY[16]={0,0,-1,0, 0,1,0,0, 1,0,0,0, 0,0,0,1};
  
  //вОКРУГ ОСИ X
  const GLfloat mX[16]={1,0,0,0, 0,0,1,0, 0,-1,0,0,  0,0,0,1};
  //вОКРУГ ОСИ Z
  const GLfloat mZ[16]={0,1,0,0, -1,0,0,0, 0,0,1,0,  0,0,0,1};
  glMultMatrixf(mY);
}
GLvoid RotateTo0(double x0,double y0)
{
  const GLfloat m[16]={0,1,0,0, 0,0,1,0, 1,0,0,0, x0,y0,0,1};
  glMultMatrixf(m); 
}
GLvoid RotateTo90(double x0,double y0)
{
  const GLfloat m[16]={-1,0,0,0, 0,0,1,0, 0,1,0,0, x0,y0,0,1};
  glMultMatrixf(m); 
}
GLvoid RotateTo180(double x0,double y0)
{
  const GLfloat m[16]={0,-1,0,0, 0,0,1,0, -1,0,0,0, x0,y0,0,1};
  glMultMatrixf(m); 
}
GLvoid RotateTo270(double x0,double y0)
{
  const GLfloat m[16]={1,0,0,0, 0,0,1,0, 0,-1,0,0, x0,y0,0,1};
  glMultMatrixf(m); 
}
GLvoid RotateOn10Y()
{
  const GLfloat mY[16]={0.985,0,-0.174,0, 0,1,0,0, 0.174,0,0.985,0, 0,0,0,1};
  glMultMatrixf(mY);  
}
GLvoid RotateOnM10Y()
{
  const GLfloat mY[16]={0.985,0,0.174,0, 0,1,0,0, -0.174,0,0.985,0, 0,0,0,1};
  glMultMatrixf(mY);  
}
GLvoid RotateOnM10X()
{
  const GLfloat mX[16]={1,0,0,0, 0,0.985,0.174,0, 0,-0.174,0.985,0, 0,0,0,1};
  glMultMatrixf(mX);  
}
GLvoid RotateOn90Y()
{
  //вОКРУГ ОСИ y
  const GLfloat mY[16]={0,0,-1,0, 0,1,0,0, 1,0,0,0, 0,0,0,1};
  glMultMatrixf(mY);  
}
GLvoid RotateOnM90Y()
{
  //вОКРУГ ОСИ y
  const GLfloat mY[16]={0,0,1,0, 0,1,0,0, -1,0,0,0, 0,0,0,1};
  glMultMatrixf(mY);  
}
GLvoid RotateOn180Z()
{
  const GLfloat mZ[16]={-1,0,0,0, 0,-1,0,0, 0,0,1,0,  0,0,0,1};
  glMultMatrixf(mZ);
}
CTempl::CTempl()
{
  Focused=0;
  group.group=0;
  groupOn_mirror=true;
  group.groupOn=true;
  ConnectWnd=NULL;
  showtext=true;
}
CTempl::~CTempl(){} 
void CTempl::Draw(BOOL ShowNode){}
void CTempl::Animate(){}
void CTempl::GetData(ELEMENT_DATA *data) const
{
  data->grdat=group; 
  data->Bounds=bound;
  data->cd=GetCommonParam();
}
void CTempl::Update(ELEMENT_DATA *update)
{
  group=update->grdat;
  SetUserParam(update);
}
void CTempl::UpdateFromBounds(ELEMENT_DATA *update){}
void CTempl::DrawNodes(BOOL ShowNode)
{
  if(!ShowNode)return;
  GLdouble params[4];
  glGetDoublev(GL_CURRENT_COLOR,params); 
  glBegin(GL_TRIANGLES);
  SetColorUb(ConnectWnd->NodeColor);
  for (int i=0;i<this->GetNodeKol();i++)
  {
    if (connected[i]) continue;
    CShemaNode n=this->GetNode(i);   
    DrawNode(n);
  }
  glEnd();
  glColor4dv(params);
}
void CTempl::InitConnect() 
{
  connected=NULL;
  connected=new bool[this->GetNodeKol()];
  for (int i=0;i<this->GetNodeKol();i++) connected[i]=0;
}
void CTempl::CopyConnect(const CTempl *prev)
{
  ConnectWnd=prev->ConnectWnd;
  showtext=prev->showtext;
  connected=new bool[GetNodeKol()];
  for (int y=0;y<GetNodeKol();y++)
  {
    connected[y]=prev->connected[y];
  }
}
bool CTempl::IsConnected(int number) const
{
  if ( (number<=this->GetNodeKol())&&(number>=0) ) return connected[number];
  else return 0;
}
bool CTempl::Connect(int number) const
{
  if ( (number>this->GetNodeKol())||(number<0) ) return 0;
  else connected[number]=1;
  return 1;
}
bool CTempl::DisConnect(int number) const
{
  if ( (number>this->GetNodeKol())||(number<0) ) return 0;
  else connected[number]=0;
  return 1;
}
void CTempl::SetUserParam(ELEMENT_DATA *update)
{
  cd.UserNumber=update->cd.UserNumber;
  strcpy(cd.UserString,"");
  if (strlen(update->cd.UserString)<1000) strcpy(cd.UserString,update->cd.UserString);
}
void CTempl::SetTypeParam(ELEMENT_DATA *update,int Type)
{
  cd.Type=update->cd.Type=Type;
  /*if ( (strlen(update->cd.TypeString)>1)&&(strlen(update->cd.TypeString)<100) ) strcpy(cd.TypeString,update->cd.TypeString);
  else*/ strcpy(update->cd.TypeString,cd.TypeString);
}
COMMON_DATA CTempl::GetCommonParam() const
{
  return cd;
}
bool CTempl::PointInRgn(MOUSEXY *m)
{
return ((m->X>=bound.left)&&(m->X<=(bound.left+bound.width))&&
    (m->Y>=bound.top-bound.height)&&(m->Y<=(bound.top)));
}
bool CTempl::PointInClickRgn(MOUSEXY *m)
{
  return false;
}
bool CTempl::OnClickInClickRgn(MOUSEXY *m){return true;};
bool CTempl::PointInTxtRgn(MOUSEXY *m)
{
  return false;
}
void CTempl::SetFocus(BOOL focus)
{
  Focused=focus;
}
int CTempl::GetNodeKol() const{return 0;}
int CTempl::GetType() const{return cd.Type;}
GROUP_DATA CTempl::GetGroup() const{return group;}
void CTempl::SetGroupON(bool on){group.groupOn=on;}
bool CTempl::GetGroupON() const{return group.groupOn;}

CShemaNode CTempl::GetNode(int number) const
{
	CShemaNode node;
	node.Angle = 0;

	return node;
};

void CTempl::ConnectionCreate(CTemplWnd * newConnectWnd)
{
  ConnectWnd=newConnectWnd;
}
void CTempl::ShowText(bool show)
{
  showtext=show;
}
bool CTempl::SwitchShowText()
{
  showtext=!showtext;
  return showtext;
}
bool CTempl::GetShowText()
{
  return showtext;
}
bool CTempl::HasSubDevice()
{
  return HasText();
}
bool CTempl::HasText()
{
  return false;
}
void CTempl::MoveText(RECTAN * newPos)
{
  return;
}
RECTAN CTempl::GetTxtBounds()
{
  RECTAN ret={0,0,0,0}; 
  return ret;
}
//-------------------------------------------------------------------
RECTAN CPribor::GetBound()
{
  if (ConnectWnd!=NULL)
  {
    bound=GetPrintRect(ConnectWnd->gmf,ConnectWnd->font_data,Xt,Yt,text);   // Печать текста на экране  
  }
  return bound;
}
void CPribor::MoveText(RECTAN * newPos)
{
  Xt=newPos->left;
  Yt=newPos->top;
  if (ConnectWnd!=NULL)bound=GetPrintRect(ConnectWnd->gmf,ConnectWnd->font_data,Xt,Yt,text);  
}
void CPribor::Update()
{
  GetBound(); 
}
void CPribor::Draw()
{
  GetBound();
  if (ConnectWnd!=NULL) {
    if (showtext) {
      glPrintFormatted(ConnectWnd->gmf,ConnectWnd->font_data,ConnectWnd->base,NULL,NULL,x,y,radius+0.1,Xt,Yt,text);   // Печать текста на экране 
    }
  }
}
void CPribor::ResetLocation()
{
  if (ConnectWnd!=NULL)
  {
    double x1=x;
    double y1=y;
    bound=GetFontRect(ConnectWnd->gmf,ConnectWnd->font_data,5*radius,angle,x1,y1,text);       
    Xt=x1;
    Yt=y1;
  }
}
//-------------------------------------------------------------------


/*
bool CTempl::PointInRgn(CPoint point,double X_move,double Y_move,float depth,CRect rectclient,CWnd* wnd)
{
  CRgn Reg;
  CPoint p;
  CRect rectan;
  x_move=X_move;
  y_move=Y_move;
  dep=depth;
  rectClient=rectclient;
  if ((angle==0)||(angle==180))
  {
    p=getpoint(x0,y0-radius);
    rectan.left=p.x;
    rectan.top=p.y;
    p=getpoint(x,y0+radius);
    rectan.right=p.x;
    rectan.bottom=p.y;
  }
  if ((angle==90)||(angle==270))
  {
    p=getpoint(x-radius,y);
    rectan.left=p.x;
    rectan.top=p.y;
    p=getpoint(x+radius,y0);
    rectan.right=p.x;
    rectan.bottom=p.y;
  }
  Reg.CreateRectRgnIndirect(&rectan);
  if(Reg.PtInRegion(point))
  {
    CBrush brA;
    VERIFY(brA.CreateSolidBrush( RGB(255, 0, 0) ));  // rgnA Red
    CClientDC pDC(wnd);
    VERIFY(pDC.FrameRgn( &Reg, &brA, 2, 2 ));
    return true;
  }
  else return false;  
}
CPoint CTempl::getpoint(double x, double y)
{
CPoint point;
point.x = (1+0.5*(x - x_move)/dep)*(float)(rectClient.right)*0.5;
point.y = (float)(rectClient.bottom)-(1+(y - y_move)/dep)*(float)(rectClient.bottom)*0.5;
return point;
}
*/
/////////////////////////////////////////////////////////////
// Вспомогательные функции для прорисовки объектов //////////
///////////////////////////////////////////////////////////// 
void auxTube(double x0, double y0, double &x, double &y, double radius,int angle,double height)
{
if (radius<=0)return;
glPushMatrix();
switch (angle)
  {
  case 0 : 
    {
    RotateTo0(x0,y0);
    y=y0;
    x=x0+height;
    break;
    }
  case 90 : 
    {
    RotateTo90(x0,y0);
    x=x0;
    y=y0+height;
    break;
    }
  case 180 : 
    {
    RotateTo180(x0,y0);
    y=y0;
    x=x0-height;
    break;
    }

  case 270 : 
    {
    RotateTo270(x0,y0);
    x=x0;
    y=y0-height;
    break;
    }
  default : return;

  }
  auxCylinder(radius,height); 
glPopMatrix();
}
void auxHalfTube(double x0, double y0, double &x, double &y, double radius,int angle,double height)
{
if (radius<=0)return;
glPushMatrix();
switch (angle)
  {
  case 0 : 
    {
    RotateTo0(x0,y0);
    y=y0;
    x=x0+height;
    break;
    }
  case 90 : 
    {
    RotateTo90(x0,y0);
    x=x0;
    y=y0+height;
    break;
    }
  case 180 : 
    {
    RotateTo180(x0,y0);
    y=y0;
    x=x0-height;
    break;
    }
  case 270 : 
    {
    RotateTo270(x0,y0);
    x=x0;
    y=y0-height;
    break;
    }
  default : return;

  }
  GLdouble vert[6][3]={-radius,0,0,
             -radius,0,height,
             0,-radius,0,
             0,-radius,height,
             radius,0,0,
             radius,0,height             
            };  
  GLdouble norm[3][3]={-1,0,0,
             0,-1,0,
             1,0,0
            };  
   glBegin(GL_QUAD_STRIP);
   glNormal3dv(norm[0]);
   glVertex3dv(vert[0]);   
   //glNormal3dv(norm[0]);
   glVertex3dv(vert[1]);
   glNormal3dv(norm[1]);
   glVertex3dv(vert[2]);
   //glNormal3dv(norm[1]);
   glVertex3dv(vert[3]);
   glNormal3dv(norm[2]);
   glVertex3dv(vert[4]);
   //glNormal3dv(norm[2]);
   glVertex3dv(vert[5]);
   glEnd(); 
glPopMatrix(); 
}
void auxCylinder(double radius,double height)
{
  if (radius<=0)return;
  GLdouble vert[6][3]={-radius,0,0,
             -radius,0,height,
             0,radius,0,
             0,radius,height,
             radius,0,0,
             radius,0,height             
            };  
  GLdouble norm[3][3]={-1,0,0,
             0,1,0,
             1,0,0
            };  
  glPushMatrix();
   glBegin(GL_QUAD_STRIP);
   glNormal3dv(norm[0]);
   glVertex3dv(vert[0]);   
   //glNormal3dv(norm[0]);
   glVertex3dv(vert[1]);
   glNormal3dv(norm[1]);
   glVertex3dv(vert[2]);
   //glNormal3dv(norm[1]);
   glVertex3dv(vert[3]);
   glNormal3dv(norm[2]);
   glVertex3dv(vert[4]);
   //glNormal3dv(norm[2]);
   glVertex3dv(vert[5]);
   glEnd(); 
  glPopMatrix();
}
void auxCone(double radius0,double radius,double height)
{
  if (radius<=0)return;
  GLdouble vert[6][3]={-radius0,0,0,
             -radius,0,height,
             0,radius0,0,
             0,radius,height,
             radius0,0,0,
             radius,0,height             
            };  
  GLdouble norm[3][3]={-1,0,0,
             0,1,0,
             1,0,0
            };  
  glPushMatrix();
   glBegin(GL_QUAD_STRIP);
   glNormal3dv(norm[0]);
   glVertex3dv(vert[0]);   
   //glNormal3dv(norm[0]);
   glVertex3dv(vert[1]);
   glNormal3dv(norm[1]);
   glVertex3dv(vert[2]);
   //glNormal3dv(norm[1]);
   glVertex3dv(vert[3]);
   glNormal3dv(norm[2]);
   glVertex3dv(vert[4]);
   //glNormal3dv(norm[2]);
   glVertex3dv(vert[5]);
   glEnd(); 
  glPopMatrix();
}
void auxTurn(double x0, double y0, double &x, double &y,double radius,int &angle_return,bool right,double height)
{
  if (radius<=0)return;
  double R=height/(cosV+0.5*koeff);
  double step=R*koeff;
  double dZ=step-radius*koeff*cosV;
  double dX=radius*koeff*sinV;
  double radix=height-radius;
  glPushMatrix();
  switch (angle_return)
  {
  case 0 : 
    {
      RotateTo0(x0,y0);   
      if (!right){dX=-dX;angle_return=90;x=x0+radix;y=y0+radix;}
      else {angle_return=270;x=x0+radix;y=y0-radix;}
      break;
    }
  case 90 : 
    {
      RotateTo90(x0,y0);
      if (!right){dX=-dX;angle_return=180;x=x0-radix;y=y0+radix;}
      else {angle_return=0;x=x0+radix;y=y0+radix;}
      break;
    }
  case 180 : 
    {
      RotateTo180(x0,y0);
      if (!right){dX=-dX;angle_return=270;x=x0-radix;y=y0-radix;}
      else {angle_return=90;x=x0-radix;y=y0+radix;}
      break;
    }

  case 270 : 
    {
      RotateTo270(x0,y0);
      if (!right){dX=-dX;x=x0+radix;y=y0-radix;}
      else {angle_return=180;x=x0-radix;y=y0-radix;}
      break;
    }
  default : return;
  }
  auxCylinder(radius*faska,step);
  for (int t=0;t<10;t++)
  {
    auxCylinder(radius,step);
    if (t!=9)
    {
    glTranslated(dX,0,dZ);  
    if (dX<0)RotateOn10Y();
    else RotateOnM10Y();
    }
  }
  auxCylinder(radius*faska,step); 
  glPopMatrix();
}
/*void CTempl::auxTurn(double x0, double y0, double &x, double &y,double radius,int &angle,bool right)
{
  if (radius<=0)return;
  double radix=2*radius;
  int angle_return=0;
  GLdouble equation[4] = {0,1,0,0};
  GLdouble eq[4] = {1,0,0,0};
  int ang=0;
  glPushMatrix();
  switch (angle)
  {
  case 0 : 
    {
    if (right){glTranslated(x0,y0-radix,0);ang=0; x=x0+radix; y=y0-radix;angle_return=270;}
    else {glTranslated(x0,y0+radix,0);ang=270; x=x0+radix; y=y0+radix;angle_return=90;}
    break;
    }
  case 90 : 
    {
    if (right){glTranslated(x0+radix,y0,0);ang=90; x=x0+radix; y=y0+radix;angle_return=0;}
    else {glTranslated(x0-radix,y0,0);ang=0; x=x0-radix; y=y0+radix;angle_return=180;}
    break;
    }
  case 180 : 
    {
    if (right){glTranslated(x0,y0+radix,0);ang=180; x=x0-radix; y=y0+radix;angle_return=90;}
    else {glTranslated(x0,y0-radix,0);ang=90; x=x0-radix; y=y0-radix;angle_return=270;}
    break;
    }

  case 270 : 
    {
    if (right){glTranslated(x0-radix,y0,0);ang=270; x=x0-radix; y=y0-radix;angle_return=180;}
    else {glTranslated(x0+radix,y0,0);ang=180; x=x0+radix; y=y0-radix;angle_return=0;}
    break;
    }
  default : return;
  }
   glEnable(GL_CLIP_PLANE0);
   glEnable(GL_CLIP_PLANE1);
   if (ang!=0)glRotated(ang, 0,0,1);

   glClipPlane(GL_CLIP_PLANE0, equation);
   glClipPlane(GL_CLIP_PLANE1, eq);
    auxSolidTorus(radius,radix);
  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glPopMatrix();
  double xtmp,ytmp;
  auxTube(x0,y0,xtmp,ytmp,1.2*radius,angle,radix/5.0);
  int ann;
  ann=angle_return-180;
  if (ann<0)ann+=360;
  auxTube(x,y,xtmp,ytmp,1.2*radius,ann,radix/5.0);
  angle=angle_return;
}
*/
void auxTrinity(double x0, double y0,double radius,int angle,double height,bool right)
{
 glPushMatrix();
 switch (angle)
 {
 case 0:{RotateTo0(x0,y0);break;}
 case 90:{RotateTo90(x0,y0);break;}
 case 180:{RotateTo180(x0,y0);break;}
 case 270:{RotateTo270(x0,y0);break;}
 }
 double faska_len=radius*faskalen;
 auxCylinder(radius*faska,faska_len);
 auxCylinder(radius,2*height);
 glTranslated(0,0,2*height-faska_len);
 auxCylinder(radius*faska,faska_len);
 glTranslated(0,0,-height+faska_len);
 if (!right)RotateOn90Y();   
 else RotateOnM90Y();
 auxCylinder(radius,height);
 glTranslated(0,0,height-faska_len);
 auxCylinder(radius*faska,faska_len);
 glPopMatrix();
}
void auxVentil(double x0, double y0,double radius,int angle)
{
  double X1,Y1;
  double height=3*radius;
  auxTrinity(x0,y0,radius,angle,height,false);
  int ang1=angle+90;
  if (ang1>=360)ang1-=360;
  glPushMatrix();
  switch (angle)
  {
  case 0:
    {
      X1=x0+height;
      Y1=y0+height; 
      RotateTo90(X1,Y1);
      break;
    }
  case 90:
    {
      X1=x0-height; 
      Y1=y0+height; 
      RotateTo180(X1,Y1);
      break;
    }
  case 180:
    {
      X1=x0-height;
      Y1=y0-height; 
      RotateTo270(X1,Y1);
      break;
    }
  case 270:
    {
      X1=x0+height;
      Y1=y0-height; 
      RotateTo0(X1,Y1);
      break;
    }
  }
  auxCylinder(radius/2.0,height/3.0);  //radius/2.0 - радиус сальника height/3.0-высота до него
  glTranslated(-radius*0.77,0,0);
  auxCylinder(radius/8.0,height/4.0);  //radius/2.0 - радиус сальника height/3.0-высота до него
  glTranslated(0,0,height/4.0-radius*0.29);
  VentSalnik(radius*0.77,0,0,0,radius,false);
  glTranslated(radius*1.54,0,0); //1,54=2*0.77
  VentSalnik(-radius*0.77,0,0,0,radius,true);
  glTranslated(0,0,-height/4.0+radius*0.29);
  auxCylinder(radius/8.0,height/4.0);  //radius/2.0 - радиус сальника height/3.0-высота до него
  glPopMatrix();
}
void VentSalnik(double X1,double Y1,double X2,double Y2,double radius,bool right)
{
  double step;
  if (right)step=radius/4.0;
  else step=(-radius/4.0);
  double tol=radius/4.0;
   glBegin(GL_TRIANGLES);
    glVertex3d(X2+step,Y2+tol,-tol); // первая поверхность
    glVertex3d(X2+step,Y2+tol,tol);
    glVertex3d(X1,Y2,0); 
    glVertex3d(X2+step,Y2-tol,-tol); // вторая поверхность
    glVertex3d(X2+step,Y2-tol,tol);
    glVertex3d(X1,Y2,0);
    glVertex3d(X2+step,Y2+tol,tol); // третья поверхность
    glVertex3d(X2+step,Y2-tol,tol);
    glVertex3d(X1,Y2,0);
    glVertex3d(X2+step,Y2+tol,-tol); // четвертая поверхность
    glVertex3d(X2+step,Y2-tol,-tol);
    glVertex3d(X1,Y2,0);
   glEnd();
   glBegin(GL_QUADS);
    glVertex3d(X2+step,Y2+tol,-tol); // первая поверхность
    glVertex3d(X2+step,Y2+tol,tol);
    glVertex3d(X2+step,Y2-tol,tol); // первая поверхность
    glVertex3d(X2+step,Y2-tol,-tol);
   glEnd();
}
void VentShturval(double X2,double Y2,double radius,double height,double part,int degree,int angle,int kol)
{
  glPushMatrix();
  switch (angle)
  {
  case 0:
    {
      RotateTo90(X2,Y2);
      break;
    }
  case 90:
    {
      RotateTo180(X2,Y2);
      break;
    }
  case 180:
    {
      RotateTo270(X2,Y2);
      break;
    }
  case 270:
    {
      RotateTo0(X2,Y2);
      break;
    }
  }
  auxCylinder(0.25*radius, height/part); 
  glTranslated(0,0,height/part);
  auxCylinder(0.5*radius, 0.1*height); 
  //glTranslated(0,0,0.1*height);
  //gluDisk(quadObj, 0,0.5*radius,_slices,1); 
  for (int i=0;i<360;i+=(360/kol)) 
  {
    int tmp=i+degree;
    if (tmp>=360)tmp-=360;
    if (tmp<0)tmp+=360;
    VentRog(0,0+height*(0.1),height,tmp);
  }
  glPopMatrix();
}
void VentRog(double X3,double Y3,double height,int angle)
{
  GLUquadricObj *quadObj;
  quadObj = gluNewQuadric(); // создаем новый объект
  glPushMatrix();
  glTranslated(X3,0,Y3-height/20.0);
  glRotated(angle,0,0,1);
  glTranslated(height,0,0);
  RotateOnM90Y();
  gluCylinder(quadObj, 0.15*height, height/15.0, height,_slices,1); 
  gluDeleteQuadric(quadObj );
  glPopMatrix();  
}
/*
void auxKran(double x0, double y0, double &x, double &y,double radius,int angle,double height,int &i,byte &opening)
{
  double X1,Y1,X2,Y2;
  int ang1;
  bool shtopor;
  switch (angle)
  {
  case 0 : 
    {
    shtopor=false;
    break;
    }
  case 180 : 
    {
    shtopor=true;
    break;
    }
  default : return;
  }
  if ((opening!=0)&&(opening!=1)&&(opening!=2)&&(opening!=3))return;
  //auxTrinity(x0,y0,x,y,radius,angle,height,X1,Y1,ang1,shtopor);
  
  GLUquadricObj *quadObj;
  quadObj = gluNewQuadric(); // создаем новый объект
  glPushMatrix();
  glTranslated(X1,Y1,0);
  glRotated(-90,1,0,0);
  gluQuadricDrawStyle(quadObj, GLU_FILL);
  gluDisk(quadObj,0, 1.2*radius,_slices,_stacks);  //1.2*radius - радиус крышки тройника
  gluCylinder(quadObj,radius/2.0,radius/2.0,height/3.0,_slices,_stacks);  //radius/2.0 - радиус сальника height/3.0-высота до него
  glTranslated(0,0,height/3.0);
  gluDisk(quadObj,0,radius/2.0,_slices,_stacks);  //radius/2.0- радиус сальника height/3.0-высота до него
  glPopMatrix();
  gluDeleteQuadric(quadObj);
  X2=X1;        //координаты конца 
  Y2=Y1+height/3.0; //сальника
  if (opening==0)
  {
    VentShturval(X2,Y2,radius,height,721*4.0/(i+1),(int)(60+i/2.0),8);//Открывается
    if (i<720)i+=20;
    else opening=1;
  }
  if (opening==1)VentShturval(X2,Y2,radius,height,4.0,60+360,8);//Открыто
  if (opening==2)
  {
    VentShturval(X2,Y2,radius,height,721*4.0/(i+1),(int)(60+i/2.0),8);//Закрывается
    if (i>0)i-=20;
    else opening=3;
  }
  if (opening==3)VentShturval(X2,Y2,radius,height,721*4.0,60,8);//Закрыто
  auxTurn(x,y,X1,Y1,radius,angle,true);
}
*/
void auxBattery(double x0, double y0,double radius,int kol,double height,double r1)
{
  if ((radius<=0)||(kol<=0))return;
  double altitude=15*radius;
  double x,y;
  x0=x0+radius/2.0;
  glPushMatrix();
  RotateTo0(x0,y0);
  auxCylinder(radius*2.0,height); //верхняя толстая труба
  glTranslated(-altitude,0,0);
  auxCylinder(radius*2.0,height); //нижняя толстая труба
  glPopMatrix();
  for (int i=0;i<kol;i++)
  {
    BatRebro(x0+radius*2.5+i*(2*radius+r1),y0-altitude,radius,3*radius,altitude-4*radius);    
  }
  BatGayka(x0+height,y0,0,radius,2.0);
  BatGayka(x0+height,y0-altitude,0,radius,2.0);
  BatGayka(x0,y0,180,radius,2.0);
  BatGayka(x0,y0-altitude,180,radius,2.0);
  
}
void BatRebro(double x0,double y0,double radius,double height,double altitude)
{
  if (radius<=0)return;
  glPushMatrix();
  glTranslated(x0,y0,2.0*radius);
  RotateOn180Z();   
  double R=height/(cosV+0.5*koeff);
  double step=R*koeff;
  double dZ=step-radius*koeff*cosV;
  double dX=radius*koeff*sinV;
  double radix=height-radius;
  auxCylinder(radius*faska,step);
  for (int t=0;t<10;t++)
  {
    auxCylinder(radius,step);
    if (t!=9)
    {
    glTranslated(0,dX,dZ);  
    RotateOnM10X();
    }
  }
  auxCylinder(radius*faska,step); 
  auxCylinder(radius,altitude);
  glTranslated(0,0,altitude);
  auxCylinder(radius*faska,step);
  for (int t=0;t<10;t++)
  {
    auxCylinder(radius,step);
    if (t!=9)
    {
    glTranslated(0,dX,dZ);  
    RotateOnM10X();
    }
  }
  auxCylinder(radius*faska,step); 
  glPopMatrix();
} 
void BatGayka(double x0,double y0,int angle,double radius,double alt)
{
  glPushMatrix();
  switch (angle)
  {
  case 0 : {RotateTo0(x0,y0);break;}
  case 180 : {RotateTo180(x0,y0);break;}
  default : return;
  }
  /*
  GLUquadricObj *quadObj;
  quadObj = gluNewQuadric(); // создаем новый объект
  */
  //gluQuadricDrawStyle(quadObj, GLU_FILL);
  //gluDisk(quadObj,radius, 2*radius,_slices,_stacks);  //
   glBegin(GL_POLYGON);
    glVertex3d(radius,-radius/1.732,radius/alt);// первая поверхность
    glVertex3d(radius,radius/1.732,radius/alt); 
    glVertex3d(0,2*radius/1.732,radius/alt);
    glVertex3d(-radius,radius/1.732,radius/alt);
    glVertex3d(-radius,-radius/1.732,radius/alt);
    glVertex3d(0,-2*radius/1.732,radius/alt);   
   glEnd();
   //GLdouble color[4];
   //GLdouble blackcolor[4];
   //glGetDoublev(GL_CURRENT_COLOR, color);
   //for (int i=0;i<4;i++)blackcolor[i]=color[i]-0.2;
   //glColor4dv(blackcolor);
   glBegin(GL_QUAD_STRIP);
    glVertex3d(radius,-radius/1.732,radius/alt);
    glVertex3d(radius,-radius/1.732,0);
    glVertex3d(radius,radius/1.732,radius/alt); 
    glVertex3d(radius,radius/1.732,0); 
    glVertex3d(0,2*radius/1.732,radius/alt);
    glVertex3d(0,2*radius/1.732,0);
    glVertex3d(-radius,radius/1.732,radius/alt);
    glVertex3d(-radius,radius/1.732,0);
    glVertex3d(-radius,-radius/1.732,radius/alt);
    glVertex3d(-radius,-radius/1.732,0);
    glVertex3d(0,-2*radius/1.732,radius/alt);
    glVertex3d(0,-2*radius/1.732,0);
    glVertex3d(radius,-radius/1.732,radius/alt);
    glVertex3d(radius,-radius/1.732,0);
   glEnd();
   //glColor4dv(color);
  glPopMatrix();
  //gluDeleteQuadric(quadObj);
}
void auxAirBattery(double x0, double y0,double radius,int quantity,double altitude)
{
  GLUquadricObj *quadObj;
  quadObj = gluNewQuadric(); // создаем новый объект для диска
  gluQuadricDrawStyle(quadObj, GLU_FILL);
  glPushMatrix();
  RotateTo270(x0,y0);
  auxCylinder(radius*faska,faskalen*radius);
  auxCylinder(radius,3*radius);
  x0-=2*radius;
  y0-=3*radius;
  glPopMatrix();
  int ang=0;
  double xt,yt,x,y;
  auxTube(x0,y0,xt,yt,radius,ang,altitude-2*radius);
  auxTurn(xt,yt,x,y,radius/4.0,ang,true,radius);
  auxTube(x,y,xt,yt,radius/4.0,ang,altitude-2*radius);
  auxTurn(xt,yt,x,y,radius/4.0,ang,true,radius);
  auxTube(x,y,xt,yt,radius,ang,altitude-2*radius);
  auxTurn(xt,yt,x,y,radius/4.0,ang,true,radius);
  auxTube(x,y,xt,yt,radius/4.0,ang,altitude-2*radius);
  auxTurn(xt,yt,x,y,radius/4.0,ang,true,radius);
  double step=(altitude-2*radius)/(2*quantity-1);
    /*
    for(int kol=0;kol<2*quantity;kol+=2)
    {
      glBegin(GL_QUAD_STRIP);
      glNormal3d(-1,0,0);
      glVertex3d(x0+step*kol,y0-altitude,0);
      glVertex3d(x0+step*kol,y0,0);
      glNormal3d(0,0,1);
      glVertex3d(x0+step*kol+step/2.0,y0-altitude,step);
      glVertex3d(x0+step*kol+step/2.0,y0,step);     
      glNormal3d(1,0,0);
      glVertex3d(x0+step*kol+step,y0-altitude,0);
      glVertex3d(x0+step*kol+step,y0,0);      
      glEnd();
    }
  */
    glBegin(GL_QUADS);
    for(int kol=0;kol<2*quantity;kol+=2)
    {
      glVertex3d(x0+step*kol,y0-altitude,0);
      glVertex3d(x0+step*kol,y0,0);
      glVertex3d(x0+step*(kol+1),y0,0);     
      glVertex3d(x0+step*(kol+1),y0-altitude,0);
    }
   glEnd();
  glPushMatrix();
  //уже учтено, что x0 и y0 сдвинуты
  glTranslated(x0-radius+altitude/2.0,y0-altitude/2.0,2*radius);
  gluDisk(quadObj,0,radius,_slices+10,_stacks);
  glTranslated(-3*radius+altitude/2.0,-3*radius-altitude/2.0,-2*radius);
  RotateTo90(0,0);
  auxCylinder(radius*faska,faskalen*radius);
  auxCylinder(radius,3*radius);
  ////
  glPopMatrix();
  gluDeleteQuadric(quadObj);
}
void AirTubes(int quantity,double radius,double altitude,GLUquadricObj *quadObj)
{
  for(int kol=1;kol<quantity;kol++)
  {
  if (kol!=0)glTranslated(0,-2*radius,0);
  gluCylinder(quadObj,radius/4.0,radius/4.0,altitude,_slices,_stacks);
  }
}
void auxSoplo(TCOLOR ColdWater,TCOLOR HotWater,double x0, double y0,double radius)
{
 if (radius<=0)return;
 GLdouble params[4];
 glGetDoublev(GL_CURRENT_COLOR,params); 
 GLUquadricObj *quadObj;
 quadObj = gluNewQuadric(); // создаем новый объект
 gluQuadricDrawStyle(quadObj, GLU_FILL);
 double height=15*radius;
 double xt,yt;
 double otevt=5*radius;
 double fas=faskalen*radius;
 double ras=faska*radius;
 auxHalfTube(x0,y0,xt,yt,radius,0,height);
 auxHalfTube(x0+otevt,y0-otevt,xt,yt,radius,90,otevt);
 glPushMatrix();
 glTranslated(x0,y0,0);
 glRotated(-90,0,1,0);
 glTranslated(0,0,-height);
 gluCylinder(quadObj,ras,ras,fas,_slices,_stacks);
 glTranslated(0,0,height-fas);
 gluCylinder(quadObj,ras,ras,fas,_slices,_stacks);
 glTranslated(0,-otevt,fas-otevt);
 glRotated(-90,1,0,0);
 gluCylinder(quadObj,ras,ras,fas,_slices,_stacks);
 glPopMatrix();
 double r=0.8*radius;
 double r1=0.3*r;
 GLfloat amb[4],ambient[4] = {0.0,0.0, 0.0, 1};
 glGetFloatv(GL_LIGHT_MODEL_AMBIENT,amb);     
 glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
 SetColorUb(ColdWater);
 glNormal3d(0,0,1);
 glBegin(GL_POLYGON);
 glVertex3d(x0+otevt-r,y0-otevt,0.1);
 glVertex3d(x0+otevt-r,y0-otevt+4*radius,0.1);
 glVertex3d(x0+otevt-r1,y0-otevt+5*radius-r1,0.1);
 glVertex3d(x0+otevt+r1,y0-otevt+5*radius-r1,0.1);
 glVertex3d(x0+otevt+r,y0-otevt+4*radius,0.1);
 glVertex3d(x0+otevt+r,y0-otevt,0.1);
 glEnd();
 SetColorUb(HotWater);
 glBegin(GL_POLYGON);
 glVertex3d(x0,y0+r,0.1);
 glVertex3d(x0+3*radius,y0+r,0.1);
 glVertex3d(x0+4.5*radius,y0+r1,0.1);
 glVertex3d(x0+4.5*radius,y0-r1,0.1);
 glVertex3d(x0+3*radius,y0-r,0.1);
 glVertex3d(x0,y0-r,0.1);
 glEnd();
 glBegin(GL_QUADS);
 glVertex3d(x0+4*radius,y0+r1,0.1);
 glVertex3d(x0+5*radius,y0+r1,0.1);
 glVertex3d(x0+5*radius,y0-r1,0.1);
 glVertex3d(x0+4*radius,y0-r1,0.1);
 //сопло расширяется
 glVertex3d(x0+5*radius,y0+r1,0.1);
 glVertex3d(x0+15*radius,y0+0.9*radius,0.1);
 glVertex3d(x0+15*radius,y0-0.9*radius,0.1);
 glVertex3d(x0+5*radius,y0-r1,0.1);
 glEnd();
 glColor3d(0,0,0);
 GLfloat w;
 glGetFloatv(GL_LINE_WIDTH,&w);
 glLineWidth((float)radius);
 glBegin(GL_LINES);
 glVertex3d(x0+3*radius,y0+r,0.2);
 glVertex3d(x0+4.5*radius,y0+r1,0.2);
 glVertex3d(x0+4.5*radius,y0-r1,0.2);
 glVertex3d(x0+3*radius,y0-r,0.2);
 glEnd();
 glLineWidth(w);
 glPopMatrix();
 glColor4dv(params);
 glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
 gluDeleteQuadric(quadObj); 
}
/*
void auxSoplo(double x0, double y0,double radius,double height)
{
 if (radius<=0)return;
 GLUquadricObj *quadObj;
 quadObj = gluNewQuadric(); // создаем новый объект
 gluQuadricDrawStyle(quadObj, GLU_FILL);
 glPushMatrix();
 glTranslated(x0,y0,0);
 glRotated(90,0,1,0); 
 gluCylinder(quadObj,radius*faska,radius*faska,0.05*height,_slices,_stacks);
 gluCylinder(quadObj,radius*0.8,radius*0.8,0.4*height,_slices,_stacks);
 glTranslated(0,0,0.4*height);
 gluCylinder(quadObj,radius*0.8,radius,0.6*height,_slices+10,_stacks);
 glTranslated(0,0,0.6*height);
 gluCylinder(quadObj,radius*faska,radius*faska,0.05*height,_slices,_stacks);
 glTranslated(0,0,-0.1*height);
 glRotated(90,1,0,0);
 glTranslated(0,-0.7*height,0);
 gluCylinder(quadObj,radius,radius,0.3*height,_slices,_stacks);
 glTranslated(0,0,0.3*height);
 gluCylinder(quadObj,radius*faska,radius*faska,0.05*height,_slices,_stacks);
 glPopMatrix();
 gluDeleteQuadric(quadObj); 
 
//double x=x0+height;
//double y=y0;
//double y1=y0-0.35*height;
//double x1=x0+0.3*height;
}
 */
void auxWaterBattery(double x0, double y0,double radius,double altitude,double radius1)
{
 if (radius<=0)return;
 GLUquadricObj *quadObj;
 quadObj = gluNewQuadric(); // создаем новый объект
 gluQuadricDrawStyle(quadObj, GLU_FILL);
 double dlina=altitude-5.5*radius;
 double dl_forsunki=2.5*radius;
 glPushMatrix();
 RotateTo270(x0,y0);
 auxCylinder(radius*faska,faskalen*radius);
 auxCylinder(radius,3*radius);
 RotateOn90Y();
 glTranslated(-3*radius,0,-dlina+1.5*radius);
 for(int t=0;t<2;t++)
 {
   if(t!=0) glTranslated(-4*radius,0,0);
   auxCylinder(radius,dlina);
   auxCylinder(radius*faska,faskalen*radius);
   glTranslated(0,0,dlina);
   auxCylinder(radius*faska,faskalen*radius);
   //форсунка============
   auxCone(radius,radius1,dl_forsunki);
   //gluCylinder(quadObj,radius,radius1,dl_forsunki,_slices,_stacks);
   glTranslated(0,0,dl_forsunki-faskalen*radius);
   auxCylinder(radius1*faska,faskalen*radius);
   //форсунка============
   glTranslated(0,0,-dlina-dl_forsunki+faskalen*radius);
 }
 glTranslated(4*radius,0,0.2*dlina);
 RotateOnM90Y();
 auxCylinder(radius,4*radius);
 glTranslated(0,0,(2-faskalen*0.5)*radius);
 auxCylinder(radius*faska,faskalen*radius);
 glTranslated(0.8*dlina-1.5*radius,0,(2+faskalen*0.5)*radius);
 auxCylinder(radius,3*radius);
 glTranslated(0,0,3*radius-faskalen*radius); 
 auxCylinder(radius*faska,faskalen*radius);
 glPopMatrix();
 
 glPushMatrix();
 glTranslated(x0-dlina+1.5*radius,y0-3*radius,0);
 auxWaterTurn(radius);
 glPopMatrix();
}
void auxWaterTurn(double radius)
{
  double R=3*radius;
  double step=R*koeff;
  double dZ=step-radius*koeff*cosV;
  double dX=radius*koeff*sinV;
  RotateTo180(0,0);
  for (int t=0;t<19;t++)
  {
    auxCylinder(radius,step);
    if (t!=18)
    {
    glTranslated(dX,0,dZ);  
    RotateOn10Y();    
    }
  }
}
void auxDrossel(double x0, double y0, double radius,int ang,double height,double part)
{
if (radius<=0)return;
GLUquadricObj *quadObj;
quadObj = gluNewQuadric(); // создаем новый объект
glPushMatrix();
 glTranslated(x0,y0,0);
 gluQuadricDrawStyle(quadObj, GLU_FILL);
 glRotated(-90,1,0,0);
 glRotated(ang,0,1,0);
 gluCylinder(quadObj, radius, radius*part, height,_slices,_stacks);
 gluCylinder(quadObj, radius*faska, radius*faska, radius*faskalen,_slices,_stacks);
 glTranslated(0,0,0.9*height);
 gluCylinder(quadObj, radius*faska, radius*faska, radius*faskalen,_slices,_stacks);
 gluDisk(quadObj, 0, radius*faska,_slices,_stacks);
glPopMatrix();
gluDeleteQuadric(quadObj);
}
void auxNasos(TCOLOR ColdWater,TCOLOR HotWater,double x0, double y0,double radius,int angle,double height, unsigned char anim)
{
if (radius<=0)return;
//double height=5*radius;
switch (angle)
  {
  case 0 : 
    {
    break;
    }
  case 180 : 
    {
    x0=x0-2*height;
    y0=y0+4*radius;
    break;
    }
  default : return;
  }
#define plus 7
GLUquadricObj *quadObj;
quadObj = gluNewQuadric(); // создаем новый объект
gluQuadricDrawStyle(quadObj, GLU_FILL);
 GLdouble params[4];
 glGetDoublev(GL_CURRENT_COLOR,params); 
 double xt,yt;
 double x=x0-height-2*radius;
 double y=y0+2*radius+height;
 //glColor3d(1,1,1);
 auxHalfTube(x0,y0,xt,yt,radius,180,height);
 //auxHalfTube(x0,y0,xt,yt,radius*faska,180,0.1*height);
 auxHalfTube(x,y,xt,yt,radius,270,height);
//auxHalfTube(x0-height-2*radius,y0+2*radius+height,xt,yt,radius*faska,270,0.1*height);
 glPushMatrix();
 glTranslated(x0,y0,0);
 glRotated(-90,0,1,0);
 gluCylinder(quadObj, radius*faska, radius*faska,0.1*height,_slices,_stacks);
 glTranslated(0,2*radius,height+2*radius);
 glRotated(-90,1,0,0); 
 glTranslated(0,0,0.9*height);
 gluCylinder(quadObj, radius*faska, radius*faska,0.1*height,_slices,_stacks);
 glTranslated(-0.25*height,2*radius,-0.9*height);
 glRotated(90,0,1,0);
 gluCylinder(quadObj, 3*radius, 3*radius,0.25*height,_slices+2*plus,_stacks);   //
 glTranslated(0,0,0.25*height);
 gluDisk(quadObj, 0,3*radius,_slices+2*plus,_stacks);
 glTranslated(0,0,0.5);
 gluDisk(quadObj, 0,0.5*radius,_slices+2*plus,_stacks);
 GLfloat amb[4],ambient[4] = {0.0,0.0, 0.0, 1};
 glGetFloatv(GL_LIGHT_MODEL_AMBIENT,amb);     
 glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
 if (anim==1)  SetColorUb(ColdWater);
 else  SetColorUb(HotWater);
 glTranslated(0,0,-0.35);
 gluDisk(quadObj, 0,2.5*radius,_slices+2*plus,_stacks);
 glPopMatrix();

 double r=radius*0.5;
 glNormal3d(0,0,1);
 glBegin(GL_QUADS);
 glVertex3d(x-r,y,0.1);
 glVertex3d(x+r,y,0.1);
 glVertex3d(x+r,y-height,0.1);
 glVertex3d(x-r,y-height,0.1);
 
 glVertex3d(x0,y0-r,0.1);
 glVertex3d(x0,y0+r,0.1);
 glVertex3d(x0-height,y0+r,0.1);
 glVertex3d(x0-height,y0-r,0.1);
 glEnd();
 glColor4dv(params);
 glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
gluDeleteQuadric(quadObj);
}
void auxTermometer(double x, double y,double rad,double width,double radius1)
{ 
  glNormal3d(0,0,1);
  GLdouble params[4];
  glGetDoublev(GL_CURRENT_COLOR,params); 
  glColor3f(0.0f,0.0f,0.0f);
  glBegin(GL_QUAD_STRIP);
  glVertex3d(x-0.8*rad,y+width,0.0f);
  glVertex3d(x+0.8*rad,y+width,0.0f);
  glVertex3d(x-0.95*rad,y+0.95*width,0.0f);
  glVertex3d(x+0.95*rad,y+0.95*width,0.0f);
  //
  glVertex3d(x-rad,y+0.8*width,0.0f);
  glVertex3d(x+rad,y+0.8*width,0.0f);
  glVertex3d(x-rad,y-0.5*width,0.0f);
  glVertex3d(x+rad,y-0.5*width,0.0f); 
  //
  glVertex3d(x-2*radius1,y-0.7*width,0.0f);
  glVertex3d(x+2*radius1,y-0.7*width,0.0f); 
  glVertex3d(x-radius1,y-0.9*width,0.0f);
  glVertex3d(x+radius1,y-0.9*width,0.0f); 
  glVertex3d(x-radius1,y-width,0.0f);
  glVertex3d(x+radius1,y-width,0.0f);
  glEnd();
  glBegin(GL_LINES);
  glVertex3d(x,y+0.7*width,0.2f);
  glVertex3d(x,y-0.4*width,0.2f); 
  for (int i=-1;i<4;i++)
  {
    glVertex3d(x-0.4*rad,y+0.2*i*width,0.2f);
    glVertex3d(x+0.4*rad,y+0.2*i*width,0.2f);
  }
  glEnd();
  glColor4dv(params);
  glBegin(GL_QUADS);
  glVertex3d(x-0.7*rad,y+0.8*width,0.1f);
  glVertex3d(x+0.7*rad,y+0.8*width,0.1f);
  glVertex3d(x+0.7*rad,y-0.5*width,0.1f); 
  glVertex3d(x-0.7*rad,y-0.5*width,0.1f);
  glEnd();
}
void DrawBound(const RECTAN *bound)
{
  glPushMatrix();
  glNormal3d(0,0,1);
  glBegin(GL_LINE_STRIP);
    glVertex3d(bound->left-visal_zazor,bound->top+visal_zazor,visal_zazor);
    glVertex3d(bound->left+bound->width+visal_zazor,bound->top+visal_zazor,visal_zazor);
    glVertex3d(bound->left+bound->width+visal_zazor,bound->top-bound->height-visal_zazor,visal_zazor);
    glVertex3d(bound->left-visal_zazor,bound->top-bound->height-visal_zazor,visal_zazor);
    glVertex3d(bound->left-visal_zazor,bound->top+visal_zazor,visal_zazor);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3d(bound->left-2*visal_zazor,bound->top+2*visal_zazor,visal_zazor);
    glVertex3d(bound->left,bound->top+2*visal_zazor,visal_zazor);
    glVertex3d(bound->left,bound->top,visal_zazor);
    glVertex3d(bound->left-2*visal_zazor,bound->top,visal_zazor);
    glVertex3d(bound->left-2*visal_zazor,bound->top+2*visal_zazor,visal_zazor);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3d(bound->left+bound->width+2*visal_zazor,bound->top+2*visal_zazor,visal_zazor);
    glVertex3d(bound->left+bound->width,bound->top+2*visal_zazor,visal_zazor);
    glVertex3d(bound->left+bound->width,bound->top,visal_zazor);
    glVertex3d(bound->left+bound->width+2*visal_zazor,bound->top,visal_zazor);
    glVertex3d(bound->left+bound->width+2*visal_zazor,bound->top+2*visal_zazor,visal_zazor);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3d(bound->left+bound->width+2*visal_zazor,bound->top-bound->height-2*visal_zazor,visal_zazor);
    glVertex3d(bound->left+bound->width,bound->top-bound->height-2*visal_zazor,visal_zazor);
    glVertex3d(bound->left+bound->width,bound->top-bound->height,visal_zazor);
    glVertex3d(bound->left+bound->width+2*visal_zazor,bound->top-bound->height,visal_zazor);
    glVertex3d(bound->left+bound->width+2*visal_zazor,bound->top-bound->height-2*visal_zazor,visal_zazor);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3d(bound->left-2*visal_zazor,bound->top-bound->height-2*visal_zazor,visal_zazor);
    glVertex3d(bound->left,bound->top-bound->height-2*visal_zazor,visal_zazor);
    glVertex3d(bound->left,bound->top-bound->height,visal_zazor);
    glVertex3d(bound->left-2*visal_zazor,bound->top-bound->height,visal_zazor);
    glVertex3d(bound->left-2*visal_zazor,bound->top-bound->height-2*visal_zazor,visal_zazor);
  glEnd();
  glPopMatrix();
}
void DrawPoint(const TDist x0,const TDist y0,double z)
{
  glBegin(GL_QUADS);
  glVertex3d(x0-visal_zazor,y0-visal_zazor,z);    
  glVertex3d(x0-visal_zazor,y0+visal_zazor,z);
  glVertex3d(x0+visal_zazor,y0+visal_zazor,z);
  glVertex3d(x0+visal_zazor,y0-visal_zazor,z);    
  glEnd();
}
void DrawPointBound(const TDist x0,const TDist y0,const TDist xt,const TDist yt,const TDist x,const TDist y,const TDist x1,const TDist y1,double z)
{
  glPushMatrix();
  glNormal3d(0,0,1);
    glBegin(GL_LINE_STRIP);
    glVertex3d(x0,y0,z);
    glVertex3d(xt,yt,z);
    glVertex3d(x,y,z);
    glVertex3d(x1,y1,z);
    glEnd();
    DrawPoint(x0,y0,z);
    DrawPoint(xt,yt,z);
    DrawPoint(x,y,z);
    DrawPoint(x1,y1,z);
  glPopMatrix();
}
void DrawNode(const CShemaNode node)
{
  TDist z=2*node.Radius;
  glPushMatrix();
  glNormal3d(0,0,1);
  switch(node.Angle)
  {
  case 0:
    {
    glVertex3d(node.X,node.Y+node.Radius,z);
    glVertex3d(node.X+z,node.Y,z);
    glVertex3d(node.X,node.Y-node.Radius,z);
    break;
    }
  case 90:
    {
    glVertex3d(node.X,node.Y+z,z);
    glVertex3d(node.X+node.Radius,node.Y,z);
    glVertex3d(node.X-node.Radius,node.Y,z);
    break;
    }
  case 180:
    {
    glVertex3d(node.X,node.Y+node.Radius,z);
    glVertex3d(node.X-z,node.Y,z);
    glVertex3d(node.X,node.Y-node.Radius,z);
    break;
    }
  case 270:
    {
    glVertex3d(node.X,node.Y-z,z);
    glVertex3d(node.X+node.Radius,node.Y,z);
    glVertex3d(node.X-node.Radius,node.Y,z);
    break;
    }
  }
  glPopMatrix();
}
void auxConnector(double x0, double y0,double radius,double height,int angle)
{
  glPushMatrix();
  switch (angle)
  {
  case 0:
    {
      RotateTo0(x0,y0);
      break;
    }
  case 90:
    {
      RotateTo90(x0,y0);
      break;
    }
  case 180:
    {
      RotateTo180(x0,y0);
      break;
    }
  case 270:
    {
      RotateTo270(x0,y0);
      break;
    }
  }
  auxCylinder(radius,height);
  auxCylinder(2*radius,height*0.2);
  glTranslated(0,0,height-radius*faskalen);
  auxCylinder(radius*faska,faskalen*radius);
  glPopMatrix();  
}

void auxDrawDistrict(const double x0,const double y0,const double xt,const double yt,const double x,const double y,const double x1,const double y1,double rad)
{
  glPushMatrix();
  glNormal3d(0,0,1);
  glBegin(GL_LINE_STRIP);
  glVertex3d(x0,y0,rad);
  glVertex3d(xt,yt,rad);
  glVertex3d(x,y,rad);
  glVertex3d(x1,y1,rad);
  glEnd();
  glPopMatrix();  
}

//static GLYPHMETRICSFLOAT gmf[256];  // Массив с информацией о нашем шрифте
//GLuint base;              // База отображаемого списка для набора символов (добавлено)
GLvoid BuildFont(HDC hDC,FONT_DATA &font_data,GLuint &base,GLYPHMETRICSFLOAT * gmf,LOGFONT * lf,COLORREF * color)           // Строим растровый шрифт
{

  HFONT  font;                     // Идентификатор шрифта Windows
  base = glGenLists(256);          // массив для 256 букв
  //LOGFONT lof;
  if (lf==NULL)
  {
  //memset(&lof, 0, sizeof(LOGFONT)); 
  font_data.lf.lfHeight =-12;// -MulDiv(12, pDC->GetDeviceCaps(LOGPIXELSY), 72);// высота шрифта
  font_data.lf.lfWidth=0;// ширина знакоместа
  font_data.lf.lfEscapement=0;//Угол перехода
  font_data.lf.lfOrientation=0;//Угол направления
  font_data.lf.lfWeight=FW_NORMAL;//Ширина шрифта
  font_data.lf.lfItalic=FALSE;// Курсив 
  font_data.lf.lfUnderline=FALSE;        // Подчеркивание
  font_data.lf.lfStrikeOut=FALSE;        // Перечеркивание
  font_data.lf.lfCharSet=RUSSIAN_CHARSET;//Идентификатор кодировки
  font_data.lf.lfOutPrecision=OUT_TT_PRECIS;// Точность вывода
  font_data.lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;//Точность отсечения
  font_data.lf.lfQuality=ANTIALIASED_QUALITY;// Качество вывода
  font_data.lf.lfPitchAndFamily=FF_DONTCARE|DEFAULT_PITCH; // Семейство и Шаг
  strcpy(font_data.lf.lfFaceName, "Arial");   
  }
  else font_data.lf= (*lf);
//  font_data.lf.lfHeight *= ScaleFactor;

  
  font=CreateFontIndirect(&font_data.lf);
  SelectObject(hDC, font);       //Выбрать шрифт, созданный нами 
  wglUseFontOutlines( hDC,         // Выбрать текущий контекст устройства (DC)
       0,            // Стартовый символ
       256,          // Количество создаваемых списков отображения
       base,         // Стартовое значение списка отображения
//       1000,         // Стартовое значение списка отображения
     0.0f,       //Отклонение от настоящего контура
//     1.0f,       //Отклонение от настоящего контура
//     0.2f,       //Толщина шрифта по оси Z
     0.0f,       //Толщина шрифта по оси Z
     WGL_FONT_POLYGONS,       //Использовать полигоны, а не линии
     gmf);       //буфер адреса для данных списка отображения
  font_data.scale.X=font_data.scale.Y=ScaleFactor*(-font_data.lf.lfHeight);  
  font_data.scale.X=font_data.scale.Y=0.2*(-font_data.lf.lfHeight);  
//  font_data.scale.X=font_data.scale.Y=1;  
  font_data.scale.Z=0;
  if (color!=NULL)
  {
    font_data.color.R=GetRValue(*color); 
    font_data.color.G=GetGValue(*color); 
    font_data.color.B=GetBValue(*color); 
  }
  else 
  {
    font_data.color.R=0;
    font_data.color.G=0;
    font_data.color.B=0;
  }


}
GLvoid KillFont(GLuint &base)                   // Удаление шрифта
{
  glDeleteLists(base, 256); // Удаление всех 256 списков отображения
}
GLvoid glPrint(GLYPHMETRICSFLOAT * gmf,FONT_DATA &font_data,GLuint &base,TCOLOR * cl_text,TCOLOR * cl_back,bool show_back,long x,long y,double z,const char *fmt, ...)     // Функция вывода текста в OpenGL
{
//  font_data.scale.X = 4.;
//  font_data.scale.Y = 4.;

  glNormal3d(0,0,1);
  GLdouble params[4];
  glGetDoublev(GL_CURRENT_COLOR,params); 
  glPushMatrix();
  float length=0, height=0;     // Переменная для нахождения
            // физической длины текста
  char text[256];    // Здесь наша строка
  va_list              ap;    // Указатель на переменный список аргументов
  if (fmt == NULL)            // Если нет текста,
    return;               // ничего не делаем
  va_start(ap, fmt);         // Анализ строки на переменные
  vsprintf(text, fmt, ap);   // И конвертация символов в реальные коды
  va_end(ap);                // Результат сохраняется в text
  for (unsigned int loop=0;loop<strlen(text);loop++)//Цикл поиска размера строки
  {
    unsigned char ch;
    ch=text[loop];
    length+=gmf[ch].gmfCellIncX;// Увеличение размера на ширину символа
    if (gmf[ch].gmfptGlyphOrigin.y>height)height=gmf[ch].gmfptGlyphOrigin.y;//максимальная высота           
    ch=text[loop];

  }
  double space=font_data.scale.X*gmf[' '].gmfCellIncX;
  long realL=round_to_dig(font_data.scale.X*(length)+2*space);
  long realH=round_to_dig(font_data.scale.Y*2*height);
  

  TCOLOR front,back;
  if (cl_text==NULL)front=font_data.color;
  else front=(*cl_text);
  if (cl_back==NULL)
  {
    back.R=255-front.R;
    back.G=255-front.G;
    back.B=255-front.B;
  }
  else back=(*cl_back);
  SetColorUb(back);
  double antialiasing=realH/4.0;
  double smooth2=antialiasing*0.5;
  //realL-=antialiasing;
  //realH-=smooth2; 
  double smooth3=antialiasing*0.2;

  if (show_back)
  {
    glBegin(GL_TRIANGLE_FAN);
  
    glVertex3d(x+antialiasing,y-realH,z-0.1f);
    glVertex3d(x+smooth2,y-realH+smooth3,z-0.1f);
    glVertex3d(x+smooth3,y-realH+smooth2,z-0.1f);
    glVertex3d(x,y-realH+antialiasing,z-0.1f);
    
    glVertex3d(x,y-antialiasing,z-0.1f);
    glVertex3d(x+smooth3,y-smooth2,z-0.1f);
    glVertex3d(x+smooth2,y-smooth3,z-0.1f);
    glVertex3d(x+antialiasing,y,z-0.1f);

    glVertex3d(x+realL-antialiasing,y,z-0.1f);
    glVertex3d(x+realL-smooth2,y-smooth3,z-0.1f);
    glVertex3d(x+realL-smooth3,y-smooth2,z-0.1f);
    glVertex3d(x+realL,y-antialiasing,z-0.1f);

    glVertex3d(x+realL,y-realH+antialiasing,z-0.1f);
    glVertex3d(x+realL-smooth3,y-realH+smooth2,z-0.1f);
    glVertex3d(x+realL-smooth2,y-realH+smooth3,z-0.1f);
    glVertex3d(x+realL-antialiasing,y-realH,z-0.1f);

    glEnd();
  }

  GLdouble matrix[16];
  glGetDoublev(GL_PROJECTION_MATRIX, matrix);

  glTranslatef(x+space,y-antialiasing-realH/2.0,z);       //Центровка на экране нашей строки
  glScalef(font_data.scale.X,font_data.scale.Y,font_data.scale.Z);

  
  glPushAttrib(GL_LIST_BIT); // Сохраняет в стеке значения битов списка отображения
  glListBase(base);          // Устанавливает базовый символ в 0
  // Создает списки отображения текста
  SetColorUb(front);
  glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
  glPopAttrib(); // Восстанавливает значение Display List Bits
  glPopMatrix();
  glColor3dv(params);
}
GLvoid glPrintFormatted(GLYPHMETRICSFLOAT * gmf,FONT_DATA &font_data,GLuint &base,TCOLOR * cl_text,TCOLOR * cl_back,const double x,const double y,const double z,const long x1,const long y1,const char *fmt, ...)     // Функция вывода форматированного  текста в OpenGL
{
  double Xc,Yc;
  RECTAN r=GetPrintRect(gmf,font_data,x1,y1,fmt);
  Xc=x1+r.width/2.0;
  Yc=y1-r.height/2.0;
  glNormal3d(0,0,1);
  TCOLOR front,back;
  if (cl_text==NULL)front=font_data.color;
  else front=(*cl_text);
  if (cl_back==NULL)
  {
    back.R=255-front.R;
    back.G=255-front.G;
    back.B=255-front.B;
  }
  else back=(*cl_back);
  GLdouble params[4];
  glGetDoublev(GL_CURRENT_COLOR,params);  
  SetColorUb(back);
  glBegin(GL_TRIANGLES);
  glVertex3d(x,y,z-0.1f);
  glVertex3d(Xc-0.5,Yc,z-0.1f);
  glVertex3d(Xc+0.5,Yc,z-0.1f);
  glVertex3d(x,y,z-0.1f);
  glVertex3d(Xc,Yc-0.5,z-0.1f);
  glVertex3d(Xc,Yc+0.5,z-0.1f); 
  glEnd();
  glPrint(gmf,font_data,base,cl_text,cl_back,true,x1,y1,z,fmt);
  glColor4dv(params);
}
RECTAN GetFontRect(GLYPHMETRICSFLOAT * gmf,FONT_DATA &font_data,double lenth,int angle, double &x,double &y,const char *fmt, ... )     // Функция вывода текста в OpenGL
{
  RECTAN rect=GetPrintRect(gmf,font_data,x,y,fmt);     // Функция вывода текста в OpenGL
  double yreal=rect.height;
  double xreal=rect.width;
  double dx,dy; 
  switch (angle)
  {
  case 0:
    {
      dx=lenth/8.0;
      dy=lenth+yreal;
      break;
    }
  case 90:
    {
      dx=lenth/8.0;
      dy=lenth+yreal;
      break;
    }
  case 180:
    {
      dx=-lenth/8.0-xreal;
      dy=lenth+yreal;
      break;
    }
  case 270:
    {
      dx=lenth/8.0;
      dy=-lenth;
      break;
    }
  default :
    {
      return rect;
    }
  }
  x+=dx;
  y+=dy;
  x=(long)x;
  y=(long)y;
  rect.left=x;
  rect.top=y;
  return rect;
}

RECTAN GetPrintRect(GLYPHMETRICSFLOAT * gmf,FONT_DATA &font_data,const long x,const long y,const char *fmt, ... )     // Функция вывода текста в OpenGL
{
  RECTAN rect={0,0,0,0};
  double length=0, height=0;     // Переменная для нахождения
            // физической длины текста
  char text[256];    // Здесь наша строка
  va_list              ap;    // Указатель на переменный список аргументов
  if (fmt == NULL)            // Если нет текста,
    return rect;               // ничего не делаем
  va_start(ap, fmt);         // Анализ строки на переменные
  vsprintf(text, fmt, ap);   // И конвертация символов в реальные коды
  va_end(ap);                // Результат сохраняется в text
  for (unsigned int loop=0;loop<strlen(text);loop++)//Цикл поиска размера строки
  {
    unsigned char ch;
    ch=text[loop];
    length+=gmf[ch].gmfCellIncX;// Увеличение размера на ширину символа
    if (gmf[ch].gmfptGlyphOrigin.y>height)height=gmf[ch].gmfptGlyphOrigin.y;//максимальная высота   
  }
  double space=font_data.scale.X*gmf[' '].gmfCellIncX;
  long realL=round_to_dig(font_data.scale.X*(length)+2*space);
  long realH=round_to_dig(font_data.scale.Y*2*height);
  rect.left=x;
  rect.top=y;
  rect.width=realL;
  rect.height=realH;
  return rect;
}
void Get2Points(VECTORXY &first,VECTORXY &last,const VECTORXY lenth)
{
  VECTORXY Temp_first;
  VECTORXY Temp_last;
  if (first.Y==last.Y)//0 или 180 градусов
  {
    if (first.X==last.X) return;
    if (first.X<last.X)//0 градусов
    {
      Temp_first.Y=last.Y-lenth.Y;
      Temp_last.Y=last.Y+lenth.Y;
      Temp_first.X=Temp_last.X=last.X-lenth.X;
      goto exxx;
    }
    else //180 градусов
    {
      Temp_first.Y=first.Y-lenth.Y;
      Temp_last.Y=first.Y+lenth.Y;
      Temp_first.X=Temp_last.X=last.X+lenth.X;
      goto exxx;
    }
  }
  if (first.X==last.X)//90 или 270 градусов
  {
    if (first.Y<last.Y)//90 градусов
    {
      Temp_first.X=last.X-lenth.Y;
      Temp_last.X=last.X+lenth.Y;
      Temp_first.Y=Temp_last.Y=last.Y-lenth.X;
      goto exxx;
    }
    else //270 градусов
    {
      Temp_first.X=first.X-lenth.Y;
      Temp_last.X=first.X+lenth.Y;
      Temp_first.Y=Temp_last.Y=last.Y+lenth.X;
      goto exxx;
    }
  }
  //общее положение 
  double dlina;
  dlina=sqrt((last.X-first.X)*(last.X-first.X)+(last.Y-first.Y)*(last.Y-first.Y));//длина вектора
  VECTORXY zero;//базовая точка----------Б->
  zero.X=first.X+(1-lenth.X/dlina)*(last.X-first.X);
  zero.Y=first.Y+(1-lenth.X/dlina)*(last.Y-first.Y);
  double k;
  k=(last.Y-first.Y)/((last.X-first.X)*1.0);//угол наклона вектора
  double k2;
  k2=-1/k;
  //первая точка
  Temp_first.Y=zero.Y-lenth.Y/sqrt(k2*k2+1);
  Temp_first.X=zero.X-k2*(zero.Y-Temp_first.Y);
  //вторая точка
  Temp_last.Y=zero.Y+lenth.Y/sqrt(k2*k2+1);
  Temp_last.X=zero.X-k2*(zero.Y-Temp_last.Y);
exxx:
  first=Temp_first;
  last=Temp_last;
  return;
}
//----------------------------------------------------------------------------
/*
long round_to_dig(const double val)
{
  long lVal=(val);
  if ( ((lVal-val)>=0.5)&&(val<0) )lVal--;
  if ( ((val-lVal)>=0.5)&&(val>=0) )lVal++;
  return lVal;
}
*/
//----------------------------------------------------------------------------
