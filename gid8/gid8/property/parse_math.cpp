#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "PropertyDial.h"


//#include "stdafx.h"

#include <sys/types.h>

//#include "PropertyGrid\test\PropGridDlg.h"

//#define PCRE_STATIC

//#include "pcre-7.0/pcreposix.h"
//#include "pcre-7.0/pcre_internal.h"
//#include "pcre-7.0/pcre.h"

//int regex_match(const char *p, const char *buf, vector<QString> & match);


//#include <regex.h>
#include <stdio.h>
 
typedef struct {
  const char *s;
  int len, prec, assoc;
} str_tok_t;
 
typedef struct {
  const char * str;
  int assoc, prec;
  regex_t re;
} pat_t;
 
enum assoc { A_NONE, A_L, A_R };
pat_t pat_eos = {"", A_NONE, 0};
 
pat_t pat_ops[] = {
  {"^\\)",  A_NONE, -1},
  {"^\\*\\*", A_R, 3},
  {"^\\^",  A_R, 3},
  {"^\\*",  A_L, 2},
  {"^/",    A_L, 2},
  {"^\\+",  A_L, 1},
  {"^-",    A_L, 1},
  {0}
};
 
pat_t pat_arg[] = {
  {"^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?"},
  {"^[a-zA-Z_][a-zA-Z_0-9]*"},
  {"^\\(", A_L, -1},
  {0}
};
 
str_tok_t stack1[256]; /* assume these are big enough */

double stack2[256];


str_tok_t queue1[256];
int l_queue, l_stack;
#define qpush(x) queue1[l_queue++] = x
#define spush(x) stack1[l_stack++] = x
#define spop()   stack1[--l_stack]

#define spush2(x) stack2[l_stack2++] = x
#define spop2()   stack2[--l_stack2]


void display(const char *s)
{
  return;
  int i;

  QString s1, ss;

  s1.Format("\033[1;1H\033[JText | %s", s);
  ss += s1;
  s1.Format("\nStack| ");
  ss += s1;
  for (i = 0; i < l_stack; i++) {
      s1.Format("%.*s ", stack1[i].len, stack1[i].s); // uses C99 format strings
      ss += s1;
  }
  s1.Format("\nQueue| ");
  ss += s1;
  for (i = 0; i < l_queue; i++) {
      s1.Format("%.*s ", queue1[i].len, queue1[i].s);
      ss += s1;
  }
  AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);
//  puts("\n\n<press enter>");
//  getchar();
}

bool is_number(const char *s, double &d)
{
    d = 0;
    vector<QString> match;
    int l = regex_match("^-?[0-9]+\\.?[0-9]*$", s, match);

    if (l > 0) {
        d = atof(s);
        return true;
    }
    return false;
}

bool is_perem(const char *s)
{
    vector<QString> match;
    int l = regex_match("^[A-Za-z_][A-Za-z0-9_]*$", s, match);

    if (l > 0) {
        return true;
    }
    return false;
}

bool is_bin_op(QString s)
{
    if (s == "+" || s == "-" || s == "*" || s == "/") {
        return true;
    }
    return false;
}



double get_val(CPropertyGrid *wnd)
{
  int i;
  double d;

  QString s1, ss;

  int l_stack2 = 0;

  for (i = 0; i < l_queue; i++) {
      s1 = queue1[i].s;
      s1 = s1.Left(queue1[i].len);

      double d;

      if (is_bin_op(s1)) {
          double d2 = spop2();
          double d1 = spop2();
          d = 0;

          if (s1 == "+") {
              d = d1 + d2;
          }
          else if (s1 == "-") {
              d = d1 - d2;
          }
          else if (s1 == "*") {
              d = d1 * d2;
          }
          else if (s1 == "/") {
              d = d1 / d2;
          }
          spush2(d);
      }
      else if (s1 == "coef24") {
          d = atof(AfxGetApp()->GetProfileString(szSection, "coef24", "2.4"));
          spush2(d);
      }
      else if (is_number(s1, d)) {
          spush2(d);
      }
      else if (is_perem(s1)) {
          wnd->GetNameValue(s1, d);
          spush2(d);
      }
  }
  d = spop2();
  
  return d;

//  ss.Format("d = %g", d);

//  AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);
//  puts("\n\n<press enter>");
//  getchar();
}

 
int prec_booster;
 
#define fail(s1, s2) {fprintf(stderr, "[Error %s] %s\n", s1, s2); return 0;}
 
int init(void)
{
  int i;
  pat_t *p;
 
  for (i = 0, p = pat_ops; p[i].str; i++)
    if (regcomp(&(p[i].re), p[i].str, REG_NEWLINE|REG_EXTENDED))
      fail("comp", p[i].str);
 
  for (i = 0, p = pat_arg; p[i].str; i++)
    if (regcomp(&(p[i].re), p[i].str, REG_NEWLINE|REG_EXTENDED))
      fail("comp", p[i].str);
 
  return 1;
}
 
pat_t* match(const char *s, pat_t *p, str_tok_t * t, const char **e)
{
  int i;
  regmatch_t m;
 
  while (*s == ' ') s++;
  *e = s;
 
  if (!*s) return &pat_eos;
 
  for (i = 0; p[i].str; i++) {
    if (regexec(&(p[i].re), s, 1, &m, REG_NOTEOL))
      continue;
    t->s = s;
    *e = s + (t->len = m.rm_eo - m.rm_so);
    return p + i;
  }
  return 0;
}
 
int parse(const char *s) 
{
  pat_t *p = NULL;
  str_tok_t *t, tok;
 
  prec_booster = l_queue = l_stack = 0;
  display(s);
  while (*s) {
    p = match(s, pat_arg, &tok, &s);
    if (!p || p == &pat_eos) fail("parse arg", s);
 
    /* Odd logic here. Don't actually stack1 the parens: don't need to. */
    if (p->prec == -1) {
      prec_booster += 100;
      continue;
    }
    qpush(tok);
    display(s);
 
re_op:    p = match(s, pat_ops, &tok, &s);
    if (!p) fail("parse op", s);
 
    tok.assoc = p->assoc;
    tok.prec = p->prec;
 
    if (p->prec > 0)
      tok.prec = p->prec + prec_booster;
    else if (p->prec == -1) {
      if (prec_booster < 100)
        fail("unmatched )", s);
      tok.prec = prec_booster;
    }
 
    while (l_stack) {
      t = stack1 + l_stack - 1;
      if (!(t->prec == tok.prec && t->assoc == A_L)
          && t->prec <= tok.prec)
        break;
      qpush(spop());
      display(s);
    }
 
    if (p->prec == -1) {
      prec_booster -= 100;
      goto re_op;
    }
 
    if (!p->prec) {
      display(s);
      if (prec_booster)
        fail("unmatched (", s);
      return 1;
    }
 
    spush(tok);
    display(s);
  }
 
  if (p->prec > 0)
    fail("unexpected eol", s);
 
  return 1;
}


bool parse_math(CPropertyGrid *wnd, const char *ss, double &d)
{
    if (!init()) return false;
    parse(ss);
    d = get_val(wnd);
    return true;
}
