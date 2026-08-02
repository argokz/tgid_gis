#include "stdafx.h"

#include <map>
using namespace std;

#pragma warning(disable:4786)
#pragma warning(disable:4503)


#include <stdio.h>
#include <windows.h>

wchar_t get_u(wchar_t u);


HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA);


void error(const char *)
{
}
#if 0

/*
 * AnsiToUnicode converts the ANSI string pszA to a Unicode string
 * and returns the Unicode string through ppszW. Space for the
 * the converted string is allocated by AnsiToUnicode.
 */ 

HRESULT __fastcall AnsiToUnicode(LPCSTR pszA, LPOLESTR* ppszW)
{

    ULONG cCharacters;
    DWORD dwError;

    // If input is null then just return the same.
    if (NULL == pszA)
    {
        *ppszW = NULL;
        return NOERROR;
    }

    // Determine number of wide characters to be allocated for the
    // Unicode string.
    cCharacters =  strlen(pszA)+1;

    // Use of the OLE allocator is required if the resultant Unicode
    // string will be passed to another COM component and if that
    // component will free it. Otherwise you can use your own allocator.
    *ppszW = (LPOLESTR) CoTaskMemAlloc(cCharacters*2);
    if (NULL == *ppszW)
        return E_OUTOFMEMORY;

    // Covert to Unicode.
    if (0 == MultiByteToWideChar(CP_ACP, 0, pszA, cCharacters,
                  *ppszW, cCharacters))
    {
        dwError = GetLastError();
        CoTaskMemFree(*ppszW);
        *ppszW = NULL;
        return HRESULT_FROM_WIN32(dwError);
    }

    return NOERROR;
}
/*
 * UnicodeToAnsi converts the Unicode string pszW to an ANSI string
 * and returns the ANSI string through ppszA. Space for the
 * the converted string is allocated by UnicodeToAnsi.
 */ 

HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA)
{

    ULONG cbAnsi, cCharacters;
    DWORD dwError;

    // If input is null then just return the same.
    if (pszW == NULL)
    {
        *ppszA = NULL;
        return NOERROR;
    }

    cCharacters = wcslen(pszW)+1;
    // Determine number of bytes to be allocated for ANSI string. An
    // ANSI string can have at most 2 bytes per character (for Double
    // Byte Character Strings.)
    cbAnsi = cCharacters*2;

    // Use of the OLE allocator is not required because the resultant
    // ANSI  string will never be passed to another COM component. You
    // can use your own allocator.
    *ppszA = (LPSTR) CoTaskMemAlloc(cbAnsi);
    if (NULL == *ppszA)
        return E_OUTOFMEMORY;

    // Convert to ANSI.
    if (0 == WideCharToMultiByte(CP_ACP, 0, pszW, cCharacters, *ppszA,
                  cbAnsi, NULL, NULL))
    {
        dwError = GetLastError();
        CoTaskMemFree(*ppszA);
        *ppszA = NULL;
        return HRESULT_FROM_WIN32(dwError);
    }
    return NOERROR;

} 

#endif

void write_utf8(unsigned code_point)
{
  if (code_point < 0x80) {
    putchar(code_point);
  } else if (code_point <= 0x7FF) {
    putchar((code_point >> 6) + 0xC0);
    putchar((code_point & 0x3F) + 0x80);
  } else if (code_point <= 0xFFFF) {
    putchar((code_point >> 12) + 0xE0);
    putchar(((code_point >> 6) & 0x3F) + 0x80);
    putchar((code_point & 0x3F) + 0x80);
  } else if (code_point <= 0x10FFFF) {
    putchar((code_point >> 18) + 0xF0);
    putchar(((code_point >> 12) & 0x3F) + 0x80);
    putchar(((code_point >> 6) & 0x3F) + 0x80);
    putchar((code_point & 0x3F) + 0x80);
  } else {
    error("invalid code_point");
  }
}


 
unsigned read_code_point_from_utf8()
{
  int code_unit1, code_unit2, code_unit3, code_unit4;
 
  code_unit1 = getchar();
  if (code_unit1 < 0x80) {
    return code_unit1;
  } else if (code_unit1 < 0xC2) {
    /* continuation or overlong 2-byte sequence */
    goto ERROR1;
  } else if (code_unit1 < 0xE0) {
    /* 2-byte sequence */
    code_unit2 = getchar();
    if ((code_unit2 & 0xC0) != 0x80) goto ERROR2;
    return (code_unit1 << 6) + code_unit2 - 0x3080;
  } else if (code_unit1 < 0xF0) {
    /* 3-byte sequence */
    code_unit2 = getchar();
    if ((code_unit2 & 0xC0) != 0x80) goto ERROR2;
    if (code_unit1 == 0xE0 && code_unit2 < 0xA0) goto ERROR2; /* overlong */
    code_unit3 = getchar();
    if ((code_unit3 & 0xC0) != 0x80) goto ERROR3;
    return (code_unit1 << 12) + (code_unit2 << 6) + code_unit3 - 0xE2080;
  } else if (code_unit1 < 0xF5) {
    /* 4-byte sequence */
    code_unit2 = getchar();
    if ((code_unit2 & 0xC0) != 0x80) goto ERROR2;
    if (code_unit1 == 0xF0 && code_unit2 < 0x90) goto ERROR2; /* overlong */
    if (code_unit1 == 0xF4 && code_unit2 >= 0x90) goto ERROR2; /* > U+10FFFF */
    code_unit3 = getchar();
    if ((code_unit3 & 0xC0) != 0x80) goto ERROR3;
    code_unit4 = getchar();
    if ((code_unit4 & 0xC0) != 0x80) goto ERROR4;
    return (code_unit1 << 18) + (code_unit2 << 12) + (code_unit3 << 6) + code_unit4 - 0x3C82080;
  } else {
    /* > U+10FFFF */
    goto ERROR1;
  }
 
  ERROR4:
    ungetc(code_unit4, stdin);
  ERROR3:
    ungetc(code_unit3, stdin);
  ERROR2:
    ungetc(code_unit2, stdin);
  ERROR1:
    return code_unit1 + 0xDC00;
}


