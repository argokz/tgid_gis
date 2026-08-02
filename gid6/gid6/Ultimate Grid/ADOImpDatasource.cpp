/*************************************************************************
        Class Implementation : CADOImpDatasource
**************************************************************************
  Source file : ADOImpDatasource.cpp
// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
*************************************************************************/
#include "stdafx.h"
#include "UGCtrl.h"
#include "ADOImpDatasource.h"
#include "win.h"

#include <OLEDBERR.H>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#include "ado.h"


CString getTableNameFromSQL(CString q);

bool isTableNum(CAdoFile* ado, const char* tn, const char* fn, int id);
bool isTableNumLine(CAdoFile* ado, const char* tn, const char* fn, int id);

bool isRemovable(CAdoFile* ado, const char* tn1, int id)
{
    CString tn = tn1;
    tn.MakeLower();

    if (tn == "rayon_ekspluatatsii") {
        if (isTableNum(ado, "uchastki_ekspluatatsii", "rayon_ekspluatatsii", id)) {
            AfxMessageBox("Нельзя удалить район эксплуатации, есть участки эксплуатации входящие в него", MB_OK | MB_ICONINFORMATION);
            return false;
        }
    }
    else if (tn == "istochniki_tepla") {
        //        if (isTableNum(ado, "", "rayon_ekspluatatsii", id)) return false;
    }
    else if (tn == "nachalniki_uchastkov") {
        if (isTableNum(ado, "uchastki_ekspluatatsii", "nachalnik_uchastka", id)) {
            AfxMessageBox("Нельзя удалить начальника участка, есть участок эксплуатации ссылающийся на него", MB_OK | MB_ICONINFORMATION);
            return false;
        }
    }
    else if (tn == "uchastki_ekspluatatsii") {
        if (isTableNum(ado, "uchastok_ms", "nomer_uchastka", id)) {
            AfxMessageBox("Нельзя удалить участок эксплуатации, есть участки МС входящие в него", MB_OK | MB_ICONINFORMATION);
            return false;
        }
        if (isTableNum(ado, "uchastok_rs", "nomer_uchastka", id)) {
            AfxMessageBox("Нельзя удалить участок эксплуатации, есть участки РС входящие в него", MB_OK | MB_ICONINFORMATION);
            return false;
        }
    }
    else if (tn == "uchastok_ms") {
        if (isTableNumLine(ado, "heatPipeSections", "magistralSite", id)) {
            AfxMessageBox("Нельзя удалить участок МС, фрагмент оцифрован, имеются участки входящие в него", MB_OK | MB_ICONINFORMATION);
            return false;
        }
    }
    else if (tn == "uchastok_rs") {
        if (isTableNumLine(ado, "heatPipeSections", "distSite", id)) {
            AfxMessageBox("Нельзя удалить участок РС, фрагмент оцифрован, имеются участки входящие в него", MB_OK | MB_ICONINFORMATION);
            return false;
        }
    }
    else if (tn == "magistral") {
        if (isTableNumLine(ado, "heatPipeSections", "magistral", id)) {
            AfxMessageBox("Нельзя удалить Магистраль, фрагмент оцифрован, имеются участки входящие в нее", MB_OK | MB_ICONINFORMATION);
            return false;
        }
    }

    return true;
}


bool CADOImpDatasource::isUpdatable(long col)
{
    //    return false;

    if (!m_bAllowEdits)  return false;

    CString col_name;
    if (GetColName(col, &col_name) == UG_ERROR) return false;
    if (col_name.CompareNoCase("id") == 0) return false;

    if (m_head) {
        set<int>::const_iterator it = m_head->is_edit.find(col);
        return it != m_head->is_edit.end();
    }



    long attr = m_pRecordSet->GetFields()->GetItem((COleVariant)col)->GetAttributes();
    attr = m_pRecordSet->GetFields()->GetItem((COleVariant)col)->GetAttributes();
    return true;
    //    return a&adFldUpdatable;
}


/***************************************************
  Standard construction/desrtuction
***************************************************/
CADOImpDatasource::CADOImpDatasource()
{
    m_pConnection = NULL; // the connection
    m_pRecordSet = NULL;  // the recordset

    m_type_of_net = 0;

    m_colorCol = -1;

    m_n_keys = 0;

    m_error = "";

    m_head = NULL;

    m_sDbName = _T(""); //
    m_sTableName = _T(""); // 

    m_q = _T("");

    m_dbConnection = _T("");// source string for the connection
    m_User = _T("");    // user login name
    m_Password = _T("");  // user login password
    m_Command = _T("");   // command to execute
    m_Filter = _T("");    // filter to be used
    m_SortOrder = _T(""); // sort order to be used
    m_sCurRowID = _T("»");

    m_showAll = 0;      // Показывать все строки

    m_numCols = -1;     // indicate that the number of columns is unknown

    // fetching
    m_nFetchedStarting = 0;
    m_pcRowsObtained = 0;
    m_nMinFetchRows = MIN_FETCH_ROWS;

    m_bAllowDelete = TRUE;

    // editing
    m_bAllowEdits = TRUE;
    m_bEditingConfirmed = FALSE;

    // flag indicating that we can use GhostRow
    m_bUseGhostRow = TRUE;
    m_bUseGhostRow = FALSE;
    m_nCurGhostRow = -1;
    m_nOldGhostRow = -1;

    // AppendRow style
    m_bUseSimpleNew = TRUE;
    m_bUseSimpleNew = FALSE;   // изучить что тут нужнно поставить !!!!!!!!!!!!!

    // number of rows waiting update
    m_iUpdatesPending = 0;
    m_sTableName_main = "";

    m_idCol = -1;

    m_rows_all = -1;

    // Initialize OLE
    CoInitialize(NULL);
}

CADOImpDatasource::~CADOImpDatasource()
{
    // make sure that all changes are saved before exiting
    if (UpdatePending(FALSE) == UG_ERROR)
        // clear our the updated cells list
        m_hUpdatedRow.Empty();

    // close recordset and connection
    Close();

    CoUninitialize();
}

/***************************************************
Open
  The ADO datasource uses this function to open the record set
  using information set by the SetPassword and parameters passed in.
Params:
  name    - represents the connection string
  option    - not used
Return:
  UG_NA   not available
  UG_SUCCESS  success
  1...    error codes (data source dependant)
****************************************************/
int CADOImpDatasource::Open(LPCTSTR name, LPCTSTR option)
{
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


    for (auto& q : list_q) {
        Execute0(q);
    }

    // open the recordset
    return OpenRecordset();
}

long getCountTable2(CAdoFile* ado, const char* q);


/***************************************************
OpenSQL
  function can be used to open a record set by specifying the SQL statement
  to be used.
Params:
  sql     - SQL statement to be used
  conStr    - (optional) connection string, it can also be set with
          SetCommand function.
Return:
  UG_SUCCESS  success
  UG_ERROR  when sql string passed in is empty
****************************************************/
int CADOImpDatasource::OpenSQL(CString sql, CString conStr, int type_of_net)
{
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

    void split_sql_expressions(CString text, std::list<CString> &list_q);

    split_sql_expressions(m_q, list_q);

    int list_n = list_q.size();

    if (list_n >= 1) {
        m_q = list_q.back();
        list_q.pop_back();
    }






    m_sTableName_main = getTableNameFromSQL(m_q);



    //  m_q = "select id FROM opora";

      // copy connection string and the SQL statement
    if (ParseSQLString(m_q) == UG_ERROR)
        return UG_ERROR;





    return Open(conStr, NULL);
}

/***************************************************
ParseSQLString  - protected
  function is called by the OpenSQL function in order to break up the SQL
  statement passed-in into separate components (command, filter, sort).
Params:
  sql     - SQL string passed into the OpenSQL
Return:
  UG_SUCCESS  success, this function will never fail.
****************************************************/
int CADOImpDatasource::ParseSQLString(CString sql)
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
int CADOImpDatasource::SetCommand(CString command)
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
int CADOImpDatasource::SetFilter(CString filter)
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
int CADOImpDatasource::SetSort(CString sort)
{
    m_SortOrder = sort.SpanExcluding(_T(";"));
    return UG_SUCCESS;
}

/***************************************************
OpenConnection  - protected
  function is used to open the connection to the data base as specified
  by connection string (m_dbConnection), User Name (m_User) and Password
  (m_Password).
Params:
  <none>
Return:
  UG_NA   not available
  UG_SUCCESS  success
  1...    error codes (data source dependant)
****************************************************/
int CADOImpDatasource::OpenConnection()
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
    if (m_ctrl != NULL)
        m_ctrl->OnDataSourceNotify(GetID(), UG_ADO_DS_OPEN_CONN, (long)m_pConnection);

    return UG_SUCCESS;
}

/***************************************************
GetSQL  - public
  The GetSQL function will create a string representing
  currently used SQL statement by combining information
  found in the m_Command, m_Filter, and m_SortOrder
Params:
  <none>
Return:
  string representing currently used SQL statement
****************************************************/
CString CADOImpDatasource::GetSQL()
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

