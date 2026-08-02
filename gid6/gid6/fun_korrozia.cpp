#include "stdafx.h"
#include "gid6.h"
#include "PotrCx.h"
#include "geofile.h"
#include "CGidResist.h"

#include "PropertyGrid\test\PropGridDlg.h"

#include "ado.h"


#include "gidrView.h"


double getRealValueDouble(CPropertyGrid* wnd, const char* fn);
int getRealValueInt(CPropertyGrid* wnd, const char* fn);
CString getRealValueText(CPropertyGrid* wnd, const char* fn);
COleDateTime getRealValueDate(CPropertyGrid* wnd, const char* fn);

void set_season()
{
    CGidrView* pView = getView();
    if (pView) {
        pView->setKorroziaSezonDlg();
    }
}

bool check_season(COleDateTime dt1) 
{
    CGidrView *pView = getView();
    int y0 = -1; // Текуший сезон

    if (pView) {
        if (pView->m_sezon_date1 == 0) {
            pView->setKorroziaSezon(pView->m_sezon_korrozia, false);
        }

        if (pView->m_sezon_date1 < dt1 && dt1 <= pView->m_sezon_date2) {
            return true;
        }
    }
    return false;
}

bool get_season(int &y1, int &y2, COleDateTime &date1, COleDateTime &date2)
{
    CGidrView *pView = getView();
    int y0 = -1; // Текуший сезон

    if (pView) {
        y1 = pView->m_sezon_korrozia;
        y2 = y1 + 1;

        date1 = pView->m_sezon_date1;
        date2 = pView->m_sezon_date2;
        return true;
    }
    return false;
}


