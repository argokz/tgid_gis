#include "stdafx.h"
#include "gid6.h"
#include "PotrCx.h"
#include "geofile.h"
#include "CGidResist.h"
#include "win.h"
#include "gidrView.h"

#include "PropertyGrid\test\PropGridDlg.h"

void ind_reset(CPropertyGrid* wnd)
{
    wnd->SetNameValue("regimID", 1);
    wnd->SetNameValue("sostoyanie", 4);

    wnd->SetNameNull("data_planirovaniya");
    wnd->SetNameNull("data_ustanovki");
    wnd->SetNameNull("data_izvlecheniya");

    wnd->SetNameNull("sredniy_ves_plastiny_posle_ispytaniy__g");
    wnd->SetNameNull("poterya_massy_srednyaya_pri_kislotnoy_obraboke__g");
    wnd->SetNameNull("srednyaya_skorost_korrozii__mm_god");
    wnd->SetNameNull("otsenka_korrozionnogo_protsessa");
    wnd->SetNameNull("agressivnost_setevoy_vody");
}

void indicatorA1(CPropertyGrid* wnd)
{
    double msr1 = 0, msr2 = 0;

    wnd->GetNameValue("sredniy_ves_plastiny_pri_ustanovke__g", msr1);
    wnd->GetNameValue("sredniy_ves_plastiny_posle_ispytaniy__g", msr2);

//    wnd->SetNameValue("poterya_massy_srednyaya_pri_kislotnoy_obraboke__g", msr1 - msr2);
}

void indicatorA2(CPropertyGrid* wnd)
{
    COleDateTime d_cur = COleDateTime::GetCurrentTime();
    double Rsr = 0;
    double rsr = 0;
    double Dsr = 0;

    int N = 0;
    double msr1 = 0;
    double M1 = 0;        // Средняя одной пластины до исп
    double msr2 = 0;

    double M2 = 0;  // Средняя одной пластины после
    double dm = 0; // Потеря массы средняя при кислотной обрабоке, г

    COleDateTime d1, d2;
    CString s7, s8;
    string ss7, ss8;

    int stateIndID = 1, sostoyanie;

    wnd->GetNameValue("sostoyanie", sostoyanie);
    wnd->GetNameValue("stateIndID", stateIndID);

    HITEM item = wnd->FindHItemName("sredniy_ves_plastiny_posle_ispytaniy__g");
    if (item) {
//        wnd->SetItemReadOnly(item, stateIndID == 2 || sostoyanie != 3);
    }

    if (stateIndID == 2) {
        wnd->SetNameNull("sredniy_ves_plastiny_posle_ispytaniy__g");
        wnd->SetNameNull("poterya_massy_srednyaya_pri_kislotnoy_obraboke__g");
        wnd->SetNameNull("srednyaya_skorost_korrozii__mm_god");
        wnd->SetNameNull("otsenka_korrozionnogo_protsessa");
        wnd->SetNameNull("agressivnost_setevoy_vody");
        return;
    }
    

    wnd->GetNameValue("radius_krugloy_plastiny__mm", Rsr);
    wnd->GetNameValue("tolschina_plastiny__mm", Dsr);
    wnd->GetNameValue("kolichestvo_plastin_v_sborke", N);
    wnd->GetNameValue("sredniy_ves_plastiny_pri_ustanovke__g", msr1);
    wnd->GetNameValue("sredniy_ves_plastiny_posle_ispytaniy__g", msr2);
    wnd->GetNameValue("poterya_massy_srednyaya_pri_kislotnoy_obraboke__g", dm);

    CString msr2s = "";

    wnd->GetNameValue("sredniy_ves_plastiny_posle_ispytaniy__g", msr2s);

    if (msr2s == "") return;


    bool is_d1 = wnd->GetNameValue("data_ustanovki", d1);
    bool is_d2 = wnd->GetNameValue("data_izvlecheniya", d2);


    wnd->GetNameValue("radius_vtulki__mm", rsr);

    M1 = msr1;        // Средняя одной пластины до исп
    M2 = msr2;  // Средняя одной пластины после

/*

    if (!is_d1) {
        AfxMessageBox(_TR("Дата установки не задана"), MB_OK|MB_ICONINFORMATION);
        return;
    }
    if (!is_d2) {
        AfxMessageBox(_TR("Дата извлечения не задана"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    if (d1 > d_cur) {
        AfxMessageBox(_TR("Дата установки должна быть раньше текущей даты"), MB_OK|MB_ICONINFORMATION);
        return;
    }


    if (d2 > d_cur) {
        AfxMessageBox(_TR("Дата извлечения должна быть раньше текущей даты"), MB_OK|MB_ICONINFORMATION);
        return;
    }


    int dt = d2 - d1; // Время испытаний, сут

    if (dt <= 0) {
        AfxMessageBox(_TR("Дата извлечения должна быть позже даты установки"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    if (M1 - M2 <= 0) {
        CString str;
        str.Format(_TR("Ошибка\n%g < %g\nСредний вес пластин увеличился"), M1, M2);
        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
        return;
    }
*/

    int dt = d2 - d1; // Время испытаний, сут

    double Scp = 2 * M_PI * (Rsr * Rsr + Rsr * Dsr - rsr * rsr); // Средняя поверхность 1 пластины

    double k = (M1 - M2 - dm)*1000000/(Scp*dt); // интенсивность коррозии
//    double k = (M1 - M2) * 1000000 / (Scp * dt); // интенсивность коррозии

    double P = 0.047 * k; // средняя скорость коррозии мм/год

//    real1.Format("%g", P);
//    wnd->SetItemValue(item, P);
    wnd->SetNameValue("srednyaya_skorost_korrozii__mm_god", P);

    //    static int getValKorroz(double d, const char *tn)
    int real2 = 0, real3 = 0;

    if (P <= 0.02) real2 = 1;
    else if (P <= 0.04) real2 = 2;
    else if (P <= 0.05) real2 = 3;
    else if (P <= 0.2)  real2 = 4;
    else                real2 = 5;

    if (P <= 0.03)  real3 = 1;
    else if (P <= 0.085) real3 = 2;
    else if (P <= 0.2)   real3 = 3;
    else                 real3 = 4;

    //    real4.Format("%f", M2-M1);
    wnd->SetNameValue("otsenka_korrozionnogo_protsessa", real2);
    wnd->SetNameValue("agressivnost_setevoy_vody", real3);


//    AfxMessageBox(_TR("Выполнено"), MB_OK|MB_ICONINFORMATION);
}