char *write_utf8(unsigned int code_point, char *buf)
{
  if (code_point < 0x80) {
    *buf++ = code_point;
  } else if (code_point <= 0x7FF) {
    *buf++ = ((code_point >> 6) + 0xC0);
    *buf++ = ((code_point & 0x3F) + 0x80);
  } else if (code_point <= 0xFFFF) {
    *buf++ = ((code_point >> 12) + 0xE0);
    *buf++ = (((code_point >> 6) & 0x3F) + 0x80);
    *buf++ = ((code_point & 0x3F) + 0x80);
  } else if (code_point <= 0x10FFFF) {
    *buf++ = ((code_point >> 18) + 0xF0);
    *buf++ = (((code_point >> 12) & 0x3F) + 0x80);
    *buf++ = (((code_point >> 6) & 0x3F) + 0x80);
    *buf++ = ((code_point & 0x3F) + 0x80);
  } else {
    error("invalid code_point");
  }
  return buf;
}


const char * read_code_point_from_utf8(const char *bufs, unsigned short &code)
{
  unsigned short code_unit1, code_unit2, code_unit3, code_unit4;
 
  unsigned char *buf = (unsigned char *) bufs;


  code_unit1 = (unsigned short) *buf++;
  if (code_unit1 < 0x80) {
    code = code_unit1;
    return (const char *) buf;
  } else if (code_unit1 < 0xC2) {
    /* continuation or overlong 2-byte sequence */
    goto ERROR1;
  } else if (code_unit1 < 0xE0) {
    /* 2-byte sequence */
    code_unit2 = *buf++;
    if ((code_unit2 & 0xC0) != 0x80) goto ERROR2;
    code = (code_unit1 << 6) + code_unit2 - 0x3080;
    return (const char *)buf;
  } else if (code_unit1 < 0xF0) {
    /* 3-byte sequence */
    code_unit2 = *buf++;
    if ((code_unit2 & 0xC0) != 0x80) goto ERROR2;
    if (code_unit1 == 0xE0 && code_unit2 < 0xA0) goto ERROR2; /* overlong */
    code_unit3 = *buf++;
    if ((code_unit3 & 0xC0) != 0x80) goto ERROR3;
    code = (code_unit1 << 12) + (code_unit2 << 6) + code_unit3 - 0xE2080;
    return (const char *)buf;

  } else if (code_unit1 < 0xF5) {
    /* 4-byte sequence */
    code_unit2 = *buf++;
    if ((code_unit2 & 0xC0) != 0x80) goto ERROR2;
    if (code_unit1 == 0xF0 && code_unit2 < 0x90) goto ERROR2; /* overlong */
    if (code_unit1 == 0xF4 && code_unit2 >= 0x90) goto ERROR2; /* > U+10FFFF */
    code_unit3 = *buf++;
    if ((code_unit3 & 0xC0) != 0x80) goto ERROR3;
    code_unit4 = *buf++;
    if ((code_unit4 & 0xC0) != 0x80) goto ERROR4;
    code =  (code_unit1 << 18) + (code_unit2 << 12) + (code_unit3 << 6) + code_unit4 - 0x3C82080;
    return (const char *)buf;
  } else {
    /* > U+10FFFF */
    goto ERROR1;
  }
 
  ERROR4:
    buf --;
  ERROR3:
    buf --;
  ERROR2:
    buf --;
  ERROR1:
    code = code_unit1 + 0xDC00;
    return (const char *)buf;

}

#include <string.h>