bool check_korrozia(CPropertyGrid* wnd)
{
    COleDateTime dt0, dt1, dt2;

    COleDateTime d_cur = COleDateTime::GetCurrentTime();

    bool is_d0 = wnd->GetNameValue("data_planirovaniya", dt0);
    
    bool is_d1 = wnd->GetNameValue("data_ustanovki", dt1);
    bool is_d2 = wnd->GetNameValue("data_izvlecheniya", dt2);

    int stateIndID;

    bool is_stateIndID = wnd->GetNameValue("stateIndID", stateIndID);


    int sostoyanie = 4;

    wnd->GetNameValue("sostoyanie", sostoyanie);

    CGidrView *pView = getView();


    int y0 = -1; // Текуший сезон

    if (pView) {
        y0 = pView->m_sezon_korrozia;
    }


    int y1 = dt0.GetYear();
    int y2 = dt1.GetYear();
    int y3 = dt2.GetYear();

    if (is_d0 && y1 != y0) {
        CString s;
        s.Format("Дата планирования должна быть в отопительном сезоне %d-%d.", y0, y0+1);
        AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
        return false;
    }

// Извлечение 

/*
    if (is_d2 && (y3 != y0 && y3 != y0+1 )) {
        CString s;
        s.Format("Дата извлечения должна быть в отопительном сезоне %d-%d.", y0, y0+1);
        AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
        return false;
    }
*/


    if (is_d1 && is_d2 && dt1 >= dt2) {
        AfxMessageBox(_TR("Дата извлечения должна быть позже даты установки."), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    if (is_d0 && is_d1 && dt0.GetYear() != dt1.GetYear()) {
        AfxMessageBox(_TR("Дата планирования и дата установки должны быть в одном году"), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    if (is_d1 && dt1 > d_cur) {
        AfxMessageBox(_TR("Дата установки должна быть раньше текущей даты"), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    if (is_d2 && dt2 > d_cur) {
        AfxMessageBox(_TR("Дата извлечения должна быть раньше текущей даты"), MB_OK|MB_ICONINFORMATION);
        return false;
    }

//    if (sostoyanie == 4) {
        if (!wnd->check_exists("data_planirovaniya")) return false;
//    }


    if (sostoyanie >= 1 && sostoyanie != 4) {
        if (!wnd->check_exists("mesto_ustanovki")) return false;
        if (!wnd->check_exists("nomer_indikatora_korrozii")) return false;
    }

    if (sostoyanie >= 2 && sostoyanie != 4) {
//        if (!wnd->check_exists(string(""))) return false;
//        if (!wnd->check_exists(string(""))) return false;
//        if (!wnd->check_exists(string(""))) return false;

        if (!wnd->check_exists("sredniy_ves_plastiny_pri_ustanovke__g")) return false;
    }

    if (sostoyanie == 3) {
        if (!wnd->check_exists("stateIndID")) return false;

        if (stateIndID == 1) {
            if (!wnd->check_exists("sredniy_ves_plastiny_posle_ispytaniy__g")) return false;
        }
    }

    return true;
}

#if 0

void copy_ind1(CAdoFile *ado)
{
    ado->AddNew(transl(tn));

    ado->write("ID_I", (long)id);
    ado->write(transl("Количество пластин в сборке"), (long)getRealValueInt(wnd, transl("Количество пластин в сборке")));
    ado->write(transl("Средний вес пластины при установке, г"), getRealValueDouble(wnd, transl("Средний вес пластины при установке, г")));
    ado->write(transl("Радиус круглой пластины, мм"), getRealValueDouble(wnd, transl("Радиус круглой пластины, мм")));
    ado->write(transl("Радиус втулки, мм"), getRealValueDouble(wnd, transl("Радиус втулки, мм")));
    ado->write(transl("Толщина пластины, мм"), getRealValueDouble(wnd, transl("Толщина пластины, мм")));

    int stateIndID;

    wnd->GetNameValue("stateIndID", stateIndID);

    ado->write("stateIndID", stateIndID);
    ado->write("data_planirovaniya", getRealValueDate(wnd, "data_planirovaniya"));
    
    ado->write(transl("Дата установки"), getRealValueDate(wnd, transl("Дата установки")));
    ado->write(transl("Дата извлечения"), getRealValueDate(wnd, transl("Дата извлечения")));

    int dt = dt2 - dt1; // Время испытаний, сут

    ado->write(transl("Количество дней испытаний"), (long)dt);

    ado->write("stateIndID", stateIndID);

    if (stateIndID == 2)  {
//        wnd->SetNameNull(string(transl("Средний вес пластины после испытаний, г")));
//        wnd->SetNameNull(string(transl("Потеря массы средняя при кислотной обрабоке, г")));
//        wnd->SetNameNull(string(transl("Средняя скорость коррозии, мм/год")));
//        wnd->SetNameNull(string(transl("Оценка коррозионного процесса")));
//        wnd->SetNameNull(string(transl("Агрессивность сетевой воды")));

        ado->write_null(transl("Средний вес пластины после испытаний, г"));
        ado->write_null(transl("Потеря массы средняя при кислотной обрабоке, г"));
        ado->write_null(transl("Средняя скорость коррозии, мм/год"));
        ado->write_null(transl("Оценка коррозионного процесса"));
        ado->write_null(transl("Агрессивность сетевой воды"));


        wnd->SetNameNull("sredniy_ves_plastiny_posle_ispytaniy__g");
        wnd->SetNameNull("poterya_massy_srednyaya_pri_kislotnoy_obraboke__g");
        wnd->SetNameNull("srednyaya_skorost_korrozii__mm_god");
        wnd->SetNameNull("otsenka_korrozionnogo_protsessa");
        wnd->SetNameNull("agressivnost_setevoy_vody");

//        ado->write_null(transl("Средний вес пластины после испытаний, г"));
//        ado->write_null(transl("Потеря массы средняя при кислотной обрабоке, г"));
//        ado->write_null(transl("Средняя скорость коррозии, мм/год"));
//        ado->write_null(transl("Оценка коррозионного процесса"));
//        ado->write_null(transl("Агрессивность сетевой воды"));
    }    
    else {
        ado->write(transl("Средний вес пластины после испытаний, г"), getRealValueDouble(wnd, transl("Средний вес пластины после испытаний, г")));
        ado->write(transl("Потеря массы средняя при кислотной обрабоке, г"), getRealValueDouble(wnd, transl("Потеря массы средняя при кислотной обрабоке, г")));
        ado->write(transl("Средняя скорость коррозии, мм/год"), getRealValueDouble(wnd, transl("Средняя скорость коррозии, мм/год")));
        ado->write(transl("Оценка коррозионного процесса"), getRealValueInt(wnd, transl("Оценка коррозионного процесса")));
        ado->write(transl("Агрессивность сетевой воды"), getRealValueInt(wnd, transl("Агрессивность сетевой воды")));
    }


    ado->write(transl("Внешний вид пластин"), getRealValueText(wnd, transl("Внешний вид пластин")));
    ado->Update();
    
}
#endif


bool ind_save(CPropertyGrid* wnd)
{
    //    int id = getID(STR_REMONT_KORROZ);

    int id = wnd->GetID("poterya_massy_srednyaya_pri_kislotnoy_obraboke__g");


    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return false;

    CString s;
    s.Format("od=%d", id);

    COleDateTime dt1, dt2, dt0;
    CString tn = _TR("Индикатор_коррозии_по_годам");

    bool is_d0 = wnd->GetNameValue("data_planirovaniya", dt0);
    bool is_d1 = wnd->GetNameValue("data_ustanovki", dt1);
    bool is_d2 = wnd->GetNameValue("data_izvlecheniya", dt2);

    int sostoyanie = 1;

    wnd->GetNameValue("sostoyanie", sostoyanie);



/*
    if (!is_d1 || !is_d2) {
        AfxMessageBox(_TR("Не заданы даты установки и извлечения. Сохранение не выполняется."), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    if (dt1 >= dt2) {
        AfxMessageBox(_TR("Дата извлечения должна быть позже даты установки. Сохранение не выполняется."), MB_OK|MB_ICONINFORMATION);
        return false;
    }
*/

    int y1 = 0;
    
    if (is_d0) {
        y1 = dt0.GetYear();
    }
    else if (is_d1) {
        y1 = dt1.GetYear();
    }


    CString q;
    q.Format("SELECT * FROM %s WHERE (YEAR(data_ustanovki)=%d OR YEAR(data_planirovaniya)=%d) AND ID_I=%d", transl(tn), y1, y1, id);

    int nr = getCountTable2(ado, q);

    if (nr > 0) {
//        if (AfxMessageBox(_TR("Сохранение обработки индикатора коррозии уже выполнено. Пересохранить? Предыдущие данные обработки будут удалены."), MB_YESNO) != IDYES) return false;
        q.Format("DELETE FROM %s WHERE (YEAR(data_ustanovki)=%d OR YEAR(data_planirovaniya)=%d) AND ID_I=%d", transl(tn), y1, y1, id);
        ado->Execute(q);
    }

    ado->AddNew(transl(tn));
    ado->write("ID_I", (long)id);

    ado->write(transl("Состояние"), (long)getRealValueInt(wnd, transl("Состояние")));



    ado->write("nomer_indikatora_korrozii", getRealValueText(wnd, "nomer_indikatora_korrozii"));

    ado->write("truboprovod", (long)getRealValueInt(wnd, transl("truboprovod")));


    ado->write(transl("Количество пластин в сборке"), (long)getRealValueInt(wnd, transl("Количество пластин в сборке")));
    ado->write(transl("Средний вес пластины при установке, г"), getRealValueDouble(wnd, transl("Средний вес пластины при установке, г")));
    ado->write(transl("Радиус круглой пластины, мм"), getRealValueDouble(wnd, transl("Радиус круглой пластины, мм")));
    ado->write(transl("Радиус втулки, мм"), getRealValueDouble(wnd, transl("Радиус втулки, мм")));
    ado->write(transl("Толщина пластины, мм"), getRealValueDouble(wnd, transl("Толщина пластины, мм")));

    int stateIndID;

    wnd->GetNameValue("stateIndID", stateIndID);

    ado->write("stateIndID", stateIndID);
    ado->write("data_planirovaniya", getRealValueDate(wnd, "data_planirovaniya"));
    
    ado->write(transl("Дата установки"), getRealValueDate(wnd, transl("Дата установки")));
    ado->write(transl("Дата извлечения"), getRealValueDate(wnd, transl("Дата извлечения")));

    int dt = dt2 - dt1; // Время испытаний, сут

    if (sostoyanie == 3) {
        ado->write(transl("Количество дней испытаний"), (long)dt);
        ado->write("stateIndID", stateIndID);
    }

    if (stateIndID == 2)  {
//        wnd->SetNameNull(string(transl("Средний вес пластины после испытаний, г")));
//        wnd->SetNameNull(string(transl("Потеря массы средняя при кислотной обрабоке, г")));
//        wnd->SetNameNull(string(transl("Средняя скорость коррозии, мм/год")));
//        wnd->SetNameNull(string(transl("Оценка коррозионного процесса")));
//        wnd->SetNameNull(string(transl("Агрессивность сетевой воды")));

        ado->write_null(transl("Средний вес пластины после испытаний, г"));
        ado->write_null(transl("Потеря массы средняя при кислотной обрабоке, г"));
        ado->write_null(transl("Средняя скорость коррозии, мм/год"));
        ado->write_null(transl("Оценка коррозионного процесса"));
        ado->write_null(transl("Агрессивность сетевой воды"));


        wnd->SetNameNull("sredniy_ves_plastiny_posle_ispytaniy__g");
        wnd->SetNameNull("poterya_massy_srednyaya_pri_kislotnoy_obraboke__g");
        wnd->SetNameNull("srednyaya_skorost_korrozii__mm_god");
        wnd->SetNameNull("otsenka_korrozionnogo_protsessa");
        wnd->SetNameNull("agressivnost_setevoy_vody");

//        ado->write_null(transl("Средний вес пластины после испытаний, г"));
//        ado->write_null(transl("Потеря массы средняя при кислотной обрабоке, г"));
//        ado->write_null(transl("Средняя скорость коррозии, мм/год"));
//        ado->write_null(transl("Оценка коррозионного процесса"));
//        ado->write_null(transl("Агрессивность сетевой воды"));
    }    
    else {
        if (sostoyanie == 3) {
            ado->write(transl("Средний вес пластины после испытаний, г"), getRealValueDouble(wnd, transl("Средний вес пластины после испытаний, г")));
            ado->write(transl("Потеря массы средняя при кислотной обрабоке, г"), getRealValueDouble(wnd, transl("Потеря массы средняя при кислотной обрабоке, г")));
            ado->write(transl("Средняя скорость коррозии, мм/год"), getRealValueDouble(wnd, transl("Средняя скорость коррозии, мм/год")));
            ado->write(transl("Оценка коррозионного процесса"), getRealValueInt(wnd, transl("Оценка коррозионного процесса")));
            ado->write(transl("Агрессивность сетевой воды"), getRealValueInt(wnd, transl("Агрессивность сетевой воды")));
        }
    }


    ado->write("primechanie", getRealValueText(wnd, "primechanie"));
    ado->write(transl("Внешний вид пластин"), getRealValueText(wnd, transl("Внешний вид пластин")));
    ado->Update();

    return true;

//    if (ask) {
//        AfxMessageBox(transl("Выполнено"), MB_OK|MB_ICONINFORMATION);
//    }
}
