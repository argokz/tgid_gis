#include "stdafx.h"
#include "gid6.h"
#include "PotrCx.h"
#include "geofile.h"

#include "PropertyGrid\test\PropGridDlg.h"


bool CPropertyGrid::readIsmemenia(CIsmenenia &ism, int n)
{
  CString s;

  if (n == 0) {
        GetNameValue(transl_l(_TR("Тепловые потоки, Гкал/ч")),        ism.f1);
        GetNameValue(transl_l(_TR("В том числе отопление")),          ism.f2);
        GetNameValue(transl_l(_TR("В том числе вентиляция")),         ism.f3);
        GetNameValue(transl_l(_TR("В том числе ГВС макс")),           ism.f4);
        GetNameValue(transl_l(_TR("В том числе ГВС средн")),           ism.f_sr);
        GetNameValue(transl_l(_TR("Прирост нагрузки")),               ism.f5);
        GetNameValue(transl_l(_TR("В том числе прирост отопление")),  ism.f6);
        GetNameValue(transl_l(_TR("В том числе прирост вентиляция")), ism.f7);
        GetNameValue(transl_l(_TR("В том числе прирост ГВС макс")),   ism.f8);
        GetNameValue(transl_l(_TR("В том числе прирост ГВС средн")),   ism.f_pr_sr);
  }
  else {
       s.Format(_TR("Тепловые потоки, Гкал/ч %d"), n);        GetNameValue(transl_l(s), ism.f1);
       s.Format(_TR("В том числе отопление %d"), n);          GetNameValue(transl_l(s), ism.f2);
       s.Format(_TR("В том числе вентиляция %d"), n);         GetNameValue(transl_l(s), ism.f3);
       s.Format(_TR("В том числе ГВС макс %d"), n);           GetNameValue(transl_l(s), ism.f4);
       s.Format(_TR("В том числе ГВС средн %d"), n);          GetNameValue(transl_l(s), ism.f_sr);


       s.Format(_TR("Прирост нагрузки %d"), n);               GetNameValue(transl_l(s), ism.f5);
       s.Format(_TR("В том числе прирост отопление %d"), n);  GetNameValue(transl_l(s), ism.f6);
       s.Format(_TR("В том числе прирост вентиляция %d"), n); GetNameValue(transl_l(s), ism.f7);
       s.Format(_TR("В том числе прирост ГВС макс %d"), n);   GetNameValue(transl_l(s), ism.f8);
       s.Format(_TR("В том числе прирост ГВС средн %d"), n);  GetNameValue(transl_l(s), ism.f_pr_sr);
  }

  return true;
}


bool CPropertyGrid::checkTU(int n) 
{
#if 0
    CString s, s1, s2, s3, s4, s5, s6, s7, s8;
//  double f1, f2, f3, f4, f5, f6, f7, f8;
   CString s;
   CIsmenenia ism;

   readIsmemenia(ism, n);

   s1.Format(_TR("Тепловые потоки, Гкал/ч %d"), n);
   s5.Format(_TR("Прирост нагрузки %d"), n); 

//  if (fabs(ism.f1 -(ism.f2+ism.f3+ism.f4)) > 0.0001) {
  if (fabs(ism.f1 -(ism.f2+ism.f3+ism.f_sr)) > 0.0001) {
    FocusName(string(transl_l(s1)));
    s.Format(_TR("Сумма %d-х изменений тепловых потоков не соответствует заданным значениям! Все равно сохранить?"), n);
    if (AfxMessageBox(s,  MB_YESNO) != IDYES ) return false;
  }
//  if (fabs(ism.f5 -(ism.f6+ism.f7+ism.f8)) > 0.0001) {
  if (fabs(ism.f5 -(ism.f6+ism.f7+ism.f_pr_sr)) > 0.0001) {
    FocusName(string(transl_l(s5)));
    s.Format(_TR("Сумма %d-х изменений прироста не соответствует заданным значениям! Все равно сохранить?"), n);
    if (AfxMessageBox(s,  MB_YESNO) != IDYES ) return false;
  }
#endif
  return true;
}


