#pragma once


struct generic;
struct layer_type;
struct ltype_type;
struct block_type;
struct layout_type;

struct cdxf
{
  cdxf() {
    l_xmin = 1e20, l_xmax = -1e20, l_ymin = 1e20, l_ymax = -1e20;
    limit_x = limit_y = limit_mx = limit_my = 0.0;
    extent_x = extent_y = extent_mx = extent_my = 0.0;
    cmlscale = 1.0;
    insunits = 5;
    en_anchor = NULL;
    ly_anchor = NULL;
    lt_anchor = NULL;
    block_anchor = NULL;
    layout_anchor = NULL;
  };
  ~cdxf();
  double limit_x;
  double limit_y;
  double extent_x;
  double extent_y;
  double limit_mx;
  double limit_my;
  double extent_mx;
  double extent_my;
  double pextmin_x;
  double pextmin_y;
  double pextmax_x;
  double pextmax_y;
  double cmlscale;
  double plinewid;
  double tracewid;
  int insunits;
  int measurement;
  int dwgcodepage;

  double l_xmin, l_xmax, l_ymin, l_ymax;
  double p25400;

  generic *en_anchor;
  layer_type *ly_anchor;
  ltype_type *lt_anchor;
  block_type *block_anchor;
  layout_type *layout_anchor;


  CDC *pDC;
  CRect m_rect;
  double m_bx, m_by, masx, masy;
};


struct layout_type
{
  layout_type *next;
//  int what;
  double xmin, xmax, ymin, ymax;

  double x10, y20, x11, y21;
  double x14, y24, x15, y25;

//  layer_type *lyr;
//  int color;
  char *name;
//  int flags;
//  ltype_type *lt;
//  double base_x;
//  double base_y;
//  void *ent;
//  generic *en_anchor;

//  int num, uze;  /// Я добавил

};



struct dash_type
{
  dash_type *next;
  double length;
};

struct style_type
{
  style_type *next;
  char *name;
  int flags;
  int gen_flags;
  double txt_scale;
  double height;
  double oblique;
  double last_height;
  char *font;
  char *big_font;
};


struct ltype_type
{
  ltype_type *next;
  char *name;
  char *descript;
  int flags;
  int alignment;
  int dashes;
  double length;
  dash_type *dash_length;
};

struct layer_type
{
  layer_type *next;
  char *name;
  int flags;
  int color;
  char *lt;
  int num, uze;  /// Я добавил
};

struct vertex_type
{
  vertex_type *next;
  int what;
  double x1, x2, y1, y2;
  layer_type *lyr;
  int color;
  double x;
  double y;
  int n71, n72, n73, n74;
  double w1, w2;
};

struct block_type
{
  block_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  char *name;
  int flags;
  ltype_type *lt;
  double base_x;
  double base_y;
  void *ent;
  generic *en_anchor;

  int num, uze;  /// Я добавил

};

struct circle_type
{
  circle_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  double x;
  double y;
  double w;
  double radius;
};


struct arc_type
{
  arc_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  double x;
  double y;
  double w;
  double a1;
  double a2;
  double radius;
};


struct ellipse_type
{
  circle_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  double x;
  double y;
  double rx;
  double ry;
  double w;
  double ratio;
  double radius;
};



struct line_type
{
  line_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  double sx;
  double sy;
  double ex;
  double ey;
  double w1;
  double w2;
  double px;
  double bulge;
  char last_one;
  int n71, n72, n73;
  vertex_type *vertex;
};

struct hatch_type
{
  line_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  int n;
  double *x;
  double *y;
  double bulge;
  char last_one;
};


struct lwpline_type
{
  line_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  int n;
  double *x;
  double *y;
  double w;
  double bulge;
  char last_one;
};

struct spline_type
{
  line_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  int n;
  double *x;
  double *y;

  int n_knot;
  double *knot;

  int n_fit;
  double *x_fit;
  double *y_fit;

  double w;
  double bulge;
  char last_one;
};


struct point_type
{
  point_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  double px;
  double py;
  double w;
};



struct solid_type
{
  solid_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  double x1, y1, x2, y2, x3, y3, x4, y4;
};

struct text_type
{
  text_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  double ix;
  double iy;
  double height;
  char *text;
  char *prompt;
  char *def_value;
  char *tag;
  int field_length;
  int att_flags;
  double angle;
  double x_scale;
  double oblique;
  char *font;
//  style_type *font;
  int gen_flags;
  int justify;
  double ax;
  double ay;
};

struct mtext_type
{
  text_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  double ix;
  double iy;
  double height;
  char *text;
  char *prompt;
  char *def_value;
  char *tag;
  int field_length;
  int att_flags;
  double angle;
  double x_scale;
  double oblique;
  char *font;
//  style_type *font;
  int gen_flags;
  int justify;
  double ax;
  double ay;
};


struct insert_type
{
  insert_type *next;
  int what;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
  ltype_type *lt;
  int att_flags;
  char *b;
  double ix;
  double iy;
  double angle;
  double x_scale;
  double y_scale;
  double z_scale;
  int rows;
  int columns;
  double r_space;
  double c_space;
};

struct generic
{
  generic *next;
  int ident;
  double xmin, xmax, ymin, ymax;
  layer_type *lyr;
  int color;
};

layer_type * add_layer(const char *s, cdxf *);
layer_type *find_layer(char *s, int color, cdxf *);
layer_type *find_layer(char *s, cdxf *);
ltype_type *find_ltype(char *s, cdxf *);
layer_type *find_style(char *s, cdxf *);

generic* line_parser(FILE *f, char *lyr, cdxf *dxf);
generic* point_parser(FILE *f, char *lyr, cdxf *dxf);
generic* circle_parser(FILE *f, char *lyr, cdxf *dxf);
generic* ellipse_parser(FILE *f, char *lyr, cdxf *dxf);
generic* arc_parser(FILE *f, char *lyr, cdxf *dxf);
generic* trace_parser(FILE *f, char *lyr);
generic* solid_parser(FILE *f, char *lyr, cdxf *dxf);
generic* text_parser(FILE *f, char *lyr, cdxf *dxf);
generic* mtext_parser(FILE *f, char *lyr, cdxf *dxf);
generic* shape_parser(FILE *f, char *lyr);
generic* insert_parser(FILE *f, char *lyr, cdxf *dxf);
generic* dimension_parser(FILE *f, char *lyr);
generic* attdef_parser(FILE *f, char *lyr);
generic* attrib_parser(FILE *f, char *lyr);
line_type * pline_parser(FILE *f, char *lyr, cdxf *dxf);
generic * lwpline_parser(FILE *f, char *lyr, cdxf *dxf);
generic * spline_parser(FILE *f, char *lyr, cdxf *dxf);
generic * hatch_parser(FILE *f, char *lyr, cdxf *dxf);
generic* vertex_parser(FILE *f, line_type *pl, char *lyr, cdxf *dxf);

