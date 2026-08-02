#include "stdafx.h"
#include "shp_dbf.h"

CString GetFName(const char *path);


void long_swp(long &l)
{
  char *s, c;

  s = (char*)&l;

  c = s[0]; s[0] = s[3]; s[3] = c;
  c = s[1]; s[1] = s[2]; s[2] = c;
}

void short_swp(unsigned short &l)
{
  char *s, c;

  s = (char*)&l;

  c = s[0]; s[0] = s[1]; s[1] = c;
}



Cshp_dbf::~Cshp_dbf()
{
  fclose(fdbf);
  fclose(fshp);
  if (pb) delete [] pb;
  delete [] off_shx;
}


Cshp_dbf::Cshp_dbf(const char *fn)
{
  m_fn = fn;
  
  DBASE_FIELD field;

  pb = NULL;

  FILE *fx;

  SHP_HEAD hdx;

  fx = fopen(GetFName(fn)+".shx", "rb"); 

  if (!fx) {
    throw 0;
  }

  fread(&hdx, 100, 1, fx);
  long_swp(hdx.File_Code);
  long_swp(hdx.File_Length);

  int i;

  long len = (hdx.File_Length-50)/4;

  nrecs = len;

  off_shx = new long[len*2];

  fread(off_shx, len, 8, fx);
  for (i = 0; i < len*2; i++) {
    long_swp(off_shx[i]);
  }

  fclose(fx);

  fshp = fopen(GetFName(fn)+".shp", "rb"); 
  if ( !fshp ) {
    throw 0;
  }
  fread(&hd, sizeof(SHP_HEAD), 1, fshp);
  long_swp(hd.File_Code);
  long_swp(hd.File_Length);

  fdbf = fopen(GetFName(fn)+".dbf", "rb"); if ( !fdbf ) throw 0;
  fread(&head, sizeof(DBASE_HEAD), 1, fdbf);

  lrecl = head.lrecl;
//  nf = min (256, (head.header-1)/32-1);
  nf = (head.header-1)/32-1;


  for ( i = 0; i < nf; i++ ) {
    fread(&field, sizeof(DBASE_FIELD), 1, fdbf);
    m_field[i] = field;

    short type;

    switch (field.type) {
    case DB_FLD_CHAR  : type = dbText; break;
    case DB_FLD_FLOAT :
    case DB_FLD_NUM   : field.dec_point == 0 ? type = dbLong : type = dbDouble; break;
    case DB_FLD_LOGIC : type = dbBoolean; break;
    case DB_FLD_MEMO  : type = dbMemo; break;
    case DB_FLD_DATE  : type = dbDate; break;
    }

//    sprintf(name, "fdbf%d", i+1);
//    td.CreateField(name, type, field.length,  0);

    typf[i] = type;
    lenf[i] = field.length;
  }

  fseek(fdbf, head.header, 0);
}

int Cshp_dbf::first()
{
//  fseek(fdbf, head.header, 0);
  fseek(fshp, sizeof(SHP_HEAD), 0);
  return next();
}


int Cshp_dbf::next()
{
  int i, ll;

  char s[2560];
  char *buf = new char[lrecl];

  for (i = 0; i < nf; i++) lvar[i] = COleVariant();

  if (fread(buf, lrecl, 1, fdbf)) {
    for (i = 0, ll = 1; i < nf; i++) {
      strncpy(s, &buf[ll], lenf[i]);
      s[lenf[i]] = 0;
      ll += lenf[i];

      CString str = s;
      str.TrimLeft();
      str.TrimRight();
      if (!str.IsEmpty()) lvar[i] = CCrack::Var(str, typf[i]);
    }
  }
  else {
    delete [] buf;
    return 0;
  }
  delete [] buf;

  long n, len;

  fread(&n, sizeof(long), 1, fshp);
  long_swp(n);
  fread(&len, sizeof(long), 1, fshp);
  long_swp(len);

  if (feof(fshp)) return 0;

  if (pb) delete [] pb;
  pb = new char [2*len-4];
  fread(&loc, sizeof(long), 1, fshp);
  fread(pb, 2*len-4, 1, fshp);

  loc = loc%10;

  return 1;
}


