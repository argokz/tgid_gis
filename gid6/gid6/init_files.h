#pragma once

bool open_special(const char *kln);
void initLookup(CString database, CString database2);
void initStruct1();

bool find_special(CString ss);
CString find_old_name_col(const char* tn1, const char* fn_new1);
CString find_new_name_col(const char* tn1, const char* fn_new1);




void initTableRusName(CString database, CString database2);
void initColumnRusName(CString database, CString database2);

