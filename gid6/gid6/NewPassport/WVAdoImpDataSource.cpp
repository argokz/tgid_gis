#include "stdafx.h"
#include "WVAdoImpDataSource.h"
#include <nlohmann/json.hpp>
#include <codecvt>
#include <locale>
#include "excel.h"
using nlohmann::json;
#include "css.h"
#include "crack.h"
#include "resource.h"
#include "InputDia.h"


CString getTableNameFromSQL(CString q);

void print_index1(FILE* f, const char* title);
void print_index2(FILE* f);
FILE* print_doc_with_path(FILE* fi, const char* fn, const char* title, void (*filtr) (CString& s1) = NULL);
void print_doc_end(FILE* f);

WVAdoImpDataSource::WVAdoImpDataSource()
{
    m_pConnection = NULL; // the connection
    m_pRecordSet = NULL;  // the recordset

    m_type_of_net = 0;

    //m_colorCol = -1;

    //m_n_keys = 0;

    //m_error = "";

    //m_head = NULL;

    m_sDbName = _T(""); //
    m_sTableName = _T(""); // 

    m_q = _T("");

    m_dbConnection = _T("");// source string for the connection
    m_User = _T("");    // user login name
    m_Password = _T("");  // user login password
    m_Command = _T("");   // command to execute
    m_Filter = _T("");    // filter to be used
    m_SortOrder = _T(""); // sort order to be used
    //m_sCurRowID = _T("»");

    m_showAll = 0;      // Показывать все строки

  // m_numCols = -1;     // indicate that the number of columns is unknown

   // fetching
    m_nFetchedStarting = 0;
    m_pcRowsObtained = 0;
    //  m_nMinFetchRows = MIN_FETCH_ROWS;

  //  m_bAllowDelete = TRUE;

    // editing
  //  m_bAllowEdits = TRUE;
  //  m_bEditingConfirmed = FALSE;

    // flag indicating that we can use GhostRow
  //  m_bUseGhostRow = TRUE;
  //  m_bUseGhostRow = FALSE;
 //   m_nCurGhostRow = -1;
 //   m_nOldGhostRow = -1;

    // AppendRow style
  //  m_bUseSimpleNew = TRUE;
  //  m_bUseSimpleNew = FALSE;   // изучить что тут нужнно поставить !!!!!!!!!!!!!

    // number of rows waiting update
  //  m_iUpdatesPending = 0;
    m_sTableName_main = "";

    // m_idCol = -1;

    // m_rows_all = -1;

     // Initialize OLE
    CoInitialize(NULL);
}

WVAdoImpDataSource::~WVAdoImpDataSource()
{
    // make sure that all changes are saved before exiting
   // if (UpdatePending(FALSE) == UG_ERROR)
        // clear our the updated cells list
     //   m_hUpdatedRow.Empty();

    // close recordset and connection
    Close();

    CoUninitialize();
}

int WVAdoImpDataSource::Open(LPCTSTR name, LPCTSTR option)
{
    OutputDebugStringA("***WVAdoImpDataSource Open \n");
    UNREFERENCED_PARAMETER(option);

    if (name)
        m_dbConnection = name;

    // make sure that the connection string has been specified
    if (m_dbConnection == _T(""))
        // there is nothing to open
        return UG_ERROR;

    // make sure that the recordset and connection objects are closed
    Close();

    // open the connection
    if (OpenConnection() == UG_ERROR)
        return UG_ERROR;

    // open the recordset
    return OpenRecordset();
}

int WVAdoImpDataSource::OpenSQL(CString sql, CString table_name, CString conStr, int type_of_net)
{
    OutputDebugStringA("***CADOImpDatasource OpenSQL1 \n");
    m_type_of_net = type_of_net;

    // Clear information on the last query.
    m_Command = "";
    m_Filter = "";
    m_SortOrder = "";

    m_q = sql;

    if (type_of_net == TYPE_OF_NET_MYSQL) {
        m_q.Replace("[", "`");
        m_q.Replace("]", "`");
        m_q.Replace(".dbo.", ".");
    }

    if (type_of_net == TYPE_OF_NET_MSSQL) {
        m_q.Replace(" IF(", " IIF(");
        m_q.Replace(",IF(", ",IIF(");
    }

    m_sTableName_main = getTableNameFromSQL(m_q);
    m_sTableName_main = table_name;

    //  m_q = "select id FROM opora";

//    CInputDialog inD(AfxGetMainWnd(), "", "?", m_q);
//    inD.DoModal();



      // copy connection string and the SQL statement
    if (ParseSQLString(m_q) == UG_ERROR)
        return UG_ERROR;

    return Open(conStr, NULL);
}

BOOL WVAdoImpDataSource::IsOpen()
{
    if (m_pRecordSet)
        if (m_pRecordSet->GetState() == adStateOpen)
            return TRUE;

    return FALSE;
}

