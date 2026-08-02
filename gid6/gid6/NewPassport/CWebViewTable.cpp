#pragma once
#include "stdafx.h"
#include "CWebViewTable.h"
#include "resource.h"
#include "wm_user.h"
#include "gidrview.h"
#include "crack.h"
#include <regex>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "win.h"

#include <vector>
#include <algorithm>
#include <map>
#include <set>
using nlohmann::json;

CWebViewTable::CWebViewTable()
    : CView()
{
}

IMPLEMENT_DYNCREATE(CWebViewTable, CView)

BEGIN_MESSAGE_MAP(CWebViewTable, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
    ON_MESSAGE(WM_USER_ADO_UPDATED, OnAdoUpdated)


END_MESSAGE_MAP()


CString findTableRusName(CString d, CString n);
CString findTableEngName(CString d, CString n);
int IsFile(const TCHAR* fN);

CString getFileDialog(CWnd* wnd, CString defext, CString path, CString filter, CString profile, CString pattern);
int updateTeplopotery(int id, BSTR jsonString, CAdoFile* mAdo);
int insertTeplopotery(int id, BSTR jsonString, CAdoFile* mAdo);
int insertTeplopoteryDefault(CAdoFile* mAdo);
int updateHeaderTeplopotery(int id, BSTR jsonString, CAdoFile* mAdo);
pair<int, CString> deleteTeplopotery(int id, int idColumn, CAdoFile* mAdo);
CString getHeaderTeplopotery(int id, CAdoFile* mAdo, BOOL init);
CString getSpecialHeader(CString tblName, BOOL init);
CString getExcelHeaderTeplopotery(int id, CAdoFile* mAdo);
int executeScriptTeplopotery(CAdoFile* mAdo, CString fileName);
FILE *popen(const TCHAR * cmd, const TCHAR * mode);
int pclose(FILE * f);
string UTF8ToANSI(string s);

int system_no_output( CString command )
{
//    command.insert( 0, "/C " );
    command = "/C " + command;

    SHELLEXECUTEINFOA ShExecInfo = {0};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = "cmd.exe";        
//    ShExecInfo.lpParameters = command.c_str();   
    ShExecInfo.lpParameters = command;
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;

    if( ShellExecuteExA( &ShExecInfo ) == FALSE )
        return -1;

    WaitForSingleObject( ShExecInfo.hProcess, INFINITE );

    DWORD rv;
    GetExitCodeProcess( ShExecInfo.hProcess, &rv );
    CloseHandle( ShExecInfo.hProcess );

    return rv;
}


void CWebViewTable::OnInitialUpdate()
{
    CView::OnInitialUpdate();
    mAdo = getAdo(getGidAdoName());
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    /*CString mq = pDoc->m_view_sql;
    int type_of_net = pDoc->m_type_of_net;

    if (type_of_net == TYPE_OF_NET_MYSQL) {
        mq.Replace("[", "`");
        mq.Replace("]", "`");
        mq.Replace(".dbo.", ".");
    }

    if (type_of_net == TYPE_OF_NET_MSSQL) {
        mq.Replace(" IF(", " IIF(");
        mq.Replace(",IF(", ",IIF(");
    }
    bool ret = mAdo->openTable0(mq);
    if (!ret)
    {
        AfxMessageBox("Ошибка в запросе при формировании таблицы");
        return;
    }*/

    impDataSource = new WVAdoImpDataSource;
    impDataSource->m_q = pDoc->m_view_sql;
    impDataSource->addText = pDoc->m_view_add_text;

    impDataSource->showAllInit(pDoc->m_view_all);
    impDataSource->setAllows(pDoc->m_view_typ);




    CString getTableNameFromSQL(CString q);
    //проверка существования пользовательских данных в таблице нормы тепловых потерь
    if (getTableNameFromSQL(pDoc->m_view_sql) == "39_normy_teplovyh_poter")
    {
        CAdoFile* adoSprav = getAdo("sprav");
        insertTeplopoteryDefault(adoSprav);
    }
    if (impDataSource->OpenSQL(pDoc->m_view_sql, pDoc->m_view_tn, pDoc->m_view_connect, pDoc->m_type_of_net) == UG_ERROR) {
        return;
    }
//    if (readTable(pDoc->m_view_db, pDoc->m_view_tn, m_head)) {
//    }
    if (readTable(pDoc->m_view_db, pDoc->m_view_head, m_head)) {
    }

    countAllRows = impDataSource->GetNumRowsComplete();

    tableName = pDoc->m_view_tn;

    titleName = tableName;
    if (pDoc->m_view_caption != pDoc->m_view_tn) {
        titleName = pDoc->m_view_caption;
    }


    mSql = pDoc->m_view_sql;

    impDataSource->SetTableName(pDoc->m_view_db, pDoc->m_view_tn);

    int nf = impDataSource->countFields();
    countFields = nf;

    CString rus_name = findTableRusName(pDoc->m_view_db, pDoc->m_view_tn);

    if (pDoc->m_view_caption != pDoc->m_view_tn) {
        rus_name = pDoc->m_view_caption;
    }

    GetParentFrame()->SetWindowText(rus_name);

    //SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
    m_pWebBrowser = std::make_unique<CWebViewBrowser>();

    if (m_pWebBrowser != nullptr)
    {
        CRect rectClient;
        GetClientRect(rectClient);

        m_pWebBrowser->CreateAsync(
            WS_VISIBLE | WS_CHILD,
            rectClient,
            this,
            1,
            [this]() {
                m_pWebBrowser->SetParent(this);
                m_pWebBrowser->DefaultSettings();
                m_pWebBrowser->setHostObject(
                    // callback для метода добавления
                    [this](BSTR jsonString, std::function<void(int, int, BSTR)> callbackInsert)
                    {

                        int id = -1;
                        if (specialForTeplopotery)
                        {
                            CAdoFile* adoSprav = getAdo("sprav");
                            id = insertTeplopotery(teplopoteryTableIndex, jsonString, adoSprav);
                        }
                        else
                        {
                            if (customEditMode)
                                id = addNewOld(jsonString);
                            else
                                id = addNew(jsonString);

                        }
                        if (id > -1)
                        {
                            impDataSource->positionById.insert(std::make_pair(id, currentCount));
                            //если все данные вытянуты то плюсуем текущее количество иначе добавленные данные сами вытянутся при необходимости
                            if (currentCount >= countAllRows - 1)
                            {
                                currentCount++;
                                //добавляем данные сразу в dataStorage так как все данные вытянуты и не смогут туда добавиться

                                dataStorageId.push_back(id);
                                dataStorage[id] = jsonString;
                                currentDataStorageIndex++;

                            }
                            callbackInsert(id, 200, L"Запись добавлена");
                        }
                        else
                            callbackInsert(id, 404, L"Ошибка при добавлении");
                    },
                    //callback для метода обновления
                        [this](INT id, BSTR jsonString, std::function<void(int, BSTR)> callbackChange)
                    {
                        int code = 404;
                        if (specialForTeplopotery)
                        {
                            CAdoFile* adoSprav = getAdo("sprav");
                            if (id == -1) {

                                code = updateHeaderTeplopotery(teplopoteryTableIndex, jsonString, adoSprav);
                            }
                            else

                                code = updateTeplopotery(teplopoteryTableIndex, jsonString, adoSprav);
                            if (code == 200) {
                                CString query = getHeaderTeplopotery(teplopoteryTableIndex, adoSprav, false);

                                if (!query.IsEmpty()) {
                                    query += "";
                                    BSTR myQuery = query.AllocSysString();
                                    m_pWebBrowser->ExecuteScript(
                                        myQuery);
                                }
                            }
                        }
                        else
                            code = updateData(id, jsonString);

                        if (code == 200) {
                            dataStorage[id] = jsonString;
                        }
                        callbackChange(code, L"Успешно изменено");

                    },
                        //callback для метода удаления
                        [this](vector<int> ids, std::function<void(int, BSTR)> callbackChange)
                    {
                        int code;
                        CString message;
                        pair<int, CString> resultData;
                        if (ids.size() == 1)
                        {
                            if (specialForTeplopotery)
                            {
                                CAdoFile* adoSprav = getAdo("sprav");
                                resultData = deleteTeplopotery(teplopoteryTableIndex, ids[0], adoSprav);
                            }
                            else
                                resultData = impDataSource->deleteData(ids[0], impDataSource->positionById[ids[0]]);
                            code = resultData.first;
                            message = resultData.second;
                        }
                        else
                        {
                            resultData = impDataSource->deleteData(ids);
                            code = resultData.first;
                            message = resultData.second;
                        }

                        if (code == 200) {
                            for (int i = 0; i < ids.size(); i++) {
                                impDataSource->positionById.erase(ids[i]);
                                currentCount--;
                                dataStorage.erase(ids[i]);
                                for (auto it = dataStorageId.begin(); it != dataStorageId.end(); ++it) {
                                    if (*it == ids[i]) {
                                        dataStorageId.erase(it);
                                        currentDataStorageIndex--;
                                        break;
                                    }
                                }
                            }
                        }

                        callbackChange(code, message.AllocSysString());
                    },

                        //callback для перемещения к объекту в расчетной схеме
                        [this](int id, BSTR type, BSTR value, std::function<void(int, BSTR)> callbackChange)
                    {

                        if (wcscmp(type, L"file") == 0) {
                            int myColId = 5;
                            AdoFile* file = m_head.getFile(myColId);
                            if (file) {
                                CString text(value);
                                CString path = AfxGetApp()->GetProfileString(szSection, file->s3, "");
                                CString p2 = path + "\\" + text;

                                if (path == "") {
                                    if (!IsFile(text)) {
                                        CString s;
                                        s.Format("Задайте каталог хранения документов");
                                        AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
                                        return;
                                    }
                                    p2 = text;
                                }

                                if (IsFile(p2)) {
                                    HINSTANCE hi = ShellExecute(this->m_hWnd, "open", p2, NULL, NULL, SW_SHOWNORMAL);
                                }
                                else {
                                    CString s;
                                    s.Format("Не найден файл %s", p2);
                                    AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
                                }
                            }
                        }
                        else

                            OnMoveTo(id);
                        callbackChange(200, L"Успешно");

                    },
                        //callback для Excel
                        [this](BSTR jsonString, std::function<void(int, BSTR)> callbackChange)
                    {
                        std::wstring ws(jsonString, SysStringLen(jsonString));

                        auto myJson = json::parse(ws);
                        json jsonData = myJson["data"];


                        CString excelData;
                        if (jsonData.is_null()) {
                            impDataSource->portionCount = 100;
                            bool hasNextData = true;

                            //догружаем данные с бд и сохраняем их в переменную dataStorage (если это необходимо)
                            while (hasNextData) {

                                int resultFetching = impDataSource->OnHitBottom(currentCount);

                                hasNextData = resultFetching == UG_SUCCESS;
                                if (hasNextData)
                                    ObtainDataToStorage();

                            }

                            CString headers;
                            headers = CString(jsonString);
                            headers = headers.Mid(1);
                            headers = headers.Left(headers.GetLength() - 1);
                            CString values;
                            values = GetAllValues();
                            int length = values.GetLength();
                            values = values.Left(length - 1);
                            //values.Replace('`', '\\\"');
                            excelData.Format("{%s, \"data\": [%s]}", headers, values);
                            /*
                             json jsonHeaders = myJson["headers"];

                            CString valuesTmp;
                            valuesTmp.Format("[%s]", GetValuesByRange(0, 150));
                            std::string s((LPCTSTR)valuesTmp);
                            json jsonValues = json::parse(valuesTmp.AllocSysString());
                            CString headers = to_string(jsonHeaders).c_str();
                            CString values = to_string(jsonValues).c_str();
                            */


                        }
                        else
                            excelData = jsonString;

                        if (specialForTeplopotery) {
                            CAdoFile* adoSprav = getAdo("sprav");
                            CString filePath, tmpName;
                            filePath = getExcelHeaderTeplopotery(teplopoteryTableIndex, adoSprav);
                            tmpName.Format("%s\\table_teplopotery_%i.html", getenv("TMP"), teplopoteryTableIndex);
                            impDataSource->OnExcelPro(excelData.AllocSysString(), filePath, tmpName, GetSafeHwnd());
                        }
                        else if (specialHeader) {
                            CString filePath, tmpName;
                            filePath.Format("custom_headers\\excel\\%s.html", impDataSource->m_sTableName_main);
                            tmpName.Format("%s\\%s.html", getenv("TMP"), impDataSource->m_sTableName);
                            impDataSource->OnExcelPro(excelData.AllocSysString(), filePath, tmpName, GetSafeHwnd());
                        }
                        else
                            impDataSource->OnExcel(excelData.AllocSysString());

                        callbackChange(200, L"OK");

                    },

                        //callback для метода pickupfile
                        [this](BSTR field, std::function<void(BSTR, BSTR, int, BSTR)> callbackPickupFile) {
                        int myColId = 5;
                        AdoFile* file = m_head.getFile(myColId);
                        if (file) {
                            CString path = AfxGetApp()->GetProfileString(szSection, file->s3, "");
                            CString p = getFileDialog(this, file->s1, path, file->s2, "AdoCUG", file->pattern);

                            if (p != "") {
                                if (p.Left(0) == "." && path != "") {
                                    CString s;
                                    s.Format("Файл должен находиться в каталоге %s", path);
                                    AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
                                    callbackPickupFile(L"", field, 404, L"BAD");
                                }
                                else {

                                    callbackPickupFile(p.AllocSysString(), field, 200, L"OK");
                                }
                            }
                        }
                    },


                        //callback для метода утверждения
                        [this](INT id, BSTR jsonString, std::function<void(int, BSTR, BSTR)> callbackChange)
                    {
                        //                      int code = updateData(id, jsonString);
                        //                      callbackChange(code, L"OK");
                    },
                    //callback для метода печати
                        [this](BSTR jsonData, int count) {
                        //считать данные с файла шаблона
                        //заменить данные в шаблоне
                        CString q, fStr, q2;
                        q2 = CString(jsonData);
                        std::stringstream stream;

                        fStr.Format("%spdfGenerate\\pdf_template.html", argpath());
                        ifstream f(fStr);
                        if (f.good())
                        {
                            q = readFile(fStr);
                            q.Replace("$table$", q2);
                            //q.Replace("$size$", "size: 891mm 630mm;");

                            
                            float scale = ((count * 200)+1+100+count) / 1123.0;
                            float w = 297 * scale;
                            float h = 210 * scale;
                            CString size;
                            size.Format("size: %fmm %fmm;",w,h);
                            q.Replace("$size$", size);
                            fStr.Format("%spdfGenerate\\test.html", argpath());

                            std::ofstream file(fStr, std::ios::out | std::ios::trunc);

                            if (file.is_open()) {
                                file << q;
                                file.close();
                                
                                fStr.Format("%spdfGenerate\\weasyprint.exe --hinting -e windows-1251 %s\\test.html %s\\output.pdf ", argpath(), getenv("TEMP"), getenv("TEMP"));
                                system_no_output(fStr);

//                                FILE *outf = popen( fStr, "r");
//                                fclose(outf);

                                CString sourcePath, destPath;
                                sourcePath.Format("%s\\output.pdf", getenv("TEMP"));
                                destPath.Format("%stableServer\\docs\\out.pdf", argpath());
                                std::ifstream sourceFile(sourcePath, std::ios::binary);
                                if (!sourceFile.is_open()) {
                                    std::cerr << "Ошибка открытия исходного файла." << std::endl;
                                    return false;
                                }

                                std::ofstream destFile(destPath, std::ios::binary | std::ios::trunc);
                                if (!destFile.is_open()) {
                                    std::cerr << "Ошибка открытия файла назначения." << std::endl;
                                    return false;
                                }

                                destFile << sourceFile.rdbuf();
                                BSTR myQuery = L"window.tableComponent.printPDF(\"out\")";
                                m_pWebBrowser->ExecuteScript(
                                    myQuery);
                            }


                        }
                    },
                        [this](BSTR jsonString, std::function<void(int, BSTR)> callbackChange)
                    {
                        _bstr_t bstr(jsonString);
                        std::wstring jsonStr(jsonString, SysStringLen(jsonString));

                        // Разбираем JSON-строку
                        json j = json::parse(jsonStr);

                        // Извлекаем строку поиска и список полей
                        CString searchText = UTF8ToANSI(j["search"].get<string>().c_str()).c_str();
                        vector<string> colNames = j["cols"].get<vector<string>>();

                        // Преобразуем имена полей в индексы полей
                        vector<int> searchFields;
                        for (const string& colName : colNames) {
                            int index = SearchUtility::ExtractFieldIndex(CString(colName.c_str()));
                            if (index != -1) {
                                searchFields.push_back(index);
                            }
                        }
                        


                        //включаем прелоудер
                        m_pWebBrowser->ExecuteScript(
                            L" window.tableComponent.overlay = true;");


                        CString query, qq = "";


                        impDataSource->portionCount = 100;
                        bool hasNextData = true;

                        //догружаем данные с бд и сохраняем их в переменную dataStorage (если это необходимо)
                        while (hasNextData) {

                            int resultFetching = impDataSource->OnHitBottom(currentCount);

                            hasNextData = resultFetching == UG_SUCCESS;
                            if (hasNextData)
                                ObtainDataToStorage();

                        }

                        //ищем данные в dataStorage и записывем в qq
                        if (searchFields.size() == 0)
                            qq = SearchUtility::GetValuesByText(searchText, dataStorage, dataStorageId);
                        else
                            qq = SearchUtility::GetValuesByText(searchText, searchFields, countFields, dataStorage, dataStorageId);

                        if (!qq.IsEmpty()) {
                            query = "window.tableComponent.$store.commit(\"setDataList\", [";
                            query += qq;
                            query += "]); window.tableComponent.overlay = false;";

                            m_pWebBrowser->ExecuteScript(
                                query.AllocSysString());
                        }
                        else {
                            query += "window.tableComponent.$store.commit(\"setDataList\", []); window.tableComponent.overlay = false;";
                            m_pWebBrowser->ExecuteScript(
                                query.AllocSysString());
                        }
                    }


                    );
                m_pWebBrowser->RegisterCallback(CWebViewBrowser::CallbackType::TitleChanged, [this]() {


                    CRect rectClient;
                    GetClientRect(&rectClient);
                    m_pWebBrowser->Resize(rectClient.Width(), rectClient.Height());

                    }

                );

                //add callback post message
                m_pWebBrowser->RegisterPostMessageCallback([this](PWSTR message) {
                    if (wcscmp(message, L"OnPageFinished") == 0 || (wcscmp(message, L"OnPageBottom") == 0)) {
                        /*if (wcscmp(message, L"OnPageFinished") == 0)
                        AfxMessageBox("This message came from Javascript : OnPageFinished");
                        else
                            AfxMessageBox("This message came from Javascript : OnPageBottom");*/



                        impDataSource->portionCount = 100;
                        
                        CString query = GetNextValues();

                        if (!query.IsEmpty()) {
                            BSTR myQuery = query.AllocSysString();
                            m_pWebBrowser->ExecuteScript(
                                myQuery);
                        }

                    }

                    else    if (wcsstr(message, L"viewAll")) {  // Показать все

                        viewAll = true;
                        initDataSource = false;

                        m_pWebBrowser->Reload();



                    }



                    else    if (wcsstr(message, L"OnPrint")) {


                        impDataSource->portionCount = 100;
                        bool hasNextData = true;

                        //догружаем данные с бд и сохраняем их в переменную dataStorage (если это необходимо)
                        while (hasNextData) {

                            int resultFetching = impDataSource->OnHitBottom(currentCount);

                            hasNextData = resultFetching == UG_SUCCESS;
                            if (hasNextData)
                                ObtainDataToStorage();

                        }

                        CString sumQuery;
                        sumQuery = GetValuesByRange(0, dataStorageId.size() - 1);
                        sumQuery = "window.tableComponent.printAll([" + sumQuery + "]);";
                        if (!sumQuery.IsEmpty()) {
                            //sumQuery = "window.tableComponent.printPDF(\"out\")";
                            //sumQuery += "window.tableComponent.disablePagination = true; window.tableComponent.onResize();";
                            BSTR myQuery = sumQuery.AllocSysString();
                            
                            m_pWebBrowser->ExecuteScript(
                                myQuery);
                        }


                    }

                    else    if (wcsstr(message, L"OnSearch")) {  // Поиск старый, уже используем метод в host object

                        std::wstring ws = message;
                        std::wstring sub = ws.substr(9);
                        CString pat(sub.c_str());


                        //включаем прелоудер
                        m_pWebBrowser->ExecuteScript(
                            L" window.tableComponent.overlay = true;");


                        CString query, qq = "";


                        impDataSource->portionCount = 100;
                        bool hasNextData = true;

                        //догружаем данные с бд и сохраняем их в переменную dataStorage (если это необходимо)
                        while (hasNextData) {

                            int resultFetching = impDataSource->OnHitBottom(currentCount);

                            hasNextData = resultFetching == UG_SUCCESS;
                            if (hasNextData)
                                ObtainDataToStorage();

                        }

                        //ищем данные в dataStorage и записывем в qq
                     
                        qq = SearchUtility::GetValuesByText(pat, dataStorage, dataStorageId);

                        if (!qq.IsEmpty()) {
                            query = "window.tableComponent.$store.commit(\"setDataList\", [";
                            query += qq;
                            query += "]); window.tableComponent.overlay = false;";

                            m_pWebBrowser->ExecuteScript(
                                query.AllocSysString());
                        }
                        else {
                            query += "window.tableComponent.$store.commit(\"setDataList\", []); window.tableComponent.overlay = false;";
                            m_pWebBrowser->ExecuteScript(
                                query.AllocSysString());
                        }
                    }

                    else    if (wcsstr(message, L"OnRange")) {  //Порционная загрузка
                        std::wstring ws = message;
                        std::wstring sub = ws.substr(8);
                        int index1 = sub.find(L"-");
                        int index2 = sub.find(L",");
                        int beginId = std::stoi(sub.substr(0, index1));
                        int endId = std::stoi(sub.substr(index1 + 1, index2));

                        CString page(sub.substr(index2 + 1).c_str());


                        impDataSource->portionCount = 100;
                        bool hasNextData = dataStorageId.size() < endId;
                        while (hasNextData) {

                            int resultFetching = impDataSource->OnHitBottom(currentCount);

                            hasNextData = resultFetching == UG_SUCCESS;
                            if (hasNextData)
                                ObtainDataToStorage();
                            hasNextData = hasNextData && dataStorageId.size() < endId;

                        }
                        CString qq, query;
                        qq = GetValuesByRange(beginId - 1, endId - 1);

                        if (!qq.IsEmpty()) {
                            query = "window.tableComponent.addData(";
                            query += page;
                            query += ", [";
                            query += qq;
                            query += "]);";

                            m_pWebBrowser->ExecuteScript(
                                query.AllocSysString());
                        }
                        else {

                        }
                    }


                    else    if (wcsstr(message, L"OnPageNext")) {

                        std::wstring ws = message;
                        std::wstring sub = ws.substr(11);
                        impDataSource->portionCount = std::stoi(sub);

                        m_pWebBrowser->ExecuteScript(
                            L" window.tableComponent.overlay = true;");
                        CString query;

                        int myCount = 0;
                        while (impDataSource->portionCount > myCount) {

                            CString newQuery = GetNextValues();
                            if (!newQuery.IsEmpty() && myCount == 0) {
                                query += "window.tableComponent.addData([";
                            }
                            query += newQuery;
                            myCount += 100;
                        }


                        if (!query.IsEmpty()) {
                            query += "]); /*window.tableComponent.overlay = false;*/";
                            m_pWebBrowser->ExecuteScript(
                                query.AllocSysString());
                        }
                        else {
                            query += "window.tableComponent.addData([]);";
                            m_pWebBrowser->ExecuteScript(
                                query.AllocSysString());
                        }

                    }
                    else if (wcsstr(message, L"OnDefault")) {
                        CAdoFile* adoSprav = getAdo("sprav");
                        int code = executeScriptTeplopotery(adoSprav, "reset_default_delete");
                        code = executeScriptTeplopotery(adoSprav, "reset_default_insert");
                        if (code == 0) {

                            impDataSource->ClearCurrentConnection();

                            initDataSource = false;
                            countRows = 0;
                            currentCount = 0;
                            dataStorageId.clear();
                            dataStorage.clear();
                            sortDataStorage.clear();
                            currentDataStorageIndex = 0;
                            Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
                            if (impDataSource->OpenSQL(pDoc->m_view_sql, pDoc->m_view_tn, pDoc->m_view_connect, pDoc->m_type_of_net) == UG_ERROR) {
                                return false;
                            }

                            countAllRows = impDataSource->GetNumRowsComplete();
                            int nf = impDataSource->countFields();
                            countFields = nf;

                            m_pWebBrowser->Reload();
                        }

                    }
                    else if (wcsstr(message, L"OnEdit")) {

                        int ado_view(CWnd * wnd, CAdoFile * ado, CString table_name, int id, CString title, bool readonly);


                        CAdoFile* ado = NULL;

                        std::wstring ws = message;
                        std::wstring sub = ws.substr(7);
                        int id = std::stoi(sub);

                        ado = getAdo(impDataSource->m_sDbName);

                        ado_view(this, ado, impDataSource->m_sTableName_main, id, titleName, !impDataSource->allowEdit);
                    }

                    else if (wcsstr(message, L"OnSort")) {

                    
                        std::wstring ws = message;
                        std::wstring sub = ws.substr(7);
                        auto myJson = json::parse(sub);
                        for (const auto& elem : myJson["sort"]) {

                            bool first = false;
                            for (long i = 0; i < countFields; i++) {
                                std::cout << elem << std::endl;
                                string title = "col_" + std::to_string(i);
                                CString title1;
                                title1.Format("col_%d", i);
                                if (elem.contains(title) > 0) {
                                    first = true;
                                    auto j1 = elem[title];
                                    if (j1.get<int>() == 0) {
                                        //asc
                                        SortData(title1, false);
                                    }
                                    else {
                                        //desc
                                        SortData(title1, true);
                                    }
                                    break;
                                    
                                }
                            }
                            if (first)
                                break;
                            
                        }
                    }
                    if (wcsstr(message, L"OnMap")) {
                        OnMarkAccessibility();
                    }
                    if (wcsstr(message, L"OnWord")) {
                        // Example usage:
                        CString wordFilePath;
                        wordFilePath.Format("%shelp\\Помощь_Анализ_допустимости.docx", argpath());

                        BSTR fileName = wordFilePath.AllocSysString();
                        impDataSource->OnWord(fileName);
                        SysFreeString(fileName);
                    }
                    if (wcsstr(message, L"OnRemoveEmpty")) {

                        //удаление полей с пустыми данными
                        

                        CString sumQuery;
                        sumQuery = GetEmptyColumns();
                        
                        sumQuery = "window.tableComponent.changeTableHeaderByCols([" + sumQuery + "]);";
                        if (!sumQuery.IsEmpty()) {
                            BSTR myQuery = sumQuery.AllocSysString();
                            m_pWebBrowser->ExecuteScript(
                                myQuery);
                        }


                    }

                    });
                m_pWebBrowser->Navigate("https://appassets.example/index.html", nullptr);

            });
    }

}

// Функция для проверки, содержит ли все элементы вектора пустые строки
bool allEmptyStrings(const std::vector<std::pair<int, std::any>>& vec) {
    for (const auto& pair : vec) {
        try {
            if (pair.second.type() == typeid(const char*)) {
                const char* str = std::any_cast<const char*>(pair.second);
                if (str != nullptr && std::strcmp(str, "") != 0) {
                    return false; // Не пустая строка
                }
            }
            else if (pair.second.type() == typeid(CString)) {
                CString str = std::any_cast<CString>(pair.second);
                if (!str.IsEmpty()) {
                    return false; // Не пустая строка
                }
            }
            else if (pair.second.type() == typeid(int) || pair.second.type() == typeid(long) || pair.second.type() == typeid(float) || pair.second.type() == typeid(double)) {
                // Если данные числовые, считаем, что строка не пустая
                return false;
            }
        }
        catch (const std::bad_any_cast&) {
            // Обработка ошибки приведения типов, если необходимо
        }
    }
    return true; // Все строки пустые или числовые
}


CString CWebViewTable::GetEmptyColumns() {
    // Поиск векторов, содержащих только пустые строки или числовые значения
    bool foundVectorsWithOnlyEmptyStrings = false;
    CString q = "";
    for (const auto& entry : sortDataStorage) {
        if (allEmptyStrings(entry.second)) {
            foundVectorsWithOnlyEmptyStrings = true;
            q += ("\""+entry.first + "\"\, ");
        }
    }
    return q;
}



int CWebViewTable::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;


    return 0;
}

