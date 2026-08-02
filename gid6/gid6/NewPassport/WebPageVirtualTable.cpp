#include "stdafx.h"
#include "WebPageVirtualTable.h"
#include "gidrview.h"
#include "wm_user.h"

#include <win.h>
#include <wil/com.h>
#include "resource.h"
#include <nlohmann/json.hpp>
#include <regex>

using nlohmann::json;
double getScreenMas();



IMPLEMENT_DYNCREATE(WebPageVirtualTable, CPaneDialog)

BEGIN_MESSAGE_MAP(WebPageVirtualTable, CPaneDialog)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_COMMAND(ID_REFRESH_DATA, OnRefreshData)
    ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)

    ON_COMMAND(ID_SHIFT_F1, OnShiftF1)
    ON_COMMAND(ID_EDIT_FIND, OnEditFind)

END_MESSAGE_MAP()


WebPageVirtualTable::WebPageVirtualTable()
{


    m_bInit = FALSE;

}

WebPageVirtualTable::~WebPageVirtualTable()
{

}

int WebPageVirtualTable::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CPaneDialog::OnCreate(lpCreateStruct) == -1)
        return -1;


    return 0;
}

void WebPageVirtualTable::OnSize(UINT nType, int cx, int cy)
{
    CPaneDialog::OnSize(nType, cx, cy);



    if (!m_bInit) return;

    SetWindowText(m_title);

    if (webViewInit && webviewController != nullptr) {
        RECT bounds;
        GetClientRect(&bounds);
        webviewController->put_Bounds(bounds);
    }
}

void WebPageVirtualTable::OnSetFocus(CWnd* pOldWnd)
{
    CPaneDialog::OnSetFocus(pOldWnd);

    if (!m_bInit) return;

    SetWindowText(m_title);
}

BOOL WebPageVirtualTable::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
    return TRUE;
    // return CPaneDialog::OnEraseBkgnd(pDC);
}


/////////////////////////////////////////////////////////////////////////////
//  OnRefreshData
//    The OnRefreshData function will force the data in current sheet to
//    be reloaded from the database.
void WebPageVirtualTable::OnRefreshData()
{
    /*  int nSheetNumber = m_grid.GetSheetNumber();
      _tagDSList* pTempItem = m_pDSList;

      for (int nIndex = 0; nIndex < nSheetNumber && pTempItem != NULL; nIndex++)
          pTempItem = pTempItem->next;

      if (pTempItem == NULL)
          return;

      pTempItem->m_dsADO->RefreshData();
      m_grid.RedrawAll();*/
}

/////////////////////////////////////////////////////////////////////////////
//  OnShowSql
//    The OnShowSql function provides user with ability to view and modify
//    current SQL query.
void WebPageVirtualTable::OnShowSql()
{

}

void setView(CView* pView0);

void WebPageVirtualTable::OnShiftF1()
{
    /*int col = m_grid.GetCurrentCol();
    long row = m_grid.GetCurrentRow();

    CString s;
    s.Format("%d %d", row, col);
    AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);*/
}

LRESULT WebPageVirtualTable::HandleInitDialog(WPARAM, LPARAM)
{
    UpdateData(FALSE);

    //    m_grid.AttachGrid(this, IDC_LIST1);
    return TRUE;
}

bool WebPageVirtualTable::init(int type_of_net, const char* db, const char* connect, const char* tn, const char* q, const char* title, bool is_head, VIRT_DATA typ)
{
    if (is_head && readTable(db, tn, m_head)) {

    }


    impDataSource = new WVAdoImpDataSource;
    impDataSource->m_q = q;

    impDataSource->setAllows(typ);
    if (impDataSource->OpenSQL(q, tn, connect, type_of_net) == UG_ERROR) {
        return false;
    }

    countAllRows = impDataSource->GetNumRowsComplete();

    impDataSource->SetTableName(db, tn);

    int nf = impDataSource->countFields();

    countFields = nf;

    m_title = title;
    m_typ = virt_data_geo;
    m_typ = typ;
    m_tn = tn;
    m_q = q;

    m_bInit = TRUE;



    SetWindowText(m_title);

    if (!webViewInit)
        InitWebView();

    return true;
}

void WebPageVirtualTable::OnEditFind()
{

}


void WebPageVirtualTable::OnDestroy()
{

    CPaneDialog::OnDestroy();
}

CString findTableRusName(CString d, CString n);
CString findTableEngName(CString d, CString n);



CString WebPageVirtualTable::GetNextValues() {

    bool hasNextData = false;
    bool firstTime = false;
    if (!initDataSource) {
        initDataSource = true;
        long rows = impDataSource->GetNumRows();
        //if (impDataSource->m_pcRowsObtained > 0)
        hasNextData = true;
        firstTime = true;
    }

    else {
        int resultFetching = impDataSource->OnHitBottom(currentCount);
        if (resultFetching == UG_SUCCESS)
        {
            hasNextData = true;
        }
    }
    CString query;
    if (hasNextData) {

        query = GetQueryAppend(firstTime);
    }
    else
        query = "";


    return query;

}

