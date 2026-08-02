#include "stdafx.h"
#include <winspool.h>
//
//  This method tests the selected printer to determine if
//  it supports color.
//
int CheckForColorPrinter(CString printer)
{
  HANDLE hPrinter=NULL;
  DWORD dwBytesNeeded=0;
  PRINTER_INFO_2* pinfo2=NULL;
  DRIVER_INFO_2*  dinfo2=NULL;
  DWORD dwColor=0;

  //  open printer.
  BOOL rc = OpenPrinter(printer.GetBuffer(0), &hPrinter, NULL);
  if (rc)
  {
    //  get printer info.
    rc = GetPrinter(hPrinter, 2, NULL, NULL, &dwBytesNeeded);
    pinfo2 = (PRINTER_INFO_2*) LocalAlloc(LPTR, dwBytesNeeded);
    rc = GetPrinter(hPrinter, 2, (LPBYTE)pinfo2, dwBytesNeeded, &dwBytesNeeded);
    //  get driver info.
    rc = GetPrinterDriver(hPrinter, NULL, 2, NULL, NULL, &dwBytesNeeded);
    dinfo2 = (DRIVER_INFO_2*) LocalAlloc(LPTR, dwBytesNeeded);
    rc = GetPrinterDriver(hPrinter, NULL, 2, (LPBYTE)dinfo2, dwBytesNeeded, &dwBytesNeeded);

    //  get handle to printer driver config module.
    HMODULE hmod = LoadLibrary(dinfo2->pConfigFile);
    if (hmod)
    {
      DWORD (WINAPI* DriverCaps)(HANDLE, LPWSTR, WORD, VOID*, PDEVMODE);

      //  get pointer to exported function.
      DriverCaps = (DWORD (WINAPI*) (HANDLE, LPWSTR, WORD, VOID*, PDEVMODE))
        GetProcAddress(hmod, "DrvDeviceCapabilities");
      if (DriverCaps)
      {
        WCHAR wszPrinter[256] = {0};   
          MultiByteToWideChar(CP_ACP, 0, printer, -1, wszPrinter, 256);
        //  call exported function.
        dwColor = (*DriverCaps)(hPrinter, wszPrinter, 32 /*DC_COLORDEVICE*/, (VOID*)NULL, pinfo2->pDevMode);
      }

      FreeLibrary(hmod);      
    }

    //  clean up.
    LocalFree(pinfo2);
    LocalFree(dinfo2);
    ClosePrinter(hPrinter);
  }

  if (dwColor)
    //  printer supports color.
    return 1;
  else
    //  printer does not support color.
    return 0;
}
