#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"


//#include "WinccDialog.h"


bool CGidrView::wincc_info(const CFPoint& point)
{
	AfxMessageBox("WinCC", MB_OK|MB_ICONINFORMATION);
	return true;
}

#include <process.h>

void CGidrView::OnWincc()
{
#if 0
	//  ShellExecute(NULL, "open", "C:/astana/Áàçû/Ñõåìû/ÖÒÐÏ2/öòðï2.jpg", NULL, NULL, SW_MAXIMIZE);

	CWinCCInitDialog dlg(AfxGetMainWnd());

	dlg.m_project = "cc_pid_cc_09_01_12_10_57_58r";
	dlg.m_server = "133-3";
	//  dlg.m_timeout = 30;
	//  dlg.m_on = TRUE;
	dlg.m_path_baza = "";
	dlg.m_path_excel = "";
	dlg.m_tag_tn = "";
	//  dlg.m_provider = "WinCCOLEDBProvider";

	dlg.m_project = AfxGetApp()->GetProfileString(szSection, "project", dlg.m_project);
	dlg.m_server = AfxGetApp()->GetProfileString(szSection, "server", dlg.m_server);
	///  dlg.m_timeout = AfxGetApp()->GetProfileInt(szSection, "timeout", dlg.m_timeout);
	////  dlg.m_on = AfxGetApp()->GetProfileInt(szSection, "wincc", dlg.m_on);
	dlg.m_path_baza = AfxGetApp()->GetProfileString(szSection, "path_baza", dlg.m_path_baza);
	dlg.m_path_excel = AfxGetApp()->GetProfileString(szSection, "path_excel", dlg.m_path_excel);
	dlg.m_tag_tn = AfxGetApp()->GetProfileString(szSection, "tag_tn", dlg.m_tag_tn);
	//  dlg.m_provider = AfxGetApp()->GetProfileString(szSection, "WinCCOLEDBProvider", dlg.m_provider);

	if (dlg.DoModal() == IDOK) {
		AfxGetApp()->WriteProfileString(szSection, "project", dlg.m_project);
		AfxGetApp()->WriteProfileString(szSection, "server", dlg.m_server);
		AfxGetApp()->WriteProfileString(szSection, "path_baza", dlg.m_path_baza);
		AfxGetApp()->WriteProfileString(szSection, "path_excel", dlg.m_path_excel);
		AfxGetApp()->WriteProfileString(szSection, "tag_tn", dlg.m_tag_tn);
	}
#endif
}
