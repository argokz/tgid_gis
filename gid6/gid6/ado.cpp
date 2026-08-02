#include "stdafx.h"

#include <oledb.h>
#include <stdio.h>
#include <conio.h>
#include "icrsint.h"

#include "ado.h"
#include "win.h"

#include "json.h"
#include "crack.h"

CString findTableRusName(CString d, CString n);


HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA);


// Function declarations
inline void TESTHR(HRESULT x) { if FAILED(x) _com_issue_error(x); };

CString get_razm(const char * s)
{
    map<CString, CString> map_razm;

    map_razm["Величина"] = "Razmer_0";
    map_razm["Гидравлическое сопротивление"] = "м*ч^2/т^2";
    map_razm["Давление"] = "м";
    map_razm["Количество теплоты"] = "Гкал";
    map_razm["Коэффициент теплоотдачи(теплопередачи)"] = "ккал/(ч*м^2*Град.С)";
    map_razm["Коэффициент теплопроводности"] = "ккал/(ч*м*Град.С)";
    map_razm["Напор"] = "м.вод.ст";
    map_razm["Нормы тепловых потерь(плотности теплового потока)"] = "ккал/(ч*м)";
    map_razm["Плотность"] = "т/м^3";
    map_razm["Расход"] = "т/ч";
    map_razm["Сигнал"] = "мА";
    map_razm["Суммарный расход"] = "т";
    map_razm["Тепловая мощность"] = "Гкал/ч";
    map_razm["Тепловая производительность"] = "ккал/ч";
    map_razm["Теплоемкость системы"] = "ккал/Град.С";
    map_razm["Удельная теплоемкость"] = "ккал/(кг*Град.С)";
    map_razm["Удельные тепловые характеристики"] = "ккал/(м^3*ч*Град.С)";
    map_razm["Удельный объем воды"] = "м^3*ч/Гкал";
    map_razm["Удельный расход воды"] = "т/Гкал";

    map<CString, CString>::const_iterator it = map_razm.find(CString(s));

    if (it != map_razm.end()) return it->second;

    return "";
}


LPCTSTR strAdoFieldType(short sType)
{
    switch (sType) {
    case adEmpty: return "Empty";
    case adTinyInt: return "TinyInt";
    case adSmallInt: return "SmallInt";
    case adInteger: return "Integer";
    case adBigInt: return "BigInt";
    case adUnsignedTinyInt: return "UnsignedTinyInt";
    case adUnsignedSmallInt: return "UnsignedSmallInt";
    case adUnsignedInt: return "UnsignedInt";
    case adUnsignedBigInt: return "UnsignedBigInt";
    case adSingle: return "Single";
    case adDouble: return "Double";
    case adCurrency: return "Currency";
    case adDecimal: return "Decimal";
    case adNumeric: return "Numeric";
    case adBoolean: return "Boolean";
    case adError: return "Error";
    case adUserDefined: return "UserDefined";
    case adVariant: return "Variant";
    case adIDispatch: return "IDispatch";
    case adIUnknown: return "IUnknown";
    case adGUID: return "GUID";
    case adDate: return "Date";
    case adDBDate: return "DBDate";
    case adDBTime: return "DBTime";
    case adDBTimeStamp: return "DBTimeStamp";
    case adBSTR: return "BSTR";
    case adChar: return "Char";
    case adVarChar: return "VarChar";
    case adLongVarChar: return "LongVarChar";
    case adWChar: return "WChar";
    case adVarWChar: return "VarWChar";
    case adLongVarWChar: return "LongVarWChar";
    case adBinary: return "Binary";
    case adVarBinary: return "VarBinary";
    case adLongVarBinary: return "LongVarBinary";
    case adChapter: return "Chapter";
    case adFileTime: return "FileTime";
    case adPropVariant: return "PropVariant";
    case adVarNumeric: return "VarNumeric";
    case adArray: return "Array";
    }
    return _T("Unknown");
}

CAdoFile::CAdoFile()
{
    nRecs = 0;
    nFlds = 0;
    m_error = "";
    m_connect = "";
    pRecordset = NULL;
    pConnection = NULL;
    m_isOpen = false;
    is_shape_gid = true;

    m_schema = "";

    m_type_of_net = TYPE_OF_NET_MYSQL;

    m_timeout = 30;
    m_timeout = 120;

}


CAdoFile::CAdoFile(CAdoFile *ado)
{
    nRecs = 0;
    nFlds = 0;
    m_error = "";
    m_connect = "";
    pRecordset = NULL;
    pConnection = NULL;
    m_isOpen = false;

    m_schema = "";

    m_type_of_net = ado->m_type_of_net;
}



bool CAdoFile::isOpen()
{
    return m_isOpen;
}


static map<CString, CAdoFile*> map_ado;


CAdoFile *getAdo(const char *database)
{
    map<CString, CAdoFile*>::iterator it = map_ado.find(database);
    if (it != map_ado.end()) {
        return it->second;
    }

    CAdoFile *ado = new CAdoFile();

    int type_of_net;
    CString server, user, password;
    int port;

    getAdoConnectParam(type_of_net, server, port, user, password);

//    CString ss;
//    ss.Format("%s  %s", database, server);
    
    log1("%s  %s", database, server);

    if (ado->openMySQL(type_of_net, server, port, database, user,  password)) {
        return ado;
    }
    else {
        delete ado;
    }


    return NULL;
}

/*
CAdoFile *getAdo()
{
map<CString, CAdoFile*>::iterator it = map_ado.begin();
if (it != map_ado.end()) {
return it->second;
}
return NULL;
}
*/

bool CAdoFile::openMySQL(int type_of_net, const char *server, int port, const char *database, const char *user, const char *password)
{
    CString connect;

    m_type_of_net = type_of_net;

    if (m_type_of_net == TYPE_OF_NET_MYSQL) {
//        CString driver = "MySQL ODBC 5.3 Unicode Driver";
        CString driver = "MySQL ODBC 8.0 Unicode Driver";
        connect.Format("Driver={%s};UID=%s;PASSWORD=%s;PORT=%d;DATABASE=%s;SERVER=%s;OPTION=10;", driver, user, password, port, database, server);
    }
    else {
#if 0
        connect.Format("Provider=SQLOLEDB;Data Source=%s,%d;User ID=%s;Password=%s;Database=%s", server, port, user, password, database);
        connect.Format("Provider=SQLOLEDB;Data Source=%s,%d;User ID=%s;Password=%s;Database=%s;OPTION=2", server, port, user, password, database);
        connect.Format("Provider=SQLNCLI11;Data Source=%s,%d;User ID=%s;Password=%s;Database=%s;OPTION=2", server, port, user, password, database);
        connect.Format("Provider=SQLNCLI11;Data Source=%s,%d;User ID=%s;Password=%s;Database=%s;Connect Timeout=1800", server, port, user, password, database);
        connect.Format("Provider=SQLNCLI11;Data Source=%s,%d;User ID=%s;Password=%s;Database=%s", server, port, user, password, database);

        connect.Format("Provider=SQLNCLI11;Data Source=%s,%d;User ID=%s;Password=%s;Database=%s;Integrated Security=SSPI", server, port, user, password, database);
        connect.Format("Provider=SQLNCLI11;Data Source=%s,%d;User ID=%s;Password=%s;Database=%s", server, port, user, password, database);
#endif


//        CString driver = "SQLNCLI11";
//        CString driver = "SQL Server";
        CString driver = "SQL Server Native Client 11.0";

        connect.Format("Provider=%s;Data Source=%s,%d;User ID=%s;Password=%s;Database=%s", driver, server, port, user, password, database);
        if (password[0] == 0) {
            connect.Format("Provider=%s;Data Source=%s;Database=%s;Trusted_Connection=yes;", driver, server, database);
        }

//        connect += ";Connect Timeout=1800;Query Timeout=120;";
        connect += ";Query Timeout=180";

        CString srv = server;

        if (srv.Find("(localdb)") >= 0) {
//        (localdb)\MyInstance
           connect.Format("Provider=%s;Server=%s;Integrated Security=SSPI;User ID=%s;Password=%s;Database=%s", driver, server, user, password, database);
        }


//        connect.Format("Provider=SQLNCLI11;Server=(localdb)\\MyInstance;Integrated Security=SSPI;User ID=%s;Password=%s;Database=%s", user, password, database);


//ADO::_ConnectionPtr spConnection (__uuidof (ADO::Connection));
//spConnection->Open (L"Provider=SQLNCLI11;Server=(localdb)\\v11.0;Integrated Security=SSPI", L"", L"", 0);


//        connect.Format("Driver={SQL Server};Server=DESKTOP-NH15USB\\SQLEXPRESS;Database=%s;Integrated Security=true;", database);
    }

    //  CString driver = "";
    //  connect.Format("Driver={%s};UID=%s;PASSWORD=%s;PORT=%d;DATABASE=%s;SERVER=%s;", driver, user, password, port, database, server);

    m_user = user;
    m_server = server;
    m_schema = database;

    bool ret = openODBC(connect);

    if (ret) {
        CString name;
        //    name.Format("%s:%d@%s@s", server, port, database, user);
        //    map_ado[name] = this;
        map_ado[database] = this;
    }
    return ret;
}

bool CAdoFile::openMDB(const char *mdb)
{
    CString connect;
    connect.Format("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Persist Security Info=False;", mdb);

    map_ado[mdb] = this;

    return openODBC(connect);
}

void log_sql_error(CString q, CString m_error)
{
    log1("------------------------");
    log1(q);
    log1("------------------------");
    log1(m_error);
    log1("------------------------");
}


bool CAdoFile::openODBC(const char *connect)
{
    log1(connect);

    CString con = connect;
    _bstr_t strCnn(con);

    bool ret = true;

    m_connect = connect;

    m_error = "";

    try {
        // open connection and record set
        TESTHR(pConnection.CreateInstance(__uuidof(Connection)));

        pConnection->Open(strCnn, "", "", adConnectUnspecified);

//pConnection->PutCommandTimeout (1800);
//pConnection->PutConnectionTimeout (1800);


        m_isOpen = true;
    }
    catch (_com_error &e) {
        // Display errors, if any. Pass a connection pointer accessed from the Connection.
        PrintProviderError(pConnection);
        PrintComError(e);
        log1(m_error);
        ret = false;
    }

    return ret;
}

