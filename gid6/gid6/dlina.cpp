#include "stdafx.h"
#include "gid6.h"
#include "gidrView.h"
#include "win.h"
#include "ado.h"


// Протяженность

void CGidrView::OnProtAll() { AfxMessageBox("!!! 12", MB_OK|MB_ICONINFORMATION); }
void CGidrView::OnProtKot() { AfxMessageBox("!!! 13", MB_OK|MB_ICONINFORMATION); }

//void viewQ2(CAdoFile* ado, const char *db, const char *q, const char *tn, const char *head, VIRT_DATA typ, CString add_text, const char *title, bool all = false);


void CGidrView::OnProtMag() 
{
    CString q = readFile(argpath()+"\\sql2\\dlina_mag.sql");

    viewQ2(getGidAdoName(), q, "", "", virt_data_unknown, "", _TR("Протяженность Магистралей"));
}


void CGidrView::OnProtNew() { AfxMessageBox("!!! 15", MB_OK|MB_ICONINFORMATION); }

void CGidrView::OnProtRs() 
{ 
    CString q = readFile(argpath()+"\\sql2\\dlina_rs.sql");

    viewQ2(getGidAdoName(), q, "", "", virt_data_unknown, "", _TR("Протяженность Распредсетей"));
}
