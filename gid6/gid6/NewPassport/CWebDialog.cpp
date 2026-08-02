#pragma once
#include "stdafx.h"
#include "CWebDialog.h"
#include "resource.h"
#include "wm_user.h"
#include "gidrview.h"
#include "crack.h"
#include <regex>
#include <nlohmann/json.hpp>
using nlohmann::json;

BEGIN_MESSAGE_MAP(CWebDialog, CDialog)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()
    ON_MESSAGE(WM_USER_ADO_UPDATED, OnAdoUpdated)
    ON_MESSAGE(WM_USER_ADO_NOUPDATED, OnAdoNoUpdated)
    ON_MESSAGE(WM_USER_REMONT_UTVERDIT, OnRemontUtverdit)
    ON_MESSAGE(WM_USER_OPRES_UTVERDIT, OnOpresUtverdit)
    ON_MESSAGE(WM_USER_TABLE_OPEN_FILE_DIALOG, OnOpenFileDialog)


END_MESSAGE_MAP()

CWebDialog::CWebDialog(CWnd* pParent, int type_of_net, const char* db, const char* connect, const char* tn, const char* q, int n_ret, VIRT_DATA typ, const char* caption, bool is_modal)
    : CDialog(IDD_TABLE_DLG, pParent)
{
    m_is_modal = is_modal;

    m_head.rows = 0;
    m_caption = tn;
    if (caption) {
        m_caption = caption;
    }
    /*
    m_n_ret = n_ret;
    m_strRet = "";
    m_n_keys = 0;
    m_idCol = -1;
    m_find = nullptr;
    */
    tableName = tn;
    mTypeOfNet = type_of_net;
    mConnect = connect;
    mDb = db;

    mSql = q;

    m_profile.Format("WebDialog\\%s", tn);

    //*************************new
    mAdo = getAdo(getGidAdoName());

    impDataSource = new WVAdoImpDataSource;
    impDataSource->m_q = q;

    impDataSource->setAllows(typ);
};

