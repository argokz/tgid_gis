#pragma once

/////////////////////////////////////////////////////////////////////////////
// CTempGraph dialog
//#include "main_tempgraph.h"
//#include "ish_tempgraph.h"
//#include "graph_tempgraph.h"
//#include "graph_tempgraph2.h"

#include <win.h>

#include "otops.h"
#include "povs.h"
#include "sks.h"
#include <cstring>
//#include "ERROR.RH"

#define IDS_ERROR                       500

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
  QString * name_str;
  QString * ed_str;
  QString * addon;
  
  COMMON_TEMP_STRUCTURE()  
  {
    memset( this, 0, sizeof(COMMON_TEMP_STRUCTURE)); 
    name_str=nullptr;
    ed_str=nullptr;    
    addon=nullptr;   
  }

  ~COMMON_TEMP_STRUCTURE()
  {
    delete ot;
    delete ot_data;
    delete pov;
    delete pov_data;
    delete skk;
    delete skk_data;
    delete [] array_ptr;

    delete [] name_str;
    delete [] ed_str;
    delete [] addon;   
  }
  void Init()
  {
    switch (feat)
    {
    case Otop:
      {
        delete ot_data;
        ot_data=new OTDATA;         
        break;
      }
    case Pov:
      {
        delete pov_data;
        pov_data=new POVDATA; 
        break;
      }
    case SkkPov:
    case SkkPon:
      {
        delete skk_data;
        skk_data=new SKDATA; 
        break;
      }
    }   

  }
  void InitAddonStrings(long quantity)
  {
    kol_addon=quantity;
    delete [] addon;
    addon=new QString[kol_addon];
  }
  void InitStrings(long quantity)
  {
    kol=quantity;
    delete [] name_str;
    delete [] ed_str;
    name_str=new QString[kol];
    ed_str=new QString[kol];
  }
  void InitArray(long quantity)
  {
    kol_array=quantity;
    delete [] array_ptr;
    array_ptr=new double*[kol_array];
  }
  
};

class CTempGraph
{
// Construction
public:
  CTempGraph(QSqlDatabase & db, QSqlQuery &query, int id, bool standart);   // standard constructor
  ~CTempGraph(void);  
  COMMON_TEMP_STRUCTURE * TempStruct;
  COMMON_TEMP_STRUCTURE * TempStruct_norm;
  bool Calculate();
  bool calculated;
  void RedrawChart();
  void defaultLoadTempGraph(QSqlDatabase & db, QSqlQuery &query, int id); //стандартная загрузка tempGraph
  void ogLoadTempGraph(QSqlDatabase & db, QSqlQuery &query, int id); //загрузка tempGraph для отдельного отпоительного температурного графика
  void SetLines(bool c1, bool c2, bool c3, bool c4);

// Implementation
protected:
  QString Tip_name;
  void AddError(UINT uID, QString& sError);
  bool CheckInputOT(/*int cxema,*/ OTOP& ot, QString& sError );
  bool CalculateOT(OTOP &tg, OTDATA& data);
  bool CheckInputPOV(/*int cxema,*/ POV& ot, QString& sError );
  bool CalculatePOV(POV &tg, POVDATA& data);
  bool CheckInputSK(/*int cxema,*/ SKK& sk, QString& sError );
  bool CalculateSK(SKK &tg, SKDATA& data);
  bool CalculateOT1(OTOP& tg, OTDATA& data, double tn, double &tau01, double& tau02, double& tau03, double& tb, double &Q0, double &tau01v, bool is_srezki);
  double find_izlom(OTOP& tg, OTDATA& data, double t_izlom);
};

