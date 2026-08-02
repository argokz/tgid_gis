#if !defined(_CRACK_H)
#define _CRACK_H

// crack.h

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

class CCrack
{
public:
  static LPCTSTR strFieldType(short sType);
  static LPCTSTR strFieldVType(short sType);
  static LPCTSTR strQueryDefType(short sType);
  static LPCTSTR strBOOL(BOOL bFlag);
  static CString strVARIANT(const COleVariant& var);
  static CString strVARIANT_UTF(const COleVariant& var);
  static CString strVARIANT2(const COleVariant& var);
  static CString strVARIANT3(const COleVariant& var);
  static CString strVARIANT(const COleVariant& var, int width);
  static CString strVARIANT2(const COleVariant& var, int width);
  static COleVariant Var(CString& s, short sType);
  static COleVariant Var2(CString& s, short sType);
};
#endif // !defined(_CRACK_H)
