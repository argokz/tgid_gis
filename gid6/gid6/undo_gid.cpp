#include "stdafx.h"
#include "gid6.h"
#include "gidrView.h"
#include "ado.h"
#include "win.h"
#include "mmenu.h"
#include "mmenu2.h"


struct ChangedObject {
    long id;
    long changeTypeID;
    long changedID;
    long old_id;
    CString text;
    COleDateTime dt;
};

static map<int, ChangedObject> map_lst;
static list<ChangedObject> lst;

CString getNameChT(int id)
{
  switch(id) {
  case CH_T_DELETE_LINE : return _TR("Удален участок");
  case CH_T_DELETE_NODE : return _TR("Удален узел");
  case CH_T_DELETE_CXEMA_BEGIN : return "";
  case CH_T_DELETE_CXEMA_END : return _TR("Удалена схема");
  case CH_T_MOVE_NODE : return _TR("Передвинут узел");
  case CH_T_MOVE_LINE : return _TR("Передвинут участок");
  case CH_T_EDIT : return _TR("Редактирование");

  case CH_T_ADD_LINE : return _TR("Добавлен участок");
  case CH_T_ADD_NODE : return _TR("Добавлен узел");

  case CH_T_RENAME_LINE : return _TR("Изменен тип участка");
  case CH_T_RENAME_NODE : return _TR("Изменен тип узла");


  case CH_T_DELETE_GEO : return _TR("Удален объект геобазы");
  case CH_T_ADD_GEO : return _TR("Добавлен объект геобазы");
  case CH_T_MOVE_GEO : return _TR("Передвинут объект геобазы");

  }
  return _TR("Ошибка");
}

CString getRemovedNodeName(CAdoFile *ado, int id)
{
    CString ret = "";
    CString q;
    q.Format("SELECT n.id, ec.name, n.externalNodeName FROM nodes n JOIN externalCodes ec ON ec.id=n.externalCodeID WHERE n.idRemoved=%d", id);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            long id = ado->read_long(0);
            CString kod = ado->readStr(1);
            CString name = ado->readStr(2);
            ret = kod + " " + name;
        }
        ado->closeTable();
    }
    return ret;
}

CString getMovedNodeName(CAdoFile *ado, int id)
{
    CString ret = "";
    CString q;
    q.Format("SELECT n.id, ec.name, n.externalNodeName FROM nodes n JOIN externalCodes ec ON ec.id=n.externalCodeID WHERE n.id=%d", id);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            long id = ado->read_long(0);
            CString kod = ado->readStr(1);
            CString name = ado->readStr(2);
            ret = kod + " " + name;
        }
        ado->closeTable();
    }
    return ret;
}


CString getRemovedLineName(CAdoFile *ado, int id)
{
    CString ret = "";
    CString q;
    q.Format("SELECT l.id, CONCAT(ec1.name, ' ', n1.externalNodeName, ' - ', ec2.name, ' ', n2.externalNodeName) FROM linesobj l JOIN nodes n1 ON n1.id=l.nodeID1 JOIN nodes n2 ON n2.id=l.nodeID2 JOIN externalCodes ec1 ON ec1.id=n1.externalCodeID JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID WHERE l.idRemoved=%d", id);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            long id = ado->read_long(0);
            ret = ado->readStr(1);
        }
        ado->closeTable();
    }
    return ret;
}


#include <json.hpp>

string get_j_string(nlohmann::json& j, const string& key);


std::string ansi_to_utf8(const std::string& ansiStr);
std::string utf8_to_ansi(const std::string & utf8Str);


