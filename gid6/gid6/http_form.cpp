// Scroll.cpp : implementation file
//

//#define __ADO

#include "stdafx.h"
#include "resource.h"
#include "Scroll.h"
#include "InputDia.h"
#include "PrintIni.h"
#include "win.h"
#include <math.h>
#include "MapDlg.h"
#include "mmenu.h"
#include "geomenu.h"
#include "ps_alma.h"
#include "json.h"

#include "ado.h"
#include "maptabs.h"

#include <json.hpp>

#include <sstream>
#include <iostream>

#define ZDAN _TR("Здания ТУ")


static map <CString, map<CString, CString> > map_path;


CString http_get_q(const char *file, const char *q, int r1, int r2, bool rus_name);


bool readTab(const char *tn, const char *real_tn, int table_num,
  map <int, CString> &map_1, map <int, map <int, CString> > &map_2,  map <CString, CString> &map_3
);

CString toUTF8(const CString &qq);

nlohmann::json get_add(const char *name, const char *key, const char *typeField)
{
    nlohmann::json j;

    j["name"] = toUTF8(name);
    j["key"] = toUTF8(key);
    j["value"] = nullptr;
    j["type"] = "input";
    j["typeField"] = typeField;

    return j;
}



nlohmann::json get_j(nlohmann::json &j, const string &key)
{
    nlohmann::json::iterator it = j.find(key);

    if (it != j.end()) {
        return it.value();
    }
    nlohmann::json j0;
    return j0;
}

string get_j_string(nlohmann::json &j, const string &key)
{
    nlohmann::json::iterator it = j.find(key);

    if (it != j.end()) {
        if (it->is_string()) {
            return it.value();
        }
    }
    return "";
}

double get_j_float(nlohmann::json &j, const string &key)
{
    nlohmann::json::iterator it = j.find(key);

    if (it != j.end()) {
        if (it->is_number()) {
            return it.value();
        }
    }
    return 0;
}



int get_j_int(nlohmann::json &j, const string &key)
{
    nlohmann::json::iterator it = j.find(key);

    if (it != j.end()) {
        if (it->is_number()) {
            return it.value();
        }
    }
    return 0;
}



nlohmann::json read_q2(nlohmann::json &j, CAdoFile *ado, const char *q, const char *v)
{
    int n = 0;

    j["selected"] = nullptr;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int id = ado->read_long(0);
            CString vv;
            
             vv = ado->readStr(1);

            if (vv == v) {
                j["selected"] = id;
            }

            j["options"][n]["value"] = id;
            j["options"][n]["text"] = toUTF8(vv);
            n++;
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return j;
}

nlohmann::json read_q1(nlohmann::json &j, CAdoFile *ado, const char *q, const char *v)
{
    int n = 0;

    j["selected"] = nullptr;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
//            int id = ado->read_long(0);
            CString vv = ado->readStr(0);

            if (vv == v) {
                j["selected"] = toUTF8(vv);
            }

            j["options"][n]["value"] = toUTF8(vv);
            j["options"][n]["text"] = toUTF8(vv);
            n++;
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return j;
}


CString getRealPath(CString code);


nlohmann::json form(nlohmann::json & j_form, CAdoFile *ado, const char *table, bool is_new)
{
    nlohmann::json j;
    string value = "";
    string type = get_j_string(j_form, "type");

    j["path"] = nullptr;


    string key = get_j_string(j_form, "key");
    if (!is_new) {
        value = ado->readStrDateJSON(key.c_str());
        if (type == "upload") {
            string path = get_j_string(j_form, "path");
            CString p = getRealPath(path.c_str());

            CString pp = value.c_str();
            CString path2 = value.c_str();

            if (pp.Find(p) == 0) {
                path2 = pp.Mid(p.GetLength());
            }
            j["path"] = toUTF8(path2);


            map_path[table][key.c_str()] = p;
        }
    }

    if (type == "select") {
        nlohmann::json j_q = get_j(j_form, "q");
        if (!j_q.is_null()) {
            string q = j_q;
            read_q2(j, ado, q.c_str(), value.c_str());
        }
    }

    
    if (type == "select1") {
        nlohmann::json j_q = get_j(j_form, "q");
        if (!j_q.is_null()) {
            string q = j_q;
            read_q1(j, ado, q.c_str(), value.c_str());
        }
        type = "select";
    }
    
    if (type == "inputLarge") {
        nlohmann::json j_q = get_j(j_form, "q");
        if (!j_q.is_null()) {
            string q = j_q;
            read_q2(j, ado, q.c_str(), value.c_str());
        }

        j["searchInput"] = "";
        j["showLarge"] = false;
    }



    string name = get_j_string(j_form, "name");
    nlohmann::json j_typeField = get_j(j_form, "typeField");
    nlohmann::json j_required = get_j(j_form, "required");

    j["key"] = key;
    j["name"] = name;
    j["type"] = type;


    if (value == "")
        j["value"] = nullptr;
    else 
        j["value"] = toUTF8(value.c_str());



    j["step"] = "any";

    if (!j_typeField.is_null()) {
        string typeField = j_typeField;
        j["typeField"] = typeField;

        if (value == "" && typeField == "number" && !j_required.is_null()) {
            j["value"] = "0";
        }
    }
    j["required"] = !j_required.is_null();

    return j;

}


