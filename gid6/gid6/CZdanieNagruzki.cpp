// CZdanieNagruzki.cpp : implementation file
//

#include "stdafx.h"
//#include "pch.h"
#include "gid6.h"
#include "CZdanieNagruzki.h"
#include "afxdialogex.h"
#include "ado.h"
#include "InputDia.h"

BOOL isAdmin();

// CZdanieNagruzki dialog

IMPLEMENT_DYNAMIC(CZdanieNagruzki, CDialogEx)

void CZdanieNagruzki::init_q(bool all)
{
    m_all = all;
    m_no_all = !all;

    if (m_str_rn == "") {
        add_q = "";
        add_q2 = "1=0";
        return;
    }


    if (all) {
        add_q = "";
        add_q2 = "1=1";
        add_q2.Format("n.RAYON IN (%s)", m_str_rn);

    }
    else {
        add_q.Format("LEFT JOIN %s z\n"
"ON\n"
"    (n.mkr=z.mkr2 OR n.mkr IS NULL AND z.mkr2 IS NULL)\n"
"    AND\n"
"    (n.street=z.street2 OR n.street IS NULL AND z.street2 IS NULL)\n"
"    AND\n"
"        LOWER(REPLACE(z.house2, ' ', '')) = LOWER(REPLACE(n.house, ' ', ''))\n", m_zdan);

        add_q2 = "z.id IS NULL";
        add_q2.Format("z.id IS NULL AND n.RAYON IN (%s)", m_str_rn);
    }
}


CZdanieNagruzki::CZdanieNagruzki(bool all, bool disable_rn, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_ZDANIE_NAGRUZKI, pParent)
    , m_street_find(_T(""))
    , m_dom_find(_T(""))
{
    m_pDSList = NULL;

    m_disable_rn = disable_rn;

    m_no_all = !all;

    m_ALL = 0;
    m_OTOP = 0;
    m_GVS = 0;
    m_VENT = 0;
    m_PAR = 0;

    m_ADDR = "";
    m_ADM_RAYON = "";
    m_RAYON = "";
    m_IST = "";
    m_TG = "";
    m_UCHASTOK = "";

    init_q(all);

    c_ist = 0;


}

CZdanieNagruzki::~CZdanieNagruzki()
{
}

void CZdanieNagruzki::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
   
    
    DDX_Control(pDX, IDC_MKR, m_ctrl_mkr);
    DDX_Control(pDX, IDC_STREET, m_ctrl_street);
    DDX_Control(pDX, IDC_DOM, m_ctrl_dom);
    DDX_Control(pDX, IDC_RN, m_ctrl_rn);

    DDX_Control(pDX, IDC_MKR_FIND, m_ctrl_mkr_find);
    DDX_Control(pDX, IDC_STREET_FIND, m_ctrl_street_find);
    DDX_Control(pDX, IDC_DOM_FIND, m_ctrl_dom_find);

    DDX_Text(pDX, IDC_STREET_FIND, m_street_find);
    DDX_Text(pDX, IDC_MKR_FIND, m_mkr_find);
    DDX_Text(pDX, IDC_DOM_FIND, m_dom_find);

    DDX_Text(pDX, IDC_ADDR, m_ADDR);
    DDX_Text(pDX, IDC_ADM_RAYON, m_ADM_RAYON);
    DDX_Text(pDX, IDC_RAYON, m_RAYON);
    DDX_Text(pDX, IDC_IST, m_IST);
    DDX_Text(pDX, IDC_TG, m_TG);
    DDX_Text(pDX, IDC_UCHASTOK, m_UCHASTOK);

    DDX_Radio(pDX, IDC_RADIO1, m_no_all);

    DDX_Text(pDX, IDC_ALL, m_ALL);
    DDX_Text(pDX, IDC_OTOP, m_OTOP);
    DDX_Text(pDX, IDC_GVS, m_GVS);
    DDX_Text(pDX, IDC_VENT, m_VENT);
    DDX_Text(pDX, IDC_PAR, m_PAR);

}


