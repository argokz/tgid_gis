#include "stdafx.h"
#include "gid6.h"
#include "PotrCx.h"
#include "geofile.h"

#include "PropertyGrid\test\PropGridDlg.h"

int CPropertyGrid::prop_fun2(int id)
{
    COleDateTime dt;
    int idS;

    GetNameValue(transl_l("sostoyanie_dogovora"), idS);

    switch (id) {
    case IDC_A :
      if (idS == 1 && AfxMessageBox(_TR("Анyллировать техническое условие?"), MB_YESNO) == IDYES) {
        GetNameValue(transl_l(_TR("Дата аннулирования")), dt);
        SetNameValue(transl_l("sostoyanie_dogovora"), 2);
        if (dt.m_dt == 0) {
//        if (dt.GetStatus() == COleDateTime::null) {
            FocusName(transl_l(_TR("Дата аннулирования")));
            AfxMessageBox(_TR("Введите дату операции"), MB_OK|MB_ICONINFORMATION);
        }
        else {
          SetNameValue(transl_l("sostoyanie_dogovora"), 2);
        }
      }

      break;
    case IDC_Z :
      if (idS == 1 && AfxMessageBox(_TR("Завершить техническое условие?"), MB_YESNO) == IDYES) {
          GetNameValue(transl_l(_TR("Дата выдачи акта допуска")), dt);

          SetNameValue(transl_l("sostoyanie_dogovora"), 4);
          
          if (dt.m_dt == 0) {
//          if (dt.GetStatus() == COleDateTime::null) {
              FocusName(transl_l(_TR("Дата выдачи акта допуска")));
              AfxMessageBox(_TR("Введите дату операции"), MB_OK|MB_ICONINFORMATION);
          }
          else {
            SetNameValue(transl_l("sostoyanie_dogovora"), 4);

            double f;
            const char *pic = _TR("Дата изменения продления");

            for (int n = 7; n >= 1; n--) {
                CString fn;
                fn.Format("%s %d", pic, n);

                GetNameValue(transl_l(fn), dt);

                if (dt.m_dt == 0) {
//                if (dt.GetStatus() == COleDateTime::null) {
                    CString tn;
                    tn.Format(_TR("Прирост нагрузки %d"), n);
                    GetNameValue(transl_l(tn), f);
                    SetNameValue(transl_l(_TR("Тепловая нагрузка по акту допуска (проекту Гкал/ч")), f);

                    tn.Format(_TR("В том числе прирост отопление %d"), n);
                    GetNameValue(transl_l(tn), f);
                    SetNameValue(transl_l(_TR("В том числе отопление по акту")), f);

                    tn.Format(_TR("В том числе прирост вентиляция %d"), n);
                    GetNameValue(transl_l(tn), f);
                    SetNameValue(transl_l(_TR("В том числе вентиляция по акту")), f);

                    tn.Format(_TR("В том числе прирост ГВС макс %d"), n);
                    GetNameValue(transl_l(tn), f);
                    SetNameValue(transl_l(_TR("В том числе ГВС макс по акту")), f);
                    goto QUIT;
                }
            }
            GetNameValue(transl_l(_TR("Прирост нагрузки")), f);
            SetNameValue(transl_l(_TR("Тепловая нагрузка по акту допуска (проекту Гкал/ч")), f);

            GetNameValue(transl_l(_TR("В том числе прирост отопление")), f);
            SetNameValue(transl_l(_TR("В том числе отопление по акту")), f);

            GetNameValue(transl_l(_TR("В том числе прирост вентиляция")), f);
            SetNameValue(transl_l(_TR("В том числе вентиляция по акту")), f);

            GetNameValue(transl_l(_TR("В том числе прирост ГВС макс")), f);
            SetNameValue(transl_l(_TR("В том числе ГВС макс по акту")), f);
          }
        }
        break;
    case IDC_D :
        SetNameValue(transl_l("sostoyanie_dogovora"), 1);
        if (idS != 1 && AfxMessageBox(_TR("Тепловая нагрузка по акту допуска будут обнулены."), MB_YESNO) == IDYES) {
//            SetNameValue(transl_l(_TR("Дата выдачи акта допуска")), );
            SetNameValue(transl_l(_TR("Тепловая нагрузка по акту допуска (проекту Гкал/ч")), 0.);
            SetNameValue(transl_l(_TR("В том числе отопление по акту")), 0.);
            SetNameValue(transl_l(_TR("В том числе вентиляция по акту")), 0.);
            SetNameValue(transl_l(_TR("В том числе ГВС макс по акту")), 0.);
            SetNameValue(transl_l("sostoyanie_dogovora"), 1);
        }
        break;
    }

QUIT:
    return 0;
}