/***************************************************
Close
  function is used to properly close instances of the recordset
  and connection objects.
Params:
  <none>
Return:
  UG_SUCCESS on success, this function will never fail.
****************************************************/
int WVAdoImpDataSource::Close()
{
    // if a recordset is open, then close it and release pointer
    if (m_pRecordSet)
    {
        try
        {
            m_pRecordSet->CancelUpdate();
        }
        catch (...)
        {
        }

        m_nFetchedStarting = 0;
        m_pcRowsObtained = 0;
        // m_hUpdatedRow.Empty();
        // m_nCurGhostRow = -1;
       //  m_nOldGhostRow = -1;
        m_curBookmark.Clear();
        // number of rows waiting update
     //   m_iUpdatesPending = 0;

        if (m_pRecordSet->GetState() == adStateOpen)
            m_pRecordSet->Close();

        m_pRecordSet.Release();
        m_pRecordSet = NULL;
    }

    // if connectoin is established, then disconnect and release pointer
    if (m_pConnection)
    {
        m_pConnection->Close();
        m_pConnection.Release();
        m_pConnection = NULL;
    }

    return UG_SUCCESS;
}

int WVAdoImpDataSource::OpenConnection()
{
    if (m_pConnection == NULL)
    {
        try
        { // create instance of the Connection object
            m_pConnection.CreateInstance(__uuidof(Connection));

            // open the connection

            m_pConnection->PutCursorLocation(adUseClient);
            m_pConnection->Open((_bstr_t)m_dbConnection, (_bstr_t)m_User, (_bstr_t)m_Password, -1);
        }
        catch (_com_error /*&e*/)
        {
            m_pConnection = NULL;
            return UG_ERROR;
        }
    }

    // provide the CUGCtrl class with information on the connection that
    // was successfuly created and open, allow user with ability to perform
    // further customization on the recordset.
    //if (m_ctrl != NULL)
     //   m_ctrl->OnDataSourceNotify(GetID(), UG_ADO_DS_OPEN_CONN, (long)m_pConnection);

    return UG_SUCCESS;
}

int WVAdoImpDataSource::OpenRecordset()
{
    CString sqlString = GetSQL();

    try // open the recordset
    {
        m_pRecordSet.CreateInstance(__uuidof(Recordset));
        m_pRecordSet->Open((_bstr_t)sqlString, _variant_t((IDispatch*)m_pConnection), adOpenKeyset, adLockOptimistic, adCmdUnknown);

        // if (m_ctrl != NULL)
             // provide the CUGCtrl class with information on the recordset that
             // was successfuly created and open, allows user with ability to perform
             // further processing on the recordset.
           //  m_ctrl->OnDataSourceNotify(GetID(), UG_ADO_DS_OPEN_RS, (long)m_pRecordSet);

        if (m_pRecordSet->GetRecordCount())
            m_pRecordSet->MoveFirst();
    }
    catch (_com_error& e)
    {
        if (1) {
            AfxMessageBox(e.Description(), MB_OK | MB_ICONINFORMATION);
        }
        return UG_ERROR;
    }

    return UG_SUCCESS;
}

CString WVAdoImpDataSource::GetSQL()
{
    if (m_Command == _T(""))
        return _T("");

    // construct the SQL statement to use
    CString sqlString = m_Command;
    if (m_Filter != _T(""))
        sqlString += _T(" WHERE ") + m_Filter;
    if (m_SortOrder != _T(""))
        sqlString += _T(" ORDER BY ") + m_SortOrder;

    sqlString += _T(";");

    if (m_showAll) {
        sqlString.Replace("AND n1.internalNodeID IS NULL", "");
        sqlString.Replace("AND n.internalNodeID IS NULL", "");
    }


    return sqlString;
}


int WVAdoImpDataSource::ParseSQLString(CString sql)
{
    if (sql == "")
        return UG_ERROR;

    CString tempSql(sql);
    tempSql.TrimRight();
    // convert the string to upper case, to make it easier to search
    tempSql.MakeUpper();
    // remove the ';' character from the end of the SQL statement
    tempSql = tempSql.SpanExcluding(_T(";"));

    // look for the WHERE sub string in the SQL statement
    // the position found indicates end of the selection
    // portion of the SQL statement.
    int filterIndex = tempSql.Find(_T("WHERE"), 0);
    // check if the sort order has been specified
    int orderIndex = tempSql.Find(_T("ORDER BY"), 0);

    if (filterIndex >= 0)
    {
        SetCommand(sql.Left(filterIndex - 1));
        sql = sql.Right(sql.GetLength() - (filterIndex + 6));

        if (orderIndex >= 0)
        {
            orderIndex -= (filterIndex + 6);
            SetFilter(sql.Left(orderIndex - 1));
            sql = sql.Right(sql.GetLength() - (orderIndex + 9));
            SetSort(sql);
        }
        else
            SetFilter(sql);
    }
    else if (orderIndex >= 0)
    {
        SetCommand(sql.Left(orderIndex - 1));
        sql = sql.Right(sql.GetLength() - (orderIndex + 9));

        SetSort(sql);
    }
    else
        SetCommand(sql);

    return UG_SUCCESS;
}


/***************************************************
SetCommand
  function is used to set the select portion of the SQL statement
  that will be used to open the recordset.
Params:
  <none>
Return:
  UG_SUCCESS  success, this function will never fail.
****************************************************/
int WVAdoImpDataSource::SetCommand(CString command)
{
    m_Command = command.SpanExcluding(_T(";"));
    return UG_SUCCESS;
}

/***************************************************
SetFilter
  function is used to set the filter criteria for the recordset
  that is currently opened. WHERE portion of the SQL string,
  but whithout WHERE word.
Params:
  <none>
Return:
  UG_SUCCESS  success, this function will never fail.
****************************************************/
int WVAdoImpDataSource::SetFilter(CString filter)
{
    m_Filter = filter.SpanExcluding(_T(";"));
    return UG_SUCCESS;
}

