#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Token.h"
#include "dxfi.h"

static double s_dx = 0;
static double s_dy = 0;

//static double s_dx = 35221.33442059075;
//static double s_dy = 5126.937927326165;

//void set_dxf_dx_dy(double dx, double dy)
//{
//    s_dx = dx;
//    s_dy = dy;
//}


generic* parser(FILE* f, char* lyr, cdxf* dxf) {
    int t;
    char* c;
    //  circle_type *cl;

    //  cl = (circle_type *) calloc(1, sizeof(circle_type));
    //  cl->what = CIRCLE;
    //  cl->color = 0;
    //  if (lyr) cl->lyr = find_layer(lyr, dxf);

    while (1)
    {
        c = get_tag(f);
        t = get_tag_token(c);

        if (t == N_0) break;
        if (t == EOFILE) {
            break;
        }

        c = get_tag(f);
        /*
            switch (t)
            {
              case N_8  : cl->lyr = find_layer(c, dxf); break;
              case N_10 : cl->x = atof(c); break;
              case N_20 : cl->y = atof(c); break;
              case N_40 : cl->radius = atof(c); break;
              case N_62 : cl->color = atoi(c); break;
        //      case N_39 : cl->w = atof(c);  break;
            }
        */
    }
    /*
      cl->xmin = cl->x-cl->radius;
      cl->xmax = cl->x+cl->radius;
      cl->ymin = cl->y-cl->radius;
      cl->ymax = cl->y+cl->radius;

      return (generic *) cl;
    */
    return NULL;
}



generic* trace_parser(FILE* f, char* lyr) { return parser(f, lyr, NULL); }
generic* shape_parser(FILE* f, char* lyr) { return parser(f, lyr, NULL); }
generic* dimension_parser(FILE* f, char* lyr) { return parser(f, lyr, NULL); }
generic* attdef_parser(FILE* f, char* lyr) { return parser(f, lyr, NULL); }
generic* attrib_parser(FILE* f, char* lyr) { return parser(f, lyr, NULL); }

//generic* insert_parser(FILE *f, char *lyr) {
//  return parser(f, lyr, NULL);
//}


generic* insert_parser(FILE* f, char* lyr, cdxf* dxf) {
    int t;
    char* c;
    insert_type* in;

    in = (insert_type*)calloc(1, sizeof(insert_type));
    in->what = INSERT;
    in->color = 0;
    if (lyr) in->lyr = find_layer(lyr, dxf);

    in->x_scale = 1;
    in->y_scale = 1;

    while (1)
    {
        c = get_tag(f);
        t = get_tag_token(c);

        if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_2: in->b = locate(c); break;
        case N_5: in->lyr = find_layer(c, dxf); break;
        case N_8: in->lyr = find_layer(c, dxf); break;
        case N_10: in->ix = atof(c) - s_dx; break;
        case N_20: in->iy = atof(c) - s_dy; break;
        case N_41: in->x_scale = atof(c); break;
        case N_42: in->y_scale = atof(c); break;

        case N_50: in->angle = atof(c); break;

            //      case N_62 : in->color = atoi(c); break;
            //      case N_39 : in->w = atof(c);  break;
        }
    }

    in->xmin = in->ix;
    in->xmax = in->ix;
    in->ymin = in->iy;
    in->ymax = in->iy;

    return (generic*) in;
}
/*

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


  */


generic* arc_parser(FILE* f, char* lyr, cdxf* dxf)
{
    //    return parser(f, lyr, NULL); 

    int t;
    char* c;
    arc_type* cl;

    cl = (arc_type*)calloc(1, sizeof(arc_type));
    cl->what = ARC;
    cl->color = 0;
    if (lyr) cl->lyr = find_layer(lyr, dxf);

    while (1)
    {
        c = get_tag(f);
        t = get_tag_token(c);

        if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_8: cl->lyr = find_layer(c, dxf); break;
        case N_10: cl->x = atof(c) - s_dx; break;
        case N_20: cl->y = atof(c) - s_dy; break;
        case N_40: cl->radius = atof(c); break;
        case N_62: cl->color = atoi(c); break;

        case N_50: cl->a1 = atof(c); break;
        case N_51: cl->a2 = atof(c); break;

        case N_39: cl->w = atof(c);  break;
        }
    }

    cl->xmin = cl->x - cl->radius;
    cl->xmax = cl->x + cl->radius;
    cl->ymin = cl->y - cl->radius;
    cl->ymax = cl->y + cl->radius;

    return (generic*) cl;

}