int isKorrozDate(CString fun)
{
    if (fun == "indikator_korrozii_data_planirovaniya") {
        return 1;
    }
    else if (fun == "indikator_korrozii_data_ustanovki") {
        return 2;
    }
    else if (fun == "indikator_korrozii_data_izvlecheniya") {
        return 3;
    }

    return 0;
}

#include "ado.h"

bool isDublNode(CAdoFile *ado, int id, int externalCodeID, CString externalNodeName)
{
    CString q;
    q.Format("SELECT * FROM nodes n WHERE n.removed=0 AND n.externalNodeName = '%s' AND n.id < > %d AND n.externalCodeID = %d", externalNodeName, id, externalCodeID);

    int dubl = false;
   
    if (ado->openTable0(q)) {
		while (!ado->isEOF()) {
            dubl = true;
            ado->MoveNext();
		}
		ado->closeTable();
	}
    return dubl;
}


bool isDublNode0(int id, int externalCodeID, CString externalNodeName)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return false;

    CString q;
    q.Format("SELECT * FROM nodes n WHERE n.removed=0 AND n.externalNodeName = '%s' AND n.id < > %d AND n.externalCodeID = %d", externalNodeName, id, externalCodeID);

    int dubl = false;
   
    if (ado->openTable0(q)) {
		while (!ado->isEOF()) {
            dubl = true;
            ado->MoveNext();
		}
		ado->closeTable();
	}
    return dubl;
}



bool isDubl(CAdoFile* ado, const char* tn, const char* fn, int id, const char* value, CString& fragment)
{
    CString q;
    q.Format("SELECT DISTINCT fr.name FROM [%s] t LEFT JOIN fragments fr ON fr.id=t.fileID WHERE t.id <> %d AND t.%s='%s' AND fr.removed=0", tn, id, fn, value);

    int dubl = false;

    fragment = "";

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            CString ss = ado->readStr(0);
            if (fragment != "") fragment += "\n";
            fragment += ss;
            dubl = true;
            ado->MoveNext();
        }
        ado->closeTable();
    }

    return dubl;

}




