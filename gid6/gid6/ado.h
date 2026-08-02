#pragma once

struct ForeignTable {
    CString field;
    CString id;
    CString name;
    CString cxema;
    CString table;
    CString q;
    bool isSort;
};

struct CColName
{
    CString name;
    CString name_full;
};

ForeignTable findLookup(CString d, CString n1, CString n2);


const CColName *findColumnRusName(CString d, CString n1, CString n2);


enum changeTypeID
{
  CH_T_DELETE_LINE = 1,  
  CH_T_DELETE_NODE = 2,
  CH_T_DELETE_CXEMA_BEGIN = 3,  
  CH_T_DELETE_CXEMA_END = 4,
  CH_T_MOVE_NODE = 5,
  CH_T_MOVE_LINE = 6,
  CH_T_EDIT_NODE = 7,
  CH_T_EDIT_LINE = 8,
  CH_T_EDIT = 9,
  CH_T_ADD_LINE = 10,
  CH_T_ADD_NODE = 11,

  CH_T_RENAME_LINE = 12,
  CH_T_RENAME_NODE = 13,

  CH_T_DELETE_GEO = 14,
  CH_T_ADD_GEO = 15,
  CH_T_MOVE_GEO = 16,
  CH_T_BEGIN = 23,  
  CH_T_END = 24,

};

enum TYPE_OF_NET {
  TYPE_OF_NET_MYSQL = 0,
  TYPE_OF_NET_MSSQL = 1,
};

enum SignLine {
  SignLine_ob = 1,
  SignLine_pp = 2,
  SignLine_oo = 3,
  SignLine_po = 4,
  SignLine_op = 5
};

enum SignNode {
  SignNode_ob = 1,
  SignNode_p = 2,
  SignNode_o = 3
};

struct CAdoField
{
  CString f1;
  CString title;
  CString name;
  short typ;
  short DefinedSize;
  short NumericScale;
  short Precision;
  bool yes;
};

struct MySQLTables
{
  CString strTABLE_CATALOG;
  CString strTABLE_SCHEMA;
  CString strTABLE_NAME;
  CString strTABLE_TYPE;
//  CString strENGINE;
//  long lVERSION;
//  CString strROW_FORMAT;
//  long lTABLE_ROWS;
//  long lAVG_ROW_LENGTH;
//  long lDATA_LENGTH;
//  long lMAX_DATA_LENGTH;
//  long lINDEX_LENGTH;
//  long lDATA_FREE;
//  long lAUTO_INCREMENT;
//  COleDateTime dtCREATE_TIME;
//  COleDateTime dtUPDATE_TIME;
//  COleDateTime dtCHECK_TIME;
//  CString strTABLE_COLLATION;
//  long lCHECKSUM;
//  CString strCREATE_OPTIONS;
  CString strTABLE_COMMENT;
};


struct MySQLColumns
{
  CString strTABLE_CATALOG;
  CString strTABLE_SCHEMA;
  CString strTABLE_NAME;
  CString strCOLUMN_NAME;
  long lORDINAL_POSITION;
  CString strCOLUMN_DEFAULT;
  CString strIS_NULLABLE;
  CString strDATA_TYPE;
  long lCHARACTER_MAXIMUM_LENGTH;
  long lCHARACTER_OCTET_LENGTH;
  long lNUMERIC_PRECISION;
  long lNUMERIC_SCALE;
  long lDATETIME_PRECISION;
  CString strCHARACTER_SET_NAME;
  CString strCOLLATION_NAME;
//  CString strCOLUMN_TYPE;
//  CString strCOLUMN_KEY;
//  CString strEXTRA;
//  CString strPRIVILEGES;
  CString strCOLUMN_COMMENT;      

  CString name_full;


  CString strREFERENCED_TABLE_SCHEMA;
  CString strREFERENCED_TABLE_NAME;
  CString strREFERENCED_COLUMN_NAME;
};

struct MySQLColumns2
{
  CString stroka, en, ru, nm, hlp, helpshiftf1, velichina, stand_razmer;
};


class CAdoFile
{
  public:
    CAdoFile();
    CAdoFile(CAdoFile *ado);

