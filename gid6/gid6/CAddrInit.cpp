// CAddrInit.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "CAddrInit.h"
#include "afxdialogex.h"
#include "ado.h"
#include "klassif.h"


// CAddrInit dialog

IMPLEMENT_DYNAMIC(CAddrInit, CDialogEx)

CAddrInit::CAddrInit(CWnd* pParent, const Klassif *kls, CString mkr, CString str, CString dom)
    : CDialogEx(IDD_ADDR_INIT, pParent)
{
    m_kls = kls;

    m_n1 = -1;
    m_n2 = -1;
    m_n3 = -1;

    m_s1 = mkr;
    m_s2 = str;
    m_s3 = dom;
}

CAddrInit::~CAddrInit()
{
}

void CAddrInit::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, m_combo1);
    DDX_Control(pDX, IDC_COMBO2, m_combo2);
    DDX_Control(pDX, IDC_COMBO3, m_combo3);

    DDX_CBIndex(pDX, IDC_COMBO1, m_n1);
    DDX_CBIndex(pDX, IDC_COMBO2, m_n2);
    DDX_CBIndex(pDX, IDC_COMBO3, m_n3);
}


BEGIN_MESSAGE_MAP(CAddrInit, CDialogEx)
END_MESSAGE_MAP()


BOOL CAddrInit::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CAdoFile *ado = m_kls->m_ado;

    CString q;
    q.Format("SELECT * FROM %s", m_kls->nazv);

    list<CString> lst;

    if (ado && ado->openTable0(q)) {
        for ( int i = 0; i < ado->NFlds(); i++ ) {
            CAdoField af = ado->Field(i);

            CString name = af.name;
            const CColName *col_name = findColumnRusName(ado->m_schema, m_kls->nazv, af.name);

            if (name.CompareNoCase(m_s1) == 0) m_n1 = i;
            if (name.CompareNoCase(m_s2) == 0) m_n2 = i;
            if (name.CompareNoCase(m_s3) == 0) m_n3 = i;

            if (col_name) {
                name = col_name->name;
            }

            map_fields[i] = af.name;

            m_combo1.AddString(name);
            m_combo2.AddString(name);
            m_combo3.AddString(name);
        }
        ado->closeTable();
    }


    UpdateData(FALSE);

    return TRUE;
}


void CAddrInit::OnOK() 
{
    UpdateData(TRUE);

    map <int, CString>::const_iterator it;

    m_s1 = "";
    m_s2 = "";
    m_s3 = "";

    it = map_fields.find(m_n1);  if (it != map_fields.end()) m_s1 = it->second;
    it = map_fields.find(m_n2);  if (it != map_fields.end()) m_s2 = it->second;
    it = map_fields.find(m_n3);  if (it != map_fields.end()) m_s3 = it->second;
    
    CDialogEx::OnOK();
}



// CAddrInit message handlers
