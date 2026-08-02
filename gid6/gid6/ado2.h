#pragma once

#include "PropertyGrid\test\PropGridDlg.h"

typedef void CLINE2;


CPropertyGrid* ado_info_id(CWnd* wnd, CAdoFile* ado, const char* tn, int id, const char* capt, TYP_OF_P typ_of_p, const char* tn_txt, long ID);
CPropertyGrid* ado_info(CWnd* wnd, CAdoFile* ado, const char* tn, int id, const char* capt, TYP_OF_P typ_of_p, const char* tn_txt, bool autodelete);
//CPropertyGrid* ado_infoView(CWnd *wnd, CAdoFile *ado, const char *tn, int id, const char *capt, TYP_OF_P typ_of_p, const char *tn_txt = NULL);
CPropertyGrid* ado_infoView(CWnd *wnd, CAdoFile *ado, const char *tn, int id, const char *capt, TYP_OF_P typ_of_p, const char *tn_txt, bool autodelete);
int ado_infoInit(CAdoFile *ado, const char *tn);

int ado_info_line2(CWnd* wnd, CAdoFile* ado, CLINE2* line, const char* tn1, int id1, const char* tn2, int id2, int idN1, int idN2, int fileID, const char* tn_out, int idgP, int idgO, const char* capt);
int ado_info_line_big2(CWnd* wnd, CAdoFile* ado, const char* tn2, int id2, int idN1, int idN2, const char* capt);

int addRemoved(CAdoFile *ado, int typeID, int id, const char *text);
