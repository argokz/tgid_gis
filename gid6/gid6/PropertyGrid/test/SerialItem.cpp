#include "StdAfx.h"
#include ".\serialitem.h"

CSerialItem::CSerialItem(void)
{
  m_serial = "12348765";
}

CSerialItem::~CSerialItem(void)
{
}

CPropertyGrid::EEditMode CSerialItem::GetEditMode()
{
  return CPropertyGrid::EM_INPLACE;
}

void CSerialItem::DrawItem(CDC& dc, CRect rc, bool focused)
{
  CString serial = m_serial;
  while (serial.GetLength()<8) serial += " ";
  serial = serial.Mid(0,4) + "-" + serial.Mid(4,4);
  rc.left += m_pGrid->GetTextMargin();
  dc.DrawText(serial, rc, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
}

CString CSerialItem::GetStringForInPlaceEdit()
{
  return m_serial;
}

bool CSerialItem::OnItemEdited(CString strNewValue)
{
  if (strNewValue.GetLength()!=8)
  {
    AfxMessageBox("Invalid serial number", MB_OK|MB_ICONINFORMATION);
    return false;
  }
  else
  {
    m_serial = strNewValue;
    return true;
  }
}
