#include "stdafx.h"
#include "Tube.h"
#include "sheme.h"
#include "math.h"
#define pi 3.1415926535897932384626433832795
CTube::CTube(ELEMENT_DATA *tube_create,BOOL FromBounds)
{
  if (tube_create==NULL)return;
  Focused=0;
  ballT=NULL;
  dev=NULL;
  termo=NULL;
  flmeter=NULL;
  heatcntr=NULL;
  InitConnect();
  if (FromBounds)UpdateFromBounds(tube_create);
  else Update(tube_create);
}
CTube::CTube(const CTube &prev_tube) //конструктор копий
{
  Focused=0;
  ballT=NULL;
  dev=NULL;
  termo=NULL;
  flmeter=NULL;
  heatcntr=NULL;  
  CopyConnect(&prev_tube);
  ELEMENT_DATA t;
  prev_tube.GetData(&t);
  Update(&t);
}
CTube::~CTube()
{ 
  if (ballT!=NULL)delete ballT;
  if (termo!=NULL)delete termo;
  if (dev!=NULL)delete dev; 
  if (flmeter!=NULL)delete flmeter; 
  if (heatcntr!=NULL)delete heatcntr; 
  if (connected!=NULL)delete connected;
} 
void CTube::Update(ELEMENT_DATA *tube_update)
{
  if ((tube_update->Radius<=0)||(tube_update->Height<=0))return;
  x0=tube_update->X0;
  y0=tube_update->Y0;
  angle=tube_update->Angle;
  height=tube_update->Height;
  radius=tube_update->Radius;
  pribor=tube_update->Pribor;
  right=tube_update->Right;
  strcpy(text,tube_update->Text);
  if (pribor!=0) tube_update->animate=0;
  anim=tube_update->animate;
  if (ballT!=NULL) delete ballT;
  ballT=NULL;
  if ((anim==1)||(anim==2))
  {
    ball_kol=(int)(0.35*height/radius);   
    ballT=new CBaloonTube*[ball_kol];
    for (int rt=0;rt<ball_kol;rt++)
    {
    ballT[rt]=new CBaloonTube((TDist)radius,(TDist)height,height*rt/ball_kol);
    }
  }
  switch (angle)
  {
  case 0 : 
    {
    ang=90;
    y=y0;
    x=x0+height;
    tube_update->Bounds.left=x0;
    tube_update->Bounds.top=y0+radius;
    tube_update->Bounds.height=2*radius;
    tube_update->Bounds.width=height;
    break;
    }
  case 90 : 
    {
    ang=0;
    x=x0;
    y=y0+height;
    tube_update->Bounds.left=x-radius;
    tube_update->Bounds.top=y;
    tube_update->Bounds.height=height;
    tube_update->Bounds.width=2*radius;
    break;
    }
  case 180 : 
    {
    ang=-90;
    y=y0;
    x=x0-height;
    tube_update->Bounds.left=x;
    tube_update->Bounds.top=y+radius;
    tube_update->Bounds.height=2*radius;
    tube_update->Bounds.width=height;
    break;
    }
  case 270 : 
    {
    ang=180;
    x=x0;
    y=y0-height;
    tube_update->Bounds.left=x0-radius;
    tube_update->Bounds.top=y0;
    tube_update->Bounds.height=height;
    tube_update->Bounds.width=2*radius;
    break;
    }
  }
  bound=tube_update->Bounds;
  if (dev!=NULL)dev->Show(ConnectWnd,0);
  if (termo!=NULL)termo->Show(ConnectWnd,0);
  if (flmeter!=NULL)flmeter->Show(ConnectWnd,0);
  if (heatcntr!=NULL)heatcntr->Show(ConnectWnd,0);
  switch (pribor)
  {
  case 1:
    {
      DEVICE_DATA devi;
      devi.X0=(x+x0)/2;
      devi.Y0=(y+y0)/2;
      devi.Xt=tube_update->Xt;
      devi.Yt=tube_update->Yt;
      if (right) 
      {
        devi.Angle=angle-90;
        if (devi.Angle<0)devi.Angle+=360;
      }
      else
      {
        devi.Angle=angle+90;
        if (devi.Angle>=360)devi.Angle-=360;
      }     
      devi.Radius=radius;
      devi.Rad=2*radius;
      devi.Height=6*radius;
      strcpy(devi.Text,text);
      if (dev==NULL) dev=new CDevice(&devi);
      else dev->Update(&devi);
      if (dev!=NULL) dev->ShowText(showtext);
      if (dev!=NULL) dev->Show(ConnectWnd,1);
      strcpy(cd.TypeString,"Манометр");
      break;
    }
  case 2:
    {
      TERMO_DATA terr;
      terr.X0=(x+x0)/2;
      terr.Y0=(y+y0)/2;
      terr.Xt=tube_update->Xt;
      terr.Yt=tube_update->Yt;      
      if (right) 
      {
        terr.Angle=angle-90;
        if (terr.Angle<0)terr.Angle+=360;
      }
      else
      {
        terr.Angle=angle+90;
        if (terr.Angle>=360)terr.Angle-=360;
      }     
      terr.Radius=radius;
      terr.Rad=radius;
      terr.Height=4*radius;
      if ((terr.Angle==90)||(terr.Angle==270))terr.Height/=2.0;
      terr.Width=6*radius;
      strcpy(terr.Text,text);
      if (termo==NULL) termo=new CTermo(&terr);
      else termo->Update(&terr);
      if (termo!=NULL) termo->ShowText(showtext);
      if (termo!=NULL) termo->Show(ConnectWnd,1);
      strcpy(cd.TypeString,"Термометр");
      break;
    }
  case 3:
    {
      FLOWMETER_DATA flm;
      flm.X0=(x+x0)/2;
      flm.Y0=(y+y0)/2;
      flm.Xt=tube_update->Xt;
      flm.Yt=tube_update->Yt;           
      flm.Radius=radius;
      switch (angle)
      {
      case 0:{flm.Angle=90;break;}
      case 90:{flm.Angle=0;break;}
      case 180:{flm.Angle=270;break;}
      case 270:{flm.Angle=180;break;}
      }       
      flm.Rad=2*radius;
      strcpy(flm.Text,text);
      if (flmeter==NULL) flmeter=new CFlowmeter(&flm);
      else flmeter->Update(&flm);
      if (flmeter!=NULL) flmeter->ShowText(showtext);
      if (flmeter!=NULL)flmeter->Show(ConnectWnd,1);
      strcpy(cd.TypeString,"Расходомер");
      break;
    }   
  case 4:
    {
      FLOWMETER_DATA htm;
      htm.X0=(x+x0)/2;
      htm.Y0=(y+y0)/2;
      htm.Xt=tube_update->Xt;
      htm.Yt=tube_update->Yt;
      htm.Radius=radius;
      switch (angle)
      {
      case 0:{htm.Angle=90;break;}
      case 90:{htm.Angle=0;break;}
      case 180:{htm.Angle=270;break;}
      case 270:{htm.Angle=180;break;}
      }       
      htm.Rad=2*radius;
      strcpy(htm.Text,text);
      if (heatcntr==NULL) heatcntr=new CHeatCounter(&htm);
      else heatcntr->Update(&htm);
      if (heatcntr!=NULL) heatcntr->ShowText(showtext);
      if (heatcntr!=NULL)heatcntr->Show(ConnectWnd,1);
      strcpy(cd.TypeString,"Теплосчетчик");
      break;
    }
  default :
    {
      strcpy(cd.TypeString,"Труба");
      break;
    }
  }
  SetTypeParam(tube_update,TUBE);
  CTempl::Update(tube_update);
}
void CTube::UpdateFromBounds(ELEMENT_DATA *tube_update)
{
  angle=tube_update->Angle;
  radius=tube_update->Radius;
  switch (angle)
  {
  case 0 : 
    {
    ang=90;
    tube_update->Bounds.height=2*radius;
    height=tube_update->Height=tube_update->Bounds.width;
    x0=tube_update->X0=tube_update->Bounds.left;
    y0=tube_update->Y0=y=tube_update->Bounds.top-radius;
    x=x0+height;
    break;
    }
  case 90 : 
    {
    ang=0;
    tube_update->Bounds.width=2*radius;
    height=tube_update->Height=tube_update->Bounds.height;
    x0=tube_update->X0=x=tube_update->Bounds.left+radius;
    y0=tube_update->Y0=tube_update->Bounds.top-height;
    y=y0+height;
    break;
    }
  case 180 : 
    {
    ang=-90;
    tube_update->Bounds.height=2*radius;
    height=tube_update->Height=tube_update->Bounds.width;
    x=tube_update->Bounds.left;
    y0=tube_update->Y0=y=tube_update->Bounds.top-radius;
    x0=tube_update->X0=x+height;
    break;
    }
  case 270 : 
    {
    ang=180;
    tube_update->Bounds.width=2*radius;
    height=tube_update->Height=tube_update->Bounds.height;
    x0=tube_update->X0=x=tube_update->Bounds.left+radius;
    y0=tube_update->Y0=tube_update->Bounds.top;
    y=y0-height;
    break;
    }
  }
  Update(tube_update);
  if (dev!=NULL){dev->ResetLocation();tube_update->Xt=dev->Xt;tube_update->Yt=dev->Yt;}
  if (termo!=NULL){termo->ResetLocation();tube_update->Xt=termo->Xt;tube_update->Yt=termo->Yt;}
  if (flmeter!=NULL){flmeter->ResetLocation();tube_update->Xt=flmeter->Xt;tube_update->Yt=flmeter->Yt;}
  if (heatcntr!=NULL){heatcntr->ResetLocation();tube_update->Xt=heatcntr->Xt;tube_update->Yt=heatcntr->Yt;}
}
void CTube::GetData(ELEMENT_DATA *tube_data)const
{
  tube_data->X0=x0;
  tube_data->Y0=y0;
  tube_data->Angle=angle;
  tube_data->Radius=radius;
  tube_data->Height=height;
  tube_data->animate=anim;  
  tube_data->Pribor=pribor;
  tube_data->Right=right;
  strcpy(tube_data->Text,text); 
  if (dev!=NULL){tube_data->Xt=dev->Xt;tube_data->Yt=dev->Yt;}
  if (termo!=NULL){tube_data->Xt=termo->Xt;tube_data->Yt=termo->Yt;}
  if (flmeter!=NULL){tube_data->Xt=flmeter->Xt;tube_data->Yt=flmeter->Yt;}
  if (heatcntr!=NULL){tube_data->Xt=heatcntr->Xt;tube_data->Yt=heatcntr->Yt;}
  CTempl::GetData(tube_data); 
}
void CTube::Draw(BOOL ShowNode)
{
  DrawNodes(ShowNode);
  double x,y;
  if (anim==0) auxTube((double)x0,(double)y0,x,y,(double)radius,angle,(double)height);
  else 
  {
   GLdouble params[4];
   glGetDoublev(GL_CURRENT_COLOR,params); 
   auxHalfTube((double)x0,(double)y0,x,y,(double)radius,angle,(double)height);
   GLfloat amb[4],ambient[4] = {0.0,0.0, 0.0, 1};
     glGetFloatv(GL_LIGHT_MODEL_AMBIENT,amb);     
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
   if (anim==1) SetColorUb(ConnectWnd->sc.ColdColor);
   else SetColorUb(ConnectWnd->sc.HotColor);
   double r=radius*0.7;
   glNormal3d(0,0,1);
   if ((angle==0)||(angle==180))
    {
    glBegin(GL_QUADS);
    glVertex3d(x0,y-r,0.1);
    glVertex3d(x,y-r,0.1);
    glVertex3d(x,y+r,0.1);
    glVertex3d(x0,y+r,0.1);
      glEnd();
    }
   else 
    {
    glBegin(GL_QUADS);
    glVertex3d(x0-r,y0,0.1);
    glVertex3d(x-r,y,0.1);
    glVertex3d(x+r,y,0.1);
    glVertex3d(x0+r,y0,0.1);
      glEnd();
    }
   glColor3dv(params);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
  }
  if ((dev!=NULL)&&(dev->ON())){dev->ShowText(showtext);dev->Draw();}
  if ((termo!=NULL)&&(termo->ON())){termo->ShowText(showtext);termo->Draw();}
  if ((flmeter!=NULL)&&(flmeter->ON())){flmeter->ShowText(showtext);flmeter->Draw();}
  if ((heatcntr!=NULL)&&(heatcntr->ON())){heatcntr->ShowText(showtext);heatcntr->Draw();}
}
void CTube::Animate()
{
  if (Focused) DrawBound(&bound);
  if (!anim)return;
  if (!group.groupOn)return;
  if (ConnectWnd!=NULL)
  {
    if (!ConnectWnd->worksp.ShowBalloons)return;
  } 
  glPushMatrix();
  glTranslated(0,0,0.1);
  switch (angle)
  {
  case 0: {RotateTo0(x0,y0); break;}
  case 90: {RotateTo90(x0,y0); break;}
  case 180: {RotateTo180(x0,y0); break;}
  case 270: {RotateTo270(x0,y0); break;}
  }
  for (int rt=0;rt<ball_kol;rt++)
  {
    if (ballT!=0) ballT[rt]->Animate();
  }
  glPopMatrix();
}
int CTube::GetNodeKol() const
{
  return 2;
}
CShemaNode CTube::GetNode(int number) const
{
  CShemaNode node;
  if (number==0)
  {
    node.X=x0;
    node.Y=y0;
    node.Radius=radius;
    node.Angle=angle-180;
    if (node.Angle<0)node.Angle+=360;
  }
  else 
  {
    node.X=x;
    node.Y=y;
    node.Radius=radius;
    node.Angle=angle;
  }
  return node;
}
bool CTube::HasText()
{
  if (pribor!=0) return true;
  else return false;
}
bool CTube::PointInTxtRgn(MOUSEXY *m)
{
  if (dev!=NULL)if (dev->ON()) return dev->PointInRgn(m);
  if (termo!=NULL)if (termo->ON()) return termo->PointInRgn(m);
  if (flmeter!=NULL)if (flmeter->ON()) return flmeter->PointInRgn(m); 
  if (heatcntr!=NULL)if (heatcntr->ON()) return heatcntr->PointInRgn(m);
  return false;
}
void CTube::MoveText(RECTAN * newPos)
{
  if (dev!=NULL)if (dev->ON()) {dev->MoveText(newPos);return;}
  if (termo!=NULL)if (termo->ON()) {termo->MoveText(newPos);return;}
  if (flmeter!=NULL)if (flmeter->ON()) {flmeter->MoveText(newPos);return;}
  if (heatcntr!=NULL)if (heatcntr->ON()) {heatcntr->MoveText(newPos);return;}
}
RECTAN CTube::GetTxtBounds()
{
  if (dev!=NULL)if (dev->ON()) return dev->GetBound();
  if (termo!=NULL)if (termo->ON()) return termo->GetBound();
  if (flmeter!=NULL)if (flmeter->ON()) return flmeter->GetBound();
  if (heatcntr!=NULL)if (heatcntr->ON()) return heatcntr->GetBound();
  return CTempl::GetTxtBounds();  
}
void CTube::ConnectionCreate(CTemplWnd * newConnectWnd)
{
  CTempl::ConnectionCreate(newConnectWnd);
  if (dev!=NULL)if (dev->ON()) dev->ConnectionCreate(newConnectWnd);
  if (termo!=NULL)if (termo->ON()) termo->ConnectionCreate(newConnectWnd);
  if (flmeter!=NULL)if (flmeter->ON()) flmeter->ConnectionCreate(newConnectWnd);
  if (heatcntr!=NULL)if (heatcntr->ON()) heatcntr->ConnectionCreate(newConnectWnd);
}
//-------------------------------------------------------------------
CBaloonTube::CBaloonTube(TDist Radius,TDist Height,double Phase)
{
  if (Radius<=0)return;
  radius=Radius;
  height=Height;
  Update();
  xp=-Phase;
}
void CBaloonTube::Update()
{
  xp=0;
  yp=0;
    storona=0.15*radius;
  step=0.1+1/radius;//..0.5*radius;
  ypos=1;
}
CBaloonTube::~CBaloonTube()
{
}
void CBaloonTube::Animate()
{
 glPushMatrix();
 if (xp<height-step)
 {
 xp+=step;
 yp=(0.5-ypos)*storona*0.8;
 ypos=!ypos;
 }
 else xp=0;
 if (xp>=0)
 {
 glNormal3d(0,1,0);
 glBegin(GL_LINE_STRIP);
    glVertex3d(yp-storona,0.1,xp);
    glVertex3d(yp,0.1,xp-storona);
    glVertex3d(yp+storona,0.1,xp);
    glVertex3d(yp,0.1,xp+storona);
    glVertex3d(yp-storona,0.1,xp);
 glEnd();
 }
 glPopMatrix();
}
//-------------------------------------------------------------------
CBaloon::CBaloon(TDist X0,TDist Y0,TDist X,TDist Y,TDist Radius,TDist Height,double *Sin1,double *Cos1,double Phase)
{
  if (Radius<=0)return;
  x0=X0;
  y0=Y0;
  x=X;
  y=Y;
  radius=Radius;
  height=Height;
  Update();
  xp=x0-Phase;
  for (int i=0; i<=9;i++)
  {
    sin1[i]=Sin1[i];
    cos1[i]=Cos1[i];
  }
}
void CBaloon::Update()
{
  xp=x0;
    yp=y0;
  pos=0;
  storona=0.15*radius;
  step=pi*radius/9.0;
}
CBaloon::~CBaloon()
{
}
void CBaloon::Animate()
{
 glPushMatrix();
 glTranslated(0,0,0.1);
 if (pos==0)
 {
 xp-=step;
 if (xp<=x0-height)
   {
   xx0=x0-height;
   yy0=y0;
   cnt=0;
   pos=1;
   }
 }
 if ((pos==1)||(pos==5))
 {
 xp=xx0-2*radius*sin1[cnt];
 yp=yy0+2*radius*(1-cos1[cnt]);
 if (cnt==9)
   {
    cnt=0;
    if (pos==5) pos=6;
    else pos=2;
   }
 cnt++;
 }
 if (pos==2)
 {
 xp=xx0-2*radius*cos1[cnt];
 yp=yy0+2*radius*(1+sin1[cnt]);
 if (cnt==9)
   {
    cnt=0;
    pos=3;
   }
 cnt++;
 }
 if (pos==3)
 {
 xp=xx0+2*radius*sin1[cnt];
 yp=yy0+2*radius*(1+cos1[cnt]);
 if (cnt==9)
   {
    cnt=0;
    pos=4;
   }
 cnt++;
 }
 if (pos==4)
 {
 xp=xx0+2*radius*cos1[cnt];
 yp=yy0+2*radius*(1-sin1[cnt]);
 if (cnt==9)
   {
    cnt=0;
    pos=5;
   }
 cnt++;
 }
 if (pos==6)
 {
 yp+=step;
 if (yp>=y-0.1*height)
   {
   xp=x0;
   yp=y0;
   pos=0;
   }
 }
 if (xp<=x0-0.1*height)
 {
 glNormal3d(0,0,1);
 glBegin(GL_LINE_STRIP);
    glVertex3d(xp,yp-storona,0.1);
    glVertex3d(xp-storona,yp,0.1);
    glVertex3d(xp,yp+storona,0.1);
    glVertex3d(xp+storona,yp,0.1);
    glVertex3d(xp,yp-storona,0.1);
 glEnd();
 }
 glPopMatrix();

}
//-------------------------------------------------------------------
CNasos::CNasos(ELEMENT_DATA * nas_create,BOOL FromBounds)
{
  Focused=0;
  InitConnect();
  ball=NULL;
  sw=NULL;
  position=0;
  for (int i=1; i<=8;i++)
  {
    sin1[i]=sin(i*pi/18.0);
    cos1[i]=cos(i*pi/18.0);
  }
  sin1[0]=0;
  sin1[9]=1;
  cos1[0]=1;
  cos1[9]=0;
  if (FromBounds)UpdateFromBounds(nas_create);
  else Update(nas_create);
}
CNasos::CNasos(const CNasos &prev_nas) //конструктор копий
{
  Focused=0;
  CopyConnect(&prev_nas);
  ball=NULL;
  sw=NULL;
  position=0;
  for (int i=1; i<=8;i++)
  {
    sin1[i]=sin(i*pi/18.0);
    cos1[i]=cos(i*pi/18.0);
  }
  sin1[0]=0;
  sin1[9]=1;
  cos1[0]=1;
  cos1[9]=0;
  ELEMENT_DATA nas;
  prev_nas.GetData(&nas); 
  Update(&nas);
}
CNasos::~CNasos()
{
  if (ball!=NULL) delete ball;
  if (connected!=NULL)delete connected;
  if (sw!=NULL) delete sw;
}
void CNasos::Update(ELEMENT_DATA * nas_update)
{
  if (nas_update->Radius<=0)return;
  x0=nas_update->X0;
  y0=nas_update->Y0;
  radius=nas_update->Radius;
  anim=nas_update->animate;
  height=5*radius;
  x=x0-height-2*radius;
  y=y0+2*radius+height;
  if (ball!=NULL) delete ball;
  ball=NULL;
  ball_kol=15/radius;
  ball=new CBaloon*[ball_kol];
  for (int bl=0;bl<ball_kol;bl++)
  {
  ball[bl]=new CBaloon(x0,y0,x,y,radius,height,sin1,cos1,-10*height*bl/ball_kol);
  }
  bound.left=x-radius;
  bound.top=y;
  bound.width=radius*8;
  bound.height=bound.width;
  SWITCH_DATA s;
  s.X0=x0-height;
  s.Y0=y0+2*radius;
  s.Z0=0;
  s.Xt=nas_update->Xt;
  s.Yt=nas_update->Yt;
  s.Height=(TDist)(height/2.0);
  if (sw==NULL) sw=new CSwitcher(&s);
  else sw->Update(&s);
  strcpy(cd.TypeString,"Насос");
  SetTypeParam(nas_update,NASOS);
  CTempl::Update(nas_update); 
}
void CNasos::UpdateFromBounds(ELEMENT_DATA * nas_update)
{
  nas_update->Bounds.height=nas_update->Radius*8;
  nas_update->Bounds.width=nas_update->Bounds.height;
  nas_update->X0=nas_update->Bounds.left+nas_update->Radius*8;
  nas_update->Y0=nas_update->Bounds.top-nas_update->Radius*7;
  Update(nas_update);
  if (sw!=NULL)
  {
    RECTAN newpos;        //вместо ResetLocation
    newpos.left=x0-2.5*height;  //вместо ResetLocation
    newpos.top=y0-0.5*height; //вместо ResetLocation  
    sw->MoveText(&newpos);    //вместо ResetLocation
  }
}
void CNasos::GetData(ELEMENT_DATA * nas_data) const
{
  nas_data->X0=x0;
  nas_data->Y0=y0;
  nas_data->Radius=radius;
  nas_data->animate=anim;
  if (sw!=NULL)
  {
    nas_data->Xt=sw->Xt;
    nas_data->Yt=sw->Yt;
  }
  CTempl::GetData(nas_data);  
} 
void CNasos::Draw(BOOL ShowNode)
{
  DrawNodes(ShowNode);
  auxNasos(ConnectWnd->sc.ColdColor,ConnectWnd->sc.HotColor,x0,y0,radius,0,height,anim);
  if (sw!=NULL) sw->Draw();
}
void CNasos::Animate()
{
  if (Focused) DrawBound(&bound);
  if (sw->ON()&&(!groupOn_mirror)) 
  {
    if (ConnectWnd!=NULL)((CSheme*)ConnectWnd)->Action((WPARAM)group.group,NASOS_OPENED);
    groupOn_mirror=true;
  }
  if (!sw->ON()&&(groupOn_mirror)) 
  {
    if (ConnectWnd!=NULL)((CSheme*)ConnectWnd)->Action((WPARAM)group.group,NASOS_CLOSED);
    groupOn_mirror=false;     
  } 

 glPushMatrix();
 glTranslated(x0-height,y0+2*radius,0.4);
  glRotated(-20*position,0,0,1);  
  for (int t=0;t<4;t++)
  {
   glRotated(90,0,0,1); 
   glNormal3d(0,0,1);
   glBegin(GL_QUADS);
    glVertex3d(0,0,0);
    glVertex3d(-0.24*radius,radius,0);
    glVertex3d(-0.14*radius,1.5*radius,0);
    glVertex3d(0.24*radius,1.2*radius,0);
    
    glVertex3d(-0.14*radius,1.5*radius,0);
    glVertex3d(-0.24*radius,radius,0);
    glVertex3d(0.1*radius,0.8*radius,0);
    glVertex3d(0.6*radius,2*radius,0);  
    
   glEnd();
  }
 if (group.groupOn)position++;
 if (position>8)position=0;
 glPopMatrix();
 sw->Animate(); 
 if (!group.groupOn)return;
 if (ConnectWnd!=NULL)
 {
  if (!ConnectWnd->worksp.ShowBalloons)return;
 }
 for (int bl=0;bl<ball_kol;bl++)
  {
   ball[bl]->Animate();
  }   
}
int CNasos::GetNodeKol() const
{
  return 2;
}
CShemaNode CNasos::GetNode(int number) const
{
  CShemaNode node;
  if (number==0)
  {
    node.X=x0;
    node.Y=y0;
    node.Radius=radius;
    node.Angle=0;
  }
  else 
  {
    node.X=x;
    node.Y=y;
    node.Radius=radius;
    node.Angle=90;
  }
  return node;
}
void CNasos::ConnectionCreate(CTemplWnd * newConnectWnd)
{
  CTempl::ConnectionCreate(newConnectWnd);
  if (sw!=NULL) sw->ConnectionCreate(newConnectWnd);  
}
//-------------------------------------------------------------------
bool CDescreetDevice::HasSubDevice()
{
  return true;  
}
bool CDescreetDevice::PointInTxtRgn(MOUSEXY *m)
{
  if (sw!=NULL) return sw->PointInRgn(m); 
}
void CDescreetDevice::MoveText(RECTAN * newPos)
{
  if (sw!=NULL) sw->MoveText(newPos); 
}
RECTAN CDescreetDevice::GetTxtBounds()
{
  if (sw!=NULL)return sw->GetBound();
  return CTempl::GetTxtBounds();  
}
bool CDescreetDevice::PointInClickRgn(MOUSEXY *m)
{
  if (sw!=NULL) return sw->PointInRgn(m); 
}
bool CDescreetDevice::OnClickInClickRgn(MOUSEXY *m)
{
  if (sw!=NULL) 
  {
    sw->Click();    
    return sw->ON();
  }
  return  CTempl::OnClickInClickRgn(m);
}
void CDescreetDevice::SetGroupON(bool on)
{
  /*if (sw!=NULL) 
  {
    if (on!=sw->ON())sw->Click();
  }*/
  CTempl::SetGroupON(on); 
}
//-------------------------------------------------------------------
CBattery::CBattery(ELEMENT_DATA *bat_create,BOOL FromBounds)
{
  Focused=0;
  InitConnect();
  if (FromBounds)UpdateFromBounds(bat_create);
  else Update(bat_create);
}
CBattery::CBattery(const CBattery &prev_bat) //конструктор копий
{
  Focused=0;
  CopyConnect(&prev_bat);
  ELEMENT_DATA bat;
  prev_bat.GetData(&bat); 
  Update(&bat);
}
void CBattery::GetData(ELEMENT_DATA *bat_data)const
{
  bat_data->X0=x0;
  bat_data->Y0=y0;
  bat_data->Radius=radius;
  bat_data->Kol=kol;
  bat_data->Height=height+radius;
  CTempl::GetData(bat_data);    
}
CBattery::~CBattery()
{
  if (connected!=NULL)delete connected;
}
void CBattery::Update(ELEMENT_DATA *bat_update)
{
  if ((bat_update->Radius<=0)||(bat_update->Kol<=0))return;
  if (bat_update->Height<10*bat_update->Radius)return;
  radius=bat_update->Radius;
  x0=bat_update->X0;
  y0=bat_update->Y0;
  kol=bat_update->Kol;
  height=bat_update->Height-radius;
  r1=(height-radius*(3+2*kol))/double(kol-1);//Формула : r1=(height-radius*(2*(зазор_начальный)+2*kol))/(kol-1) !  (зазор_начальный) изменить и в auxBattery(...)
  //x=x0+radius+height;
  x=x0;
  y=y0-15*radius; 
  //height=1.7*(radius*(kol+1)+radius*(kol)*0.5);

  bound.left=bat_update->X0;
  bound.top=bat_update->Y0+2*radius;
  bound.height=19*radius;
  bound.width=bat_update->Height;
  
  bat_update->Bounds=bound;

  strcpy(cd.TypeString,"Батарея");
  SetTypeParam(bat_update,BATTERY);
  CTempl::Update(bat_update);
}
void CBattery::UpdateFromBounds(ELEMENT_DATA *bat_update)
{
bat_update->X0=bat_update->Bounds.left;
bat_update->Y0=bat_update->Bounds.top-2*bat_update->Radius;
bat_update->Height=bat_update->Bounds.width;
Update(bat_update);
}
void CBattery::Draw(BOOL ShowNode)
{
  DrawNodes(ShowNode);
  auxBattery(x0,y0,radius,kol,height,r1);
}
void CBattery::Animate()
{
  if (Focused) DrawBound(&bound);
}
int CBattery::GetNodeKol() const
{
  return 2;
}
CShemaNode CBattery::GetNode(int number) const
{
  CShemaNode node;
  if (number==0)
  {
    node.X=x0;
    node.Y=y0;
    node.Radius=radius;
    node.Angle=180;
  }
  else 
  {
    node.X=x;
    node.Y=y;
    node.Radius=radius;
    node.Angle=180;
  }
  return node;
}
//-------------------------------------------------------------------
CTurn::CTurn(ELEMENT_DATA *turn_create,BOOL FromBounds)
{ 
  Focused=0;
  InitConnect();
  if (FromBounds)UpdateFromBounds(turn_create);
  else Update(turn_create);
}
CTurn::CTurn(const CTurn &prev_turn) //конструктор копий
{
  Focused=0;
  CopyConnect(&prev_turn);
  ELEMENT_DATA turn;
  prev_turn.GetData(&turn); 
  Update(&turn);
}
CTurn::~CTurn()
{ 
  if (connected!=NULL)delete connected;
}
void CTurn::Update(ELEMENT_DATA *turn_update)
{
  if (turn_update->Radius<=0)return;
  if ((turn_update->Angle!=0)&&
    (turn_update->Angle!=90)&&
    (turn_update->Angle!=180)&&
    (turn_update->Angle!=270))return;
  if(turn_update->Height<3*turn_update->Radius)return;
  angle=turn_update->Angle;
  radius=turn_update->Radius;
  x0=turn_update->X0;
  y0=turn_update->Y0;
  right=turn_update->Right;
  height=turn_update->Height;
  TDist radix=height-radius;    //
  bound.width=bound.height=height;
  switch (angle)
  {
  case 0 : 
    {
      bound.left=x0;
      if (!right) 
      {
        angle1=90;x=x0+radix;y=y0+radix;
        bound.top=y0+radix;
      }
      else 
      {
        angle1=270;x=x0+radix;y=y0-radix;
        bound.top=y0+radius;
      }
      break;
    }
  case 90 : 
    {
      bound.top=y0+height;
      if (!right)
      {
        angle1=180;x=x0-radix;y=y0+radix;
        bound.left=x0-radix; 
      }
      else 
      {
        angle1=0;x=x0+radix;y=y0+radix;
        bound.left=x0-radius;
      }
      break;
      }
  case 180 : 
    {
      bound.left=x0-height;
      if (!right)
      {
        angle1=270;x=x0-radix;y=y0-radix;
        bound.top=y0+radius;
      }
      else 
      {
        angle1=90;x=x0-radix;y=y0+radix;
        bound.top=y0+radix;
      }
      break;
    }

  case 270 : 
    {
      bound.top=y0;
      if (!right)
      {
        x=x0+radix;y=y0-radix;
        bound.left=x0-radius;
      }
      else 
      {
        angle1=180;x=x0-radix;y=y0-radix;
        bound.left=x0-radix;
      }
      break;
    }
  }
  turn_update->Bounds=bound;
  strcpy(cd.TypeString,"Уголок");
  SetTypeParam(turn_update,TURN);
  CTempl::Update(turn_update);
}
void CTurn::UpdateFromBounds(ELEMENT_DATA *turn_update)
{
  turn_update->Height=turn_update->Bounds.height;
  TDist radix=turn_update->Height-turn_update->Radius;
  switch (turn_update->Angle)
  {
  case 0 : 
    {
      turn_update->X0=turn_update->Bounds.left;
      if (!turn_update->Right){turn_update->Y0=turn_update->Bounds.top-radix;}
      else {turn_update->Y0=turn_update->Bounds.top-turn_update->Radius;}
      break;
    }
  case 90 : 
    {
      turn_update->Y0=turn_update->Bounds.top-turn_update->Height;
      if (!turn_update->Right){turn_update->X0=turn_update->Bounds.left+radix;}
      else {turn_update->X0=turn_update->Bounds.left+turn_update->Radius;}
      break;
    }
  case 180 : 
    {
      turn_update->X0=turn_update->Bounds.left+turn_update->Height;
      if (!turn_update->Right){turn_update->Y0=turn_update->Bounds.top-turn_update->Radius;}
      else {turn_update->Y0=turn_update->Bounds.top-radix;}
      break;
    }
  case 270 : 
    {
      turn_update->Y0=turn_update->Bounds.top;
      if (!turn_update->Right){turn_update->X0=turn_update->Bounds.left+turn_update->Radius;}
      else {turn_update->X0=turn_update->Bounds.left+radix;}
      break;
    }
  }
  Update(turn_update);
}
void CTurn::GetData(ELEMENT_DATA *turn_data)const
{
  turn_data->Angle=angle;
  turn_data->Height=height;
  turn_data->Radius=radius;
  turn_data->Right=right;
  turn_data->X0=x0;
  turn_data->Y0=y0;
  CTempl::GetData(turn_data);   
}
void CTurn::Draw(BOOL ShowNode)
{
  DrawNodes(ShowNode);
  int a=angle;
  double x,y;
  auxTurn(x0,y0,x,y,radius,a,right,height);
}
void CTurn::Animate()
{
  if (Focused) DrawBound(&bound);
}
int CTurn::GetNodeKol() const
{
  return 2;
}
CShemaNode CTurn::GetNode(int number) const
{
  CShemaNode node;
  if (number==0)
  {
    node.X=x0;
    node.Y=y0;
    node.Radius=radius;
    node.Angle=angle-180;
    if (node.Angle<0)node.Angle+=360;
  }
  else 
  {
    node.X=x;
    node.Y=y;
    node.Radius=radius;
    if (right)node.Angle=angle-90;
    else node.Angle=angle-270;
    if (node.Angle<0)node.Angle+=360;   
  }
  return node;
}
//-------------------------------------------------------------------
CTrinity::CTrinity(ELEMENT_DATA *tri_create,BOOL FromBounds)
{
  Focused=0;
  InitConnect();
  if (FromBounds)UpdateFromBounds(tri_create);
  else Update(tri_create);
}
CTrinity::CTrinity(const CTrinity &prev_tri) //конструктор копий
{
  Focused=0;
  CopyConnect(&prev_tri);
  ELEMENT_DATA tri;
  prev_tri.GetData(&tri); 
  Update(&tri);
}
CTrinity::~CTrinity()
{
  if (connected!=NULL)delete connected;
}
void CTrinity::Update(ELEMENT_DATA *tri_update)
{
  if (tri_update->Radius<=0)return;
  if (tri_update->Height<2*tri_update->Radius)return;
  radius=tri_update->Radius;
  x0=tri_update->X0;
  y0=tri_update->Y0;
  angle=tri_update->Angle;
  height=tri_update->Height;
  right=tri_update->Right;
  switch (angle)
  {
  case 0:
    {
      x=x0+2*height;y=y0;x1=x0+height;
      bound.height=height+radius;
      bound.width=2*height;
      bound.left=x0;
      if (right) {y1=y0-height;ang=270;bound.top=y0+radius;}
      else {y1=y0+height;ang=90;bound.top=y1;}
      break;
    }
  case 90:
    {
      y=y0+2*height;x=x0;y1=y0+height;
      bound.width=height+radius;
      bound.height=2*height;
      bound.top=y;      
      if (right) {x1=x0+height;ang=0;bound.left=x-radius;}
      else {x1=x0-height;ang=180;bound.left=x1;}
      break;
    }
  case 180:
    {
      x=x0-2*height;y=y0;x1=x0-height;
      bound.height=height+radius;
      bound.width=2*height;
      bound.left=x;
      if (right) {y1=y0+height;ang=90;bound.top=y1;}
      else {y1=y0-height;ang=270;bound.top=y+radius;}
      break;
    }
  case 270:
    {
      y=y0-2*height;x=x0;y1=y0-height;
      bound.width=height+radius;;
      bound.height=2*height;
      bound.top=y0;     
      if (right) {x1=x0-height;ang=180;bound.left=x1;}
      else {x1=x0+height;ang=0;bound.left=x0-radius;}
      break;
    }
  }
  tri_update->Bounds=bound;
  strcpy(cd.TypeString,"Тройник");
  SetTypeParam(tri_update,TRINITY);
  CTempl::Update(tri_update);
}
void CTrinity::UpdateFromBounds(ELEMENT_DATA *tri_update)
{
  switch (tri_update->Angle)
  {
  case 0:
    {
      tri_update->Height=tri_update->Bounds.height-tri_update->Radius;
      tri_update->Bounds.width=2*tri_update->Height;//tri_update->Bounds.height;
      tri_update->X0=tri_update->Bounds.left;
      if (tri_update->Right) tri_update->Y0=tri_update->Bounds.top-tri_update->Radius;
      else tri_update->Y0=tri_update->Bounds.top-tri_update->Height;
      break;
    }
  case 90:
    {
      tri_update->Height=tri_update->Bounds.width-tri_update->Radius;
      tri_update->Bounds.height=2*tri_update->Height;
      tri_update->Y0=tri_update->Bounds.top-tri_update->Bounds.height;
      if (tri_update->Right) tri_update->X0=tri_update->Bounds.left+tri_update->Radius;
      else tri_update->X0=tri_update->Bounds.left+tri_update->Height;
      break;
    }
  case 180:
    {
      tri_update->Height=tri_update->Bounds.height-tri_update->Radius;
      tri_update->Bounds.width=2*tri_update->Height;
      tri_update->X0=tri_update->Bounds.left+tri_update->Bounds.width;
      if (tri_update->Right) tri_update->Y0=tri_update->Bounds.top-tri_update->Height;
      else tri_update->Y0=tri_update->Bounds.top-tri_update->Radius;
      break;
    }
  case 270:
    {
      tri_update->Height=tri_update->Bounds.width-tri_update->Radius;
      tri_update->Bounds.height=2*tri_update->Height;
      tri_update->Y0=tri_update->Bounds.top;
      if (tri_update->Right) tri_update->X0=tri_update->Bounds.left+tri_update->Height;
      else tri_update->X0=tri_update->Bounds.left+tri_update->Radius;
      break;
    }
  }
  Update(tri_update);
}
void CTrinity::GetData(ELEMENT_DATA *tri_data)const
{
  tri_data->X0=x0;
  tri_data->Y0=y0;
  tri_data->Angle=angle;
  tri_data->Height=height;
  tri_data->Radius=radius;
  tri_data->Right=right;
  CTempl::GetData(tri_data);    
}
void CTrinity::Draw(BOOL ShowNode)
{
  DrawNodes(ShowNode);
  auxTrinity(x0,y0,radius,angle,height,right);  
}
void CTrinity::Animate()
{
  if (Focused) DrawBound(&bound);
}
int CTrinity::GetNodeKol() const
{
  return 3;
}
CShemaNode CTrinity::GetNode(int number) const
{
  CShemaNode node;
  if (number==0)
  {
    node.X=x0;
    node.Y=y0;
    node.Radius=radius;
    node.Angle=angle-180;
    if (node.Angle<0)node.Angle+=360;
  }
  else
  {
    if (number==1)
    {
      node.X=x;
      node.Y=y;
      node.Radius=radius;
      node.Angle=angle;
    }
    else 
    {
      node.X=x1;
      node.Y=y1;
      node.Radius=radius;
      node.Angle=ang;
    }
  }
  return node;
}
//-------------------------------------------------------------------
#define min_open 0
#define step_open 20
#define max_open step_open*10
CVentil::CVentil(ELEMENT_DATA * vent_create,BOOL FromBounds)
{
  if (vent_create==NULL)return;
  Focused=0;
  InitConnect();
  sw=NULL;
  position=min_open;
  if (FromBounds)UpdateFromBounds(vent_create);
  else Update(vent_create);
}
CVentil::CVentil(const CVentil &prev_vent) //конструктор копий
{
  Focused=0;
  sw=NULL;
  position=min_open;
  CopyConnect(&prev_vent);
  ELEMENT_DATA vent;
  prev_vent.GetData(&vent); 
  Update(&vent);
}
CVentil::~CVentil()
{
  if (connected!=NULL)delete connected;
}
void CVentil::Update(ELEMENT_DATA * vent_update)
{
  if(vent_update->Radius<=0)return;
  if ((vent_update->Opening!=0)&&
     (vent_update->Opening!=1)&&
     (vent_update->Opening!=2)&&
     (vent_update->Opening!=3))return;
  opening=vent_update->Opening;
/////////////////////////////////
  opening=1;

  x0=vent_update->X0;
  y0=vent_update->Y0;
  radius=vent_update->Radius;
  angle=vent_update->Angle;
  height=3*radius;
  SWITCH_DATA s;
  s.Xt=vent_update->Xt;
  s.Yt=vent_update->Yt;     
  s.Height=(TDist)(2.5*radius);
  s.Z0=0;
  switch (angle)
  {
  case 0: 
    {
      y=y0;
      x=x0+2*height;
      bound.left=x0;
      bound.top=y0+6*radius;
      bound.height=7*radius;
      bound.width=2*height;

      s.X0=x0+height;
      s.Y0=(TDist)(y0+height+height/3.0);
      break;
    }
  case 90: 
    {
      x=x0;
      y=y0+2*height;
      bound.left=x0-6*radius;
      bound.top=y;
      bound.height=2*height;
      bound.width=7*radius;

      s.X0=(TDist)(x0-height-height/3.0);
      s.Y0=y0+height;
      break;
    }
  case 180: 
    {
      y=y0;
      x=x0-2*height;
      bound.left=x;
      bound.top=y0+radius;
      bound.height=7*radius;
      bound.width=2*height;

      s.X0=x0-height;
      s.Y0=(TDist)(y0-height-height/3.0);
      break;
    }
  case 270:
    {
      x=x0;
      y=y0-2*height;
      bound.left=x0-radius;
      bound.top=y0;
      bound.height=2*height;
      bound.width=7*radius;

      s.X0=(TDist)(x0+height+height/3.0);
      s.Y0=y0-height;
      break;
    }
  }
  if (opening==0) position=min_open;
  if (opening==1) position=max_open;
  Infocus=FALSE;
  vent_update->Bounds=bound;  
  
  if (sw==NULL) sw=new CSwitcher(&s);
  else sw->Update(&s);
  
  strcpy(cd.TypeString,"Вентиль");
  SetTypeParam(vent_update,VENTIL);
  CTempl::Update(vent_update);
}
void CVentil::UpdateFromBounds(ELEMENT_DATA * vent_update)
{
  switch (vent_update->Angle)
  {
  case 0: 
    {
      vent_update->X0=vent_update->Bounds.left;
      vent_update->Y0=vent_update->Bounds.top-6*vent_update->Radius;
      break;
    }
  case 90: 
    {
      vent_update->X0=vent_update->Bounds.left+6*vent_update->Radius;
      vent_update->Y0=vent_update->Bounds.top-6*vent_update->Radius;
      break;
    }
  case 180: 
    {
      vent_update->X0=vent_update->Bounds.left+6*vent_update->Radius;
      vent_update->Y0=vent_update->Bounds.top-vent_update->Radius;
      break;
    }
  case 270: 
    {
      vent_update->X0=vent_update->Bounds.left+vent_update->Radius;
      vent_update->Y0=vent_update->Bounds.top;//-6*vent_update->Radius;
      break;
    }
  }
  Update(vent_update);
  if (sw!=NULL)     //вместо ResetLocation
  {
    RECTAN newpos;        
    switch (angle)
    {
    case 0: 
      {
        newpos.left=x0-(2+1.25)*radius;     //вместо ResetLocation
        newpos.top=y0+(6+1.25)*radius;  //вместо ResetLocation  
        break;
      }
    case 90: 
      {
        newpos.left=x0-(6+1.25)*radius; //вместо ResetLocation
        newpos.top=y0-(2-1.25)*radius;  //вместо ResetLocation  
        break;
      }
    case 180: 
      {
        newpos.left=x0-(8+1.25)*radius; //вместо ResetLocation
        newpos.top=y0-(6-1.25)*radius;  //вместо ResetLocation  
        break;
      }
    case 270: 
      {
        newpos.left=x0+(6-1.25)*radius; //вместо ResetLocation
        newpos.top=y0+(2+1.25)*radius;  //вместо ResetLocation  
        break;
      }
    }
    sw->MoveText(&newpos);    //вместо ResetLocation
  }
}
void CVentil::GetData(ELEMENT_DATA * vent_data)const
{
  vent_data->X0=x0;
  vent_data->Y0=y0;
  vent_data->Radius=radius;
  vent_data->Angle=angle;
  vent_data->Opening=opening;
  if (sw!=NULL)
  {
    vent_data->Xt=sw->Xt;
    vent_data->Yt=sw->Yt;
  } 
  CTempl::GetData(vent_data); 
}
void CVentil::Draw(BOOL ShowNode)
{
  DrawNodes(ShowNode);
  auxVentil(x0,y0,radius,angle);
  sw->Draw();
}
void CVentil::Animate()
{
  if (Focused) DrawBound(&bound);
glPushMatrix();
double X2,Y2;
switch (angle)  
{
case 0:
  {
    X2=x0+height; //координаты конца 
    Y2=y0+height+height/3.0;  //сальника
    break;
  }
case 90:
  {
    X2=x0-height-height/3.0; //координаты конца 
    Y2=y0+height; //сальника
    break;
  }
case 180:
  {
    X2=x0-height; //координаты конца 
    Y2=y0-height-height/3.0;  //сальника
    break;
  }
case 270:
  {
    X2=x0+height+height/3.0; //координаты конца 
    Y2=y0-height; //сальника
    break;
  }
}
if (sw->ON()&&(!groupOn_mirror)) 
{
  if (opening==1)//Открыто
  {
    if (ConnectWnd!=NULL)((CSheme*)ConnectWnd)->Action((WPARAM)group.group,VENTIL_OPENED);
    groupOn_mirror=true;
  } 
  
}
if (!sw->ON()&&(groupOn_mirror)) 
{
  if (opening==0)//Закрыто
  {
    if (ConnectWnd!=NULL)((CSheme*)ConnectWnd)->Action((WPARAM)group.group,VENTIL_CLOSED);
    groupOn_mirror=false;     
  } 
}
if (sw->ON()) {if (opening!=1)opening=2;}
else {if (opening!=0)opening=3;}
  switch (opening)  
  {
  case 0 :  //Закрыто
       {
         VentShturval(X2,Y2,radius,height,(max_open+1)*2.0,60+min_open,angle);
         break;
       }
  case 1 : //Открыто
       {
         VentShturval(X2,Y2,radius,height,2.0,60+max_open,angle);
         break;
       }
  case 2 :  //Открывается
      {
      VentShturval(X2,Y2,radius,height,(max_open+1)*2.0/(position+1),(int)(60+position) ,angle);
      if (position<max_open)position+=step_open;
      else {position=max_open;opening=1;}
      break;
      }
  case 3 : //Закрывается
      { 
      VentShturval(X2,Y2,radius,height,(max_open+1)*2.0/(position+1),(int)(60+position) ,angle);
      if (position>min_open)position-=step_open;
      else {position=min_open;opening=0;}
      break;
      }
  }
glPopMatrix();
sw->Animate();
}
int CVentil::GetNodeKol() const
{
  return 2;
}
CShemaNode CVentil::GetNode(int number) const
{
  CShemaNode node;
  if (number==0)
  {
    node.X=x0;
    node.Y=y0;
    node.Radius=radius;
    int ang=angle-180;
    if (ang<0)ang+=360;
    node.Angle=ang;
  }
  else 
  {
    node.X=x;
    node.Y=y;
    node.Radius=radius;
    node.Angle=angle;
  }
  return node;
}
void CVentil::ConnectionCreate(CTemplWnd * newConnectWnd)
{
  CTempl::ConnectionCreate(newConnectWnd);
  if (sw!=NULL) sw->ConnectionCreate(newConnectWnd);  
}
//-------------------------------------------------------------------
CDrossel::CDrossel(ELEMENT_DATA *dros_create,BOOL FromBounds)
:CTube(NULL,FromBounds)
{
  Focused=0;
  ballT=NULL;
  dev=NULL;
  termo=NULL;
  flmeter=NULL;
  heatcntr=NULL;
  InitConnect();
  if (FromBounds)UpdateFromBounds(dros_create);
  else Update(dros_create);
}
CDrossel::CDrossel(const CDrossel &prev_dros) //конструктор копий
  :CTube(prev_dros)
{
  Focused=0;
  ballT=NULL;
  dev=NULL;
  termo=NULL;
  flmeter=NULL;
  heatcntr=NULL;
  CopyConnect(&prev_dros);
  ELEMENT_DATA t;
  prev_dros.GetData(&t);
  Update(&t);
}
/*
CDrossel::~CDrossel()
{ 
  if (ballT!=NULL)delete ballT;
  ballT=NULL;
  if (connected!=NULL)delete connected;
}
*/
void CDrossel::Update(ELEMENT_DATA *dros_update)
{
  if(dros_update->Radius<=0)return;
  if(dros_update->Height<6*dros_update->Radius)return;
  x0=dros_update->X0;
  y0=dros_update->Y0;
  angle=dros_update->Angle;
  height=dros_update->Height;
  radius=dros_update->Radius;
  right=dros_update->Right;
  strcpy(text,dros_update->Text);
  anim=dros_update->animate;
  if (ballT!=NULL) delete ballT;
  ballT=NULL;
  if ((anim==1)||(anim==2))
  {
    ball_kol=(int)(0.35*height/radius);   
    ballT=new CBaloonTube*[ball_kol];
    for (int rt=0;rt<ball_kol;rt++)
    {
    ballT[rt]=new CBaloonTube((TDist)radius,(TDist)height,height*rt/ball_kol);
    }
  }
  switch (angle)
  {
  case 0 : 
    {
    ang=90;
    y=y0;
    x=x0+height;
    dros_update->Bounds.left=x0;
    dros_update->Bounds.top=y0+radius;
    dros_update->Bounds.height=2*radius;
    dros_update->Bounds.width=height;
    break;
    }
  case 90 : 
    {
    ang=0;
    x=x0;
    y=y0+height;
    dros_update->Bounds.left=x-radius;
    dros_update->Bounds.top=y;
    dros_update->Bounds.height=height;
    dros_update->Bounds.width=2*radius;
    break;
    }
  case 180 : 
    {
    ang=-90;
    y=y0;
    x=x0-height;
    dros_update->Bounds.left=x;
    dros_update->Bounds.top=y+radius;
    dros_update->Bounds.height=2*radius;
    dros_update->Bounds.width=height;
    break;
    }
  case 270 : 
    {
    ang=180;
    x=x0;
    y=y0-height;
    dros_update->Bounds.left=x0-radius;
    dros_update->Bounds.top=y0;
    dros_update->Bounds.height=height;
    dros_update->Bounds.width=2*radius;
    break;
    }
  }
  bound=dros_update->Bounds;
  strcpy(cd.TypeString,"Дроссель");
  SetTypeParam(dros_update,DROSSEL);
  CTempl::Update(dros_update);
}
void CDrossel::UpdateFromBounds(ELEMENT_DATA *dros_update)
{
  if(dros_update->Radius<=0)return;
  if(dros_update->Height<6*dros_update->Radius)return;
  angle=dros_update->Angle;
  radius=dros_update->Radius;
  switch (angle)
  {
  case 0 : 
    {
    ang=90;
    dros_update->Bounds.height=2*radius;
    height=dros_update->Height=dros_update->Bounds.width;
    x0=dros_update->X0=dros_update->Bounds.left;
    y0=dros_update->Y0=y=dros_update->Bounds.top-radius;
    x=x0+height;
    break;
    }
  case 90 : 
    {
    ang=0;
    dros_update->Bounds.width=2*radius;
    height=dros_update->Height=dros_update->Bounds.height;
    x0=dros_update->X0=x=dros_update->Bounds.left+radius;
    y0=dros_update->Y0=dros_update->Bounds.top-height;
    y=y0+height;
    break;
    }
  case 180 : 
    {
    ang=-90;
    dros_update->Bounds.height=2*radius;
    height=dros_update->Height=dros_update->Bounds.width;
    x=dros_update->Bounds.left;
    y0=dros_update->Y0=y=dros_update->Bounds.top-radius;
    x0=dros_update->X0=x+height;
    break;
    }
  case 270 : 
    {
    ang=180;
    dros_update->Bounds.width=2*radius;
    height=dros_update->Height=dros_update->Bounds.height;
    x0=dros_update->X0=x=dros_update->Bounds.left+radius;
    y0=dros_update->Y0=dros_update->Bounds.top;
    y=y0-height;
    break;
    }
  }
  Update(dros_update);  
}
void CDrossel::Draw(BOOL ShowNode)
{
  if (ShowNode)
  {
  DrawNodes(ShowNode);
  }
  double xt,yt;
  bool condition=(showtext==0)||(anim==0) ;
  if ( condition )
  {
    auxTube((double)x0,(double)y0,xt,yt,(double)radius,angle,(double)height);
  //  auxTube((double)(x0+xt)/2.0,(double)(y0+yt)/2.0,xt,yt,(double)radius*faska,angle,(double)radius);
  //  return;
  } 
   GLdouble params[4];
   glGetDoublev(GL_CURRENT_COLOR,params); 
   if ( !condition )auxHalfTube((double)x0,(double)y0,xt,yt,(double)radius,angle,(double)height);
   TDist rad=2*radius;
   switch (angle)
   {
   case 0 : {auxHalfTube(x0+1.4*rad-0.5*radius,y0,xt,yt,radius*faska,angle,0.2*rad+radius); break;}
   case 90 : {auxHalfTube(x0,y0+1.4*rad-0.5*radius,xt,yt,radius*faska,angle,0.2*rad+radius); break;}
   case 180 : {auxHalfTube(x0-1.4*rad+0.5*radius,y0,xt,yt,radius*faska,angle,0.2*rad+radius);break;}
   case 270 : {auxHalfTube(x0,y0-1.4*rad+0.5*radius,xt,yt,radius*faska,angle,0.2*rad+radius);break;}
   }
   
   GLfloat amb[4],ambient[4] = {0.0,0.0, 0.0, 1};
     glGetFloatv(GL_LIGHT_MODEL_AMBIENT,amb);     
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
   if (anim==1) SetColorUb(ConnectWnd->sc.ColdColor);
   else SetColorUb(ConnectWnd->sc.HotColor);
   double r=radius*0.7;
   glNormal3d(0,0,1);
  switch (angle)
  {
  case 180 :
    {
    if ( !condition )
    {
      glBegin(GL_POLYGON);
      glVertex3d(x0,y-r,0.1);
      glVertex3d(x0-1*rad,y-r,0.1);
      glVertex3d(x0-1.3*rad,y-0.8*r,0.1);
      glVertex3d(x0-1.45*rad,y-0.4*r,0.1);
      glVertex3d(x0-1.45*rad,y+0.4*r,0.1);
      glVertex3d(x0-1.3*rad,y+0.8*r,0.1);
      glVertex3d(x0-1*rad,y+r,0.1);
      glVertex3d(x0,y+r,0.1);
      glEnd();
      glBegin(GL_POLYGON);
      glVertex3d(x,y-r,0.1);
      glVertex3d(x0-2*rad,y-r,0.1);
      glVertex3d(x0-1.7*rad,y-0.8*r,0.1);
      glVertex3d(x0-1.55*rad,y-0.4*r,0.1);
      glVertex3d(x0-1.55*rad,y+0.4*r,0.1);
      glVertex3d(x0-1.7*rad,y+0.8*r,0.1);
      glVertex3d(x0-2*rad,y+r,0.1);
      glVertex3d(x,y+r,0.1);
      glEnd();
    }
    glBegin(GL_QUADS);
    glVertex3d(x0-1.45*rad,y-0.4*r,0.1);
    glVertex3d(x0-1.55*rad,y-0.4*r,0.1);
    glVertex3d(x0-1.55*rad,y+0.4*r,0.1);
    glVertex3d(x0-1.45*rad,y+0.4*r,0.1);
    glColor3d(0.4,0.2,0.2);
    glVertex3d(x0-1.3*rad,y-radius*faska*1.2,0.05);
    glVertex3d(x0-1.7*rad,y-radius*faska*1.2,0.05);
    glVertex3d(x0-1.7*rad,y+radius*faska*1.2,0.05);
    glVertex3d(x0-1.3*rad,y+radius*faska*1.2,0.05);
    glEnd();
    break;
    }
  case 0:
    {
    if ( !condition )
    {
      glBegin(GL_POLYGON);
      glVertex3d(x0,y-r,0.1);
      glVertex3d(x0+1*rad,y-r,0.1);
      glVertex3d(x0+1.3*rad,y-0.8*r,0.1);
      glVertex3d(x0+1.45*rad,y-0.4*r,0.1);
      glVertex3d(x0+1.45*rad,y+0.4*r,0.1);
      glVertex3d(x0+1.3*rad,y+0.8*r,0.1);
      glVertex3d(x0+1*rad,y+r,0.1);
      glVertex3d(x0,y+r,0.1);
      glEnd();
      glBegin(GL_POLYGON);
      glVertex3d(x,y-r,0.1);
      glVertex3d(x0+2*rad,y-r,0.1);
      glVertex3d(x0+1.7*rad,y-0.8*r,0.1);
      glVertex3d(x0+1.55*rad,y-0.4*r,0.1);
      glVertex3d(x0+1.55*rad,y+0.4*r,0.1);
      glVertex3d(x0+1.7*rad,y+0.8*r,0.1);
      glVertex3d(x0+2*rad,y+r,0.1);
      glVertex3d(x,y+r,0.1);
      glEnd();
    }
    glBegin(GL_QUADS);
    glVertex3d(x0+1.45*rad,y-0.4*r,0.1);
    glVertex3d(x0+1.55*rad,y-0.4*r,0.1);
    glVertex3d(x0+1.55*rad,y+0.4*r,0.1);
    glVertex3d(x0+1.45*rad,y+0.4*r,0.1);
    glColor3d(0.4,0.2,0.2);
    glVertex3d(x0+1.3*rad,y-radius*faska*1.2,0.05);
    glVertex3d(x0+1.7*rad,y-radius*faska*1.2,0.05);
    glVertex3d(x0+1.7*rad,y+radius*faska*1.2,0.05);
    glVertex3d(x0+1.3*rad,y+radius*faska*1.2,0.05);
    glEnd();
    break;
    }
  case 270: 
    {
    if ( !condition )
    {
      glBegin(GL_POLYGON);
      glVertex3d(x0-r,y0,0.1);
      glVertex3d(x0-r,y0-1*rad,0.1);
      glVertex3d(x0-0.8*r,y0-1.3*rad,0.1);
      glVertex3d(x0-0.4*r,y0-1.45*rad,0.1);
      glVertex3d(x0+0.4*r,y0-1.45*rad,0.1);
      glVertex3d(x0+0.8*r,y0-1.3*rad,0.1);
      glVertex3d(x0+r,y0-1*rad,0.1);
      glVertex3d(x0+r,y0,0.1);
      glEnd();
      glBegin(GL_POLYGON);
      glVertex3d(x0-r,y,0.1);
      glVertex3d(x0-r,y0-2*rad,0.1);
      glVertex3d(x0-0.8*r,y0-1.7*rad,0.1);
      glVertex3d(x0-0.4*r,y0-1.55*rad,0.1);
      glVertex3d(x0+0.4*r,y0-1.55*rad,0.1);
      glVertex3d(x0+0.8*r,y0-1.7*rad,0.1);
      glVertex3d(x0+r,y0-2*rad,0.1);
      glVertex3d(x0+r,y,0.1);
      glEnd();
    }
    glBegin(GL_QUADS);
    glVertex3d(x0-0.4*r,y0-1.45*rad,0.1);
    glVertex3d(x0-0.4*r,y0-1.55*rad,0.1);
    glVertex3d(x0+0.4*r,y0-1.55*rad,0.1);
    glVertex3d(x0+0.4*r,y0-1.45*rad,0.1);
    glColor3d(0.4,0.2,0.2);
    glVertex3d(x0-radius*faska*1.2,y0-1.3*rad,0.05);
    glVertex3d(x0-radius*faska*1.2,y0-1.7*rad,0.05);
    glVertex3d(x0+radius*faska*1.2,y0-1.7*rad,0.05);
    glVertex3d(x0+radius*faska*1.2,y0-1.3*rad,0.05);
    glEnd();
    break;
    }
  case 90:
    {
    if ( !condition )
    {
      glBegin(GL_POLYGON);
      glVertex3d(x0-r,y0,0.1);
      glVertex3d(x0-r,y0+1*rad,0.1);
      glVertex3d(x0-0.8*r,y0+1.3*rad,0.1);
      glVertex3d(x0-0.4*r,y0+1.45*rad,0.1);
      glVertex3d(x0+0.4*r,y0+1.45*rad,0.1);
      glVertex3d(x0+0.8*r,y0+1.3*rad,0.1);
      glVertex3d(x0+r,y0+1*rad,0.1);
      glVertex3d(x0+r,y0,0.1);
      glEnd();
      glBegin(GL_POLYGON);
      glVertex3d(x0-r,y,0.1);
      glVertex3d(x0-r,y0+2*rad,0.1);
      glVertex3d(x0-0.8*r,y0+1.7*rad,0.1);
      glVertex3d(x0-0.4*r,y0+1.55*rad,0.1);
      glVertex3d(x0+0.4*r,y0+1.55*rad,0.1);
      glVertex3d(x0+0.8*r,y0+1.7*rad,0.1);
      glVertex3d(x0+r,y0+2*rad,0.1);
      glVertex3d(x0+r,y,0.1);
      glEnd();
    }
    glBegin(GL_QUADS);
    glVertex3d(x0-0.4*r,y0+1.45*rad,0.1);
    glVertex3d(x0-0.4*r,y0+1.55*rad,0.1);
    glVertex3d(x0+0.4*r,y0+1.55*rad,0.1);
    glVertex3d(x0+0.4*r,y0+1.45*rad,0.1);
    glColor3d(0.4,0.2,0.2);
    glVertex3d(x0-radius*faska*1.2,y0+1.3*rad,0.05);
    glVertex3d(x0-radius*faska*1.2,y0+1.7*rad,0.05);
    glVertex3d(x0+radius*faska*1.2,y0+1.7*rad,0.05);
    glVertex3d(x0+radius*faska*1.2,y0+1.3*rad,0.05);
    glEnd();
    break;
    }
  }
 glColor3dv(params);
 glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
}
bool CDrossel::HasText()
{
  return false;
}
//-------------------------------------------------------------------
CAirBattery::CAirBattery(ELEMENT_DATA *airbat_create,BOOL FromBounds)
{
  Focused=0;
  InitConnect();
  sw=NULL;
  position=0;
  if (FromBounds)UpdateFromBounds(airbat_create);
  else Update(airbat_create);
}
CAirBattery::CAirBattery(const CAirBattery &prev_airbat) //конструктор копий
{
  Focused=0;
  CopyConnect(&prev_airbat);
  sw=NULL;
  position=0;
  ELEMENT_DATA air;
  prev_airbat.GetData(&air);  
  Update(&air);
  
}
CAirBattery::~CAirBattery()
{
  if (sw!=NULL)delete sw;
  if (connected!=NULL)delete connected;
}
void CAirBattery::Update(ELEMENT_DATA *airbat_update)
{
  if (airbat_update->Radius<=0)return;
  if (airbat_update->Height<6*airbat_update->Radius)return;
  radius=airbat_update->Radius;
  x0=airbat_update->X0;
  y0=airbat_update->Y0;
  kol=airbat_update->Kol;
  altitude=airbat_update->Height;
  x=x0+altitude-6*radius;
  y=y0-altitude-6*radius;
  bound.left=x0-3*radius;
  bound.top=y0;
  bound.width=altitude;
  bound.height=altitude+6*radius;
  airbat_update->Bounds=bound;
  SWITCH_DATA s;
  s.X0=(TDist)((x0+x)/2.0);
  s.Y0=(TDist)((y0+y)/2.0);
  s.Z0=radius;
  s.Height=(TDist)(radius*2.5);
  s.Xt=airbat_update->Xt;
  s.Yt=airbat_update->Yt;
  if (sw==NULL) sw=new CSwitcher(&s);
  else sw->Update(&s);  
  strcpy(cd.TypeString,"Водовоздушный подогреватель");
  SetTypeParam(airbat_update,AIRBATTERY);
  CTempl::Update(airbat_update);
}
void CAirBattery::UpdateFromBounds(ELEMENT_DATA *airbat_update)
{
  if (airbat_update->Radius<=0)return;
  if (airbat_update->Height<6*airbat_update->Radius)return;
  airbat_update->X0=airbat_update->Bounds.left+3*airbat_update->Radius;
  airbat_update->Y0=airbat_update->Bounds.top;
  airbat_update->Height=airbat_update->Bounds.width;
  Update(airbat_update);  
  if (sw!=NULL)
  {
    RECTAN newpos;        //вместо ResetLocation
    newpos.left=x0-(5+1.25)*radius; //вместо ResetLocation
    newpos.top=y0-(5-1.25)*radius;  //вместо ResetLocation  
    sw->MoveText(&newpos);    //вместо ResetLocation
  }
}
void CAirBattery::GetData(ELEMENT_DATA * airbat_data)const
{
  airbat_data->X0=x0;
  airbat_data->Y0=y0;
  airbat_data->Radius=radius;
  airbat_data->Kol=kol;
  airbat_data->Height=altitude;
  if (sw!=NULL)
  {
    airbat_data->Xt=sw->Xt;
    airbat_data->Yt=sw->Yt;
  }
  CTempl::GetData(airbat_data);   
}
void CAirBattery::Draw(BOOL ShowNode)
{
  DrawNodes(ShowNode);
  auxAirBattery(x0,y0,radius,kol,altitude);
  sw->Draw();
}
void CAirBattery::Animate()
{
  if (Focused) DrawBound(&bound);
  if (sw->ON()&&(!groupOn_mirror)) 
  {
    if (ConnectWnd!=NULL)((CSheme*)ConnectWnd)->Action((WPARAM)group.group,AIRBAT_OPENED);
    groupOn_mirror=true;
  }
  if (!sw->ON()&&(groupOn_mirror)) 
  {
    if (ConnectWnd!=NULL)((CSheme*)ConnectWnd)->Action((WPARAM)group.group,AIRBAT_CLOSED);
    groupOn_mirror=false;     
  }
  double al=altitude/2-radius;
  glPushMatrix();
  glNormal3d(0,0,1);
  glTranslated((x0+x)/2.0,(y0+y)/2.0,1.5*radius);
  glRotated(20*position,0,0,1); 
  for (int t=0;t<4;t++)
  {
   glRotated(90,0,0,1); 
   glBegin(GL_QUADS);
    glVertex3d(0,0,0);
    glVertex3d(al,0,0);
    glVertex3d(al-radius,radius,radius);
    glVertex3d(0,0,radius);
   glEnd();
  }
  glPopMatrix();
  if (group.groupOn) position++;
  if (position>17)position=0;
  sw->Animate();
}
int CAirBattery::GetNodeKol() const
{
  return 2;
}
CShemaNode CAirBattery::GetNode(int number) const
{
  CShemaNode node;
  if (number==0)
  {
    node.X=x0;
    node.Y=y0;
    node.Radius=radius;
    node.Angle=90;
  }
  else 
  {
    node.X=x;
    node.Y=y;
    node.Radius=radius;
    node.Angle=270;
  }
  return node;
}
void CAirBattery::ConnectionCreate(CTemplWnd * newConnectWnd)
{
  CTempl::ConnectionCreate(newConnectWnd);
  if (sw!=NULL) sw->ConnectionCreate(newConnectWnd);  
}
//-------------------------------------------------------------------
CVodoVodo::CVodoVodo(ELEMENT_DATA*vodobat_create,BOOL FromBounds)
{
  Focused=0;
  InitConnect();
  if (FromBounds)UpdateFromBounds(vodobat_create);
  else Update(vodobat_create);
}
CVodoVodo::CVodoVodo(const CVodoVodo &prev_vodobat) //конструктор копий
{
  Focused=0;
  CopyConnect(&prev_vodobat);
  ELEMENT_DATA vodo;
  prev_vodobat.GetData(&vodo);  
  Update(&vodo);
}
CVodoVodo:: ~CVodoVodo()
{
  if (connected!=NULL)delete connected;
}
void CVodoVodo::Update(ELEMENT_DATA *airbat_update)
{
  if (airbat_update->Radius<=0)return;
  if (airbat_update->Radius1>2*airbat_update->Radius)return;
  if (airbat_update->Height<15*airbat_update->Radius)return;
  x0=airbat_update->X0;
  y0=airbat_update->Y0;
  radius=airbat_update->Radius;
  radius1=airbat_update->Radius1;
  altitude=airbat_update->Height;
  //
  x=x0;
  y=y0-10*radius;
  x1=x0+4*radius;
  y1=y0-3*radius;
  x2=x1;
  y2=y1-4*radius;
  //
  bound.left=x0-altitude+4*radius;
  bound.top=y0;
  bound.height=10*radius;
  bound.width=altitude;
  airbat_update->Bounds=bound;
  strcpy(cd.TypeString,"Водоводяной обогреватель");
  SetTypeParam(airbat_update,VODOBATTERY);
  CTempl::Update(airbat_update);
}
void CVodoVodo::UpdateFromBounds(ELEMENT_DATA *airbat_update)
{
  airbat_update->Height=airbat_update->Bounds.width;
  airbat_update->X0=airbat_update->Bounds.left+airbat_update->Height-4*airbat_update->Radius;
  airbat_update->Y0=airbat_update->Bounds.top;
  airbat_update->Bounds.height=10*airbat_update->Radius;
  Update(airbat_update);
}
void CVodoVodo::GetData(ELEMENT_DATA *airbat_data)const
{
  airbat_data->X0=x0;
  airbat_data->Y0=y0;
  airbat_data->Radius=radius;
  airbat_data->Radius1=radius1;
  airbat_data->Height=altitude;
  CTempl::GetData(airbat_data);   
}
void CVodoVodo::Draw(BOOL ShowNode)
{
  DrawNodes(ShowNode);
  auxWaterBattery(x0,y0,radius,altitude,radius1);
}
void CVodoVodo::Animate()
{
  if (Focused) DrawBound(&bound);
}
int CVodoVodo::GetNodeKol() const
{
  return 4;
}
CShemaNode CVodoVodo::GetNode(int number) const
{
  CShemaNode node;
  switch (number)
  {
  case 0:
    {
    node.X=x0;
    node.Y=y0;
    node.Radius=radius;
    node.Angle=90;
    break;
    }
  case 1:
    {
    node.X=x;
    node.Y=y;
    node.Radius=radius;
    node.Angle=270;
    break;
    }
  case 2:
    {
    node.X=x1;
    node.Y=y1;
    node.Radius=radius;
    node.Angle=0;
    break;
    }
  default:
    {
    node.X=x2;
    node.Y=y2;
    node.Radius=radius;
    node.Angle=0;
    break;
    }
  }
  return node;
}
/*bool CVodoVodo::PointInRgn(CPoint point,double X_move,double Y_move,float depth,CRect rectclient,CWnd* wnd)
{
  CRgn Reg;
  CRect rectan;
  CPoint p;
  x_move=X_move;
  y_move=Y_move;
  dep=depth;
  rectClient=rectclient;
    p=getpoint(x0-0.92*altitude-3*radius,y0);
    rectan.left=p.x;
    rectan.top=p.y;
    p=getpoint(x0+0.05*altitude+3*radius,y);
    rectan.right=p.x;
    rectan.bottom=p.y;    
  Reg.CreateRectRgnIndirect(&rectan);
  if(Reg.PtInRegion(point))
  {
    CBrush brA;
    VERIFY(brA.CreateSolidBrush( RGB(0,255,0) ));  
    CClientDC pDC(wnd);
    VERIFY(pDC.FrameRgn( &Reg, &brA, 2, 2 ));
    return true;
  }
  else return false;
}*/
//-------------------------------------------------------------------
CSoplo::CSoplo(ELEMENT_DATA *soplo_create,BOOL FromBounds)
{
  Focused=0;
  InitConnect();
  ballT=NULL;
  ballT90=NULL;
  if (FromBounds)UpdateFromBounds(soplo_create);
  else Update(soplo_create);
}
CSoplo::CSoplo(const CSoplo &prev_soplo) //конструктор копий
{
  Focused=0;
  ballT=NULL;
  ballT90=NULL;
  CopyConnect(&prev_soplo);
  ELEMENT_DATA t;
  prev_soplo.GetData(&t);
  Update(&t);
}
CSoplo::~CSoplo()
{
  if (ballT!=NULL)delete ballT;
  ballT=NULL;
  if (ballT90!=NULL)delete ballT90;
  ballT90=NULL;
  if (connected!=NULL)delete connected;
}
void CSoplo::Update(ELEMENT_DATA *soplo_update)
{
  if (soplo_update->Radius<=0)return;
  x0=soplo_update->X0;
  y0=soplo_update->Y0;
  radius=soplo_update->Radius;
  height=15*radius;
  if (ballT!=NULL) delete ballT;
  ballT=NULL;
  ball_kol=5;//0.35*15;   
  ballT=new CBaloonTube*[ball_kol];
  for (int rt=0;rt<ball_kol;rt++)
  {
  ballT[rt]=new CBaloonTube(radius,height,height*rt/ball_kol);
  }

  if (ballT90!=NULL) delete ballT90;
  ballT90=NULL;
  ball_kol90=1;   
  ballT90=new CBaloonTube*[ball_kol90];
  for (int rt=0;rt<ball_kol90;rt++)
  {
  ballT90[rt]=new CBaloonTube(radius,5*radius,5*radius*rt/ball_kol90);
  }

  x=x0+15*radius;
  y=y0;
  y1=y0-5*radius;
  x1=x0+5*radius;

  bound.left=x0;
  bound.top=y0+radius;
  bound.height=6*radius;
  bound.width=15*radius;
  soplo_update->Bounds=bound;
  strcpy(cd.TypeString,"Сопло");
  SetTypeParam(soplo_update,SOPLO);
  CTempl::Update(soplo_update);
}
void CSoplo::UpdateFromBounds(ELEMENT_DATA *soplo_update)
{
  soplo_update->X0=soplo_update->Bounds.left;
  soplo_update->Y0=soplo_update->Bounds.top-soplo_update->Radius;
  soplo_update->Bounds.height=6*soplo_update->Radius;
  soplo_update->Bounds.width=15*soplo_update->Radius;
  Update(soplo_update); 
}
void CSoplo::GetData(ELEMENT_DATA *soplo_data)const
{
  soplo_data->X0=x0;
  soplo_data->Y0=y0;
  soplo_data->Radius=radius;
  CTempl::GetData(soplo_data);      
}
void CSoplo::Draw(BOOL ShowNode)
{
  DrawNodes(ShowNode);
  auxSoplo(ConnectWnd->sc.ColdColor,ConnectWnd->sc.HotColor,x0,y0,radius);
  //auxBattery(x,y,radius,8,height,radius);
}
void CSoplo::Animate()
{
  if (Focused) DrawBound(&bound);
  if (!group.groupOn)return;
  if (ConnectWnd!=NULL)
  {
    if (!ConnectWnd->worksp.ShowBalloons)return;
  }
  glPushMatrix();
  glTranslated(0,0,0.1);
  RotateTo0(x0,y0);
  for (int rt=0;rt<ball_kol;rt++)
  {
    ballT[rt]->Animate();
  }
  glPopMatrix();
  glPushMatrix();
  glTranslated(0,0,0.1);
  RotateTo90(x1,y1);  
  for (int rt=0;rt<ball_kol90;rt++)
  {
    ballT90[rt]->Animate();
  }
  glPopMatrix();
}
int CSoplo::GetNodeKol() const
{
  return 3;
}
CShemaNode CSoplo::GetNode(int number) const
{
  CShemaNode node;
  switch (number)
  {
  case 0:
    {
    node.X=x0;
    node.Y=y0;
    node.Radius=radius;
    node.Angle=180;
    break;
    }
  case 1:
    {
    node.X=x;
    node.Y=y;
    node.Radius=radius;
    node.Angle=0;
    break;
    }
  default:
    {
    node.X=x1;
    node.Y=y1;
    node.Radius=radius;
    node.Angle=270;
    break;
    }
  }
  return node;
}
//-------------------------------------------------------------------
CSwitcher::CSwitcher(SWITCH_DATA *switch_create)
{
  Focused=0;
  on=1;
  Update(switch_create);
}
CSwitcher::~CSwitcher()
{
}
void CSwitcher::Update(SWITCH_DATA *switch_update)
{
  if (switch_update->Height<=0)return;
  x0=switch_update->X0;
  y0=switch_update->Y0;
  z0=switch_update->Z0;
  Xt=switch_update->Xt;
  Yt=switch_update->Yt;
  height=(TDist)(switch_update->Height/2.0);
  x=Xt+height;
  y=Yt-height;  
  GetBound();
}
RECTAN CSwitcher::GetBound()
{
  bound.left=Xt;
  bound.top=Yt;
  bound.height=bound.width=2*height;
  return bound;
}
void CSwitcher::GetData(SWITCH_DATA *switch_data)const
{
  switch_data->X0=x0;
  switch_data->Y0=y0;
  switch_data->Z0=z0;
  switch_data->Xt=Xt;
  switch_data->Yt=Yt;
  switch_data->Height=2*height;
}
void CSwitcher::Draw()
{
  GLdouble params[4];
  glGetDoublev(GL_CURRENT_COLOR,params); 
  glColor3d(0.8,0.8,0.8);
  glNormal3d(0,0,1);
  glBegin(GL_LINE_STRIP);
    glVertex3d(x0,y0,z0);
    glVertex3d(x,y0,z0);
    glVertex3d(x,y,z0);
  glEnd();
  glBegin(GL_QUADS);
    glVertex3d(x-height,y+height,z0);
    glVertex3d(x+height,y+height,z0);
    glVertex3d(x+height,y-height,z0);
    glVertex3d(x-height,y-height,z0);
  glEnd();
  glColor4dv(params); 
}