int CPropertyGrid::prop_after(CString fun, HITEM item)
{
    CString strValue;

    GetItemValue(item, strValue);

     CAdoFile* ado = getAdo(getPsAdoName());
     if (!ado) return 0;


    CString getAlsecoTxt(CString mkr2, CString str2, CString dom2, double otop, double vent, double gvs, double par);

     CItem* pItem = FindItem(item);
     if (pItem) {
         if (fun == "alseco_text") {
            CString mkr1, str1, dom1;
            CString mkr2, str2, dom2;
            double otop, gvs, vent, par;

            GetNameValue("id_adr_mas", mkr1);
            GetNameValue("street_nam", str1);
            GetNameValue("number_1", dom1);
            GetNameValue("mkr2", mkr2);
            GetNameValue("street2", str2);
            GetNameValue("house2", dom2);
            GetNameValue("otop", otop);
            GetNameValue("gvs", gvs);
            GetNameValue("vent", vent);
            GetNameValue("par", par);

            if (mkr2 == "") mkr2 = mkr1;
            if (str2 == "") str2 = str1;
            if (dom2 == "") dom2 = dom1;

            CString txt = getAlsecoTxt(mkr2, str2, dom2, otop, vent, gvs, par);
            
            SetNameValue("txt", txt);
            return 0;
         }


         if (fun == "externalCodes_name") {
             CString fragment;
             if (isDubl(ado, "externalCodes", "name", pItem->m_id_bd, strValue, fragment)) {
                 CString s;
                 s.Format("Расчетная схема с названием %s есть во фрагментах: \n %s", strValue, fragment);
                 AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
             }
             return 0;
         }

         if (fun == "noNodeDuble") {
             CString fragment, kod;
             int externalCodeID;

             GetNameValue("externalCodeID", externalCodeID);
             GetNameValue("externalCodeID", kod);

             if (isDublNode(ado, pItem->m_id_bd, externalCodeID, strValue)) {
                 CString s;
                 s.Format("Узел %s %s уже существует", kod, strValue);
                 AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
             }
             return 0;
         }

         if (fun == "graphTypeID") {
//            AfxMessageBox(pItem->m_strValue.c_str());
              CString s = CString("Температурный график (") + pItem->m_strValue + CString(")");
//              GetParent()->SetWindowText(s);
              GetParent()->GetParent()->SetWindowText(s);
            return 0;
         }


         if (fun == "set_defect_remont_potrbiteli") {
            CGidrView* view = getView();
            if (view) {
                view->setDefectForRemonts();
            }
            return 0;
         }

         if (fun == "set_defect_potrbiteli") {
            CGidrView* view = getView();
            if (view) {
                view->setDefect();
            }
            return 0;
         }
     }


     if (fun == "indicator") {
         indicatorA1(this);
         indicatorA2(this);
    }

    if (isKorrozDate(fun)) {
        int regimID = 1;


/*

        GetNameValue("regimID",regimID);

        if (fun == "indikator_korrozii_data_planirovaniya") {
            if (strValue != "") {
                if (regimID == 1) {
                    SetNameValue("regimID", 2);
                }
            }
        }
        else if (fun == "indikator_korrozii_data_ustanovki") {
            if (strValue != "") {
                if (regimID == 2) {
                    SetNameValue("regimID", 3);
                    SetNameValue("sostoyanie", 2);
                }
                else if (regimID == 3) {
                    SetNameValue("regimID", 4);
                    SetNameValue("sostoyanie", 2);
                }
                indicatorA1(this);
                indicatorA2(this);
            }
        }
        else if (fun == "indikator_korrozii_data_izvlecheniya") {
            if (strValue != "") {
                SetNameValue("regimID", 5);
                SetNameValue("sostoyanie", 3);
                indicatorA1(this);
                indicatorA2(this);
            }
        }
*/

/*
        GetNameValue("regimID",regimID);

        if (fun == "indikator_korrozii_data_planirovaniya") {
            if (strValue != "") {
                if (regimID == 1) {
                    SetNameValue("regimID", 2);
                    SetNameValue("sostoyanie", 1);
                }
            }
        }
        else if (fun == "indikator_korrozii_data_ustanovki") {
            if (strValue != "") {
                if (regimID == 2) {
                    SetNameValue("regimID", 3);
                    SetNameValue("sostoyanie", 2);
                }
                else if (regimID == 3) {
                    SetNameValue("regimID", 4);
                    SetNameValue("sostoyanie", 2);
                }
                indicatorA1(this);
                indicatorA2(this);
            }
        }
        else if (fun == "indikator_korrozii_data_izvlecheniya") {
            if (strValue != "") {
                SetNameValue("regimID", 5);
                SetNameValue("sostoyanie", 3);
                indicatorA1(this);
                indicatorA2(this);
            }
        }
    }
*/
        
        int sostoyanie = 4;

        GetNameValue("sostoyanie",sostoyanie);

        if (fun == "indikator_korrozii_data_planirovaniya") {
            if (strValue != "") {
                if (sostoyanie == 4) {
                    SetNameValue("sostoyanie", 1);
                }
            }
        }
        else if (fun == "indikator_korrozii_data_ustanovki") {
            if (strValue != "") {
                if (sostoyanie == 1) {
                    SetNameValue("sostoyanie", 2);
                }
                indicatorA1(this);
                indicatorA2(this);
            }
        }
        else if (fun == "indikator_korrozii_data_izvlecheniya") {
            if (strValue != "") {
                SetNameValue("sostoyanie", 3);
                indicatorA1(this);
                indicatorA2(this);
            }
        }
    }

    if (fun == "set_remont") {
      AfxMessageBox("!!");
    }
    
    return 0;
}