void CWebViewTable::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    CRect rectClient;

    if (m_pWebBrowser != nullptr)
    {
        GetClientRect(&rectClient);
        m_pWebBrowser->Resize(rectClient.Width(), rectClient.Height());
    }
}

BOOL CWebViewTable::OnPreparePrinting(CPrintInfo* pInfo)
{

    return DoPreparePrinting(pInfo);
}

void CWebViewTable::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
}

void CWebViewTable::OnEndPrinting(CDC* /*pDC*/, CPrintInfo*)
{
}

void CWebViewTable::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{

    CView::OnPrint(pDC, pInfo);
}


double to_double(COleVariant& var);

//Заполнение заголовков таблицы
CString CWebViewTable::GetHeaderTable() {
    BOOL readOnly = true;
    BOOL editMode = true;
    BOOL wordMode = false;
    BOOL markMode = false;
    BOOL addMode = false;
    BOOL deleteMode = true;
    if (impDataSource->allowAdd && impDataSource->allowEdit && impDataSource->allowDelete)
        readOnly = false;

    deleteMode = impDataSource->allowDelete;
    editMode = impDataSource->allowEdit;
    addMode = impDataSource->allowAdd;

    switch (impDataSource->myType) {
    case virt_data_node_noedit:
    case virt_data_line_noedit:
        wordMode = true;
        markMode = true;
        break;
    }

    BOOL isTechnicalCondition = strcmp(tableName, "Технические условия") == 0;


    BOOL isMoveTo = false;
    switch (impDataSource->myType) {
    case virt_data_node_noedit:
    case virt_data_line_noedit:
    case virt_data_node:
    case virt_data_node_out:
    case virt_data_line:
    case virt_data_line_out:
    case virt_data_line_big:
    case virt_data_line_big_osmotr:
    case virt_data_geo:
    case virt_data_geo_del:
    case virt_data_geo_edit:
    case virt_data_tu:
        isMoveTo = true;
        break;
    }

    CString totalPage;
    CString query, rusTableName;
    rusTableName = findTableRusName(impDataSource->m_sDbName, impDataSource->m_sTableName);

    if (titleName != impDataSource->m_sTableName) {
        rusTableName = titleName;
    }
    if (impDataSource->m_sTableName_main == "39_normy_teplovyh_poter") {

        specialForTeplopotery = true;
        customEditMode = true;
        teplopoteryTableIndex = atoi(impDataSource->addText);
        if (teplopoteryTableIndex < 12)
        {
            editMode = false;
            deleteMode = false;
        }
    }

    if ((regex_match(titleName.GetString(), regex("Тип (арматуры|назначения|конструкции|конструкции камеры|исполнения|месторасположения|материала|характеристики)")))) {
        customEditMode = true;
    }


    if (impDataSource->m_sTableName_main == "DR_OUT") {
        specialHeader = true;
        editMode = false;
        deleteMode = false;
    }

    if (specialForTeplopotery || specialHeader) {
        readOnly = true;
        liteMode = true;
    }
    if (viewAll) {
        liteMode = true;
    }
    totalPage.Format(

        "window.MainComponent.$store.commit(\"setTableHeader\",\"%s\");"
        "window.MainComponent.$store.commit(\"setTotalPage\",%d);"
        "window.MainComponent.$store.commit(\"setReadonly\",%s);"
        "window.MainComponent.$store.commit(\"setMoveMode\",%s);"
        "window.MainComponent.$store.commit(\"setTechnicalCondition\",%s);"
        "window.MainComponent.$store.commit(\"setEditMode\",%s);"
        "window.MainComponent.$store.commit(\"setAddMode\",%s);"
        "window.MainComponent.$store.commit(\"setDeleteMode\",%s);"
        "window.MainComponent.$store.commit(\"setLiteMode\",%s);"
        "window.MainComponent.$store.commit(\"setCustomEditMode\",%s);"
        "window.MainComponent.$store.commit(\"setOnWordMode\",%s);"
        "window.MainComponent.$store.commit(\"setOnMapMode\",%s);",
        rusTableName,
        //    titleName,

        countAllRows, readOnly ? "true" : "false", isMoveTo ? "true" : "false",
        isTechnicalCondition ? "true" : "false",
        editMode ? "true" : "false",
        addMode ? "true" : "false",
        deleteMode ? "true" : "false",
        liteMode ? "true" : "false",
        customEditMode ? "true" : "false",
        wordMode ? "true" : "false",
        markMode ? "true" : "false" );



    CString gg = "";

    if (tableName == "remont2") {
        m_setGrouping = "col_3";
        gg.Format(" window.MainComponent.$store.commit(\"setGrouping\",\"%s\");", m_setGrouping);
        query += gg;
    }

    if (tableName == "osmotr_zapros_po_trubam") {
        m_setGrouping = "col_1";
        gg.Format(" window.MainComponent.$store.commit(\"setGrouping\",\"%s\");", m_setGrouping);
        query += gg;
    }

    if (tableName.Find("Анализ контуров ремонта") == 0) {
        m_setGrouping = "col_3";
        gg.Format(" window.MainComponent.$store.commit(\"setGrouping\",\"%s\");", m_setGrouping);
        query += gg;
    }

    if (specialForTeplopotery) {
        CAdoFile* adoSprav = getAdo("sprav");
        query += getHeaderTeplopotery(teplopoteryTableIndex, adoSprav, true);
    }
    else if (specialHeader) {
        query += getSpecialHeader(impDataSource->m_sTableName_main, true);
    }
    
    
    query += totalPage + " window.MainComponent.$store.commit(\"setHeaderList\", [";
    for (long i = 0; i < countFields; i++) {
        vField.push_back(impDataSource->nameField(i));
    }
    if (m_head.width.size() > 0)
        for (long i = 0; i < countFields; i++) {
            set<int>::const_iterator it = m_head.is_edit.find(i);

            CString fn = impDataSource->nameField(i);

            if (it != m_head.is_edit.end() || i == 0 || fn.Find("_TAB") == -1) {

                //              == "nodeName1" || fn == "nodeName2") {
                impDataSource->positionS.insert(std::make_pair(i, i));
            }
            else {
                impDataSource->positionS.insert(std::make_pair(i, i - 1));
                impDataSource->positionS[i - 1] = -1;
            }
        }
    else if ((regex_match(titleName.GetString(), regex("Тип (арматуры|назначения|конструкции|конструкции камеры|исполнения|месторасположения|материала|характеристики)"))))
        for (long i = 0; i < countFields; i++) {
            int index;
            if (i == 2)
                index = -2;
            else if (i == 3)
                index = -1;
            else
                index = i;
            impDataSource->positionS.insert(std::make_pair(i, index));

        }
    else {
        for (long i = 0; i < countFields; i++) {
            impDataSource->positionS.insert(std::make_pair(i, i));
        }
    }

    int ii = 0;

    for (long i = 0; i < countFields; i++) {

        if (impDataSource->positionS[i] != -1) {

            /*
                        if (m_head.width.size() > 0 && m_head.width[ii] == 0 && ii > 0) ii++;

                        CString fName = m_head.map_h[0][ii];
                        CString fName2 = impDataSource->getFullName1(impDataSource->nameField(i));
                        ii++;
            */

            CString fName;
            if (m_head.width.size() > 0)
                fName = m_head.map_h[0][i];
            else
                fName = impDataSource->getFullName1(impDataSource->nameField(i));


            if (fName == "")
                fName = impDataSource->getFullName1(impDataSource->nameField(i));

            DataTypeEnum fieldN = impDataSource->fieldVariant(i);
            CString varType;

            //Для Excel формат ячейки
            CString colName;
            colName.Format("col_%d", i);
            impDataSource->excelType.insert(std::pair<CString, int>(colName, fieldN));

            switch (fieldN) {
            case adLongVarWChar:
            case adVarChar:
            case adVarWChar:
                varType = "string";
                break;
            case adInteger:
            case adDouble:
                varType = "number";
                break;
            case adDBTimeStamp:
            case adDate:
            case adDBDate:
                varType = "date";
                break;
            }

            if (strcmp(fName, "Здание") == 0)
                varType = "zdanie";


            CString dynamicData = " type : \"" + varType + "\", list : []";
            const AdoLookup* look = m_head.getLookup(i);
            if (look) {

                CString bd = look->db;
                bd.Replace("$ps$", getPsAdoName());
                bd.Replace("$main$", getGidAdoName());

                CAdoFile* ado = getAdo(bd);

                if (ado->openTable0(look->getQ())) {
                    dynamicData = " type : \"dynamic\", list : [";
                    while (!ado->isEOF()) {
                        long id = ado->read_long(0);
                        CString name = CCrack::strVARIANT_UTF(ado->read(1));
                        CString idNameData;
                        idNameData.Format(" {id : %d, value : `%s`},", id, name);
                        dynamicData += idNameData;
                        ado->MoveNext();
                    }
                    dynamicData += "]";

                    ado->closeTable();
                }

                //bool ret = viewVirtualDlg(this, look->db, look->getQ(), id, look->rus_name, text);
            }
            else if (impDataSource->positionS[i] == -2) {
                dynamicData = " type : \"multiply\", list : [";
                if (mAdo->openTable0("select id, name as 'tblName', rus_name as 'Объект' from listPtsTables")) {

                    while (!mAdo->isEOF()) {
                        CString idNameData;
                        idNameData.Format(" {name : \"%s\", rus_name : \"%s\"},", mAdo->readStr(1), mAdo->readStr(2));
                        dynamicData += idNameData;

                        mAdo->MoveNext();
                    }

                    mAdo->closeTable();
                }
                dynamicData += "]";
            }
            CString title;

            title.Format("{value: \"col_%d\", text : \"%s\",sortable : true, %s},", i, fName, dynamicData);
            query += title;

        }
    }
    query += "]);\n";
    return query;
}