bool undo_edit(CAdoFile *ado, ChangedObject &co, int &fileID)
{
    long affected;
    CString q;
    CString qq = "";

    try {
        std::string utf8_text = ansi_to_utf8((const char*)co.text);
        nlohmann::json json_data = nlohmann::json::parse(utf8_text.c_str());

        for (const auto& item : json_data) {
            for (auto it = item.begin(); it != item.end(); ++it) {
                std::string topKey = it.key();  // Имя верхнеуровневого ключа
                CString tn = it.key().c_str();
                auto value = it.value();        // Значение, которое может быть объектом            

                if (value.is_object()) {
                    int id = value["id"];

                    for (auto inner_it = value.begin(); inner_it != value.end(); ++inner_it) {
                        std::string innerKey = inner_it.key();
                        auto innerValue = inner_it.value();

                        CString col = utf8_to_ansi(innerKey).c_str();
                        CString s = utf8_to_ansi(innerValue.dump()).c_str();

                        if (innerValue.is_string()) {
                            s = utf8_to_ansi(innerValue).c_str();
                            if (s.Left(2) == "N'") {
//                                s = s.Mid(1, s.GetLength() - 2);  // Обработать 'Текст'
                            }
                            else {
                                s = "'" + s + "'";
                            }
                        }


                        if (col != "id") {
                            CString ss;
                            ss.Format("[%s] = %s", col, s);
                            if (qq != "") qq += ",\n";
                            qq += ss;
                        }
                    }

                    q.Format("UPDATE [%s] SET\n%s where id=%d", tn, qq, id);
                    //            AfxMessageBox(q);
                    bool ret = ado->Execute(q, &affected);
                    if (!ret) {
                        AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
                        return false;
                    }
                    else {
                        q.Format("DELETE FROM changed_object WHERE id=%d", co.id);
                        ret = ado->Execute(q, &affected);
                    }
                }
                else {
                    // Если значение не объект, выводим его напрямую
    //                std::cout << "Значение: " << value << std::endl;
                }
            }
        }
    }
    catch (nlohmann::json::exception &ex) {
        log1((const char*)co.text);
        AfxMessageBox(ex.what());
    }

    return true;
}


bool undo_move_node(CAdoFile *ado, ChangedObject &co, int &fileID)
{
    long affected;
    CString q;
    double x, y;

    if (sscanf(co.text, "%lg %lg", &x, &y) != 2) {
        AfxMessageBox(_TR("Ошибка"), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    q.Format("SELECT id, fileID FROM nodes WHERE id=%d", co.id);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            fileID = ado->read_long("fileID");
        }
        ado->closeTable();
    }

    q.Format("UPDATE nodes SET x=%g, y=%g WHERE id=%d", x, y, co.changedID);

    bool ret = ado->Execute(q, &affected);
    if (ret) {
        reset_shape_node(ado, co.changedID);
        
        q.Format("DELETE FROM changed_object WHERE id=%d", co.id);
        ret = ado->Execute(q, &affected);
    }

    if (!ret) {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    return true;
}

bool undo_move_line(CAdoFile *ado, ChangedObject &co, int &fileID)
{
    long id = -1;
    long affected;
    CString q;

    q.Format("SELECT n1.fileID FROM linesobj l "
        " JOIN nodes n1 ON l.nodeID1=n1.id "
        " WHERE l.id=%d", co.changedID);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            fileID = ado->read_long("fileID");
        }
        ado->closeTable();
    }

    q.Format("UPDATE linesobj SET coords='%s' WHERE id=%d", co.text, co.changedID);

    bool ret = ado->Execute(q, &affected);
    if (ret) {
        reset_shape_line(ado, co.changedID);
        q.Format("DELETE FROM changed_object WHERE id=%d", co.id);
        ret = ado->Execute(q, &affected);
    }

    if (!ret) {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    return true;
}


bool undo_node(CAdoFile *ado, ChangedObject &co, int &fileID)
{
    long id = -1;
    long affected;
    CString q;

    q.Format("SELECT id, fileID FROM nodes WHERE idRemoved=%d", co.id);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            id = ado->read_long("id");
            fileID = ado->read_long("fileID");
        }
        ado->closeTable();
    }
    if (id <= 0) {
        AfxMessageBox(_TR("Ошибка"), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    q.Format("UPDATE nodes SET removed=0, idRemoved=NULL WHERE idRemoved=%d", co.id);

    bool ret = ado->Execute(q, &affected);
    if (ret) {
        q.Format("DELETE FROM changed_object WHERE id=%d", co.id);
        ret = ado->Execute(q, &affected);
    }

    if (!ret) {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    return true;
}


bool undo_line(CAdoFile *ado, ChangedObject &co, int &fileID)
{
    bool quit = false;
    CString q;
    q.Format("SELECT n1.fileID, CONCAT(ec1.name, ' ', n1.externalNodeName) AS name1, n1.removed AS n1r, "
                             "  CONCAT(ec2.name, ' ', n2.externalNodeName) AS name2, n2.removed AS n2r FROM linesobj l "
        " JOIN nodes n1 ON l.nodeID1=n1.id "
        " JOIN nodes n2 ON l.nodeID2=n2.id "
        " JOIN externalCodes ec1 on ec1.id = n1.externalCodeID "
        " JOIN externalCodes ec2 on ec2.id = n2.externalCodeID "
        " WHERE l.idRemoved=%d", co.id);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            CString s1 = ado->readStr("name1");
            CString s2 = ado->readStr("name2");
            long l1 = ado->read_long("n1r");
            long l2 = ado->read_long("n1r");
            fileID = ado->read_long("fileID");

            if (l1 || l2) {
                q.Format(_TR("Отмена невозможна, нужно откатить удаление узла %s "), l1 ? s1 : s2);
                AfxMessageBox(q, MB_OK|MB_ICONINFORMATION);
                quit = true;
            }
        }

        ado->closeTable();
    }
    long affected;
    if (quit) return false;

    q.Format("UPDATE linesobj SET removed=0, idRemoved=NULL WHERE idRemoved=%d", co.id);

    bool ret = ado->Execute(q, &affected);
    if (ret) {
        q.Format("DELETE FROM changed_object WHERE id=%d", co.id);
        ret = ado->Execute(q, &affected);
    }

    if (!ret) {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
        return false;
    }
    return true;
}


