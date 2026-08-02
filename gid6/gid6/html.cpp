#include "stdafx.h"
#include "html.h"
#include "css.h"

HTML::HTML(FILE *f1, CSS *_css) {
  n_td0 = 38;
  f = f1;
  n_yellow = -1;
  is_tr = false;
  n_tr = 0;
  css = _css;
  n_colspan = 1;
  n_rowspan = 1;
}

HTML::~HTML() {
  print_tr2();
}


void HTML::set_tr(int n) {
  n_td0 = n;
}

/*
void HTML::print_td(double d) 
{
  CString old;
  
  if (css) {
    old = css->css.text_align;
    css->css.text_align = "right";
  }
  if (d != 0) {
    print_td("%.3f", d);
  }
  else {
    print_td("&nbsp;");
  }
  if (css) css->css.text_align = old;
}
*/


void HTML::print_td(double d) 
{
  CString old;
  
  if (css) {
    old = css->css.text_align;
    css->css.text_align = "right";
  }
  if (d != 0) {
    print_td("%lg", d);
  }
  else {
    print_td("&nbsp;");
  }
  if (css) css->css.text_align = old;
}


void HTML::print_td() 
{
  print_td_st("", "&nbsp;");
}

void HTML::print_td(const char *fmt, ...)
{
  va_list argptr;
  char str[25500];

  va_start( argptr, fmt );
  vsprintf( str, fmt, argptr );


  CString style = "";
  
  if (css) style = css->init();

  if (!str[0]) strcpy(str, "&nbsp;");

  print_td_st(style, str);
  va_end( argptr );
}

/*
void HTML::print_td_st(const char *cl1, double d)
{
  if (d != 0) {
    print_td_st(cl1, "%.3f", d);
  }
  else {
    print_td_st(cl1, "&nbsp;");
  }
}
*/

void HTML::print_td_st(const char *cl1, double d)
{
  if (d != 0) {
    print_td_st(cl1, "%lg", d);
  }
  else {
    print_td_st(cl1, "&nbsp;");
  }
}



void HTML::set_colspan(int sp)
{
  n_colspan = sp;
  if (n_colspan <= 0) n_colspan = 1;
}

void HTML::set_rowspan(int sp)
{
  n_rowspan = sp;
  if (n_rowspan <= 0) n_rowspan = 1;
}


void HTML::print_td_st(const char *cl1, const char *fmt, ...)
{
  va_list argptr;
  char str[25500];

  va_start( argptr, fmt );
  vsprintf( str, fmt, argptr );

  if (cl1 && cl1[0]) {
    fprintf(f, "<td colspan=%d rowspan=%d class='%s'>%s</td>", n_colspan, n_rowspan, cl1, str);
  }
  else {
    fprintf(f, "<td colspan=%d rowspan=%d>%s</td>", n_colspan, n_rowspan, str);
  }

//  }
//  else {
//    fprintf(f, "<td>&nbsp;</td>");
//  }

  n_td ++;

  va_end( argptr );
}

void HTML::print_tr()
{
  is_tr = true;
  n_td = 0;
  n_tr ++;
  fprintf(f, "<tr>");
}

void HTML::print_tr2()
{
  if (!is_tr) return;

  for (; n_td < n_td0; ) {
    print_td("&nbsp;"); // E
  }
  fprintf(f, "</tr>\n");
  is_tr = false;
}

void HTML::print_head(const char *title)
{
  fprintf(f, "<html>\n");
  fprintf(f, "<head>\n");
  fprintf(f, "<title>%s</title>\n", title);
  fprintf(f, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\n");
  fprintf(f, "<meta name=Generator content=\"Microsoft Excel 14\">\n");
  fprintf(f, "<link rel=Stylesheet href=style.css>\n");
  fprintf(f, "</head>\n<body>\n");
}


void HTML::print_title(const char *title, int n)
{
  fprintf(f, "<table border=0>");
  print_tr();
  set_colspan(n);

  int old_bold = css->css.bold;
  css->css.bold = 1;
  print_td(title);;
  css->css.bold = old_bold;

  set_colspan(0);
  print_tr2();
  fprintf(f, "</table>");
}