/***************************************************
SetSort
  function is used to set the sort criteria for the recordset
  that is currently opened. ORDER BY portion of the SQL string,
  but whithout ORDER BY word.
Params:
  sort    - string representing the sort criteria
Return:
  UG_SUCCESS  success, this function will never fail.
****************************************************/
int WVAdoImpDataSource::SetSort(CString sort)
{
    m_SortOrder = sort.SpanExcluding(_T(";"));
    return UG_SUCCESS;
}

void WVAdoImpDataSource::showAllInit(bool all)
{
    m_showAll = all;
}



void WVAdoImpDataSource::SetTableName(CString db, CString tn)
{

    m_sDbName = db;
    m_sTableName = tn;

    /*
      CAdoFile *ado = getAdo(m_sDbName);
      if (ado) {
          m_rows_all = getCountTable2(ado, m_q);
      }
    */
}

long WVAdoImpDataSource::GetNumRowsComplete()
{
    // make sure the recordset is open
    if (m_pRecordSet == NULL)
        return -1;

    // The RecordCount ADO property will return exact
    // number of records found in the recordset if recordset
    // Supports (adApproxPosition) or Supports (adBookmark).
    return m_pRecordSet->RecordCount;
}

int WVAdoImpDataSource::countFields()
{
    int m_numCols = -1;
    if (m_pRecordSet)
    {
        try
        {
            m_numCols = (int)m_pRecordSet->GetFields()->GetCount();
        }
        catch (_com_error /*&e*/)
        { // although the GetCount function returns -1 on error, we
          // set m_numCols = -1 just to make sure.
            m_numCols = -1;
        }
    }
    else
        m_numCols = -1;

    return m_numCols;
}
CString bstr2a(_bstr_t b);
CString WVAdoImpDataSource::nameField(int index)
{
    CString name = "";
    if (m_pRecordSet)
    {
        try
        {
            VARIANT vIntegerType;
            vIntegerType.vt = VT_I2;
            vIntegerType.intVal = index;
            FieldsPtr      pFields;
            FieldPtr       pField;

            m_pRecordSet->get_Fields(&pFields);
            pFields->get_Item(vIntegerType, &pField);
            _bstr_t s = pField->GetName();

            name = (LPCSTR)s;
        }
        catch (_com_error /*&e*/)
        { // although the GetCount function returns -1 on error, we
          // set m_numCols = -1 just to make sure.
            name = "";
        }
    }
    else
        name = "";

    return name;

}
DataTypeEnum WVAdoImpDataSource::fieldVariant(int index)
{

    if (m_pRecordSet)
    {
        try
        {
            VARIANT vIntegerType;
            vIntegerType.vt = VT_I2;
            vIntegerType.intVal = index;
            FieldsPtr      pFields;
            FieldPtr       pField;

            m_pRecordSet->get_Fields(&pFields);
            pFields->get_Item(vIntegerType, &pField);
            return pField->GetType();

        }
        catch (_com_error /*&e*/)
        {
            // although the GetCount function returns -1 on error, we

        }
    }


    return DataTypeEnum();

}
void WVAdoImpDataSource::ClearCurrentConnection() {

    m_nFetchedStarting = 0;
    m_pcRowsObtained = 0;
    m_pConnection = NULL;
    m_pRecordSet = NULL;
};
int WVAdoImpDataSource::FetchNewRows(long row)
{
    OutputDebugStringA("WVAdoImpDataSource FetchNewRows\n");
    long nStartingDelta = m_nFetchedStarting;

    // find out how many rows need to be fetched
    m_nMaxFetchRows = 100;


    // reset starting row for the fetch rows buffer
    m_nFetchedStarting = row;

    try
    {
        if (m_curBookmark.vt == VT_EMPTY || m_nFetchedStarting <= 0)
        { // if the book mark does not exist then adjust
          // the starting position to the beggining of the dataset
            nStartingDelta = 0;
            m_nFetchedStarting = 0;
            m_pRecordSet->MoveFirst();
        }
        else
        { // adjust starting position in relation to current position
            nStartingDelta = m_nFetchedStarting - nStartingDelta;

            // move current record in the recordset to point to new fetch
            // starting position.
            m_pRecordSet->PutBookmark(m_curBookmark);
            // move current row as calculated
            m_pRecordSet->Move(nStartingDelta);
        }

        // store book mark to first row in the fetch array
        m_curBookmark = m_pRecordSet->GetBookmark();
        // fetch rows
        m_curRecords = m_pRecordSet->GetRows(m_nMaxFetchRows);
        // retrieve information on howmany rows were fetched
        if (m_curRecords.vt != VT_EMPTY)
            SafeArrayGetUBound(m_curRecords.parray, 2, &m_pcRowsObtained);
        else
            m_pcRowsObtained = 0;
    }
    catch (_com_error& e) {
        // Display errors, if any. Pass a connection pointer accessed from the Connection.
        //        PrintProviderError(pConnection);
        //        PrintComError(e);
        //        ret = false;

        _bstr_t bstrSource(e.Source());
        _bstr_t bstrDescription(e.Description());

        CString err;
        err.Format("Code = %08lx\nCode meaning = %s\nSourcre = %s\ntDescription = %s", e.Error(), e.ErrorMessage(), bstr2a(bstrSource), bstr2a(bstrDescription));
        //        AfxMessageBox(err, MB_OK|MB_ICONINFORMATION);

        m_curBookmark.Clear();
        m_pcRowsObtained = 0;
        return UG_ERROR;
    }
    catch (...)
    {
        m_curBookmark.Clear();
        m_pcRowsObtained = 0;
        return UG_ERROR;
    }

    m_pcRowsObtained += 1;

    return UG_SUCCESS;
}