bool CPropertyGrid::checkTU()
{
   if (AfxMessageBox(_TR("Сохранить изменения?"),  MB_YESNO) != IDYES ) return false;

   CString s;
   CIsmenenia ism;
   readIsmemenia(ism, 0);

//  if (fabs(ism.f1 -(ism.f2+ism.f3+ism.f4)) > 0.00001) {
  if (fabs(ism.f1 -(ism.f2+ism.f3+ism.f4)) > 0.00001) {
    s = _TR("Значение поля \"Тепловые потоки\" не соответствует значению: Gсум=Gот+Gв+Gгвс.макс ! Все равно сохранить?");
    FocusName(transl_l("Тепловые потоки, Гкал/ч"));
    if (AfxMessageBox(s,  MB_YESNO) != IDYES ) return false;
//    if (AfxMessageBox(_TR("Сумма тепловых потоков не соответствует заданным значениям! Все равно сохранить?"),  MB_YESNO) != IDYES ) return false;
  }
//  if (fabs(ism.f5 -(ism.f6+ism.f7+ism.f8)) > 0.00001) {
  if (fabs(ism.f5 -(ism.f6+ism.f7+ism.f8)) > 0.00001) {
    s = _TR("Значение поля \"Прирост нагрузки\" не соответствует значению: Gсум=Gот+Gв+Gгвс.макс ! Все равно сохранить?");
    FocusName(transl_l("Прирост нагрузки"));
    if (AfxMessageBox(s,  MB_YESNO) != IDYES ) return false;
//    if (AfxMessageBox(_TR("Сумма прироста не соответствует заданным значениям! Все равно сохранить?"),  MB_YESNO) != IDYES ) return false;
  }

  int sost;

  GetNameValue("sostoyanie_dogovora",        sost);

  if (sost == 4) {
      CString s;
      GetNameValue("data_vydachi_akta_dopuska",   s);

      if (s == "") {
          FocusName("data_vydachi_akta_dopuska");
          if (AfxMessageBox(_TR("Не указано значение поля Дата выдачи акта допуска ! Все равно сохранить?"),  MB_YESNO) != IDYES) {
              return false;
          }
      }
  }



  for (int i = 1; i <= 7; i++) {
    if (!checkTU(i)) return false;
  }

  return true;
}

// Проверяет, что это ТУ

bool CPropertyGrid::isTU()
{
    CItem *pItem = FindItemName("istochnik");
    if (pItem) {
        CString table = pItem->m_table;
        if (table.CompareNoCase("tehnicheskie_usloviya") == 0) {
            return true;
        }
    }
    return false;
}

void CPropertyGrid::copyVal(const char *tn, const char *pic, int n)
{
    CString fn1 = pic, fn2;
    if (n > 1) {
        fn1.Format("%s %d", pic, n - 1);
    }
    fn2.Format("%s %d", pic, n);

    fn1 = transl_l(fn1);
    fn2 = transl_l(fn2);

    CString val1, val2;

//    setEdit(dlg, fn2);

//    CString val1 = dlg->getValue(fn1);
//    CString val2 = dlg->getValue(fn2);

    GetNameValue(fn1, val1);
    GetNameValue(fn2, val2);


    if (val2 == "") {
        SetNameValue(fn2, val1);
//        dlg->setValue(tn, fn2, val1);
    }
}


int nIsmenenia(const char *s)
{
    char c = 0;
    CString tn = s;
    
    if (tn.GetLength() > 0) {
        c = tn[tn.GetLength() - 1];
    }

    int n = '0' <= c && c <= '9' ? c-'0' : 0;

    return n;
}

void CPropertyGrid::copyVal2(const char *fn1, const char *fn2)
{
  CString val1, val2;

  GetNameValue(transl_l(fn1), val1);
  GetNameValue(transl_l(fn2), val2);

  if (val2 == "") {
      SetNameValue(transl_l(fn2), val1);
  }
}


bool CPropertyGrid::copyVal3(const char *fn, const char *fn1, const char *pic, int n)
{
  CString fn2, s;
  fn2.Format("%s %d", pic, n);

  if (transl_l(fn) != transl_l(fn2)) return false;

  s.Format(_TR("Дата изменения продления %d"), n);

  CString d, val;
 
  GetNameValue(transl_l(s), d);
  GetNameValue(transl_l(fn2), val);
  SetNameValue(transl_l(fn1), val);

  return (d != "");
}