generic* circle_parser(FILE* f, char* lyr, cdxf* dxf)
{
    int t;
    char* c;
    circle_type* cl;

    cl = (circle_type*)calloc(1, sizeof(circle_type));
    cl->what = CIRCLE;
    cl->color = 0;
    if (lyr) cl->lyr = find_layer(lyr, dxf);

    while (1)
    {
        c = get_tag(f);
        t = get_tag_token(c);

        if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_8: cl->lyr = find_layer(c, dxf); break;
        case N_10: cl->x = atof(c) - s_dx; break;
        case N_20: cl->y = atof(c) - s_dy; break;
        case N_40: cl->radius = atof(c); break;
        case N_62: cl->color = atoi(c); break;
        case N_39: cl->w = atof(c);  break;
        }
    }

    cl->xmin = cl->x - cl->radius;
    cl->xmax = cl->x + cl->radius;
    cl->ymin = cl->y - cl->radius;
    cl->ymax = cl->y + cl->radius;

    return (generic*) cl;
}

generic* ellipse_parser(FILE* f, char* lyr, cdxf* dxf) {
    int t;
    char* c;
    ellipse_type* el;

    el = (ellipse_type*)calloc(1, sizeof(ellipse_type));
    el->what = ELLIPSE;
    el->color = 0;
    if (lyr) el->lyr = find_layer(lyr, dxf);

    while (1)
    {
        c = get_tag(f);
        t = get_tag_token(c);

        if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_8: el->lyr = find_layer(c, dxf); break;
        case N_10: el->x = atof(c) - s_dx; break;
        case N_20: el->y = atof(c) - s_dy; break;
        case N_11: el->rx = atof(c); break;
        case N_21: el->ry = atof(c); break;
        case N_40: el->ratio = atof(c); break;
        case N_62: el->color = atoi(c); break;
            //      case N_43 : el->w = atof(c);  break;
        }
    }

    double r = hypot(el->rx, el->ry);


    el->xmin = el->x - r;
    el->xmax = el->x + r;
    el->ymin = el->y - r;
    el->ymax = el->y + r;

    return (generic*) el;
}


generic* line_parser(FILE* f, char* lyr, cdxf* dxf) {
    int t;
    char* c;
    line_type* lt;

    lt = (line_type*)calloc(1, sizeof(line_type));
    lt->what = LINE;
    lt->color = 0;
    if (lyr) lt->lyr = find_layer(lyr, dxf);

    while (1)
    {
        c = get_tag(f);
        t = get_tag_token(c);

        if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_8: lt->lyr = find_layer(c, dxf); break;
        case N_10: lt->sx = atof(c) - s_dx; break;
        case N_20: lt->sy = atof(c) - s_dy; break;
        case N_11: lt->ex = atof(c) - s_dx; break;
        case N_21: lt->ey = atof(c) - s_dy; break;
        }
    }

    lt->xmin = fmin(lt->sx, lt->ex); lt->xmax = fmax(lt->sx, lt->ex);
    lt->ymin = fmin(lt->sy, lt->ey); lt->ymax = fmax(lt->sy, lt->ey);

    return (generic*) lt;
}