bool CAdoFile::reConnect()
{
    log1("<<reconnect");
    closeConnect();
    bool ret = openODBC(m_connect);
    log1(">>reconnect");

    return ret;
}



bool CAdoFile::closeConnect()
{
    log1("closeConnect");

    map<CString, CAdoFile*>::iterator p = map_ado.find(m_schema);
    if (p != map_ado.end()) map_ado.erase(p);

    if (pConnection)
        if (pConnection->State == adStateOpen)
            pConnection->Close();

    m_isOpen = false;

    return true;
}

bool CAdoFile::openQ(const char *tn, int CursorType, int LockType, int CommandType)
{
    CString q = correctQ(tn);

    log1(q);

    _bstr_t strQ(q);

    bool ret = true;

    unsigned long error = 0;
    bool first = true;
    pRecordsetOld = pRecordset;

    while (true) {

        try {
            m_error = "";
            TESTHR(pRecordset.CreateInstance(__uuidof(Recordset)));

            CursorTypeEnum ct = (CursorTypeEnum)CursorType;
            adoLockTypeEnum lt = (adoLockTypeEnum)LockType;

            pRecordset->Open(strQ, _variant_t((IDispatch *)pConnection, true), ct, lt, CommandType);
            nFlds = pRecordset->Fields->Count;
            ret = true;

            for (int i = 0; i < nFlds; i++) {
                CAdoField af = Field(i);
            }

        }
        catch (_com_error &e) {
            // Display errors, if any. Pass a connection pointer accessed from the Connection.
            PrintProviderError(pConnection);
            PrintComError(e);
            error = e.Error();
            ret = false;
        }

        //    if (error == 0x80004005 && first) {
        if (!ret && first && error != 0x80040e37) {
            log_sql_error(q, m_error);
            reConnect();
            first = false;
        }
        else {
            break;
        }
    }
    return ret;
}

bool CAdoFile::openTable__(const char *tn)
{
    return openQ(tn, adOpenKeyset, adLockOptimistic, adCmdText);
}

bool CAdoFile::openTable0(const char *tn)
{
    return openQ(tn, adOpenForwardOnly, adLockReadOnly, adCmdText);
}

bool CAdoFile::openTableS(const char *tn)
{
    CString q;
    q.Format("SELECT * FROM [%s]", tn);
    return openTable0(q);
}



bool CAdoFile::openSchema()
{
    bool ret = true;

    try {
        m_error = "";
        pRecordset = pConnection->OpenSchema(adSchemaTables);
    }
    catch (_com_error &e) {
        // Display errors, if any. Pass a connection pointer accessed from the Connection.
        PrintProviderError(pConnection);
        PrintComError(e);
        ret = false;
    }

    return ret;
}


CString CAdoFile::correctQ(CString q)
{
    if (m_type_of_net == TYPE_OF_NET_MSSQL) {
      q.Replace("`", "");
    }

    if (m_type_of_net == TYPE_OF_NET_MYSQL) {
      q.Replace("[", "`");
      q.Replace("]", "`");
      q.Replace(".dbo.", ".");
    }


    if (m_type_of_net == TYPE_OF_NET_MSSQL) {
      q.Replace(" IF(", "IIF(");
      q.Replace(",IF(", ",IIF(");
      q.Replace("CREATE TEMPORARY TABLE ", "CREATE TABLE ");
    }

    if (m_type_of_net == TYPE_OF_NET_MSSQL) {
        q.Replace(" ps.", "");
    }
    return q;
}

long CAdoFile::ExecuteInsert_old(const char *q1, long *affected)
{
    int id = 0;

    CString q = correctQ(q1);

    CString last = "SELECT LAST_INSERT_ID()";


    if (m_type_of_net == TYPE_OF_NET_MSSQL) {
        last = "SELECT SCOPE_IDENTITY() AS n";
        if (q.Find("indikator_korrozii") != -1 && q.Find("indikator_korrozii_po_godam") == -1) {
            last.Format("SELECT IDENT_CURRENT('indikator_korrozii')");
        }
    }

    if (Execute(q, affected)) {
        if (openTable0(last)) {
            if (!isEOF()) {
                id = read_long(0);
            }
            closeTable();
        }
    }
    return id;
}

#if 0


long CAdoFile::ExecuteInsert(const char* q1, long* affected)
{
    int id = 0;

    CString last = "SELECT LAST_INSERT_ID()";

    if (m_type_of_net == TYPE_OF_NET_MSSQL) {
        last = "SELECT SCOPE_IDENTITY()";
    }

    CString q = correctQ(q1);

    if (Execute(q, affected)) {
        if (openTable0(last)) {
            if (!isEOF()) {
                id = read_long(0);
            }
            closeTable();
        }
    }
    return id;
}

#endif

long CAdoFile::ExecuteInsert(const char *_q1, long *affected)
{
    CString q = correctQ(_q1);

    return ExecuteInsert_old(_q1, affected);

/**

    CString q1 = _q1;

    long id = -1;

    if (q1.Find(" VALUES (") == -1) return ExecuteInsert_old(_q1, affected);

    q1.Replace(" VALUES (", " OUTPUT Inserted.ID VALUES (");

    if (Execute(q1, affected)) {
        if (!isEOF()) {
            id = read_long(0);
        }
        closeTable();
    }

    return id;
*/

}


bool CAdoFile::Execute(const char *q1)
{
  long affected;

  return Execute(q1, &affected);
}

long CAdoFile::GetCommandTimeout()
{
    _CommandPtr pCmdChange = NULL;

    TESTHR(pCmdChange.CreateInstance(__uuidof(Command)));

    return pCmdChange->GetCommandTimeout();
}


bool CAdoFile::PutCommandTimeout(long pl)
{
    _CommandPtr pCmdChange = NULL;

    TESTHR(pCmdChange.CreateInstance(__uuidof(Command)));

//    pCmdChange->PutCommandTimeout(pl);

    return true;
}


#if 1

bool CAdoFile::Execute(const char *q1, long *affected, CommandTypeEnum cmd)
{
    CString q = correctQ(q1);
    
    //  q.MakeUpper();

    _bstr_t strQ(q);

    bool ret = true;

    *affected = 0;

    if (pConnection->State != adStateOpen) {
        return false;
    }

    //  PrintProviderError(pConnection);
    //  AfxMessageBox(m_error);

    unsigned long error = 0;
    bool first = true;

//    pRecordsetOld = pRecordset;

    while (true) {

        try {
            //    TESTHR(pRecordset.CreateInstance(__uuidof(Recordset)));
            //    pRecordset->Open(strQ, _variant_t((IDispatch *)pConnection,true), adOpenKeyset, adLockOptimistic, adCmdText);

            m_error = "";

            _CommandPtr pCmdChange = NULL;

            TESTHR(pCmdChange.CreateInstance(__uuidof(Command)));
            pCmdChange->ActiveConnection = pConnection;
            pCmdChange->CommandText = strQ;

            if (m_timeout > 0) {
//                pCmdChange->PutCommandTimeout(m_timeout);
                pCmdChange->PutCommandTimeout(1800);
            }

            COleVariant liRecordsAffected;

//            pRecordset = 
            pCmdChange->Execute(&liRecordsAffected, NULL, cmd);

            *affected = (long)V_I4(&liRecordsAffected);
            ret = true;
        }
        catch (_com_error &e) {
            // Display errors, if any. Pass a connection pointer accessed from the Connection.
            PrintProviderError(pConnection);
            PrintComError(e);
            error = e.Error();
            ret = false;
        }

        if (error == 0x80004005 && first) {
            log_sql_error(q, m_error);
            reConnect();
            first = false;
        }
        else {
            break;
        }
    }

    return ret;
}

#else

bool CAdoFile::Execute(const char* q1, long* affected, CommandTypeEnum cmd)
{
    CString q = correctQ(q1);

    //  q.MakeUpper();

    _bstr_t strQ(q);

    bool ret = true;

    *affected = 0;

    if (pConnection->State != adStateOpen) {
        return false;
    }

    //  PrintProviderError(pConnection);
    //  AfxMessageBox(m_error);

    unsigned long error = 0;
    bool first = true;

    while (true) {

        try {
            //    TESTHR(pRecordset.CreateInstance(__uuidof(Recordset)));
            //    pRecordset->Open(strQ, _variant_t((IDispatch *)pConnection,true), adOpenKeyset, adLockOptimistic, adCmdText);

            m_error = "";

            _CommandPtr pCmdChange = NULL;

            TESTHR(pCmdChange.CreateInstance(__uuidof(Command)));
            pCmdChange->ActiveConnection = pConnection;
            pCmdChange->CommandText = strQ;

            if (m_timeout > 0) {
                //                pCmdChange->PutCommandTimeout(m_timeout);
                pCmdChange->PutCommandTimeout(1800);
            }

            COleVariant liRecordsAffected;

            pCmdChange->Execute(&liRecordsAffected, NULL, cmd);
            *affected = (long)V_I4(&liRecordsAffected);
            ret = true;
        }
        catch (_com_error& e) {
            // Display errors, if any. Pass a connection pointer accessed from the Connection.
            PrintProviderError(pConnection);
            PrintComError(e);
            error = e.Error();
            ret = false;
        }

        if (error == 0x80004005 && first) {
            log_sql_error(q, m_error);
            reConnect();
            first = false;
        }
        else {
            break;
        }
    }

    return ret;
}

#endif 


long CAdoFile::count()
{
    return 0;
}

bool CAdoFile::isEOF()
{
    if (pRecordset)
        if (pRecordset->State == adStateOpen)
            return pRecordset->adoEOF != 0;

    return true;
}

