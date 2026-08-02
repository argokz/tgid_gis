#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>
//#include <bcnthasp.h>
#include "hasp.h"


/*
 * A list of NetHASP services.
 */

/*#define NET_LAST_STATUS           40
#define NET_GET_HASP_CODE         41
#define NET_LOGIN                 42
#define NET_LOGOUT                43
#define NET_READ_WORD             44
#define NET_WRITE_WORD            45
#define NET_GET_ID_NUMBER         46
#define NET_READ_MEMO_BLOCK       52
#define NET_WRITE_MEMO_BLOCK      53

#define OK                        0
#define NET_READ_ERROR            131
#define NET_WRITE_ERROR           132

#define LPT_IBM_ALL_HASP25  0
#define LPT_IBM_ALL_HASP36  50
#define LPT_NEC_ALL_HASP36  60

#define MEMO_BUFFER_SIZE    20
#define TIME_BUFFER_SIZE    16

extern "C" {
  void hasp(int Service, int SeedCode, int LptNum, int Pass1, int Pass2, int *p1, int *p2, int *p3, int *p4);
}  */

int   LptNum, SeedCode;
int   Pass1;
int   Pass2;
int   p1, p2, p3, p4;
long  ID;

int ProgCode;   /* The program number for LOGIN */
int RetStatus;    /* The value returned from NET_LAST_STATUS */
int dummy;      /* A dummy parameter for NET_LAST_STATUS */

int login() {
//   Pass1 = 16509;
//   Pass2 = 17434;

   Pass1 = 1469;
   Pass2 = 13083;

//   ProgCode = 5;
//   ProgCode = 31;
   ProgCode = 43;
   SeedCode = 100;

   hasp(NETHASP_LOGIN, SeedCode, ProgCode, Pass1, Pass2, &p1, &p2,
        &p3, &p4);
   hasp(NETHASP_LASTSTATUS, SeedCode, ProgCode, Pass1, Pass2, &RetStatus,
        &dummy, &dummy, &dummy);


  return RetStatus;
}

int logout() {
  hasp(NETHASP_LOGOUT, SeedCode, ProgCode, Pass1, Pass2,
      &p1, &p2, &p3, &p4);
  hasp(NETHASP_LASTSTATUS, SeedCode, ProgCode, Pass1, Pass2,
      &RetStatus, &dummy, &dummy, &dummy);

  return RetStatus;
}

unsigned long GetID() {

//   Pass1 = 16509;
//   Pass2 = 17434;
//   ProgCode = 5;
//   SeedCode = 100;

  hasp(NETHASP_HASPID, SeedCode, ProgCode, Pass1, Pass2,
      &p1, &p2, &p3, &p4);
  hasp(NETHASP_LASTSTATUS, SeedCode, ProgCode, Pass1, Pass2,
      &RetStatus, &dummy, &dummy, &dummy);

//  if ( RetStatus != 0 ) return -1;
//  return 0xFFFF0000*(unsigned long)p1+p2;
  return 65536*(unsigned long)p2+p1;
}

void SetNetHaspCfgFileName() {

//   Pass1 = 16509;
//   Pass2 = 17434;
//   ProgCode = 5;
//   SeedCode = 100;
   char str[80]={"c:\\gid_lit\\nethasp.ini"};

  p2 = sizeof(str);
  hasp(85, SeedCode, ProgCode, Pass1, Pass2,
      &p1, &p2, &p3, (int *)str);
  hasp(NETHASP_LASTSTATUS, SeedCode, ProgCode, Pass1, Pass2,
      &RetStatus, &dummy, &dummy, &dummy);

//  if ( RetStatus != 0 ) return -1;
//  return 0xFFFF0000*(unsigned long)p1+p2;
}


int read_word(int n) {
  p1 = n;         /* The address from which the data word will be read */
  p2 = 0;

  hasp(NETHASP_READMEMO, SeedCode, ProgCode, Pass1, Pass2,
      &p1, &p2, &p3, &p4);
  hasp(NETHASP_LASTSTATUS, SeedCode, ProgCode, Pass1, Pass2,
      &RetStatus, &dummy, &dummy, &dummy);

  if ( RetStatus != 0 ) return -1;

  return p2;
}

int write_word(int n, int x) {
  p1 = n;     /* The address to which the data word will be written */
  p2 = x;      /* The value to be written                  */

  hasp(NETHASP_WRITEMEMO, SeedCode, ProgCode, Pass1, Pass2,
        &p1, &p2, &p3, &p4);
  hasp(NETHASP_LASTSTATUS, SeedCode, ProgCode, Pass1, Pass2,
        &RetStatus, &dummy, &dummy, &dummy);

  if ( RetStatus != 0 ) return -1;

  return 0;
}

/*
void main( void )
{

  int x;

  if (login()) return;

  if ((x = read_word(1)) == -1) return;

  x --;
  printf("%d", x);

  if (x == 0) {
    return;
  }

  if (write_word(1, x) == -1) return;

  logout();
}
*/

#if 0

void psw(int k, long l, char *s) {
  long n;
  int i;
  n = l*(k+1) & 0xFFFF;

  for (i = 0; n && i < 8; i++ ) {
    s[i] = 'A' + n%26;
    n = n/26;
  }
  s[i] = 0;
}