generic* text_parser(FILE* f, char* lyr, cdxf* dxf) {
    int t;
    char* c;
    text_type* te;

    te = (text_type*)calloc(1, sizeof(text_type));
    te->what = TEXT;
    if (lyr) te->lyr = find_layer(lyr, dxf);

    while (1)
    {
        t = get_tag_token(get_tag(f));  if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_8: te->lyr = find_layer(c, dxf); break;
        case N_10: te->ix = atof(c) - s_dx; break;
        case N_20: te->iy = atof(c) - s_dy; break;
        case N_40: te->height = atof(c);  break;
        case N_1: te->text = locate(c);  break;
        case N_50: te->angle = atof(c);   break;
        case N_41: te->x_scale = atof(c); break;
        case N_51: te->oblique = atof(c); break;
        case N_7:  te->font = locate(c);  break;
            // Font
            break;
        case N_70: te->att_flags = atoi(c); break;
        case N_71: te->gen_flags = atoi(c); break;
        case N_72: te->justify = atoi(c);   break;
        case N_11: te->ax = atof(c);  break;
        case N_21: te->ay = atof(c);  break;
        }
    }

    //  if (!strcmp(te->text, "ул. Степная")) {
    //    int i;
    //    i = 0;
    //  }


    double dd = te->height * ((te->text != 0) ? strlen(te->text) : 1);

    te->xmin = te->ix - dd; te->xmax = te->ix + dd;
    te->ymin = te->iy - dd; te->ymax = te->iy + dd;

    return (generic*) te;
}


generic* mtext_parser(FILE* f, char* lyr, cdxf* dxf) {
    int t;
    char* c;
    mtext_type* te;

    te = (mtext_type*)calloc(1, sizeof(mtext_type));
    te->what = MTEXT;
    if (lyr) te->lyr = find_layer(lyr, dxf);

    while (1)
    {
        t = get_tag_token(get_tag(f));  if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_8: te->lyr = find_layer(c, dxf); break;
        case N_10: te->ix = atof(c) - s_dx; break;
        case N_20: te->iy = atof(c) - s_dy; break;
        case N_40: te->height = atof(c);  break;
        case N_1: te->text = locate(c);  break;
        case N_50: te->angle = atof(c);   break;
        case N_41: te->x_scale = atof(c); break;
        case N_51: te->oblique = atof(c); break;
        case N_7:  te->font = locate(c);  break;
            // Font
            break;
        case N_70: te->att_flags = atoi(c); break;
        case N_71: te->gen_flags = atoi(c); break;
        case N_72: te->justify = atoi(c);   break;
        case N_11: te->ax = atof(c);  break;
        case N_21: te->ay = atof(c);  break;
        }
    }

    double dd = te->height * ((te->text != 0) ? strlen(te->text) : 1);

    te->xmin = te->ix - dd; te->xmax = te->ix + dd;
    te->ymin = te->iy - dd; te->ymax = te->iy + dd;

    return (generic*) te;
}


line_type* pline_parser(FILE* f, char* lyr, cdxf* dxf) {
    int t;
    char* c;
    line_type* pl;

    pl = (line_type*)calloc(1, sizeof(line_type));
    pl->what = POLYLINE;
    if (lyr) pl->lyr = find_layer(lyr, dxf);
    pl->xmin = 1e20; pl->xmax = -1e20; pl->ymin = 1e20; pl->ymax = -1e20;
    pl->w1 = pl->w2 = dxf->plinewid;
    pl->w1 = pl->w2 = dxf->tracewid;
    //  pl->color = -1;

    while (1)
    {
        t = get_tag_token(get_tag(f));    if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_8: pl->lyr = find_layer(c, dxf);  break;
        case N_6: pl->lt = find_ltype(c, dxf);   break;
        case N_40: pl->w1 = atof(c) - s_dx; break;
        case N_41: pl->w2 = atof(c) - s_dy; break;
        case N_70: pl->last_one = atoi(c) & 1;  break;
        case N_71: pl->n71 = atoi(c);  break;
        case N_72: pl->n72 = atoi(c);  break;
        case N_73: pl->n73 = atoi(c);  break;
        case N_62: pl->color = atoi(c); break;
        }
    }

    return pl;
}