bool undo_gid(CAdoFile *ado, int id, int &fileID)
{
  map<int, ChangedObject>::const_iterator it = map_lst.find(id);
  if (it == map_lst.end()) {
    AfxMessageBox(_TR("Ошибка"), MB_OK|MB_ICONINFORMATION);
    return false;
  }

  bool ret = false;

  CString s;
  ChangedObject co = it->second;

//  CString dts = co.dt.Format("%Y-%m-%d %H:%M:%S");

  CString dts = dateTimeToString(co.dt);
  

  if (co.changeTypeID == CH_T_DELETE_NODE) {
      s.Format(_TR("Восстановить узел %s удаленный %s?"), getRemovedNodeName(ado, co.id), dts);
      if (AfxMessageBox(s, MB_YESNO) == IDYES) {
          ret = undo_node(ado, co, fileID);
      }
  }
  else if (co.changeTypeID == CH_T_DELETE_LINE) {
      s.Format(_TR("Восстановить линию %s удаленную %s?"), getRemovedLineName(ado, co.id), dts);
      if (AfxMessageBox(s, MB_YESNO) == IDYES) {
          ret = undo_line(ado, co, fileID);
      }
  }
  else if (co.changeTypeID == CH_T_MOVE_NODE) {
      s.Format(_TR("Вернуть узел %s передвинутый %s на место?"), getMovedNodeName(ado, co.id), dts);
      if (AfxMessageBox(s, MB_YESNO) == IDYES) {
          ret = undo_move_node(ado, co, fileID);
      }
  }
  else if (co.changeTypeID == CH_T_MOVE_LINE) {
      s.Format(_TR("Вернуть линию %s передвинутую %s на место?"), getMovedNodeName(ado, co.id), dts);
      if (AfxMessageBox(s, MB_YESNO) == IDYES) {
          ret = undo_move_line(ado, co, fileID);
      }
  }
  else if (co.changeTypeID == CH_T_EDIT) {
      s.Format(_TR("Отменить редактирование %s атрибутов?"), dts);
      if (AfxMessageBox(s, MB_YESNO) == IDYES) {
          ret = undo_edit(ado, co, fileID);
      }
  }

  
  return ret;
}


void CGidrView::Undo(bool only_del)
{
    CAdoFile *ado = m_cxema->m_ado;
    if (!ado) return;

    CString q;

    map_lst.clear();
    lst.clear();

    CString qq = "";

    if (only_del) {
        qq.Format(" WHERE changeTypeID IN (1, 2) ");
    }


    q.Format("SELECT * FROM changed_object ORDER BY id DESC LIMIT 20");
    if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
        q.Format("SELECT TOP 200 * FROM changed_object %s ORDER BY id DESC", qq);
    }


    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            ChangedObject co;
            co.id = ado->read_long("id");
            co.changeTypeID = ado->read_long("changeTypeID");
            co.changedID = ado->read_long("changedID");
            
            co.old_id = ado->read_long("old_id");
            co.dt = ado->read_date("dateRemoved");
            co.text = ado->readStr("changedText");
            lst.push_back(co);

            ado->MoveNext();
        }
        ado->closeTable();
    }

    if (lst.size() <= 0) {
      AfxMessageBox(_TR("Нет допустимых отмен"), MB_OK|MB_ICONINFORMATION);
      return;
    }


    CMMenu2 menu(AfxGetMainWnd(), IDD_MENU_MULTI, _TR("Отмена изменений"));

    menu.AddColumn(_TR("Время"));
    menu.AddColumn(_TR("Объект"));
    menu.AddColumn(_TR("Тип"));