CString http_get_form(const char *table0, int id)
{
    CString pp;
    CString s, fn;

//    CString tn = "tehnicheskie_usloviya";
    CString tn = table0;

    fn.Format("%s\\tab2\\%s.json", argpath(), table0);
        
    s = readFile(fn);

    s.Replace("\n", " ");
    s.Replace("\r", " ");

    nlohmann::json j0;
    
    j0 = nlohmann::json::parse((const char *)s);

    nlohmann::json j;

    stringstream  ss;


    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZDAN));

    if (!kls) return "";

    CAdoFile *ado = kls->m_ado;


    string qq = get_j_string(j0, "q");
    CString q = qq.c_str();


//    CString q
//    q.Format("SELECT * FROM %s WHERE id=%d", tn, id);
    pp.Format("%d", id);
    q.Replace("$ID$", pp);


    if (!ado->openTable0(q)) return "";

    string title = get_j_string(j0, "title");

//    j["id"] = id;
    j["title"] = title;

    nlohmann::json j_tabs = get_j(j0, "tabs");

    int sz = j_tabs.size();

    for (int i = 0; i < sz; i++) {
        nlohmann::json j_tab = j_tabs[i];
    
        string j_type  = get_j_string(j_tab, "type");
        string title_tabs = get_j_string(j_tab, "title");
        j["tabs"][i]["title"] = title_tabs;

        bool is_new = j_type == "add";

        nlohmann::json j_forms = get_j(j_tab, "form");
        int sz_form = j_forms.size();

        j["tabs"][i]["type"] = is_new ? "add" : "edit";

        j["tabs"][i]["table"] = nullptr;
        j["tabs"][i]["form"] = nullptr;
        
        
        string table = get_j_string(j_tab, "table");

        if (table != "") { 
            CString ss = http_get_q(ado->m_schema, table.c_str(), 0, 0, true);

            nlohmann::json j_table = nlohmann::json::parse((const char *)ss);
            nlohmann::json j_data = get_j(j_table, "data");
            j["tabs"][i]["table"]["data"] = j_data;
            j["tabs"][i]["table"]["info"]["db"] = ado->m_schema;
            j["tabs"][i]["table"]["info"]["type"] = "table";
            j["tabs"][i]["table"]["info"]["name"] = "changeTU";
            j["tabs"][i]["table"]["info"]["text"] = "changeTU";
        }
        else {
            for (int k = 0; k < sz_form; k++) {
                nlohmann::json j_form = j_forms[k];
                string type = get_j_string(j_form, "type");

                if (type == "group") {
                    string groupName = get_j_string(j_form, "groupName");

                    j["tabs"][i]["form"][k]["name"] = nullptr;
                    j["tabs"][i]["form"][k]["show"] = true;
                    j["tabs"][i]["form"][k]["type"] = type;
                    j["tabs"][i]["form"][k]["groupName"] = groupName;


                    nlohmann::json j_forms2 = get_j(j_form, "data");
                    int sz_form2 = j_forms2.size();

                    for (int kk = 0; kk < sz_form2; kk++) {
                        nlohmann::json j_form2 = j_forms2[kk];

                        nlohmann::json jj = form(j_form2, ado, table0, is_new);
                        j["tabs"][i]["form"][k]["data"][kk] = jj;
                    }
                }
                else {
                    nlohmann::json jj = form(j_form, ado, table0, is_new);
                    j["tabs"][i]["form"][k] = jj;
                }
            }
        }
    }


    return j.dump().c_str();
}


void set_form(nlohmann::json &j, CAdoFile *ado)
{
    string type = get_j_string(j, "type");

    int sz = j.size();
    for (int i = 0; i < sz; i++) {
        nlohmann::json j0 = j[i];

        if (type == "group") {
            nlohmann::json j2 = get_j(j0, "data");
            int sz2 = j2.size();

            for (int k = 0; k < sz2; k++) {
                nlohmann::json j0 = j2[k];
                string key = get_j_string(j0, "key");
                string value = get_j_string(j0, "value");
                string selected = get_j_string(j0, "selected");

                //                    ss << key << " : " << value << selected << "\n";
                ado->write(key.c_str(), value.c_str());
            }
        }
        else {
            string key = get_j_string(j0, "key");
            string value = get_j_string(j0, "value");
            string selected = get_j_string(j0, "selected");


            //               ss << key << " : " << value << selected << "\n";
            ado->write(key.c_str(), value.c_str());
        }
    }

}