void psw(int k, char *s) {
  char ss[8];

  psw(k, 31991L, s);
  psw(k, 22259L, ss); strcat(s, ss);
  psw(k, 27011L, ss); strcat(s, ss);
  s[8] = 0;
}

#endif

void HaspError(int n, char *s) {
  switch (n ) {
//    case 15 : strcpy(s, "Не найден активный NetHASP сервер"); break;

    case 0 : strcpy(s, "Operation successful."); break;
    case 1 : strcpy(s, "The IPX,  NetBIOS or TCP/IP protocols have not been installed properly.  Act accordingly."); break;
    case 2 : strcpy(s, "Communication error. Unable to get socket number. Applies to IPX and TCP/IP.Verify that the protocol is installed properly."); break;
    case 3 : strcpy(s, "Communication error. NetBIOS - unable to establish session.IPX - unable to get NetHASP Server's immediate address. Verify that the protocol is installed properly."); break;
    case 4 : strcpy(s, "No NetHASP Server was found.  Check whether your software has a path and read permission to the address file."); break;
    case 5 : strcpy(s, "Can't read the NetHASP Server address file."); break;
    case 6 : strcpy(s, "Can't close the NetHASP Server address file."); break;
    case 7 : strcpy(s, "Communication error. Failed to send packet. Applies to IPX and NetBIOS.Verify that the protocol is installed properly."); break;
    case 8 : strcpy(s, "No answer from the NetHASP Server.IPX - The network is busy or incorrect address files found. In the last case, delete all copies of the HASPADDR.DAT and NEWHADDR.DAT files. NetBIOS - the network is busy."); break;
    case 10  : strcpy(s, "You called the hasp() routine with one of the services, without first calling the LOGIN service."); break;
    case 11  : strcpy(s, "Communication error. Adapter error. Applies only to NetBIOS.Verify that the protocol is installed properly."); break;
    case 15  : strcpy(s, "No active NetHASP Server was found. Applies to IPX and NetBIOS."); break;
    case 18  : strcpy(s, "Can't Login because the SET_SERVER_BY_NAME call was not successful."); break;
    case 19  : strcpy(s, "Syntax error in the NetHASP configuration file (LASTSTATUS service returns the line number in which the error occurred in Par2)."); break;
    case 20  : strcpy(s, "Error handling the NetHASP configuration file (LASTSTATUS  service returns the system error code in Par2)."); break;
    case 21  : strcpy(s, "NetHASP did not succeed in allocating memory.  This error is unique to NetHASP interfaces under Microsoft Windows and DOS extenders."); break;
    case 22  : strcpy(s, "NetHASP did not succeed in freeing memory.  This error is unique to NetHASP interfaces under Microsoft Windows and DOS extenders."); break;
    case 23  : strcpy(s, "Invalid NetHASP memory address."); break;
    case 24  : strcpy(s, "Invalid NetHASP service."); break;
    case 25  : strcpy(s, "Failed to load winsock.dll. Applies only to TCP/IP."); break;
    case 26  : strcpy(s, "Failed to unload winsock.dll. Applies only to TCP/IP."); break;
    case 129 : strcpy(s, "The appropriate NetHASP key is not connected to the NetHASP Server."); break;
    case 130 : strcpy(s, "The Program Number (Prg Num) specified is not in the Program List of the NetHASP memory."); break;
    case 131 : strcpy(s, "Error reading from the NetHASP memory."); break;
    case 132 : strcpy(s, "Error writing to the NetHASP memory."); break;
    case 133 : strcpy(s, "The current LOGIN request exceeds the number of stations which may run the software concurrently."); break;
    case 134 : strcpy(s, "The current LOGIN request exceeds the number of authorized activations for the software."); break;
    case 135 : strcpy(s, "You called the hasp() routine with the LOGOUT service without first calling the LOGIN service."); break;
    case 136 : strcpy(s, "The Server is busy.  This may occur if your NetHASP system is not well adapted to the network."); break;
    case 137 : strcpy(s, "There is no space in the user list."); break;
    case 138 : strcpy(s, "NetHASP internal error.  The Number of Authorized Stations is larger than the maximum number designated by the NetHASP model."); break;
    case 139 : strcpy(s, "The NetHASP Server crashed and was reactivated, or you called the hasp() routine with one of the services without first calling the LOGIN service."); break;
    case 140 : strcpy(s, "The NetHASP Server does not serve your station's network."); break;
    case 141 : strcpy(s, "Invalid service."); break;
    case 142 : strcpy(s, "Can't find the NetHASP Server matching the name specified in the NetHASP configuration file."); break;
    case 150 : strcpy(s, "No NetHASP Server with the assigned name was found (this error is returned by SET_SERVER_BY_NAME)."); break;
    case 151 : strcpy(s, "Two or more different NetHASP Servers with the assigned name were found (this error is returned by SET_SERVER_BY_NAME)."); break;

    case 152  : strcpy(s, "The decode operation failed because the service was not supported by the hardware."); break;
    case 155  : strcpy(s, "An old version of the License Manager has been found. The API used required a newer version of the License Manager to be installed."); break;
    case 157  : strcpy(s, "Buffer size too small for requested function."); break;
    case 158  : strcpy(s, "HASP License Manager running on machine with old HASP driver installed."); break;
    case 159  : strcpy(s, "HASP License Manager running on machine with no HASP driver installed."); break;

    default: sprintf(s, "Ошибка : %d", n);
  }
}