BEGIN_MESSAGE_MAP(CZdanieNagruzki, CDialogEx)
  ON_LBN_SELCHANGE(IDC_MKR, OnSelchangeMkr)
  ON_LBN_SELCHANGE(IDC_STREET, OnSelchangeStreet)
  ON_LBN_SELCHANGE(IDC_DOM, OnSelchangeDom)
  ON_EN_CHANGE(IDC_STREET_FIND, OnChangeStreetFind)
  ON_EN_CHANGE(IDC_MKR_FIND, OnChangeMkrFind)

  ON_CLBN_CHKCHANGE(IDC_RN, OnChkChangeRn)
  ON_BN_CLICKED(IDC_RADIO1, OnChangeVal)
  ON_BN_CLICKED(IDC_RADIO2, OnChangeVal)

    ON_LBN_SELCHANGE(IDC_RN, &CZdanieNagruzki::OnLbnSelchangeRn)
END_MESSAGE_MAP()


// CZdanieNagruzki message handlers


CString str_tr(CString s)
{
    s.MakeLower();
    s.Replace(" ", "");
    s.Replace("бульвар.", "");
    s.Replace("бульв.", "");
    s.Replace("бульвар", "");
    s.Replace("ул.", "");
    s.Replace("пер.", "");
    s.Replace("пл.", "");
    s.Replace("площадь", "");
    s.Replace(".", "");
    s.Replace("жирау", "жырау");

    return s;
}


bool almost_equal(CString s1, CString s2)
{
    s1 = str_tr(s1);
    s2 = str_tr(s2);

    return s1 == s2;
}


#include "geofile.h"
#include "Klassif.h"

void CZdanieNagruzki::init_dialog()
{
    m_ctrl_mkr.ResetContent();
    m_ctrl_street.ResetContent();
    m_ctrl_dom.ResetContent();

    CString q;

     q.Format("select distinct n.mkr from nagruzki n %s where %s order by mkr", add_q, add_q2);

    int nn = 0;
    int nn_mkr = -1;
    CString mkr2 = "";

    if (m_ado->openTable0(q)) {
        while ( !m_ado->isEOF() ) {
          CString s = m_ado->readStr(0);

          if (nn_mkr == -1 && almost_equal(s, m_mkr)) {
              nn_mkr = nn;
              mkr2 = s;
          }

          nn += 1;

          m_ctrl_mkr.AddString(s);
          m_ado->MoveNext();
      }
      m_ado->closeTable();
    }

    q.Format("select distinct n.street from nagruzki n %s where %s order by street", add_q, add_q2);

//    CInputDialog inD(AfxGetMainWnd(), "", "?", q);
//    inD.DoModal();
    nn = 0;
    int nn_street = -1;
    CString street2 = "";

    if (m_ado->openTable0(q)) {
        while ( !m_ado->isEOF() ) {
          CString s = m_ado->readStr(0);

          if (nn_street == -1 && almost_equal(s, m_street)) {
              nn_street = nn;
              street2 = s;
          }

          nn += 1;

          m_ctrl_street.AddString(s);
          m_ado->MoveNext();
      }
      m_ado->closeTable();
    }

    if (nn_street >= 0 || nn_mkr >= 0) {
        m_ctrl_mkr.SetCurSel(nn_mkr);
        m_ctrl_street.SetCurSel(nn_street);
        setDoms(mkr2, street2);
    }
    

    m_mkr_find = m_mkr;
    m_street_find = m_street;
    m_dom_find = m_dom;

    m_mkr = "";
    m_street = "";
    m_dom = "";
}