CString CAdoFile::readStr(int id)
{
    COleVariant var = read(id);

#if 0
    CString strRet;

    if (var.vt == VT_R4 || var.vt == VT_R8) {

        CAdoField af = Field(id);
        int width = af.Precision;
        switch (var.vt) {
        case VT_R4:
            strRet.Format(_T("%.*g"), width, (double)V_R4(&var));
            return strRet;
        case VT_R8:
            strRet.Format(_T("%.*lg"), width, V_R8(&var));
            return strRet;
        }
    }
#endif

//    CString s = CCrack::strVARIANT2(var);
    CString s = CCrack::strVARIANT_UTF(var);
    return s;
}

CString CAdoFile::readStr(const char *fn)
{
    COleVariant var = read(fn);
    CString strRet;

    int width = 7;

    switch (var.vt) {
    case VT_R4:
    // "Why Floating-Point Numbers May Lose Precision" MSDN
        {
            double d = ((double)((int)(var.fltVal * 10000))) / 10000;
            strRet.Format(_T("%.*g"), width, d);
        }
        return strRet;
    case VT_R8:
        strRet.Format(_T("%.*lg"), width, V_R8(&var));
        return strRet;
    }

    return CCrack::strVARIANT2(var);
}


CString CAdoFile::readStrDateJSON(const char *fn)
{
    COleVariant var = read(fn);

    if (var.vt == VT_DATE) {
        CString s;
        COleDateTime dt = read_date(fn);
        s = dt.Format(_T("%Y-%m-%d"));
        return s;
    }
    
    return readStr(fn);
}



long to_long(const COleVariant &var)
{
    if (var.vt == VT_NULL) return 0L;
    else if (var.vt == VT_I4) return (long) V_I4(&var);
    else if (var.vt == VT_R4) return (long) V_R4(&var);
    else if (var.vt == VT_R8) return (long) V_R8(&var);
    else if (var.vt == VT_I2) return (long) V_I2(&var);
    else if (var.vt == VT_BOOL) return (long) V_BOOL(&var);
    else if (var.vt == VT_DECIMAL) {
        DECIMAL dc = var.decVal;

        if (dc.scale != 0) return 0;

        long l = (long)dc.Lo32;

        if (dc.sign) l = -l;

        return l;
    }
    return 0L;
}

COleDateTime CAdoFile::read_date(const char *fn)
{
    COleVariant var = read(fn);

    if (var.vt == VT_NULL) {
        VARIANT v = {};
        v.vt = VT_NULL;
        COleDateTime vt2(v);
        return vt2;
    }

    return V_DATE(&var);
}

COleDateTime CAdoFile::read_date(int id)
{
    COleVariant var = read(id);

    if (var.vt == VT_NULL) {
        VARIANT v = {};
        v.vt = VT_NULL;
        COleDateTime vt2(v);
        return vt2;
    }

    return V_DATE(&var);
}

long CAdoFile::read_long(const char *fn)
{
    COleVariant var = read(fn);
    return to_long(var);
}

long CAdoFile::read_long(int id)
{
    COleVariant var = read(id);
    return to_long(var);
}

double to_double(COleVariant &var)
{
    if (var.vt == VT_NULL) return 0L;

    if (var.vt == VT_I4) return (double)V_I4(&var);
    if (var.vt == VT_I2) return (double)V_I2(&var);

    if (var.vt == VT_R4) return (double)V_R4(&var);
    if (var.vt == VT_R8) return (double)V_R8(&var);

    return 0L;
}


double CAdoFile::read_double(const char *fn)
{
    COleVariant var = read(fn);
    return to_double(var);
}


double CAdoFile::read_double_null(const char *fn, double val0)
{
    COleVariant var = read(fn);

    if (var.vt == VT_NULL) return val0;

    return to_double(var);
}



double CAdoFile::read_double(int id)
{
    COleVariant var = read(id);
    return to_double(var);
}


COleVariant CAdoFile::read(int id)
{
    COleVariant var;

    try {
        if (pRecordset) {
            if (pRecordset->State == adStateOpen) {
                _bstr_t bstrVal;
                VARIANT vIntegerType;
                vIntegerType.vt = VT_I2; //Variant type for Integer
                vIntegerType.intVal = id;

                FieldPtr pFld = pRecordset->Fields->GetItem(&vIntegerType);
                var = pFld->GetValue();
            }
        }
    }
    catch (_com_error & e) {
        PrintProviderError(pConnection);
        PrintComError(e);
    }
    catch (...) {
    }
    return var;
}

COleVariant CAdoFile::read(const char *fn)
{
    COleVariant var;

    if (pRecordset) {
        if (pRecordset->State == adStateOpen) {
            //      CString f;
            //      f.Format("[%s]", fn);
            //      COleVariant v = COleVariant(f, VT_BSTRT);

            try {
                m_error = "";
                FieldPtr pFld = pRecordset->Fields->GetItem(fn);
                var = pFld->GetValue();
            }
            catch (_com_error &e) {
                PrintProviderError(pConnection);
                PrintComError(e);
            }
        }
    }
    return var;
}

CString CAdoFile::FieldName(int i)
{
    CString s = "";

    CAdoField fa = Field(i);

    if (fa.yes) {
        s = Field(i).name;
    }

    return s;
}

CAdoField GetField(FieldPtr &pFld)
{
    CAdoField field;

    _bstr_t s = pFld->GetName();

    field.name = (LPCTSTR)s;
    field.title = (LPCTSTR)s;
    field.f1 = (LPCTSTR)s;


    field.typ = pFld->GetType();
    field.DefinedSize = pFld->GetDefinedSize();
    field.Precision = pFld->GetPrecision();
    field.NumericScale = pFld->GetNumericScale();

    long attr = pFld->GetAttributes();

    //      DataFormat dataformat = pFld->GetDataFormat ();

    //IUnknownPtr  = pFld->GetDataFormat ();
    //COleVariant v1 = pFld->GetChunk ( long Length );
    //  long l1 = pFld->GetActualSize ();
    //  long l2 = pFld->GetAttributes ();

    //COleVariant v2 = pFld->GetOriginalValue ();
    //COleVariant v3 = pFld->GetUnderlyingValue ();
    //COleVariant v4 = pFld->GetValue ();

    field.yes = true;

    return field;
}



CAdoField CAdoFile::Field(int i)
{
    CString ss;
    CAdoField field;

    field.yes = false;

    if (pRecordset) {
        if (pRecordset->State == adStateOpen) {
            try {
                m_error = "";
                VARIANT vIntegerType;
                vIntegerType.vt = VT_I2; //Variant type for Integer
                vIntegerType.intVal = i;

                FieldPtr pFld = pRecordset->Fields->GetItem(&vIntegerType);
                field = GetField(pFld);
            }
            catch (_com_error &e) {
                PrintProviderError(pConnection);
                PrintComError(e);
                field.yes = false;
            }
        }
    }
    return field;
}

CAdoField CAdoFile::Field(const char *fn)
{
    CString ss;
    CAdoField field;

    field.yes = false;

    if (pRecordset) {
        if (pRecordset->State == adStateOpen) {
            FieldPtr pFld = pRecordset->Fields->GetItem(fn);
            field = GetField(pFld);
        }
    }
    return field;
}


bool CAdoFile::closeTable()
{
    if (pRecordset)
        if (pRecordset->State == adStateOpen)
            pRecordset->Close();

    pRecordset = pRecordsetOld;

    return true;
}

void CAdoFile::MoveNext()
{
    if (pRecordset)
        if (pRecordset->State == adStateOpen)
            pRecordset->MoveNext();

}

void OpenList(CAdoFile *ado, const char *q, list<COleVariant> & lst)
{
    lst.clear();

    if (!ado || !ado->openTable0(q)) return;

    while (!ado->isEOF()) {
        COleVariant var = ado->read(0);
        lst.push_back(var);
        ado->MoveNext();
    }
    ado->closeTable();
}

void OpenList2(CAdoFile *ado, const char *q, list<COleVariant> & lst)
{
    lst.clear();

    if (!ado || !ado->openTable0(q)) return;

    while (!ado->isEOF()) {
        COleVariant var = ado->read(0);
        lst.push_back(var);
        ado->MoveNext();
    }
    ado->closeTable();
}


void OpenList2(CAdoFile *ado, const char *q, list< vector<COleVariant> > & lst)
{
    lst.clear();

    if (!ado || !ado->openTable0(q)) return;

    while (!ado->isEOF()) {
        int nf = ado->NFlds();

        vector<COleVariant> v(nf);

        for (long i = 0; i < nf; i++) {
            v[i] = ado->read(i);
        }

        lst.push_back(v);

        ado->MoveNext();
    }
    ado->closeTable();
}

int OpenX(CAdoFile *ado, const char *q, map<CString, COleVariant> & lst, list<CAdoField> & lstFld)
{
    lstFld.clear();
    lst.clear();

    if (!ado || !ado->openTable0(q)) return false;

    int n = 0;

    if (!ado->isEOF()) {
        for (long i = 0; i < ado->NFlds(); i++) {
            COleVariant var = ado->read(i);
            CAdoField f = ado->Field(i);
            lst[f.name] = var;
            lstFld.push_back(f);
        }
        n++;
    }
    ado->closeTable();

    return n;
}

int OpenX2(CAdoFile *ado, const char *q, map<int, map<CString, COleVariant> > & lst, list<CAdoField> & lstFld)
{
    lstFld.clear();
    lst.clear();

    if (!ado || !ado->openTable0(q)) return 0;

    int n = 0;

    if (!ado->isEOF()) {
        for (long i = 0; i < ado->NFlds(); i++) {
            COleVariant var = ado->read(i);
            CAdoField f = ado->Field(i);
            lst[n][f.name] = var;
            if (n == 0) lstFld.push_back(f);
        }
        n++;
    }

    ado->closeTable();

    return n;
}




CString OpenXJSon(CAdoFile *ado, const char *q)
{
    map<CString, COleVariant> lst;
    list<CAdoField> lstFld;

    if (!ado || !ado->openTable0(q)) return "";

    JSON json;

    json.add("[");

    while (!ado->isEOF()) {
        json.add("[");

        int nf = ado->NFlds();

        for (long i = 0; i < nf; i++) {
            COleVariant var = ado->read(i);
            CAdoField f = ado->Field(i);

            //      json.add(f.name, CCrack::strVARIANT2(var));
            json.add1(CCrack::strVARIANT2(var));
        }
        json.add("]");

        ado->MoveNext();
    }

    json.add("]");

    ado->closeTable();

    return json.str();
}


