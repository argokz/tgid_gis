#pragma once

#include "PropertyGrid\test\PropGridDlg.h"

CPropGridDlg *dialog_pr(CWnd *wnd, const char *fn, long idd, const char *capt, int id);

void setPropValueInit(CPropGridDlg *pm_cEditDlg, long idd, CString col, CString val);

bool getPropValue(long idd, CString col, CString &val) ;
bool getPropValue0(long idd, CString col, CString &val) ;
int getPropID(long idd);

