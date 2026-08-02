#pragma once
#define UG_SUCCESS      0
#define UG_ERROR        1
#define UG_NA           -1

#include "stdafx.h"
#include "ado.h"
#include <any>
#include "Ultimate Grid/virt_data.h"

class WVAdoImpDataSource
{
public:
    WVAdoImpDataSource();
    virtual ~WVAdoImpDataSource();
    // Oppening the record set.
    virtual int Open(LPCTSTR name, LPCTSTR option);
    virtual int OpenSQL(CString sql, CString table_name, CString conStr, int type_of_net);
    virtual BOOL IsOpen();
    virtual int Close();
    CString GetSQL();

	virtual int SetCommand(CString command);
	virtual int SetFilter(CString filter);
	virtual int SetSort(CString sort);
	void showAllInit(bool all);
	void SetTableName(CString db, CString tn);
	long GetNumRows();
	int OnHitBottom(int row);
	double to_double(_variant_t& var);
	int countFields();
	CString nameField(int index);
	DataTypeEnum fieldVariant(int index);
	int addNew(int row, map<int, std::any> mapParams);
	int updateData(int id, int row, map<int, std::any> &mapParams);
    int addNewOld(int row, map<int, std::any> mapParams);
	CString updateData2(int id, int row, int countFields);
	pair<int, CString> WVAdoImpDataSource::deleteData(vector<int> ids);
    pair<int, CString> execute(CString q);
    pair<int, CString> deleteData(int id, int row);

    //setAllows измен€ет параметры позвол€ющии редактирование удаление или изменение
    void setAllows(VIRT_DATA type);
    long GetNumRowsComplete();

protected:
    int OpenConnection();
    int OpenRecordset();

    int ParseSQLString(CString sql);
    int FetchNewRows(long row);


public:
    int m_type_of_net;
    CString   m_sDbName;    // 
    CString   m_sTableName;    // 
    CString   m_sTableName_main;    // 
    CString m_q;
    CString addText;
    long    m_pcRowsObtained; // number of rows fetched with the last fetch operation
    _variant_t  m_curRecords;   // safe array containing rows fetched from DB
    long portionCount;
    map<int, int> positionById;
    map<int, int> positionS;

    //столбцы которые нужно заполнить автоматически
    vector<CString> mDataKey;
    vector<int> mDataValue;

    BOOL allowAdd, allowEdit, allowDelete;
    VIRT_DATA myType;
    map<CString, int> excelType;
    CString getFullName1(CString tempStr);
    void OnExcel(BSTR jsonStr);
    void OnWord(BSTR fileName);
    void OnExcelPro(BSTR jsonStr, CString filePath, CString tmpName, HWND hwnd);
    void ClearCurrentConnection();

protected:
    _ConnectionPtr  m_pConnection;  // the connection
    _RecordsetPtr m_pRecordSet; // the recordset

    CString   m_dbConnection;   // source string for the connection
    CString   m_User;       // user login name
    CString   m_Password;     // user login password

    CString   m_Command;      // command to execute (SQL statement without WHERE and 
                // ORDER BY clauses)
    CString   m_Filter;     // filter to be used (WHERE clause)
    CString   m_SortOrder;    // sort order to be used (ORDER BY clause)

    int m_showAll;      // ѕоказывать все строки

    long m_nFetchedStarting;

    long m_nMaxFetchRows;




    _variant_t  m_curBookmark;    // long integer representing book mark of the record that
};