string UTF8ToANSI(string s)
{
    BSTR    bstrWide;
    char* pszAnsi;
    int     nLength;
    const char* pszCode = s.c_str();

    nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, NULL, NULL);
    bstrWide = SysAllocStringLen(NULL, nLength);

    MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

    nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
    pszAnsi = new char[nLength];

    WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
    SysFreeString(bstrWide);

    string r(pszAnsi);
    delete[] pszAnsi;
    return r;
}



int WVAdoImpDataSource::addNewOld(int row, map<int, std::any> mapParams) {

    try {
        if (positionById.size() > 0)
            m_pRecordSet->MoveLast();
        m_pRecordSet->AddNew();



        map<int, std::any>::iterator it = mapParams.begin();
        while (it != mapParams.end())
        {
            _variant_t tempVar, dbVariant;
            tempVar.vt = VT_I4;
            long myPosition = positionS[it->first];
            if (myPosition == -2)
                myPosition = 3;
            tempVar.lVal = myPosition;

            json* myJson = std::any_cast<json>(&(it->second));
            if (myJson->is_string()) {
                string w_value = myJson->get<std::string>();
                string ansi = UTF8ToANSI(w_value.c_str());
                CString ansiString = ansi.c_str();

                DataTypeEnum fieldN = fieldVariant(myPosition);
                if (w_value.empty() && fieldN != adVarWChar && fieldN != adWChar && fieldN != adVarChar && fieldN != adLongVarWChar && fieldN != adLongVarChar && fieldN != adChar)
                {
                    dbVariant.vt = VT_NULL;
                }
                else {
                    dbVariant.bstrVal = ansiString.AllocSysString();
                    dbVariant.vt = VT_BSTR;
                }

                m_pRecordSet->PutCollect(tempVar, dbVariant);
            }
            else if (myJson->is_number_integer()) {
                dbVariant.intVal = myJson->get<int>();
                dbVariant.vt = VT_I4;
                m_pRecordSet->PutCollect(tempVar, dbVariant);
            }
            else if (myJson->is_number_float()) {
                dbVariant.dblVal = myJson->get<double>();
                dbVariant.vt = VT_R8;
                m_pRecordSet->PutCollect(tempVar, dbVariant);
            }
            else {
            }
            it++;
        }
        for (int i = 0; i < mDataKey.size(); i++) {

            _variant_t tempVar, dbVariant;
            tempVar.vt = VT_BSTR;
            tempVar.bstrVal = mDataKey[i].AllocSysString();
            dbVariant.intVal = mDataValue[i];
            dbVariant.vt = VT_I4;
            m_pRecordSet->PutCollect(tempVar, dbVariant);
        }
        if (mapParams.size() > 0 || mDataKey.size() > 0)
        {
            m_pRecordSet->Update();

            VARIANT vIntegerType;
            vIntegerType.vt = VT_I2;
            vIntegerType.intVal = 0;
            FieldsPtr      pFields;
            FieldPtr       pField;

            m_pRecordSet->get_Fields(&pFields);
            pFields->get_Item(vIntegerType, &pField);
            int id = pField->GetValue();
            return id;
        }
    }
    catch (_com_error& e)
    {
        //      AfxMessageBox(e.Description(), MB_OK|MB_ICONINFORMATION);


        ///      m_pRecordSet->CancelUpdate();
        return -1;
    }

    return -1;
    //FetchNewRows(row);
}