int IsFile(const TCHAR* fN);
string UTF8ToANSI(string s);
CWebDialog::~CWebDialog() {

}
CString getFileDialog(CWnd* wnd, CString defext, CString path, CString filter, CString profile, CString pattern);
BOOL CWebDialog::OnInitDialog()
{
    UpdateData(FALSE);

    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);

    if (mDataKey.size() > 0) {
        impDataSource->mDataKey = mDataKey;
        impDataSource->mDataValue = mDataValue;
    }

    if (impDataSource->OpenSQL(mSql, tableName, mConnect, mTypeOfNet) == UG_ERROR) {
        return false;
    }

    countAllRows = impDataSource->GetNumRowsComplete();

    impDataSource->SetTableName(mDb, tableName);

    int nf = impDataSource->countFields();

    countFields = nf;
    int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
    int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
    int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
    int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

    if (cx > 0 && cy > 0) {
        SetWindowPos(&wndTopMost, bx, by, cx, cy, SWP_NOZORDER);
    }
    SetWindowText(m_caption);

    //this->ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME, 0, 0);
    //this->ModifyStyle(WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME | WS_BORDER, 0, 0);
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
                                    HINSTANCE hi = ShellExecute(0, "open", p2, NULL, NULL, SW_SHOWNORMAL);
                                    //                                  HINSTANCE hi = ShellExecute(this->m_hWnd, "open", p2, NULL, NULL, SW_SHOWNORMAL);
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
                        

                        pickupFileField = field;
                        gCallbackPickupFile = callbackPickupFile;

                        /*int myColId = 5;
                        AdoFile* file = m_head.getFile(myColId);
                        if (file) {
                            CString path = AfxGetApp()->GetProfileString(szSection, file->s3, "");
                            CString p = getFileDialog(0, file->s1, path, file->s2, "AdoCUG", file->pattern);

                            if (p != "") {
                                if (p.Left(0) == "." && path != "") {
                                    CString s;
                                    s.Format("Файл должен находиться в каталоге %s", path);
                                    AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
                                    gCallbackPickupFile(L"", pickupFileField.AllocSysString(), 404, L"BAD");
                                }
                                else {

                                    gCallbackPickupFile(p.AllocSysString(), pickupFileField.AllocSysString(), 200, L"OK");
                                }
                            }

                        }*/
                        PostMessage(WM_USER_TABLE_OPEN_FILE_DIALOG, 0, 0);

                    },

                        //callback для метода утверждения
                        [this](INT id, BSTR jsonString, std::function<void(int, BSTR, BSTR)> callbackChange)
                    {
                        int code = utverdit(id, jsonString);
                    },
                        //callback для метода печати
                        [this](BSTR jsonString, int count) {
                    },
                    //callback для метода поиска 
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

                    });

                //add callback post message
                m_pWebBrowser->RegisterPostMessageCallback([this](PWSTR message) {
                    if (wcscmp(message, L"OnPageFinished") == 0 || (wcscmp(message, L"OnPageBottom") == 0)) {
                        /*if (wcscmp(message, L"OnPageFinished") == 0)
                        AfxMessageBox("This message came from Javascript : OnPageFinished");
                        else
                            AfxMessageBox("This message came from Javascript : OnPageBottom");*/



                        impDataSource->portionCount = 100;
                        CString query = GetNextValues();

                        //Взять первую порцию данных
                        if (!query.IsEmpty()) {
                            BSTR myQuery = query.AllocSysString();
                            m_pWebBrowser->ExecuteScript(
                                myQuery);
                        }

                        impDataSource->portionCount = countAllRows - 100;

                        //если данных больше одной порции взять остальное
                        if (impDataSource->portionCount > 0) {
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




                    }

                    else    if (wcsstr(message, L"OnSearch")) {  // Поиск

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
                        qq = SearchUtility::GetValuesByText(pat,dataStorage, dataStorageId);

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

                    else if (wcsstr(message, L"OnEdit")) {

                        int ado_view(CWnd* wnd, CAdoFile* ado, CString table_name, int id, CString title, bool readonly);


                        CAdoFile* ado = NULL;

                        std::wstring ws = message;
                        std::wstring sub = ws.substr(7);
                        int id = std::stoi(sub);

                        ado = getAdo(impDataSource->m_sDbName);

                        ado_view(this, ado, impDataSource->m_sTableName_main, id, m_caption, !impDataSource->allowEdit);
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
                    else    if (wcsstr(message, L"transferID")) {
                        // Факторы риска
                        std::wstring ws = message;
                        std::wstring sub = ws.substr(11);
                        void faktory_riska_truboprovoda_osmotr_ok(CWnd * wnd, std::wstring sub);
                        faktory_riska_truboprovoda_osmotr_ok(this, sub);
                        //                      OnOK();
                    }
                    else    if (wcsstr(message, L"okBtn")) {
                        if (!transIDMode) {
                            OnOK();
                        }
                    }
                    else    if (wcsstr(message, L"cancelBtn")) {
                        OnCancel();
                    }
                    });
                m_pWebBrowser->Navigate("https://appassets.example/index.html", nullptr);

            });
    }
    return true;
}

LRESULT CWebDialog::OnOpenFileDialog(WPARAM wParam, LPARAM lParam)
{
    int myColId = 5;

    AdoFile* file = m_head.getFile(myColId);
    if (file) {
        CString path = AfxGetApp()->GetProfileString(szSection, file->s3, "");
        CString p = getFileDialog(0, file->s1, path, file->s2, "AdoCUG", file->pattern);

        if (p != "") {
            if (p.Left(0) == "." && path != "") {
                CString s;
                s.Format("Файл должен находиться в каталоге %s", path);
                AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
                gCallbackPickupFile(L"", pickupFileField.AllocSysString(), 404, L"BAD");
            }
            else {

                gCallbackPickupFile(p.AllocSysString(), pickupFileField.AllocSysString(), 200, L"OK");
            }
        }

    }
    pickupFileField = "";
    gCallbackPickupFile = nullptr;
    return 0;

}

void CWebDialog::OnOK()
{
    CDialog::OnOK();
    OnCancel();
}

BOOL CWebDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT nID = LOWORD(wParam);

    if (nID == IDC_EDIT) {
    }

    return CDialog::OnCommand(wParam, lParam);
}


CString findTableRusName(CString d, CString n);
CString findTableEngName(CString d, CString n);

void faktory_riska_truboprovoda_osmotr_cancel(WPARAM wParam, LPARAM lParam);
void faktory_riska_truboprovoda_osmotr_ok2(WPARAM wParam, LPARAM lParam);