BOOL CZdanieNagruzki::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    KlList* m_kl_list = m_geofile->m_kl_list;
    Klassif* kls = m_kl_list->findKlN(m_zdan);

    if (!kls) return FALSE;

    m_ado = kls->m_ado;

    m_grid1.AttachGrid(this, IDC_MSFLEXGRID1);

    m_pDSList = new _tagDSList;
    m_pDSList->m_dsADO = new CADOImpDatasource;

    m_ctrl_rn.EnableWindow(!m_disable_rn);  

    setGrid("!!", "!!", "!!");

    CString q;
    
    q.Format("SELECT DISTINCT rayon FROM nagruzki ORDER BY rayon");

    int i = 0;

    m_str_rn = "";

     m_ctrl_rn.AddString("Все районы");
     m_ctrl_rn.SetCheck(0, 1);
     c_ist = 1;

     i++;

    if (m_ado->openTable0(q)) {
        while ( !m_ado->isEOF() ) {
            CString s = m_ado->readStr(0);

            CString ss;
            ss.Format("nagruzki/rn/%s", s);

            int on = AfxGetApp()->GetProfileInt(szSection, ss, 1);

            if (!m_no_all) {
                on = 1;
            }

            m_ctrl_rn.AddString(s);
            m_ctrl_rn.SetCheck(i, on);

            if (on) {
                if (m_str_rn != "") m_str_rn += ",";
                m_str_rn += "'";
                m_str_rn += s;
                m_str_rn += "'";
            }
            else {
                if (c_ist == 1) {
                    m_ctrl_rn.SetCheck(0, 0);
                    c_ist = 0;
                }
            }
        
            i++;

//          m_ctrl_rn.SetCheck(i, item.on);

            m_ado->MoveNext();
        }
        m_ado->closeTable();
    }
    init_q(!m_no_all);

    
    init_dialog();


    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CZdanieNagruzki::OnChangeStreetFind()
{
    UpdateData(TRUE);

    char pic[256];

    m_ctrl_street_find.GetLine(0, pic, 255);
    pic[m_ctrl_street_find.LineLength(0)] = 0;

    m_street_find = pic;

    CString q;

    m_ctrl_street.ResetContent();

    q.Format("select distinct n.street from nagruzki n %s where street like '%%%s%%' AND %s order by street", add_q, pic, add_q2);

    if (m_street_find == "")  {
        q.Format("select distinct n.street from nagruzki n %s where %s order by street", add_q, add_q2);
    }

    if (m_ado->openTable0(q)) {
         while ( !m_ado->isEOF() ) {
           CString s = m_ado->readStr(0);
           m_ctrl_street.AddString(s);
           m_ado->MoveNext();
       }
       m_ado->closeTable();
    }

    UpdateData(FALSE);
}


void CZdanieNagruzki::OnChangeMkrFind()
{
    UpdateData(TRUE);

    char pic[256];

    m_ctrl_mkr_find.GetLine(0, pic, 255);
    pic[m_ctrl_mkr_find.LineLength(0)] = 0;

    m_mkr_find = pic;

    CString q;

    m_ctrl_mkr.ResetContent();

    q.Format("select distinct n.mkr from nagruzki n %s where mkr like '%%%s%%' and %s order by mkr", add_q, pic, add_q2);

    if (m_mkr_find == "")  {
        q.Format("select distinct n.mkr from nagruzki n %s where %s order by mkr", add_q, add_q2);
    }
    
    if (m_ado->openTable0(q)) {
         while ( !m_ado->isEOF() ) {
           CString s = m_ado->readStr(0);
           m_ctrl_mkr.AddString(s);
           m_ado->MoveNext();
       }
       m_ado->closeTable();
    }

    UpdateData(FALSE);
}


int decode_dom(const char* s);


struct less_dom2
{
    using result_type = bool;
using first_argument_type = const CString&;
using second_argument_type = const CString&;
    result_type operator() ( const CString arg1, const CString & arg2) const
    {
        int n1 = decode_dom(arg1);
        int n2 = decode_dom(arg2);

        if (n1 < n2) return true;
        if (n1 > n2) return false;

        return arg1 < arg2;
    }
};

void CZdanieNagruzki::setDoms(CString mkr, CString street) 
{
    CString q, wh;

    if (mkr != "" && street == "") {
        wh.Format("mkr='%s'", mkr);
    }
    else if (mkr == "" && street != "") {
        wh.Format("street='%s'", street);
    }
    else {
        wh.Format("mkr='%s' AND street='%s'", mkr, street);
    }

    q.Format("select distinct n.house from nagruzki n %s where %s and %s", add_q, wh, add_q2);


//    CInputDialog inD(AfxGetMainWnd(), "", "?", q);
//    inD.DoModal();


    m_ctrl_dom.ResetContent();

    set<CString, less_dom2> set_str;

    if (m_ado->openTable0(q)) {
        while ( !m_ado->isEOF() ) {
          CString s = m_ado->readStr(0);
//           m_ctrl_dom.AddString(s);

           if (s != "") set_str.insert(s);
           m_ado->MoveNext();
        }
        m_ado->closeTable();
    }


    CString dom;
    int nn = 0;
    int nn_dom = -1;

    auto it = set_str.begin();

    for (; it != set_str.end(); it++) {
        CString s = *it;
        m_ctrl_dom.AddString(s);

        CString s1 = s;
        CString s2 = m_dom;

        s1.MakeLower();
        s1.Replace(" ", "");

        s2.MakeLower();
        s2.Replace(" ", "");

       if (s1 == s2) {
           nn_dom = nn;
           m_dom = "";
           dom = s;
       }
       nn += 1;
    }

    if (nn_dom >= 0) {
        m_ctrl_dom.SetCurSel(nn_dom);

        setNagr(mkr, street, dom);
        setGrid(mkr, street, dom);
    }

    UpdateData(FALSE);
}

