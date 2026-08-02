#include "stdafx.h"
#include "ado.h"
#include "PropertyGrid\test\PropGridDlg.h"
#include "win.h"


CString argpath();


static map<long, map<CString, CString> > map_pr;

static map<long, map<CString, int> > map_id;


static map<long, map<CString, CString> > map_pr0;

static map<long, map<CString, int> > map_id0;


static map<long, long > map_pr_id;
static map<long, long > map_pr0_id;

CString trim_br(CString s);

bool table_updated(list<CItemChanged> & lst, void *param, long idd)
{
    map<long, map<CString, CString> >::iterator it = map_pr.find(idd);
    if (it != map_pr.end()) {
        it->second.clear();
    }

    for (list<CItemChanged>::iterator it = lst.begin(); it != lst.end(); ++it) {
        CItemChanged ic = *it;

        CString s = ic.sql_value;

        if (ic.m_is_lookup) {
            s.Format("%d", ic.index);
            map_id[idd][ic.m_name] = ic.index;
        }

        CString val = trim_br(ic.sql_value);

        if (ic.m_undefined) {
            s = "";
        }

        map_pr[idd][ic.m_name] = val;
    }

    return true;
}

int getPropID(long idd) 
{
    map<long, long >::const_iterator it = map_pr_id.find(idd);

    if (it != map_pr_id.end()) {
        return it->second;
    }
    return -1;
}

int getPropID0(long idd) 
{
    {
        map<long, long >::const_iterator it = map_pr_id.find(idd);

        if (it != map_pr_id.end()) {
            return it->second;
        }
    }


    {
        map<long, long >::const_iterator it = map_pr0_id.find(idd);

        if (it != map_pr0_id.end()) {
            return it->second;
        }
    }
    
    
    return -1;
}



bool getPropValue(long idd, CString col, CString &val) 
{
    val = "";
    map<long, map<CString, CString> >::iterator it = map_pr.find(idd);
    if (it != map_pr.end()) {
        map<CString, CString>::iterator it2 = it->second.find(col);
        if (it2 != it->second.end()) {
            val = it2->second;
            return true;
        }
    }
    return false;
}

bool getPropValue0(long idd, CString col, CString &val) 
{
    val = "";

    {
       auto it = map_pr.find(idd);
       if (it != map_pr.end()) {
           auto it2 = it->second.find(col);
           if (it2 != it->second.end()) {
               val = it2->second;
               return true;
           }
       }
    }

    {
        auto it = map_pr0.find(idd);
        if (it != map_pr0.end()) {
            auto it2 = it->second.find(col);
            if (it2 != it->second.end()) {
                val = it2->second;
                return true;
            }
        }

    }


    return false;
}



void setPropValueInit(CPropGridDlg *pm_cEditDlg, long idd, CString col, CString val)
{
    pm_cEditDlg->m_ctrlGrid.SetNameValueInit(col, val);

    map_pr0[idd][col] = val;
}

CPropGridDlg *getPropGridDlg2(CWnd *wnd, CAdoFile *ado, const char *capt, bool modal, TYP_OF_P typ, void *param2)
{
    CPropGridDlg *cEditDlg = new CPropGridDlg(wnd, table_updated, (void*)ado, capt, capt, typ, param2);

    if (cEditDlg != NULL) {
        BOOL ret = cEditDlg->Create(IDD_PROPGRID_DIALOG, wnd);

        if (!ret) {
            delete cEditDlg;
            return NULL;
        }
    }
    return cEditDlg;
}





bool readFun(const char *tab, const char *tn);
bool readValidate(const char* tab, const char* tn);
CString getFun(CString name);
CString getValidate(CString name);



CPropGridDlg *dialog_pr(CWnd *wnd, const char *fn, long idd, const char *capt, int id)
{
    CPropGridDlg *pm_cEditDlg = getPropGridDlg2(wnd, NULL, capt, true, TP_ZERO, NULL);
    if (!pm_cEditDlg) return NULL;
    
    if (idd > 0) {
      pm_cEditDlg->setSend(idd);
    }

    map_pr_id[idd] = id;

    CString fn1;
    fn1.Format("%sdialog\\%s.txt", argpath(), fn);

    FILE *f = fopen(fn1, "r");

    HITEM hi = 0;
    HSECTION hs = -1;

    readFun("dialog", fn);
    readValidate("dialog", fn);

    if (f) {
        char str[1024 + 1];

        while (!feof(f)) {
            if (fgets(str, 1024, f) == NULL) break;
            str[1024] = 0;
            trim(str);

            if (str[0] == '-') {
            }
            else if (str[0] != ' ') {
                hs = pm_cEditDlg->addSection(str, 0, 0);
            }
            else {
                if (hs >= 0) {
                    vector<CString> match;

                    int l = regex_match("\\s+([^ ]+)\\s+([a-zA-Z]+)\\s+(.+)\\s*$", str, match);
                    if (l > 1) {

                        CString name = match[1];
                        CString type = match[2];
                        CString label = match[3];

                        CGridData dg(name, "");
                        dg.label = dg.help = label;

                        if (type == "A") {
                        }
                        else if (type == "L") {
//                            dg.typ = adInteger;

                            int l = regex_match("\\s+([^ ]+)\\s+([a-zA-Z]+)\\s+\"(.+?)\",\"(.+?)\",\"(.+?)\",\"(.+?)\"\\s+(.+)\\s*$", str, match);
                            if (l > 1) {
                                name = match[1];
                                type = match[2];

                                CString schema = match[3];
                                CString tab = match[4];
                                CString id = match[5];
                                CString name_id = match[6];
                                
                                label = match[7];
                                dg.label = dg.help = label;

                                CString q;
                                q.Format("SELECT %s, %s FROM %s", id, name_id, tab);

                                hi = pm_cEditDlg->addDataGrid(hs, dg);
                        
                                pm_cEditDlg->m_ctrlGrid.SetItemTable(hi, schema, "", -1);
                                pm_cEditDlg->m_ctrlGrid.SetItemLookup(hi, (const char*)q, 1);
                                CString valid = getValidate(dg.name);
                                if (valid != "") {
                                    pm_cEditDlg->m_ctrlGrid.SetItemValidate(hi, valid);
                                }
                                continue;
                            }
                        }
                        else if (type == "M") {
                            dg.typ = adLongVarWChar;
                        }
                        else if (type == "F") {
                            dg.typ = adDouble;
                        }
                        else if (type == "N") {
                            dg.typ = adInteger;
                        }
                        else if (type == "D") {
                            dg.typ = adDate;
                        }


                        hi = pm_cEditDlg->addDataGrid(hs, dg);

                        CString fun = getFun(dg.name);

                        if (fun != "") {
                            pm_cEditDlg->m_ctrlGrid.SetItemFun(hi, fun);
                        }

                        CString valid = getValidate(dg.name);
                        if (valid != "") {
                            pm_cEditDlg->m_ctrlGrid.SetItemValidate(hi, valid);
                        }
                    }
                }
            }
        }

        fclose(f);
    }

    return pm_cEditDlg;
}