// Добавил 2022-11-10 Цок

LRESULT CWebDialog::OnAdoNoUpdated(WPARAM wParam, LPARAM lParam)
{
    faktory_riska_truboprovoda_osmotr_cancel(wParam, lParam);

    return 0;
}

#include "PropertyGrid/test/PropGridDlg.h"


LRESULT CWebDialog::OnAdoUpdated(WPARAM wParam, LPARAM lParam)
{
    impDataSource->ClearCurrentConnection();

    initDataSource = false;
    countRows = 0;
    currentCount = 0;
    currentDataStorageIndex = 0;
    dataStorage.clear();
    dataStorageId.clear();

    if (impDataSource->OpenSQL(mSql, tableName, mConnect, mTypeOfNet) == UG_ERROR) {
        return false;
    }

    countAllRows = impDataSource->GetNumRowsComplete();
    int nf = impDataSource->countFields();
    countFields = nf;

    m_pWebBrowser->Reload();

    CPropertyParam* param = (CPropertyParam*)lParam;
    if (param) {
        //        if (param->table == "faktory_riska_truboprovoda" && param->typ == TP_FAKTORY_AUTODELETE) {
        if (param->table == "faktory_riska_truboprovoda") {
            faktory_riska_truboprovoda_osmotr_ok2(wParam, lParam);
        }
    }


    return 0;
}

int CWebDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

void CWebDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    CRect rectClient;

    if (m_pWebBrowser != nullptr)
    {
        //m_staticBrowser.GetClientRect(rectClient);
        //m_staticBrowser.ClientToScreen(rectClient);
        //ScreenToClient(rectClient);
        GetClientRect(&rectClient);
        m_pWebBrowser->Resize(rectClient.Width(), rectClient.Height());
    }
}

void CWebDialog::OnSetFocus(CWnd* pOldWnd)
{
    CDialog::OnSetFocus(pOldWnd);

}

BOOL CWebDialog::OnEraseBkgnd(CDC* pDC)
{

    return CDialog::OnEraseBkgnd(pDC);
}


void setView(CView* pView0);

void CWebDialog::OnDestroy()
{
    WINDOWPLACEMENT wpi;
    GetWindowPlacement(&wpi);

    AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right - wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom - wpi.rcNormalPosition.top);

    m_pWebBrowser.reset();

    CDialog::OnDestroy();

    if (!m_is_modal) delete this;
}

void CWebDialog::OnCancel() {

    CDialog::OnCancel();
    if (!m_is_modal)
        OnDestroy();
}

double to_double(COleVariant& var);


void CWebDialog::addDefaultKey(CString key, int value) {
    mDataKey.push_back(key);
    mDataValue.push_back(value);
}
void CWebDialog::addDefaultKeys(vector<CString> keys, vector<int> values) {
    mDataKey = keys;
    mDataValue = values;
}


#if 0
void CWebDialog::setHead(const char* db, const char* name)
{
    if (readTable(mDb, name, m_head)) {
    }
}
#endif

void CWebDialog::setTransIDMode(bool value) {
    transIDMode = value;
};

void CWebDialog::setUtverditMode(bool value) {
    utverditMode = value;
};


void CWebDialog::setColorMode(CString colorKey1) {
    colorMode = true;
    colorKey = colorKey1;
};

int getIndex(vector<CString> v, CString K)
{
    auto it = find(v.begin(), v.end(), K);

    if (it != v.end())
    {
        int index = it - v.begin();
        return index;
    }
    else {

        return -1;
    }
}

