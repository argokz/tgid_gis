#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDaoWork frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

//#include "C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\um\gl\GL.h" 
//#include "C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\um\gl\GLU.h" 

//#include <gl/gl.h>			//Open GL headers
//#include <gl/glu.h>
//#include <gl/glaux.h>




struct TCOLOR
{
	GLubyte R;
	GLubyte G;
	GLubyte B;
};
struct WRECT
{
	double left,
		   top,
		   width,
		   height;
};
struct SCREEN
{
	int CX,CY,XL2,YL2,Step;
	WRECT Wrect;
	int OriginalStep;		
	TCOLOR ForeColor,BackColor,BackColor2,AnimColor,ColdColor,HotColor;
};
struct SCALE_FONT
{
	double X;
	double Y;
	double Z;	
};
struct FONT_DATA
{
	LOGFONT lf;
	SCALE_FONT scale;
	TCOLOR color;
};
struct WORKSPACE
{
	bool DrawVect; 
	bool ShowFont; 
	bool ShowBalloons;
};
#define ONLY_ONE_CLICK 0x0000001
#define ONLY_TWO_CLICK 0x0000002 
#define ONLY_OPEN_OBJECT 0x0000004 
#define ONLY_CLOSE_OBJECT 0x0000008 
#define ONLY_SHOW_TEXT 0x000010

class CTemplWnd :public CWnd
{
public:	
	CTemplWnd() ;    // constructor used by dynamic creation
	~CTemplWnd();
	FONT_DATA font_data;
	GLYPHMETRICSFLOAT gmf[256];
	GLuint base;
	SCREEN sc;
	TCOLOR NodeColor;
	WORKSPACE worksp;
	void ResetColors(SCREEN * scr=NULL);
	void ResetWorkspace();
	void _ResetColors(SCREEN * scr);

// Attributes
protected:	
};

/////////////////////////////////////////////////////////////////////////////
