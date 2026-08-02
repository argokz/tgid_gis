// TuFind.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "ado.h"
#include "TuFind.h"
#include "geofile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTuFind dialog




CTuFind::CTuFind(CWnd* pParent /*=NULL*/)
    : CDialog(CTuFind::IDD, pParent)
{
    //{{AFX_DATA_INIT(CTuFind)
    m_text = "";
    //}}AFX_DATA_INIT

    m_street = "";
}


void CTuFind::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTuFind)
    DDX_Control(pDX, IDC_TEXT, m_ctrl_text);
    DDX_CBString(pDX, IDC_TEXT, m_text);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTuFind, CDialog)
    //{{AFX_MSG_MAP(CTuFind)
    ON_CBN_EDITCHANGE(IDC_TEXT, OnEditchangeText)
    ON_CBN_SELCHANGE(IDC_TEXT, OnSelchangeText)
    ON_CBN_EDITUPDATE(IDC_TEXT, OnEditupdateText)
    ON_CBN_SETFOCUS(IDC_TEXT, OnSetfocusText)
    ON_CBN_DROPDOWN(IDC_TEXT, OnDropdownText)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTuFind message handlers



int decode_dom2(const char *s)
{
    for (int i = 0; s[i]; i++) {
        if ('0' <= s[i] && s[i] <= '9') {
            return atoi(&s[i]);
        }
    }
    return 0;
}


struct less_dom 
{
    using result_type = bool;
    using first_argument_type = const CString&;
    using second_argument_type = const CString&;
    result_type operator() (const CString arg1, const CString & arg2) const
    {
        int n1 = decode_dom2(arg1);
        int n2 = decode_dom2(arg2);

        if (n1 < n2) return true;
        if (n1 > n2) return false;

        return arg1 < arg2;
    }
};


static map<CString, int, less_dom> map_dom;
static map<CString, CString> map_str1;
static map<CString, int> map_nazv;


static map<CString, int> map_nazv2;


BOOL CTuFind::OnInitDialog()
{
    CDialog::OnInitDialog();

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Здания ТУ"));
    if (!kls || !kls->m_ado) {
        return TRUE;
    }

    map_str1.clear();
    map_nazv.clear();

    CAdoFile *ado = kls->m_ado;

    CString q;

    q.Format("select distinct name, name_2 from %s", transl("Улицы"));
    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            while (!ado->isEOF()) {
                CString s1 = ado->readStr(0);
                CString s2 = ado->readStr(1);

                map_str1[s1] = s2;
                set_str.insert(s1);

                ado->MoveNext();
            }
        }
        ado->closeTable();
    }


    q.Format("select * from %s", transl("Технические условия"));
    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            //        long id = ado->read_long("ID");
            long id = ado->read_long(transl("Здание"));
            CString s1 = ado->readStr(transl("Наименование организации, запрашивающей ТУ"));
            CString s2 = ado->readStr(transl("Наименование объекта"));
            CString s3 = ado->readStr(transl("Номер ТУ"));

            s1 += " организация";
            s2 += " объект";

            //        if (id != 0) {
            if (s1 != "") map_nazv[s1] = id;
            if (s2 != "") map_nazv[s2] = id;
            if (s3 != "") map_nazv[s3] = id;
            //        }

            ado->MoveNext();
        }
        ado->closeTable();
    }

    m_ctrl_text.SetFocus();

    return FALSE;
}

void drawDom(CComboBox &m_ctrl_text, const char * street)
{
    int i;

    for (i = m_ctrl_text.GetCount() - 1; i >= 0; i--) m_ctrl_text.DeleteString(i);

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
    if (!kls || !kls->m_ado) {
        return;
    }


    CAdoFile *ado = kls->m_ado;

    CString q, street2;

    map<CString, CString>::const_iterator it = map_str1.find(street);
    if (it != map_str1.end()) {
        street2 = it->second;
    }

    map_dom.clear();

    q.Format("select distinct %s, ID from %s WHERE %s='%s'", transl("Дом"), transl("Здания потребителей"), transl("Улица"), street2);
    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            CString s1 = ado->readStr(0);
            int id = ado->read_long(1);

            CString s;
            s.Format("%s %s", street, s1);
            //        m_ctrl_text.AddString(s);

            map_dom[s] = id;

            ado->MoveNext();
        }
        ado->closeTable();
    }

    {
        map<CString, int, less_dom>::iterator it = map_dom.begin();
        for (; it != map_dom.end(); it++) {
            m_ctrl_text.AddString(it->first);
        }
    }
}

unsigned int cyr(unsigned int c)
{

    char cy[] = "фисвуапршолдьтщзйкыегмцчня";

    if ('a' <= c && c <= 'z') {
        return cy[c - 'a'];
    }
    if (c == ';') return 'ж';
    if (c == ';') return 'э';
    if (c == ',') return 'б';
    if (c == '.') return 'ю';
    return c;
}

CString cyr(const char *s)
{
    CString qq = "";

    for (int i = 0; s[i]; i++) {
        qq += (char) cyr(s[i]);
    }

    return qq;
}