wchar_t *utf8_to_u2(const char *buf1)
{
  char *buf = (char *) buf1;
  int i = 0, l = strlen(buf1);

  wchar_t *out = new wchar_t[strlen(buf1)+1];

  while (buf < buf1+l) {
    unsigned short c;
    buf = (char *) read_code_point_from_utf8(buf, c);
    out[i++] = c;
  }
  out[i] = 0;

  return out;
}


char *utf8_to_a2(const char *buf1, bool kaz)
{
  wchar_t *out = utf8_to_u2(buf1);

  if (kaz) {
      for (int i = 0; out[i]; i++) {
        out[i] = get_u(out[i]);
      }
  }
  
  char *buf2;

  UnicodeToAnsi(out, &buf2);


//  CoTaskMemFree(buf2);
 
  return buf2;
}




int utf8_to_u(const char *buf1, wchar_t *out)
{
  char *buf = (char *) buf1;
  int i = 0, l = strlen(buf1);
  while (buf < buf1+l) {
    unsigned short c;
    buf = (char *) read_code_point_from_utf8(buf, c);
    out[i++] = c;
  }
  out[i] = 0;

  return i;
}

int utf8_to_a(const char* buf1, char* a)
{
    //  wchar_t out[1024000];

    int l1 = strlen(buf1);

    wchar_t* out = new wchar_t[l1+1];

    int l = utf8_to_u(buf1, out);

    for (int i = 0; out[i]; i++) {
        out[i] = get_u(out[i]);
    }

    char* buf2;

    UnicodeToAnsi(out, &buf2);

    strcpy(a, buf2);

    CoTaskMemFree(buf2);

    delete[] out;

    return l;
}




/*
void main()
{
  unsigned short out[256];
  int l = utf8_to_u("ZweibrГјcken", out);

  for (int i = 0; out[0]; i++) {
    out[i] = get_u(out[i]);
    fprintf(stderr, "%x ", out[i]);
  }
}
*/
static int is_init = 0;
static map<wchar_t, wchar_t> map_u;

