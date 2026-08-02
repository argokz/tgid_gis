#include <afxwin.h>
#include <afxmt.h>
#include <comdef.h>
#include <comutil.h>
#include <adoint.h>  // ADO interfaces

#pragma comment(lib, "comsuppw.lib") // Import the COM support library

// Функция для выполнения запроса
UINT ExecuteQuery(LPVOID pParam)
{
    CoInitialize(NULL);

    // Создание соединения
    _ConnectionPtr pConnection("ADODB.Connection");
    pConnection->ConnectionString = "your_connection_string";
    pConnection->Open("", "", "", adConnectUnspecified);

    // Создание команды
    _CommandPtr pCommand("ADODB.Command");
    pCommand->ActiveConnection = pConnection;
    pCommand->CommandText = "your_sql_query";
    pCommand->CommandType = adCmdText;

    // Выполнение команды
    _RecordsetPtr pRecordset = pCommand->Execute(NULL, NULL, adCmdText);

    // Обработка результатов (если нужно)
    // ...

    // Закрытие соединения
    pConnection->Close();

    CoUninitialize();

    return 0;
}

// Запуск асинхронного запроса
void StartAsyncQuery()
{
    AfxBeginThread(ExecuteQuery, NULL);
}

// Пример использования
class CMyApp : public CWinApp
{
public:
    virtual BOOL InitInstance()
    {
        StartAsyncQuery();
        return TRUE;
    }
};

CMyApp theApp;