bool inStr(CString s, set<CString> &setp)
{
    s.TrimRight();
    s.MakeLower();
    set<CString>::const_iterator it = setp.begin();

    for (; it != setp.end(); it++) {
        CString p = *it;
        p.TrimRight();
        p.MakeLower();
        CString pp = cyr(p);

        if (s.Find(p) < 0 && s.Find(pp) < 0) {
            return false;
        }
    }
    return true;
}


void CTuFind::initList(set<CString> &setp)
{
    {
        map<CString, int>::const_iterator it;
        for (it = map_nazv.begin(); it != map_nazv.end(); it++) {
            CString s = it->first;
            if (inStr(s, setp)) {
                m_ctrl_text.AddString(it->first);
            }
        }
    }

    if (m_ctrl_text.GetCount() == 0 && setp.size() == 1) {

        CString s, line = *setp.begin();
        line.TrimRight();

        set<CString> setp2;
        int pos = 0;

        while (pos != -1) {
            pos = line.Find(" ");
            if (pos != -1) {
                setp2.insert(line.Left(pos));
                line = line.Mid(pos + 1);
            }
        }
        setp2.insert(line);

        if (setp2.size() > 1) initList(setp2);
    }
}


void CTuFind::OnEditchangeText()
{
    UpdateData(TRUE);

    int i;
    CString p = m_text;

    for (i = m_ctrl_text.GetCount() - 1; i >= 0; i--) m_ctrl_text.DeleteString(i);

    if (p == "") return;
    p.MakeLower();

    CString pp = cyr(p);


    set<CString>::const_iterator it;
    for (it = set_str.begin(); it != set_str.end(); it++) {
        CString s = *it;
        s.MakeLower();


        if (s.Find(p) == 0 || s.Find(pp) == 0) {
            m_ctrl_text.AddString(*it);
        }
        if (s == p) {
            m_street = *it;
            drawDom(m_ctrl_text, m_street);
        }
    }

    m_ctrl_text.SetEditSel(-1, 0);

    set<CString> setp;
    setp.insert(p);

    initList(setp);
}

void CTuFind::OnSelchangeText()
{
}


void CTuFind::OnEditupdateText()
{

}

void CTuFind::OnSetfocusText()
{
    m_ctrl_text.SetEditSel(-1, 0);
}

void CTuFind::OnDropdownText()
{
}

void CTuFind::OnOK()
{
    UpdateData(TRUE);

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Здания ТУ"));
    if (!kls || !kls->m_ado) {
        return;
    }

    int cur = m_ctrl_text.GetCurSel();

    if (cur < 0) return;

    CString p, street;

    m_ctrl_text.GetLBText(cur, street);
    p = street;
    p.MakeLower();

    CString pp = cyr(p);

    set<CString>::const_iterator it;
    for (it = set_str.begin(); it != set_str.end(); it++) {
        CString s = *it;
        s.MakeLower();


        if (s == p) {
            drawDom(m_ctrl_text, street);
            return;
        }
    }

    m_id_nagr = -1;
    m_id_tu = -1;


    {
        map<CString, int, less_dom>::iterator it = map_dom.find(street);
        if (it != map_dom.end()) {
            m_id_nagr = it->second;
        }
        else {
            map<CString, int>::const_iterator it = map_nazv2.find(street);
            if (it != map_nazv2.end()) {
                m_id_tu = it->second;
            }
            else {

                map_nazv2.clear();
                map<CString, int>::const_iterator it = map_nazv.find(street);
                if (it != map_nazv.end()) {
                    m_id_tu = it->second;

                    CString q;
                    CAdoFile *ado = kls->m_ado;

                    q.Format("select * from %s", transl("Технические условия"));
                    if (ado->openTable0(q)) {
                        while (!ado->isEOF()) {
                            //        long id = ado->read_long("ID");
                            long id = ado->read_long(transl("Здание"));
                            CString s1 = ado->readStr(transl("Наименование организации, запрашивающей ТУ"));
                            CString s2 = ado->readStr(transl("Наименование объекта"));
                            CString s3 = ado->readStr(transl("Номер ТУ"));

                            s1 += " организация";
                            s2 += " объект";
                            if (s1 == street) {
                                map_nazv2[s3 + " - " + s2] = id;
                            }
                            if (s2 == street) {
                                map_nazv2[s3 + " - " + s1] = id;
                            }
                            ado->MoveNext();
                        }
                        ado->closeTable();
                    }
                    if (map_nazv2.size() > 1) {
                        map<CString, int>::iterator it = map_nazv2.begin();
                        for (int i = m_ctrl_text.GetCount() - 1; i >= 0; i--) m_ctrl_text.DeleteString(i);

                        for (; it != map_nazv2.end(); it++) {
                            m_ctrl_text.AddString(it->first);
                        }
                        return;
                    }
                }

                if (m_id_tu <= 0) {
                    AfxMessageBox(_TR("Нет на карте"), MB_OK|MB_ICONINFORMATION);
                    return;
                }
            }
        }
    }

    CDialog::OnOK();
}