CString CWebDialog::GetHeaderTable() {
    //  BOOL readOnly = ;
    BOOL editeMode = impDataSource->allowEdit;
    BOOL addMode = impDataSource->allowAdd;
    BOOL deleteMode = impDataSource->allowDelete;

    // Добавил 2022-11-15 Цок
    BOOL readOnly = !editeMode && !addMode;

    BOOL setLiteSearch = TRUE;

    //  if (impDataSource->allowAdd && impDataSource->allowEdit && impDataSource->allowDelete)
    //      readOnly = false;

    //    if (!impDataSource->allowDelete)
    //      deleteMode = false;

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
    case virt_data_geo_edit:
    case virt_data_geo_del:
    case virt_data_tu:
        isMoveTo = true;
        break;
    }

    CString gg = "";

    if (m_setGrouping != "") {
        gg.Format(" window.MainComponent.$store.commit(\"setGrouping\",\"%s\");", m_setGrouping);
    }
    CString totalPage;
    CString query, rusTableName;
    rusTableName = findTableRusName(impDataSource->m_sDbName, impDataSource->m_sTableName);

    //    if (pDoc->m_view_caption != pDoc->m_view_tn) {
    //        rusTableName = pDoc->m_view_caption;
    //    }


    totalPage.Format("window.MainComponent.$store.commit(\"setLiteMode\",true);"
        " window.MainComponent.$store.commit(\"setTableHeader\",\"%s\");"
        " window.MainComponent.$store.commit(\"setTotalPage\",%d);"

        // Добавил 2022-11-15 Цок
        " window.MainComponent.$store.commit(\"setReadonly\",%s);"

        " window.MainComponent.$store.commit(\"setEditMode\",%s);"
        " window.MainComponent.$store.commit(\"setAddMode\",%s);"
        " window.MainComponent.$store.commit(\"setDeleteMode\",%s);"



        " window.MainComponent.$store.commit(\"setMoveMode\",%s);"
        " window.MainComponent.$store.commit(\"setTechnicalCondition\",%s);"
        " window.MainComponent.$store.commit(\"setTransIDMode\",%s);"

        //
        " window.MainComponent.$store.commit(\"setUtverdit\", %s);"
        //                     " window.MainComponent.$store.commit(\"setUtverditKey\",\"col_6\");"
        " window.MainComponent.$store.commit(\"setUtverditKey\",\"%s\");"


        "%s\n"
        //                     " window.MainComponent.$store.commit(\"setGrouping\",\"%s\");"

        " window.MainComponent.$store.commit(\"setLiteSearch\", %s);"

        ,
        rusTableName,
        countAllRows,

        // Добавил 2022-11-15 Цок
        readOnly ? "true" : "false",
        editeMode ? "true" : "false",
        addMode ? "true" : "false",
        deleteMode ? "true" : "false",


        isMoveTo ? "true" : "false",
        isTechnicalCondition ? "true" : "false",
        transIDMode ? "true" : "false",
        utverditMode ? "true" : "false",

        m_setUtverditKey,

        gg,


        setLiteSearch ? "true" : "false"
    );


    CString colorModeString;
    if (colorMode) {
        colorModeString.Format("window.MainComponent.$store.commit(\"setColorMode\",true);"
            " window.MainComponent.$store.commit(\"setColorKey\",\"%s\");"
            // Добавил Цок ГН  20221111
            " window.MainComponent.$store.commit(\"setColorTrueValues\", [\"выполнено\"]);"


            , colorKey);



        totalPage += colorModeString;
    }
    query += totalPage + " window.MainComponent.$store.commit(\"setHeaderList\", [";
    for (long i = 0; i < countFields; i++) {
        vField.push_back(impDataSource->nameField(i));
    }
    if (m_head.width.size() > 0)
        for (long i = 0; i < countFields; i++) {
            //set<int>::const_iterator it = m_head.is_edit.find(i);
            if (getIndex(mDataKey, vField[i]) != -1) {
                impDataSource->positionS[i] = -1;
            }

            else if (m_head.getLookup(i) == NULL || i == 0) {
                impDataSource->positionS.insert(std::make_pair(i, i));
            }
            else {
                impDataSource->positionS.insert(std::make_pair(i, i - 1));
                impDataSource->positionS[i - 1] = -1;

            }

        }
    else if ((regex_match(tableName.GetString(), regex("Тип (арматуры|назначения|конструкции|конструкции камеры|исполнения|месторасположения|материала|характеристики)"))))
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
            case adDBTimeStamp:
            case adDate:
            case adDBDate:
                varType = "date";
                break;
            }

            if (strcmp(fName, "Здание") == 0)
                varType = "zdanie";

            if (m_head.m_file.find(i) != m_head.m_file.end()) {
                varType = "file";
            }
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

CString CWebDialog::GetNextValues() {

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
            query += "window.MainComponent.$store.commit(\"setDataList\", [";
        }

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

    if (hasNextData) {

        query += GetQueryAppend();
    }
    else
        query += "";

    if (firstTime)
        query += "]);";

    return query;
}


