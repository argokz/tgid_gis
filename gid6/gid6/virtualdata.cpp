#include "stdafx.h"
#include "virtualdata.h"

CString CVirtListData::GetItemText(int nSubItem, int nIndex, int first, int last)
{
  CString sText;
  switch(nSubItem)
  {
  case 0:     
    sText.Format( "Item no.%d", nIndex+1);
    break;
  case 1:     
    sText.Format( "SubItem no.%d", nIndex+1);
    break;
  case 2:       
    sText.Format( nIndex%2 ? "Decimal: %d": "Hex: %X" , nIndex+1);
    break;
  }

  sText.Format( "%d %d", nIndex+1, nSubItem+1);


  return sText;
}



int CVirtListData::GetImage(int nIndex)
{
  return nIndex%3 - 1;
}

int CVirtListData::GetStateImage(int nIndex)
{
  // Ќумераци€ иконок состо€ни€ начинаетс€ с единицы
  return INDEXTOSTATEIMAGEMASK(nIndex%2 + 1); 
}

int CVirtListData::GetCount()
{ 
  return 1000000; 
}


int CVirtListData::GetNFlds()
{
  return 10;
}


CString CVirtListData::GetFieldName(int i)
{
  CString qq;
  qq.Format("Field %d", i);
  return qq;
}


int CVirtListData::GetFieldWidth(int i)
{
  return 100;
}

void * CVirtListData::getData(int row)
{
  return NULL;
}