int Cshp_dbf::next1()
{
  long n, len;

  fread(&n, sizeof(long), 1, fshp);
  long_swp(n);
  fread(&len, sizeof(long), 1, fshp);
  long_swp(len);

  if (len <= 2) return 0;

  if (feof(fshp)) return 0;

  if (pb) delete [] pb;
  pb = new char [2*len-4];
  fread(&loc, sizeof(long), 1, fshp);
  fread(pb, 2*len-4, 1, fshp);

  loc = loc%10;

  return 1;
}


int Cshp_dbf::read_coor()
{
  double x1, x2, y1, y2, x;
  long ind = 0;
  long i, n1, n2, n3, n4 = 0;
  COleVariant var;

  if (loc == 5 || loc == 3 || loc == 8) {
    x1 = *(double*)&pb[ind]; ind += sizeof(double);
    y1 = *(double*)&pb[ind]; ind += sizeof(double);
    x2 = *(double*)&pb[ind]; ind += sizeof(double);
    y2 = *(double*)&pb[ind]; ind += sizeof(double);

    n1 = *(long*)&pb[ind]; ind += sizeof(long);
    if (loc != 8) {
      n2 = *(long*)&pb[ind]; ind += sizeof(long);
      n3 = *(long*)&pb[ind]; ind += sizeof(long);
    }
    else {
      n2 = n1;
    }

    if (n2 > 0) {
      CString str = "", ss;

      for (i = 0; i < n2*2; i++) {
        x = *(double*)&pb[ind]; ind += sizeof(double);

        if ((i%2 && (x < y1 || x > y2)) || (i%2 == 0 && (x < x1 || x > x2))) {
          return FALSE;
        }

        ss.Format("%ld ", (long) (x * 100));
        str += ss;
      }
    }
  }
  return TRUE;
}

int Cshp_dbf::read_coor1(double &x, double &y)
{
  double x1, x2, y1, y2;
  long ind = 0;
  long n1;
  COleVariant var;

  x1 = *(double*)&pb[ind]; ind += sizeof(double);
  y1 = *(double*)&pb[ind]; ind += sizeof(double);
  x2 = *(double*)&pb[ind]; ind += sizeof(double);
  y2 = *(double*)&pb[ind]; ind += sizeof(double);

  n1 = *(long*)&pb[ind]; ind += sizeof(long);

  if (n1 > 0) {
    x = *(double*)&pb[ind]; ind += sizeof(double);
    y = *(double*)&pb[ind]; ind += sizeof(double);
  }
  x = x*100;  y = -y*100;
  return TRUE;
}

int Cshp_dbf::read_coor2(double &x1, double &y1, double &x2, double &y2, CCoordList &cl)
{
  long ind = 0;
  long i, n1, n2, n3, n4 = 0;
  COleVariant var;

  if (loc == 1) {
    x1 = x2 = *(double*)&pb[ind]; ind += sizeof(double);
    y1 = y2 = *(double*)&pb[ind]; ind += sizeof(double);
    x1 = x1*100;  y1 = -y1*100;
    x2 = x2*100;  y2 = -y2*100;
    cl.push_back(CFPoint(x1, y1));
    return TRUE;
  }


  ind += sizeof(double)*4;

  n1 = *(long*)&pb[ind]; ind += sizeof(long);
  if (loc != 8) {
    n2 = *(long*)&pb[ind]; ind += sizeof(long);
    n3 = *(long*)&pb[ind]; ind += sizeof(long);
  }
  else {
    n2 = n1;
  }

//  if (n1 != 1) return FALSE;
 

  if (n2 > 0) {
    ind = 0;

    x1 = *(double*)&pb[ind]; ind += sizeof(double);
    y1 = *(double*)&pb[ind]; ind += sizeof(double);
    x2 = *(double*)&pb[ind]; ind += sizeof(double);
    y2 = *(double*)&pb[ind]; ind += sizeof(double);

    n1 = *(long*)&pb[ind]; ind += sizeof(long);
    if (loc != 8) {
      n2 = *(long*)&pb[ind]; ind += sizeof(long);
      n3 = *(long*)&pb[ind]; ind += sizeof(long);
    }
    else {
      n2 = n1;
    }

    double x, y;

    for (i = 0; i < n2; i++) {
      x = *(double*)&pb[ind]; ind += sizeof(double);
      y = *(double*)&pb[ind]; ind += sizeof(double);

      double dx = 1;

      if (x < x1-dx || x > x2+dx || y < y1-dx || y > y2+dx) {
        return FALSE;
//        continue;
      }

      cl.push_back(CFPoint(x*100, -y*100));
    }
  }

  CFRect rect = cl.GetFRect();

  x1 = x1*100;  y1 = -y1*100;
  x2 = x2*100;  y2 = -y2*100;

  if (n2 > 100000) {
    x1 = rect.left;
    x2 = rect.right;
    y1 = rect.top;
    y2 = rect.bottom;
  }
  return TRUE;
}