CString CWebDialog::GetQueryAppend() {
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

CString CWebDialog::ObtainDataToStorage() {
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
                       
                        tdQuery.Format(_T("%s\"col_%d\" : \"\""), comma2, i);
                        break;
                    case 2:
                    case 3:
                        
                        tdQuery.Format(_T("%s\"col_%d\" : %d"), comma2, i, v.lVal);

                        break;
                    case 4:
                    case 5:
                        d = impDataSource->to_double(v);
                       
                        tdQuery.Format(_T("%s\"col_%d\" : %f"), comma2, i, d);
                        break;
                    case 7:
                    case 8:
                        s = CCrack::strVARIANT_UTF(v);
                        
                        s.Replace("\n", "");
                        s.Replace("\r", "");
                        s.Replace("\\", "\\\\");
                        s.Replace("\"", "\\\"");
                        
                        tdQuery.Format(_T("%s\"col_%d\" : \"%s\""), comma2, i, s);

                        break;
                    default:
                        
                        tdQuery.Format(_T("%s\"col_%d\" : \"?\""), comma2, i);
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

CString getFileDialog(CWnd* wnd, CString defext, CString path, CString filter, CString profile, CString pattern);
void CWebDialog::OnMoveTo(int id) // перейти в окно схемы
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
        case virt_data_geo_edit:
            //            pView->SendMessage(WM_USER_FIND_GEO_EDIT, (WPARAM)id, (LPARAM)(LPCSTR)impDataSource->m_sTableName);

/*
            if (impDataSource->m_sTableName == "faktory_riska_truboprovoda") {
                std::wstring s = std::to_wstring(id);
                void faktory_riska_truboprovoda_osmotr_ok(CWnd * wnd, std::wstring sub);
                faktory_riska_truboprovoda_osmotr_ok(this, s);
            }
            else {
                pView->geoEdit(this, (LPCSTR)impDataSource->m_sTableName, id);
            }

            break;
*/
        case virt_data_geo:
        case virt_data_geo_del:
//        case virt_data_geo_edit:
            pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM)(LPCSTR)impDataSource->m_sTableName);
            break;
        case virt_data_geo_big_line:
            pView->SendMessage(WM_USER_FIND_GEO_BIG_LINE, (WPARAM)id, (LPARAM)(LPCSTR)impDataSource->m_sTableName);
            break;
        }
    }
}


int CWebDialog::addNew(BSTR str)
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

int CWebDialog::updateData(int id, BSTR str)
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


void remont_utverdit(CWnd* wnd, int id);
bool remont_utverdit_ok(CWnd* wnd, int id);
void opres_utverdit(CWnd* wnd, int id);
bool opres_utverdit_ok(CWnd* wnd, int id);



int CWebDialog::utverdit(int id, BSTR str)
{
    std::wstring ws(str, SysStringLen(str));

    m_code = id;
    m_str = str;

    if (this->tableName == "remont2") {
        remont_utverdit(this, id);
    }
    else if (this->tableName == "opres") {
        opres_utverdit(this, id);
    }

    return 200;
}

LRESULT CWebDialog::OnRemontUtverdit(WPARAM wParam, LPARAM)
{
    remont_utverdit_ok(this, m_code);

    impDataSource->ClearCurrentConnection();

    initDataSource = false;
    countRows = 0;
    currentCount = 0;

    if (impDataSource->OpenSQL(mSql, tableName, mConnect, mTypeOfNet) == UG_ERROR) {
        return false;
    }

    countAllRows = impDataSource->GetNumRowsComplete();
    int nf = impDataSource->countFields();
    countFields = nf;

    m_pWebBrowser->Reload();

    return 0L;
}

LRESULT CWebDialog::OnOpresUtverdit(WPARAM wParam, LPARAM)
{
    opres_utverdit_ok(this, m_code);

    impDataSource->ClearCurrentConnection();

    initDataSource = false;
    countRows = 0;
    currentCount = 0;

    if (impDataSource->OpenSQL(mSql, tableName, mConnect, mTypeOfNet) == UG_ERROR) {
        return false;
    }

    countAllRows = impDataSource->GetNumRowsComplete();
    int nf = impDataSource->countFields();
    countFields = nf;

    m_pWebBrowser->Reload();

    return 0L;
}