void init_u()
{
map_u.clear();

map_u[0x00C0] = 0x0041;
map_u[0x00C1] = 0x0041;
map_u[0x00C2] = 0x0041;
map_u[0x00C3] = 0x0041;
map_u[0x00C4] = 0x0041;
map_u[0x00C5] = 0x0041;
map_u[0x00C6] = 0x0041;
map_u[0x00C7] = 0x0043;
map_u[0x00C8] = 0x0045;
map_u[0x00C9] = 0x0045;
map_u[0x00CA] = 0x0045;
map_u[0x00CB] = 0x0045;
map_u[0x00CC] = 0x0049;
map_u[0x00CD] = 0x0049;
map_u[0x00CE] = 0x0049;
map_u[0x00CF] = 0x0049;
map_u[0x00D1] = 0x004E;
map_u[0x00D2] = 0x004F;
map_u[0x00D3] = 0x004F;
map_u[0x00D4] = 0x004F;
map_u[0x00D5] = 0x004F;
map_u[0x00D6] = 0x004F;
map_u[0x00D9] = 0x0055;
map_u[0x00DA] = 0x0055;
map_u[0x00DB] = 0x0055;
map_u[0x00DC] = 0x0055;
map_u[0x00DD] = 0x0059;
map_u[0x00E0] = 0x0061;
map_u[0x00E1] = 0x0061;
map_u[0x00E2] = 0x0061;
map_u[0x00E3] = 0x0061;
map_u[0x00E4] = 0x0061;
map_u[0x00E5] = 0x0061;
map_u[0x00E6] = 0x0041;
map_u[0x00E7] = 0x0063;
map_u[0x00E8] = 0x0065;
map_u[0x00E9] = 0x0065;
map_u[0x00EA] = 0x0065;
map_u[0x00EB] = 0x0065;
map_u[0x00EC] = 0x0069;
map_u[0x00ED] = 0x0069;
map_u[0x00EE] = 0x0069;
map_u[0x00EF] = 0x0069;
map_u[0x00F1] = 0x006E;
map_u[0x00F2] = 0x006F;
map_u[0x00F3] = 0x006F;
map_u[0x00F4] = 0x006F;
map_u[0x00F5] = 0x006F;
map_u[0x00F6] = 0x006F;
map_u[0x00F9] = 0x0075;
map_u[0x00FA] = 0x0075;
map_u[0x00FB] = 0x0075;
map_u[0x00FC] = 0x0075;
map_u[0x00FD] = 0x0079;
map_u[0x00FF] = 0x0079;
map_u[0x0100] = 0x0041;
map_u[0x0101] = 0x0061;
map_u[0x0102] = 0x0041;
map_u[0x0103] = 0x0061;
map_u[0x0104] = 0x0041;
map_u[0x0105] = 0x0061;
map_u[0x0106] = 0x0043;
map_u[0x0107] = 0x0063;
map_u[0x0108] = 0x0043;
map_u[0x0109] = 0x0063;
map_u[0x010A] = 0x0043;
map_u[0x010B] = 0x0063;
map_u[0x010C] = 0x0043;
map_u[0x010D] = 0x0063;
map_u[0x010E] = 0x0044;
map_u[0x010F] = 0x0064;
map_u[0x0112] = 0x0045;
map_u[0x0113] = 0x0065;
map_u[0x0114] = 0x0045;
map_u[0x0115] = 0x0065;
map_u[0x0116] = 0x0045;
map_u[0x0117] = 0x0065;
map_u[0x0118] = 0x0045;
map_u[0x0119] = 0x0065;
map_u[0x011A] = 0x0045;
map_u[0x011B] = 0x0065;
map_u[0x011C] = 0x0047;
map_u[0x011D] = 0x0067;
map_u[0x011E] = 0x0047;
map_u[0x011F] = 0x0067;
map_u[0x0120] = 0x0047;
map_u[0x0121] = 0x0067;
map_u[0x0122] = 0x0047;
map_u[0x0123] = 0x0067;
map_u[0x0124] = 0x0048;
map_u[0x0125] = 0x0068;
map_u[0x0128] = 0x0049;
map_u[0x0129] = 0x0069;
map_u[0x012A] = 0x0049;
map_u[0x012B] = 0x0069;
map_u[0x012C] = 0x0049;
map_u[0x012D] = 0x0069;
map_u[0x012E] = 0x0049;
map_u[0x012F] = 0x0069;
map_u[0x0130] = 0x0049;
map_u[0x0131] = 0x0069;
map_u[0x0134] = 0x004A;
map_u[0x0135] = 0x006A;
map_u[0x0136] = 0x004B;
map_u[0x0137] = 0x006B;
map_u[0x0139] = 0x004C;
map_u[0x013A] = 0x006C;
map_u[0x013B] = 0x004C;
map_u[0x013C] = 0x006C;
map_u[0x013D] = 0x004C;
map_u[0x013E] = 0x006C;
map_u[0x0143] = 0x004E;
map_u[0x0144] = 0x006E;
map_u[0x0145] = 0x004E;
map_u[0x0146] = 0x006E;
map_u[0x0147] = 0x004E;
map_u[0x0148] = 0x006E;
map_u[0x014C] = 0x004F;
map_u[0x014D] = 0x006F;
map_u[0x014E] = 0x004F;
map_u[0x014F] = 0x006F;
map_u[0x0150] = 0x004F;
map_u[0x0151] = 0x006F;
map_u[0x0154] = 0x0052;
map_u[0x0155] = 0x0072;
map_u[0x0156] = 0x0052;
map_u[0x0157] = 0x0072;
map_u[0x0158] = 0x0052;
map_u[0x0159] = 0x0072;
map_u[0x015A] = 0x0053;
map_u[0x015B] = 0x0073;
map_u[0x015C] = 0x0053;
map_u[0x015D] = 0x0073;
map_u[0x015E] = 0x0053;
map_u[0x015F] = 0x0073;
map_u[0x0160] = 0x0053;
map_u[0x0161] = 0x0073;
map_u[0x0162] = 0x0054;
map_u[0x0163] = 0x0074;
map_u[0x0164] = 0x0054;
map_u[0x0165] = 0x0074;
map_u[0x0168] = 0x0055;
map_u[0x0169] = 0x0075;
map_u[0x016A] = 0x0055;
map_u[0x016B] = 0x0075;
map_u[0x016C] = 0x0055;
map_u[0x016D] = 0x0075;
map_u[0x016E] = 0x0055;
map_u[0x016F] = 0x0075;
map_u[0x0170] = 0x0055;
map_u[0x0171] = 0x0075;
map_u[0x0172] = 0x0055;
map_u[0x0173] = 0x0075;
map_u[0x0174] = 0x0057;
map_u[0x0175] = 0x0077;
map_u[0x0176] = 0x0059;
map_u[0x0177] = 0x0079;
map_u[0x0178] = 0x0059;
map_u[0x0179] = 0x005A;
map_u[0x017A] = 0x007A;
map_u[0x017B] = 0x005A;
map_u[0x017C] = 0x007A;
map_u[0x017D] = 0x005A;
map_u[0x017E] = 0x007A;
map_u[0x01A0] = 0x004F;
map_u[0x01A1] = 0x006F;
map_u[0x01AF] = 0x0055;
map_u[0x01B0] = 0x0075;
map_u[0x01CD] = 0x0041;
map_u[0x01CE] = 0x0061;
map_u[0x01CF] = 0x0049;
map_u[0x01D0] = 0x0069;
map_u[0x01D1] = 0x004F;
map_u[0x01D2] = 0x006F;
map_u[0x01D3] = 0x0055;
map_u[0x01D4] = 0x0075;
map_u[0x01D5] = 0x00DC;
map_u[0x01D6] = 0x00FC;
map_u[0x01D7] = 0x00DC;
map_u[0x01D8] = 0x00FC;
map_u[0x01D9] = 0x00DC;
map_u[0x01DA] = 0x00FC;
map_u[0x01DB] = 0x00DC;
map_u[0x01DC] = 0x00FC;
map_u[0x01DE] = 0x00C4;
map_u[0x01DF] = 0x00E4;
map_u[0x01E0] = 0x0226;
map_u[0x01E1] = 0x0227;
map_u[0x01E2] = 0x00C6;
map_u[0x01E3] = 0x00E6;
map_u[0x01E6] = 0x0047;
map_u[0x01E7] = 0x0067;
map_u[0x01E8] = 0x004B;
map_u[0x01E9] = 0x006B;
map_u[0x01EA] = 0x004F;
map_u[0x01EB] = 0x006F;
map_u[0x01EC] = 0x01EA;
map_u[0x01ED] = 0x01EB;
map_u[0x01EE] = 0x01B7;
map_u[0x01EF] = 0x0292;
map_u[0x01F0] = 0x006A;
map_u[0x01F4] = 0x0047;
map_u[0x01F5] = 0x0067;
map_u[0x01F8] = 0x004E;
map_u[0x01F9] = 0x006E;
map_u[0x01FA] = 0x00C5;
map_u[0x01FB] = 0x00E5;
map_u[0x01FC] = 0x00C6;
map_u[0x01FD] = 0x00E6;
map_u[0x01FE] = 0x00D8;
map_u[0x01FF] = 0x00F8;
map_u[0x0200] = 0x0041;
map_u[0x0201] = 0x0061;
map_u[0x0202] = 0x0041;
map_u[0x0203] = 0x0061;
map_u[0x0204] = 0x0045;
map_u[0x0205] = 0x0065;
map_u[0x0206] = 0x0045;
map_u[0x0207] = 0x0065;
map_u[0x0208] = 0x0049;
map_u[0x0209] = 0x0069;
map_u[0x020A] = 0x0049;
map_u[0x020B] = 0x0069;
map_u[0x020C] = 0x004F;
map_u[0x020D] = 0x006F;
map_u[0x020E] = 0x004F;
map_u[0x020F] = 0x006F;
map_u[0x0210] = 0x0052;
map_u[0x0211] = 0x0072;
map_u[0x0212] = 0x0052;
map_u[0x0213] = 0x0072;
map_u[0x0214] = 0x0055;
map_u[0x0215] = 0x0075;
map_u[0x0216] = 0x0055;
map_u[0x0217] = 0x0075;
map_u[0x0218] = 0x0053;
map_u[0x0219] = 0x0073;
map_u[0x021A] = 0x0054;
map_u[0x021B] = 0x0074;
map_u[0x021E] = 0x0048;
map_u[0x021F] = 0x0068;
map_u[0x0226] = 0x0041;
map_u[0x0227] = 0x0061;
map_u[0x0228] = 0x0045;
map_u[0x0229] = 0x0065;
map_u[0x022A] = 0x00D6;
map_u[0x022B] = 0x00F6;
map_u[0x022C] = 0x00D5;
map_u[0x022D] = 0x00F5;
map_u[0x022E] = 0x004F;
map_u[0x022F] = 0x006F;
map_u[0x0230] = 0x022E;
map_u[0x0231] = 0x022F;
map_u[0x0232] = 0x0059;
map_u[0x0233] = 0x0079;
map_u[0x1E00] = 0x0041;
map_u[0x1E01] = 0x0061;
map_u[0x1E02] = 0x0042;
map_u[0x1E03] = 0x0062;
map_u[0x1E04] = 0x0042;
map_u[0x1E05] = 0x0062;
map_u[0x1E06] = 0x0042;
map_u[0x1E07] = 0x0062;
map_u[0x1E08] = 0x00C7;
map_u[0x1E09] = 0x00E7;
map_u[0x1E0A] = 0x0044;
map_u[0x1E0B] = 0x0064;
map_u[0x1E0C] = 0x0044;
map_u[0x1E0D] = 0x0064;
map_u[0x1E0E] = 0x0044;
map_u[0x1E0F] = 0x0064;
map_u[0x1E10] = 0x0044;
map_u[0x1E11] = 0x0064;
map_u[0x1E12] = 0x0044;
map_u[0x1E13] = 0x0064;
map_u[0x1E14] = 0x0112;
map_u[0x1E15] = 0x0113;
map_u[0x1E16] = 0x0112;
map_u[0x1E17] = 0x0113;
map_u[0x1E18] = 0x0045;
map_u[0x1E19] = 0x0065;
map_u[0x1E1A] = 0x0045;
map_u[0x1E1B] = 0x0065;
map_u[0x1E1C] = 0x0228;
map_u[0x1E1D] = 0x0229;
map_u[0x1E1E] = 0x0046;
map_u[0x1E1F] = 0x0066;
map_u[0x1E20] = 0x0047;
map_u[0x1E21] = 0x0067;
map_u[0x1E22] = 0x0048;
map_u[0x1E23] = 0x0068;
map_u[0x1E24] = 0x0048;
map_u[0x1E25] = 0x0068;
map_u[0x1E26] = 0x0048;
map_u[0x1E27] = 0x0068;
map_u[0x1E28] = 0x0048;
map_u[0x1E29] = 0x0068;
map_u[0x1E2A] = 0x0048;
map_u[0x1E2B] = 0x0068;
map_u[0x1E2C] = 0x0049;
map_u[0x1E2D] = 0x0069;
map_u[0x1E2E] = 0x00CF;
map_u[0x1E2F] = 0x00EF;
map_u[0x1E30] = 0x004B;
map_u[0x1E31] = 0x006B;
map_u[0x1E32] = 0x004B;
map_u[0x1E33] = 0x006B;
map_u[0x1E34] = 0x004B;
map_u[0x1E35] = 0x006B;
map_u[0x1E36] = 0x004C;
map_u[0x1E37] = 0x006C;
map_u[0x1E38] = 0x1E36;
map_u[0x1E39] = 0x1E37;
map_u[0x1E3A] = 0x004C;
map_u[0x1E3B] = 0x006C;
map_u[0x1E3C] = 0x004C;
map_u[0x1E3D] = 0x006C;
map_u[0x1E3E] = 0x004D;
map_u[0x1E3F] = 0x006D;
map_u[0x1E40] = 0x004D;
map_u[0x1E41] = 0x006D;
map_u[0x1E42] = 0x004D;
map_u[0x1E43] = 0x006D;
map_u[0x1E44] = 0x004E;
map_u[0x1E45] = 0x006E;
map_u[0x1E46] = 0x004E;
map_u[0x1E47] = 0x006E;
map_u[0x1E48] = 0x004E;
map_u[0x1E49] = 0x006E;
map_u[0x1E4A] = 0x004E;
map_u[0x1E4B] = 0x006E;
map_u[0x1E4C] = 0x00D5;
map_u[0x1E4D] = 0x00F5;
map_u[0x1E4E] = 0x00D5;
map_u[0x1E4F] = 0x00F5;
map_u[0x1E50] = 0x014C;
map_u[0x1E51] = 0x014D;
map_u[0x1E52] = 0x014C;
map_u[0x1E53] = 0x014D;
map_u[0x1E54] = 0x0050;
map_u[0x1E55] = 0x0070;
map_u[0x1E56] = 0x0050;
map_u[0x1E57] = 0x0070;
map_u[0x1E58] = 0x0052;
map_u[0x1E59] = 0x0072;
map_u[0x1E5A] = 0x0052;
map_u[0x1E5B] = 0x0072;
map_u[0x1E5C] = 0x1E5A;
map_u[0x1E5D] = 0x1E5B;
map_u[0x1E5E] = 0x0052;
map_u[0x1E5F] = 0x0072;
map_u[0x1E60] = 0x0053;
map_u[0x1E61] = 0x0073;
map_u[0x1E62] = 0x0053;
map_u[0x1E63] = 0x0073;
map_u[0x1E64] = 0x015A;
map_u[0x1E65] = 0x015B;
map_u[0x1E66] = 0x0160;
map_u[0x1E67] = 0x0161;
map_u[0x1E68] = 0x1E62;
map_u[0x1E69] = 0x1E63;
map_u[0x1E6A] = 0x0054;
map_u[0x1E6B] = 0x0074;
map_u[0x1E6C] = 0x0054;
map_u[0x1E6D] = 0x0074;
map_u[0x1E6E] = 0x0054;
map_u[0x1E6F] = 0x0074;
map_u[0x1E70] = 0x0054;
map_u[0x1E71] = 0x0074;
map_u[0x1E72] = 0x0055;
map_u[0x1E73] = 0x0075;
map_u[0x1E74] = 0x0055;
map_u[0x1E75] = 0x0075;
map_u[0x1E76] = 0x0055;
map_u[0x1E77] = 0x0075;
map_u[0x1E78] = 0x0168;
map_u[0x1E79] = 0x0169;
map_u[0x1E7A] = 0x016A;
map_u[0x1E7B] = 0x016B;
map_u[0x1E7C] = 0x0056;
map_u[0x1E7D] = 0x0076;
map_u[0x1E7E] = 0x0056;
map_u[0x1E7F] = 0x0076;
map_u[0x1E80] = 0x0057;
map_u[0x1E81] = 0x0077;
map_u[0x1E82] = 0x0057;
map_u[0x1E83] = 0x0077;
map_u[0x1E84] = 0x0057;
map_u[0x1E85] = 0x0077;
map_u[0x1E86] = 0x0057;
map_u[0x1E87] = 0x0077;
map_u[0x1E88] = 0x0057;
map_u[0x1E89] = 0x0077;
map_u[0x1E8A] = 0x0058;
map_u[0x1E8B] = 0x0078;
map_u[0x1E8C] = 0x0058;
map_u[0x1E8D] = 0x0078;
map_u[0x1E8E] = 0x0059;
map_u[0x1E8F] = 0x0079;
map_u[0x1E90] = 0x005A;
map_u[0x1E91] = 0x007A;
map_u[0x1E92] = 0x005A;
map_u[0x1E93] = 0x007A;
map_u[0x1E94] = 0x005A;
map_u[0x1E95] = 0x007A;
map_u[0x1E96] = 0x0068;
map_u[0x1E97] = 0x0074;
map_u[0x1E98] = 0x0077;
map_u[0x1E99] = 0x0079;
map_u[0x1EA0] = 0x0041;
map_u[0x1EA1] = 0x0061;
map_u[0x1EA2] = 0x0041;
map_u[0x1EA3] = 0x0061;
map_u[0x1EA4] = 0x00C2;
map_u[0x1EA5] = 0x00E2;
map_u[0x1EA6] = 0x00C2;
map_u[0x1EA7] = 0x00E2;
map_u[0x1EA8] = 0x00C2;
map_u[0x1EA9] = 0x00E2;
map_u[0x1EAA] = 0x00C2;
map_u[0x1EAB] = 0x00E2;
map_u[0x1EAC] = 0x1EA0;
map_u[0x1EAD] = 0x1EA1;
map_u[0x1EAE] = 0x0102;
map_u[0x1EAF] = 0x0103;
map_u[0x1EB0] = 0x0102;
map_u[0x1EB1] = 0x0103;
map_u[0x1EB2] = 0x0102;
map_u[0x1EB3] = 0x0103;
map_u[0x1EB4] = 0x0102;
map_u[0x1EB5] = 0x0103;
map_u[0x1EB6] = 0x1EA0;
map_u[0x1EB7] = 0x1EA1;
map_u[0x1EB8] = 0x0045;
map_u[0x1EB9] = 0x0065;
map_u[0x1EBA] = 0x0045;
map_u[0x1EBB] = 0x0065;
map_u[0x1EBC] = 0x0045;
map_u[0x1EBD] = 0x0065;
map_u[0x1EBE] = 0x00CA;
map_u[0x1EBF] = 0x00EA;
map_u[0x1EC0] = 0x00CA;
map_u[0x1EC1] = 0x00EA;
map_u[0x1EC2] = 0x00CA;
map_u[0x1EC3] = 0x00EA;
map_u[0x1EC4] = 0x00CA;
map_u[0x1EC5] = 0x00EA;
map_u[0x1EC6] = 0x1EB8;
map_u[0x1EC7] = 0x1EB9;
map_u[0x1EC8] = 0x0049;
map_u[0x1EC9] = 0x0069;
map_u[0x1ECA] = 0x0049;
map_u[0x1ECB] = 0x0069;
map_u[0x1ECC] = 0x004F;
map_u[0x1ECD] = 0x006F;
map_u[0x1ECE] = 0x004F;
map_u[0x1ECF] = 0x006F;
map_u[0x1ED0] = 0x00D4;
map_u[0x1ED1] = 0x00F4;
map_u[0x1ED2] = 0x00D4;
map_u[0x1ED3] = 0x00F4;
map_u[0x1ED4] = 0x00D4;
map_u[0x1ED5] = 0x00F4;
map_u[0x1ED6] = 0x00D4;
map_u[0x1ED7] = 0x00F4;
map_u[0x1ED8] = 0x1ECC;
map_u[0x1ED9] = 0x1ECD;
map_u[0x1EDA] = 0x01A0;
map_u[0x1EDB] = 0x01A1;
map_u[0x1EDC] = 0x01A0;
map_u[0x1EDD] = 0x01A1;
map_u[0x1EDE] = 0x01A0;
map_u[0x1EDF] = 0x01A1;
map_u[0x1EE0] = 0x01A0;
map_u[0x1EE1] = 0x01A1;
map_u[0x1EE2] = 0x01A0;
map_u[0x1EE3] = 0x01A1;
map_u[0x1EE4] = 0x0055;
map_u[0x1EE5] = 0x0075;
map_u[0x1EE6] = 0x0055;
map_u[0x1EE7] = 0x0075;
map_u[0x1EE8] = 0x01AF;
map_u[0x1EE9] = 0x01B0;
map_u[0x1EEA] = 0x01AF;
map_u[0x1EEB] = 0x01B0;
map_u[0x1EEC] = 0x01AF;
map_u[0x1EED] = 0x01B0;
map_u[0x1EEE] = 0x01AF;
map_u[0x1EEF] = 0x01B0;
map_u[0x1EF0] = 0x01AF;
map_u[0x1EF1] = 0x01B0;
map_u[0x1EF2] = 0x0059;
map_u[0x1EF3] = 0x0079;
map_u[0x1EF4] = 0x0059;
map_u[0x1EF5] = 0x0079;
map_u[0x1EF6] = 0x0059;
map_u[0x1EF7] = 0x0079;
map_u[0x1EF8] = 0x0059;
map_u[0x1EF9] = 0x0079;


// РљР°Р·Р°С…СЃРєРёРµ Р±СѓРєРІС‹

map_u[1240] = 0x42D;  //У
map_u[1241] = 0x44D;  //У™
map_u[1170] = 0x413;  //Т’
map_u[1171] = 0x433;  //Т“
map_u[1178] = 0x41A;  //Тљ
map_u[1179] = 0x43A;  //Т›
map_u[1186] = 0x41D;  //Тў
map_u[1187] = 0x43D;  //ТЈ
map_u[1256] = 0x41E;  //УЁ
map_u[1257] = 0x43E;  //У©
map_u[1200] = 0x423;  //Т°
map_u[1201] = 0x443;  //Т±
map_u[1198] = 0x423;  //Т®
map_u[1199] = 0x443;  //ТЇ
map_u[1210] = 0x48;  //Тє
map_u[1211] = 0x68;  //Т» 
map_u[1030] = 0x49;  //Р†
map_u[1110] = 0x69;  //С–

}