    bool openMDB(const char *mdb);
    bool openMySQL(int type_of_net, const char *server, int port, const char *database, const char *user, const char *password);
    bool isOpen();
    bool openQ(const char *tn, int CursorType, int LockType, int CommandType);
    bool openTable__(const char *tn);
    bool openTable0(const char *tn);
    bool openTableS(const char *tn);
    bool openSchema();

    bool reConnect();

    long GetCommandTimeout();
    bool PutCommandTimeout(long ms);

    void SetTimeout(long pl);

    bool Execute(const char *q);
    bool Execute(const char *q, long *affected, CommandTypeEnum cmd = adCmdText);
    long ExecuteInsert_old(const char *q1, long *affected);
    long ExecuteInsert(const char *q1, long *affected);
    bool closeTable();
    bool closeConnect();
    bool isEOF();
    long count();
    long NFlds() { return nFlds;};
    long NRecs() { return nRecs;};
    void MoveNext();

    long read_long(const char *fn);
    long read_long(int id);

    COleDateTime read_date(const char *fn);
    COleDateTime read_date(int id);

    double read_double_null(const char *fn, double val0);

    double read_double(const char *fn);
    double read_double(int id);

    CString readStr(const char *fn);
    CString readStrDateJSON(const char *fn);


    CString readStr(int id);
    COleVariant read(int id);
    COleVariant read(const char *fn);
    CString FieldName(int i);
    CAdoField Field(int i);
    CAdoField Field(const char *fn);
    void clearError() {m_error = "";}
    CString getError() {return m_error;}

    void TableInfo(const char *table);
    void OldInfo(const char *table);

    CString correctQ(CString q);

    CString getTableName(const char *table);
    CString getTableName(const char *schema, const char *table);

    MySQLColumns *findMySQLColumnsS(const char *strTABLE_SCHEMA, const char *strTABLE_NAME, const char * strCOLUMN_NAME);
    MySQLColumns *findMySQLColumns(const char *strTABLE_NAME, const char * strCOLUMN_NAME);

    MySQLColumns *findMySQLColumnsOrdS(const char *strTABLE_SCHEMA, const char *strTABLE_NAME, int i);
    MySQLColumns *findMySQLColumnsOrd(const char *strTABLE_NAME, int i);

    MySQLTables *findMySQLTable(const char *strTABLE_NAME);
    MySQLTables *findMySQLTableS(const char *strTABLE_SCHEMA, const char *strTABLE_NAME);


    MySQLColumns2 *findMySQLColumns2(const char *table, const char *column);
    MySQLColumns2 *findMySQLOldColumns(const char *table, const char *column);


    int NColsMySQL_S(const char *strTABLE_SCHEMA, const char *strTABLE_NAME);
    int NColsMySQL(const char *strTABLE_NAME);

    bool getMapColumn(const char *strTABLE_SCHEMA, const char *strTABLE_NAME, map<int, MySQLColumns> &map_col);

    bool isMySQL_OK(const char *strTABLE_SCHEMA, const char *strTABLE_NAME);
    bool isMySQL2_OK(const char *strTABLE_NAME);

    CString m_user;
    CString m_server;
    CString m_schema;
    int m_type_of_net;

    CString getConnect() { return m_connect;};

    void Edit(const char *tn, int id);
    void AddNew(const char *tn);
    long Update();

    void Refresh();


    bool write0(const char *fn, const char *v);

    bool write(const char *fn, int v);


    bool write(const char *fn, COleVariant &var);
    bool write(const char *fn, double v);
    bool write(const char *fn, long v);
    bool write(const char *fn, short v);
    bool write(const char *fn, const char *v);
    bool write(const char *fn, char v);
    bool writen0(const char *fn, double v);
    bool write(const char *fn, CTime v);
    bool write(const char *fn, COleDateTime v);

    bool write_object(const char *fn, const char *v);

    bool write_null(const char *fn);

    bool StartTransaction();
    bool CancelTransaction();
    bool FinishTransaction();

    bool GotoKey(int id);
    bool GotoKey2(int id);

    bool is_shape_gid;

  private:

   map<CString, CString> map_val;
   bool is_new;
   CString m_TableName;
   CString m_TableNameUpdate;
    int m_idUpdate;
    long m_timeout;