CString OpenXJSon1(CAdoFile *ado, const char *q)
{
    map<CString, COleVariant> lst;
    list<CAdoField> lstFld;


    if (!ado || !ado->openTable0(q)) return "";

    JSON json;

    json.add("[");

    int nf = ado->NFlds();

    for (long i = 0; i < nf; i++) {
        COleVariant var = ado->read(i);
        CAdoField f = ado->Field(i);

        json.add("{");
        json.add("name", f.name);
        json.add("typ", strAdoFieldType(f.typ));
        json.add("DefinedSize", f.DefinedSize);
        json.add("}");
    }
    json.add("]");


    ado->closeTable();

    return json.str();
}


CString OpenXJSon2(CAdoFile *ado, const char *q)
{
    map<CString, COleVariant> lst;
    list<CAdoField> lstFld;

    if (!ado || !ado->openTable0(q)) return "";

    JSON json;

    json.add("\"fields\" : {");
    json.add("[");

    int nf = ado->NFlds();

    for (long i = 0; i < nf; i++) {
        COleVariant var = ado->read(i);
        CAdoField f = ado->Field(i);

        json.add("{");
        json.add("name", f.name);
        json.add("typ", strAdoFieldType(f.typ));
        json.add("DefinedSize", f.DefinedSize);
        json.add("}");
    }
    json.add("]");
    json.add("}");



    json.add("\"out\" : {");
    json.add("[");

    while (!ado->isEOF()) {
        json.add("[");

        int nf = ado->NFlds();

        for (long i = 0; i < nf; i++) {
            COleVariant var = ado->read(i);
            CAdoField f = ado->Field(i);

            //      json.add(f.name, CCrack::strVARIANT2(var));
            json.add1(CCrack::strVARIANT2(var));
        }
        json.add("]");

        ado->MoveNext();
    }

    json.add("]");
    json.add("}");


    ado->closeTable();

    return json.str();
}


CString bstr2a(_bstr_t b)
{
    char *buf;
    UnicodeToAnsi(b, &buf);
    CString s = buf;
    CoTaskMemFree(buf);

    return s;
}


void CAdoFile::PrintProviderError(_ConnectionPtr pConnection)
{
    return;
    // Print Provider Errors from Connection object.
    // pErr is a record object in the Connection's Error collection.
    ErrorPtr pErr = NULL;

    CString s = "";
    m_error = "";

    if ((pConnection->Errors->Count) > 0) {
        long nCount = pConnection->Errors->Count;
        // Collection ranges from 0 to nCount -1.
        for (long i = 0; i < nCount; i++) {
            pErr = pConnection->Errors->GetItem(i);
            CString ss;

            CString msg = bstr2a(pErr->Description);
            ss.Format("Error number: %x %s\n", pErr->Number, msg);
            s += ss;
        }
        m_error = s;
    }
    //   AfxMessageBox(s);
}

void CAdoFile::PrintComError(_com_error &e) {
    _bstr_t bstrSource(e.Source());
    _bstr_t bstrDescription(e.Description());

    // Print COM errors. 
    //   printf("Error\n");
    //   printf("\tCode = %08lx\n", e.Error());
    //   printf("\tCode meaning = %s\n", e.ErrorMessage());
    //   printf("\tSource = %s\n", (LPCSTR) bstrSource);
    //   printf("\tDescription = %s\n", (LPCSTR) bstrDescription);

    //   m_error = (LPCSTR) bstrDescription;

    CString err;
    err.Format("Code = %08lx\nCode meaning = %s\nSourcre = %s\ntDescription = %s", e.Error(), e.ErrorMessage(), bstr2a(bstrSource), bstr2a(bstrDescription));
    m_error = err;

    //   m_error = err;
    //   AfxMessageBox(m_error);
}

#include "coordlis.h"


void ShapeToCl(COleVariant &var, CCoordList& cl);

CString OpenXJSonCoord1(CAdoFile *ado, const char *tn, CFRect rect, double dmas)
{
    map<CString, COleVariant> lst;
    list<CAdoField> lstFld;

    CString q;
    q.Format("SELECT ID, SHAPE FROM [%s]", tn);

    if (!ado || !ado->openTable0(q)) return "";

    JSON json;

    json.add("[");

    while (!ado->isEOF()) {
        json.add("[");

        COleVariant var = ado->read(0);
        json.add1(CCrack::strVARIANT2(var));

        var = ado->read(1);

        CCoordList cl;

        ShapeToCl(var, cl);

        CFPoint pt = cl.first();

        pt.x = (pt.x - rect.left)*dmas / 1000000;
        pt.y = (pt.y - rect.top)*dmas / 1000000;

        json.add1(pt.x);
        json.add1(pt.y);
        json.add("]");

        ado->MoveNext();
    }

    json.add("]");



    ado->closeTable();

    return json.str();
}


COleVariant getV(const map<CString, COleVariant> & lst, const char *fn)
{
    map<CString, COleVariant>::const_iterator it = lst.find(fn);
    if (it != lst.end()) {
        return it->second;
    }

    COleVariant v;

    return v;
}


CString getVstr(const map<CString, COleVariant> & lst, const char *fn)
{
    map<CString, COleVariant>::const_iterator it = lst.find(fn);
    if (it != lst.end()) {
        return CCrack::strVARIANT2(it->second);
    }
    return "";
}


CString getVstr(const map<int, map<CString, COleVariant> > & lst, int n, const char *fn)
{
    map<int, map<CString, COleVariant> >::const_iterator it = lst.find(n);

    if (it != lst.end()) {
        return getVstr(it->second, fn);
    }
    return "";
}

void splitTableName(const CString schema0, const CString tn0, CString &schema, CString &tn)
{
  schema = schema0;
  tn = tn0;
//  if (m_type_of_net != TYPE_OF_NET_MYSQL) return;


  int n = tn0.Find(".");

  if (n == -1) return;

  schema = tn0.Left(n);
  tn = tn0.Mid(n+1);
}


void CAdoFile::MySQLTableInfo(const char *table)
{
    MySQLTableInfo(this->m_schema, table);
}