int WVAdoImpDataSource::addNew(int row, map<int, std::any> mapParams) {

    try {
        if (positionById.size() > 0)
            m_pRecordSet->MoveLast();
        m_pRecordSet->AddNew();


        FieldsPtr pFields = m_pRecordSet->Fields;
        map<int, std::any>::iterator it = mapParams.begin();
        if (it != mapParams.end())
        {


            VARIANT tempVar, dbVariant;
            tempVar.vt = VT_I4;
            long myPosition = positionS[it->first];
            if (myPosition == -2)
                myPosition = 3;
            tempVar.lVal = myPosition;

            DataTypeEnum fieldN = fieldVariant(myPosition);

            switch (fieldN) {
            case adTinyInt:
            case adSmallInt:
            case adInteger:
            case adBigInt:
                dbVariant.vt = VT_NULL;
                m_pRecordSet->PutCollect(tempVar, dbVariant);
                break;
            case adSingle:
            case adDouble:
            case adCurrency:
            case adDecimal:
                dbVariant.vt = VT_NULL;
                m_pRecordSet->PutCollect(tempVar, dbVariant);
                break;
            case adBoolean:
                dbVariant.vt = VT_NULL;
                m_pRecordSet->PutCollect(tempVar, dbVariant);
                break;
            case adVarChar:
            case adLongVarChar:
            case adVarWChar:
            case adLongVarWChar:
            case adBSTR:
            case adChar:
            case adWChar:
                dbVariant.bstrVal = L"";
                dbVariant.vt = VT_BSTR;
                m_pRecordSet->PutCollect(tempVar, dbVariant);
                break;
            case adDate:
            case adDBDate:
            case adDBTime:
            case adDBTimeStamp:
                dbVariant.vt = VT_NULL;
                m_pRecordSet->PutCollect(tempVar, dbVariant);
                break;
            default:
                // Неизвестный тип данных, не устанавливаем значение
                break;
            }
        }
        for (int i = 0; i < mDataKey.size(); i++) {

            VARIANT tempVar, dbVariant;
            tempVar.vt = VT_BSTR;
            tempVar.bstrVal = mDataKey[i].AllocSysString();
            dbVariant.intVal = mDataValue[i];
            dbVariant.vt = VT_I4;
            m_pRecordSet->PutCollect(tempVar, dbVariant);
        }
        if (mapParams.size() > 0 || mDataKey.size() > 0)
        {
            m_pRecordSet->Update();

            VARIANT vIntegerType;
            vIntegerType.vt = VT_I2;
            vIntegerType.intVal = 0;
            FieldsPtr      pFields;
            FieldPtr       pField;

            m_pRecordSet->get_Fields(&pFields);
            pFields->get_Item(vIntegerType, &pField);
            int id = pField->GetValue();
            return id;
        }

    }
    catch (_com_error& e)
    {
        AfxMessageBox(e.Description(), MB_OK | MB_ICONINFORMATION);


        ///      m_pRecordSet->CancelUpdate();
        return -1;
    }

    return -1;
    //FetchNewRows(row);
}

void WVAdoImpDataSource::OnWord(BSTR fileName) {
    try {
        // Convert BSTR to CString
        CString wordFileName(fileName);

        // Check if the file exists
        DWORD dwAttrib = GetFileAttributes(wordFileName);
        if (dwAttrib == INVALID_FILE_ATTRIBUTES) {
            // File doesn't exist
            AfxMessageBox(_T("Word file not found!"), MB_OK | MB_ICONERROR);
            return;
        }

        // Launch Word with the specified file
        HINSTANCE hResult = ShellExecute(NULL, _T("open"), _T("winword.exe"), "\"" + wordFileName + "\"", NULL, SW_SHOWNORMAL);

        // Check if the operation was successful
        if ((int)hResult <= 32) {
            // Handle error
            CString errorMsg;
            switch ((int)hResult) {
            case ERROR_FILE_NOT_FOUND:
                errorMsg = _T("Microsoft Word not found!");
                break;
            case ERROR_PATH_NOT_FOUND:
                errorMsg = _T("Path to Word file not found!");
                break;
            default:
                errorMsg.Format(_T("Error opening Word file. Error code: %d"), (int)hResult);
                break;
            }
            AfxMessageBox(errorMsg, MB_OK | MB_ICONERROR);
        }
    }
    catch (...) {
        AfxMessageBox(_T("Error opening Word file!"), MB_OK | MB_ICONERROR);
    }
}

void WVAdoImpDataSource::OnExcel(BSTR jsonStr) {
    CExcel ex;
    ex.open(NULL);
    if (ex.IsOpen()) {
        std::wstring ws(jsonStr, SysStringLen(jsonStr));

        auto myJson = json::parse(ws);

        vector<std::pair<CString, CString>> myHeaders;
        json jsonHeaders = myJson["headers"];
        json jsonData = myJson["data"];
        int countFields = 0;
        for (auto it = jsonHeaders.begin(); it != jsonHeaders.end(); ++it)
        {
            auto arrElement = it.value();
            auto colName = arrElement["field"];
            auto displayName = arrElement["displayName"];
            string wColName = colName.get<std::string>();
            string wDisplayName = displayName.get<std::string>();
            string ansiDisplayName = UTF8ToANSI(wDisplayName.c_str());

            myHeaders.push_back(std::make_pair(wColName.c_str(), ansiDisplayName.c_str()));
            countFields++;
        }
        int countRow = jsonData.size();



        ex.init_arr(countRow + 0 + 20, countFields);

        int r = 0;



        for (int c = 0; c < countFields; c++) {

            CString ss = myHeaders[c].second;

            CString s;
            s.Format("%d", c + 1);


            s = ss;


            ex.set_hv_align(CRange(1, c + 1), -4108, 2);
            ex.set_val(CRange(1, c + 1), L"WrapText", COleVariant(1L));


            ex.set_border(CRange(1, c + 1), 5);

            ex.set_bold(CRange(1, c + 1));


            ex.set_arr_str(CRange(1, c + 1), s);

        }
        r++;
        int rr = 0;
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it)
        {
            rr++;
            r++;
            auto arrElement = it.value();
            for (int c = 0; c < countFields; c++) {
                auto valueJson = arrElement[myHeaders[c].first.GetString()];

                COleVariant varValue;

                if (valueJson.is_string()) {
                    string w_value = valueJson.get<std::string>();
                    string ansi = UTF8ToANSI(w_value.c_str());
                    CString ansiString = ansi.c_str();
                    varValue.bstrVal = ansiString.AllocSysString();
                    varValue.vt = VT_BSTR;
                    ex.set_arr2(CRange(r, c + 1), varValue);

                }
                else if (valueJson.is_number_integer()) {
                    varValue.intVal = valueJson.get<int>();
                    varValue.vt = VT_I4;
                    ex.set_arr2(CRange(r, c + 1), varValue);

                }
                else if (valueJson.is_number_float()) {
                    varValue.dblVal = valueJson.get<double>();
                    varValue.vt = VT_R8;
                    ex.set_arr2(CRange(r, c + 1), varValue);
                }
                else {
                    int qq;
                    qq = 1;
                }
            }
        }
        for (int c = 0; c < countFields; c++) {
            ex.set_typ2(CRange(1, c + 1, countRow + 0 + 20, c + 1), excelType[myHeaders[c].first.GetString()]);
        }
        /*






        it = list_col.begin();

        for (int c = 0; c < nCol; c++) {
            int cc = *it++;
            CAdoField fld = ado->Field(cc);

        }

        */
        ex.example2(CRange(0, 0), -1);
    }
}