CString http_set_form(const char *table1, int id1, const char *data)
{

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZDAN));
    if (!kls) return "";
    CAdoFile *ado = kls->m_ado;

    nlohmann::json j;

    j = nlohmann::json::parse((const char *)data);

    int id = atoi(get_j_string(j, "id").c_str());
    string table = get_j_string(j, "tableName");


    string type = get_j_string(j, "type");

    j = get_j(j, "data");

    bool is_new = type == "add";

    if (is_new) {
        if (table == "tehnicheskie_usloviya") table = "changeTU";


        ado->AddNew(table.c_str());
        set_form(j, ado);
        ado->Update();
    }
    else {
        ado->Edit(table.c_str(), id);
        set_form(j, ado);
        ado->Update();
    }

//    AfxMessageBox(ss.str().c_str(), MB_OK|MB_ICONINFORMATION);

    return "";
}


CString http_delete_form(const char *table1, int id1, const char *data)
{

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZDAN));
    if (!kls) return "";
    CAdoFile *ado = kls->m_ado;

    nlohmann::json j;

    j = nlohmann::json::parse((const char *)data);

    int id = atoi(get_j_string(j, "id").c_str());
    string table = get_j_string(j, "tableName");

    CString q;

    q.Format("DELETE FROM %s WHERE ID=%d", table.c_str(), id);
    ado->Execute(q);

//    AfxMessageBox(ss.str().c_str(), MB_OK|MB_ICONINFORMATION);

    return "";
}


CString http_get_form2(const char *table, int id)
{
    CString pp;
    CString s, fn;

    bool is_new = false;

//    CString tn = "tehnicheskie_usloviya";
    CString tn = table;

    fn.Format("%s\\tab2\\%s.json", argpath(), table);
        
    s = readFile(fn);

    s.Replace("\n", " ");
    s.Replace("\r", " ");

    nlohmann::json j0;
   

    j0 = nlohmann::json::parse((const char *)s);

    nlohmann::json j;

    stringstream  ss;


    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZDAN));

    if (!kls) return "";

    CAdoFile *ado = kls->m_ado;


    string qq = get_j_string(j0, "q");
    CString q = qq.c_str();

    pp.Format("%d", id);
    q.Replace("$ID$", pp);

    if (!ado->openTable0(q)) return "";

    string title = get_j_string(j0, "title");

//    j["id"] = id;
    j["title"] = title;

    nlohmann::json j_forms = get_j(j0, "form");
    int sz_form = j_forms.size();

    j["type"] = is_new ? "add" : "edit";
    j["table"] = nullptr;
    j["form"] = nullptr;
    
        for (int k = 0; k < sz_form; k++) {
            nlohmann::json j_form = j_forms[k];
            string type = get_j_string(j_form, "type");

            if (type == "group") {
                string groupName = get_j_string(j_form, "groupName");

                j["form"][k]["name"] = nullptr;
                j["form"][k]["show"] = true;
                j["form"][k]["type"] = type;
                j["form"][k]["groupName"] = groupName;


                nlohmann::json j_forms2 = get_j(j_form, "data");
                int sz_form2 = j_forms2.size();

                for (int kk = 0; kk < sz_form2; kk++) {
                    nlohmann::json j_form2 = j_forms2[kk];

                    nlohmann::json jj = form(j_form2, ado, table, is_new);
                    j["form"][k]["data"][kk] = jj;
                }
            }
            else {
                nlohmann::json jj = form(j_form, ado, table, is_new);
                j["form"][k] = jj;
            }
    }
    


    return j.dump().c_str();
}



CString http_edit_field(const char *table, int id)
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZDAN));
    if (!kls) return "";
    CAdoFile *ado = kls->m_ado;
    if (!ado) return "";

    CString s = http_get_form2(table, id);
    
    return s;
}


CString http_uploadFileForm(const char *path, const char *tmpn, const char *filename, const char *key, const char *tableName, int id)
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZDAN));
    if (!kls) return "";
    CAdoFile *ado = kls->m_ado;
    if (!ado) return "";

    CString ppp = path;

    if (IsFile(path)) {

        map <CString, map<CString, CString> >::const_iterator it1 = map_path.find(tableName);
        if (it1 != map_path.end()) {
            map<CString, CString>::const_iterator it2 = it1->second.find(key);
            if (it2 != it1->second.end()) {
                CString p = it2->second;
                CString pp = path;

                if (pp.Find(p) == 0) {
                    ppp = pp.Mid(p.GetLength());
                }
            }
        }


        HINSTANCE hi = ShellExecute(AfxGetMainWnd()->m_hWnd, "open", path, NULL, NULL, SW_SHOWNORMAL);
    }

    ado->Edit(tableName, id);
    ado->write(key, ppp) ;
    ado->Update();
  
    return "";
}