CString CWebViewTable::GetNextValues() {

    bool hasNextData = false;
    bool firstTime = false;
    CString query;
    if (!initDataSource) {
        initDataSource = true;
        long rows = impDataSource->GetNumRows();

        hasNextData = true;
        firstTime = true;

        if (firstTime) {
            query += GetHeaderTable();
            if (viewAll)
                query += "window.MainComponent.$store.commit(\"setAllMode\", true); window.MainComponent.$store.commit(\"setDataList\", [";
            else
                query += "window.MainComponent.$store.commit(\"setDataList\", [";

        }
        if (dataStorageId.empty())
            ObtainDataToStorage();

    }

    else {
        if ((currentDataStorageIndex + impDataSource->portionCount) > dataStorageId.size())
        {
            int resultFetching = impDataSource->OnHitBottom(currentCount);
            if (resultFetching == UG_SUCCESS)
            {
                hasNextData = true;
                ObtainDataToStorage();
            }
        }
        else {
            hasNextData = true;
        }
    }

    if (true) {
        //догружаем данные с бд и сохраняем их в переменную dataStorage (если это необходимо)
        bool hasNextData = true;
        while (hasNextData) {

            int resultFetching = impDataSource->OnHitBottom(currentCount);

            hasNextData = resultFetching == UG_SUCCESS;
            if (hasNextData)
                ObtainDataToStorage();

        }

        CString sumQuery;

        int countPage = ceil(dataStorageId.size() / 1000.0);
        sumQuery = GetValuesByRange(0, dataStorageId.size() - 1);
        if (!sumQuery.IsEmpty()) {
            query += sumQuery;
        }
    }
    else
        if (hasNextData) {

            query += GetQueryAppend();
        }
        else
            query += "";

    if (firstTime)
        query += "]);";

    return query;
}



