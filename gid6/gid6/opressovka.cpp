#include "stdafx.h"
#include "gid6.h"

#include <fstream>

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"
#include "win.h"
#include "ado.h"

#include <locale.h>

#include "TreeCtrlX.h"

#include "mmenu.h"
#include "mmenu2.h"
#include "dialog_pr.h"

#include "ado2.h"


//////////////////////////////////////////////////////////////////////////////////

void CGidrView::OnUpdateOpr(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_id_opr > 0 || m_id_opr_old > 0);
}



void CGidrView::OnUpdateSaveOpres(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_cxema->m_graph->isMark());
}

void CGidrView::OnUpdateSaveCurentOpres(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_cxema->m_graph->isMark() && m_id_opr > 0);
}


void CGidrView::OnNewOpres()
{
}


bool isOpres(CLINE2* l, int m_id_opr)
{
    if (!l) return false;
    if (m_id_opr <= 0) return false;

    CString opres = "opres";

    CString q;
    q.Format("Select lineID From %sDeployed Where directionID = %d AND lineID IN (%d, %d)", opres, m_id_opr, bline(l)->line.nomP, bline(l)->line.nomO);

    bool ret = false;

    CAdoFile* ado = getAdo(getPsAdoName());

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            ret = true;
        }
        ado->closeTable();
    }
    return ret;
}

void CGidrView::saveRemont2()
{
}

#if 0

bool isRemont2(CLINE2* l, int m_id_opr)
{
}

#endif


void CGidrView::OnUpdateSaveRemont2(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_cxema->m_graph->isMark());
}

void CGidrView::OnUpdateSaveCurentRemont2(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_cxema->m_graph->isMark() && m_id_remont2 > 0);
}




CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);



void CGidrView::OnZhurnalDefectOpres()
{
    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    CString tn = "defect";

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) {
        CString s;
        s.Format("Нет объекта Повреждения");
        AfxMessageBox(s);
        return;
    }
    
    if (m_id_opr <= 0) {
        CString s;
        s.Format("Выберите контур опрессовок");
        AfxMessageBox(s);
        return;
    }


    CString fStr;

    fStr.Format("%ssql\\objects\\defect\\defectsByOpresID.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        fStr.Format("objects\\defect\\defectsByOpresID");
        CString q = readQ(kls->m_ado, fStr, "", m_cxema->m_par);

        CString ss;
        ss.Format("%d", m_id_opr);

        q.Replace("$objID$", ss);

        ss.Format("SELECT name FROM opres WHERE id=%d", m_id_opr);
        ss = readTableValue(ado, ss);

        ss.Format("Дефекты контура опрессовок %s", ss);

//        RunTableNiz(NULL, ado, tn, q, kls->RusName(), true, true);
        RunTableNiz(ado, tn, q, ss, true, false, virt_data_geo);
    }

//    viewNizGeoQ("defect", "stateID", "data_osmotra", true);
}