generic* lwpline_parser(FILE* f, char* lyr, cdxf* dxf)
{
    int t;
    char* c;
    lwpline_type* pl;

    pl = (lwpline_type*)calloc(1, sizeof(lwpline_type));
    pl->what = LWPOLYLINE;
    //  pl->color = -1;
    pl->w = dxf->plinewid;
    pl->w = dxf->tracewid;
    //  pl->w = 0.05;
    if (lyr) pl->lyr = find_layer(lyr, dxf);

    int n_x = 0;
    int n_y = 0;

    pl->xmin = 1e20; pl->xmax = -1e20; pl->ymin = 1e20; pl->ymax = -1e20;

    while (1)
    {
        t = get_tag_token(c = get_tag(f));    if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_8: pl->lyr = find_layer(c, dxf);  break;
        case N_6: pl->lt = find_ltype(c, dxf);   break;
        case N_62: pl->color = atoi(c); break;
        case N_70: pl->last_one = atoi(c) & 1;  break;
        case N_43: pl->w = atof(c);  break;
        case N_90:
            pl->n = atoi(c);
            pl->x = (double*)malloc(pl->n * sizeof(double));
            pl->y = (double*)malloc(pl->n * sizeof(double));
            break;
        case N_10:
            pl->x[n_x++] = atof(c) - s_dx;
            pl->xmin = fmin(pl->xmin, pl->x[n_x - 1]);
            pl->xmax = fmax(pl->xmax, pl->x[n_x - 1]);
            break;
        case N_20:
            pl->y[n_y++] = atof(c) - s_dy;
            pl->ymin = fmin(pl->ymin, pl->y[n_y - 1]);
            pl->ymax = fmax(pl->ymax, pl->y[n_y - 1]);
            break;
        }
    }

    return (generic*) pl;
}


generic* spline_parser(FILE* f, char* lyr, cdxf* dxf)
{
    int t;
    char* c;
    spline_type* pl;

    pl = (spline_type*)calloc(1, sizeof(spline_type));
    pl->what = SPLINE;
    //  pl->color = -1;
    pl->w = dxf->plinewid;
    pl->w = dxf->tracewid;
    //  pl->w = 0.05;
    if (lyr) pl->lyr = find_layer(lyr, dxf);

    int n_x = 0;
    int n_y = 0;
    int n_x_fit = 0;
    int n_y_fit = 0;
    int n_x_knot = 0;

    pl->xmin = 1e20; pl->xmax = -1e20; pl->ymin = 1e20; pl->ymax = -1e20;

    while (1)
    {
        t = get_tag_token(c = get_tag(f));    if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_8: pl->lyr = find_layer(c, dxf);  break;
        case N_6: pl->lt = find_ltype(c, dxf);   break;
        case N_62: pl->color = atoi(c); break;
        case N_70: pl->last_one = atoi(c) & 1;  break;
        case N_43: pl->w = atof(c);  break;
        case N_73:
            pl->n = atoi(c);
            pl->x = (double*)malloc(pl->n * sizeof(double));
            pl->y = (double*)malloc(pl->n * sizeof(double));
            break;
        case N_74:
            pl->n_fit = atoi(c);
            pl->x_fit = (double*)malloc(pl->n * sizeof(double));
            pl->y_fit = (double*)malloc(pl->n * sizeof(double));
            break;
        case N_72:
            pl->n_knot = atoi(c);
            pl->knot = (double*)malloc(pl->n_knot * sizeof(double));
            break;
        case N_10:
            pl->x[n_x++] = atof(c) - s_dx;
            pl->xmin = fmin(pl->xmin, pl->x[n_x - 1]);
            pl->xmax = fmax(pl->xmax, pl->x[n_x - 1]);
            break;
        case N_20:
            pl->y[n_y++] = atof(c) - s_dy;
            pl->ymin = fmin(pl->ymin, pl->y[n_y - 1]);
            pl->ymax = fmax(pl->ymax, pl->y[n_y - 1]);
            break;

        case N_11:
            pl->x_fit[n_x_fit++] = atof(c) - s_dx;
            //        pl->xmin = min(pl->xmin, pl->x_fit[n_x_fit-1]);
            //        pl->xmax = max(pl->xmax, pl->x_fit[n_x_fit-1]);
            break;
        case N_21:
            pl->y_fit[n_y_fit++] = atof(c) - s_dy;
            //        pl->ymin = min(pl->ymin, pl->y_fit[n_y-1]);
            //        pl->ymax = max(pl->ymax, pl->y_fit[n_y-1]);
            break;

        case N_40:
            pl->knot[n_x_knot++] = atof(c);
            break;
        }
    }

    //  if () pl->w = 0.05;

    return (generic*) pl;
}