CString CWebViewTable::GetAllValues() {
    int c1 = 0;
    int count = 0;
    CString query;

    while (c1 != dataStorageId.size()) {
        CString row;
        row = dataStorage[dataStorageId[c1]];

        //****
        //  // Перевод CString в std::string для удобства обработки
        std::string strData = CT2A(row);


        // Паттерн для поиска двойных кавычек после одинарных
        //std::regex pattern("`([^`]*?)`\\s*\"([^\"]*)\"");

        // Замена найденных двойных кавычек на двойные кавычки с обратным слэшем перед ними
        //strData = std::regex_replace(strData, pattern, "`$1` \"\\\\$2\"");

        //// Паттерн регулярного выражения для поиска блоков одинарных кавычек и двойных кавычек внутри них
        //std::regex pattern("`\"(.*?)\"`");

        //// Поиск и замена совпадений вручную
        //std::sregex_iterator iter(strData.begin(), strData.end(), pattern);
        //std::sregex_iterator end;

        //while (iter != end) {
        //    // Получаем совпадение
        //    std::smatch match = *iter;
        //    // Получаем содержимое блока одинарных кавычек
        //    std::string block = match.str(1);
        //    // Заменяем каждую двойную кавычку в блоке на двойную кавычку с обратным слешем перед ней
        //    for (size_t i = 0; i < block.length(); ++i) {
        //        if (block[i] == '"') {
        //            block.insert(i, "\\"); // Вставляем обратный слеш перед каждой двойной кавычкой
        //            ++i; // Пропускаем вставленный обратный слеш
        //        }
        //    }
        //    // Заменяем найденное совпадение в строке
        //    strData.replace(match.position(), match.length(), "`\"" + block + "\"`");
        //    // Перемещаемся к следующему совпадению
        //    ++iter;
        //}



        // Обратно переводим std::string в CString
        row = strData.c_str();
        //row.Replace("`","\"");
        //****
        query += row;
        count++;
        c1++;
    }
    return query;
}

