#if !defined(__bbb_h)              // Sentry, use file only if it's not already included.
#define __bbb_h

#pragma pack(push, 1)

/* PCX File Header */
struct PCXHEADER {
  char            manuf;          /* Always =10 for Paintbrush   */
  char            hard;           /* Version information         */
  char            encod;          /* Run-length encoding (=1)    */
  char            bitpx;          /* Bits per pixel              */
  unsigned short  x1;             /* Picture dimensions (incl)   */
  unsigned short  y1;
  unsigned short  x2;
  unsigned short  y2;
  unsigned short  hres;           /* Display horiz resolution    */
  unsigned short  vres;           /* Display vert  resolution    */
  char            clrma[48];      /* Pallete                     */
  char            vmode;          /* (ignored)                   */
  char            nplanes;        /* Number of planes (ver 2.5=0)*/
  unsigned short  bplin;          /* Bytes per line              */
  unsigned short  palinfo;        /* Palette Info (1=col, 2=gray)*/
  unsigned short  shres;          /* Scanner resolution          */
  unsigned short  svres;          /*                             */

  double x0;
  double y0;
  double dx;
  double dy;

  char            xtra[54-4*sizeof(double)];

        /* Extra space (filler)        */
};

struct PODL {
  unsigned short sign;
  unsigned short typ;
  unsigned short DD;
  unsigned short BTS;
  unsigned short xx, yy;
  unsigned long x, y, dx, dy;
  unsigned short BitCount;
  unsigned short nColors;

  double x0, y0;
  double dx0, dy0;
};

#pragma pack(pop)


class PodlObj {
  public:
    CString fN;
    double x0, y0, dx, dy;
    int mx0, my0;
    int typ;

    bool operator<(const PodlObj& mc) const
    {
      return fN < mc.fN; 
    };
};

#include "coordlis.h"

class CPodlList : public set<PodlObj> {
public:
  CPodlList();
  void openDir(const char *dir);
  void openFile(const char *fn);
  void Draw(CDC *dc, CRect rect, double m_bx, double m_by, double masx, double masy);
  CFPoint find();

private:
  void OpenQQQ();
  void OpenQQQ(const char *fN);
  void OpenBBB(const char *fN, int typ);
};



#endif                                      // __bbb_h sentry.
