#include "stdafx.h"
#include "gid6.h"
#include "mmenu2.h"
#include "ado.h"

/*
struct DIAM
{
    double d1, d2, d3, tol;
};
*/


#include "utp/utp_diam_rasch.h"

void utp_diam_ras(CMMenu2* wnd, CMenuRow& row, void* pp1, void* pp2, void* pp3, void* pp4)
{
    CUTP_diam_rasch dlg(wnd);
    dlg.DoModal();
}

bool get_diametr(CWnd* wnd, int standardID, double& d1, double& d2, double& d3, double& tol);



#include "CDiams.h"


DIAMS m_diam;


DIAMS* diametr(CWnd* wnd, int standardID)
{
    CDiams dlg(wnd, standardID);
    if (dlg.DoModal() == IDOK) {
        m_diam = dlg.m_diams;
        return &m_diam;
    }
    return NULL;


    //  CMMenu2 menu(AfxGetMainWnd(), IDD_MENU10, "");
    CMMenu2 menu(AfxGetMainWnd(), IDD_MENU_DIAM, _TR("Выберите диаметр трубопровода"));

    CAdoFile* ado = getAdo(getGidAdoName());

    if (!ado) return NULL;

    CString q;

    CString app = _TR("Россия");
    if (standardID == 2) app = _TR("АВВ");

    q.Format("SELECT * FROM standardTubes WHERE stand='%s'", app);

    menu.AddColumn(_TR("Условный"));
    menu.AddColumn(_TR("Внешний"));
    menu.AddColumn(_TR("Внутренний"));
    menu.AddColumn(_TR("Толщина"));

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            DIAMS* d = new DIAMS;

            d->d1 = ado->read_double("diametr_usl");
            d->d2 = ado->read_double("diamVne");
            d->d3 = ado->read_double("diametr");
            d->tol = ado->read_double("tol");

            CString s1, s2, s3, s4;

            s1.Format("%g", d->d1);
            s2.Format("%g", d->d2);
            s3.Format("%g", d->d3);
            s4.Format("%g", d->tol);

            menu.Add(1, s1);
            menu.Add(2, s2);
            menu.Add(3, s3);
            menu.Add(4, s4);

            menu.setWidth(1, 100);
            menu.setWidth(2, 100);
            menu.setWidth(3, 100);
            menu.setWidth(4, 100);

            menu.Add((void*)d);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    menu.AddProc(IDC_ADD, utp_diam_ras, (void*)nullptr, (void*)nullptr);

    if (menu.DoModal() == IDOK) {
        return (DIAMS*)menu.getV();
    }

    return NULL;
}

bool get_diametr(CWnd* wnd, int standardID, double& d1, double& d2, double& d3, double& tol)
{
    DIAMS* d = diametr(wnd, standardID);
    if (!d) return false;
    d1 = d->d1;
    d2 = d->d2;
    d3 = d->d3;
    tol = d->tol;
    return true;
}