CString CWebViewTable::GetValuesByRange(int firstIndex, int endIndex) {
    int c1 = firstIndex;
    CString query;
    int c2 = endIndex;
    if (endIndex > dataStorageId.size() - 1)
        c2 = dataStorageId.size() - 1;
    while (c1 <= c2) {
        CString row;
        row = dataStorage[dataStorageId[c1]];
        query += row;
        c1++;
    }
    return query;
}


CString CWebViewTable::GetQueryAppend() {
    CString query;
    int size1 = dataStorageId.size() - 1;
    if (currentDataStorageIndex <= size1) {

        int c1 = 0;

        while (c1 != impDataSource->portionCount) {
            if (currentDataStorageIndex <= size1)
            {

                query += dataStorage[dataStorageId[currentDataStorageIndex]];


            }
            else
            {
                break;
            }
            currentDataStorageIndex++;
            c1++;
        }

    }
    else
    {
        query = "";
    }
    return query;

}

CString CWebViewTable::ObtainDataToStorage() {
    CString qStorage;
    int count = 0;
    if (impDataSource->m_pcRowsObtained > 0)
    {

        while (count != impDataSource->m_pcRowsObtained)
        {

            qStorage = "{";
            char buffer[100];
            sprintf_s(buffer, "My Number %d\n", currentCount);
            OutputDebugStringA(buffer);
            int id = -1;
            bool firstColumn = true;
            for (long i = 0; i < countFields; i++) {
                long rgIndices[2];
                rgIndices[0] = i;
                rgIndices[1] = count;
                if (impDataSource->positionS[i] != -1)
                    try
                {
                    _variant_t v;

                    if (impDataSource->m_curRecords.vt != VT_EMPTY)
                        SafeArrayGetElement(impDataSource->m_curRecords.parray, rgIndices, &v);

                    if (i == 0) {
                        impDataSource->positionById.insert(std::make_pair(v.lVal, currentCount));
                        id = v.lVal;

                    }
                    CString tdQuery, s, comma2, colName;
                    double d;
                    colName.Format("col_%d", i);
                    if (!firstColumn)
                        comma2 = ",";
                    switch (v.vt)
                    {
                    case 1:
                        sortDataStorage[colName].push_back(std::pair(id, ""));
                        tdQuery.Format(_T("%s\"col_%d\" : \"\""), comma2, i);
                        break;
                    case 2:
                    case 3:
                        sortDataStorage[colName].push_back(std::pair(id,v.lVal));
                        tdQuery.Format(_T("%s\"col_%d\" : %d"), comma2,i, v.lVal);

                        break;
                    case 4:
                    case 5:
                        d = impDataSource->to_double(v);
                        sortDataStorage[colName].push_back(std::pair(id, d));
                        tdQuery.Format(_T("%s\"col_%d\" : %f"), comma2, i, d);
                        break;
                    case 7:
                    case 8:
                        s = CCrack::strVARIANT_UTF(v);
                        
                        s.Replace("\n", "");
                        s.Replace("\r", "");
                        s.Replace("\\", "\\\\");
                        s.Replace("\"", "\\\"");
                        sortDataStorage[colName].push_back(std::pair(id, s));
                        tdQuery.Format(_T("%s\"col_%d\" : \"%s\""), comma2,  i, s );

                        break;
                    default:
                        sortDataStorage[colName].push_back(std::pair(id, "?"));
                        tdQuery.Format(_T("%s\"col_%d\" : \"?\""), comma2,i);
                        break;
                    }
                    if (impDataSource->positionS[i] == -2) {
                        //для объектов птс генерация массива из строки 
                        CString sToken = _T("");
                        tdQuery = "";
                        int indexToken = 0;
                        bool firstToken = true;
                        CString all = "";
                        while (AfxExtractSubString(sToken, CCrack::strVARIANT_UTF(v), indexToken, ',')) {
                            if (!firstToken) {
                                all += "\",\"";
                            }
                            else {
                                firstToken = false;
                            }
                            all += sToken;
                            indexToken++;
                        }
                        // Обернуть в кавычки всю строку, если строка не пустая
                        if (!all.IsEmpty()) {
                            all = "\"" + all + "\"";
                        }
                        CString arr;
                        arr.Format("[%s]", all);
                        sortDataStorage[colName].push_back(std::pair(id, arr));
                        tdQuery.Format(_T("%s\"col_%d\" : [%s]"), comma2, i, all);
                    }
                    if (firstColumn)
                        firstColumn = false;
                    if (s.IsEmpty())
                        tdQuery.Replace("\\", "\\\\");


                    qStorage += tdQuery.AllocSysString();
                }
                catch (...)
                {
                    CString q;
                    q.Replace("\\", "\\\\");
                }
            }

            qStorage += "},";
            dataStorageId.push_back(id);
            dataStorage[id] = qStorage;


            //mAdo->m_type_of_net
            currentCount++;
            count += 1;
        }

    }

    return "OK";

}

