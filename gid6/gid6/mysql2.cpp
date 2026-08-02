#include "stdafx.h"
#include "gid6.h"
#include "Cxema.h"

#include "gid6Doc.h"
#include "gidrView.h"

#include "ado.h"
#include "InputDia.h"
#include "Win.h"
#include "mmenu.h"

int regex_match(const char *p, const char *buf, vector<CString> & match);


// const CString standartPath = "C:\\Program Files\\Microsoft SQL Server\\MSSQL11.SQLEXPRESS\\MSSQL\\Backup\\";
// 2016 версия  "C:\\Program Files\\Microsoft SQL Server\\MSSQL13.SQLEXPRESS\\MSSQL\\Backup\\"
/**
bool createBackup(CAdoFile *ado, CString name)
{   
    CString save_dir = standartPath + name+".bak";
    CString q; 
    q.Format(
        "BACKUP DATABASE [%s] "
        "TO DISK = N'%s' WITH "
        "    NOFORMAT "
        ",     NOINIT "
        ",     NAME = N'%s_20141212' "
        ",     SKIP "
        ",     REWIND "
        ",     NOUNLOAD "
        ",     STATS = 10", name, save_dir, name);
    CInputDialog inD(AfxGetMainWnd( ), "", _TR("Запрос"), q);
    if (inD.DoModal() != IDOK) return false;
    return ado->Execute(q);
} 

bool createBackup(CAdoFile *ado, CString name, CString newName)
{
    CString bakPath = standartPath + name + ".bak";
    CString sqlCommand;
    sqlCommand.Format(
        "USE[master]"
        "RESTORE DATABASE[%s]"
        "FROM  DISK = N'%s' WITH FILE = 1"
        ", MOVE N'%s' TO N'C:\Program Files\Microsoft SQL Server\MSSQL11.SQLEXPRESS\MSSQL\DATA\%s.mdf'"
        ", MOVE N'%s_log' TO N'C:\Program Files\Microsoft SQL Server\MSSQL11.SQLEXPRESS\MSSQL\DATA\%s_log.ldf'"
        ", NOUNLOAD"
        ", STATS = 5", newName, bakPath, name, newName, name, newName);

    CInputDialog inD(AfxGetMainWnd(), "", _TR("Не удалось восстановить базу данны их файла. Запрос :"), sqlCommand);
    if (inD.DoModal() != IDOK) return false;
    return ado->Execute(sqlCommand);
}

*/


CString getFileName(CAdoFile *ado, CString name)
{
    CString q, file_name = "";
    q.Format("SELECT * FROM [master].[sys].[master_files] WHERE name='%s'", name);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            file_name = ado->readStr("physical_name");
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return file_name;
}

CString getBackupFileDir(CAdoFile *ado, CString name)
{
    CString s1, s2, q;
    CString old_name, save_dir;
    old_name = save_dir = getFileName(ado, name);

    int i = save_dir.Find("\\MSSQL\\DATA\\");

    if (i >= 0) {
        save_dir = save_dir.Left(i);
        save_dir += "\\MSSQL\\Backup\\";
        return save_dir;
    }

    return "";
}

bool createBackup(CAdoFile *ado, CString name, CString save_dir)
{

    if (save_dir != "") {
        CString q;
        q.Format(
            "BACKUP DATABASE [%s]\n"
            "TO DISK = N'%s' WITH\n"
            "    NOFORMAT\n"
            ",     NOINIT\n"
            ",     NAME = N'%s'\n"
            ",     SKIP\n"
            ",     REWIND\n"
            ",     NOUNLOAD\n"
            ",     STATS = 10", name, save_dir, name);

//        CInputDialog inD(AfxGetMainWnd(), "", _TR("Запрос"), q);
//        if (inD.DoModal() != IDOK) return false;

//        AfxMessageBox(q, MB_OK|MB_ICONINFORMATION);

        return ado->Execute(q);
    }

/*

    CString bakPath = standartPath + name + ".bak";
    CString sqlCommand;
    sqlCommand.Format(
        "USE[master]"
        "RESTORE DATABASE[%s]"
        "FROM  DISK = N'%s' WITH FILE = 1"
        ", MOVE N'%s' TO N'C:\Program Files\Microsoft SQL Server\MSSQL11.SQLEXPRESS\MSSQL\DATA\%s.mdf'"
        ", MOVE N'%s_log' TO N'C:\Program Files\Microsoft SQL Server\MSSQL11.SQLEXPRESS\MSSQL\DATA\%s_log.ldf'"
        ", NOUNLOAD"
        ", STATS = 5", newName, bakPath, name, newName, name, newName);

    CInputDialog inD(AfxGetMainWnd(), "", _TR("Не удалось восстановить базу данны их файла. Запрос :"), sqlCommand);
    if (inD.DoModal() != IDOK) return false;
    return ado->Execute(sqlCommand);
*/

    return false;
}