generic* hatch_parser(FILE* f, char* lyr, cdxf* dxf)
{
    int t;
    char* c;
    hatch_type* pl;

    pl = (hatch_type*)calloc(1, sizeof(hatch_type));
    pl->what = HATCH;
    pl->n = 0;
    if (lyr) pl->lyr = find_layer(lyr, dxf);

    int n_x = 0;
    int n_y = 0;

    while (1)
    {
        t = get_tag_token(c = get_tag(f));    if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_8: pl->lyr = find_layer(c, dxf);  break;
        case N_70: pl->last_one = atoi(c) & 1;  break;
        case N_93:
            pl->n = atoi(c);
            pl->x = (double*)malloc(pl->n * sizeof(double));
            pl->y = (double*)malloc(pl->n * sizeof(double));
            n_x = 0;
            n_y = 0;
            break;
        case N_10: if (pl->n && n_x < pl->n) pl->x[n_x++] = atof(c) - s_dx;  break;
        case N_20: if (pl->n && n_y < pl->n) pl->y[n_y++] = atof(c) - s_dy;  break;
        }
    }


    if (pl->n >= 1) {
        pl->xmin = pl->xmax = pl->x[0];
        pl->ymin = pl->ymax = pl->y[0];

        for (int i = 0; i < pl->n; i++) {
            pl->xmin = fmin(pl->xmin, pl->x[i]);
            pl->xmax = fmax(pl->xmax, pl->x[i]);
            pl->ymin = fmin(pl->ymin, pl->y[i]);
            pl->ymax = fmax(pl->ymax, pl->y[i]);
        }
    }


    return (generic*) pl;
}

generic* vertex_parser(FILE* f, line_type* pl, char* lyr, cdxf* dxf) {
    int t;
    char* c;
    vertex_type* v;
    vertex_type vv;

    vv.next = nullptr;
    vv.color = 0;
    vv.x = vv.y = 0;
    vv.n71 = vv.n72 = vv.n73 = vv.n74 = 0;
    vv.what = VERTEX;
    //  vv.color = -1;
    vv.w1 = vv.w2 = -1;

    while (1)
    {
        t = get_tag_token(get_tag(f));   if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_62: vv.color = atoi(c); break;
        case N_10: vv.x = atof(c) - s_dx; break;
        case N_20: vv.y = atof(c) - s_dy; break;
        case N_40: vv.w1 = atof(c);  break;
        case N_41: vv.w2 = atof(c);  break;
        case N_71: vv.n71 = atoi(c);  break;
        case N_72: vv.n72 = atoi(c);  break;
        case N_73: vv.n73 = atoi(c);  break;
        case N_74: vv.n74 = atoi(c);  break;
        }
    }

    if (vv.x == 0 && vv.y == 0) {
        vv.x = vv.x;
        //    return NULL;
    }

    v = (vertex_type*)calloc(1, sizeof(vertex_type));

    if (v)
    {
        v->next = pl->vertex;
        pl->vertex = v;
        v->x = vv.x;
        v->y = vv.y;
        v->color = vv.color;
        v->n71 = vv.n71;
        v->n72 = vv.n72;
        v->n73 = vv.n73;
        v->n74 = vv.n74;
    }

    pl->xmin = fmin(pl->xmin, v->x);
    pl->xmax = fmax(pl->xmax, v->x);
    pl->ymin = fmin(pl->ymin, v->y);
    pl->ymax = fmax(pl->ymax, v->y);

    return (generic*) v;
}