void CZdanieNagruzki::OnSelchangeStreet() 
{
    UpdateData(TRUE);

    CString street = "", mkr = "";

    int mk = m_ctrl_mkr.GetCurSel();
    if (mk >= 0) m_ctrl_mkr.GetText(mk, mkr);

    int st = m_ctrl_street.GetCurSel();
    if (st >= 0) m_ctrl_street.GetText(st, street);

    setDoms(mkr, street);

    setGrid("!!", "!!", "!!");

    UpdateData(FALSE);
}

void CZdanieNagruzki::OnSelchangeMkr() 
{
    UpdateData(TRUE);

    CString street = "", mkr = "";

    int mk = m_ctrl_mkr.GetCurSel();
    if (mk >= 0) m_ctrl_mkr.GetText(mk, mkr);

//    int st = m_ctrl_street.GetCurSel();
//    if (st >= 0) m_ctrl_street.GetText(st, street);

    CString q;

    m_ctrl_street.ResetContent();

    q.Format("select distinct street from nagruzki n %s where mkr='%s' and %s order by street", add_q, mkr, add_q2);
    if (mkr == "") {
        q.Format("select distinct street from nagruzki n %s where %s order by street", add_q, add_q2);
    }
    else {
        m_ctrl_street.AddString("");
    }

    m_street = "";
    m_street_find = "";

    if (m_ado->openTable0(q)) {
        while ( !m_ado->isEOF() ) {
          CString s = m_ado->readStr(0);
          m_ctrl_street.AddString(s);
          m_ado->MoveNext();
      }
      m_ado->closeTable();
    }

    setDoms(mkr, "");

    setGrid("!!", "!!", "!!");

    UpdateData(FALSE);
}

void CZdanieNagruzki::setGrid(CString mkr, CString street, CString dom)
{
/*
    m_OTOP = 0;
    m_GVS = 0;
    m_VENT = 0;
    m_PAR = 0;

    m_ADDR = "";
    m_ADM_RAYON = "";
    m_RAYON = "";
    m_IST = "";
    m_TG = "";
    m_UCHASTOK = "";
*/

    CString q, wh;

    if (mkr != "" && street == "") {
        wh.Format("mkr='%s'", mkr);
    }
    else if (mkr == "" && street != "") {
        wh.Format("street='%s'", street);
    }
    else {
        wh.Format("mkr='%s' AND street='%s'", mkr, street);
    }

    q.Format("select n.id, n.name AS 'Наименование',n.dogovor AS 'Номер договора',"
        "n.otop AS 'Отопление, ккал/ч',n.gvs AS 'ГВС, ккал/ч',n.vent AS 'Вентиляция, ккал/ч',n.par AS 'Пар, ккал/ч' "
        "from nagruzki n %s "
        "where %s and house='%s' and %s ORDER BY numb", add_q, wh, dom, add_q2);


//    CInputDialog inD(AfxGetMainWnd(), "", "?", q);
//    inD.DoModal();

    if (m_pDSList->m_dsADO->OpenSQL(q, m_ado->getConnect(), m_ado->m_type_of_net) == UG_ERROR) {
        return;
    }

    m_grid1.SetUniformRowHeight(TRUE);

    m_grid1.AddDataSource(m_pDSList->m_dsADO);
    m_grid1.SetDefDataSource(m_pDSList->m_dsADO->GetID());
    m_grid1.SetGridUsingDataSource(m_pDSList->m_dsADO->GetID());

//    m_grid1.setName("Нагрузки Алматы", virt_data_unknown);

    
    if (isAdmin()) {
        m_grid1.setName("Нагрузки Алматы", virt_data_edit_del);
    }
    else {
        m_grid1.setName("Нагрузки Алматы", virt_data_unknown);
    }
}


