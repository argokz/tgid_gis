#include "stdafx.h"
#include "Resource.h"
#include "win.h"

#include "gid6.h"

#include "gid6Doc.h"
#include "gidrView.h"
#include "GeoFile.h"

#include <sys/stat.h>
#include <share.h>
#include <math.h>
#include <direct.h>
#include <algorithm>
//#include "TestSheet.h"
//#include "GeoEdit.h"
#include "KlMenu.h"
//#include "EditDlg_shp.h"

//#include "TestSheet.h"
#include "ps_alma.h"


void OnOff(CTreeCtrl &m_ctlTree, HTREEITEM hItem, int ch);

HTREEITEM InsertItem(CTreeCtrl *ctlTree, LPCTSTR txt, int nImage, int nSelectedImage, HTREEITEM i);
HTREEITEM find_ins(CTreeCtrl *tree, HTREEITEM i2, int kod, const char *fn);
HTREEITEM FindChild(CTreeCtrl *tree, HTREEITEM hItem, const char *txt);


static map <CString, int> map_n;
static map <CString, int> map_seq;  // ќбъект по пор€дку

int num_map(const char *fn)
{
    map<CString, int>::const_iterator it;

    it = map_n.find(CString(fn));

    if (it != map_n.end()) {
        return it->second;
    }
    return -1;
}

int seq_map(const char* fn)
{
    map<CString, int>::const_iterator it;

    it = map_seq.find(CString(fn));

    if (it != map_seq.end()) {
        return it->second;
    }
    return -1;
}



#if 0
void InitTree(CTreeCtrl &m_ctlTree, KlList *m_kl_list, CString database, const char * fn, HTREEITEM i2, int n)
{

    HTREEITEM i3;
    KlList::iterator it = m_kl_list->begin();

    bool isChecked = false;

    for (; it != m_kl_list->end(); ++it) {
        Klassif *kl = &it->second;

        CString name = kl->nazv;

        if (num_map(name) == n) {

            if (kl->m_database == database) {
                if (1) {
                    int kod = kl->ins ? 5 : 4;

                    name = kl->RusName();

                    i3 = find_ins(&m_ctlTree, i2, kod, name);
                    if (kl->ins) {
                        isChecked = true;
                    }
                }
                else {
                    i3 = i2;
                }

                m_ctlTree.SetItemData(i3, (DWORD)kl);
                m_ctlTree.SetCheck(i3, kl->ins);
            }
        }
    }
    m_ctlTree.SetCheck(i2, isChecked);
    OnOff(m_ctlTree, i2, isChecked);
}
#endif


void InitTree(CTreeCtrl& m_ctlTree, KlList* m_kl_list, CString database, const char* fn1, HTREEITEM i2, int n)
{
    map<int, Klassif*> map_kl;

    if (n == -1) {
        int seq = 1;

        KlList::iterator it = m_kl_list->begin();
        for (; it != m_kl_list->end(); ++it) {
            Klassif* kl = &it->second;
            map_kl[seq++] = kl;
        }
    }
    else {
        KlList::iterator it = m_kl_list->begin();

        for (; it != m_kl_list->end(); ++it) {
            Klassif* kl = &it->second;
            int seq = seq_map(kl->nazv);
            map_kl[seq] = kl;
        }
    }

    bool isChecked = false;
    HTREEITEM i3;

    map<int, Klassif*>::const_iterator it = map_kl.begin();

    int ii = 0;

    for (; it != map_kl.end(); ++it) {
        Klassif* kl = it->second;

        CString name = kl->nazv;

        if (num_map(name) == n) {

            if (kl->m_database == database) {
                if (1) {
                    int kod = kl->ins ? 5 : 4;

                    name = kl->RusName();

                    i3 = find_ins(&m_ctlTree, i2, kod, name);
                    if (kl->ins) {
                        isChecked = true;
                    }
                }
                else {
                    i3 = i2;
                }

                m_ctlTree.SetItemData(i3, (DWORD)kl);
                m_ctlTree.SetCheck(i3, kl->ins);
            }
        }
        ii++;
    }
    m_ctlTree.SetCheck(i2, isChecked);
    OnOff(m_ctlTree, i2, isChecked);
}



CString getGidAdoName();

void CGeoFile::initGeoTree(const char * database, CTreeCtrl &ctlTree, HTREEITEM i1)
{
    ctlTree.SetItemData(TVI_ROOT, (DWORD)0);

    HTREEITEM i2, i3 = 0;

    i2 = ctlTree.GetChildItem(TVI_ROOT);

    CString lstn;
    
    lstn.Format("%skls\\%s.lst", argpath(), database);
    //  lstn = GetFName(fn)+".lst";
    if (database == getGidAdoName()) {
        lstn.Format("%skls\\GID.lst", argpath());
    }


    i2 = FindChild(&ctlTree, i2, GetTitle(database));
    if (!i2) {
        i2 = InsertItem(&ctlTree, GetTitle(database), 5, 5, i1);
    }

    char s[256];
    CString fn = "";

    FILE *f = fopen(lstn, "r");

    if (f) {
        static int id = 1;
        static int num = 0;
        while (!feof(f)) {
            fgetstr(s, 255, f);
            if (s[0] && s[0] != '-') {
                fn = &s[1];

                if (s[0] != ' ') {
                    if (num) InitTree(ctlTree, m_kl_list, database, fn, i3, num);
                    num++;
                    i3 = 0;
                    if (s[0] != '=') {
                        i3 = find_ins(&ctlTree, i2, 5, s);
                    }
                }
                else {
                    map_n[fn] = num;
                    map_seq[fn] = id ++;

                    if (i3 == 0) {
                        Klassif* kls = m_kl_list->findKlN(transl(fn));
                        if (kls) {
                            kls->ins = 0;
                        }
                    }
                }
            }
        }
//        if (num && i3) InitTree(ctlTree, m_kl_list, database, fn, i3, num);


        if (num) {
            if (i3) {
                InitTree(ctlTree, m_kl_list, database, fn, i3, num);
            }
        }



        fclose(f);
    }

    InitTree(ctlTree, m_kl_list, database, "!!!!", i2, -1);

/**

    KlList::iterator it = m_kl_list->begin();

    for (; it != m_kl_list->end(); ++it) {
        Klassif *kl = &it->second;
        if (num_map(kl->nazv) == -1) {
            //      i3 = find_ins(&ctlTree, i2, 5,  "ќстальные");
            InitTree(ctlTree, m_kl_list, database, kl->nazv, i2, -1);
        }
    }
*/
}

void CGeoFile::initGeoTree(CTreeCtrl &ctlTree)
{
    map_n.clear();

    if (::IsWindow(ctlTree)) {
        ctlTree.DeleteAllItems();
    }

    if (!m_kl_list) return;

    //  HTREEITEM i1 = ctlTree.InsertItem("СЂЃ®", 0, 0);
    HTREEITEM i1 = 0;

    set<CString> fn;

    KlList::iterator it;

    for (it = m_kl_list->begin(); it != m_kl_list->end(); ++it) {
        const Klassif *kl = &it->second;
        CString str;
        str = kl->m_database;
        fn.insert(str);
    }

    {
        set<CString>::const_iterator it;
        for (it = fn.begin(); it != fn.end(); ++it) {
            CString database = *it;
            initGeoTree(database, ctlTree, i1);
        }
    }
}