wchar_t get_u(wchar_t u)
{
    if (!is_init) {
        init_u();
        is_init = 1;
    }

    if (u > 128) {
//    printf ("\n%x = %d\n", u, u);
//    exit(0);
    }
  
    map<wchar_t, wchar_t>::const_iterator it = map_u.find((int)u);

    if (it != map_u.end()) {
//    printf ("\n%d -> %d\n", u, it->second);
//    exit(0);
        u = it->second;
        return u;
    }
    return u;
}


#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <windows.h>

// Преобразование строки из ANSI в UTF-8
std::string ansi_to_utf8(const std::string& ansiStr) {
    // Определяем кодировку системы (например, CP1251 для русского языка)
    UINT codePage = 1251;
    
    // Вычисляем количество символов для преобразования в UTF-16 (широкие символы)
    int wideCharCount = MultiByteToWideChar(codePage, 0, ansiStr.c_str(), -1, nullptr, 0);
    if (wideCharCount == 0) {
        throw std::runtime_error("Error converting ANSI to UTF-16");
    }

    // Преобразуем ANSI строку в широкую строку (UTF-16)
    std::wstring wideStr(wideCharCount, 0);
    MultiByteToWideChar(codePage, 0, ansiStr.c_str(), -1, &wideStr[0], wideCharCount);

    // Преобразуем широкую строку (UTF-16) в строку UTF-8
    int utf8CharCount = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8CharCount == 0) {
        throw std::runtime_error("Error converting UTF-16 to UTF-8");
    }

    std::string utf8Str(utf8CharCount, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], utf8CharCount, nullptr, nullptr);
    
    return utf8Str;
}