void CSwitcher::Animate()
{
  GLfloat amb[4],ambient[4] = {0.7,0.7, 0.7, 1};
  glGetFloatv(GL_LIGHT_MODEL_AMBIENT,amb);    
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  glNormal3d(0,0,1);
  double h1=0.7*height;
  GLdouble params[4];
  glGetDoublev(GL_CURRENT_COLOR,params); 
  if (on)
  {
  glColor3d(0,0,0);
  glBegin(GL_LINE_STRIP);
    //glVertex3d(x-h1,y+h1,z0+0.1);
    glVertex3d(x+h1,y+h1,z0+0.1);
    glVertex3d(x+h1,y-h1,z0+0.1);
    glVertex3d(x-h1,y-h1,z0+0.1);
  glEnd();
  }
  if (on) glColor3d(0,1,0);
  else glColor3d(1,0,0);
  glBegin(GL_QUADS);
    glVertex3d(x-h1,y+h1,z0+0.1);
    glVertex3d(x+h1,y+h1,z0+0.1);
    glVertex3d(x+h1,y-h1,z0+0.1);
    glVertex3d(x-h1,y-h1,z0+0.1);
  glEnd();
  glColor3dv(params);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
}
bool CSwitcher::Click()
{
  on=!on;
  return on;
}
bool CSwitcher::ON()
{
  return on;
}
void CSwitcher::ResetLocation()
{
  Xt=x0-10*height;
  Yt=y0+10*height;
  bound=GetBound(); 
}
void CSwitcher::MoveText(RECTAN * newPos)
{
  Xt=newPos->left;
  Yt=newPos->top;
  x=Xt+height;
  y=Yt-height;  
  bound=GetBound(); 
}
//-------------------------------------------------------------------
CDevice::CDevice(DEVICE_DATA *dev_create)
{
  Focused=0;
  on=1;
  Update(dev_create);
}
CDevice::~CDevice()
{
}
void CDevice::Update(DEVICE_DATA *dev_update)
{
  if (dev_update->Radius<=0)return;
  x0=dev_update->X0;
  y0=dev_update->Y0;
  radius=dev_update->Radius;
  rad=dev_update->Rad;
  height=dev_update->Height;
  angle=dev_update->Angle;
  Xt=dev_update->Xt;
  Yt=dev_update->Yt;
  strcpy(text,dev_update->Text);
  switch (angle)
  {
  case 0:
    {
      x=x0+height-rad;
      y=y0;
      break;
    }
  case 90:
    {
      x=x0;
      y=y0+height-rad;
      break;
    }
  case 180:
    {
      x=x0-height+rad;
      y=y0;
      break;
    }
  case 270:
    {
      x=x0;
      y=y0-height+rad;
      break;
    }
  default:
    {
      return;
    }   
  }
  CPribor::Update();
}
void CDevice::GetData(DEVICE_DATA *dev_data)const
{
  dev_data->X0=x0;
  dev_data->Y0=y0;
  dev_data->Xt=Xt;
  dev_data->Yt=Yt;  
  dev_data->Radius=radius;
  dev_data->Rad=rad;
  dev_data->Angle=angle;
  dev_data->Height=height;
  strcpy(dev_data->Text,text);
  dev_data->Bounds=bound;
}
void CDevice::Draw()
{
  //if (!on)return;
  double ras=radius*0.5;
  GLUquadricObj *quadObj;
  quadObj = gluNewQuadric(); // создаем новый объект
  gluQuadricDrawStyle(quadObj, GLU_FILL);
  glPushMatrix();
  glTranslated(x,y,ras);
  GLdouble params[4];
  glGetDoublev(GL_CURRENT_COLOR,params); 
  gluDisk(quadObj,0,0.8*rad,30,1);
  glColor3f(0.0f,0.0f,0.0f);
  gluDisk(quadObj,0.8*rad,rad,30,1);
  double ras1=0.4*rad,ras2=0.6*rad,zorder=ras+0.1;
  glNormal3d(0,0,1);
  glBegin(GL_LINES);
  glVertex3f(-ras1,0.0f,zorder);
  glVertex3f(-ras2,0.0f,zorder);
  glVertex3f(ras1,0.0f,zorder);
  glVertex3f(ras2,0.0f,zorder);
  glVertex3f(0.0f,-ras1,zorder);
  glVertex3f(0.0f,0.0f-ras2,zorder);
  glVertex3f(0.0f,0.0f+ras1,zorder);
  glVertex3f(0.0f,0.0f+ras2,zorder);
  glEnd();
  glBegin(GL_TRIANGLES);
  glVertex3f(ras2/2.0,ras2/2.0,zorder);
  glVertex3f(-ras1/2.0,0.0f,zorder);
  glVertex3f(0.0f,-ras1/2.0,zorder);
  glEnd();
  glTranslated(0,0,-ras);
  switch (angle)
  {
  case 0:{glRotated(-90,0,1,0);break;}
  case 90:{glRotated(90,1,0,0);break;}
  case 180:{glRotated(90,0,1,0);break;}
  case 270:{glRotated(-90,1,0,0);break;}
  } 
  double move=(height-radius-faskalen*ras)/2.0;
  gluCylinder(quadObj,ras,ras,move,_slices,_stacks);
  glTranslated(0,0,move);
  glColor3dv(params);
  gluCylinder(quadObj,ras*faska,ras*faska,faskalen*ras,_slices,_stacks);
  gluCylinder(quadObj,ras,ras,height-rad-move,_slices,_stacks);
  glPopMatrix();
  gluDeleteQuadric(quadObj);  
  CPribor::Draw();  
}
bool CDevice::Click()
{
  on=!on;
  return on;  
}
void CDevice::Show(CTemplWnd * t_wnd,bool show) 
{
  ConnectWnd=t_wnd;
  on=show;
}
bool CDevice::ON()
{
  return on;
}
//-------------------------------------------------------------------
CFlowmeter::CFlowmeter(FLOWMETER_DATA *flow_create)
{
  Focused=0;
  on=1;
  Update(flow_create);
}
CFlowmeter::~CFlowmeter()
{
}
void CFlowmeter::Update(FLOWMETER_DATA *flow_update)
{
  if (flow_update->Radius<=0)return;
  x=x0=flow_update->X0;
  y=y0=flow_update->Y0;
  radius=flow_update->Radius;
  rad=flow_update->Rad;
  angle=flow_update->Angle;
  strcpy(text,flow_update->Text);
  Xt=flow_update->Xt;
  Yt=flow_update->Yt;
  switch (angle)
  {
  case 0:
    {
      /*bound.left=x0;
      bound.top=y0+radius;
      bound.height=2*rad;
      bound.width=height;
      */
      break;
    }
  case 90:
    {
      /*
      bound.left=x0-rad;
      bound.top=y0+height;
      bound.height=height;
      bound.width=2*rad;
      */
      break;
    }
  case 180:
    {
      /*
      bound.left=x0-height;
      bound.top=y0+rad;
      bound.height=2*rad;
      bound.width=height;
      */
      break;
    }
  case 270:
    {
      /*
      bound.left=x0-rad;
      bound.top=y0;
      bound.height=height;
      bound.width=2*rad;
      */
      break;
    }
  default:
    {
      return;
    }   
  }
  CPribor::Update();
}
void CFlowmeter::GetData(FLOWMETER_DATA *flow_data)const
{
  flow_data->X0=x0;
  flow_data->Y0=y0;
  flow_data->Xt=Xt;
  flow_data->Yt=Yt;
  flow_data->Radius=radius;
  flow_data->Rad=rad;
  flow_data->Angle=angle;
  strcpy(flow_data->Text,text);
  flow_data->Bounds=bound;
}
void CFlowmeter::Draw()
{
  //if (!on)return;
  double ras=radius*0.5;
  GLdouble params[4];
  glGetDoublev(GL_CURRENT_COLOR,params); 
  glPushMatrix();
  glTranslated(x,y,radius);
  GLUquadricObj *quadObj;
  quadObj = gluNewQuadric(); // создаем новый объект
  gluQuadricDrawStyle(quadObj, GLU_FILL);
  gluDisk(quadObj,0,0.8*rad,30,1);
  glColor3f(0.0f,0.0f,0.0f);
  gluDisk(quadObj,0.8*rad,rad,30,1);
  //_стрелки и шкала
  double ras1=0.2*rad,ras2=0.3*rad,zorder=ras+0.1,ras3=0.1*rad,ras4=0.4*rad;
  glNormal3d(0,0,1);
  glBegin(GL_QUADS);  
  glVertex3f(-ras1,ras2,zorder);
  glVertex3f(-ras4,ras2,zorder);
  glVertex3f(-ras4,0.0f,zorder);
  glVertex3f(-ras1,0.0f,zorder);
  glVertex3f(ras3,ras2,zorder);
  glVertex3f(-ras3,ras2,zorder);
  glVertex3f(-ras3,0.0f,zorder);
  glVertex3f(ras3,0.0f,zorder);
  glVertex3f(ras1,ras2,zorder);
  glVertex3f(ras4,ras2,zorder);
  glVertex3f(ras4,0.0f,zorder);
  glVertex3f(ras1,0.0f,zorder);
  glEnd();

  glTranslated(0.0f,-0.4*rad,0.0f);
  glBegin(GL_LINES);
  glVertex3f(-ras1,0.0f,zorder);
  glVertex3f(-ras2,0.0f,zorder);
  glVertex3f(ras1,0.0f,zorder);
  glVertex3f(ras2,0.0f,zorder);
  glVertex3f(0.0f,-ras1,zorder);
  glVertex3f(0.0f,0.0f-ras2,zorder);
  glVertex3f(0.0f,0.0f+ras1,zorder);
  glVertex3f(0.0f,0.0f+ras2,zorder);
  glEnd();
  glBegin(GL_TRIANGLES);
  glVertex3f(ras2/2.0,ras2/2.0,zorder);
  glVertex3f(-ras1/2.0,0.0f,zorder);
  glVertex3f(0.0f,-ras1/2.0,zorder);
  glEnd();
  //конец _стрелки и шкала
  glColor3dv(params);
  glPopMatrix();
  gluDeleteQuadric(quadObj);  
  SCALE_FONT color={0.0f,0.0f,0.0f};
  CPribor::Draw();    
}
bool CFlowmeter::Click()
{
  on=!on;
  return on;  
}
void CFlowmeter::Show(CTemplWnd * t_wnd,bool show) 
{
  ConnectWnd=t_wnd;
  on=show;
}