void CAdoFile::MySQLTableInfo(const char *schema, const char *table)
{
    CString q;

    if (CString(table) == "Act") {
        int qq;
        qq = 1;
    }
    if (CString(table) == "?") {
        return;
    }


    if (!isOpen()) return;
    if (isMySQL_OK(schema, table)) return;

    CString tt = table;
    tt.MakeLower();

    CString cx = schema;
    cx.MakeLower();

    MySQLColumns col;
    map_mysql_columns[cx][tt]["DSDSFDGFDSFGDFHGDF"] = col;

    //SELECT * FROM `COLUMNS` WHERE TABLE_NAME='nodes' AND TABLE_SCHEMA ='LifanDB3'
    //SELECT * FROM KEY_COLUMN_USAGE WHERE TABLE_NAME='nodes' AND TABLE_SCHEMA ='LifanDB3'
    //SELECT * FROM REFERENTIAL_CONSTRAINTS WHERE TABLE_NAME='nodes' AND UNIQUE_CONSTRAINT_SCHEMA ='LifanDB3'
    //SELECT * FROM TABLE_CONSTRAINTS WHERE TABLE_NAME='nodes' AND TABLE_SCHEMA ='LifanDB3'
    //REFERENTIAL_CONSTRAINTS
    //  q.Format("SELECT * FROM information_schema.TABLES WHERE TABLE_NAME=`%s` AND TABLE_SCHEMA =%s", table, schema);

    q.Format("SELECT * FROM information_schema.TABLES WHERE TABLE_NAME='%s' AND TABLE_SCHEMA='%s'", table, schema);
    if (m_type_of_net == TYPE_OF_NET_MSSQL) {
        q.Format("SELECT * FROM information_schema.TABLES WHERE TABLE_NAME='%s'", table);
    }

    if (this->openTable0(q)) {
        while (!this->isEOF()) {
            MySQLTables tab;           
                        
            tab.strTABLE_CATALOG = this->readStr("TABLE_CATALOG");
            tab.strTABLE_SCHEMA = this->readStr("TABLE_SCHEMA");
            tab.strTABLE_NAME = this->readStr("TABLE_NAME");
            tab.strTABLE_TYPE = this->readStr("TABLE_TYPE");
//            tab.strENGINE = this->readStr("ENGINE");
//            tab.lVERSION = this->read_long("VERSION");
//            tab.strROW_FORMAT = this->readStr("ROW_FORMAT");
//            tab.lTABLE_ROWS = this->read_long("TABLE_ROWS");
//            tab.lAVG_ROW_LENGTH = this->read_long("AVG_ROW_LENGTH");
//            tab.lDATA_LENGTH = this->read_long("DATA_LENGTH");
//            tab.lMAX_DATA_LENGTH = this->read_long("MAX_DATA_LENGTH");
//            tab.lINDEX_LENGTH = this->read_long("INDEX_LENGTH");
//            tab.lDATA_FREE = this->read_long("DATA_FREE");
//            tab.lAUTO_INCREMENT = this->read_long("AUTO_INCREMENT");
//            tab.dtCREATE_TIME = this->read_date("CREATE_TIME");
//            tab.dtUPDATE_TIME = this->read_date("UPDATE_TIME");
//            tab.dtCHECK_TIME = this->read_date("CHECK_TIME");
//            tab.strTABLE_COLLATION = this->readStr("TABLE_COLLATION");
//            tab.lCHECKSUM = this->read_long("CHECKSUM");
//            tab.strCREATE_OPTIONS = this->readStr("CREATE_OPTIONS");
//            tab.strTABLE_COMMENT = this->readStr("TABLE_COMMENT");
            tab.strTABLE_COMMENT = "";

//            if (tab.strTABLE_NAME == "tkamera")  {
            if (tab.strTABLE_NAME.Left(1) == "0") {
                    int qqq;
                qqq = 1;
            }


            if (tab.strTABLE_COMMENT == "") {
              tab.strTABLE_COMMENT = findTableRusName(schema, tab.strTABLE_NAME);
            }


//            CString schema = tab.strTABLE_SCHEMA;
            CString schema1 = schema;
            CString table_name = tab.strTABLE_NAME;

            schema1.MakeLower();
            table_name.MakeLower();

            map_mysql2_tables[schema1][table_name] = tab;

            this->MoveNext();
        }
        this->closeTable();
    }
    
    q.Format("SELECT * FROM information_schema.COLUMNS WHERE TABLE_NAME='%s' AND TABLE_SCHEMA='%s'", table, schema);

    if (m_type_of_net == TYPE_OF_NET_MSSQL) {
        q.Format("SELECT * FROM information_schema.COLUMNS WHERE TABLE_NAME='%s'", table);
    }

    if (this->openTable0(q)) {
        while (!this->isEOF()) {
            MySQLColumns col;

            col.strTABLE_CATALOG = this->readStr("TABLE_CATALOG");
            col.strTABLE_SCHEMA = this->readStr("TABLE_SCHEMA");
            col.strTABLE_NAME = this->readStr("TABLE_NAME");
            col.strCOLUMN_NAME = this->readStr("COLUMN_NAME");
            col.lORDINAL_POSITION = this->read_long("ORDINAL_POSITION");
            col.strCOLUMN_DEFAULT = this->readStr("COLUMN_DEFAULT");
            col.strIS_NULLABLE = this->readStr("IS_NULLABLE");
            col.strDATA_TYPE = this->readStr("DATA_TYPE");
            col.lCHARACTER_MAXIMUM_LENGTH = this->read_long("CHARACTER_MAXIMUM_LENGTH");
            col.lCHARACTER_OCTET_LENGTH = this->read_long("CHARACTER_OCTET_LENGTH");
            col.lNUMERIC_PRECISION = this->read_long("NUMERIC_PRECISION");
            col.lNUMERIC_SCALE = this->read_long("NUMERIC_SCALE");
            col.lDATETIME_PRECISION = this->read_long("DATETIME_PRECISION");
            col.strCHARACTER_SET_NAME = this->readStr("CHARACTER_SET_NAME");
            col.strCOLLATION_NAME = this->readStr("COLLATION_NAME");
//            col.strCOLUMN_TYPE = this->readStr("COLUMN_TYPE");
//            col.strCOLUMN_KEY = this->readStr("COLUMN_KEY");
//            col.strEXTRA = this->readStr("EXTRA");
//            col.strPRIVILEGES = this->readStr("PRIVILEGES");
//            col.strCOLUMN_COMMENT = this->readStr("COLUMN_COMMENT");


            col.strCOLUMN_COMMENT = "";
            col.name_full = "";

            if (col.strCOLUMN_COMMENT == "") {
              const CColName *col_name = findColumnRusName(schema, table, col.strCOLUMN_NAME);
              if (col_name) {
                  col.strCOLUMN_COMMENT = col_name->name;
                  col.name_full = col_name->name_full;
              }
            }

            col.strREFERENCED_TABLE_SCHEMA = "";
            col.strREFERENCED_TABLE_NAME = "";
            col.strREFERENCED_COLUMN_NAME = "";

//            CString schema = col.strTABLE_SCHEMA;
            CString schema1 = schema;
            CString table_name = col.strTABLE_NAME;
            CString column_name = col.strCOLUMN_NAME;

            schema1.MakeLower();
            table_name.MakeLower();
            column_name.MakeLower();

            map_mysql_columns[schema1][table_name][column_name] = col;
            map_mysql_columnsOrd[schema1][table_name][col.lORDINAL_POSITION] = col;

            this->MoveNext();
        }
        this->closeTable();
    }

    if (m_type_of_net == 0) {

        q.Format("SELECT * FROM information_schema.KEY_COLUMN_USAGE WHERE TABLE_NAME='%s' AND TABLE_SCHEMA ='%s'", table, schema);

        map<CString, map<CString, map<CString, MySQLColumns> > > map_mysql_columns;

        if (0 && this->openTable0(q) ) {
            while (!this->isEOF()) {
                CString schema = this->readStr("TABLE_SCHEMA");
                CString table_name = this->readStr("TABLE_NAME");
                CString column_name = this->readStr("COLUMN_NAME");

                CString strCONSTRAINT_NAME = this->readStr("CONSTRAINT_NAME");
                CString strCOLUMN_NAME = this->readStr("COLUMN_NAME");

                if (1 || strCONSTRAINT_NAME.Find("_LOOKUP") != -1) {
                    schema.MakeLower();
                    table_name.MakeLower();
                    column_name.MakeLower();

                    MySQLColumns *col = findMySQLColumnsS(schema, table_name, column_name);

                    if (col) {
                        col->strREFERENCED_COLUMN_NAME = this->readStr("REFERENCED_COLUMN_NAME");
                        col->strREFERENCED_TABLE_NAME = this->readStr("REFERENCED_TABLE_NAME");
                        col->strREFERENCED_TABLE_SCHEMA = this->readStr("REFERENCED_TABLE_SCHEMA");
                    }

                    col = findMySQLColumnsOrdS(schema, table_name, col->lORDINAL_POSITION);
                    if (col) {
                        col->strREFERENCED_COLUMN_NAME = this->readStr("REFERENCED_COLUMN_NAME");
                        col->strREFERENCED_TABLE_NAME = this->readStr("REFERENCED_TABLE_NAME");
                        col->strREFERENCED_TABLE_SCHEMA = this->readStr("REFERENCED_TABLE_SCHEMA");
                    }
                }

                this->MoveNext();
            }
            this->closeTable();
        }

#if 0
        q.Format(
          "SELECT c.TABLE_NAME, t.TABLE_SCHEMA FROM information_schema.KEY_COLUMN_USAGE k "
          "JOIN information_schema.TABLES t ON k.REFERENCED_TABLE_NAME = t.TABLE_NAME AND k.REFERENCED_TABLE_SCHEMA = t.TABLE_SCHEMA "
          "JOIN information_schema.COLUMNS c ON c.TABLE_NAME = t.TABLE_NAME AND c.TABLE_SCHEMA = t.TABLE_SCHEMA "
          "WHERE k.TABLE_NAME = '%s' AND c.COLUMN_NAME = 'name' AND k.TABLE_SCHEMA = '%s'", table, schema);

        if (this->openTable0(q)) {
            while (!this->isEOF()) {
                CString schema = this->readStr("TABLE_SCHEMA");
                CString table_name = this->readStr("TABLE_NAME");
                this->MoveNext();
            }
            this->closeTable();
        }
#endif
    }

    if (m_type_of_net == 1 && 0) {

        q.Format("SELECT OBJECT_NAME(f.object_id) as ForeignKeyConstraintName,"
            "     OBJECT_NAME(f.parent_object_id) TableName, "
            "     COL_NAME(fk.parent_object_id,fk.parent_column_id) ColumnName, "
            "     OBJECT_NAME(fk.referenced_object_id) as ReferencedTableName, "
            "     COL_NAME(fk.referenced_object_id,fk.referenced_column_id) as ReferencedColumnName "

            " FROM sys.foreign_keys AS f "
            "     INNER JOIN sys.foreign_key_columns AS fk  "
            "         ON f.OBJECT_ID = fk.constraint_object_id "
            "     INNER JOIN sys.tables t "
            "         ON fk.referenced_object_id = t.object_id "

            " WHERE OBJECT_NAME(f.parent_object_id) = '%s' "
            // "     and COL_NAME(fk.referenced_object_id,fk.referenced_column_id) = 'your key column name' "

            , table);

        if (this->openTable0(q)) {
            while (!this->isEOF()) {

                CString schema1 = schema;
                CString table_name = this->readStr("TableName");
                CString column_name = this->readStr("ColumnName");

                schema1.MakeLower();
                table_name.MakeLower();
                column_name.MakeLower();

                MySQLColumns *col = findMySQLColumnsS(schema, table_name, column_name);
                if (col) {
                    col->strREFERENCED_COLUMN_NAME = this->readStr("ReferencedColumnName");
                    col->strREFERENCED_TABLE_NAME = this->readStr("ReferencedTableName");
                    col->strREFERENCED_TABLE_SCHEMA = schema;
                }

                col = findMySQLColumnsOrdS(schema, table_name, col->lORDINAL_POSITION);
                if (col) {
                    col->strREFERENCED_COLUMN_NAME = this->readStr("ReferencedColumnName");
                    col->strREFERENCED_TABLE_NAME = this->readStr("ReferencedTableName");
                    col->strREFERENCED_TABLE_SCHEMA = schema;
                }

                this->MoveNext();
            }
            this->closeTable();
        }
    }
}





MySQLTables *CAdoFile::findMySQLTable(const char *strTABLE_NAME)
{
    return findMySQLTableS(m_schema, strTABLE_NAME);
}

MySQLTables *CAdoFile::findMySQLTableS(const char *strTABLE_SCHEMA, const char *strTABLE_NAME)
{
    MySQLTableInfo(strTABLE_SCHEMA, strTABLE_NAME);

    CString schema = strTABLE_SCHEMA;
    CString table_name = strTABLE_NAME;

    schema.MakeLower();
    table_name.MakeLower();

    map<CString, map<CString, MySQLTables> >::iterator it1 = map_mysql2_tables.find(schema);

    if (it1 != map_mysql2_tables.end()) {
        map<CString, MySQLTables>::iterator it2 = it1->second.find(table_name);
        if (it2 != it1->second.end()) {
            return &it2->second;
        }
    }
    return NULL;
}


MySQLColumns *CAdoFile::findMySQLColumnsOrd(const char *strTABLE_NAME, int i)
{
    return findMySQLColumnsOrdS(m_schema, strTABLE_NAME, i);
}

