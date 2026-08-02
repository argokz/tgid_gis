#if !defined(_CSS_H)
#define _CSS_H

class CSS_CLASS
{
  map<CString, CString> map_c;
public:
  CSS_CLASS()
  {
    color = "";
    bk_color = "";
    bold = 0;
    border = 0;

    font_family = "";
    font_size = "";

    border_left = 0;
    border_right = 0;
    border_top = 0;
    border_bottom = 0;
    
    text_align = "left";
    vertical_align = "justify";
    mso_number_format = "";
  }

  int m_nom;
  CString color;
  CString bk_color;
  int bold;
  int border;

  int border_left;
  int border_right;
  int border_top;
  int border_bottom;

  CString font_family;
  CString font_size;
  CString text_align;
  CString vertical_align;
  CString mso_number_format;

  void s(const char *p,  const char *v);
  void s(const char *p);
  CString get_text() const;
  bool operator<(const CSS_CLASS &c) const;
};

class CSS
{
  set<CSS_CLASS> set_class;
  int n_css;
  CString prefix;

public:
  CSS_CLASS css;
  
  CSS() {
    n_css = 0;
    prefix = "xx";
  }

  CString init();
  void print(FILE *f) const;
  void print(const char *fn) const;
};


#endif

