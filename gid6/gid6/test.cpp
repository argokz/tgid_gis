#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"
#include "ado.h"

bool create_vyd_line_table_BIG(CAdoFile* ado, CGraph2* graph, CString& tn);
bool create_vyd_node_table_BIG(CAdoFile* ado, CGraph2* graph, CString& tn, bool is_big);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);


#if 0
// Выбор ответственного

void CGidrView::OnTest()
{
//    CElemList dlg(this, m_cxema->m_ado);
//    dlg.DoModal();

//    viewUchPTS();

    CAdoFile *ado = getAdo(getGidAdoName());
    if (ado) {
        CString vyd;
        CString name = _TR("Участки ПТС");
        CString tn = "";

        CGraph2* graph = m_cxema->m_graph;

        if (create_vyd_line_table_BIG(ado, graph, vyd)) {
            CString q = readQ(ado, "UT_BIG", name, 0);
            if (q != "") {
                q.Replace("$vyd$", vyd);
//                viewQ2(ado->m_schema, q, name, name, virt_data_line_big);
//                RunTableNiz(ado, tn, q, ss, true, false);
                RunTableNiz(ado, name, q, name, true, false, virt_data_line_big);
            }
        }
    }
}
#endif


#include "Filtr3.h"

bool fun_on_elem(int id, CString name)
{
    AfxMessageBox(name);

    return true;
}

//#include "CDefectElements.h"
#include "CDefectElem.h"


void CGidrView::OnTest()
{

    CDefectElem dlg(223, this);

    dlg.DoModal();

/*


    {
        CFiltr3 dlg(this, "");
        CString q = "SELECT id, name FROM remontTruboprovodaSpisok ORDER BY ord";

        CAdoFile* ado = getAdo(getGidAdoName());

        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {
                int id = ado->read_long(0);
                CString s = ado->readStr(1);
                dlg.Add(s, id, 0);
                ado->MoveNext();
            }
            ado->closeTable();
        }

        dlg.setFunOn(fun_on_elem);
        
        
        dlg.DoModal();

    }
    
    
    CFiltr3 dlg(this, "Элементы");
    CString q = "SELECT id, name FROM spisokElementov ORDER BY ord";

    CAdoFile* ado = getAdo(getGidAdoName());

    if (ado->openTable0(q)) {
        while ( !ado->isEOF() ) {
            int id = ado->read_long(0);
            CString s = ado->readStr(1);
            dlg.Add(s, id, 0);
            ado->MoveNext();
        }
        ado->closeTable();
    }
    dlg.DoModal();
*/

}