// Компаратор для сравнения пар по второму значению
struct CompareSecond {
    bool operator()(const std::pair<int, std::any>& left, const std::pair<int, std::any>& right) const {
        try {
            // Получаем типы значений вторых элементов пар
            const std::type_info& leftType = left.second.type();
            const std::type_info& rightType = right.second.type();

            // Сравниваем типы значений
            if (leftType != rightType) {
                // Если типы не совпадают, считаем, что их нельзя сравнивать,
                // и просто возвращаем false
                return false;
            }

            // Если типы совпадают, пробуем выполнить сравнение значений
            if (leftType == typeid(int)) {
                return std::any_cast<int>(left.second) < std::any_cast<int>(right.second);
            }
            else if (leftType == typeid(double)) {
                return std::any_cast<double>(left.second) < std::any_cast<double>(right.second);
            }
            else if (leftType == typeid(std::string)) {
                return std::any_cast<std::string>(left.second) < std::any_cast<std::string>(right.second);
            }
            else if (leftType == typeid(const char*)) {
                return std::strcmp(std::any_cast<const char*>(left.second), std::any_cast<const char*>(right.second)) < 0;
            }
            else if (leftType == typeid(CString)) {
                return std::strcmp(std::any_cast<CString>(left.second), std::any_cast<CString>(right.second)) < 0;
            }
            // Добавьте дополнительные типы, если необходимо

            // В случае, если типы неизвестны или не поддерживаются, просто вернем false
            return false;
        }
        catch (const std::bad_any_cast&) {
            // Если произошла ошибка приведения типов, считаем, что их нельзя сравнивать,
            // и просто возвращаем false
            return false;
        }
    }
};