MySQLColumns *CAdoFile::findMySQLColumnsOrdS(const char *strTABLE_SCHEMA, const char *strTABLE_NAME, int i)
{
    MySQLTableInfo(strTABLE_SCHEMA, strTABLE_NAME);
    
    CString schema = strTABLE_SCHEMA;
    CString table_name = strTABLE_NAME;

    schema.MakeLower();
    table_name.MakeLower();

    map<CString, map<CString, map<int, MySQLColumns> > >::iterator it1 = map_mysql_columnsOrd.find(schema);
    if (it1 != map_mysql_columnsOrd.end()) {
        map<CString, map<int, MySQLColumns> >::iterator it2 = it1->second.find(table_name);
        if (it2 != it1->second.end()) {
            map<int, MySQLColumns>::iterator it3 = it2->second.find(i);
            if (it3 != it2->second.end()) {
                return &it3->second;
            }
        }
    }
    return NULL;
}

MySQLColumns *CAdoFile::findMySQLColumns(const char *strTABLE_NAME, const char * strCOLUMN_NAME)
{
    return findMySQLColumnsS(m_schema, strTABLE_NAME, strCOLUMN_NAME);
}

int CAdoFile::NColsMySQL(const char *strTABLE_NAME) 
{
    CString st, tn;
    splitTableName(m_schema, strTABLE_NAME, st, tn);

    return NColsMySQL_S(m_schema, strTABLE_NAME); 
};


int CAdoFile::NColsMySQL_S(const char *strTABLE_SCHEMA, const char *strTABLE_NAME)
{
    MySQLTableInfo(strTABLE_SCHEMA, strTABLE_NAME);

    CString schema = strTABLE_SCHEMA;
    CString table_name = strTABLE_NAME;

    schema.MakeLower();
    table_name.MakeLower();

    map<CString, map<CString, map<CString, MySQLColumns> > >::iterator it1 = map_mysql_columns.find(schema);
    if (it1 != map_mysql_columns.end()) {
        map<CString, map<CString, MySQLColumns> >::iterator it2 = it1->second.find(table_name);
        if (it2 != it1->second.end()) {
            return it2->second.size();
        }
    }
    return 0;
}

bool CAdoFile::getMapColumn(const char *strTABLE_SCHEMA, const char *strTABLE_NAME, map<int, MySQLColumns> &map_col)
{
    MySQLTableInfo(strTABLE_SCHEMA, strTABLE_NAME);

    CString schema = strTABLE_SCHEMA;
    CString table_name = strTABLE_NAME;

    schema.MakeLower();
    table_name.MakeLower();

    map<CString, map<CString, map<int, MySQLColumns> > >::iterator it1 = map_mysql_columnsOrd.find(schema);
    if (it1 != map_mysql_columnsOrd.end()) {
        map<CString, map<int, MySQLColumns> >::iterator it2 = it1->second.find(table_name);
        if (it2 != it1->second.end()) {
            map_col = it2->second;
            return true;
        }
    }

    return false;
}

MySQLColumns *CAdoFile::findMySQLColumnsS(const char *strTABLE_SCHEMA, const char *strTABLE_NAME, const char * strCOLUMN_NAME)
{
    MySQLTableInfo(strTABLE_SCHEMA, strTABLE_NAME);

    CString schema = strTABLE_SCHEMA;
    CString table_name = strTABLE_NAME;
    CString column_name = strCOLUMN_NAME;

    schema.MakeLower();
    table_name.MakeLower();
    column_name.MakeLower();

    map<CString, map<CString, map<CString, MySQLColumns> > >::iterator it1 = map_mysql_columns.find(schema);
    if (it1 != map_mysql_columns.end()) {

        map<CString, map<CString, MySQLColumns> >::iterator it2 = it1->second.find(table_name);
        if (it2 != it1->second.end()) {
            map<CString, MySQLColumns>::iterator it3 = it2->second.find(column_name);
            if (it3 != it2->second.end()) {
                return &it3->second;
            }
        }
    }
    return NULL;
}

bool CAdoFile::isMySQL_OK(const char *strTABLE_SCHEMA, const char *strTABLE_NAME)
{
    CString schema = strTABLE_SCHEMA;
    CString table_name = strTABLE_NAME;

    schema.MakeLower();
    table_name.MakeLower();

    map<CString, map<CString, map<CString, MySQLColumns> > >::iterator it1 = map_mysql_columns.find(schema);
    if (it1 != map_mysql_columns.end()) {
        map<CString, map<CString, MySQLColumns> >::iterator it2 = it1->second.find(table_name);
        if (it2 != it1->second.end()) {
            return true;
        }
    }
    return false;
}

bool CAdoFile::isMySQL2_OK(const char *strTABLE_NAME)
{
    CString table_name = strTABLE_NAME;

    table_name.MakeLower();

    map<CString, map<CString, MySQLColumns2> >::iterator it1 = map_mysql2_columns.find(table_name);
    if (it1 != map_mysql2_columns.end()) {
        return true;
    }
    return false;
}

void CAdoFile::TableInfo(const char *table)
{
    return;

    if (isMySQL2_OK(table)) return;
    
    MySQLColumns2 c;
    CString t = table;
    t.MakeLower();

    CString q;

    q.Format("SELECT rt.newName, rc.newName, "
        "h.stroka,h.en,h.ru,h.nm,h.hlp,h.helpshiftf1,h.velichina,h.stand_razmer "
        "FROM tables_columns as tc  "
        "join renamed_tables as rt on rt.id = tc.tableID "
        "join renamed_columns as rc on rc.id = tc.columnID "
//        "join vsp.HELP_OLD as h on h.tab = rt.oldName AND h.en = rc.oldName WHERE lower(rt.newName)='%s'", t);
        "left join vsp.HELP_OLD as h ON h.en = rc.oldName AND (h.tab = rt.oldName OR rt.newName='linesObj' AND h.stroka='UT' OR rt.newName='nodes' AND h.stroka='US') "
//        "left join vsp."
        "WHERE lower(rt.newName)='%s'", t);


     if (m_type_of_net == 1) {
        q.Replace("vsp.", "vsp.dbo.");
     }


    if (openTable0(q)) {
        while (!this->isEOF()) {
            CString newName = this->readStr("newName");
            newName.MakeLower();

            c.stroka = this->readStr("stroka");
            c.en = this->readStr("en");
            c.ru = this->readStr("ru");
            c.nm = this->readStr("nm");
            c.hlp = this->readStr("hlp");
            c.helpshiftf1 = this->readStr("helpshiftf1");


            c.velichina = this->readStr("velichina");
            c.stand_razmer = this->readStr("stand_razmer");
            if (c.stand_razmer == "") c.stand_razmer = get_razm(c.velichina);

            if (c.stroka == "") c.ru = newName;
            if (c.ru == "") c.ru = c.stroka;
            if (c.helpshiftf1 == "") c.helpshiftf1 = c.hlp;

            map_mysql2_columns[t][newName] = c;
            this->MoveNext();
        }
        closeTable();
    }


    q.Format("SELECT "
        "h.stroka,h.en,h.ru,h.nm,h.hlp,h.helpshiftf1,h.velichina,h.stand_razmer "
        "FROM vsp.HELP_OLD as h WHERE lower(h.tab)='%s'", t);

    if (m_type_of_net == 1) {
        q.Replace("vsp.", "vsp.dbo.");
    }


    if (openTable0(q)) {
        while (!this->isEOF()) {
            CString newName = this->readStr("en");
            newName.MakeLower();

            c.stroka = this->readStr("stroka");
            c.en = this->readStr("en");
            c.ru = this->readStr("ru");
            c.nm = this->readStr("nm");
            c.hlp = this->readStr("hlp");
            c.helpshiftf1 = this->readStr("helpshiftf1");
            c.velichina = this->readStr("velichina");
            c.stand_razmer = this->readStr("stand_razmer");
            if (c.stand_razmer == "") c.stand_razmer = get_razm(c.velichina);

            if (c.stroka == "") c.ru = newName;
            if (c.ru == "") c.ru = c.stroka;
            if (c.helpshiftf1 == "") c.helpshiftf1 = c.ru;

            map_mysql2_columns[t][newName] = c;
            this->MoveNext();
        }
        closeTable();
    }




    map_mysql2_columns[t][""] = c;
}


static map<CString, map<CString, MySQLColumns2> > map_mysqlOLD_columns;

void CAdoFile::OldInfo(const char *table)
{
//    return;

    //    if (isMySQL2_OK(table)) return;
    static bool is_open = false;

    if (is_open) return;
    is_open = true;

    MySQLColumns2 c;
    CString t = table;
    t.MakeLower();

//    map<CString, map<CString, MySQLColumns2> > ::const_iterator it = map_mysqlOLD_columns.find(t);
//    if (it != map_mysqlOLD_columns.end()) return;

    CString q;

    q.Format("SELECT stroka,en,ru,nm,hlp,helpshiftf1,velichina,stand_razmer FROM vsp.HELP_OLD WHERE stroka='%s' OR tab='%s'", table, table);
    q.Format("SELECT stroka,en,ru,nm,hlp,helpshiftf1,velichina,stand_razmer FROM vsp.HELP_OLD WHERE lower(stroka)='%s'", t);

    if (t.Find("_out") != -1) {
      q.Format("SELECT stroka,en,ru,nm,hlp,helpshiftf1,velichina,stand_razmer FROM vsp.HELP_OLD WHERE tab='%s'", table);
    }

    //    q.Format("SELECT stroka,en,ru,nm,hlp,helpshiftf1,velichina,stand_razmer FROM vsp.HELP_OLD WHERE tab='%s'", table);
    q.Format("SELECT tab,stroka,en,ru,nm,hlp,helpshiftf1,velichina,stand_razmer FROM vsp.HELP_OLD");

     if (m_type_of_net == 1) {
        q.Replace("vsp.", "vsp.dbo.");
     }


    if (openTable0(q)) {
        while (!this->isEOF()) {
            c.stroka = this->readStr("stroka");
            c.en = this->readStr("en");
            c.ru = this->readStr("ru");
            c.nm = this->readStr("nm");
            c.hlp = this->readStr("hlp");
            c.helpshiftf1 = this->readStr("helpshiftf1");
            c.velichina = this->readStr("velichina");
            c.stand_razmer = this->readStr("stand_razmer");
            if (c.stand_razmer == "") c.stand_razmer = get_razm(c.velichina);

            CString t =  this->readStr("tab");
            t.MakeLower();

            CString cen = c.en;
            cen.MakeLower();

            map_mysqlOLD_columns[t][c.en] = c;
            map_mysqlOLD_columns[transl(t)][transl(cen)] = c;

            this->MoveNext();
        }
        closeTable();
    }
    map_mysqlOLD_columns[t][""] = c;
}