/*
    menu.setWidth(1, 150);
    menu.setWidth(2, 150);
    menu.setWidth(3, 200);
*/

double getScreenMas();

    menu.setWidth(1, 120*getScreenMas());
    menu.setWidth(2, 120*getScreenMas());
    menu.setWidth(3, 120*getScreenMas());

    for (list<ChangedObject>::const_iterator it = lst.begin(); it != lst.end(); it++) {
        ChangedObject co = *it;
        CString s;
        CString tn = "";

        if (it->changeTypeID == CH_T_DELETE_NODE) {
            tn = getRemovedNodeName(ado, it->id);
        }
        else if (it->changeTypeID == CH_T_DELETE_LINE) {
            tn = getRemovedLineName(ado, it->id);
        }
        else if (it->changeTypeID == CH_T_MOVE_NODE) {
            tn = getMovedNodeName(ado, it->id);
        }

        menu.Add(1, it->dt.Format("%Y-%m-%d %H:%M:%S"));

        if (tn == "") {
          menu.Add(2, getNameChT(it->changeTypeID));
          menu.Add(3, tn);
        }
        else {
          menu.Add(2, getNameChT(it->changeTypeID));
          menu.Add(3, tn);
        }
        menu.Add((void*)it->id);

        map_lst[co.id] = co;
    }


    if (menu.DoModal() == IDOK) {
      long id = (long) menu.getV();
      int fileID;
      if (undo_gid(ado, id, fileID)) {
          CString par;
          par.Format("%d", fileID);
          CWaitCursor wait;

          set<int>  v;
          v.insert(fileID);
          m_cxema->m_graph->clearFile(v);
          Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
          m_cxema->openGid(ado, par, pDoc->m_user_gid);
          Invalidate();
      }
    }

    InvalidateG();
}


void CGidrView::OnEditUndo()
{
    Undo(false);
}

void CGidrView::OnEditUndoDel()
{
    Undo(true);
}

CString getPar(set<int>& v);
void reset_max_changed_object(CAdoFile* ado);


bool read_undo_edit(CAdoFile *ado, const CString &text, std::map<CString, int> &map_table, std::map<CString, std::map<CString, CString> > &map_table2)
{
    CString q;
    CString qq = "";

    try {
        std::string utf8_text = ansi_to_utf8((const char*)text);
        nlohmann::json json_data = nlohmann::json::parse(utf8_text.c_str());

        for (const auto& item : json_data) {
            for (auto it = item.begin(); it != item.end(); ++it) {
                std::map<CString, CString> map_val;

                std::string topKey = it.key();  // Имя верхнеуровневого ключа
                CString tn = it.key().c_str();
                auto value = it.value();        // Значение, которое может быть объектом            

                if (value.is_object()) {
                    int id = value["id"];

                    for (auto inner_it = value.begin(); inner_it != value.end(); ++inner_it) {
                        std::string innerKey = inner_it.key();
                        auto innerValue = inner_it.value();

                        CString col = utf8_to_ansi(innerKey).c_str();

                        CString s = utf8_to_ansi(innerValue.dump()).c_str();

                        if (innerValue.is_string()) {
                            s = utf8_to_ansi(innerValue).c_str();
                            if (s.Left(2) == "N'") {
                            }
                            else {
                                s = "'" + s + "'";
                            }
                        }

                        if (col != "id") {
                            map_val[col] = s;
                        }
                    }
                }
                else {
                    // Если значение не объект, выводим его напрямую
    //                std::cout << "Значение: " << value << std::endl;
                }
            }
        }
    }
    catch (nlohmann::json::exception &ex) {
        log1((const char*)text);
        AfxMessageBox(ex.what());
    }
    
    return true;
}