//Заполнение заголовков таблицы
CString WebPageVirtualTable::GetHeaderTable() {
    BOOL readOnly = true;
    if (impDataSource->allowAdd && impDataSource->allowEdit && impDataSource->allowDelete)
        readOnly = false;

    BOOL isTechnicalCondition = strcmp(m_tn, "Технические условия") == 0;


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
    totalPage.Format("window.MainComponent.$store.commit(\"setLiteMode\",true); window.MainComponent.$store.commit(\"setTableHeader\",\"%s\"); window.MainComponent.$store.commit(\"setTotalPage\",%d); window.MainComponent.$store.commit(\"setReadonly\",%s); window.MainComponent.$store.commit(\"setMoveMode\",%s); window.MainComponent.$store.commit(\"setTechnicalCondition\",%s);", rusTableName, countAllRows, readOnly ? "true" : "false", isMoveTo ? "true" : "false", isTechnicalCondition ? "true" : "false");
    query += totalPage + " window.MainComponent.$store.commit(\"setHeaderList\", [";
    for (long i = 0; i < countFields; i++) {
        vField.push_back(impDataSource->nameField(i));
    }
    if (m_head.width.size() > 0)
        for (long i = 0; i < countFields; i++) {


            if (m_head.getLookup(i) == NULL || i == 0) {
                impDataSource->positionS.insert(std::make_pair(i, i));
            }
            else {
                impDataSource->positionS.insert(std::make_pair(i, i - 1));
                impDataSource->positionS[i - 1] = -1;

            }

        }
    else if ((regex_match(m_tn.GetString(), regex("Тип (арматуры|назначения|конструкции|конструкции камеры|исполнения|месторасположения|материала|характеристики)"))))
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

    for (long i = 0; i < countFields; i++) {

        if (impDataSource->positionS[i] != -1) {
            CString fName;
            if (m_head.width.size() > 0)
                fName = m_head.map_h[0][i];
            else
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
CString WebPageVirtualTable::GetQueryAppend(bool firstTime) {
    CString query;
    int count = 0;
    if (impDataSource->m_pcRowsObtained > 0)
    {

        while (count != impDataSource->m_pcRowsObtained)
        {

            if (count == 0 && firstTime) {
                query += GetHeaderTable();
                query += "window.MainComponent.$store.commit(\"setDataList\", [";
            }


            query += "{";

            char buffer[100];
            sprintf_s(buffer, "My Number %d\n", currentCount);
            OutputDebugStringA(buffer);
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
                    }
                    CString tdQuery;
                    switch (v.vt)
                    {
                    case 1:
                        tdQuery.Format(_T("\"col_%d\" : \"\","), i);
                        break;
                    case 2:
                    case 3:
                        tdQuery.Format(_T("\"col_%d\" : %d,"), i, v.lVal);

                        break;
                    case 4:
                    case 5:
                        tdQuery.Format(_T("\"col_%d\" : %f,"), i, impDataSource->to_double(v));
                        break;
                    case 7:
                    case 8:
                        tdQuery.Format(_T("\"col_%d\" : `%s`,"), i, CCrack::strVARIANT_UTF(v));

                        break;
                    default:
                        tdQuery.Format(_T("\"col_%d\" : \"?\","), i);
                        break;
                    }
                    if (impDataSource->positionS[i] == -2) {
                        //для объектов птс генерация массива из строки 
                        CString sToken = _T("");
                        tdQuery = "";
                        int indexToken = 0;
                        CString all = "";
                        while (AfxExtractSubString(sToken, CCrack::strVARIANT_UTF(v), indexToken, ','))
                        {
                            all = all + "\"" + sToken + "\",";
                            indexToken++;
                        }
                        tdQuery.Format(_T("\"col_%d\" : [%s],"), i, all);
                    }
                    tdQuery.Replace("\\", "\\\\");
                    query += tdQuery;
                }
                catch (...)
                {
                }


            }
            query += "},";


            //mAdo->m_type_of_net
            currentCount++;
            count += 1;
        }
        if (firstTime)
            query += "]);";
    }
    else
    {
        query += GetHeaderTable();
        query += " this.MainComponent.$store.commit(\"setDataList\", []);";
    }
    return query;
}

void WebPageVirtualTable::OnMoveTo(int id) // перейти в окно схемы
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
            pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM)(LPCSTR)m_tn);
            break;
        case virt_data_geo_big_line:
            pView->SendMessage(WM_USER_FIND_GEO_BIG_LINE, (WPARAM)id, (LPARAM)(LPCSTR)m_tn);
            break;
        }
    }
}