MySQLColumns2 *CAdoFile::findMySQLOldColumns(const char *table, const char *column)
{
    if (!table[0]) return NULL;

    OldInfo(table);
    
    CString table_name = table;
    CString column_name = column;

    table_name.MakeLower();
    column_name.MakeLower();

    {
        map<CString, map<CString, MySQLColumns2> >::iterator it1 = map_mysqlOLD_columns.find(table_name);
        if (it1 != map_mysqlOLD_columns.end()) {
            map<CString, MySQLColumns2>::iterator it2 = it1->second.find(column_name);
            if (it2 != it1->second.end()) {
                return &it2->second;
            }
        }
    }
    return NULL;
}


MySQLColumns2 *CAdoFile::findMySQLColumns2(const char *table, const char *column)
{
    if (!table[0]) return NULL;

    TableInfo(table);
    
    CString table_name = table;
    CString column_name = column;

    table_name.MakeLower();
    column_name.MakeLower();

    map<CString, map<CString, MySQLColumns2> >::iterator it1 = map_mysql2_columns.find(table_name);
    if (it1 != map_mysql2_columns.end()) {
        map<CString, MySQLColumns2>::iterator it2 = it1->second.find(column_name);
        if (it2 != it1->second.end()) {
            return &it2->second;
        }
    }
    return NULL;
}


CString CAdoFile::getTableName(const char *table)
{
  return CString("<<")+table+CString(">>");
}



CString getFullName(CString cx, CString tn, CString fn)
{
    CAdoFile *ado = getAdo(cx);

    if (ado) {
        CString title = fn;
        CString f1 = fn;

        MySQLColumns *col = ado->findMySQLColumns(tn, fn);
        MySQLColumns2 *col2 = ado->findMySQLColumns2(tn, fn);
        MySQLColumns2 *colOld = ado->findMySQLOldColumns(tn, fn);

        if (col) {
            if (col->strCOLUMN_COMMENT != "") {
                title = col->strCOLUMN_COMMENT;
            }
            //           dg.NumericScale = (short)col->lNUMERIC_SCALE;
        }

        if (colOld) {
            if (colOld->nm != "")  title = colOld->nm;
            if (colOld->helpshiftf1 != "")  f1 = colOld->helpshiftf1;
            if (colOld->stand_razmer != "") {
                title += ", ";
                title += colOld->stand_razmer;
            }
        }
        else if (col2) {
            if (col2->nm != "")  title = col2->nm;
            if (col2->helpshiftf1 != "") f1 = col2->helpshiftf1;
            if (col2->stand_razmer != "") {
                title += ", ";
                title += col2->stand_razmer;
            }
        }

        return title;
    }
    return fn;
}


void CAdoFile::AddNew(const char *tn)
{
  is_new = true;
  map_val.clear();
  m_TableNameUpdate = tn;
}


void CAdoFile::Edit(const char *tn, int id)
{
  is_new = false;
  map_val.clear();
  m_TableNameUpdate = tn;
  m_idUpdate = id;
}


long CAdoFile::Update()
{
  long n = 0;
  CString q;
  long affected;

  if (is_new) {

    CString cols = "";
    CString vals = "";

    map<CString, CString>::const_iterator it = map_val.begin();
    for (; it != map_val.end(); it++) {
      CString col = it->first;
      CString val = it->second;

      if (1) {
        if (cols != "") cols += ",";
        if (vals != "") vals += ",";

        cols += col;
        vals += val;
      }
    }
    q.Format("INSERT INTO %s (%s) VALUES (%s)", m_TableNameUpdate, cols, vals);
    n = ExecuteInsert(q, &affected);
  }
  else {
    CString ss = "";
    
    map<CString, CString>::const_iterator it = map_val.begin();
    for (; it != map_val.end(); it++) {
      CString col = it->first;
      CString val = it->second;
      if (1) {

        CString s;
        s.Format("%s=%s", col, val);
        if (ss != "") ss += ",";
        ss += s;
      }
    }

    q.Format("UPDATE %s SET %s WHERE id=%d", m_TableNameUpdate, ss, m_idUpdate);

    n = -1;

    if (Execute(q, &affected)) {
        n = m_idUpdate;
    }
  }
  
  return n;
}


bool CAdoFile::write_null(const char *fn)
{
    map_val[fn] = "NULL";
    return true;
}



bool CAdoFile::write0(const char *fn, const char *v)
{
    if (!strcmp(v, "inf")) return true;
    if (!strcmp(v, "-inf")) return true;
    if (!strcmp(v, "nan")) return true;
    if (!strcmp(v, "-nan(ind)")) return true;
    if (!strcmp(v, "nan(ind)")) return true;

    map_val[fn] = v;
    return true;
}


bool CAdoFile::write(const char *fn, const char *v)
{
    CString vv;
    vv.Replace("'", "''");
    vv.Format("'%s'", v);

    return write0(fn, vv);
}

bool CAdoFile::write_object(const char *fn, const char *v)
{
    return write0(fn, v);
}



bool CAdoFile::write(const char *fn, double v)
{
  CString s;
  s.Format("%lg", v);
  return write0(fn, s);
}

bool CAdoFile::writen0(const char *fn, double v)
{
  CString s;
  s.Format("%lg", v);
  return write0(fn, s);
}

bool CAdoFile::write(const char *fn, int v)
{
  return write(fn, (long) v);
}


bool CAdoFile::write(const char *fn, long v)
{
  CString s;
  s.Format("%ld", v);
  return write0(fn, s);
}

bool CAdoFile::write(const char *fn, short v)
{
  CString s;
  s.Format("%d", v);
  return write0(fn, s);
}

bool CAdoFile::write(const char *fn, char v)
{
  CString s;
  s.Format("%c", v);
  return write0(fn, s);
}

bool CAdoFile::write(const char *fn, CTime v)
{
  CString s;
  s.Format("%c", v);
  return write0(fn, s);
}

bool CAdoFile::write(const char *fn, COleDateTime v)
{
  CString s;
//  s = v.Format("%Y-%m-%d");
  s = v.Format("%Y%m%d");

  if (v.m_dt == 0) {
//  if (v.GetStatus() == COleDateTime::null) {
      return write_null(fn);
  }
  return write(fn, s);
}

bool CAdoFile::StartTransaction()
{
  try
  {
    pConnection->BeginTrans();
  }
  catch(...)
  {
    return false;
  }

  return true;
}

bool CAdoFile::CancelTransaction()
{
  try
  {
    pConnection->RollbackTrans();
  }
  catch(...)
  {
    return false;
  }

  return true;
}

bool CAdoFile::FinishTransaction()
{
  try
  {
    pConnection->CommitTrans();
  }
  catch(...)
  {
    return false;
  }

  return true;
}

CString dateTimeToString(COleDateTime v)
{
  CString s = v.Format("%Y%m%d %H:%M:%S");
  return s;
//  v.Format("%Y-%m-%d %H:%M:%S");

}

CString dateToString(COleDateTime v)
{
  CString s = v.Format("%Y%m%d");
  return s;
}

CString dateToStringV(COleDateTime v)
{
  CString s = v.Format("%d-%m-%Y");
  return s;
}


CString get_gid_info(CAdoFile *ado, const char * fn)
{
    CString v = "";
        CString q;
    q.Format("SELECT TOP 1 value FROM gid_info WHERE name='%s'", fn);
    
    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            v = ado->readStr("value");
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return v;

}


bool set_gid_info(CAdoFile *ado, const char * fn, const char * v)
{
    CString q;
    long affected;
    bool ret = false;
    q.Format("UPDATE gid_info SET value='%s' WHERE name='%s'", v, fn);

    bool r = ado->Execute(q, &affected);

    if (r && affected == 1) return true;

    q.Format("INSERT INTO gid_info (name, value) VALUES ('%s', '%s')", fn, v);
    if (ado->Execute(q, &affected)) {
        ret = true;
    }


    return ret;
}

// Последний ID
// Только MSSQL

long get_last_id(CAdoFile *ado, const char *tn) 
{
  CString q;
  long id = 0;

  q.Format(
"select"
"  OBJECT_NAME(object_id) as table_name "
"  , name as col_name "
"  , ISNULL(CAST(last_value AS INT), 0) AS next_id "
"from "
"  sys.identity_columns "
"WHERE "
"  OBJECT_NAME(object_id) LIKE '%s'", tn);

  if (ado->openTable0(q)) {
      if (!ado->isEOF()) {
          id = ado->read_long("next_id");
      }
      ado->closeTable();
  }
  return id;
}


double read_double_db(CAdoFile *ado, const char *q, const char *fn)
{
    double V = 0;

    if (ado->openTable0(q)) {
        if ( !ado->isEOF() ) {
            V = ado->read_double(fn);
        }
        ado->closeTable();
    }
    return V;
}


long read_long_db(CAdoFile *ado, const char *q, const char *fn)
{
    long V = 0;

    if (ado->openTable0(q)) {
        if ( !ado->isEOF() ) {
            V = ado->read_long(fn);
        }
        ado->closeTable();
    }
    return V;
}

/*
bool CAdoFile::GotoKey(int id)
{
    COleVariant key((long)id);
    COleVariant Start(1L);
    CString pp;
    pp.Format("id=%d", id);
    _bstr_t p(pp);


    try {
        if (pRecordset)
            if (pRecordset->State == adStateOpen)
//                pRecordset->Seek(key, adSeekFirstEQ);
                pRecordset->Find(p, 0, adSearchForward, Start);
    }
    catch (_com_error &e) {
        // Display errors, if any. Pass a connection pointer accessed from the Connection.
        PrintProviderError(pConnection);
        PrintComError(e);
    }


  return true;
}
*/