    long nFlds;
    long nRecs;
    CString m_connect;
    CString m_error;
    bool m_isOpen;

    _RecordsetPtr pRecordsetOld;

    _RecordsetPtr pRecordset;
    _ConnectionPtr pConnection;
   
    map<CString, map<CString, map<CString, MySQLColumns> > > map_mysql_columns;
    map<CString, map<CString, map<int, MySQLColumns> > > map_mysql_columnsOrd;

    map<CString, map<CString, MySQLColumns2> > map_mysql2_columns;

//    map<CString, map<CString, MySQLColumns2> > map_mysqlOLD_columns;


    map<CString, map<CString, MySQLTables> > map_mysql2_tables;

    void MySQLTableInfo(const char *table);
    void MySQLTableInfo(const char *schema, const char *table);

    bool openODBC(const char *connect);
    void PrintProviderError(_ConnectionPtr pConnection);
    void PrintComError(_com_error &e);
};

struct CPropertyParam
{
    CAdoFile *ado;
    CString table;
    int id;
    int typ;
    list<int> m_list_id;
};

//void OpenX(const char *w1, const char *w2, map<CString, COleVariant> & lst, list<CString> & lstFld);
int OpenX(CAdoFile *ado, const char *q, map<CString, COleVariant> & lst, list<CAdoField> & lstFld);
LPCTSTR strAdoFieldType(short sType);
void OpenList(CAdoFile *ado, const char *q, list<COleVariant> & lst);
void OpenList2(CAdoFile *ado, const char *q, list< vector<COleVariant> > & lst);
CString OpenXJSon(CAdoFile *ado, const char *q) ;
CString OpenXJSon1(CAdoFile *ado, const char *q);
CString OpenXJSon2(CAdoFile *ado, const char *q);
CString getVstr(const map<CString, COleVariant> & lst, const char *fn);
CString getVstr(const map<int, map<CString, COleVariant> > & lst, int n, const char *fn);
COleVariant getV(const map<CString, COleVariant> & lst, const char *fn);

int OpenX2(CAdoFile *ado, const char *q, map<int, map<CString, COleVariant> > & lst, list<CAdoField> & lstFld);

long to_long(const COleVariant &var);

CAdoFile *getAdo(const char *name);

CString getPsAdoName();
CString getGidAdoName();

void getAdoConnectParam(int & type_of_net, CString &server, int &port, CString &user, CString &password);
void setGidAdoName(const char *name);
long getCountTable2(CAdoFile *ado, const char *q);

CString dateTimeToString(COleDateTime v);
CString dateToString(COleDateTime v);
const map<CString, ForeignTable> *findLookup(CString d, CString n1);
long get_last_id(CAdoFile *ado, const char *tn);
long getGeoColor(int color_id);

bool reset_shape_node(CAdoFile *ado, int id);
bool reset_shape_line(CAdoFile *ado, int id);
bool reset_shape_line(CAdoFile *ado, CString q);

//bool ado_insert(CAdoFile* ado, const char* tn, const char* head = "", const char* values = "");
//bool ado_insert_int1(CAdoFile* ado, const char* tn, const char* head, int id1);
//bool ado_insert_int2(CAdoFile* ado, const char* tn, const char* head, int id1, int id2);

bool readTableValues(CAdoFile* ado, const char *q, int n, vector<CString> & vv);
long readTableValueInt(CAdoFile* ado, const char *q);
CString readTableValue(CAdoFile * ado, const char* q);
bool isEmptyQ(CAdoFile* ado, const char *q);


bool ado_equal(CAdoFile* ado1, CAdoFile* ado2);

class CAdoInsert
{
public:
    CAdoInsert(CAdoFile* ado, const char *tn, const char *head) {
        m_ado = ado;
        m_head = head;
        m_tn = tn;
        n_insert = 0;
        m_values = "";
    };
    ~CAdoInsert() {
        ado_insert();
    }
    bool ado_insert(const char* values = "");
    bool ado_insert_int1(int id);
    bool ado_insert_int2(int id1, int id2);
private:
    int n_insert;
    CString m_values;
    CString m_head;
    CString m_tn;
    CAdoFile* m_ado;
};