bool CFlowmeter::ON()
{
  return on;
}
void CFlowmeter::ResetLocation()
{
  double XP;
  switch (angle)
  {
  case 0: {XP=x+2*radius; break;}
  case 90: {XP=x; break;}
  case 180: {XP=x-2*radius; break;}
  case 270: {XP=x; break;}
  }
  if (ConnectWnd!=NULL)
  {
    double x1=XP;
    double y1=y;
    bound=GetFontRect(ConnectWnd->gmf,ConnectWnd->font_data,6*radius,angle,x1,y1,text);         
    Xt=x1;
    Yt=y1;
  }
}
//-------------------------------------------------------------------
CHeatCounter::CHeatCounter(FLOWMETER_DATA *flow_create)
 :CFlowmeter(flow_create)
{
}
void CHeatCounter::Draw()
{
  //CFlowmeter::Draw();
  //Прорисовка как и расходометра
  double ras=radius*0.5;
  GLdouble params[4];
  glGetDoublev(GL_CURRENT_COLOR,params); 
  glPushMatrix();
  glTranslated(x,y,radius);
  GLUquadricObj *quadObj;
  quadObj = gluNewQuadric(); // создаем новый объект
  gluQuadricDrawStyle(quadObj, GLU_FILL);
  gluDisk(quadObj,0,0.8*rad,30,1);
  glColor3f(0.0f,0.0f,0.0f);
  gluDisk(quadObj,0.8*rad,rad,30,1);
  //_стрелки и шкала
  double ras1=0.2*rad,ras2=0.3*rad,zorder=ras+0.1,ras3=0.1*rad,ras4=0.4*rad;
  glNormal3d(0,0,1);
  glBegin(GL_QUADS);  
  glVertex3f(-ras1,ras2,zorder);
  glVertex3f(-ras4,ras2,zorder);
  glVertex3f(-ras4,0.0f,zorder);
  glVertex3f(-ras1,0.0f,zorder);
  glVertex3f(ras3,ras2,zorder);
  glVertex3f(-ras3,ras2,zorder);
  glVertex3f(-ras3,0.0f,zorder);
  glVertex3f(ras3,0.0f,zorder);
  glVertex3f(ras1,ras2,zorder);
  glVertex3f(ras4,ras2,zorder);
  glVertex3f(ras4,0.0f,zorder);
  glVertex3f(ras1,0.0f,zorder);
  glEnd();

  glTranslated(0.0f,-0.4*rad,0.0f);
  glBegin(GL_LINES);
  glVertex3f(-ras1,0.0f,zorder);
  glVertex3f(-ras2,0.0f,zorder);
  glVertex3f(ras1,0.0f,zorder);
  glVertex3f(ras2,0.0f,zorder);
  glVertex3f(0.0f,-ras1,zorder);
  glVertex3f(0.0f,0.0f-ras2,zorder);
  glVertex3f(0.0f,0.0f+ras1,zorder);
  glVertex3f(0.0f,0.0f+ras2,zorder);
  glEnd();
  glBegin(GL_TRIANGLES);
  glVertex3f(ras2/2.0,ras2/2.0,zorder);
  glVertex3f(-ras1/2.0,0.0f,zorder);
  glVertex3f(0.0f,-ras1/2.0,zorder);
  glEnd();
  //конец _стрелки и шкала
  glColor3dv(params);
  glPopMatrix();
  gluDeleteQuadric(quadObj);  
  SCALE_FONT color={0.0f,0.0f,0.0f};
  /*double XP;
  switch (angle)
  {
  case 0: {XP=x+2*radius; break;}
  case 90: {XP=x; break;}
  case 180: {XP=x-2*radius; break;}
  case 270: {XP=x; break;}
  }*/
  double x1,y1,x2,y2;
  x1=x0-rad;
  y1=y0+2*radius;
  x2=x0+rad;
  y2=y0+2*radius;
  glNormal3d(0,0,1);
  glBegin(GL_LINES);
  glVertex3d(x,y,0.0f);
  glVertex3d(x1,y1,0.0f);
  glVertex3d(x,y,0.0f);
  glVertex3d(x2,y2,0.0f);
  glEnd();
  auxTermometer(x1,y1+3*radius,0.5*radius,3*radius,0.25*radius);
  auxTermometer(x2,y2+3*radius,0.5*radius,3*radius,0.25*radius);
  CPribor::Draw();
}
//-------------------------------------------------------------------
CTermo::CTermo(TERMO_DATA *termo_create)
{
  Focused=0;
  on=1;
  Update(termo_create);
}
CTermo::~CTermo()
{
}
void CTermo::Update(TERMO_DATA *termo_update)
{
  if (termo_update->Radius<=0)return;
  x0=termo_update->X0;
  y0=termo_update->Y0;
  radius=termo_update->Radius;
  rad=termo_update->Rad/2.0;
  height=termo_update->Height;
  width=termo_update->Width/2.0;
  angle=termo_update->Angle;
  strcpy(text,termo_update->Text);
  Xt=termo_update->Xt;
  Yt=termo_update->Yt;
  radius1=radius/4.0;
  switch (angle)
  {
  case 0:
    {
      x=x0+height;
      y=y0+radius1*4+width;
      /*bound.left=x0;
      bound.top=y0+radius;
      bound.height=2*rad;
      bound.width=height;
      */
      break;
    }
  case 90:
    {
      x=x0;
      y=y0+height+width;
      /*
      
        .left=x0-rad;
      bound.top=y0+height;
      bound.height=height;
      bound.width=2*rad;
      */
      break;
    }
  case 180:
    {
      x=x0-height;
      y=y0+radius1*4+width;
      /*
      bound.left=x0-height;
      bound.top=y0+rad;
      bound.height=2*rad;
      bound.width=height;
      */
      break;
    }
  default:
    {
      return;
    }   
  }
  CPribor::Update();  
}
void CTermo::GetData(TERMO_DATA *termo_data)const
{
  termo_data->X0=x0;
  termo_data->Y0=y0;
  termo_data->Xt=Xt;
  termo_data->Yt=Yt;
  termo_data->Radius=radius;
  termo_data->Rad=rad*2;
  termo_data->Angle=angle;
  termo_data->Height=height;  
  termo_data->Width=2*width;
  strcpy(termo_data->Text,text);
  termo_data->Bounds=bound; 
}
void CTermo::Draw()
{
  //if (!on)return;
  if (angle==270)return;
  GLUquadricObj *quadObj;
  quadObj = gluNewQuadric(); // создаем новый объект
  gluQuadricDrawStyle(quadObj, GLU_FILL);
  auxTermometer(x,y,rad,width,radius1);
  double x2,y2;
  switch (angle)
  {
  case 0:
    {
    int an=270;
    auxTurn(x,y-width,x2,y2,radius1,an,1,5*radius1);
    glPushMatrix();
    glTranslated(x2,y2,0.0f);
    glRotated(-90,0,1,0);
    gluCylinder(quadObj,radius1,radius1,x2-x0,_slices,_stacks);
    glPopMatrix();
    break;
    }
  case 90:
    {
    glPushMatrix();
    glTranslated(x,y-width,0.0f);
    glRotated(90,1,0,0);
    gluCylinder(quadObj,radius1,radius1,y-width-y0,_slices,_stacks);
    glPopMatrix();
    break;
    }
  case 180:
    {
    int an=270;
    auxTurn(x,y-width,x2,y2,radius1,an,0,5*radius1);
    glPushMatrix();
    glTranslated(x2,y2,0.0f);
    glRotated(90,0,1,0);
    gluCylinder(quadObj,radius1,radius1,x0-x2,_slices,_stacks);
    glPopMatrix();
    break;
    }
  }
  CPribor::Draw();    
}
bool CTermo::Click()
{
  on=!on;
  return on;  
}
void CTermo::Show(CTemplWnd * t_wnd,bool show) 
{
  ConnectWnd=t_wnd;
  on=show;
}
bool CTermo::ON()
{
  return on;
}
//-------------------------------------------------------------------
CConnector::CConnector(ELEMENT_DATA *conn_create,BOOL FromBounds)
{
  Focused=0;
  dev=NULL;
  InitConnect();
  if (FromBounds)UpdateFromBounds(conn_create);
  else Update(conn_create);
}
CConnector::CConnector(const CConnector &prev_conn) //конструктор копий 
{
  dev=NULL;
  Focused=0;
  CopyConnect(&prev_conn);
  ELEMENT_DATA t;
  prev_conn.GetData(&t);
  Update(&t);
}
CConnector::~CConnector ()
{
  if (connected!=NULL)delete connected;
  if (dev!=NULL)delete dev;
}
void CConnector::GetData(ELEMENT_DATA *conn_data)const
{
  conn_data->X0=x0;
  conn_data->Y0=y0;
  conn_data->Radius=radius;
  conn_data->Angle=angle;
  conn_data->Height=height;
  conn_data->animate=anim;
  strcpy(conn_data->Text,text); 
  if (dev!=NULL){conn_data->Xt=dev->Xt;conn_data->Yt=dev->Yt;}
  CTempl::GetData(conn_data);     
}
void CConnector::Update(ELEMENT_DATA *conn_update)
{
  if (conn_update->Radius<0)return;
  x0=conn_update->X0;
  y0=conn_update->Y0;
  radius=conn_update->Radius;
  angle=conn_update->Angle;
  height=conn_update->Height;
  strcpy(text,conn_update->Text);
  DEVICE_DATA dd;
  dd.Xt=conn_update->Xt;
  dd.Yt=conn_update->Yt;
  dd.Radius=radius;
  dd.Rad=2*dd.Radius;
  dd.Height=2*radius+2*dd.Rad;      
  switch (angle)
  {
  case 0 : 
    {
      y=y0;
      x=x0+height;
      bound.left=x0;
      bound.top=y0+2*radius;
      bound.height=4*radius;
      bound.width=height;
  
      dd.X0=x-0.4*height;
      dd.Y0=y;
      dd.Angle=90;
      break;  
    }
  case 90 : 
    {
      x=x0;
      y=y0+height;
      bound.left=x0-2*radius;
      bound.top=y;
      bound.height=height;
      bound.width=4*radius;
      
      dd.X0=x;
      dd.Y0=y-0.4*height;     
      dd.Angle=180;     
      break;
    }
  case 180 : 
    {
      y=y0;
      x=x0-height;
      bound.left=x;
      bound.top=y0+2*radius;
      bound.height=4*radius;
      bound.width=height;
  
      dd.X0=x0-0.4*height;
      dd.Y0=y;
      dd.Angle=90;
      break;
    }
  case 270 : 
    {
      x=x0;
      y=y0-height;
      bound.left=x0-2*radius;
      bound.top=y0;
      bound.height=height;
      bound.width=4*radius;
      
      dd.X0=x;
      dd.Y0=y0-0.4*height;      
      dd.Angle=0;     
      break;
    }
  }
  conn_update->Bounds=bound;
  anim=conn_update->animate;
  strcpy(dd.Text,text);
  if (dev==NULL) 
  {
    dev=new CDevice(&dd);
  }
  else dev->Update(&dd);  
  if (dev!=NULL) dev->ShowText(showtext);
  if (anim) dev->Show(ConnectWnd);
  else dev->Show(ConnectWnd,0);
  strcpy(cd.TypeString,"Камера");
  SetTypeParam(conn_update,CONNECTOR);
  CTempl::Update(conn_update);
} 
void CConnector::UpdateFromBounds(ELEMENT_DATA *conn_update)
{
  switch (conn_update->Angle)
  {
  case 0 : 
    {
      conn_update->X0=conn_update->Bounds.left;
      conn_update->Y0=conn_update->Bounds.top-2*conn_update->Radius;
      conn_update->Bounds.height=4*conn_update->Radius;
      break;
    }
  case 90 : 
    {
      conn_update->X0=conn_update->Bounds.left+2*conn_update->Radius;
      conn_update->Y0=conn_update->Bounds.top-conn_update->Bounds.height;
      conn_update->Bounds.width=4*conn_update->Radius;
      break;
    }
  case 180 : 
    {
      conn_update->X0=conn_update->Bounds.left+conn_update->Bounds.width;
      conn_update->Y0=conn_update->Bounds.top-2*conn_update->Radius;
      conn_update->Bounds.height=4*conn_update->Radius;
      break;
    }
  case 270 : 
    {
      conn_update->X0=conn_update->Bounds.left+2*conn_update->Radius;
      conn_update->Y0=conn_update->Bounds.top;
      conn_update->Bounds.width=4*conn_update->Radius;
      break;
    }   
  }
  Update(conn_update);
  if (dev!=NULL){dev->ResetLocation();conn_update->Xt=dev->Xt;conn_update->Yt=dev->Yt;dev->ShowText();} 
}
void CConnector::Draw(BOOL ShowNode)
{
  DrawNodes(ShowNode);
  auxConnector(x0,y0,radius,height,angle);  
  if ((dev!=NULL)&&(dev->ON())){dev->ShowText(showtext);dev->Draw();}
}
void CConnector::Animate()
{
  if (Focused) DrawBound(&bound);
}
int CConnector::GetNodeKol() const
{
  return 1;
}
CShemaNode CConnector::GetNode(int number) const
{
  CShemaNode node;
  node.X=x;
  node.Y=y;
  node.Radius=radius;
  node.Angle=angle;
  return node;
}
bool CConnector::HasText()
{
  if (anim!=0) return true;
  else return false;
}
bool CConnector::PointInTxtRgn(MOUSEXY *m)
{
  if (dev!=NULL)if (dev->ON()) return dev->PointInRgn(m);
  return false;
}
RECTAN CConnector::GetTxtBounds()
{
  if (dev!=NULL)if (dev->ON()) return dev->GetBound();
  return CTempl::GetTxtBounds();  
}
void CConnector::MoveText(RECTAN * newPos)
{
  if (dev!=NULL)if (dev->ON()) dev->MoveText(newPos); 
}
void CConnector::ConnectionCreate(CTemplWnd * newConnectWnd)
{
  CTempl::ConnectionCreate(newConnectWnd);
  if (dev!=NULL)if (dev->ON()) dev->ConnectionCreate(newConnectWnd);  
}
//-------------------------------------------------------------------
CKran::CKran(ELEMENT_DATA* kran_create,BOOL FromBounds)
:CVentil(NULL,FromBounds)
{
  Focused=0;
  InitConnect();
  sw=NULL;
  position=min_open;
  if (FromBounds)UpdateFromBounds(kran_create);
  else Update(kran_create); 
  ballT=NULL;
  ball_kol=(int)(1);    
  ballT=new CBaloonTube*[ball_kol];
  for (int rt=0;rt<ball_kol;rt++)
  {
  ballT[rt]=new CBaloonTube((TDist)radius,(TDist)7*radius,7*radius*rt/ball_kol);
  } 
}
CKran::CKran(const CKran &prev_kran) //конструктор копий
:CVentil(prev_kran)
{
  Focused=0;
  sw=NULL;
  position=min_open;
  CopyConnect(&prev_kran);
  ELEMENT_DATA kran;
  prev_kran.GetData(&kran); 
  Update(&kran);
  ballT=NULL;
  ball_kol=(int)(2);    
  ballT=new CBaloonTube*[ball_kol];
  for (int rt=0;rt<ball_kol;rt++)
  {
  ballT[rt]=new CBaloonTube((TDist)radius,(TDist)7*radius,7*radius*rt/ball_kol);
  } 
  
}
CKran::~CKran()
{
  if (ballT!=NULL) delete ballT;
}
void CKran::Update(ELEMENT_DATA* kran_update)
{
  char ty[100];
  strcpy(ty,"");
  if ( (strlen(kran_update->cd.TypeString)>1)&&(strlen(kran_update->cd.TypeString)<100) )strcpy(ty,kran_update->cd.TypeString);
  CVentil::Update(kran_update);
  strcpy(kran_update->cd.TypeString,ty);
  strcpy(cd.TypeString,"Кран");
  SetTypeParam(kran_update,KRAN);
  CTempl::Update(kran_update);
}
void CKran::UpdateFromBounds(ELEMENT_DATA* kran_update)
{
  CVentil::UpdateFromBounds(kran_update);
  Update(kran_update);
  CVentil::UpdateFromBounds(kran_update);
}
void CKran::Draw(BOOL ShowNode)
{
  CVentil::Draw(ShowNode);
  double x1,y1;
  int an=0;
  auxTurn(x,y,x1,y1,radius,an,1,3*radius);
}
void CKran::Animate()
{
  CVentil::Animate();
  if (!group.groupOn)return;
  if (ConnectWnd!=NULL)
  {
    if (!ConnectWnd->worksp.ShowBalloons)return;
  }
  if (opening!=0)
  {
    double x1=x+2*radius,y1=y-2*radius;
    glPushMatrix();
    glTranslated(0,0,0.1);
    RotateTo270(x1-radius/2.0,y1);
    for (int rt=0;rt<ball_kol;rt++)
    {
      if (ballT!=0) ballT[rt]->Animate();
      glTranslated(radius,0,radius/2.0);
      if (ballT!=0) ballT[rt]->Animate();
      glTranslated(-radius,0,-radius/2.0);
    }
    glPopMatrix();
    GLdouble params[4];
    glGetDoublev(GL_CURRENT_COLOR,params); 
    SetColorUb(ConnectWnd->sc.HotColor);
    glNormal3d(0,0,1);
    glBegin(GL_QUADS);
    glVertex3d(x1-radius,y1,0.0f);
    glVertex3d(x1+radius,y1,0.0f);
    glVertex3d(x1+2*radius,y1-8*radius,0.0f);
    glVertex3d(x1-2*radius,y1-8*radius,0.0f);
    glEnd();
    glColor4dv(params);
  }
}
int CKran::GetNodeKol() const
{
  return 1;
}
CShemaNode CKran::GetNode(int number) const
{
  CShemaNode node;
  node.X=x0;
  node.Y=y0;
  node.Radius=radius;
  node.Angle=180;
  return node;
}
//-------------------------------------------------------------------
CLabel::CLabel(ELEMENT_DATA *label_create,BOOL FromBounds)
{
  Focused=0;
  InitConnect();
  if (FromBounds)UpdateFromBounds(label_create);
  else Update(label_create);
}
CLabel::CLabel(const CLabel &prev_label) //конструктор копий
{
  Focused=0;
  CopyConnect(&prev_label);
  ELEMENT_DATA t;
  prev_label.GetData(&t);
  Update(&t);
}
CLabel::~CLabel()
{
  if (connected!=NULL)delete connected;
}
void CLabel::Update(ELEMENT_DATA *label_update)
{
  x0=label_update->X0;
  y0=label_update->Y0;
  front.R=label_update->Radius1;
  front.G=label_update->Xt;
  front.B=label_update->Yt;
  back.R=label_update->Opening;
  back.G=label_update->Pribor;
  back.B=label_update->Kol;
  anim=label_update->animate;
  //height=label_update->Height;
  strcpy(text,label_update->Text);  
  if (ConnectWnd!=NULL)
  {
    bound=GetPrintRect(ConnectWnd->gmf,ConnectWnd->font_data,x0,y0,text);         
  }
  label_update->Bounds=bound;
  strcpy(cd.TypeString,"Текстовая метка");
  SetTypeParam(label_update,LABEL);
  CTempl::Update(label_update);
}
void CLabel::UpdateFromBounds(ELEMENT_DATA *label_update)
{
  label_update->X0=label_update->Bounds.left;
  label_update->Y0=label_update->Bounds.top;
  Update(label_update); 
}
void CLabel::GetData(ELEMENT_DATA *label_data)const
{
  label_data->X0=x0;
  label_data->Y0=y0;
  strcpy(label_data->Text,text);
  label_data->Radius=1;
  //label_data->Height=height;
  label_data->Radius1=front.R;
  label_data->Xt=front.G;
  label_data->Yt=front.B;
  label_data->Opening=back.R;
  label_data->Pribor=back.G;
  label_data->Kol=back.B;   
  label_data->animate=anim;
  CTempl::GetData(label_data);      
}
void CLabel::Draw(BOOL ShowNode)
{
  if (ConnectWnd!=NULL)
  {
    bound=GetPrintRect(ConnectWnd->gmf,ConnectWnd->font_data,x0,y0,text);
    glPrint(ConnectWnd->gmf,ConnectWnd->font_data,ConnectWnd->base,&front,&back,anim,x0,y0,0.1,text);   // Печать текста на экране      
  }   
}
void CLabel::Animate()
{
  if (Focused) DrawBound(&bound);
}
//-------------------------------------------------------------------
CExtTermo::CExtTermo(ELEMENT_DATA *exterm_create,BOOL FromBounds)
{
  Focused=0;
  termo=NULL;
  InitConnect();
  if (FromBounds)UpdateFromBounds(exterm_create);
  else Update(exterm_create);
}
CExtTermo::CExtTermo(const CExtTermo &prev_exterm) //конструктор копий
{
  Focused=0;
  termo=NULL;
  CopyConnect(&prev_exterm);
  ELEMENT_DATA t;
  prev_exterm.GetData(&t);
  Update(&t);
}
CExtTermo::~CExtTermo()
{
  if (connected!=NULL)delete connected;
  if (termo!=NULL)delete termo;
}
void CExtTermo::Update(ELEMENT_DATA *exterm_update)
{
  x0=exterm_update->X0;
  y0=exterm_update->Y0;
  radius=exterm_update->Radius;
  
  if (termo!=NULL)termo->Show(ConnectWnd,0);

  TERMO_DATA terr;
  terr.X0=x0;
  terr.Y0=y0;
  terr.Xt=exterm_update->Xt;
  terr.Yt=exterm_update->Yt;      
  terr.Angle=90;
  terr.Radius=radius;
  terr.Rad=radius;
  terr.Height=2*radius;
  terr.Width=6*radius;
  strcpy(terr.Text,exterm_update->Text);
  if (termo==NULL) termo=new CTermo(&terr);
  else termo->Update(&terr);
  if (termo!=NULL) termo->ShowText(showtext);
  if (termo!=NULL) termo->Show(ConnectWnd,1);
  
  bound.left=x0-2*radius;
  bound.top=y0+9*radius;
  bound.height=9*radius;
  bound.width=3*radius;
  exterm_update->Bounds=bound;

  strcpy(cd.TypeString,"Датчик температуры наружного воздуха");
  SetTypeParam(exterm_update,EXTERM);
  CTempl::Update(exterm_update);
}
void CExtTermo::UpdateFromBounds(ELEMENT_DATA *exterm_update)
{
  exterm_update->X0=exterm_update->Bounds.left+2*exterm_update->Radius;
  exterm_update->Y0=exterm_update->Bounds.top-9*exterm_update->Radius;
  exterm_update->Bounds.height=9*exterm_update->Radius;
  exterm_update->Bounds.width=3*exterm_update->Radius;
  Update(exterm_update);  
  if (termo!=NULL){termo->ResetLocation();exterm_update->Xt=termo->Xt;exterm_update->Yt=termo->Yt;}
}
void CExtTermo::GetData(ELEMENT_DATA *exterm_data)const
{
  exterm_data->X0=x0;
  exterm_data->Y0=y0;
  exterm_data->Radius=radius;
  if (termo!=NULL)
  {
    exterm_data->Xt=termo->Xt;
    exterm_data->Yt=termo->Yt;
    strcpy(exterm_data->Text,termo->text);
  } 
  CTempl::GetData(exterm_data);     
}
void CExtTermo::Draw(BOOL ShowNode)
{
  DrawNodes(ShowNode);  

  glNormal3d(0,0,1);
  GLdouble params[4];
  glGetDoublev(GL_CURRENT_COLOR,params);   
  GLfloat amb[4],ambient[4] = {0.0,0.0, 0.0, 1};
    glGetFloatv(GL_LIGHT_MODEL_AMBIENT,amb);    
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
   
  double w=radius;
  double delta=radius*5;
  double dist=2*radius;
  double xl=x0-radius;
  double z=radius+0.1;
  glColor3d(1,1,1);
  glBegin(GL_QUADS);
  glVertex3d(xl,y0+radius,z);
  glVertex3d(xl,y0+dist,z);
  glVertex3d(x0+radius,y0+dist,z);
  glVertex3d(x0+radius,y0+radius,z);

  glVertex3d(xl,y0+radius+delta,z);
  glVertex3d(xl,y0+dist+delta,z);
  glVertex3d(x0+radius,y0+dist+delta,z);
  glVertex3d(x0+radius,y0+radius+delta,z);
  
  glVertex3d(xl-w,y0,z);
  glVertex3d(xl-w,y0+2*dist+delta,z);
  glVertex3d(xl,y0+2*dist+delta,z);
  glVertex3d(xl,y0,z);
  glEnd();
  
  glColor3d(0,0,0);
  double z1=z+0.1;
  double depth=0.1*radius;
  w-=depth;
  xl-=depth;
  glBegin(GL_LINES);  
  for (double i=2*dist+delta-w;i>=0;i-=radius)
  {
    glVertex3d(xl-w,y0+i,z1);
    glVertex3d(xl,y0+i+w,z1);   
  }
  glEnd();
  
  glBegin(GL_QUADS);
  glVertex3d(xl-2*depth,y0,z1);
  glVertex3d(xl-2*depth,y0+2*dist+delta,z1);
  glVertex3d(xl,y0+2*dist+delta,z1);
  glVertex3d(xl,y0,z1);
  glEnd();
  glBegin(GL_LINE_STRIP); 
    glVertex3d(xl,y0,z1);
    glVertex3d(xl,y0+radius+depth,z1);
    glVertex3d(x0+radius-depth,y0+radius+depth,z1);
    glVertex3d(x0+radius-depth,y0+dist-depth,z1);
    glVertex3d(xl,y0+dist-depth,z1);
    glVertex3d(xl,y0+radius+delta+depth,z1);
    glVertex3d(x0+radius-depth,y0+radius+delta+depth,z1);
    glVertex3d(x0+radius-depth,y0+dist+delta-depth,z1);
    glVertex3d(xl,y0+dist+delta-depth,z1);  
    glVertex3d(xl,y0+2*dist+delta,z1);  
  glEnd();

  glColor3dv(params);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
  if ((termo!=NULL)&&(termo->ON())){termo->ShowText(showtext);termo->Draw();}
}
void CExtTermo::Animate()
{
  if (Focused) DrawBound(&bound);
}
int CExtTermo::GetNodeKol() const
{
  return 1;
}
CShemaNode CExtTermo::GetNode(int number) const
{
  CShemaNode node;
  node.X=x0;
  node.Y=y0;
  node.Radius=radius/2.0;
  node.Angle=270;
  return node;
}
bool CExtTermo::HasText()
{
  return true;  
}
bool CExtTermo::PointInTxtRgn(MOUSEXY *m)
{
  if (termo!=NULL)if (termo->ON()) return termo->PointInRgn(m);
  return false;
}
void CExtTermo::MoveText(RECTAN * newPos)
{
  if (termo!=NULL)if (termo->ON()) {termo->MoveText(newPos);return;}  
}
RECTAN CExtTermo::GetTxtBounds()
{
  if (termo!=NULL)if (termo->ON()) return termo->GetBound();
  return CTempl::GetTxtBounds();  
}
void CExtTermo::ConnectionCreate(CTemplWnd * newConnectWnd)
{
  CTempl::ConnectionCreate(newConnectWnd);
  if (termo!=NULL)if (termo->ON()) termo->ConnectionCreate(newConnectWnd);  
}
//-------------------------------------------------------------------
CConnection::CConnection(ELEMENT_DATA *cotion_create,BOOL FromBounds)
{
  Focused=0;
  InitConnect();
  if (FromBounds)UpdateFromBounds(cotion_create);
  else Update(cotion_create);
}
CConnection::CConnection(const CConnection &prev_cotion) //конструктор копий
{
  Focused=0;
  CopyConnect(&prev_cotion);
  ELEMENT_DATA t;
  prev_cotion.GetData(&t);
  Update(&t);
}
CConnection::~CConnection()
{
  if (connected!=NULL)delete connected;
}
void CConnection::Update(ELEMENT_DATA *cotion_update)
{
  x0=cotion_update->X0;
  y0=cotion_update->Y0;
  xt=cotion_update->Xt;
  yt=cotion_update->Yt;
  x=cotion_update->Radius1;
  y=cotion_update->Height;
  x1=cotion_update->Angle;
  y1=cotion_update->Kol;
  rad=cotion_update->Radius/2.0;

  bound.left=x0;
  if (bound.left>xt)bound.left=xt;
  if (bound.left>x)bound.left=x;
  if (bound.left>x1)bound.left=x1;
  bound.top=y0;
  if (bound.top<yt)bound.top=yt;
  if (bound.top<y)bound.top=y;
  if (bound.top<y1)bound.top=y1;
  TDist maxX,minY;
  maxX=x0;
  if (maxX<xt)maxX=xt;
  if (maxX<x)maxX=x;
  if (maxX<x1)maxX=x1;
  minY=y0;
  if (minY>yt)minY=yt;
  if (minY>y)minY=y;
  if (minY>y1)minY=y1;
  bound.width=maxX-bound.left;
  bound.height=bound.top-minY;
  cotion_update->Bounds=bound;
  strcpy(cd.TypeString,"Соединительный провод");
  SetTypeParam(cotion_update,COTION);
  CTempl::Update(cotion_update);
}
void CConnection::UpdateFromBounds(ELEMENT_DATA *cotion_update)
{
  //cotion_update->X0=cotion_update->Bounds.left;
  //cotion_update->Y0=cotion_update->Bounds.top;
  Update(cotion_update);  
}
void CConnection::GetData(ELEMENT_DATA *cotion_data)const
{
  cotion_data->X0=x0;
  cotion_data->Y0=y0;
  cotion_data->Xt=xt;
  cotion_data->Yt=yt;
  cotion_data->Radius1=x;
  cotion_data->Height=y;
  cotion_data->Angle=x1;
  cotion_data->Kol=y1;
  cotion_data->Radius=rad*2;

  CTempl::GetData(cotion_data);     
}
void CConnection::Draw(BOOL ShowNode)
{
  auxDrawDistrict(x0,y0,xt,yt,x,y,x1,y1,rad);       
}
void CConnection::Animate()
{
  if (Focused)DrawPointBound(x0,y0,xt,yt,x,y,x1,y1,2*rad);  
}
//------------------------------------------------------------------- 