bool CAdoFile::GotoKey(int id)
{
    COleVariant key((long)id);
    COleVariant Start(1L);
    CString pp;
    pp.Format("id=%d", id);
    _bstr_t p(pp);

    if (pRecordset) {
        if (pRecordset->State == adStateOpen) {
            try {
        //                pRecordset->Seek(key, adSeekFirstEQ);
                pRecordset->Find(p, 0, adSearchForward, Start);
            }
            catch (_com_error &e) {
                // Display errors, if any. Pass a connection pointer accessed from the Connection.
                PrintProviderError(pConnection);
                PrintComError(e);
                return false;
            }
        }
    }
    return true;
}

bool CAdoFile::GotoKey2(int id)
{
    COleVariant key((long)id);
    COleVariant Start(0L);
    CString pp;
    pp.Format("id=%d", id);
    _bstr_t p(pp);

    if (pRecordset) {
        if (pRecordset->State == adStateOpen) {
            try {
                //                pRecordset->Seek(key, adSeekFirstEQ);
                pRecordset->Find(p, 0, adSearchForward, Start);
            }
            catch (_com_error & e) {
                // Display errors, if any. Pass a connection pointer accessed from the Connection.
                PrintProviderError(pConnection);
                PrintComError(e);
                return GotoKey(id);
            }
        }
    }
    return true;
}



/*


bool CAdoFile::GotoKey2(int id)
{
    COleVariant key((long)id);
    COleVariant Start(1L);
    CString pp;
    pp.Format("id=%d", id);
    _bstr_t p(pp);

    if (pRecordset) {
        if (pRecordset->State == adStateOpen) {
            try {
                        pRecordset->Seek(key, adSeekFirstEQ);
        //                pRecordset->Find(p, 0, adSearchForward, Start);
            }
            catch (_com_error &e) {
                // Display errors, if any. Pass a connection pointer accessed from the Connection.
                PrintProviderError(pConnection);
                PrintComError(e);
            }
        }
    }

  return true;
}
*/


void CAdoFile::SetTimeout(long pl)
{
}


long getGeoColor(int color_id) 
{
    if (color_id == 1) {
        return 0x0000FF; // RED
    }
    else if (color_id == 2) {
        return 0xFF0000; // BLUE
    }
    else if (color_id == 3) {
        return 0x00FF00; // GREEN
    }
    else if (color_id == 4) {
        return 0x000000; // BLACK
    }
    else if (color_id == 1000) {
        return 16777092; //  Здание ТУ
    }
    else if (color_id == 57) {
        return 16764159; //  Дом с нагрузкой
    }
    else if (color_id == 56) {
        return 10210815; //  Дом без нагрузки
    }



    else if (color_id == 1001) {
        return 0x00FFFF; //  Здание ТУ (Завершенный)
//        return 0x00C4C4; //  Здание ТУ (Завершенный)
//        return 0xC4C400; //  Здание ТУ (Завершенный)
    }
    else if (color_id >= 2000 && color_id < 2030) {
        int c = color_id-2000;
        if ((c & (1 << (4-1))) && !(c & (1 << (1-1)))) {
            return 0x00FFFF; //  Здание ТУ (Завершенный)
//            return 0xC4C400; //  Здание ТУ (Завершенный) Темноголубой
        }
        return 16777092; //  Здание ТУ
    }

    return color_id;
}


bool reset_shape_node(CAdoFile* ado, int id)
{
    if (!ado->is_shape_gid) return false;
    if (id <= 0) return false;

    CString q1, q2;

    long affected;

    q1.Format(

        " UPDATE nodes"
        " SET"
        " shape = geometry::STGeomFromText(CONCAT('POINT(', "
//        "x/100"
        " try_convert(decimal(20,0), x)/100 "
        ", ' ',  "
//        "-y/100"
        " try_convert(decimal(20,0), -y)/100 "
        ", ')'), 0)"

        " WHERE internalNodeID IS NULL "
        "   AND (x <> 0 OR y <> 0) "
        "   AND removed=0 AND id=%d", id);

    bool ret1 = ado->Execute(q1, &affected);


    q2.Format(

        " UPDATE  l"
        " SET"
        " l.shape = geometry::STGeomFromText(CONCAT('LINESTRING(', "
//        " n1.x/100" 
        " try_convert(decimal(20,0), n1.x)/100 "
        ", ' ',  "
//        "-n1.y/100"
        " try_convert(decimal(20,0), -n1.y)/100 "
        ", ',',  dbo.str_coords(REPLACE(try_convert(nvarchar, l.coords), ',', ' ')),"
//        "n2.x/100"
        " try_convert(decimal(20,0), n2.x)/100 "
        ", ' ', "
//        "-n2.y/100"
        " try_convert(decimal(20,0), -n2.y)/100 "
        ", ')'), 0).MakeValid()"

        " FROM linesobj l"
        " JOIN nodes n1 ON n1.id=l.nodeID1"
        " JOIN nodes n2 ON n2.id=l.nodeID2"

        " WHERE n1.internalNodeID IS NULL "
        "   AND (n1.x <> n2.x OR n1.y <> n2.y) "
        "   AND (n1.x <> 0 OR n1.y <> 0) "
        "   AND (n2.x <> 0 OR n2.y <> 0) "
        "   AND l.removed=0 AND (n1.id=%d OR n2.id=%d)", id, id);

    return ado->Execute(q2, &affected);
        }


bool reset_shape_line(CAdoFile* ado, CString qq)
{
    if (!ado->is_shape_gid) return false;

    CString q;
    long affected;

    q.Format(

        " UPDATE  l"
        " SET"
        " l.shape = geometry::STGeomFromText(CONCAT('LINESTRING(', "
//        " n1.x/100 "
        " try_convert(decimal(20,0), n1.x)/100 "
        " , ' ',  " 
//        " -n1.y/100" 
        " try_convert(decimal(20,0), -n1.y)/100 "
        ", ',',  dbo.str_coords(REPLACE(l.coords, ',', ' ')),"
//        " n2.x/100 " 
        " try_convert(decimal(20,0), n2.x)/100 "
        ", ' ', "
//        " -n2.y/100 "
        " try_convert(decimal(20,0), -n2.y)/100 "
        ", ')'), 0).MakeValid()"

        " FROM linesobj l"
        " JOIN nodes n1 ON n1.id=l.nodeID1"
        " JOIN nodes n2 ON n2.id=l.nodeID2"

        " WHERE n1.internalNodeID IS NULL "
        "   AND (n1.x <> n2.x OR n1.y <> n2.y) "
        "   AND (n1.x <> 0 OR n1.y <> 0) "
        "   AND (n2.x <> 0 OR n2.y <> 0) "
        "   AND l.removed=0 AND (%s)", qq);

    return ado->Execute(q, &affected);
}


bool reset_shape_line(CAdoFile* ado, int id)
{
    if (!ado->is_shape_gid) return false;
    if (id <= 0) return false;

    CString qq;
    qq.Format("l.id=%d", id);

    return reset_shape_line(ado, qq);
}


bool isTableNum(CAdoFile *ado, const char *tn, const char *fn, int id)
{
    CString q;

    q.Format("SELECT id FROM [%s] WHERE [%s]=%d", tn, fn, id);

    long nc = getCountTable2(ado, q);

    return nc > 0;
}

bool isTableNumLine(CAdoFile *ado, const char *tn, const char *fn, int id)
{
    CString q;

    q.Format("SELECT l.id FROM [%s] hps JOIN linesobj l ON l.id=hps.lineID WHERE hps.[%s]=%d AND l.removed=0", tn, fn, id);

    long nc = getCountTable2(ado, q);

    return nc > 0;
}



void CAdoFile::Refresh()
{
    FinishTransaction();
}


bool CAdoInsert::ado_insert(const char* values)
{
    bool ret = true;

    if (!values || values[0]) {
//        if (head[0]) m_head = head;
        n_insert++;
        if (m_values != "") m_values += ",";
        m_values += "(";
        m_values += values;
        m_values += ")\n";
    }

    if (n_insert == 999 || !values || values[0] == 0) {
        if (n_insert) {
            long affected;
            CString sql;

            sql.Format("INSERT INTO %s (%s) VALUES %s", m_tn, m_head, m_values);

            if (!m_ado->Execute(sql, &affected) && affected) {
                ret = false;
            }
        }
        n_insert = 0;
        m_values = "";
//        m_head = "";
    }
    return ret;
}


bool CAdoInsert::ado_insert_int1(int id)
{
    CString values;
    values.Format("%d", id);
    return ado_insert(values);
}

bool CAdoInsert::ado_insert_int2(int id1, int id2)
{
    CString values;
    values.Format("%d,%d", id1, id2);
    return ado_insert(values);
}

CString readTableValue(CAdoFile* ado, const char *q)
{
    CString v = "";
    
    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            v = ado->readStr(0);
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return v;
}




bool readTableValues(CAdoFile* ado, const char *q, int n, vector<CString> & vv)
{
    vv.clear();

    bool ret = false;
    
    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            for (int i = 0; i < n; i++ ) {
                CString v = ado->readStr(i);
                vv.push_back(v);
            }
            ret = true;
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return ret;
}



bool isEmptyQ(CAdoFile* ado, const char *q)
{
    bool empty = true;
    
    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            empty = false;
        }
        ado->closeTable();
    }
    return empty;
}



long readTableValueInt(CAdoFile* ado, const char *q)
{
    long v = 0;
    
    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            v = ado->read_long(0);
//            ado->MoveNext();
        }
        ado->closeTable();
    }
    return v;
}

long sizeOfQ(CAdoFile* ado, const char *qq)
{
    CString q;

    q.Format("SELECT count(*) FROM (%s\n) _QQQ", qq);
    return readTableValueInt(ado, q);
}

bool ado_equal(CAdoFile* ado1, CAdoFile* ado2)
{
    if (!ado1 || !ado2) return false;

    if (ado1->m_server == ado2->m_server && ado1->m_schema == ado2->m_schema) return true;

    return false;
}