int WebPageVirtualTable::addNew(BSTR str)
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
    return impDataSource->addNew(currentCount, arr);

}

int WebPageVirtualTable::updateData(int id, BSTR str)
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

void WebPageVirtualTable::InitWebView() {



    HWND hWnd = m_hWnd;//CWnd::GetSafeHwnd();
    // <-- WebView2 sample code starts here -->
    // Step 3 - Create a single WebView within the parent window
    // Locate the browser and set up the environment for WebView



    CString argpath_2();


    CString cs = argpath_2();
    BSTR bstr = cs.AllocSysString();
    std::wstring ws(bstr, SysStringLen(bstr));

    std::wstring m_userDataFolder;

    m_userDataFolder = ws;

    //    L"C:\\MyAppUserDataFolder"

    //    auto options = Microsoft::WRL::Make<CoreWebView2ExperimentalEnvironmentOptions>();
    //wil::com_ptr<ICoreWebView2> webviewWindow;
    //wil::com_ptr <ICoreWebView2Controller> webviewController;
    CreateCoreWebView2EnvironmentWithOptions(
        //        nullptr, m_userDataFolder.c_str(), options.Get(),
        nullptr, m_userDataFolder.c_str(), nullptr,
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [hWnd, this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

                // Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
                env->CreateCoreWebView2Controller(hWnd, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                    [this](HRESULT result, ICoreWebView2Controller* controller)->HRESULT {

                        if (controller != nullptr) {
                            webviewController = controller;
                            controller->get_CoreWebView2(&webviewWindow);
                        }

                        // Add a few settings for the webview
                        // The demo step is redundant since the values are the default settings
                        ICoreWebView2Settings* Settings;
                        webviewWindow->get_Settings(&Settings);
                        Settings->put_IsScriptEnabled(TRUE);
                        Settings->put_AreDefaultScriptDialogsEnabled(TRUE);
                        Settings->put_IsWebMessageEnabled(TRUE);
                        Settings->put_AreDefaultContextMenusEnabled(false);
                        RECT bounds;
                        GetClientRect(&bounds);
                        bounds.bottom -= 40;
                        // Resize WebView to fit the bounds of the parent window
                        controller->put_Bounds(bounds);

                        wil::com_ptr<ICoreWebView2_3> wvWnd3 = webviewWindow.try_query<ICoreWebView2_3>();

                        CString fStr;
                        fStr.Format("%stableServer", argpath());
                        wvWnd3->SetVirtualHostNameToFolderMapping(L"appassets.example",
                            fStr.AllocSysString(),
                            COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);
                        //*************AddHostObject


                        m_hostObject = Microsoft::WRL::Make<HostObjectSample>(
                            // callback для метода добавления
                            [this](BSTR jsonString, std::function<void(int, int, BSTR)> callbackInsert)
                            {
                                int id = addNew(jsonString);
                                if (id > -1)
                                {
                                    impDataSource->positionById.insert(std::make_pair(id, currentCount));
                                    if (currentCount >= countAllRows - 1)
                                        currentCount++;
                                    callbackInsert(id, 200, L"OK");
                                }
                                else
                                    callbackInsert(id, 404, L"BAD");
                            },
                            //callback для метода обновления
                                [this](INT id, BSTR jsonString, std::function<void(int, BSTR)> callbackChange)
                            {

                                int code = updateData(id, jsonString);

                                callbackChange(code, L"OK");

                            },
                                //callback для метода удаления
                                [this](vector<int> ids, std::function<void(int, BSTR)> callbackChange)
                            {
                                int code;
                                CString message;
                                pair<int, CString> resultData;
                                if (ids.size() == 1)
                                {
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

                                callbackChange(code, message.AllocSysString());
                            },


                                //callback для перемещения к объекту в расчетной схеме
                                [this](int id, BSTR type, BSTR value, std::function<void(int, BSTR)> callbackChange)
                            {
                                OnMoveTo(id);
                                callbackChange(200, L"OK");

                            },
                                //callback для Excel
                                [this](BSTR jsonString, std::function<void(int, BSTR)> callbackChange)
                            {
                                impDataSource->OnExcel(jsonString);
                                callbackChange(200, L"OK");

                            },

                                //callback для метода pickupfile
                                [this](BSTR field, std::function<void(BSTR, BSTR, int, BSTR)> callbackPickupFile) {
                            },
                            
                            //callback для метода утверждения
                                [this](INT id, BSTR jsonString, std::function<void(int, BSTR, BSTR)> callbackChange)
                            {

//                              int code = updateData(id, jsonString);

//                              callbackChange(code, L"OK");

                            },

                            //callback для метода печати
                                [this](BSTR jsonString, int count) {
                            },
                            //callback для метода поиска
                                [this](BSTR jsonString, std::function<void(int, BSTR)> callbackChange) {
                            }


                           


                            );


                        wvWnd3->add_NavigationStarting(
                            Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(
                                [this](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT
                                {
                                    wil::unique_cotaskmem_string navigationTargetUri;
                                    args->get_Uri(&navigationTargetUri);


                                    //! [AddHostObjectToScript]
                                    VARIANT remoteObjectAsVariant = {};
                                    m_hostObject.query_to<IDispatch>(&remoteObjectAsVariant.pdispVal);
                                    remoteObjectAsVariant.vt = VT_DISPATCH;

                                    // We can call AddHostObjectToScript multiple times in a row without
                                    // calling RemoveHostObject first. This will replace the previous object
                                    // with the new object. In our case this is the same object and everything
                                    // is fine.

                                    webviewWindow->AddHostObjectToScript(L"table", &remoteObjectAsVariant);
                                    remoteObjectAsVariant.pdispVal->Release();

                                    return S_OK;
                                }).Get(), &m_navigationStartingToken);


                        //**********~AddHostObject
                        // Schedule an async task to navigate to Bing
                        wvWnd3->Navigate(L"https://appassets.example/index.html");

                        // Step 4 - Navigation events
                        // register an ICoreWebView2NavigationStartingEventHandler to cancel any non-https navigation

                        wvWnd3->add_NavigationStarting(Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(
                            [](ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
                                PWSTR uri;
                                args->get_Uri(&uri);
                                std::wstring source(uri);
                                if (source.substr(0, 5) != L"https") {
                                    args->put_Cancel(true);
                                }
                                CoTaskMemFree(uri);
                                return S_OK;
                            }).Get(), &m_navigationStartingToken);

                        wvWnd3->add_WebMessageReceived(Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                            [this](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                                PWSTR message;

                                args->TryGetWebMessageAsString(&message);
                                // processMessage(&message);
                                if (wcscmp(message, L"OnPageFinished") == 0 || (wcscmp(message, L"OnPageBottom") == 0)) {
                                    /*if (wcscmp(message, L"OnPageFinished") == 0)
                                    AfxMessageBox("This message came from Javascript : OnPageFinished");
                                    else
                                        AfxMessageBox("This message came from Javascript : OnPageBottom");*/
                                    impDataSource->portionCount = 100;
                                    CString query = GetNextValues();

                                    if (!query.IsEmpty()) {
                                        BSTR myQuery = query.AllocSysString();
                                        webview->ExecuteScript(
                                            myQuery,
                                            nullptr);
                                    }

                                }
                                else
                                    if (wcscmp(message, L"GoToDown") == 0) {
                                        //AfxMessageBox("This message came from Javascript : GoToDown");
                                        CString query = GetNextValues();

                                        if (!query.IsEmpty()) {
                                            query.Append("isScroll = true;");
                                            query.Append("window.scrollTo(0, document.body.scrollHeight);");
                                            query.Append("window.chrome.webview.postMessage('GoToDown');");
                                            BSTR myQuery = query.AllocSysString();
                                            webview->ExecuteScript(
                                                myQuery,
                                                nullptr);
                                        }
                                        else {
                                            query.Append("isScroll = false; window.scrollTo(0, document.body.scrollHeight);");
                                            BSTR myQuery = query.AllocSysString();
                                            webview->ExecuteScript(
                                                myQuery,
                                                nullptr);
                                        }
                                    }
                                /*  else    if (wcscmp(message, L"AppendNew") == 0) {
                                    addNew();
                                    CString query;
                                    query = GetQueryAppend();
                                    BSTR myQuery = query.AllocSysString();
                                    webview->ExecuteScript(
                                        myQuery,
                                        nullptr);
                                }*/
                                    else    if (wcsstr(message, L"OnPageNext")) {

                                        std::wstring ws = message;
                                        std::wstring sub = ws.substr(11);
                                        impDataSource->portionCount = std::stoi(sub);

                                        webview->ExecuteScript(
                                            L" window.tableComponent.overlay = true;",
                                            nullptr);
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
                                            webview->ExecuteScript(
                                                query.AllocSysString(),
                                                nullptr);
                                        }
                                        else {
                                            query += "window.tableComponent.addData([]);";
                                            webview->ExecuteScript(
                                                query.AllocSysString(),
                                                nullptr);
                                        }

                                    }



                                webview->PostWebMessageAsString(message);
                                CoTaskMemFree(message);
                                return S_OK;
                            }).Get(), &m_navigationStartingToken);

                        webViewInit = true;
                        return S_OK;
                    }).Get());

                return S_OK;
            }).Get());




    // <-- WebView2 sample code ends here -->
}
