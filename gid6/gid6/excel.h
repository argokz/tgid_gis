#ifndef __EXCEL_H__
#define __EXCEL_H__

const int xlAlignCenter = -4108;

void ShowExcel();


struct CDop
{
    int r, c;
    CString name;
};


class CRange
{
public:
  CRange() : r1(0), c1(0), r2(0), c2(0) {}
  CRange(int r, int c) : r1(r), c1(c), r2(r), c2(c)  {}
  CRange(int _r1, int _c1, int _r2, int _c2) : r1(_r1), c1(_c1), r2(_r2), c2(_c2)  {}
  CRange(const char *a);
  CRange(CString a) { CRange((const char *)a); };

  int r1, c1, r2, c2;
};



class CExcel 
{
public:
  CExcel() {
    isOpen = false;
    pXlBooks = 0;
    pXlBook = 0;
    pXlApp = 0;
    row1 = col1 = 999999;
    row2 = col2 = 0;
    row = col = 1;
    wait = new CWaitCursor();
  }
  ~CExcel();
  int open(const char *path);
  int close();
  int example2(CRange r, int ntab);
  int init_arr();
  bool IsOpen() { return isOpen;};
    
  int init_arr(int _row, int _col);
  int set_arr(CRange r, COleVariant & var);
  int set_arr2(CRange r, COleVariant & var);

  int set_arr_str(CRange r, CString str);


  int set_var(CRange r, COleVariant & var);
  int set_typ(CRange r, int typ);
  int set_typ2(CRange r, int typ);


  int set_str(CRange r, CString str);

  int copy(CRange r, int ntab);

  int setTab(int ntab);
  int property(CRange r);

  void set_range(CRange r);

  int set_v_align(CRange r, int typ);
  int set_h_align(CRange r, int typ);
  int set_hv_align(CRange r, int typH, int typV);
  int set_border(CRange r, int typ);
  int set_val(CRange r, const wchar_t *field, COleVariant &val);
  int set_bold(CRange r);


  IDispatch *getRange(int r1, int c1, int r2, int c2);
//  IDispatch *getCell(int r1, int c1);

public:
  int row1, col1, row2, col2;


private:
  bool isOpen;

  IDispatch *pXlSheet;
  IDispatch *pXlBooks;
  IDispatch *pXlBook;
  IDispatch *pXlApp;
  VARIANT arr;
  int row, col;

  CWaitCursor *wait;

};


#endif