bool restoreBackup(CAdoFile *ado, CString name_old, CString name_new, CString save_dir)
{
    CString dir = "";
    vector<CString> match;

    int l = regex_match("(.+)\\\\MSSQL\\\\Backup\\\\.+", save_dir, match);

    if (l > 0) {
        dir =  match[1];
    }

    CString sqlCommand;
    sqlCommand.Format(
        "USE [master]\n"
        "RESTORE DATABASE [%s]\n"
        "FROM DISK = N'%s' WITH FILE = 1\n"
        ", MOVE N'%s' TO N'%s\\MSSQL\\DATA\\%s.mdf'\n"
        ", MOVE N'%s_log' TO N'%s\\MSSQL\\DATA\\%s_log.ldf'\n"
        ", NOUNLOAD\n"
        ", STATS = 5", name_new, 
        save_dir,
        name_old, dir, name_new,
        name_old, dir, name_new
        );

//    CInputDialog inD(AfxGetMainWnd(), "", _TR("Не удалось восстановить базу данны их файла. Запрос :"), sqlCommand);
//    if (inD.DoModal() != IDOK) return false;
    return ado->Execute(sqlCommand);

}

void CGidrView::OnSqlSave()
{
    if (!isAdmin()) {
        AfxMessageBox(_TR("Для выполнения этой операции у вас должны быть права администратора!"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CAdoFile *ado = m_cxema->m_ado;
    CString dir = getBackupFileDir(m_cxema->m_ado, getGidAdoName());
    CString name = getGidAdoName();

    CString s = "";
    CInputDialog inD(AfxGetMainWnd(), "", _TR("Введите название новой базы данных"), s);
    if (inD.DoModal() == IDOK) {
        CString new_name = inD.m_strInput;
        CString q;
        q.Format("SELECT name, database_id, create_date FROM sys.databases WHERE LOWER(name)=LOWER('%s')", new_name);

        int n = getCountTable2(ado, q);

        if (n > 0) {
            q.Format(_TR("База данных %s уже существует"), new_name);
            AfxMessageBox(q, MB_OK|MB_ICONINFORMATION);
        }
        else {
            q.Format(_TR("Копировать базу %s в %s?"), name, new_name);
            if (AfxMessageBox(q, MB_YESNO) == IDYES) {
                dir += name;

                COleDateTime v = COleDateTime::GetCurrentTime();
                CString dd = v.Format("_%Y%m%d%H%M%S.bak");

                dir += dd;

                bool error = true;

//                long tt = ado->GetCommandTimeout();
//                ado->PutCommandTimeout(2);
//                long tt2 = ado->GetCommandTimeout();

//                CString ss;
//                ss.Format("%d", tt2);
//                AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);

                ado->SetTimeout(1800);
                ado->SetTimeout(2);

                if (createBackup(ado, name, dir)) {
                    if (restoreBackup(ado, name, new_name, dir)) {
                        error = false;
                        AfxMessageBox(_TR("Выйдите из программы и зайдите опять."), MB_OK|MB_ICONINFORMATION);
                    }
                }

                ado->SetTimeout(30);

//                ado->PutCommandTimeout(tt);

                if (error) {
                    AfxMessageBox(m_cxema->m_ado->getError(), MB_OK|MB_ICONINFORMATION);
                }
            }
        }
    }
}

void CGidrView::OnSqlRestore()
{
}


CString get_gid_info(CAdoFile *ado, const char * fn);
bool set_gid_info(CAdoFile *ado, const char * fn, const char * v);



void CGidrView::OnSqlBackup()
{
    if (!isAdmin()) {
        AfxMessageBox(_TR("Для выполнения этой операции у вас должны быть права администратора!"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CAdoFile *ado = m_cxema->m_ado;

    CString dir = getBackupFileDir(m_cxema->m_ado, getGidAdoName());
    CString name = getGidAdoName();

    //    AfxMessageBox(dir+" "+name, MB_OK|MB_ICONINFORMATION);

    CString save_dir = "E:\\!!!!\\123";

    save_dir = get_gid_info(ado, "backup_path");

    if (save_dir == "") {
        save_dir = "E:\\!!!!\\123";
        set_gid_info(ado, "backup_path", save_dir);
    }

    COleDateTime v = COleDateTime::GetCurrentTime();
    CString sdt = v.Format("%Y%m%d%H%M%S");


    CString q;
    q.Format("BACKUP DATABASE [%s] TO  DISK = N'%s\\%s_%s.bak' WITH NOFORMAT, NOINIT, NAME = N'AlmatyGID-Full Database Backup', SKIP, NOREWIND, NOUNLOAD, STATS = 10",
        name, save_dir, name, sdt);

    if (!ado->Execute(q)) {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
    }
    else {
        AfxMessageBox("OK!", MB_OK|MB_ICONINFORMATION);
    }
}


void CGidrView::OnSqlBackupRestore()
{
    if (!isAdmin()) {
        AfxMessageBox(_TR("Для выполнения этой операции у вас должны быть права администратора!"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CAdoFile *ado = m_cxema->m_ado;
    CString s = "";
    CInputDialog inD(AfxGetMainWnd(), "", _TR("Введите название новой базы данных"), s);
    if (inD.DoModal() == IDOK) {
        CString new_name = inD.m_strInput;
        CInputDialog inD2(AfxGetMainWnd(), "", _TR("Введите название файла"), s);
        if (inD2.DoModal() == IDOK) {
            CString file_name = inD.m_strInput;
            CString dir_name = getFileName(ado, ado->m_schema);
            CString old_name = ado->m_schema;

            CString q;
            q.Format("USE[master] RESTORE DATABASE[%s] FROM  DISK = N'%s' WITH  FILE = 1, MOVE N'%s' TO N'%s\\%s.mdf', MOVE N'%s_log' TO N'%s\\%s_log.ldf', NOUNLOAD, STATS = 5",
                new_name, file_name,
                old_name, dir_name, new_name,
                old_name, file_name, new_name);
                //    CInputDialog inD(AfxGetMainWnd(), "", _TR("Не удалось восстановить базу данны их файла. Запрос :"), sqlCommand);
                //    if (inD.DoModal() != IDOK) return false;
//                ado->Execute(q);
        }
    }
}

CString getListOfGidTables();

void CGidrView::OnSqlCopyFragment()
{
  if (!isAdmin()) {
        AfxMessageBox(_TR("Для выполнения этой операции у вас должны быть права администратора!"), MB_OK|MB_ICONINFORMATION);
        return;
  }

  CMMenu menu(this, IDD_MENU1, _TR("Выберите Базу данных, источник копируемого фрагмента"));

  CAdoFile *ado = m_cxema->m_ado;

  CString q = getListOfGidTables();

  if (ado->openTable0(q)) {
    while (!ado->isEOF()) {
      CString s = ado->readStr(0);
      if (s.CompareNoCase(ado->m_schema)) {
        menu.Add(s, (void*)0);
      }
      ado->MoveNext();
    }
    ado->closeTable();
  }

  if (menu.DoModal() != IDOK) return;

  CString str;

  CAdoFile *ado2 = getAdo(menu.m_strText);

  if (ado2) {
    CMMenu menu2(this, IDD_MENU1, _TR("Список фрагментов"));

    if (ado2->openTable0("SELECT id, name  FROM fragments ORDER BY name")) {
      while (!ado2->isEOF()) {
        long id = ado2->read_long("id");
        CString s = ado2->readStr("name");
        menu2.Add(s, (void*)id);
        ado2->MoveNext();
      }
      ado2->closeTable();
    }
    if (menu2.DoModal() == IDOK) {
      str.Format(_TR("Скопировать фрагмент \"%s\" из базы %s в базу %s ?"), menu2.m_strText, ado2->m_schema, ado->m_schema);
      int ret = AfxMessageBox(str, MB_YESNO, MB_OK|MB_ICONINFORMATION);
      if (ret == IDYES) {
        CWaitCursor wait;
        
        CString q;
        int id = (int)menu2.getV();
        q.Format("exec dbo.copyDB [%s], [%s], %d", ado2->m_schema, ado->m_schema, id);
        if (ado->Execute(q)) {
            long fileID = get_last_id(ado, "fragments");
            openFragment(fileID, menu2.m_strText);
            AfxMessageBox(_TR("Выполнено копирование фрагмента.\nУстановите в объектах фрагмента Расчетная схема Источник тепла"), MB_OK|MB_ICONINFORMATION);
        }
        else {
            AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
        }
      }
    }
  }
}

bool delObject(CAdoFile *ado, const char *tn, int id);


void CGidrView::OnSqlDelFragment()
{
  if (!isAdmin()) {
      AfxMessageBox(_TR("Для выполнения этой операции у вас должны быть права администратора!"), MB_OK|MB_ICONINFORMATION);
      return;
  }

  CString str;
  CAdoFile *ado = m_cxema->m_ado;
  
  CMMenu menu2(this, IDD_MENU1, _TR("Список фрагментов"));

  if (ado->openTable0("SELECT id, name  FROM fragments ORDER BY name")) {
    while (!ado->isEOF()) {
      long id = ado->read_long("id");
      CString s = ado->readStr("name");

      map<long, CGidFile *>::iterator it = m_cxema->m_file_map.find(id);

      if (it != m_cxema->m_file_map.end()) {
          menu2.Add(s, (void*)id);
      }
      ado->MoveNext();
    }
    ado->closeTable();
  }
  if (menu2.DoModal() == IDOK) {
    str.Format(_TR("Удалить фрагмент \"%s\" из текущей базы %s?"), menu2.m_strText, ado->m_schema);
    int ret = AfxMessageBox(str, MB_YESNO, MB_OK|MB_ICONINFORMATION);
    if (ret == IDYES) {
        int fileID = (int)menu2.getV();
        if (delObject(ado, "fragments", fileID)) {
            closeFragment(fileID);
            m_cxema->openFragments(m_cxema->m_ado);

            AfxMessageBox("Выполнено удаление фрагмента", MB_OK|MB_ICONINFORMATION);
        }
        else {
            AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
        }
    }
  }
}



CString getPar(set<int> &v);

void CGidrView::openFragment(int fileID, const char * name)
{
    m_CurNode = NULL;
    m_CurLine = NULL;

    CAdoFile *ado = getAdo(getGidAdoName());

    set<int> v2;
    CString par;

    v2.insert(fileID);

    m_cxema->m_par = getPar(v2);
    CWaitCursor wait;

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    m_cxema->openGid(ado, m_cxema->m_par, pDoc->m_user_gid);


    CGidFile *gid_file = new CGidFile;
    gid_file->id = fileID;
    gid_file->name = name;
    gid_file->m_schema = ado->m_schema;
    m_cxema->m_file_map[fileID] = gid_file;
    
    m_cxema->initGidTree();

    CFRect rect = m_cxema->m_graph->getBorder(m_parent_id);
    SetBorder(rect);

    Invalidate();
}



void CGidrView::closeFragment(int fileID)
{
    m_CurNode = NULL;
    m_CurLine = NULL;

    CAdoFile *ado = getAdo(getGidAdoName());

    set<int> v2d;
    CString par;

    v2d.insert(fileID);

    if (v2d.size() > 0) {
        m_cxema->m_par = getPar(v2d);
        m_cxema->m_graph->clearFile(v2d);
    }

    m_cxema->m_file_map.erase(fileID);

    m_cxema->initGidTree();

    CFRect rect = m_cxema->m_graph->getBorder(m_parent_id);
    SetBorder(rect);

    Invalidate();
}


void chTableFileID(CAdoFile* ado, const char *file_name, int fileID1, int fileID2)
{
    CString sql;

    sql.Format("UPDATE %s SET fileID=%d WHERE fileID=%d", file_name, fileID2, fileID1);

    if (!ado->Execute(sql)) {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
    }
}


bool chFileID(CAdoFile* ado, int fileID1, int fileID2)
{
    
    if (!ado->Execute("ALTER TABLE nodes DISABLE TRIGGER all")) {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
        return false;
    }


    chTableFileID(ado, "nodes", fileID1, fileID2);

    chTableFileID(ado, "externalCodes", fileID1, fileID2);

    
    chTableFileID(ado, "texts", fileID1, fileID2);
    chTableFileID(ado, "gvsLoadGraphs", fileID1, fileID2);
    chTableFileID(ado, "setPressNodes", fileID1, fileID2);
    chTableFileID(ado, "directions", fileID1, fileID2);
    chTableFileID(ado, "calcTemperatures", fileID1, fileID2);
    chTableFileID(ado, "specExpends", fileID1, fileID2);
    chTableFileID(ado, "CALCULATION", fileID1, fileID2);
    chTableFileID(ado, "calculations", fileID1, fileID2);
    chTableFileID(ado, "varCoefficients", fileID1, fileID2);
    chTableFileID(ado, "sortLinesForUchastok", fileID1, fileID2);

    if (!ado->Execute("ALTER TABLE nodes ENABLE TRIGGER all")) {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    return true;


}


// Открытый фрагментов, если нескольео то -1


int nFragments(CAdoFile *ado, CCxema *cxema, CString &fragment)
{
  int n_open = 0;
  int fragmentID = -1;

  if (ado->openTable0("SELECT id, name  FROM fragments ORDER BY name")) {
    while (!ado->isEOF()) {
      long id = ado->read_long("id");
      CString s = ado->readStr("name");

      map<long, CGidFile *>::iterator it = cxema->m_file_map.find(id);

      if (it != cxema->m_file_map.end()) {
          n_open ++;
          fragmentID = id;
          fragment = s;
      }
      ado->MoveNext();
    }
    ado->closeTable();
  }

  if (n_open == 1)  return fragmentID;

  return -1;
}

#include "Filtr3.h"

void Run(CWnd* win, const char* cmd, const char* prtn, int id);



void CGidrView::OnEndPython()
{
/*
    if (!reopen_all()) return;

    m_cxema->m_bIsGidrInf = true;

    checkPjezo();
    InvalidateG();
    GetDocument()->SetModifiedFlag(true);
*/
    AfxMessageBox(_TR("Выполнено"), MB_OK | MB_ICONINFORMATION);
}


void CGidrView::OnAddFragment()
{
    if (!isAdmin()) {
        AfxMessageBox(_TR("Для выполнения этой операции у вас должны быть права администратора!"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CString sect;
    CAdoFile* ado = m_cxema->m_ado;

    sect.Format("DB\\%s", ado->m_schema);

    CFiltr3 dlg(this, "Выберите фрагменты для объединения");

    for (auto it : m_cxema->map_fragments) {
        bool on = AfxGetApp()->GetProfileInt(sect, it.second.m_name, 0);
        if (on) {
            dlg.Add(it.second.m_name, it.second.m_id, false);
        }
    }

    if (dlg.DoModal() != IDOK) return;

    CString ss = "";

    int n = 0;

    for (auto it : dlg.map_item) {
        if (it.second.on) {
            CString s;
            s.Format(" %d ", it.second.id);
            ss += s;
            n += 1;
        }
    }

    if (n < 2) {
        AfxMessageBox(_TR("Для выполнения этой операции вы должны выбрать два или более фрагментов"), MB_OK|MB_ICONINFORMATION);
        return;
    }


    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CString outName;
    outName.Format("%s\\out.txt", getenv("TMP"));

    CString cmd;

    cmd.Format("\"%s\" \"%s\\unite\\unite_tgid.py\" -rdbms MsSql -server %s -database %s -port %d -user %s -password %s -out_file \"%s\" -files %s", 
        python_exe(), argpath(), pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_port, pDoc->m_user, pDoc->m_password, outName, ss);

   log1(cmd);

//    AfxMessageBox(str);

    Run(this, cmd, argpath_2() + "protocol.txt", ID_END_UNITE);



#if 0

  CString str;
  CAdoFile *ado = m_cxema->m_ado;

  CString fragment;
  int fragmentID = nFragments(ado, m_cxema, fragment);
  
  if (fragmentID <= 0) {
    AfxMessageBox("Для выполнения операции должен быть открыт только один фрагмент", MB_OK|MB_ICONINFORMATION);
    return;
  }

  CMMenu menu2(this, IDD_MENU1, _TR("Список фрагментов"));

  if (ado->openTable0("SELECT id, name  FROM fragments ORDER BY name")) {
    while (!ado->isEOF()) {
      long id = ado->read_long("id");
      CString s = ado->readStr("name");

      map<long, CGidFile *>::iterator it = m_cxema->m_file_map.find(id);

      if (it == m_cxema->m_file_map.end()) {
          menu2.Add(s, (void*)id);
      }
      ado->MoveNext();
    }
    ado->closeTable();
  }
  if (menu2.DoModal() == IDOK) {
    str.Format(_TR("Добавить фрагмент \"%s\" из текущей базы %s к текущему фрагменту %s? Внимание после выполнения операции фрагмент \"%s\" будет уничтожен!!!"), menu2.m_strText, ado->m_schema, fragment, menu2.m_strText);
    int ret = AfxMessageBox(str, MB_YESNO);
    if (ret == IDYES) {
        int fileID1 = (int)menu2.getV();

        if (chFileID(ado, fileID1, fragmentID)) {
            AfxMessageBox("Операция выполнена. Выйдите из программы и зайдите опять.", MB_OK|MB_ICONINFORMATION);
        }
    }
  }
#endif
}