int Cshp_dbf::read_coor_xy(double &x1, double &y1, double &x2, double &y2)
{
  long ind = 0;
  long n1, n2, n3, n4 = 0;
  COleVariant var;


  if (loc == 1) {
    x1 = x2 = *(double*)&pb[ind]; ind += sizeof(double);
    y1 = y2 = *(double*)&pb[ind]; ind += sizeof(double);
    x1 = x1*100;  y1 = -y1*100;
    x2 = x2*100;  y2 = -y2*100;
    return TRUE;
  }

  ind += sizeof(double)*4;

  n1 = *(long*)&pb[ind]; ind += sizeof(long);
  if (loc != 8) {
    n2 = *(long*)&pb[ind]; ind += sizeof(long);
    n3 = *(long*)&pb[ind]; ind += sizeof(long);
  }
  else {
    n2 = n1;
  }

//  if (n1 != 1) return FALSE;
 
  if (n2 > 0) {
    ind = 0;

    x1 = *(double*)&pb[ind]; ind += sizeof(double);
    y1 = *(double*)&pb[ind]; ind += sizeof(double);
    x2 = *(double*)&pb[ind]; ind += sizeof(double);
    y2 = *(double*)&pb[ind]; ind += sizeof(double);

    n1 = *(long*)&pb[ind]; ind += sizeof(long);
    if (loc != 8) {
      n2 = *(long*)&pb[ind]; ind += sizeof(long);
      n3 = *(long*)&pb[ind]; ind += sizeof(long);
    }
    else {
      n2 = n1;
    }
    x1 = x1*100;  y1 = -y1*100;
    x2 = x2*100;  y2 = -y2*100;

    return TRUE;
  }
  return FALSE;

}


int Cshp_dbf::field_number(const char *fn)
{
  int i;
  CString str1, str2;
  str1 = fn;
  str1.MakeUpper();

  for ( i = 0; i < nf; i++ ) {
    str2 = m_field[i].name;
    str2.MakeUpper();
    if (str1 == str2) return i;
  }

  return -1;
}

int Cshp_dbf::go_to(long n)
{
  int len = sizeof(DBASE_FIELD);

  fseek(fdbf, sizeof(DBASE_FIELD)+1+nf*32+(n-1)*(lrecl), 0);
  fseek(fshp, off_shx[(n-1)*2]*2, 0);
	return next();
}

int Cshp_dbf::go_to_shp(long n)
{
  fseek(fshp, off_shx[(n-1)*2]*2, 0);
	return next1();
}

BOOL Cshp_dbf::FieldName(int fH, char *s) {
  lstrcpy(s, m_field[fH-1].name);
  return true;
}

COleVariant Cshp_dbf::read(const char *fn)
{
  int i = field_number(fn);
	if (i >= 0) return read(i);

  return COleVariant();
}
