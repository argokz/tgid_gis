#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "gidrView.h"

#include "geofile.h"

#include "CTeploset2.h"

#include "InputDia.h"
#include "Filtr3.h"
#include "ado.h"
#include "MMenu.h"
#include "mmenu2.h"

#include "InputDialogDate.h"

#include "ps_alma.h"

int vydMsRs(CGraph2* m_graph, int ms, int rs);


void CGidrView::OnOsmotrAdd()
{
    CTeploset2 dlg(this, m_is_CTeploset);

    int nachalnik = 0;

    if (dlg.DoModal() == IDOK) {
        m_cxema->m_graph->reset();
        vydMsRs(m_cxema->m_graph, dlg.m_ms, dlg.m_rs);

        if (!m_cxema->m_graph->isMark()) {
           AfxMessageBox(_TR("Участок не описан"), MB_OK|MB_ICONINFORMATION);
           return;
        }

        MoveVyd();
        Invalidate();

        int ue = 0;

        if (dlg.m_rs) {
          URS *rs = ps_alma.getRs(dlg.m_rs);
          if (rs) ue = rs->ue;
        }
        if (dlg.m_ms) {
          UMS *ms = ps_alma.getMs(dlg.m_ms);
          if (ms) ue = ms->ue;
        }

        if (ue) {
          const UE *u = ps_alma.getUE(ue);
          if (u) {
            nachalnik = u->nach;
          }
        }

        CString str = dlg.m_strName;

        CString s;
        s.Format("Осмотр %s", str);

        SaveOpresNew("osmotr", "Осмотр", "name", s, "data_osmotra", nachalnik, m_id_osmotr2);
    }
}