void resetGid(CCxema * m_cxema, const CString & m_user_gid)
{
//    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    CGraph2* graph = m_cxema->m_graph;

    CString ss;
    int get_max_changed_object();

        int max_changed_object = get_max_changed_object();

//    ss.Format("%d", );
//    AfxMessageBox(ss);

    CString q;

//    max_changed_object = 169694;

    q.Format("select id, changeTypeID, changedID, changedText  from changed_object where id > %d order by id", max_changed_object);

    std::set<int> set_nodes;
    std::set<int> set_lines;
    std::map<CString, std::set<int> > set_geo;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int changedID = ado->read_long("changedID");
            int changeTypeID = ado->read_long("changeTypeID");
            CString changedText = ado->readStr("changedText");
            

            switch (changeTypeID) {
            case CH_T_DELETE_LINE: 
            case CH_T_MOVE_LINE:
            case CH_T_ADD_LINE: 
            case CH_T_RENAME_LINE: 
                set_lines.insert(changedID);
                break;
            case CH_T_DELETE_NODE: 
            case CH_T_MOVE_NODE: 
            case CH_T_ADD_NODE: 
            case CH_T_RENAME_NODE: 
                set_nodes.insert(changedID);
                break;
//            case CH_T_DELETE_CXEMA_BEGIN: return "";
//            case CH_T_DELETE_CXEMA_END: return _TR("Удалена схема");


            case CH_T_DELETE_GEO: 
            case CH_T_ADD_GEO: 
            case CH_T_MOVE_GEO: 
                {
                    CString tn = changedText;
                    int id = changedID;
                    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
                    if (kls) {
                        set_geo[tn].insert(id);
                    }
                }
                break;
            
            case CH_T_EDIT: 
                {
                    std::map<CString, int> map_table;
                    std::map<CString, std::map<CString, CString> > map_table2;
                    read_undo_edit(ado, changedText, map_table, map_table2);

                    for (auto & [tn, id] : map_table) {
                        if (tn == "linesobj") {
                            set_lines.insert(id);
                        }
                        else if (tn == "nodes") {
                            set_nodes.insert(id);
                        }
                        else {
                            Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
                            if (kls) {
                                set_geo[tn].insert(id);
                            }
                        }
                    }
                }
                break;

/**

            case CH_T_DELETE_GEO: return _TR("Удален объект геобазы");
            case CH_T_ADD_GEO: return _TR("Добавлен объект геобазы");
            case CH_T_EDIT_GEO: return _TR("Редактировался объект геобазы");
*/

            }

            ado->MoveNext();
        }
        ado->closeTable();
    }

//    CGraph2* graph = m_cxema->m_graph;


    for (const int& id : set_nodes) {
        CNode2 * n = graph->find(id);
        if (n) {
            for (CLINE2* l = n->lines; l; l = next(l)) {
                set_lines.insert(bline(l)->line.nomP);
                set_lines.insert(bline(l)->line.nomO);
            }
        }
    }


    graph->init_find_line_nom();

    for (const int& id : set_lines) {
        CLINE2 *l = graph->find_line_nom(id);
        if (l) {
            graph->remove_line(l);
        }
    }

    for (const int& id : set_nodes) {
        CNode2 *n = graph->find(id);
        if (n) {
            graph->remove_node(*n);
        }
    }

    for (auto & [tn, set_id] : set_geo) {
        Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
        if (kls) {
            for (auto& id : set_id) {
                int n = kls->GetNom2(id);
                if (n > 0) {
                    kls->geo4.erase(kls->geo4.begin() + (n - 1));
                }
            }
        }

/**

        CNode2* n = graph->find(id);
        if (n) {

            graph->remove_node(*n);
        }
*/
    }


    CString par2;

    if (set_nodes.size() > 0) {
        par2.Format(" (n.id in (%s)) ", getPar(set_nodes));
        m_cxema->openNodesNew(ado, m_cxema->m_par, par2, 0, m_user_gid);
    }

    if (set_lines.size() > 0) {
        par2.Format(" (l.id in (%s)) ", getPar(set_lines));
        m_cxema->openLinesNew(ado, m_cxema->m_par, par2, 0, m_user_gid);
    }

    reset_max_changed_object(ado);

void set_cxema_updated(bool yes);

    set_cxema_updated(false);
}

void CGidrView::OnResetGid()
{
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CString m_user_gid = pDoc->m_user_gid;
    resetGid(m_cxema, m_user_gid);

    InvalidateG();
}