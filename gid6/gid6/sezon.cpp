#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"

#include "electro.h"

#include "OtopSezon.h"

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);

CString get_color_q(CString q, int sost);


void CGidrView::setKorroziaSezon(int year1, bool force)
{
    Invalidate();

    if (year1 > 0) {

        CAdoFile* ado = m_cxema->m_ado;

        CString q;

        q.Format(
"SELECT \n"
"hs1.date2 AS d1, IIF (hs2.date2 IS NULL,DATEADD(year, 1, hs1.date2), hs2.date2)  AS d2\n"
"FROM heating_seasons hs1\n"
"LEFT JOIN heating_seasons hs2 ON YEAR(hs2.date1) = YEAR(hs1.date1)+1\n"

"WHERE YEAR(hs2.date1)=%d\n", year1);

        if (ado->openTable0(q)) {
            if (!ado->isEOF()) {
                m_sezon_date1 = ado->read_date(0);
                m_sezon_date2 = ado->read_date(1);
                //            ado->MoveNext();
            }
            ado->closeTable();
        }
    }

    if (m_sezon_korrozia == year1 && !force) return;
    m_sezon_korrozia = year1;

    CString tn = REM_INDICATOR;
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) return;

    if (kls->ins) {
        kls->ins = 1;
        m_isGeobaza = TRUE;

        CString fn = "objects\\defect\\indikator_korrozii";

        CString qq = readQ(kls->m_ado, fn, "", m_cxema->m_par);

        qq.Replace("$and_condition$","");
        qq.Replace("$j$", "");
        qq.Replace("$order$", "");

        CString s2015;
        s2015.Format("%d", m_sezon_korrozia);
        qq.Replace("$YEAR1$", s2015);

        CString q_color = get_color_q(qq, -1);

        m_geofile->setVisibleColor(kls, q_color);
        
        //CVirtualTable2 *a = (CVirtualTable2 *)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ);
        WebPageVirtualTable* a = (WebPageVirtualTable*)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ);
        if (a) {
            BOOL delay = TRUE;
            a->ShowPane(FALSE, delay, FALSE);
            resetZhurnal(true);
        }
    }

    SetWindowText2();
}


void CGidrView::setKorroziaSezonDlg()
{
//    CString tn = REM_INDICATOR;
//    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
//    if (!kls) return;

//    if (!kls->ins) return;

    OnInitSezon();

    COtopSezon dlg1(this, "!!", m_sezon_korrozia);
    dlg1.setTab(m_cxema->m_ado, "!");
    if (dlg1.DoModal() == IDOK) {
        setKorroziaSezon(dlg1.m_year1, true);
    }
}


void CGidrView::OnSezon()
{
    setKorroziaSezonDlg();
}
