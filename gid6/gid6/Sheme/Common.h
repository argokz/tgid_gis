#ifndef COMMON_MODULE
#define COMMON_MODULE

enum {NONE=0,TUBE=1,BATTERY=2,TURN=3,TRINITY=4,VENTIL=5,NASOS=6,DROSSEL=7,AIRBATTERY=8,VODOBATTERY=9,SOPLO=10,CONNECTOR=11,KRAN=12, LABEL=13,EXTERM=14,COTION=15};
#define WM_MY_SHEME_ACTION WM_USER+61
#define VENTIL_OPENED 0x00000001
#define VENTIL_CLOSED 0x00000002
#define NASOS_OPENED 0x00000004
#define NASOS_CLOSED 0x00000008
#define AIRBAT_OPENED 0x00000010
#define AIRBAT_CLOSED 0x00000020

#define faska 1.4
#define faskalen (float)0.5
#define _slices 4
#define _stacks 1
#define visal_zazor 0.3
#define ed_min_cosFI 0.0152
#define sinFI 0.174
#define koeff 0.174
#define sinV 0.087
#define cosV 0.996

//#define ScaleFactor 0.2
#define ScaleFactor 1

typedef long TDist;
struct COMMON_DATA
{
  long Type;
  char TypeString[100]; 
  long UserNumber;
  char UserString[1000];
};
struct RECTAN
{
  TDist left,
       top,
       width,
       height;
};

struct GROUP_DATA 
{
  long group;
  bool groupOn;
};
struct ELEMENT_DATA 
{
  COMMON_DATA cd;
  TDist X0;
  TDist Y0;
  TDist Radius;
  long Angle;
  TDist Height;
  unsigned char animate;
  unsigned char Pribor;
  bool Right;
  char  Text[256];
  long Kol;
  unsigned char Opening;
  TDist Radius1;
  TDist Xt;
  TDist Yt; 
  RECTAN Bounds;
  GROUP_DATA grdat; 
};
/*struct MOUSEXY
{
  TDist X;
  TDist Y;
};*/
struct CShemaNode
{
  TDist X;
  TDist Y;
  double Radius;
  int Angle;
  BOOL Connected;
};
struct VECTORXY
{
  double X;
  double Y;
};
typedef VECTORXY MOUSEXY;


struct PLACE
{
  int Type;
  int Number;
};

struct CDuo
{
  PLACE place;
  int first;
  int second;
};
struct DATA_TRANSFER
{
  long group;
  long PARAM;
  PLACE place;
  COMMON_DATA cd;
};
#endif //COMMON_MODULE