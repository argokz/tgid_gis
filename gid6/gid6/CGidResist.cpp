// CGidResist.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "CGidResist.h"
#include "afxdialogex.h"


// CGidResist dialog

IMPLEMENT_DYNAMIC(CGidResist, CDialogEx)

CGidResist::CGidResist(CWnd* pParent, double len)
	: CDialogEx(IDD_UTP_GID_RESIST, pParent)
{
  m_edit1 = 0;
  m_edit2 = len;
  m_edit3 = 0;
}

CGidResist::~CGidResist()
{
}

void CGidResist::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

  DDX_Text(pDX, IDC_UTEDIT1, m_edit1);
  DDX_Text(pDX, IDC_UTEDIT2, m_edit2);
  DDX_Text(pDX, IDC_UTEDIT3, m_edit3);

}


BEGIN_MESSAGE_MAP(CGidResist, CDialogEx)
  ON_EN_CHANGE(IDC_UTEDIT1, OnChange)
  ON_EN_CHANGE(IDC_UTEDIT2, OnChange)
  ON_EN_CHANGE(IDC_UTEDIT3, OnChange)
END_MESSAGE_MAP()


// CGidResist message handlers


void CGidResist::OnChange()
{
  UpdateData(TRUE);

	if (m_edit2 != 0) {	
    m_edit3 = m_edit1/m_edit2/m_edit2;
	}
	else {
    m_edit3 = 0;
	}
  UpdateData(FALSE);
}

