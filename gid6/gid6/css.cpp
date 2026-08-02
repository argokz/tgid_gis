#include "stdafx.h"
#include "css.h"

CString CSS_CLASS::get_text() const
{
  CString str = "", s;

  if (bk_color != "") {
    s.Format("background:%s;\n", bk_color);
    str+=s;
  }

  if (color != "") {
    s.Format("color:%s;\n", color);
    str+=s;
  }

  if (font_family != "") {
    s.Format("font-family:%s;\n", font_family);
    str+=s;
  }

  if (font_size != "") {
    s.Format("font-size:%s;\n", font_size);
    str+=s;
  }

  
  if (text_align != "") {
    s.Format("text-align:%s;\n", text_align);
    str+=s;
  }

  if (vertical_align != "") {
    s.Format("vertical-align:%s;\n", vertical_align);
    str+=s;
  }
  
  if (mso_number_format != "") {
    s.Format("mso-number-format:%s;\n", mso_number_format);
    str+=s;
  }

  if (border) {
    s.Format("border:.5pt solid windowtext;\n");
    str+=s;
  }
  if (border_left) {
    s.Format("border-left:.5pt solid windowtext;\n");
    str+=s;
  }
  if (border_right) {
    s.Format("border-right:.5pt solid windowtext;\n");
    str+=s;
  }
  if (border_top) {
    s.Format("border-top:.5pt solid windowtext;\n");
    str+=s;
  }
  if (border_bottom) {
    s.Format("border-bottom:.5pt solid windowtext;\n");
    str+=s;
  }

  
  
  if (bold) {
    str += "font-weight:700;\n";
  }
  else {
    str += "font-weight:400;\n";
  }

  str += "vertical-align:top;\n";

  map<CString, CString>::const_iterator it = map_c.begin();

  for (; it != map_c.end(); it++) {
    if (it->second != "") {
      s.Format("%s:%s;", it->first, it->second);
      str+=s;
    }
  }
  

  return str;
}


bool CSS_CLASS::operator<(const CSS_CLASS &c) const
{
  CString s1, s2;
  s1 = get_text();
  s2 = c.get_text();

  if (s1 < s2) return true;
  if (s1 > s2) return false;

  return false;
}


CString CSS::init()
{
  CString s = "";

  set<CSS_CLASS>::const_iterator it = set_class.find(css);

  int n = 0;

  if (it == set_class.end()) {
    n_css++;
    css.m_nom = n_css;
    n = n_css;;
    set_class.insert(css);
  }
  else {
    n = it->m_nom;
  }
  s.Format("%s%d", prefix, n);

  return s;
}


void CSS::print(FILE *f) const
{
  set<CSS_CLASS>::const_iterator it = set_class.begin();

  for (;it != set_class.end(); it++) {
    CString name;
    CSS_CLASS css = *it;
    name.Format("%s%d", prefix, css.m_nom);

    CString s = css.get_text();

    fprintf(f, ".%s {\n", (LPCTSTR)name);
    fprintf(f, "%s", (LPCTSTR)s);
    fprintf(f, "}\n");
  }

  fclose(f);
}

void CSS::print(const char *fn) const
{
  FILE *f = fopen(fn, "w");
  if (f) print(f);
  fclose(f);
}


void CSS_CLASS::s(const char *p,  const char *v)
{
  map_c[p] = v;
}

void CSS_CLASS::s(const char *p)
{
  map_c.erase(p);
}