bool CPropertyGrid::change_value_tu(HITEM item)
{
    CItem *pItem = FindItem(item);
    if (!pItem) return false;

        int last_n = lastIsmenenia();
        
        int n = nIsmenenia(pItem->m_name);
        CString tn = pItem->m_name;

        if (tn.Find(transl_l(_TR("Дата изменения продления "))) == 0) {
            copyVal(tn, _TR("Тепловые потоки, Гкал/ч"), n);
            copyVal(tn, _TR("В том числе отопление"), n);
            copyVal(tn, _TR("В том числе вентиляция"), n);
            copyVal(tn, _TR("В том числе ГВС макс"), n);
            copyVal(tn, _TR("В том числе ГВС средн"), n);
            copyVal(tn, _TR("Прирост нагрузки"), n);
            copyVal(tn, _TR("В том числе прирост отопление"), n);
            copyVal(tn, _TR("В том числе прирост вентиляция"), n);
            copyVal(tn, _TR("В том числе прирост ГВС макс"), n);
            copyVal(tn, _TR("В том числе прирост ГВС средн"), n);
        }
        if (n > 0) {
            if (n == last_n) {
                copyVal3(tn, _TR("Тепловые потоки, Гкал/ч"), _TR("Тепловые потоки, Гкал/ч"), n);
                copyVal3(tn, _TR("В том числе отопление"), _TR("В том числе отопление"), n);
                copyVal3(tn, _TR("В том числе вентиляция"), _TR("В том числе вентиляция"), n);
                copyVal3(tn, _TR("В том числе ГВС макс"), _TR("В том числе ГВС макс"), n);
                copyVal3(tn, _TR("В том числе ГВС средн"), _TR("В том числе ГВС средн"), n);
                copyVal3(tn, _TR("Прирост нагрузки"), _TR("Прирост нагрузки"), n);
                copyVal3(tn, _TR("В том числе прирост отопление"), _TR("В том числе прирост отопление"), n);
                copyVal3(tn, _TR("В том числе прирост вентиляция"), _TR("В том числе прирост вентиляция"), n);
                copyVal3(tn, _TR("В том числе прирост ГВС макс"), _TR("В том числе прирост ГВС макс"), n);
                copyVal3(tn, _TR("В том числе прирост ГВС средн"), _TR("В том числе прирост ГВС средн"), n);
            }
            if (n > last_n) {
                if (transl_l(tn).Find(transl_l(_TR("Дата изменения продления"))) != 0) {
                    
                }
            }
        }
        if (transl_l(tn) == transl_l(_TR("Дата выдачи акта допуска"))) {
              copyVal2(_TR("Прирост нагрузки"), _TR("Тепловая нагрузка по акту допуска (проекту) Гкал/ч"));
              copyVal2(_TR("В том числе прирост отопление"), _TR("В том числе отопление по акту"));
              copyVal2(_TR("В том числе прирост вентиляция"), _TR("В том числе вентиляция по акту"));
              copyVal2(_TR("В том числе прирост ГВС макс"), _TR("В том числе ГВС макс по акту"));
              copyVal2(_TR("В том числе прирост ГВС средн"), _TR("В том числе ГВС средн по акту"));
        }

    return true;
}

bool CPropertyGrid::isIsmeneniaZero(int n)
{
    CIsmenenia ism;
    readIsmemenia(ism, n);

    CString s;
    CString s1, s2;

    s.Format("izmeneniya_prodleniya_%d", n);        GetNameValue(transl_l(s), s1);
    s.Format("data_izmeneniya_prodleniya_%d", n);   GetNameValue(transl_l(s), s2);

    GetNameValue(transl_l(""),     s1);
    GetNameValue(transl_l(""),     s2);


//    return s1 == "" && s2 == "";
    return s2 == "";


//    && ism.f1 == 0 && ism.f2 == 0 && ism.f3 == 0 && ism.f4 == 0 && ism.f5 == 0 && ism.f6 == 0 && ism.f7 == 0 && ism.f8 == 0;
}


int CPropertyGrid::lastIsmenenia()
{
    for (int n = 7; n >= 1; n--) {
      if (!isIsmeneniaZero(n)) return n;
    }
    return 0;
}


bool CPropertyGrid::check_openTU(CSection* pSection, bool msg) 
{
    CString name = pSection->m_title;

    if (name.Find(_TR("Изменения ")) == 0) {
        int n = nIsmenenia(name);

        if (n == 1 || !isIsmeneniaZero(n - 1)) {
            return true;
        }
        if (msg)  AfxMessageBox(_TR("Не заполнены значения предыдущих Изменений или дата введения Изменений"), MB_OK|MB_ICONINFORMATION);
        return false;
    }
    return true;
}



bool CPropertyGrid::isCanModTU(HITEM item)
{
    CItem *pItem = FindItem(item);
    if (pItem) {
        int last_n = lastIsmenenia();
        int n = nIsmenenia(pItem->m_name);

        if (n == 0) return true;

        CString s;
        CString d;

        s = pItem->m_name;
        if (s.Find("tehnicheskie_usloviya_") == 0) {
            return true;
        }

        s.Format(_TR("Дата изменения продления %d"), n);
        GetNameValue(transl_l(s), d);

        if (d == "" && transl_l(s) != transl_l(pItem->m_name)) {
            s.Format(_TR("Введите значение \"Дата изменения продления %d\" ТУ"), n);
            AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
            return false;
        }
        return true;
    }
    
    return true;
}