// Преобразование строки из UTF-8 в ANSI
std::string utf8_to_ansi(const std::string& utf8Str) {
    // Преобразуем строку UTF-8 в широкую строку (UTF-16)
    int wideCharCount = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    if (wideCharCount == 0) {
        throw std::runtime_error("Error converting UTF-8 to UTF-16");
    }

    std::wstring wideStr(wideCharCount, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideCharCount);

    // Преобразуем широкую строку (UTF-16) в ANSI
    UINT codePage = 1251;  // Например, CP1251 для русского языка
    int ansiCharCount = WideCharToMultiByte(codePage, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (ansiCharCount == 0) {
        throw std::runtime_error("Error converting UTF-16 to ANSI");
    }

    std::string ansiStr(ansiCharCount, 0);
    WideCharToMultiByte(codePage, 0, wideStr.c_str(), -1, &ansiStr[0], ansiCharCount, nullptr, nullptr);
    
    return ansiStr;
}

/*
int main() {
    std::string ansiStr = "Пример ANSI строки";
    std::string utf8Str = ansi_to_utf8(ansiStr);
    
    std::cout << "UTF-8: " << utf8Str << std::endl;
    
    std::string backToAnsi = utf8_to_ansi(utf8Str);
    std::cout << "ANSI: " << backToAnsi << std::endl;

    return 0;
}
*/