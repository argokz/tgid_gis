#pragma once
#include "..\CustomItem.h"

class CSerialItem : public ICustomItem
{
public:
  CSerialItem(void);
  ~CSerialItem(void);
  virtual CPropertyGrid::EEditMode GetEditMode();
  virtual void DrawItem(CDC& dc, CRect rc, bool focused);
  virtual CString GetStringForInPlaceEdit();
  virtual bool OnItemEdited(CString strNewValue);

protected:
  CString m_serial;
};