//вариант с сложными Header
void WVAdoImpDataSource::OnExcelPro(BSTR jsonStr, CString filePath, CString tmpName, HWND hwnd) {
    
    FILE* f = fopen(tmpName, "w");
    if (!f) {
        //      AfxMessageBox(strerror(errno), MB_OK|MB_ICONINFORMATION);
        AfxMessageBox("Закройте предыдущую таблицу", MB_OK | MB_ICONINFORMATION);
        return;
    }
    print_index1(f, "");
    FILE* g = 0;
    g = print_doc_with_path(f, filePath, _TR(m_sTableName));
    if (g) {

        std::wstring ws(jsonStr, SysStringLen(jsonStr));

        auto myJson = json::parse(ws);

        vector<std::pair<CString, CString>> myHeaders;
        json jsonHeaders = myJson["headers"];
        json jsonData = myJson["data"];
        int countFields = 0;
        for (auto it = jsonHeaders.begin(); it != jsonHeaders.end(); ++it)
        {
            auto arrElement = it.value();
            auto colName = arrElement["field"];
            auto displayName = arrElement["displayName"];
            string wColName = colName.get<std::string>();
            string wDisplayName = displayName.get<std::string>();
            string ansiDisplayName = UTF8ToANSI(wDisplayName.c_str());

            myHeaders.push_back(std::make_pair(wColName.c_str(), ansiDisplayName.c_str()));
            countFields++;
        }
        int countRow = jsonData.size();

        int rr = 0;
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it)
        {
            rr++;
            
            auto arrElement = it.value();
            fprintf(g, "<tr>");
            for (int c = 0; c < countFields; c++) {
                auto valueJson = arrElement[myHeaders[c].first.GetString()];

                COleVariant varValue;

                if (valueJson.is_string()) {
                    string w_value = valueJson.get<std::string>();
                    string ansi = UTF8ToANSI(w_value.c_str());
                    CString ansiString = ansi.c_str();
                    varValue.bstrVal = ansiString.AllocSysString();
                    varValue.vt = VT_BSTR;
                    fprintf(g, "<td>%s</td>", ansiString);
                

                }
                else if (valueJson.is_number_integer()) {
                    varValue.intVal = valueJson.get<int>();
                    varValue.vt = VT_I4;
                    fprintf(g, "<td>%d</td>", varValue.intVal);

                }
                else if (valueJson.is_number_float()) {
                    varValue.dblVal = valueJson.get<double>();
                    varValue.vt = VT_R8;
                    fprintf(g, "<td>%f</td>", varValue.dblVal);
                }
                else {
                    int qq;
                    qq = 1;
                }
            }
            fprintf(g, "</tr>");
        }

        
        

        print_doc_end(g);
        print_index2(f);
        fclose(f);
        setlocale(LC_NUMERIC, "eng");
        

        CSS css;
        CString cssName;
        cssName.Format("%s\\style.css", getenv("TMP"));
        css.print(cssName);
        HINSTANCE hRun = ShellExecute(hwnd, "open", "excel", "\"" + tmpName + "\"", NULL, SW_SHOWMAXIMIZED);
    }
}

