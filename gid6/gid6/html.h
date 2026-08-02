#pragma once

#include "css.h"

class HTML {
  int n_td;
  int n_yellow;
  int n_td0;
  int n_colspan;
  int n_rowspan;
  bool is_tr;

public :
  CSS *css;
  FILE *f;
  int n_tr;
    
    
  HTML(FILE *f1, CSS *_css);
  ~HTML();
  void set_tr(int n);
  void set_colspan(int sp);
  void set_rowspan(int sp);
  void print_td(double d);
  void print_td();
  void print_td(const char *fmt, ...);
  void print_td_st(const char *cl1, double d);
  void print_td_st(const char *cl1, const char *fmt, ...);
  void print_tr();
  void print_tr2();

  void print_head(const char *title);
  void print_title(const char *title, int n);

};