/***************************************************
OpenRecordset - protected
  function is used to create and open recordset object that will be
  used to populate the grid with.
Params:
  <none>
Return:
  UG_NA   not available
  UG_SUCCESS  success
  1...    error codes (data source dependant)
****************************************************/
int CADOImpDatasource::OpenRecordset()
{
    CString sqlString = GetSQL();

    try // open the recordset
    {
        m_pRecordSet.CreateInstance(__uuidof(Recordset));
    
        log1(sqlString);

        m_pRecordSet->Open((_bstr_t)sqlString, _variant_t((IDispatch*)m_pConnection), adOpenKeyset, adLockOptimistic, adCmdUnknown);

        if (m_ctrl != NULL)
            // provide the CUGCtrl class with information on the recordset that
            // was successfuly created and open, allows user with ability to perform
            // further processing on the recordset.
            m_ctrl->OnDataSourceNotify(GetID(), UG_ADO_DS_OPEN_RS, (long)m_pRecordSet);

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

/***************************************************
IsOpen
  virtual function is most commonly used by datasources
  that bind to a database or some form of an external data.
  It is used to provide feedback to the developer who requires
  to know if a connection to the database is currently open
  or closed.
Params:
  <none>
Return:
  FALSE if the datasource is closed and TRUE if it is open.
***************************************************/
BOOL CADOImpDatasource::IsOpen()
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
int CADOImpDatasource::Close()
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
        m_hUpdatedRow.Empty();
        m_nCurGhostRow = -1;
        m_nOldGhostRow = -1;
        m_curBookmark.Clear();
        // number of rows waiting update
        m_iUpdatesPending = 0;

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

/***************************************************
SetPassword
  A virtual function that provides standard interface
  to set user name and password used to open the data
  source.
Params:
  user  - user name to use
  pass  - password
Return:
  UG_SUCCESS  success, this function will not fail.
****************************************************/
int CADOImpDatasource::SetPassword(LPCTSTR user, LPCTSTR pass)
{
    m_User = user;
    m_Password = pass;

    return UG_SUCCESS;
}

/***************************************************
GetNumRowsComplete
  function completes functionality of GetNumRows,
  and allows us to determine the total number of
  records in the recordset without consideration
  for howmany rows grid is currently aware of.
Params:
  <none>
Return:
  number of rows found in the recordset, or
  -1 on error.
****************************************************/
long CADOImpDatasource::GetNumRowsComplete()
{
    // make sure the recordset is open
    if (m_pRecordSet == NULL)
        return -1;

    // The RecordCount ADO property will return exact
    // number of records found in the recordset if recordset
    // Supports (adApproxPosition) or Supports (adBookmark).
    return m_pRecordSet->RecordCount;
}

/***************************************************
GetNumRows
  A virtual function that provides standard interface
  for the grid to find out how many rows are in the
  data source.  If the data source is not able to
  determine how many rows there are, than it should
  only return a value greater than zero defining how
  many rows it is aware of.  The grid then will use
  the OnHitBottom notification to check if there are
  additional rows.
Params:
  <none>
Return:
  number of rows found in the recordset, this number might
  not represent all of the rows in the recordset, or
  -1 on error.
****************************************************/
long CADOImpDatasource::GetNumRows()
{
    // make sure the recordset is open
    if (m_pRecordSet == NULL)
        return -1;

    // fetch the first batch of rows
    FetchNewRows(0);

    // when number of fetched rows is zero (empty recordser)
    // and ghost row is enabled than we must properly identify
    // number of rows (1)
    if (m_pcRowsObtained == 0 && m_bUseGhostRow == TRUE)
    {
        m_pcRowsObtained = 1;
        m_nCurGhostRow = 0;
    }

    // assume virtual loading through OnHitBottom
    return m_pcRowsObtained;
}

HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA);
CString bstr2a(_bstr_t b);


/***************************************************
FetchNewRows
  This function is called by the GetCell notification when
  it needs to display rows that are not currently cached

  The next fetch position will always be set to bookmark of the
  first row in the array of fetched HROWs.
Parameters
  row   The row of interest
Returns
  UG_SUCCESS
  UG_ERROR
****************************************************/
int CADOImpDatasource::FetchNewRows(long row)
{
    // Determine howmany rows are currently visible on the screen, use the window
    // size and default row size as a guide.
    CRect rect;
    m_ctrl->GetWindowRect(rect);
    long nVisibleRows = (rect.bottom - rect.top) / m_ctrl->m_GI->m_defRowHeight;
    // nStartingDelta determines the position change in relation to current DB position
    long nStartingDelta = m_nFetchedStarting;

    // find out how many rows need to be fetched
    m_nMaxFetchRows = nVisibleRows * 3;

    // make sure that enough rows are fetched at one time
    if (m_nMaxFetchRows < m_nMinFetchRows)
        m_nMaxFetchRows = m_nMinFetchRows;

    // reset starting row for the fetch rows buffer
    m_nFetchedStarting = row - (m_nMaxFetchRows / 2);

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

/***************************************************
GetNumCols
  A virtual function that provides standard interface
  for the grid to find out how many columns are in the
  data source.
Params:
  <none>
Return:
  number of columns in the datasource, or
  -1 encountered error
****************************************************/
int CADOImpDatasource::GetNumColsReal()
{
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

int CADOImpDatasource::GetNumCols()
{
    /*

      if (m_head && m_head->cols > 0) {
          m_numCols = m_head->cols;
          return m_numCols;
      }
    */
    return GetNumColsReal();
}


/***************************************************
GetColName
  A virtual function that provides standard interface
  to provide the grid with the name of a column.
Params:
  col   - column number for which to return name
  string  - pointer to a string which should be populated
        with the column name.
Return:
  UG_NA   not available
  UG_SUCCESS  success
  1...    error codes (data source dependant)
****************************************************/

int CADOImpDatasource::getNewCol(int col)
{
    if (m_head && m_head->cols > 0) {
        int cc = GetNumColsReal();

        for (int c = 0; c < cc; c++) {
            CString ss;
            if (GetColNameReal(c, &ss) == UG_SUCCESS) {
                CString qq;
                qq = ss;
            }
        }
    }

    return col;
}


int CADOImpDatasource::GetColNameReal(int col, CString* string)
{
    if (m_pRecordSet == NULL || col < 0 || col > m_numCols)
        return UG_ERROR;

    try
    {
        *string = VarToString(m_pRecordSet->GetFields()->GetItem((COleVariant)(long)col)->GetName());
    }
    catch (_com_error /*&e*/)
    {
        return UG_ERROR;
    }

    return UG_SUCCESS;
}


int CADOImpDatasource::GetColName(int col, CString* string)
{
    col = getNewCol(col);
    return GetColNameReal(col, string);
}

/***************************************************
VarToString - protected
  function is a helper function that is used to convert strings
  storred in a _variant_tobject to CString.
Params:
  theVariant  - _variant_t object from which extract string.
Return:
  string    - contained string
****************************************************/
CString CADOImpDatasource::VarToString(_variant_t theVariant)
{
    CString tempStr;

    if (theVariant.vt != VT_NULL)
    {
        tempStr = (const char*)_bstr_t(theVariant);
        tempStr.TrimLeft();
        tempStr.TrimRight();
        return tempStr;
    }
    return _T("");
}

/***************************************************
GetCell
  A virtual function that provides standard way
  for the grid to populate a cell object.  This
  function is called as a result of the
  CUGCtrl::GetCell being called.
Params:
  col, row  - coordinates of the cell to retrieve
          information on.
  cell    - pointer to CUGCell object to populate
          with the information found.
Return:
  UG_NA   not available
  UG_SUCCESS  success
  1...    error codes (data source dependant)
****************************************************/


int regex_match(const char* p, const char* buf, vector<CString>& match);

CString getFullName(CString cx, CString tn, CString fn);


static map<CString, CString> map_sql_name;

CString getTableNameFromSQL(CString q)
{
    CString q0 = q;

    auto it = map_sql_name.find(q);
    if (it != map_sql_name.end()) {
        return it->second;
    }

    vector<CString> match;

    q.Replace("\nFROM ", " FROM ");
    q.Replace("\nFROM\n", " FROM ");
    q.Replace("\nfrom ", " FROM ");
    q.Replace("\nfrom\n", " FROM ");


    int i = q.Find(" FROM ");
    if (i >= 0 && i + 256 < q.GetLength()) {
        q = q.Left(i + 256);
    }

    int l = regex_match(".+\\s+FROM\\s+\\[?([A-Za-z0-9_\\.]+)\\]?", q, match);
    if (l >= 1) {
        map_sql_name[q0] = match[1];
        return match[1];
    }


    i = q.Find(" from ");
    if (i >= 0 && i + 256 < q.GetLength()) {
        q = q.Left(i + 256);
    }

    l = regex_match(".+\\s+from\\s+\\[?([A-Za-z0-9_\\.]+)\\]?", q, match);
    if (l >= 1) {
        map_sql_name[q0] = match[1];
        return match[1];
    }

    map_sql_name[q0] = "";
    return "";
}

unsigned long IdToColor(int id)
{
    id = id / 19;

    if (id <= 1) return 0xFFFFFF;
    if (id == 2) return 0x00FF00;
    if (id == 3) return 0xFFFF00;
    if (id == 4) return 0xFF8000;
    if (id >= 5) return 0x0000FF;

    return 0;
}

CString remove____(CString s)
{
    int i = s.Find("____");
    if (i >= 0) {
        s = s.Mid(i + 4);
    }
    return s;
}

CString find_new_name_col(const char* tn1, const char* fn_new1);


CString CADOImpDatasource::getFullName1(CString tempStr)
{
    //    return tempStr;

    tempStr = remove____(tempStr);

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



int CADOImpDatasource::GetCell(int col, long row, CUGCell* cell)
{
    if (m_pRecordSet == NULL)
        return UG_ERROR;

    if (row < 0 && m_head) {
        CString name = m_head->getText(m_head->rows + row, col);
        if (name == "$") {
            CString tempStr;
            GetColName(col, &tempStr);
            CString m_sTableName1 = getTableNameFromSQL(m_q);

            if (m_sTableName1 == "") m_sTableName1 = m_sTableName;

            tempStr = getFullName(m_sDbName, m_sTableName1, tempStr);
            cell->SetText(tempStr);
            return UG_SUCCESS;
        }
        else {
            cell->SetText(name);
        }

        return UG_SUCCESS;
    }

    if (col >= m_ctrl->GetNumberCols() || row >= m_ctrl->GetNumberRows())
        return UG_ERROR;

    if (col >= 0 && row == -2)
    {
        CString tempStr;
        tempStr.Format("%d", col);
        cell->SetText(tempStr);
        return UG_SUCCESS;
    }


    if (col >= 0 && row == -1)
    { // display column headings
        CString tempStr;
        GetColName(col, &tempStr);
        tempStr = getFullName1(tempStr);

#if 0
        tempStr = remove____(tempStr);

        CString m_sTableName1 = getTableNameFromSQL(m_q);

        if (m_sTableName1 == "") m_sTableName1 = m_sTableName;

        CString tempStr2 = getFullName(m_sDbName, m_sTableName1, tempStr);

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

        tempStr = tempStr2;
#endif

        cell->SetText(tempStr);
        return UG_SUCCESS;
    }
    else if (col == -1 && row == m_ctrl->GetCurrentRow())
    { // display current row indicator
        cell->SetText(m_sCurRowID);
    }
    else if (col >= 0 && row >= 0)
    {

        if (row == m_ctrl->GetCurrentRow() && col == m_ctrl->GetCurrentCol() &&
            (!(row >= m_nFetchedStarting && row <= m_nFetchedStarting + m_pcRowsObtained)))
        {
            return UG_SUCCESS;
        }

        // populate cell with data from data source
        _variant_t var = GetDBCellData(col, row);

        if (var.vt == VT_ERROR)
        { // the retrieving of cell's data failed
            return UG_ERROR;
        }

        if (m_head && !m_head->isEdit(col)) {
            cell->SetBackColor(GetSysColor(COLOR_BTNFACE));
        }

        if (1 && row >= 0 && m_colorCol >= 0) {

            //      CUGCell cell2;
            //      GetCell(m_colorCol, row, &cell2);
            //      cell2.GetNumber();

            _variant_t var = GetDBCellData(m_colorCol, row);
            long to_long(const COleVariant & var);

            long l = to_long(var);

            unsigned long color = IdToColor(l);



            cell->SetBackColor(color);
        }

        // set the value to the grid.
        PopulateCellFromDB(cell, var);
    }

    return UG_SUCCESS;
}

/***************************************************
GetDBCellData
  function looks for data for a given cell.  The data can be retrieved
  from the database (fetched buffer), database's defaults (in case of
  the ghost row) or list of made updates.
Params:
  col, row
Returns:
  _variant_t object representing cell's data
*****************************************************/
_variant_t CADOImpDatasource::GetDBCellData(int col, long row)
{
    _variant_t result;

    long rownum = m_hUpdatedRow.GetUpdateRowNumber();

    if (rownum == row)
    {
        if (m_hUpdatedRow.IsUpdated(col))
        { // the row in question is not in the fetched rows array,
          // check the new rows list for possible values
          // pSafeRowArray = m_hUpdatedRow.GetRowSArray( row );
            result = m_hUpdatedRow.GetUpdatedVal(col);
        }
    }

    if (row != m_nCurGhostRow && result.vt == VT_EMPTY)
    {
        if ((row - m_nFetchedStarting) < 0 ||
            (row - m_nFetchedStarting) >= (long)m_pcRowsObtained)
        { // the row that the grid needs is not in the fetched buffer.
            int fetchRow = row;
            // If the requested row is visible than fetch rows based on
            // the top row
            if (row >= m_ctrl->GetTopRow() && row <= m_ctrl->GetBottomRow())
                fetchRow = m_ctrl->GetTopRow();
            // Fetch new rows from the data source.
            FetchNewRows(fetchRow);
        }

        if (row >= m_nFetchedStarting &&
            row < m_nFetchedStarting + ((long)m_pcRowsObtained))
        {
            long rgIndices[2];
            rgIndices[0] = col;
            rgIndices[1] = row - m_nFetchedStarting;

            try
            {
                if (m_curRecords.vt != VT_EMPTY)
                    SafeArrayGetElement(m_curRecords.parray, rgIndices, &result);
            }
            catch (...)
            {
            }
        }
        else
        {
            long rowsFound = 0;
            OnHitBottom(row, 0, &rowsFound);
        }
    }

    if ((row == m_nCurGhostRow || row == m_nOldGhostRow) && result.vt == VT_EMPTY)
    {
        // retrieve column's data type
        DataTypeEnum type;
        m_pRecordSet->GetFields()->GetItem((COleVariant)(long)col)->get_Type(&type);
        if (!isUpdatable(col))
        {
            result.vt = VT_NULL;
            return result;
        }

        switch (type)
        {
        case adTinyInt: // DBTYPE_I1:
            result.cVal = (char)0;
            result.vt = VT_I1;
            break;
        case adUnsignedTinyInt: // DBTYPE_UI1:
            result.bVal = (BYTE)0;
            result.vt = VT_UI1;
            break;
        case adSmallInt: // DBTYPE_I2:
            result.iVal = (short)0;
            result.vt = VT_I2;
            break;
        case adUnsignedSmallInt: // DBTYPE_UI2:
            result.uiVal = (unsigned short)0;
            result.vt = VT_UI2;
            break;
        case adInteger: // DBTYPE_I4:
            result.lVal = (long)0;
            result.vt = VT_I4;
            break;
        case adUnsignedInt: // DBTYPE_UI4:
            result.ulVal = (unsigned long)0;
            result.vt = VT_UI4;
            break;
        case adBigInt: // DBTYPE_I8:
            result.intVal = (LONG)0;
            result.vt = VT_I8;
            break;
        case adUnsignedBigInt: // DBTYPE_UI8:
            result.uintVal = (ULONG)0;
            result.vt = VT_UI8;
            break;
        case adSingle: // DBTYPE_R4:
            result.fltVal = (float)0;
            result.vt = VT_R4;
            break;
        case adDouble: // DBTYPE_R8:
            result.dblVal = (double)0;
            result.vt = VT_R8;
            break;
        case adBoolean: // DBTYPE_BOOL:
            result.boolVal = FALSE;
            result.vt = VT_BOOL;
            break;
        default:
            result.vt = VT_NULL;
            break;
        }
    }

    return result;
}

/***************************************************
PopulateCellFromDB
  Now that we have the value loaded as variant we can do
  further processing based on the data type and the value loaded.
  This function will make sure that the grid shows data correctly
Params:
  cell
  dbVariant
Returns:
  <none>
*****************************************************/
void CADOImpDatasource::PopulateCellFromDB(CUGCell* cell, _variant_t dbVariant)
{
    COleCurrency currency;
    COleDateTime dateTime;
    CString cellString;

    switch (dbVariant.vt)
    {
    case VT_DECIMAL:
    {
        DECIMAL dc = dbVariant.decVal;

        if (dc.scale == 0) {
            long l = (long)dc.Lo32;
            if (dc.sign) l = -l;

            cell->SetNumber(l);
            cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        }
        else {
            long l = (long)dc.Lo32;
            if (dc.sign) l = -l;

            double dd = (double)l / pow(10., dc.scale);

            cell->SetNumber(dd);
            cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        }
    }

    break;
    case VT_I1:   // char
        cell->SetNumberDecimals(0);
        cell->SetNumber(dbVariant.cVal);
        cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        break;
    case VT_UI1:  // Byte
        cell->SetNumberDecimals(0);
        cell->SetNumber(dbVariant.bVal);
        cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        break;
    case VT_I2:   // Short
        cell->SetNumberDecimals(0);
        cell->SetNumber(dbVariant.iVal);
        cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        break;
    case VT_UI2:  // unsigned short
        cell->SetNumberDecimals(0);
        cell->SetNumber(dbVariant.uiVal);
        cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        break;
    case VT_I4:   // long
        cell->SetNumberDecimals(0);
        cell->SetNumber(dbVariant.lVal);
        cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        break;
    case VT_UI4:  // unsigned long
        cell->SetNumberDecimals(0);
        cell->SetNumber(dbVariant.ulVal);
        cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        break;
    case VT_INT:  // int
        cell->SetNumberDecimals(0);
        cell->SetNumber(dbVariant.intVal);
        cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        break;
    case VT_UINT: // unsigned int
        cell->SetNumberDecimals(0);
        cell->SetNumber(dbVariant.uintVal);
        cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        break;
    case VT_R4:   // float
      // When converting from a float data type to a double
      // we multiply the float by 10000 and type cast the result
      // to an integer, the integer is then converted to a double
      // and divided by 10000.  We are required to do this
      // due to a precision loss when converting from float
      // data type to double.  This issue is described in
      // "Why Floating-Point Numbers May Lose Precision"
      // article in MSDN.
        cell->SetNumber(((double)((int)(dbVariant.fltVal * 10000))) / 10000);
        cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        break;
    case VT_R8:   // double
        cell->SetNumber(dbVariant.dblVal);
        cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        break;
    case VT_BOOL: // VARIANT_BOOL
        cell->SetBool(dbVariant.boolVal);
        cell->SetCellType(UGCT_CHECKBOX);
        cell->SetCellTypeEx(UGCT_CHECKBOXUSEALIGN);
        cell->SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
        break;
    case VT_CY:   // CY
        cell->SetNumberDecimals(2);
        cell->SetNumber(dbVariant.lVal / (double)10000);
        cell->SetDataType(UGCELLDATA_CURRENCY);
        cell->SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
        break;
    case VT_DATE: // DATE
        dateTime = dbVariant.date;
        cell->SetText(dateTime.Format(VAR_DATEVALUEONLY));

        cellString.Format(_T("%f"), dbVariant.date);
        cell->SetLabelText(cellString);
        cell->SetDataType(UGCELLDATA_TIME);
        cell->SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
        break;
    case VT_BSTR: // BSTR
        cell->SetText(CString(dbVariant.bstrVal));
        cell->SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
        break;
    default:
        // cell is empty
        cell->SetText(_T(""));
        cell->SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
        break;
    }
}

/***************************************************
GetDefaultValue
  This function is used to make sure that the variant
  contains proper value only used for new rows.
Params:
  cell
  dbVariant
Returns:
  <none>
*****************************************************/
_variant_t CADOImpDatasource::SetDBCellData(int col, CUGCell* pCell)
{
    DataTypeEnum type;
    _variant_t dbVariant;
    COleCurrency currency;
    COleDateTime dateTime;
    CString cellString;

    // retrieve column's data type
    m_pRecordSet->GetFields()->GetItem((COleVariant)(long)col)->get_Type(&type);

    switch (type)
    {
    case adTinyInt: // DBTYPE_I1:
        if (pCell->GetPropertyFlags() & UGCELL_STRING_SET)
        {
            dbVariant.cVal = (char)pCell->GetNumber();
            dbVariant.vt = VT_I1;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adUnsignedTinyInt: // DBTYPE_UI1:
        if (pCell->GetPropertyFlags() & UGCELL_STRING_SET)
        {
            dbVariant.bVal = (BYTE)pCell->GetNumber();
            dbVariant.vt = VT_UI1;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adSmallInt: // DBTYPE_I2:
        if (pCell->GetPropertyFlags() & UGCELL_STRING_SET)
        {
            dbVariant.iVal = (short)pCell->GetNumber();
            dbVariant.vt = VT_I2;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adUnsignedSmallInt: // DBTYPE_UI2:
        if (pCell->GetPropertyFlags() & UGCELL_STRING_SET)
        {
            dbVariant.uiVal = (unsigned short)pCell->GetNumber();
            dbVariant.vt = VT_UI2;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adInteger: // DBTYPE_I4:
        if (pCell->GetPropertyFlags() & UGCELL_STRING_SET)
        {
            dbVariant.lVal = (long)pCell->GetNumber();
            dbVariant.vt = VT_I4;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adUnsignedInt: // DBTYPE_UI4:
        if (pCell->GetPropertyFlags() & UGCELL_STRING_SET)
        {
            dbVariant.ulVal = (unsigned long)pCell->GetNumber();
            dbVariant.vt = VT_UI4;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adBigInt: // DBTYPE_I8:
        if (pCell->GetPropertyFlags() & UGCELL_STRING_SET)
        {
            dbVariant.intVal = (LONG)pCell->GetNumber();
            dbVariant.vt = VT_I8;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adUnsignedBigInt: // DBTYPE_UI8:
        if (pCell->GetPropertyFlags() & UGCELL_STRING_SET)
        {
            dbVariant.uintVal = (ULONG)pCell->GetNumber();
            dbVariant.vt = VT_UI8;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adSingle: // DBTYPE_R4:
        if (pCell->GetPropertyFlags() & UGCELL_STRING_SET)
        {
            dbVariant.fltVal = (float)pCell->GetNumber();
            dbVariant.vt = VT_R4;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adDouble: // DBTYPE_R8:
        if (pCell->GetPropertyFlags() & UGCELL_STRING_SET)
        {
            dbVariant.dblVal = (double)pCell->GetNumber();
            dbVariant.vt = VT_R8;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adDecimal:
        //dbVariant.decVal = 0;
        //dbVariant.vt = VT_DECIMAL;
        break;
    case adNumeric:
        break;
    case adBoolean: // DBTYPE_BOOL:
        if (pCell->GetPropertyFlags() & UGCELL_STRING_SET)
        {
            if ((BOOL)pCell->GetNumber() == FALSE)
                dbVariant.boolVal = 0;
            else
                dbVariant.boolVal = -1;
            dbVariant.vt = VT_BOOL;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adCurrency: // DBTYPE_CY:
        currency.ParseCurrency(pCell->GetText());
        if (currency.GetStatus() == 0)
        {
            dbVariant.cyVal = currency;
            dbVariant.vt = VT_CY;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adChar:
    case adVarChar:
    case adLongVarChar:
    case adWChar:
    case adVarWChar:
    case adLongVarWChar:
        //case DBTYPE_STR:
        //case DBTYPE_WSTR:
    case adBSTR: // DBTYPE_BSTR:
        cellString = pCell->GetText();
        if (cellString.GetLength() > 0)
        {
            dbVariant.bstrVal = cellString.AllocSysString();
            dbVariant.vt = VT_BSTR;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adDate:  // DBTYPE_DATE:
    case adDBDate:  // DBTYPE_DBDATE:
    case adDBTime:  // DBTYPE_DBTIME:
    case adDBTimeStamp: // DBTYPE_DBTIMESTAMP:
        dateTime.ParseDateTime(pCell->GetText());
        if (dateTime.GetStatus() == 0)
        {
            dbVariant.date = dateTime;
            dbVariant.vt = VT_DATE;
        }
        else
            dbVariant.vt = VT_NULL;
        break;
    case adEmpty: // DBTYPE_EMPTY:
    //case DBTYPE_NULL:
        dbVariant.vt = VT_NULL;
        break;
    case adUserDefined:
    case adVariant:
    case adIDispatch:
    case adIUnknown:
    case adBinary:
    case adVarBinary:
    case adLongVarBinary:
    case adError: //DBTYPE_ERROR:
    default:
        dbVariant.vt = VT_ERROR;
        break;
    }

    return dbVariant;
}

/***************************************************
SetCell
  This function is called everytime user made changes to the
  data.  It is used to make changed permanent.
Params:
  col, row
  cell
Returns:
  <none>
*****************************************************/
int CADOImpDatasource::SetCell(int col, long row, CUGCell* cell)
{
    // variant that will hold new data
    _variant_t variantVal;
    _variant_t saRow;

    if (m_bUseGhostRow == TRUE && row == m_nCurGhostRow)
    { // if needed show new Ghost row
        m_nOldGhostRow = m_nCurGhostRow;
        m_ctrl->SetNumberRows(m_ctrl->GetNumberRows() + 1, FALSE);
        m_nCurGhostRow = m_ctrl->GetNumberRows() - 1;
        // update grid's view
        CRect rect;
        m_ctrl->GetRangeRect(m_ctrl->GetLeftCol(),
            m_ctrl->GetTopRow(),
            m_ctrl->GetRightCol(),
            m_ctrl->GetBottomRow(),
            rect);
        m_ctrl->m_CUGGrid->m_drawHint.ClearHints();
        m_ctrl->m_CUGGrid->m_drawHint.AddHint(m_ctrl->GetLeftCol(),
            m_ctrl->GetTopRow(),
            m_ctrl->GetRightCol(),
            m_ctrl->GetBottomRow());

        // The PaintDrawHintsNow call causes the text to be lost, so we grab a copy of the cell first
        // ( as I presume other data is also lost )
        CUGCell c;
        cell->CopyCellInfo(cell, &c);

        m_ctrl->m_CUGGrid->PaintDrawHintsNow(rect);

        // And copy it back.
        cell->CopyCellInfo(&c, cell);

        m_ctrl->m_CUGGrid->m_drawHint.ClearHints();
        m_ctrl->m_CUGSideHdg->Update();
    }

    variantVal = SetDBCellData(col, cell);

    if (variantVal.vt == VT_ERROR)
    { // an error occured while converting data
        return UG_ERROR;
    }

    m_hUpdatedRow.SetUpdateRowNumber(row);
    m_hUpdatedRow.UpdateCell(col, variantVal);
    // increase number of pending updates
    m_iUpdatesPending++;

    return UG_SUCCESS;
}

/***************************************************
RefreshGrid
  Purpose of this function is to make sure that the grid
  gets fresh data from the database.

  This is simply done by calling FetchNewRows function looking
  for current top row.  Also I determine here if rows were
  added or deleted, so the grid will show all fetchd rows.
Params:
  <none>
Returns:
  UG_SUCCESS on success, this function will never fail.
***************************************************/
int CADOImpDatasource::RefreshGrid(void)
{
    // fetch new rows
    if (FetchNewRows(m_ctrl->GetTopRow()) == UG_ERROR)
    {
        //    return UG_ERROR;
    }

    // update grid's view
    CRect rect;
    m_ctrl->GetRangeRect(m_ctrl->GetLeftCol(),
        m_ctrl->GetTopRow(),
        m_ctrl->GetRightCol(),
        m_ctrl->GetBottomRow(),
        rect);
    m_ctrl->m_CUGGrid->m_drawHint.ClearHints();
    m_ctrl->m_CUGGrid->m_drawHint.AddHint(m_ctrl->GetLeftCol(),
        m_ctrl->GetTopRow(),
        m_ctrl->GetRightCol(),
        m_ctrl->GetBottomRow());
    m_ctrl->m_CUGGrid->PaintDrawHintsNow(rect);
    m_ctrl->m_CUGGrid->m_drawHint.ClearHints();
    m_ctrl->m_CUGSideHdg->Update();

    return UG_SUCCESS;
}

/////////////////////////////////////////////////////////////////
//  OnHitBottom
//    This notification is calle when the last row in the gird is 
//    visible in the view, use this notification to:
//    - check the data base if there are more rows ready for viewing
//    - if necessarry fetch new range of rows.
int CADOImpDatasource::OnHitBottom(long numrows, long rowspast, long* rowsfound)
{
    UNREFERENCED_PARAMETER(rowspast);

    if (m_pcRowsObtained < m_nMaxFetchRows)
    { // the recordset alredy reached the bottom, there is no need to try
      // to fetch additional (new) rows.  They will always be the same
        *rowsfound = 0;

        if (!m_bEditingConfirmed)
            IsRecordsetUpdatable();

        // if the Ghost Row is enabled, but not shown yet ...
        if (m_nCurGhostRow < 0 && m_bUseGhostRow == TRUE)
        {
            m_nCurGhostRow = numrows;
            *rowsfound = 1;
        }
    }
    else if (FetchNewRows(numrows) == UG_SUCCESS)
    { // calculate the actual number of rows found
        *rowsfound = (m_nFetchedStarting + m_pcRowsObtained) - m_ctrl->GetNumberRows();

        if (!m_bEditingConfirmed)
            IsRecordsetUpdatable();

        // if we have reached the bottom of the rowset, add ghost row
        if (m_bUseGhostRow == TRUE && m_pcRowsObtained < m_nMaxFetchRows)
            *rowsfound = *rowsfound + 1;
    }
    else
    { // fetching operation failed
        *rowsfound = 0;
    }

    return UG_SUCCESS;
}

/***************************************************
OnRowChange
  Sent whenever the current row changes
Params:
  oldrow    - row that is losing the focus
  newrow    - row that user moved into
Return:
  UG_NA   not available
  UG_SUCCESS  success
  1...    error codes (data source dependant)
****************************************************/
void CADOImpDatasource::OnRowChange(long oldRow, long newRow)
{
    UNREFERENCED_PARAMETER(newRow);

    // make changes permanent
    if (m_iUpdatesPending > 0)
    {
        long nUpdateRow = oldRow;
        m_ctrl->OnDataSourceNotify(GetID(), UG_ADO_DS_UPDATE_RECORD, (long)&nUpdateRow);

        if (nUpdateRow < 0)
        {
            CancelUpdates();
            // Adjust the current row and edit information
            m_ctrl->m_GI->m_currentRow = oldRow;
            m_ctrl->m_editInProgress = FALSE;
        }
        // set pending updates
        else if (UpdatePending(FALSE) == UG_ERROR)
        {
            // Prompt the user that the data cannot be saved
            // and allow them to continue editing current row or
            // to loose the changes
            CString szErrMessage = _T("Ошибка при обновлении базы данных.\n");
            szErrMessage += m_error;
            szErrMessage += "\n";
            szErrMessage += _T("Вы желаете потерять обновления и продолжить редактировать другую строку?");
            // Allow for the error string to be modified before
            // the message box is displayed.
            m_ctrl->OnDataSourceNotify(GetID(), UG_ADO_DS_UPDATE_ERROR, (long)&szErrMessage);
            // if the error message is empty then assume that the
            // developer does not want to show the message box.
            if (szErrMessage.GetLength() > 0)
            {

                //        if ( 1 || m_ctrl->MessageBox( szErrMessage, _T("Ошибка при обновлении."), MB_YESNO ) == IDYES )
                if (m_ctrl->MessageBox(szErrMessage, _T("Ошибка при обновлении."), MB_YESNO) == IDYES)
                {

                    UpdatePending(FALSE);
                    // clear our the updates list
                    CancelUpdates();
                    // make sure the editing is canceled
                    m_ctrl->m_editInProgress = FALSE;
                    return;
                }
            }

            // reset user's position to the last cell
            m_ctrl->m_GI->m_dragRow = oldRow;
            m_ctrl->m_GI->m_currentRow = oldRow;
            m_ctrl->m_editRow = oldRow;
            m_ctrl->m_GI->m_multiSelect->ClearAll();

            if (m_ctrl->m_editInProgress == FALSE)
                m_ctrl->RedrawRow(newRow);
        }
        else
        { // indicate that all cells were updated
            m_nOldGhostRow = -1;
            // and fully redraw the updated row
            m_ctrl->RedrawRow(nUpdateRow);
        }
    }
    else if (m_bUseGhostRow != TRUE)
    {
        // clear the ghost row information
        m_nOldGhostRow = -1;
        m_nCurGhostRow = -1;
        // remove the additional row
        if (m_hUpdatedRow.GetUpdateRowNumber() >= 0)
        {
            m_hUpdatedRow.SetUpdateRowNumber(-1);
            m_ctrl->SetNumberRows(m_ctrl->GetNumberRows() - 1);
        }
    }
}

/***************************************************
UpdatePending
  function is used to make permanent all of the changes that the
  user has made.
Params:
  <none>
Return:
  UG_SUCCESS  success
  UG_ERROR  if no updates are pending
****************************************************/
int CADOImpDatasource::UpdatePending(BOOL redraw /*= TRUE*/)
{
    if (m_hUpdatedRow.Count() == 0)
        return UG_ERROR;

    _variant_t tempVar;
    int numCols = GetNumCols();

    try
    {
        if (m_hUpdatedRow.GetUpdateRowNumber() == m_nOldGhostRow && m_nOldGhostRow >= 0)
        { // insert new record into the database
            m_pRecordSet->AddNew();
        }
        else
        {
            int nStartingDelta = m_hUpdatedRow.GetUpdateRowNumber() - m_nFetchedStarting;
            // move to proper record in the database
            m_pRecordSet->PutBookmark(m_curBookmark);
            // move current row as calculated
            m_pRecordSet->Move(nStartingDelta);
        }

        tempVar.vt = VT_I4;

        for (int nIndex = 0; nIndex < numCols; nIndex++)
        {
            if (m_hUpdatedRow.IsUpdated(nIndex) && isUpdatable(nIndex))
            {
                tempVar.lVal = nIndex;
                m_pRecordSet->PutCollect(tempVar, m_hUpdatedRow.GetUpdatedVal(nIndex));
            }

            /*
                  if ( m_head && m_hUpdatedRow.IsUpdated( nIndex ))
                  {
                    int r = m_hUpdatedRow.GetUpdateRowNumber();
                    int id = m_head->getRealValue(r, nIndex);
                    if (id > 0) {
                      tempVar.lVal = nIndex;
                      _variant_t tempVar2;
                      tempVar2.vt = VT_I4;
                      tempVar2.lVal = id;
                      m_pRecordSet->PutCollect( tempVar, tempVar2);
                    }
                  }
            */
        }

        if (m_n_keys >= 1) {
            m_pRecordSet->PutCollect(m_keyName, m_keyValue);
        }
        if (m_n_keys >= 2) {
            m_pRecordSet->PutCollect(m_keyName2, m_keyValue2);
        }


        // update the record
        m_pRecordSet->Update();
        // clear our the updated cols list
        m_hUpdatedRow.Empty();
        m_iUpdatesPending = 0;
        // current ghost row does not contain any data
        // at this point so there is not need to store
        // the last ghost row position.
        m_nOldGhostRow = -1;
        if (m_bUseGhostRow != TRUE)
            // clear the ghost row number when not used
            m_nCurGhostRow = -1;

        // reload the memory buffer
        if (m_ctrl->GetSafeHwnd() != NULL)
        {
            FetchNewRows(m_ctrl->GetTopRow());

            if (redraw)
                RefreshGrid();
        }
    }
    catch (_com_error& e)
    {
        //      AfxMessageBox(e.Description(), MB_OK|MB_ICONINFORMATION);
        m_error = (const char*)e.Description();

        ///      m_pRecordSet->CancelUpdate();
        return UG_ERROR;
    }

    catch (...)
    {
        m_pRecordSet->CancelUpdate();

        return UG_ERROR;
    }

    return UG_SUCCESS;
}

/***************************************************
AppendRow
  just let the grid go ahad and create new row, the data source
  will handle the user's imput when necessary.
Params:
  <none>
Return:
  UG_SUCCESS  success
****************************************************/
int CADOImpDatasource::AppendRow()
{
    // check if the recordset supports updates
    if (!m_bEditingConfirmed)
        IsRecordsetUpdatable();

    if (!m_bAllowEdits)
        // make sure that new records are not added
        // when edits are not allowed
        return UG_ERROR;

    // Once it it confirmed that the recordset is updatable
    // than we can check if the recordset allows appends
    try
    {
        _variant_t bSupports = m_pRecordSet->Supports(adAddNew);

        if (bSupports.boolVal == FALSE)
            return UG_ERROR;
    }
    catch (...)
    {
        return UG_ERROR;
    }

    // return success if user decided to use standard way of appending rows
    if (m_bUseSimpleNew == FALSE)
        return UG_SUCCESS;

    // if GhostRow is enabled than make sure that all records are loaded
    // and move the focus rectangle to the first column in the ghost row
    if (m_bUseGhostRow == TRUE)
    {
        if (m_nCurGhostRow < 0)
        {
            m_ctrl->SetNumberRows(GetNumRowsComplete());
            m_ctrl->GotoCell(0, m_ctrl->GetNumberRows() - 1);
        }

        m_ctrl->GotoCell(0, m_nCurGhostRow);

        return UG_NA;
    }

    // In non-ghost row mode, make sure that the grid reflects the full
    // row number in the recordset.
    m_ctrl->SetNumberRows(GetNumRowsComplete() + 1);
    m_ctrl->GotoCell(0, m_ctrl->GetNumberRows() - 1);
    m_hUpdatedRow.SetUpdateRowNumber(m_ctrl->GetNumberRows() - 1);
    m_nCurGhostRow = m_ctrl->GetNumberRows() - 1;
    m_nOldGhostRow = m_ctrl->GetNumberRows() - 1;
    m_ctrl->RedrawRow(m_ctrl->GetNumberRows() - 1);

    return UG_NA;
}

inline void TESTHR(HRESULT x) { if FAILED(x) _com_issue_error(x); };


/***************************************************
DeleteRow
  A virtual function that provides standard interface
  to delete specified row from the data source.
Params:
  row     - indicates the row number to delete
Return:
  UG_NA   not available
  UG_SUCCESS  success
  1...    error codes (data source dependant)
****************************************************/
int CADOImpDatasource::DeleteRow(long row)
{
    if (!m_bAllowDelete) return UG_ERROR;

    if (!m_bEditingConfirmed)
        IsRecordsetUpdatable();

    // make sure that rows cannot be deleted when edits are not allowed
    // 
//    if (!m_bAllowEdits)
//        return UG_ERROR;

    int id = 0;
    CUGCell cell;
    GetCell(0, row, &cell);
    cell.GetNumber(&id);

    CAdoFile* ado = getAdo(m_sDbName);

    if (ado && !isRemovable(ado, m_sTableName_main, id)) {
        return UG_ERROR;
    }

    if (id <= 0) {
        return UG_ERROR;
    }

    CString ss;
    //  AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);
    //  return UG_ERROR;

    ss.Format("Удалить строку № %d в таблице %s ?", id, m_sTableName_main);

    if (AfxMessageBox(ss, MB_YESNO) != IDYES) return UG_ERROR;

    //  Properties *property = m_pRecordSet->GetProperties();
    //  property->GetItem(0);
    //    m_pRecordSet->"Unique Table"

    _variant_t tempVar;

    try
    {
        long nRowUpdated = m_hUpdatedRow.GetUpdateRowNumber();

        if (nRowUpdated == row && nRowUpdated >= 0)
        {
            m_hUpdatedRow.Empty();
            m_iUpdatesPending = 0;
        }

        if (nRowUpdated == m_nOldGhostRow && nRowUpdated >= 0)
        { // delete half populated ghost row, not saved yet
            m_nCurGhostRow = m_ctrl->GetNumberRows() - 1;
            m_nOldGhostRow = -1;
        }
        else
        {
            /**
                        //      int nStartingDelta = row - m_nFetchedStarting;
                              // move to proper record in the database
                        //      m_pRecordSet->PutBookmark( m_curBookmark );
                              // move current row as calculated
                        //      m_pRecordSet->Move( nStartingDelta );
                              // delete record from database
                        //      m_pRecordSet->Delete( adAffectCurrent );

                              // update position of the ghost row
                        if (m_nCurGhostRow == m_ctrl->GetNumberRows() - 1)
                        {
                            m_nCurGhostRow -= 1;
                        }
                    }
                    // the SAFEARRAY of fetched rows needs to be updated
                    // after the record is deleted from the databse.
                    // And the grid's view needs to be updated.
            */

            CString q;
            q.Format("DELETE FROM [%s] WHERE id=%d", m_sTableName_main, id);

//            if (m_sTableName_main == "defect") {
//            }

            m_error = "";

            _CommandPtr pCmdChange = NULL;

            _bstr_t strQ(q);

            TESTHR(pCmdChange.CreateInstance(__uuidof(Command)));
            pCmdChange->ActiveConnection = m_pConnection;
            pCmdChange->CommandText = strQ;

            COleVariant liRecordsAffected;

            pCmdChange->Execute(&liRecordsAffected, NULL, adCmdText);
            long affected = (long)V_I4(&liRecordsAffected);

            //            RefreshGrid();
            Refresh2();
        }
    }

    catch (_com_error& e)
    {
        if (e.Error() == DB_E_CONCURRENCYVIOLATION) {
            if (m_pRecordSet->GetEditMode() == adEditDelete)
                m_pRecordSet->CancelUpdate();

            return ReOpen();
        }
        else {
            AfxMessageBox(e.Description(), MB_OK | MB_ICONINFORMATION);
        }

        try
        {
            if (m_pRecordSet->GetEditMode() == adEditDelete)
                m_pRecordSet->CancelUpdate();
        }
        catch (_com_error& e)
        {
            e;
        }

        return UG_ERROR;
    }

    catch (...)
    {
        if (m_pRecordSet->GetEditMode() == adEditDelete)
            m_pRecordSet->CancelUpdate();

        return UG_ERROR;
    }

    return UG_SUCCESS;
}

/***************************************************
OnEditStart
  This message is sent whenever the grid is ready to
  start editing a cell
Params:
  col, row - location of the cell that edit was requested over
  edit -  pointer to a pointer to the edit control,
      allows for swap of edit control if edit
      control is swapped permanently (for the
      whole grid) is it better to use 'SetNewEditClass'
      function.
Return:
  TRUE - to allow the edit to start
  FALSE - to prevent the edit from starting
****************************************************/
int CADOImpDatasource::OnEditStart(int col, long row, CWnd** edit)
{
    UNREFERENCED_PARAMETER(col);
    UNREFERENCED_PARAMETER(row);
    UNREFERENCED_PARAMETER(edit);

    if (isUpdatable(col))
    {
        if (!m_bEditingConfirmed)
            IsRecordsetUpdatable();

        return m_bAllowEdits;
    }

    return FALSE;
}

/***************************************************
IsRecordsetUpdatable
  function is used to double check if the recordset allows
  editing.
Params:
  <none>
Return:
  TRUE - to allow the edit to start
  FALSE - to prevent the edit from starting
****************************************************/
int CADOImpDatasource::IsRecordsetUpdatable()
{
    if (m_bAllowEdits == TRUE)
    {
        try
        {
            _variant_t bSupports = m_pRecordSet->Supports(adUpdate);

            if (bSupports.boolVal == FALSE)
                m_bAllowEdits = FALSE;
        }
        catch (...)
        {
            m_bAllowEdits = FALSE;
        }
    }

    // if editing is not allowed, than make sure that
    // the ghost row is also disabled.
    if (m_bAllowEdits == FALSE)
        m_bUseGhostRow = FALSE;

    m_bEditingConfirmed = TRUE;

    return m_bAllowEdits;
}

/***************************************************
OnEditVerify
  This notification is sent every time the user hits
  a key while in edit mode.  It is mostly used to create
  custom behavior of the edit contol, because it is
  so eazy to allow or disallow keys hit.
Params:
  col, row  - location of the edit cell
  edit    - pointer to the edit control
  vcKey   - virtual key code of the pressed key
Return:
  TRUE - to accept pressed key
  FALSE - to do not accept the key
****************************************************/
int CADOImpDatasource::OnEditVerify(int col, long row, CWnd* edit, UINT* vcKey)
{
    UNREFERENCED_PARAMETER(col);
    UNREFERENCED_PARAMETER(edit);
    UNREFERENCED_PARAMETER(vcKey);

    if (m_bUseGhostRow == TRUE && row == m_nCurGhostRow)
    { // if needed show new Ghost row
        m_nOldGhostRow = m_nCurGhostRow;
        m_ctrl->SetNumberRows(m_ctrl->GetNumberRows() + 1, FALSE);
        m_nCurGhostRow = m_ctrl->GetNumberRows() - 1;
        m_hUpdatedRow.SetUpdateRowNumber(row);

        // update the grid's view
        CRect rect;
        m_ctrl->GetRangeRect(m_ctrl->GetLeftCol(),
            m_ctrl->GetTopRow(),
            m_ctrl->GetRightCol(),
            m_ctrl->GetBottomRow(),
            rect);
        m_ctrl->m_CUGGrid->m_drawHint.ClearHints();
        m_ctrl->m_CUGGrid->m_drawHint.AddHint(m_ctrl->GetLeftCol(),
            m_ctrl->GetTopRow(),
            m_ctrl->GetRightCol(),
            m_ctrl->GetBottomRow());
        m_ctrl->m_CUGGrid->PaintDrawHintsNow(rect);
        m_ctrl->m_CUGGrid->m_drawHint.ClearHints();
        m_ctrl->m_CUGSideHdg->Update();
    }

    return TRUE;
}

/***************************************************
OnEditFinish
  This notification is sent when the edit is being finised
Params:
  col, row  - coordinates of the edit cell
  edit    - pointer to the edit control
  string    - actual string that user typed in
  cancelFlag  - indicates if the edit is being cancelled
Return:
  TRUE - to allow the edit it proceede
  FALSE - to force the user back to editing of that same cell
****************************************************/
int CADOImpDatasource::OnEditFinish(int col, long row, CWnd* edit, LPCTSTR string, BOOL cancelFlag)
{
    UNREFERENCED_PARAMETER(col);
    UNREFERENCED_PARAMETER(edit);
    UNREFERENCED_PARAMETER(string);
    // if we are editing the last row and the user canceled the edit and this was
    // the only value in the new row, than we should delete the new row.

    if (cancelFlag == TRUE && row == m_nOldGhostRow && m_hUpdatedRow.Count() == 0)
    {
        m_ctrl->SetNumberRows(m_ctrl->GetNumberRows() - 1, FALSE);
        m_nCurGhostRow = m_ctrl->GetNumberRows() - 1;
        m_nOldGhostRow = -1;

        RefreshGrid();
    }

    return TRUE;
}

CString colNameBr(int type_of_net, CString name)
{
    if (type_of_net == 0) {
        name.Format("`%s`", name);
    }
    else {
        name.Format("[%s]", name);
    }
    return name;
}


/***************************************************
SortBy
  A virtual function that provides standard interface
  to sort data in the data source.  This function is
  called when user calls CUGCtrl::SortBy
Params:
  cols    - array of columns to be sorted, in the
          sort order.
  num     - number of elements in the array
  flags   - sort flag identifying the sort direction
          UG_SORT_ASCENDING
          UG_SORT_DESCENDING
Return:
  UG_NA   not available
  UG_SUCCESS  success
  1...    error codes (data source dependant)
****************************************************/
int CADOImpDatasource::SortBy(int* cols, int num, int flags)
{
    // clear previous sort criteria
    m_SortOrder = "";

    CString tempColName;
    // crate new sort (ORDER BY) string
    for (int nIndex = 0; nIndex < num; nIndex++)
    {
        if (m_SortOrder != "")
            m_SortOrder += ", ";

        GetColName(cols[nIndex], &tempColName);

        m_SortOrder += colNameBr(m_type_of_net, tempColName);

        if (flags == UG_SORT_DESCENDING)
            m_SortOrder += " DESC";
    }

    // provide the developer with a chance to perform final adjustments
    // to the ORDER BY string created.
    if (m_ctrl != NULL)
        m_ctrl->OnDataSourceNotify(GetID(), UG_ADO_DS_SORTBY, (long)&m_SortOrder);

    // re-open the recordset using the new sort criteria
    if (Open(NULL, NULL) != UG_ERROR)
    {
        return FetchNewRows(m_ctrl->GetTopRow());
    }
    return UG_ERROR;
}


int CADOImpDatasource::ReOpen()
{
    // re-open the recordset using the new sort criteria
    if (Open(NULL, NULL) != UG_ERROR)
    {
        return FetchNewRows(m_ctrl->GetTopRow());
    }
    return UG_ERROR;
}


/***************************************************
SetOption
  Datasource dependant function. Used to set data source
  specific information and modes of operation
Params:
  option    - integer identifying the option to set
  param1    - option depanded parameter
  param2    - option depanded parameter
Return:
  UG_SUCCESS  success
****************************************************/
int CADOImpDatasource::SetOption(int option, long param1, long param2)
{
    UNREFERENCED_PARAMETER(param2);

    switch (option)
    {

    case UG_ADO_DS_SET_DELETEMODE:
        if (param1 != FALSE)
            param1 = TRUE;

        m_bAllowDelete = param1;
        break;


        /*
          case UG_ADO_DS_SET_EDITMODE:
            if ( param1 != FALSE )
              param1 = TRUE;

            m_bAllowEdits = param1;
            m_bEditingConfirmed = FALSE;
            break;
        */

    case UG_ADO_DS_SET_EDITMODE:
        if (param1 != FALSE)
            param1 = TRUE;

        m_bAllowEdits = param1;
        m_bEditingConfirmed = FALSE;
        break;



    case UG_ADO_DS_USE_GHOSTROW:
        if (param1 != FALSE)
            param1 = TRUE;

        m_bUseGhostRow = param1;
        break;
    case UG_ADO_DS_SET_FETCHSIZE:
        m_nMinFetchRows = param1;
        break;
    case UG_ADO_DS_SET_ROWIDCHAR:
        m_sCurRowID = CString((char)param1);
        break;
    case UG_ADO_DS_USE_SIMPLENEW:
        if (param1 != FALSE)
            param1 = TRUE;

        m_bUseSimpleNew = param1;
        break;
    }
    return UG_SUCCESS;
}

/***************************************************
GetOption
  Datasource dependant function. Used to get data source
  specific information and modes of operation
Params:
  option    - integer identifying the option to set
  param1    - option depanded parameter
  param2    - not used
Return:
  UG_SUCCESS  success
****************************************************/
int CADOImpDatasource::GetOption(int option, long& param1, long& param2)
{
    UNREFERENCED_PARAMETER(param2);

    switch (option)
    {
    case UG_ADO_DS_SET_EDITMODE:
        m_bAllowEdits = (BOOL)param1;
        m_bEditingConfirmed = FALSE;
        break;
    case UG_ADO_DS_USE_GHOSTROW:
        m_bUseGhostRow = (BOOL)param1;
        m_bEditingConfirmed = FALSE;
        break;
    case UG_ADO_DS_SET_FETCHSIZE:
        m_nMinFetchRows = param1;
        break;
    case UG_ADO_DS_SET_ROWIDCHAR:
        param1 = (long)m_sCurRowID.GetAt(0);
        break;
    }
    return UG_SUCCESS;
}

/****************************************************
StartTransaction
  A virtual function that provides standard method
  to start a transaction.  Very important when
  working with databases.
Params:
  <none>
Return:
  UG_SUCCESS  success
  UG_ERROR  error codes (data source dependant)
****************************************************/
int CADOImpDatasource::StartTransaction()
{
    try
    {
        m_pConnection->BeginTrans();
    }
    catch (...)
    {
        return UG_ERROR;
    }

    return UG_SUCCESS;
}

/****************************************************
CancelTransaction
  A virtual function that provides standard method
  to cancel (undo) changes that were made after
  last call to the StartTransaction.
Params:
  <none>
Return:
  UG_SUCCESS  success
  UG_ERROR  error codes (data source dependant)
****************************************************/
int CADOImpDatasource::CancelTransaction()
{
    try
    {
        m_pConnection->RollbackTrans();
    }
    catch (...)
    {
        return UG_ERROR;
    }

    return UG_SUCCESS;
}

/****************************************************
FinishTransaction
  A virtual function that provides standard method
  to make permanent the changes that were made
  after the last call to the StartTransaction.
Params:
  <none>
Return:
  UG_SUCCESS  success
  UG_ERROR  error codes (data source dependant)
****************************************************/
int CADOImpDatasource::FinishTransaction()
{
    try
    {
        m_pConnection->CommitTrans();
    }
    catch (...)
    {
        return UG_ERROR;
    }

    return UG_SUCCESS;
}


/****************************************************
UpdateNow
  function forces current changes to be made permanent
  without waiting for user's action (ie change row).
Params:
  <none>
Return:
  UG_SUCCESS  success
  UG_NA   the updaate command is not necessary,
        there is nothing to update.
  UG_ERROR  error, the update operation failed.
****************************************************/
int CADOImpDatasource::UpdateNow()
{
    if (m_iUpdatesPending <= 0)
        return UG_NA;

    return UpdatePending(FALSE);
}

/****************************************************
CancelUpdates
  function cancels (or un-does) current changes.
  It will delete all entries from the m_hUpdatedRow
  object and it will reset the ghostrow position.
Params:
  <none>
Return:
  UG_SUCCESS  success
  UG_ERROR  error, this function will never fail.
****************************************************/
int CADOImpDatasource::CancelUpdates()
{
    // clear our the updated cols list
    m_hUpdatedRow.Empty();
    m_iUpdatesPending = 0;

    if (m_nOldGhostRow > 0)
    {
        // adjust number of rows in the grid
        m_ctrl->SetNumberRows(m_ctrl->GetNumberRows() - 1, FALSE);
        m_ctrl->m_CUGSideHdg->Update();
        m_nCurGhostRow = m_ctrl->GetNumberRows() - 1;
        m_nOldGhostRow = -1;
    }

    return UG_SUCCESS;
}

/****************************************************
RefreshView
  function forces the datasource to refresh current block
  of rows by reallocating the memory buffer.
Params:
  <none>
Return:
  UG_SUCCESS  success
  UG_ERROR  error, the fetch operation failed
        or the recordset is not open at the moment.
****************************************************/
int CADOImpDatasource::RefreshData()
{
    // make sure the recordset is open
    if (m_pRecordSet == NULL)
        return UG_ERROR;

    m_nCurGhostRow = -1;
    m_nOldGhostRow = -1;

    return FetchNewRows(m_ctrl->GetTopRow());
}


void CADOImpDatasource::SetTableName(CString db, CString tn)
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

int CADOImpDatasource::Refresh2()
{
    // re-open the recordset using the new sort criteria
    if (Open(NULL, NULL) != UG_ERROR)
    {
        int ret = FetchNewRows(m_ctrl->GetTopRow());
        RefreshGrid();
        return ret;
    }

    return UG_ERROR;
}


void CADOImpDatasource::SetKey(CString key_name, int key_value)
{
    m_n_keys = 1;

    m_keyName = COleVariant(key_name);
    m_keyValue = COleVariant((long)key_value);
}


void CADOImpDatasource::SetIdCol(int idCol)
{
    m_idCol = idCol;
}

void CADOImpDatasource::SetKey2(CString key_name1, int key_value1, CString key_name2, int key_value2)
{
    m_n_keys = 2;

    m_keyName = COleVariant(key_name1);
    m_keyValue = COleVariant((long)key_value1);

    m_keyName2 = COleVariant(key_name2);
    m_keyValue2 = COleVariant((long)key_value2);
}



void CADOImpDatasource::SetColorCol(int col)
{
    m_colorCol = col;
}

#include "ado.h"
#include "excel.h"

void ExcelQ1(CAdoFile* ado, CExcel& ex, const char* strQ, const char* tn0, list<int>& list_col, list<CString>& list_col_name);

void CADOImpDatasource::excel(list<int>& list_col)
{
    CExcel ex;

    CAdoFile* ado = getAdo(m_sDbName);

    if (!ado) return;

    //    list<CString> list_col_name;
    //    ado->getFullName1(s);
    list<CString> list_col_name;

    int numCols = GetNumCols();

    list<int>::const_iterator it = list_col.begin();

    int col = 0;


    for (; it != list_col.end(); it++) {
        CString tempStr;
        GetColName(*it, &tempStr);

        if (m_head) {
            tempStr = m_head->getRus(tempStr);

            if (tempStr == "") {
                tempStr = m_head->getText(0, col);
            }
        }
        else {
            tempStr = getFullName1(tempStr);
        }

        col++;

        list_col_name.push_back(tempStr);
    }

    ex.open(NULL);
    if (ex.IsOpen()) {
        CString q = m_q;

        q = GetSQL();

        ExcelQ1(ado, ex, q, m_sTableName, list_col, list_col_name);
    }
}

void CADOImpDatasource::showAllInit(bool all)
{
    m_showAll = all;
}


int CADOImpDatasource::ShowAll(int all)
{
    // clear previous sort criteria

    CString tempColName;

    // provide the developer with a chance to perform final adjustments
    // to the ORDER BY string created.
    if (m_ctrl != NULL)
        m_ctrl->OnDataSourceNotify(GetID(), UG_ADO_DS_SORTBY, (long)&m_SortOrder);

    m_showAll = all;



    // re-open the recordset using the new sort criteria
    if (Open(NULL, NULL) != UG_ERROR)
    {
        //    m_ctrl->SetNumberRows( m_ctrl->GetNumberRows() + 1, FALSE );
        int r = FetchNewRows(m_ctrl->GetTopRow());
        //        if (m_pcRowsObtained > m_ctrl->GetNumberRows()) {
        m_ctrl->SetNumberRows(m_pcRowsObtained, FALSE);
        //        }
        return r;
    }
    return UG_ERROR;
}


int CADOImpDatasource::FindFirst(CString* string, int* col, long* row, long flags)
{
    CString fn;

    if (GetColName(*col, &fn) == UG_ERROR) return UG_NA;

    AfxMessageBox(fn, MB_OK | MB_ICONINFORMATION);

    return UG_NA;
}

int CADOImpDatasource::FindNext(CString* string, int* col, long* row, int flags)
{
    return UG_NA;
}


int CADOImpDatasource::Execute(const char* q)
{
    CString ss;

    _variant_t tempVar;

    try
    {
        m_error = "";

        _CommandPtr pCmdChange = NULL;

        log1(q);

        _bstr_t strQ(q);

        TESTHR(pCmdChange.CreateInstance(__uuidof(Command)));
        pCmdChange->ActiveConnection = m_pConnection;
        pCmdChange->CommandText = strQ;

        COleVariant liRecordsAffected;

        pCmdChange->Execute(&liRecordsAffected, NULL, adCmdText);
        long affected = (long)V_I4(&liRecordsAffected);

        //        RefreshGrid();
        Refresh2();
    }

    catch (_com_error& e)
    {
        _bstr_t bstrSource(e.Source());
        _bstr_t bstrDescription(e.Description());

        CString err;
        err.Format("Code = %08lx\nCode meaning = %s\nSourcre = %s\ntDescription = %s", e.Error(), e.ErrorMessage(), bstr2a(bstrSource), bstr2a(bstrDescription));

        log1(err);
        return UG_ERROR;
    }
    catch (...)
    {
        log1("Неизвестная ошибка");
        return UG_ERROR;
    }


    // In non-ghost row mode, make sure that the grid reflects the full
    // row number in the recordset.
    m_ctrl->SetNumberRows(GetNumRowsComplete() + 1);
    m_ctrl->GotoCell(0, m_ctrl->GetNumberRows() - 1);
    m_hUpdatedRow.SetUpdateRowNumber(m_ctrl->GetNumberRows() - 1);
    m_nCurGhostRow = m_ctrl->GetNumberRows() - 1;
    m_nOldGhostRow = m_ctrl->GetNumberRows() - 1;
    m_ctrl->RedrawRow(m_ctrl->GetNumberRows() - 1);

    return UG_SUCCESS;


}

int CADOImpDatasource::Execute0(const char* q)
{
    CString ss;

    _variant_t tempVar;

    try
    {
        m_error = "";

        _CommandPtr pCmdChange = NULL;

        log1(q);

        _bstr_t strQ(q);

        TESTHR(pCmdChange.CreateInstance(__uuidof(Command)));
        pCmdChange->ActiveConnection = m_pConnection;
        pCmdChange->CommandText = strQ;

        COleVariant liRecordsAffected;

        pCmdChange->Execute(&liRecordsAffected, NULL, adCmdText);
        long affected = (long)V_I4(&liRecordsAffected);

    }

    catch (_com_error& e)
    {
        _bstr_t bstrSource(e.Source());
        _bstr_t bstrDescription(e.Description());

        CString err;
        err.Format("Code = %08lx\nCode meaning = %s\nSourcre = %s\ntDescription = %s", e.Error(), e.ErrorMessage(), bstr2a(bstrSource), bstr2a(bstrDescription));
        log1(err);
        return UG_ERROR;
    }
    catch (...)
    {
        log1("Неизвестная ошибка");
        return UG_ERROR;
    }


    return UG_SUCCESS;


}