//Новый метод при вызове PropertyDialog
CString WVAdoImpDataSource::updateData2(int id, int row, int countFields) {
    m_pRecordSet->Requery(-1);
    m_pRecordSet->MoveFirst();
    m_pRecordSet->Move(row);

    VARIANT vIntegerType;
    vIntegerType.vt = VT_I2;
    vIntegerType.intVal = 0;
    FieldsPtr      pFields;
    FieldPtr       pField;

    m_pRecordSet->get_Fields(&pFields);
    pFields->get_Item(vIntegerType, &pField);
    int idRow = pField->GetValue();
    CString qStorage;
    if (id == idRow)
    {
        
        m_pRecordSet->MoveFirst();
        m_pRecordSet->Move(row);
        qStorage = "{";
        for (long i = 0; i < countFields; i++) {
            
            if (positionS[i] != -1)
                try
            {
                _variant_t v;
                v = m_pRecordSet->GetCollect(i);

                
                CString tdQuery, s, comma;
                if (i != countFields - 1)
                    comma = ",";
                switch (v.vt)
                {
                case 1:
                    tdQuery.Format(_T("\"col_%d\" : \"\"%s"), i, comma);
                    break;
                case 2:
                case 3:
                    tdQuery.Format(_T("\"col_%d\" : %d%s"), i, v.lVal, comma);

                    break;
                case 4:
                case 5:
                    tdQuery.Format(_T("\"col_%d\" : %f%s"), i, to_double(v), comma);
                    break;
                case 7:
                case 8:
                    s = CCrack::strVARIANT_UTF(v);
                    s.Replace("\\", "\\\\");
                    s.Replace("\"", "\\\"");
                    tdQuery.Format(_T("\"col_%d\" : \"%s\"%s"), i, s, comma);

                    break;
                default:
                    tdQuery.Format(_T("\"col_%d\" : \"?\"%s"), i, comma);
                    break;
                }
                if (positionS[i] == -2) {
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
                    tdQuery.Format(_T("\"col_%d\" : [%s]%s"), i, all, comma);
                }
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
        
       

    }
    return qStorage;
}
int WVAdoImpDataSource::updateData(int id, int row, map<int, std::any> &mapParams) {
    m_pRecordSet->MoveFirst();
    m_pRecordSet->Move(row);

    VARIANT vIntegerType;
    vIntegerType.vt = VT_I2;
    vIntegerType.intVal = 0;
    FieldsPtr      pFields;
    FieldPtr       pField;

    m_pRecordSet->get_Fields(&pFields);
    pFields->get_Item(vIntegerType, &pField);
    int idRow = pField->GetValue();

    if (id == idRow)
    {
        try {

            map<int, std::any>::iterator it = mapParams.begin();

            while (it != mapParams.end())
            {
                if (it->first == 0)
                {

                }
                else  {

                    _variant_t tempVar, dbVariant;
                    tempVar.vt = VT_I4;
                    long myPosition = positionS[it->first];
                    if (myPosition == -2)
                        myPosition = 3;
                    tempVar.lVal = myPosition;

                    DataTypeEnum fieldN = fieldVariant(myPosition);

                    json* myJson = std::any_cast<json>(&(it->second));

                    if (myJson->is_string()) {
                        string w_value = myJson->get<std::string>();
                        string ansi = UTF8ToANSI(w_value.c_str());
                        CString ansiString = ansi.c_str();

                        if (w_value.empty() && fieldN != adVarWChar && fieldN != adWChar && fieldN != adVarChar && fieldN != adLongVarWChar && fieldN != adLongVarChar && fieldN != adChar)
                        {
                            dbVariant.vt = VT_NULL;
                        }
                        else {
                            dbVariant.bstrVal = ansiString.AllocSysString();
                            dbVariant.vt = VT_BSTR;
                        }


                        m_pRecordSet->PutCollect(tempVar, dbVariant);
                    }
                    else if (myJson->is_number_integer()) {
                        dbVariant.intVal = myJson->get<int>();
                        dbVariant.vt = VT_I4;
                        m_pRecordSet->PutCollect(tempVar, dbVariant);

                    }

                }
                it++;

            }
            if (mapParams.size() > 0)
            {
                m_pRecordSet->Update();
                return 200;

            }

        }
        catch (_com_error& e)
        {
            //      AfxMessageBox(e.Description(), MB_OK|MB_ICONINFORMATION);


            ///      m_pRecordSet->CancelUpdate();
            return 404;
        }
    }


    return 404;
}

CString getFullName(CString cx, CString tn, CString fn);
CString find_new_name_col(const char* tn1, const char* fn_new1);
CString remove____(CString s);

bool isRus(CString s)
{
    if (s.GetLength() == 0) return true;
    if ('А' <= s[0] && s[0] <= 'Я') return true;
    if ('а' <= s[0] && s[0] <= 'я') return true;
    return false;
}


CString WVAdoImpDataSource::getFullName1(CString tempStr)
{
    //    return tempStr;

    tempStr = remove____(tempStr);

    if (isRus(tempStr)) return tempStr;


    CString m_sTableName1 = getTableNameFromSQL(m_q);

    if (m_sTableName1 == "") m_sTableName1 = m_sTableName;

    CString tempStr2 = getFullName(m_sDbName, m_sTableName1, tempStr);

    if (tempStr2 == tempStr) {
        CString new_col = find_new_name_col(m_sTableName1, tempStr);
        tempStr2 = getFullName(m_sDbName, m_sTableName1, new_col);
    }

    if (tempStr2 == tempStr) {
        tempStr2 = getFullName(m_sDbName, "?", tempStr);

        const CColName* findColumnRusName(CString d, CString n1, CString n2);


        const CColName* col_name = findColumnRusName(m_sDbName, "?", tempStr);
        if (col_name) {
            tempStr2 = col_name->name;
        }

        if (tempStr2 == tempStr) {
            CString new_col = find_new_name_col(m_sTableName1, tempStr);
            tempStr2 = getFullName(m_sDbName, "?", new_col);
        }
    }


    if (tempStr2 == tempStr) {
        if (m_sTableName1 == "Istochniki_tepla") {
            tempStr2 = getFullName(m_sDbName, "prisoedinennaya_nagruzka_istochnikov", tempStr);
        }
        else if (m_sTableName1 == "nodes") {
            tempStr2 = getFullName(m_sDbName, transl("Узел"), tempStr);
        }
        else {
            if (m_sTableName != "") {
                tempStr2 = getFullName(m_sDbName, m_sTableName, tempStr);
            }
        }
    }

    return tempStr2;
}





inline void TESTHR(HRESULT x) { if FAILED(x) _com_issue_error(x); };


pair<int, CString> WVAdoImpDataSource::execute(CString q) {
    try {
        CString m_error = "";

        _CommandPtr pCmdChange = NULL;

        _bstr_t strQ(q);

        TESTHR(pCmdChange.CreateInstance(__uuidof(Command)));
        pCmdChange->ActiveConnection = m_pConnection;
        pCmdChange->CommandText = strQ;

        COleVariant liRecordsAffected;

        pCmdChange->Execute(&liRecordsAffected, NULL, adCmdText);
        long affected = (long)V_I4(&liRecordsAffected);
        return std::make_pair(200, "Okey");
    }
    catch (_com_error& e)
    {

        return std::make_pair(404, (LPCTSTR)e.Description());
    }

}



pair<int, CString> WVAdoImpDataSource::deleteData(int id, int row) {

    CString q;

    if (m_sTableName_main == "osmotr" || m_sTableName_main == "remont2") {

        q.Format("DELETE FROM %sDeployed WHERE directionID=%d", m_sTableName_main, id);
        execute(q);

        if (m_sTableName_main == "osmotr") {
            q.Format("DELETE FROM faktory_riska_truboprovoda WHERE objID=%d AND obj_type_faktory_riskaID=2", id);
            execute(q);
        }
        if (m_sTableName_main == "remont2") {
            q.Format("DELETE FROM faktory_riska_truboprovoda WHERE objID=%d AND obj_type_faktory_riskaID=3", id);
            execute(q);
        }

    }
    q.Format("DELETE FROM [%s] WHERE id=%d", m_sTableName_main, id);

    return execute(q);
}

void  WVAdoImpDataSource::setAllows(VIRT_DATA typ) {
    myType = typ;
//    allowEdit = typ == virt_data_edit || typ == virt_data_edit_add || typ == virt_data_edit_add_del || typ == virt_data_edit_del
//        || typ == virt_data_node || typ == virt_data_line || virt_data_geo;

    allowEdit = typ == virt_data_edit || typ == virt_data_edit_add || typ == virt_data_edit_add_del || typ == virt_data_edit_del
        || typ == virt_data_node || typ == virt_data_line || typ == virt_data_geo_edit;

    allowAdd = typ == virt_data_edit_add_del;
    allowDelete = typ == virt_data_edit_add_del || typ == virt_data_del || typ == virt_data_geo_del || typ == virt_data_geo_edit;
}

pair<int, CString> WVAdoImpDataSource::deleteData(vector<int> ids) {
    try {
        CString q;

        CString idsString;
        int arrSize = ids.size();

        for (int i = 0; i < arrSize; i++) {
            CString number;
            number.Format("%d", ids[i]);
            idsString += number;
            if (i != arrSize - 1)
                idsString += ",";
        }
        q.Format("DELETE FROM [%s] WHERE id in(%s)", m_sTableName_main, idsString);


        CString m_error = "";

        _CommandPtr pCmdChange = NULL;

        _bstr_t strQ(q);

        TESTHR(pCmdChange.CreateInstance(__uuidof(Command)));
        pCmdChange->ActiveConnection = m_pConnection;
        pCmdChange->CommandText = strQ;

        COleVariant liRecordsAffected;

        pCmdChange->Execute(&liRecordsAffected, NULL, adCmdText);
        long affected = (long)V_I4(&liRecordsAffected);
        return std::make_pair(200, "Okey");
    }
    catch (_com_error& e)
    {

        return std::make_pair(404, (LPCTSTR)e.Description());
    }

}

double WVAdoImpDataSource::to_double(_variant_t& var)
{
    if (var.vt == VT_NULL) return 0L;

    if (var.vt == VT_I4) return (double)V_I4(&var);
    if (var.vt == VT_I2) return (double)V_I2(&var);

    if (var.vt == VT_R4) return (double)V_R4(&var);
    if (var.vt == VT_R8) return (double)V_R8(&var);

    return 0L;
}

long WVAdoImpDataSource::GetNumRows()
{
    OutputDebugStringA("CADOImpDatasource GetNumRows\n");
    // make sure the recordset is open
    if (m_pRecordSet == NULL)
        return -1;

    // fetch the first batch of rows
    FetchNewRows(0);

    // when number of fetched rows is zero (empty recordser)
    // and ghost row is enabled than we must properly identify
    // number of rows (1)
    //if (m_pcRowsObtained == 0 && m_bUseGhostRow == TRUE)
    //{
     //   m_pcRowsObtained = 1;
     //   m_nCurGhostRow = 0;
    //}

    // assume virtual loading through OnHitBottom
    return m_pcRowsObtained;
}

int WVAdoImpDataSource::OnHitBottom(int row) {
    //если последнее вытягивание было меньше чем максимальное вытягивание за раз, тогда мы достигли конца данных
    if (m_pcRowsObtained < m_nMaxFetchRows)
    { // the recordset alredy reached the bottom, there is no need to try
      // to fetch additional (new) rows.  They will always be the same
    }
    else if (FetchNewRows(row) == UG_SUCCESS)
    {
        return UG_SUCCESS;
    }
    return UG_NA;


}