CString CWebViewTable::SortData(CString colName, bool desc) {
    // Сортировка данных по второму значению pair
    std::sort(sortDataStorage[colName].begin(), sortDataStorage[colName].end(), CompareSecond());
    return "";
}


void setView(CView* pView0);
void CWebViewTable::OnMoveTo(int id) // перейти в окно схемы
{

    VIRT_DATA typ = impDataSource->myType;

    CGidrView* pView = getView();
    if (pView) {
        setView(pView);

        switch (typ) {
        case virt_data_node_noedit:
        case virt_data_node:
        case virt_data_node_out:
            pView->SendMessage(WM_USER_FIND_NODE, (WPARAM)id);
            break;
        case virt_data_line_noedit:
        case virt_data_line:
        case virt_data_line_out:
            pView->SendMessage(WM_USER_FIND_LINE, (WPARAM)id);
            break;
        case virt_data_line_big:
            pView->SendMessage(WM_USER_FIND_LINE_BIG, (WPARAM)id);
            break;
        case virt_data_line_big_osmotr:
            pView->SendMessage(WM_USER_FIND_LINE_BIG_OSMOTR, (WPARAM)id);
            break;
        case virt_data_geo:
        case virt_data_geo_del:
        case virt_data_geo_edit:
        {
            CString eng_name = findTableEngName(impDataSource->m_sDbName, impDataSource->m_sTableName);
            pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM)(LPCSTR)eng_name);
        }
        break;
        case virt_data_tu:
        {
            pView->SendMessage(WM_USER_FIND_GEO_TU, (WPARAM)id, (LPARAM)(LPCSTR)transl("Здания ТУ"));
        }
        break;
        }
    }
}