void CZdanieNagruzki::setNagr(CString mkr, CString street, CString dom)
{
    CString q, wh;

    m_ALL = 0;
    m_OTOP = 0;
    m_GVS = 0;
    m_VENT = 0;
    m_PAR = 0;

    m_ADDR = "";
    m_ADM_RAYON = "";
    m_RAYON = "";
    m_IST = "";
    m_TG = "";
    m_UCHASTOK = "";

    if (mkr != "" && street == "") {
        wh.Format("mkr='%s'", mkr);
    }
    else if (mkr == "" && street != "") {
        wh.Format("street='%s'", street);
    }
    else {
        wh.Format("mkr='%s' AND street='%s'", mkr, street);
    }

    q.Format("select n.OTOP, n.GVS, n.VENT, n.PAR, n.ADDR, n.ADM_RAYON, n.RAYON, n.IST, n.TG, n.UCHASTOK from nagruzki n %s where %s and house='%s' and %s ORDER BY numb", add_q, wh, dom, add_q2);


//    CInputDialog inD(AfxGetMainWnd(), "", "?", q);
//    inD.DoModal();


    if (m_ado->openTable0(q)) {
         while ( !m_ado->isEOF() ) {

           double OTOP = m_ado->read_double("OTOP");
           double GVS = m_ado->read_double("GVS");
           double VENT = m_ado->read_double("VENT");
           double PAR = m_ado->read_double("PAR");

           m_ADDR = m_ado->readStr("ADDR");
           m_ADM_RAYON = m_ado->readStr("ADM_RAYON");
           m_RAYON = m_ado->readStr("RAYON");
           m_IST = m_ado->readStr("IST");
           m_TG = m_ado->readStr("TG");
           m_UCHASTOK = m_ado->readStr("UCHASTOK");

           m_OTOP += OTOP;
           m_GVS  += GVS;
           m_VENT += VENT;
           m_PAR  += PAR; 

           m_ALL += (OTOP+GVS+VENT+PAR); 

           m_ado->MoveNext();
       }
       m_ado->closeTable();
    }
}

void CZdanieNagruzki::OnSelchangeDom() 
{
    CString q, mkr = "", street = "", dom = "";

    int n1 = m_ctrl_mkr.GetCurSel();
    if (n1 >= 0) m_ctrl_mkr.GetText(n1, mkr);

    int n2 = m_ctrl_street.GetCurSel();
    if (n2 >= 0) m_ctrl_street.GetText(n2, street);

    int n3 = m_ctrl_dom.GetCurSel();
    if (n3 >= 0) m_ctrl_dom.GetText(n3, dom);

    setNagr(mkr, street, dom);
    setGrid(mkr, street, dom);
                                
    UpdateData(FALSE);
}


void CZdanieNagruzki::init(CString mkr, CString street, CString dom)
{
    m_mkr = mkr;
    m_street = street;
    m_dom = dom;
}

void CZdanieNagruzki::OnOK() 
{
    UpdateData(TRUE);

    m_mkr2 = "";
    m_street2 = "";
    m_house2 = "";

    int r1 = m_ctrl_mkr.GetCurSel();
    int r2 = m_ctrl_street.GetCurSel();
    int r3 = m_ctrl_dom.GetCurSel();

    if (r1 >= 0) {
        m_ctrl_mkr.GetText(r1, m_mkr2);
    }
    if (r2 >= 0) {
        m_ctrl_street.GetText(r2, m_street2);
    }
    if (r3 >= 0) {
        m_ctrl_dom.GetText(r3, m_house2);
    }

    CDialogEx::OnOK();
}


void CZdanieNagruzki::OnChkChangeRn()
{
  int c = m_ctrl_rn.GetCheck(0);

  if (c != c_ist) {
    c_ist = c;
    for ( int i = 1; i < m_ctrl_rn.GetCount(); i++ ) {
        m_ctrl_rn.SetCheck(i, c);
    }
  }
    
    
    m_str_rn = "";

    for ( int i = 1; i < m_ctrl_rn.GetCount(); i++ ) {
        CString s;
        m_ctrl_rn.GetText(i, s);
        int on = m_ctrl_rn.GetCheck(i);

        CString ss;
        ss.Format("nagruzki/rn/%s", s);

        if (on) {
            if (m_str_rn != "") m_str_rn += ",";
            m_str_rn += "'";
            m_str_rn += s;
            m_str_rn += "'";
        }

        AfxGetApp()->WriteProfileInt(szSection, ss, on);
    }
    init_q(m_no_all);
    init_dialog();
}


void CZdanieNagruzki::OnChangeVal()
{
    UpdateData(TRUE);

    CString qq;
//    qq.Format("%d", m_no_all);
//    AfxMessageBox(qq);

    init_q(!m_no_all);
    init_dialog();

    UpdateData(FALSE);
}



void CZdanieNagruzki::OnLbnSelchangeRn()
{
    // TODO: Add your control notification handler code here
}