generic* solid_parser(FILE* f, char* lyr, cdxf* dxf) {
    int t;
    char* c;
    solid_type* sl;

    sl = (solid_type*)calloc(1, sizeof(solid_type));
    sl->what = SOLID;
    if (lyr) sl->lyr = find_layer(lyr, dxf);

    while (1)
    {
        t = get_tag_token(get_tag(f));   if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_10: sl->x1 = atof(c) - s_dx; break;
        case N_20: sl->y1 = atof(c) - s_dy; break;
        case N_11: sl->x2 = atof(c) - s_dx; break;
        case N_21: sl->y2 = atof(c) - s_dy; break;
        case N_12: sl->x3 = atof(c) - s_dx; break;
        case N_22: sl->y3 = atof(c) - s_dy; break;
        case N_13: sl->x4 = atof(c) - s_dx; break;
        case N_23: sl->y4 = atof(c) - s_dy; break;
        case N_8: sl->lyr = find_layer(c, dxf); break;
        case N_62: sl->color = atoi(c); break;
        }
    }
    return (generic*) sl;
}


generic* point_parser(FILE* f, char* lyr, cdxf* dxf)
{
    int t;
    char* c;
    point_type* cl;

    cl = (point_type*)calloc(1, sizeof(point_type));
    cl->what = POINT1;
    cl->color = 0;
    if (lyr) cl->lyr = find_layer(lyr, dxf);

    while (1)
    {
        c = get_tag(f);
        t = get_tag_token(c);

        if (t == N_0) break;

        c = get_tag(f);

        switch (t)
        {
        case N_8: cl->lyr = find_layer(c, dxf); break;
        case N_10: cl->px = atof(c) - s_dx; break;
        case N_20: cl->py = atof(c) - s_dy; break;
        case N_62: cl->color = atoi(c); break;
        case N_39: cl->w = atof(c);  break;
        }
    }

    cl->xmin = cl->px;
    cl->xmax = cl->px;
    cl->ymin = cl->py;
    cl->ymax = cl->py;

    return (generic*) cl;
}

/*

static int block_num = 0;


block_type * add_block(const char *s, cdxf *dxf)
{
  block_type *l = NULL;

  if (l = (block_type*) calloc(1, sizeof(block_type)))
  {
    l->next = dxf->block_anchor;
    dxf->block_anchor = l;
    l->name = locate(s);

    l->num = block_num++;
    l->uze = 1;
  }

  return l;
}
*/


block_type* find_block(char* s, cdxf* dxf)
{
    block_type* l;

    for (l = dxf->block_anchor; l; l = l->next) {
        if (!qstricmp(s, l->name)) return l;
    }
    return NULL;
}

/**

block_type* block_parser(FILE *f, char *lyr, cdxf *dxf) {
  int t;
  char *c;
  block_type *block;

  block = (block_type *) calloc(1, sizeof(block_type));
  block->what = BLOCK;
  block->color = 0;
  if (lyr) block->lyr = find_layer(lyr, dxf);

  dxf->block_anchor = block;

  while (1)
  {
    c = get_tag(f);
    t = get_tag_token(c);

    c = get_tag(f);

    if (t == N_0) break;

    switch (t)
    {
      case N_2  :  add_block(c, dxf); break;
      case N_8  : block->lyr = find_layer(c, dxf); break;
      case N_10 : block->base_x = atof(c); break;
      case N_20 : block->base_y = atof(c); break;
    }
  }

  block->xmin = block->xmax = block->base_x;
  block->ymin = block->ymax = block->base_y;

  return block;
}
*/

block_type* block(FILE* f, cdxf* dxf)
{
    int t;
    char* c;

//    int state = 1;
//    int quit = FALSE;

    block_type* block;

    block = (block_type*)calloc(1, sizeof(block_type));
    block->what = BLOCK;
    block->color = 0;
    //    if (lyr) block->lyr = find_layer(lyr, dxf);

    //    dxf->block_anchor = block;


    while (1)
    {
        c = get_tag(f);
        t = get_tag_token(c);
        if (t == N_0) break;

        c = get_tag(f);


        switch (t)
        {
        case N_2:  block->name = locate(c);   break;
        case N_8: block->lyr = find_layer(c, dxf); break;
        case N_10: block->base_x = atof(c); break;
        case N_20: block->base_y = atof(c); break;
        }
    }

    block->xmin = block->xmax = block->base_x;
    block->ymin = block->ymax = block->base_y;

//    if (!strcmp(block->name, "стр")) {
//        int qq;
//        qq = 1;
//    }


    block->next = dxf->block_anchor;
    dxf->block_anchor = block;


    return block;
}