int CWebViewTable::addNew(BSTR str)
{
    std::wstring ws(str, SysStringLen(str));

    auto myJson = json::parse(ws);
    map<int, std::any> arr;
    for (long i = 0; i < countFields; i++) {
        string title = "col_" + std::to_string(i);


        if (myJson.contains(title) > 0) {
            auto j1 = myJson[title];
            arr.insert(std::make_pair(i, j1));
        }
    }
    //догружаем данные с бд и сохраняем их в переменную dataStorage (если это необходимо)
    bool hasNextData = true;
    while (hasNextData) {

        int resultFetching = impDataSource->OnHitBottom(currentCount);

        hasNextData = resultFetching == UG_SUCCESS;
        if (hasNextData)
            ObtainDataToStorage();

    }

    int result = impDataSource->addNew(currentCount, arr);

    return result;

}


int CWebViewTable::addNewOld(BSTR str)
{
    std::wstring ws(str, SysStringLen(str));

    auto myJson = json::parse(ws);
    map<int, std::any> arr;
    for (long i = 0; i < countFields; i++) {
        string title = "col_" + std::to_string(i);
        auto j1 = myJson[title];
        if (!j1.is_null()) {
            arr.insert(std::make_pair(i, j1));

        }
    }
    int result = impDataSource->addNewOld(currentCount, arr);

    return result;

}

int CWebViewTable::updateData(int id, BSTR str)
{
    std::wstring ws(str, SysStringLen(str));

    auto myJson = json::parse(ws);
    map<int, std::any> arr;
    for (long i = 0; i < countFields; i++) {
        string title = "col_" + std::to_string(i);
        auto j1 = myJson[title];
        if (!j1.is_null()) {
            arr.insert(std::make_pair(i, j1));

        }
    }
    return  impDataSource->updateData(id, impDataSource->positionById[id], arr);

}

void CWebViewTable::OnMarkAccessibility()
{
    // Get the current view
    CGidrView* gid = getView();
    if (!gid) return;

    // Move current view to ensure it's active
    gid->moveCurrent();


    int modeCol = -1;
    CString markColumn = _T(impDataSource->addText);  // The column we're looking for

   
    for (long i = 0; i < countFields; i++) {
        CString fieldName = impDataSource->nameField(i);
        if (fieldName.CompareNoCase(markColumn) == 0) {
            modeCol = i;
            break;
        }
    }

    
    if (modeCol == -1) {
        for (long i = 0; i < countFields; i++) {
            CString headerText;
            if (m_head.width.size() > 0)
                headerText = m_head.map_h[0][i];
            else
                headerText = impDataSource->getFullName1(impDataSource->nameField(i));

            if (headerText.CompareNoCase(markColumn) == 0) {
                modeCol = i;
                break;
            }
        }
    }

    // If still not found, exit
    if (modeCol == -1) {
        return;
    }

    // Define colors (matching the Qt implementation)
    const int COLOR_ABOVE = 0x00A5FF;  // Orange
    const int COLOR_BELOW = 0xFF0000;  // Blue
    const int COLOR_OTHER = 0x0000FF;  // Red

    // Ensure we have all data loaded
    bool hasNextData = true;
    while (hasNextData) {
        int resultFetching = impDataSource->OnHitBottom(currentCount);
        hasNextData = resultFetching == UG_SUCCESS;
        if (hasNextData)
            ObtainDataToStorage();
    }

    // Map to store node ID to color mapping
    std::unordered_map<int, int> idToColor;

    // Process each row in our data storage
    for (const auto& id : dataStorageId) {
        // Get the data for this row
        auto it = dataStorage.find(id);
        if (it != dataStorage.end()) {
            CString rowData = it->second;

            // Extract the mode value from the JSON string
            CString modeValue = _T("");

            // Parse the JSON-like string to find the mode column value
            CString searchPattern;
            searchPattern.Format(_T("\"col_%d\" : "), modeCol);

            int pos = rowData.Find(searchPattern);
            if (pos != -1) {
                // Move past the pattern
                pos += searchPattern.GetLength();

                // Handle different value types (string vs number)
                if (pos < rowData.GetLength() && rowData[pos] == '"') {
                    // String value - extract between quotes
                    int start = pos + 1;
                    int end = rowData.Find('"', start);
                    if (end != -1) {
                        modeValue = rowData.Mid(start, end - start);
                    }
                }
                else {
                    // Number or other value - extract until comma or closing brace
                    int start = pos;
                    int end1 = rowData.Find(',', start);
                    int end2 = rowData.Find('}', start);

                    int end = (end1 != -1 && end2 != -1) ? min(end1, end2) :
                        (end1 != -1) ? end1 : end2;

                    if (end != -1) {
                        modeValue = rowData.Mid(start, end - start);
                        modeValue.Trim();
                    }
                }
            }

            modeValue.MakeLower().Trim();

            // Determine color based on mode value
            int color = COLOR_OTHER; // Default red color

            
            if (modeValue.Find(_T("выше")) != -1 || modeValue.Find(_T("превыш")) != -1 || modeValue.Find(_T("перерасход")) != -1) {
                color = COLOR_ABOVE; // Orange
            }
            
            else if (modeValue.Find(_T("ниже")) != -1 || modeValue.Find(_T("дефицит")) != -1) {
                color = COLOR_BELOW; // Blue
            }

            // Add to our mapping
            idToColor[id] = color;
        }
    }

    // Apply the colors to nodes
    if (!idToColor.empty()) {
        gid->applyNodeColors(idToColor);
    }
    CGidrView* pView = getView();
    if (pView) 
        setView(pView);
}


void CWebViewTable::OnDraw(CDC* pDC)
{

}

void CWebViewTable::OnDestroy()
{
    m_pWebBrowser.reset();

    CView::OnDestroy();
}


LRESULT CWebViewTable::OnAdoUpdated(WPARAM wParam, LPARAM lParam)
{
    CPropertyParam* param = (CPropertyParam*)lParam;
    if (param) {


        CString q = impDataSource->updateData2(param->id, impDataSource->positionById[param->id], countFields);
        dataStorage[param->id] = q;
        CString query;
        query.Format("window.tableComponent.updateItem(%d, %s);", param->id, q);
        BSTR myQuery = query.AllocSysString();
        m_pWebBrowser->ExecuteScript(
            myQuery);

        //метод для преобразования arr в json и сохранения в dataStorage

        //AfxMessageBox(param->table);


    }

    return 0;
}