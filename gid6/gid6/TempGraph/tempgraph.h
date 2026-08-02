#pragma once

/////////////////////////////////////////////////////////////////////////////
// CTempGraph dialog
//#include "main_tempgraph.h"
//#include "ish_tempgraph.h"
//#include "graph_tempgraph.h"
//#include "graph_tempgraph2.h"

#include "otops.h"
#include "povs.h"
#include "sks.h"
//#include "ERROR.RH"

#define IDS_ERROR                       500

class CAdoFile;

enum TFeature{Otop,Pov,SkkPov,SkkPon};

struct COMMON_TEMP_STRUCTURE 
{
  TFeature feat;
  OTOP * ot;
  OTDATA * ot_data;
  POV * pov;
  POVDATA * pov_data;
  SKK * skk;
  SKDATA * skk_data;
  long kol_array;
  double ** array_ptr;
  long kol;
  long kol_addon;
  CString * name_str;
  CString * ed_str;
  CString * addon;
  
  COMMON_TEMP_STRUCTURE()  
  {
    memset( this, 0, sizeof(COMMON_TEMP_STRUCTURE)); 
    name_str=NULL;
    ed_str=NULL;    
    addon=NULL;   
  }
  void DeleteStruct(void * ptr)
  {
    if (ptr!=0) 
    {
      delete[] ptr;
      ptr=0;
    }
  }
  void DeleteStrings(CString * ptr)
  {
    if (ptr!=NULL)
    {
      delete[] ptr;
      ptr=NULL;
    }
  }
  ~COMMON_TEMP_STRUCTURE()
  {
    DeleteStruct(ot);
    DeleteStruct(ot_data);
    DeleteStruct(pov);
    DeleteStruct(pov_data);
    DeleteStruct(skk);
    DeleteStruct(skk_data);
    DeleteStruct(array_ptr);
    DeleteStrings(name_str);
    DeleteStrings(ed_str);
    DeleteStrings(addon);   
  }
  void Init()
  {
    switch (feat)
    {
    case Otop:
      {
        DeleteStruct(ot_data);
        ot_data=new OTDATA;         
        break;
      }
    case Pov:
      {
        DeleteStruct(pov_data);
        pov_data=new POVDATA; 
        break;
      }
    case SkkPov:
    case SkkPon:
      {
        DeleteStruct(skk_data);
        skk_data=new SKDATA; 
        break;
      }
    }   

  }
  void InitAddonStrings(long quantity)
  {
    kol_addon=quantity;
    DeleteStrings(addon);
    addon=new CString[kol_addon];
  }
  void InitStrings(long quantity)
  {
    kol=quantity;
    DeleteStrings(name_str);
    DeleteStrings(ed_str);
    name_str=new CString[kol];
    ed_str=new CString[kol];
  }
  void InitArray(long quantity)
  {
    kol_array=quantity;
    DeleteStruct(array_ptr);
    array_ptr=new double*[kol_array];
  }
  
};

class CTempGraph
{
// Construction
public:
  CTempGraph(CAdoFile *ado, int id, bool standart);   // standard constructor
  ~CTempGraph(void);  
  COMMON_TEMP_STRUCTURE * TempStruct;
  COMMON_TEMP_STRUCTURE * TempStruct_norm;
  BOOL Calculate();
  BOOL calculated;
  void RedrawChart();
  void CTempGraph::defaultLoadTempGraph(CAdoFile* ado, int id); //стандартная загрузка tempGraph
  void CTempGraph::ogLoadTempGraph(CAdoFile* ado, int id); //загрузка tempGraph для отдельного отпоительного температурного графика
  void SetLines(BOOL c1, BOOL c2, BOOL c3, BOOL c4);

// Implementation
protected:
  CString Tip_name;
  void AddError(UINT uID, CString& sError);
  bool CheckInputOT(/*int cxema,*/ OTOP& ot, CString& sError );
  bool CalculateOT(OTOP &tg, OTDATA& data);
  bool CheckInputPOV(/*int cxema,*/ POV& ot, CString& sError );
  bool CalculatePOV(POV &tg, POVDATA& data);
  bool CheckInputSK(/*int cxema,*/ SKK& sk, CString& sError );
  bool CalculateSK(SKK &tg, SKDATA& data);
  bool CalculateOT1(OTOP& tg, OTDATA& data, double tn, double &tau01, double& tau02, double& tau03, double& tb, double &Q0, double &tau01v, bool is_srezki);
  double find_izlom(OTOP& tg, OTDATA& data, double t_izlom);
};

