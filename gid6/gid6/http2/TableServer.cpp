#include "stdafx.h"
#include "json.h"
#include "HTTPThrea.h"
#include "ado.h"

void startServer() {
    //CWinApp* pApp = AfxGetApp();
    //CString m_szTilesDir;
    //CString m_szWebDir;
    //CHttpThread* m_pThread;
    //m_szTilesDir = pApp->GetProfileString("WebServer2", "Tiles Dir", tiles);
    //m_szWebDir = pApp->GetProfileString("WebServer2", "Web Dir", "");
    //if (m_pThread) {
    //    AfxMessageBox("Сервер уже запущен");
    //    return;
    //}

    ////  closeWeb();

    //UpdateData();

    //CString s;

    //s.Format("%s\\config.json", m_szWebDir);

    //FILE* f = fopen(s, "w");
    //if (f) {
    //    JSON json;

    //    json.add("{");
    //    json.add("db", getGidAdoName());
    //    json.add("ps", getPsAdoName());

    //    CString s;
    //    s.Format("http://%s:%d", "localhost", 8080);

    //    json.add("ip", s);
    //    json.add("}");

    //    fprintf(f, "%s", json.str());

    //    fclose(f);
    //}




    ////  if (AfxMessageBox("Создать новую карту?", MB_YESNO) == IDYES) {
    ////    DeleteDirectory(filename);
    ////  }



    //UpdateData();
    ////  m_bRun = WebServer.Start(LPCTSTR(m_szHomeDir), LPCTSTR(m_szDefIndex), m_Port, m_PTO * 1000);

    //int port = 8080;

    //m_pThread = new CHttpThread(nullptr, 0, port, m_PTO, false);
    //m_pThread->CreateThread();
    //m_pThread->m_bAutoDelete = FALSE;

    //if (m_pThread) {
    //    nTimerID = SetTimer(1, 500, NULL);
    //}

    //UpdateData(FALSE);
}