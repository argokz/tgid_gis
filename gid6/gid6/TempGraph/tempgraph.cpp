// tempgraph.cpp : implementation file
//

#include "stdafx.h"
#include "tempgraph.h"
#include "../ado.h"


CString ftoa(double num)
{
    char buffer[100];
    sprintf(buffer, "%.15g", num);
    CString str = buffer;
    return str;
}

CString itoa(int num)
{
    CString str;
    str.Format("%d", num);
    return str;
}


/////////////////////////////////////////////////////////////////////////////
// CTempGraph dialog


void InitOtoplTempStructure(CAdoFile* ado, COMMON_TEMP_STRUCTURE& tempstruct, COMMON_TEMP_STRUCTURE& tempstruct_norm)
{
    tempstruct.feat = Otop;
    tempstruct.DeleteStruct(tempstruct.ot);
    tempstruct.ot = new OTOP();

    tempstruct.ot->THOR = ado->read_double("tn_5"); // Расчетная температура наружного воздуха
    tempstruct.ot->THK = ado->read_double("tn_1"); // Температура наружного воздуха конца отопительного сезона
    tempstruct.ot->TVR = ado->read_double("tvn_r"); // Расчетная температура воздуха внутри помещения

    tempstruct.ot->TAURP = ado->read_double("t1_r");  // Расчетная темп-ра сетевой воды в подающем трубопроводе
    tempstruct.ot->TAURO = ado->read_double("t2_r");  //      -"-                       в обратном
    tempstruct.ot->TAURS = ado->read_double("t3_r");  //      -"-                       после узла смешения

    tempstruct.ot->QR = ado->read_double("q_r");  // Расчетная тепловая нагузка на отопление
    tempstruct.ot->TSMIN = ado->read_double("t1_2r");  // Температура нижней срезки  сетевой воды

    tempstruct.ot->TSMAX = ado->read_double("t1_4r");  // Температура верхней срезки  сетевой воды
    tempstruct.ot->T2MIN = ado->read_double("t2_2r");  // Температура нижней срезки обратной воды

    tempstruct.ot->TEC[0] = 0;   // Наименование ТЭЦ
    tempstruct.ot->MAG[0] = 0;   // Наименование магистали
    tempstruct.ot->GDN[0] = 0;   // Начало отопительного сезона
    tempstruct.ot->GDK[0] = 0;   // Конец отопительного сезона

    tempstruct.ot->KSR = ado->read_double("uf");  // Поправка коэффициента смешения
    tempstruct.ot->V = ado->read_double("v");  // Скорость ветра
    tempstruct.ot->TB = ado->read_double("tvb_tr");  // Требуемая температура воздуха внутри отап-х помещениях


/*
    tempstruct.ot->QMAX = ado->read_double("");  // Отношение максимального количества тепла отпускаемого ТЭЦ ????
                                                 // к расчетному теплу на отопление магистрали


    tempstruct.ot->QMAX = atof(Value);  // Отношение максимального количества тепла отпускаемого ТЭЦ ????
                                        // к расчетному теплу на отопление магистрали


    else
        tempstruct.ot->QMAX = 100;
*/

    tempstruct.ot->QMAX = ado->read_double("hSourcePower");

    if (tempstruct.ot->QMAX == 0) {
        tempstruct.ot->QMAX = 100;
    }



    tempstruct.ot->KOL = -tempstruct.ot->THOR + tempstruct.ot->THK + 1;
    tempstruct_norm.feat = Otop;
    tempstruct_norm.DeleteStruct(tempstruct_norm.ot);
    tempstruct_norm.ot = new OTOP();
    (*tempstruct_norm.ot) = (*tempstruct.ot);
    tempstruct_norm.ot->TSMIN = 0;
    tempstruct_norm.ot->TSMAX = 200;
    tempstruct_norm.ot->T2MIN = 0;
}



void InitPovTempStructure(CAdoFile* ado, COMMON_TEMP_STRUCTURE& tempstruct, COMMON_TEMP_STRUCTURE& tempstruct_norm)
{
    tempstruct.feat = Pov;
    tempstruct.DeleteStruct(tempstruct.pov);
    tempstruct.pov = new POV();

    tempstruct.pov->THOR = ado->read_double("tn_5"); // Расчетная температура наружного воздуха
    tempstruct.pov->THK = ado->read_double("tn_1"); // Температура наружного воздуха конца отопительного сезона
    tempstruct.pov->TVR = ado->read_double("tvn_r"); // Расчетная температура воздуха внутри помещения

    tempstruct.pov->TAURP = ado->read_double("t1_r");  // Расчетная темп-ра сетевой воды в подающем трубопроводе
    tempstruct.pov->TAURO = ado->read_double("t2_r");  //      -"-                       в обратном
    tempstruct.pov->TAURS = ado->read_double("t3_r");  //      -"-                       после узла смешения

    tempstruct.pov->QOR = ado->read_double("q_r");  // Расчетная тепловая нагузка на отопление
    tempstruct.pov->QGW = ado->read_double("q_gv");   // Среднечасовая нагрузка горячего водоснабжения, Гкал/ч
    tempstruct.pov->TSMIN = ado->read_double("t1_2r");  // Температура нижней срезки  сетевой воды

    tempstruct.pov->TSMIN = 0; // Для повышенного графика не бывает нижней срезки, сказала Надежда Ивановна (2022.01.19)
                              // Манюк стр 155,166
    
    
    tempstruct.pov->TSMAX = ado->read_double("t1_4r");  // Температура верхней срезки  сетевой воды

    tempstruct.pov->TVRO = ado->read_double("tg_r");  // Температура горячей воды в точке водоразбора
    tempstruct.pov->TV = ado->read_double("tx_r");  // Температура холодной воды
    tempstruct.pov->TB = ado->read_double("tvb_tr");  // Требуемая температура воздуха внутри отап-х помещениях

    tempstruct.pov->NEDOG = ado->read_double("t_gv1");  //  Величина недогрева водопроводной воды в первой ступени подогревателя горячего водоснабжения, Град. C
    tempstruct.pov->KSR = ado->read_double("uf");  // Поправка коэффициента смешения
    tempstruct.pov->V = ado->read_double("v");  // Скорость ветра


    tempstruct.pov->TEC[0] = 0;   // Наименование ТЭЦ
    tempstruct.pov->MAG[0] = 0;   // Наименование магистали
    tempstruct.pov->GDN[0] = 0;   // Начало отопительного сезона
    tempstruct.pov->GDK[0] = 0;   // Конец отопительного сезона




    if (1) {
        tempstruct.pov->QMAX = ado->read_double("hSourcePower");
        //    atof(Value);  // Отношение максимального количества тепла отпускаемого ТЭЦ ????
                                // к расчетному теплу на отопление магистрали
    }
    else
        tempstruct.pov->QMAX = 100;

    tempstruct.pov->KOL = -tempstruct.pov->THOR + tempstruct.pov->THK + 1;


    tempstruct_norm.feat = Pov;
    tempstruct_norm.DeleteStruct(tempstruct_norm.pov);
    tempstruct_norm.pov = new POV();
    (*tempstruct_norm.pov) = (*tempstruct.pov);
    tempstruct_norm.pov->TSMIN = 0;
    tempstruct_norm.pov->TSMAX = 200;
    //  tempstruct_norm.pov->T2MIN=0;
}




void InitSKTempStructure(CAdoFile* ado, COMMON_TEMP_STRUCTURE& tempstruct, COMMON_TEMP_STRUCTURE& tempstruct_norm, BOOL pov)
{
    if (pov) tempstruct.feat = SkkPov;
    else tempstruct.feat = SkkPon;

    tempstruct.DeleteStruct(tempstruct.skk);
    tempstruct.skk = new SKK();
    if (pov)tempstruct.skk->IsPov = 1;
    else tempstruct.skk->IsPov = 0;
    ///tempstruct.skk->iw;     // Точка излома /??????? 


    tempstruct.skk->THOR = ado->read_double("tn_5"); // Расчетная температура наружного воздуха
    tempstruct.skk->THK = ado->read_double("tn_1"); // Температура наружного воздуха конца отопительного сезона
    tempstruct.skk->TVR = ado->read_double("tvn_r"); // Расчетная температура воздуха внутри помещения


    tempstruct.skk->TAURP = ado->read_double("t1_r");  // Расчетная темп-ра сетевой воды в подающем трубопроводе
    tempstruct.skk->TAURO = ado->read_double("t2_r");  //      -"-                       в обратном
    tempstruct.skk->TAURS = ado->read_double("t3_r");  //      -"-                       после узла смешения

    tempstruct.skk->QOR = ado->read_double("q_r");  // Расчетная тепловая нагузка на отопление
    tempstruct.skk->QGW = ado->read_double("q_gv");   // Среднечасовая нагрузка горячего водоснабжения, Гкал/ч

    tempstruct.skk->TSMIN = ado->read_double("t1_2r");  // Температура нижней срезки  сетевой воды
    tempstruct.skk->TSMIN = 0; // Для скорректированного графика не бывает нижней срезки, сказала Надежда Ивановна (2022.01.19)
                              // Манюк стр 155,166



    tempstruct.skk->TSMAX = ado->read_double("t1_4r");  // Температура верхней срезки  сетевой воды
    tempstruct.skk->T2MIN = ado->read_double("t2_2r");  // Температура нижней срезки обратной воды

    tempstruct.skk->KGUP = ado->read_double("g1");  // Коэффициент гидравлической устойчивости подающего труб-да
    tempstruct.skk->KGUO = ado->read_double("g2");  // Коэффициент гидравлической устойчивости обратного труб-да

    long u = ado->read_long("pr");

    tempstruct.skk->PSN = 0;
    tempstruct.skk->PSY = 0;
    tempstruct.skk->PSP = 0;
    tempstruct.skk->PSO = 0;
    switch (u)
    {
    case 1: {tempstruct.skk->PSN = 1; break; }// Признак водоразбора воды на ГВ с переключением
    case 2: {tempstruct.skk->PSP = 1; break; }// Признак водоразбора воды на ГВ только из подающего
    case 3: {tempstruct.skk->PSO = 1; break; }// Признак водоразбора воды на ГВ только из обратного
    case 4: {tempstruct.skk->PSY = 1; break; }// Признак водоразбора воды на ГВ с узлом смешения
    }

    tempstruct.skk->T2GW = ado->read_double("t2_gv"); // Темпер-ра сетевой воды начала водоразбора из обрат-го тр-да
    tempstruct.skk->TV = ado->read_double("tx_r");  // Температура холодной воды
    tempstruct.skk->TB = ado->read_double("tvb_tr");  // Требуемая температура воздуха внутри отап-х помещениях
    tempstruct.skk->TVRO = ado->read_double("tg_r");  // Температура горячей воды в точке водоразбора


    tempstruct.skk->KSR = ado->read_double("uf");  // Поправка коэффициента смешения
    tempstruct.skk->V = ado->read_double("v");  // Скорость ветра

    tempstruct.skk->TEC[0] = 0;   // Наименование ТЭЦ
    tempstruct.skk->MAG[0] = 0;   // Наименование магистали
    tempstruct.skk->GDN[0] = 0;   // Начало отопительного сезона
    tempstruct.skk->GDK[0] = 0;   // Конец отопительного сезона

  /*
     CHeatSourceMain * Mainn=(CHeatSourceMain *)(GetParent()->GetParent());
    CString Name_str,Ed_str;
    if (Mainn!=NULL)
    {
      Mainn->GetPower(Value,Name_str,Ed_str);
      tempstruct.skk->QMAX = atof(Value);  // Отношение максимального количества тепла отпускаемого ТЭЦ ????
                          // к расчетному теплу на отопление магистрали
    }
    else
  */

    tempstruct.skk->QMAX = ado->read_double("hSourcePower");
    //    atof(Value);  // Отношение максимального количества тепла отпускаемого ТЭЦ ????
                            // к расчетному теплу на отопление магистрали
    if (tempstruct.skk->QMAX == 0) {
        tempstruct.skk->QMAX = 100;
    }

    tempstruct.skk->KOL = -tempstruct.skk->THOR + tempstruct.skk->THK + 1;

    if (pov)tempstruct_norm.feat = SkkPov;
    else tempstruct_norm.feat = SkkPon;
    tempstruct_norm.DeleteStruct(tempstruct_norm.skk);
    tempstruct_norm.skk = new SKK();
    (*tempstruct_norm.skk) = (*tempstruct.skk);
    tempstruct_norm.skk->TSMIN = 0;
    tempstruct_norm.skk->TSMAX = 200;
    tempstruct_norm.skk->T2MIN = 0;
}





CTempGraph::CTempGraph(CAdoFile* ado, int id, bool standart)
{
    if (standart) {
        defaultLoadTempGraph(ado, id);
    }
    else {
        ogLoadTempGraph(ado, id);
    }
    
}

void CTempGraph::defaultLoadTempGraph(CAdoFile* ado, int id) {
    COMMON_TEMP_STRUCTURE tempstruct;
    COMMON_TEMP_STRUCTURE tempstruct_norm;
    //  CString Value = "О";


    long typ = 0;
    CString q;
    q.Format("SELECT * FROM heatSources WHERE ID = %d", id);

    if (ado->openTable0(q)) {

        typ = ado->read_long("graphTypeID");


        if (typ == 1 || typ == 0)  // О
        {
            InitOtoplTempStructure(ado, tempstruct, tempstruct_norm);
        }
        if (typ == 3)  // П
        {
          
            InitPovTempStructure(ado, tempstruct, tempstruct_norm);
        }

        if (typ == 2)  // "СВ"
        {
           
            InitSKTempStructure(ado, tempstruct, tempstruct_norm, TRUE);
        }
        if (typ == 4) // "СН"
        {
            
            InitSKTempStructure(ado, tempstruct, tempstruct_norm, FALSE);
        }


        ado->closeTable();


        if (AfxMessageBox("Пересчитать температурный график?", MB_YESNO) == IDYES) {



            TempStruct_norm = &tempstruct_norm;
            TempStruct = TempStruct_norm;
            calculated = Calculate();
            if (!calculated) return;

            TempStruct = &tempstruct;
            calculated = Calculate();
            if (!calculated) return;
            CString q;

            //    }
            CString SQL_string_D;
            CString TableName1 = "deployedTempGraphs";
            long affected;

            q.Format("DELETE FROM %s WHERE hSourceID=%d", TableName1, id);
            ado->ExecuteInsert(q, &affected);

            SQL_string_D = "INSERT INTO " + TableName1 + " ( hSourceID,tn,t1,t2,t3,tv,t_bn ) VALUES ( ";
            if (tempstruct.feat == Otop) {
                SQL_string_D = "INSERT INTO " + TableName1 + " ( hSourceID,tn,Q_otn,t1,t2,t3,tv,t_bn ) VALUES ( ";
            }
            else if (tempstruct.feat == Pov) {
                SQL_string_D = "INSERT INTO " + TableName1 + " ( hSourceID,tn,t1,t2,tv,t_bn, tg ) VALUES ( ";
            }
            else {
                SQL_string_D = "INSERT INTO " + TableName1 + " ( hSourceID,tn,t1,t2,t3,tv ) VALUES ( ";
            }

            CString s;


            //  SQL_string_D += ltoa(koddd) + ", ";
            if (tempstruct.feat == Otop)
            {
                for (long i = 0; i <= tempstruct.ot_data->n; i++)
                {
                    CString SQL_string = SQL_string_D;
                    s.Format("%d, %.1f, %.3f, %.1f, %.1f, %.1f, %.1f, %.1f)", id, tempstruct.array_ptr[0][i], tempstruct.array_ptr[1][i], tempstruct.array_ptr[2][i], tempstruct.array_ptr[3][i], tempstruct.array_ptr[4][i], tempstruct.array_ptr[5][i], tempstruct.array_ptr[6][i]);
                    SQL_string += s;
                    ado->ExecuteInsert(SQL_string, &affected);
                }
            }
            if (tempstruct.feat == Pov)
            {
                for (long i = 0; i <= tempstruct.pov_data->n; i++)
                {
                    CString SQL_string = SQL_string_D;

                    s.Format("%d, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f)", id, tempstruct.array_ptr[0][i], tempstruct.array_ptr[1][i], tempstruct.array_ptr[2][i], tempstruct.array_ptr[3][i], tempstruct.array_ptr[4][i], tempstruct.array_ptr[5][i]);
                    SQL_string += s;
                    ado->ExecuteInsert(SQL_string, &affected);

                }
            }
            if ((tempstruct.feat == SkkPov) || (tempstruct.feat == SkkPon))
            {
                for (long i = 0; i <= tempstruct.skk_data->n; i++)
                {
                    CString SQL_string = SQL_string_D;
                    s.Format("%d, %.1f, %.1f, %.1f, %.1f, %.1f)", id, tempstruct.array_ptr[0][i], tempstruct.array_ptr[1][i], tempstruct.array_ptr[3][i], tempstruct.array_ptr[2][i], tempstruct.array_ptr[4][i]);
                    SQL_string += s;
                    ado->ExecuteInsert(SQL_string, &affected);
                }
            }
        }
    }
}

void CTempGraph::ogLoadTempGraph(CAdoFile* ado, int id) {
    COMMON_TEMP_STRUCTURE *tempstruct = new COMMON_TEMP_STRUCTURE;
    COMMON_TEMP_STRUCTURE *tempstruct_norm = new COMMON_TEMP_STRUCTURE;
    //  CString Value = "О";


    long typ = 0;
    CString q;
    q.Format("SELECT * FROM heatSources WHERE ID = %d", id);

    if (ado->openTable0(q)) {

        typ = ado->read_long("graphTypeID");

        InitOtoplTempStructure(ado, *tempstruct, *tempstruct_norm);

        ado->closeTable();

        TempStruct_norm = tempstruct_norm;
        TempStruct = TempStruct_norm;
        calculated = Calculate();
        if (!calculated) return;

        TempStruct = tempstruct;
        calculated = Calculate();
        if (!calculated) return;
    }
}
//----------------------------------------------------------------------------

CTempGraph::~CTempGraph(void)
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CTempGraph::RedrawChart()
{
    //AfxMessageBox("Срфк", MB_OK|MB_ICONINFORMATION);
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
BOOL CTempGraph::Calculate()
{
    TempStruct->Init();
    if (TempStruct->feat == Otop)
    {
        Tip_name = "Отопительный";
        OTOP* ot = (TempStruct->ot);
        OTDATA* data = (TempStruct->ot_data);
        data->Init((*ot).KOL);
        CString sError;
        if (!CheckInputOT((*ot), sError))
        {
            AfxMessageBox(sError, MB_OK|MB_ICONINFORMATION);
            return FALSE;
        }
        else
        {
            CalculateOT((*ot), (*data));
            TempStruct->DeleteStruct(TempStruct->array_ptr);
            TempStruct->InitArray(7);
            TempStruct->array_ptr[0] = (*data).tn;
            TempStruct->array_ptr[1] = (*data).QO;
            TempStruct->array_ptr[2] = (*data).tau01;
            TempStruct->array_ptr[3] = (*data).tau02;
            TempStruct->array_ptr[4] = (*data).tau03;
            TempStruct->array_ptr[5] = (*data).tau01v;
            TempStruct->array_ptr[6] = (*data).tb;

            return TRUE;
        }
    }
    if (TempStruct->feat == Pov)
    {
        Tip_name = "Повышенный";
        POV* ot = (TempStruct->pov);
        POVDATA* data = (TempStruct->pov_data);
        data->Init((*ot).KOL);
        CString sError;
        if (!CheckInputPOV((*ot), sError))
        {
            AfxMessageBox(sError, MB_OK|MB_ICONINFORMATION);
            return FALSE;
        }
        else
        {
            CalculatePOV((*ot), (*data));
            TempStruct->DeleteStruct(TempStruct->array_ptr);
            TempStruct->InitArray(6);
            TempStruct->array_ptr[0] = (*data).tn;
            TempStruct->array_ptr[1] = (*data).tau01;
            TempStruct->array_ptr[2] = (*data).tau02;
            TempStruct->array_ptr[3] = (*data).tau01v;
            TempStruct->array_ptr[4] = (*data).tb;
            TempStruct->array_ptr[5] = (*data).tg;

            return TRUE;
        }
    }
    if ((TempStruct->feat == SkkPov) || (TempStruct->feat == SkkPon))
    {
        if (TempStruct->feat == SkkPov) Tip_name = "Скорректированный повышенный";
        else Tip_name = "Скорректированный пониженный";
        SKK* ot = (TempStruct->skk);
        SKDATA* data = (TempStruct->skk_data);
        data->Init((*ot).KOL);
        CString sError;
        if (!CheckInputSK((*ot), sError))
        {
            AfxMessageBox(sError, MB_OK|MB_ICONINFORMATION);
            return FALSE;
        }
        else
        {
            CalculateSK((*ot), (*data));
            TempStruct->DeleteStruct(TempStruct->array_ptr);
            TempStruct->InitArray(5);
            TempStruct->array_ptr[0] = (*data).tn;
            TempStruct->array_ptr[1] = (*data).tau01;
            TempStruct->array_ptr[2] = (*data).tau03;
            TempStruct->array_ptr[3] = (*data).tau02;
            TempStruct->array_ptr[4] = (*data).tau01v;

            return TRUE;
        }
    }
    return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CTempGraph::AddError(UINT uID, CString& sError)
{
    char s[256];
    HINSTANCE hInstance = 0;

    sprintf(s, "Код ошибки %d ", uID);
    sError += s;

    CString s1;
    s1.LoadString(uID);
    sError += s1;
    sError += "\n";
}
//----------------------------------------------------------------------------
#define ERR_OT IDS_ERROR+14
//----------------------------------------------------------------------------
bool CTempGraph::CheckInputOT(/*int cxema,*/ OTOP& ot, CString& sError)
{
    OTOP& tg = ot;
    bool Ok = true;
    //CString nl="\r\n\r\n";

    double a1, a6;
    //  double DTAU,TETA,DTR;

    if (tg.TVR == 0) a6 = tg.TB;
    else            a6 = tg.TB / tg.TVR;

    //  if(cxema == 0)
    a1 = (tg.QR - tg.QMAX) / tg.QR;
    /*
      if(cxema == 2)
      {
        a1 = (tg.QOR - tg.QMAX) / tg.QOR;
        a4 = 1.95 * tg.KGUO + 1;
        a5 = tg.QGW / tg.QOR;

        if(tg.PSN == 1)
        {
          ur = (tg.TAURP - tg.TAURS) / (tg.TAURS - tg.TAURO);
          DTAU = tg.TAURP-tg.TAURO;
          TETA = tg.TAURS+tg.TAURO;
          DTR  = TETA/2.0 - tg.TVR;
          TETA = tg.TAURS-tg.TAURO;
          t    = tg.THK;
          qopc = (tg.TB-t) / (tg.TVR+tg.THOR);
          t01  = t + qopc*( tg.TVR - tg.THOR+(0.5+ur)*DTAU /
                 (1+ur) + DTR/pow( qopc, 0.2) );
          t02  = t01 - qopc*DTAU + 10.;
          if(t01 < tg.TSMIN)
            { t01 = tg.TSMIN; t02 = t01 - qopc*DTAU - 5; }
        }
      }
    */
    if (tg.THOR == 0 && tg.THK == 0) { AddError(ERR_OT + 0, sError); Ok = false; }
    if (tg.TVR <= tg.THK) { AddError(ERR_OT + 1, sError); Ok = false; }
    if (tg.TB < tg.THK) { AddError(ERR_OT + 2, sError); Ok = false; }
    if (a6 > 2) { AddError(ERR_OT + 3, sError); Ok = false; }
    if (tg.TAURP < tg.TAURO ||
        tg.TAURP < tg.TAURS) {
        AddError(ERR_OT + 4, sError); Ok = false;
    }
    if (tg.TAURS <= tg.TAURO) { AddError(ERR_OT + 5, sError); Ok = false; }
    if (tg.TSMIN >= tg.TAURP) { AddError(ERR_OT + 6, sError); Ok = false; }
    if (tg.TSMIN >= tg.TSMAX) { AddError(ERR_OT + 7, sError); Ok = false; }
    if (tg.T2MIN >= tg.TAURO) { AddError(ERR_OT + 8, sError); Ok = false; }
    if (a1 > 0.5) { AddError(ERR_OT + 9, sError); Ok = false; }
    /*
      if(cxema==2)
      {
        if(tg.TVRO <= tg.TV)     { AddError(ERR_OT+10,sError); Ok = false; }
        if(tg.KGUP > a4)         { AddError(ERR_OT+11,sError); Ok = false; }
        if(a5 > 4)               { AddError(ERR_OT+12,sError); Ok = false; }
        if(tg.T2GW <= t02)       { AddError(ERR_OT+13,sError); Ok = false; }
      }
    */

    //  AddError(ERR_OT+9,sError);

    return Ok;
}
//----------------------------------------------------------------------------


int init_tn(double t1, double t2, double *tn)
{
    double tt1 = floor(t1);
    double tt2 = floor(t2);

    int n = 0;

    if (tt1 < t1) {
        tn[n++] = t1;
        tt1 += 1;
    }

    for (double t = tt1; t <= tt2; t += 1) {
        tn[n++] = t;
    }

    if (tt2 < t2) {
        tn[n++] = t2;
    }

    return n-1;
}


int init_tn_obr(double t1, double t2, double* tn)
{
    double tt1 = floor(t1);
    double tt2 = floor(t2);

    int n = 0;

    if (tt2 < t2) {
        tn[n++] = t2;
    }

    for (double t = tt2; t > tt1; t -= 1) {
        tn[n++] = t;
    }

//    if (tt1 < t1) {
        tn[n++] = t1;
//    }

    return n - 1;
}


bool CTempGraph::CalculateOT1(OTOP& tg, OTDATA& data, double tn, double &tau01, double& tau02, double& tau03, double& tb, double& QO, double& tau01v, bool is_srezki)
{
    int tm, n;
    double DTAU, TETA, DTR;
    double uf, tbn, eq, ex = 0.001, dq;
    double qopc, qq, qocn, qoc, qocnv, qocv, tbv;
    int pr = 0; // Спросить у Надежды Ивановны


    double TSMIN = tg.TSMIN;
    double TSMAX = tg.TSMAX;
    double QMAX = tg.QMAX;


    if (!is_srezki) {
        tg.TSMIN = 0;
        tg.TSMAX = 200;
        tg.QMAX = 100000;
    }

    //        t = data.tn[i];

        //  ur = (tg.TAURP - tg.TAURS) / (tg.TAURS - tg.TAURO);
    uf = tg.KSR;
    //  if(tg.TAURP == tg.TAURS)
    //    uf = 0; // добвалено 27.02.2002
    DTAU = tg.TAURP - tg.TAURO;
    TETA = tg.TAURS + tg.TAURO;
    DTR = TETA / 2.0 - tg.TVR;
    TETA = tg.TAURS - tg.TAURO;

    tm = 1;
    tbn = tg.TB;
    qq = qopc = (tg.TB - tn) / (tg.TVR - tg.THOR);
    n = 100;

    do {
        ex = 0.01;              // по дефициту мощности
        tau01 = tn + qopc * (tg.TVR - tg.THOR + (0.5 + uf) *
            DTAU / (1 + uf) + DTR / pow(qopc, 0.2));
        qocn = qopc;
        qoc = qocn;

        do {                    // по нижней верхней срезке
            tau02 = tau01 - DTAU * qopc;
            tau03 = tau02 + TETA * qopc;

            do {                  // Температура внутри помещения
                qoc = qocn;
                tb = tn + (tg.TB - tn) * qoc / qq;  // qopc; ????

                // соответствует Qo/QOR ( см. Зингер стр.160 )
                qocn = (tau01 - tb) / ((0.5 + uf) / (1 + uf) * DTAU + DTR / pow(qoc, 0.2));
                eq = (qoc - qocn) / qoc;  if (eq < 0) eq = -eq;

            } while (eq > ex);

            if (tau01 < tg.TSMIN || tau01 > tg.TSMAX || tau02 < tg.T2MIN)
            {
                // срезка
                if (tau01 < tg.TSMIN) { tm = 0; tau01 = tg.TSMIN; }
                if (tau01 > tg.TSMAX) { tm = 0; tau01 = tg.TSMAX; }
                if (tau02 < tg.T2MIN)
                {
                    tm = 0;
                    if (tau01 < tg.TSMAX) tau01 = tau01 + 0.05;
                    else tm = 1;
                }
                do {
                    qocn = (tau01 - tb) / ((0.5 + uf) / (1 + uf) * DTAU + DTR / pow(qoc, 0.2));
                    eq = (qoc - qocn) / qoc; if (eq < 0) eq = -eq;
                    qoc = qocn;
                    tb = tn + (tg.TB - tn) * qoc / qq;
                } while (eq > ex);
                qopc = qocn;
            }
            else tm = 1;

        } while (tm == 0); // по нижней верхней срезке

        if (qoc > tg.QMAX && n >= 1)
        {
            qopc = tg.QMAX;
            tbn = tn + qopc * (tg.TB - tn) / qq;
            data.defm = (tg.QMAX - qq) * 100 / tg.QMAX;
            if (data.defm < 0) data.defm = -data.defm;
            qopc = (tbn - tn) / (tg.TVR - tg.THOR);
            dq = ex + 1;
            n--;
        }
        else dq = ex - 1;

    } while (dq > ex);

    QO = qocn;
    if (tg.V > 3)      // Поправка на ветер
    {
        tau01v = tau01 + (tau01 - tg.TB) * (tg.V / 100);
        qocnv = qocn;
        do {
            qocv = qocnv;
            if (pr == 1) tbv = tg.TB;
            else        tbv = tn + (tg.TB - tn) * qocv / qq;
            qocnv = (tau01v - tbv) / ((0.5 + uf) / (1 + uf) * DTAU + DTR / pow(qocv, 0.2));
            eq = (qocv - qocnv) / qocv; if (eq < 0) eq = -eq;
        } while (eq > ex);

        if (qocnv > tg.QMAX)
        {
            qocnv = tg.QMAX;
            tbn = tn + qocnv * (tg.TB - tn) / qq;
            qocnv = (tbn - tn) / (tg.TVR - tg.THOR);
            tau01v = tn + qocnv * (tg.TVR - tg.THOR + (0.5 + uf) * DTAU / (1 + uf) +
                DTR / pow(qocnv, 0.2));
        }
        if (tau01 <= tg.TSMIN) tau01v = tg.TSMIN; // срезка
        if (tau01v > tg.TSMAX) tau01v = tg.TSMAX;
    }
    else tau01v = tau01;

    tg.TSMIN = TSMIN;
    tg.TSMAX = TSMAX;
    tg.QMAX = QMAX;

    return true;
}

double CTempGraph::find_izlom(OTOP& tg, OTDATA& data, double t_izlom)
{
    double tn1 = tg.THOR;
    double tn2 = tg.THK;

    if (t_izlom == 0) return tn2;

    while (1) {
        double tn = (tn1+tn2)/2;

        double tau01, tau02, tau03, tb, QO, tau01v;

        CalculateOT1(tg, data, tn, tau01, tau02, tau03, tb, QO, tau01v, false);

        if (tau01 < t_izlom) {
            tn2 = tn;
        }
        else {
            tn1 = tn;
        }

        if (fabs(tau01-t_izlom) < 0.001 || fabs(tn1 - tn2) < 0.001) return tn;
    }
    return 0;
}





bool CTempGraph::CalculateOT(OTOP& tg, OTDATA& data)
{
    /////////////////////////////////////////////////////////////////////////////
    bool Ok = true;

    int i, kl;

    data.n = tg.KOL = kl = init_tn_obr(tg.THOR, tg.THK, data.tn);
    
//    +tg.TSMIN != 0 ? 1 : 0;

    tg.QMAX = tg.QMAX / tg.QR;

    double t_izl = find_izlom(tg, data, tg.TSMIN);

    for (i = 0; i <= kl; i++)
    {
        CalculateOT1(tg, data, data.tn[i], data.tau01[i], data.tau02[i], data.tau03[i], data.tb[i], data.QO[i], data.tau01v[i], true);
    }
    tg.QMAX = tg.QMAX * tg.QR;


    return Ok;
}







//----------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#define ERR_POW IDS_ERROR+0
//----------------------------------------------------------------------------
bool CTempGraph::CheckInputPOV(/*int cxema,*/ POV& ot, CString& sError)
{

    POV& tg = ot;
    bool Ok = true;
    //CString nl("\r\n\r\n");

    double a3;
    if (tg.TVR == 0) a3 = tg.TB;
    else              a3 = tg.TB / tg.TVR;

    if (tg.THOR == 0 && tg.THK == 0) { AddError(ERR_POW + 1, sError); Ok = false; }
    if (tg.TVR <= tg.THK) { AddError(ERR_POW + 2, sError); Ok = false; }
    if (tg.TB < tg.THK) { AddError(ERR_POW + 3, sError); Ok = false; }
    if (a3 > 2) { AddError(ERR_POW + 4, sError); Ok = false; }
    if (tg.TAURP <= tg.TAURO ||
        tg.TAURP <= tg.TAURS) {
        AddError(ERR_POW + 5, sError); Ok = false;
    }
    if (tg.TAURS <= tg.TAURO) { AddError(ERR_POW + 6, sError); Ok = false; }
    if (tg.TSMIN >= tg.TAURP) { AddError(ERR_POW + 7, sError); Ok = false; }
    if (tg.TSMIN >= tg.TSMAX) { AddError(ERR_POW + 8, sError); Ok = false; }
    if ((tg.QOR - tg.QMAX) / tg.QOR > 0.5) { AddError(ERR_POW + 9, sError); Ok = false; }  // a1
    if (tg.TVRO <= tg.TV) { AddError(ERR_POW + 10, sError); Ok = false; }
    if (tg.QGW / tg.QOR > 0.7) { AddError(ERR_POW + 11, sError); Ok = false; } // a5

    if (tg.QGW <= 0) { AddError(ERR_POW + 12, sError); Ok = false; } // a5
    if (tg.QOR <= 0) { AddError(ERR_POW + 13, sError); Ok = false; } // a5

    return Ok;
}
/////////////////////////////////////////////////////////////////////////////
bool CTempGraph::CalculatePOV(POV& tg, POVDATA& data)
{
    bool Ok = true;

    // --------------------------------------------------------------------------
    int i, nn, psr;

    double DTAU, TETA, DTR, NC;
    double t, u;
    double qopc, qoc, defn;
    double qocv, qocnv, tbv;
    double tbn, eq, dq, ex = 0.001;
    double t02is, tis, isr = 0;
    double twis, dlt, dlt2is, dlt1, dlt2, ts;
    int kol;
    double tmp;
    //  double *qq;
    double gg, go;
    //  double qopc1;

    double qq[100];
    //  qq = (double *)malloc((kol+1)*sizeof(double));
    //  qq = new double[ kol+1 ];
//    kol = tg.THK - tg.THOR;
    tg.QMAX = tg.QMAX / tg.QOR;

//    for (i = kol; i >= 0; i--) data.tn[i] = tg.THOR + (kol - i);

    data.n = tg.KOL = kol = init_tn_obr(tg.THOR, tg.THK, data.tn);


    u = (tg.TAURP - tg.TAURS) / (tg.TAURS - tg.TAURO);
    if (tg.KSR != 0.0) u = tg.KSR;

    DTAU = tg.TAURP - tg.TAURO;
    t = DTAU;
    TETA = tg.TAURS + tg.TAURO;
    DTR = TETA / 2.0 - tg.TVR;
    TETA = tg.TAURS - tg.TAURO;
    NC = tg.QGW / tg.QOR;
    dlt = 1.2 * NC * DTAU;
    tis = tg.THK;

    for (i = 0; i <= kol; i++) // отопительный температурный график
    {
        t = data.tn[i];
        qq[i] = qopc = (tg.TB - t) / (tg.TVR - tg.THOR);
        data.t01[i] = t + qopc * (tg.TVR - tg.THOR +
            (0.5 + u) * DTAU / (1 + u) + DTR / pow(qopc, 0.2));
        data.t02[i] = data.t01[i] - qopc * DTAU;
        if (data.t01[i] >= tg.TVRO && isr == 0)
        {
            isr = 1; tis = t; t02is = data.t02[i];
        }
    }

    if (tis == tg.THK)  t02is = data.t02[0];
    twis = t02is - tg.NEDOG;
    dlt2is = dlt * (twis - tg.TV) / (tg.TVRO - tg.TV);
    data.defm = 0;
    defn = 0;

    for (i = 0; i <= kol; i++) // повышенный температурный график
    {
        t = data.tn[i];
        nn = 1000;
        tbn = tg.TB;
        qopc = qq[i];
        psr = 0;

        do {                // по дефициту мощности

            if (nn < 1000)
            {
                data.t01[i] = t + qopc * (tg.TVR - tg.THOR + (0.5 + u) * DTAU / (1 + u) + DTR / pow(qopc, 0.2));
                data.t02[i] = data.t01[i] - qopc * DTAU;
            }

            dlt2 = dlt2is * (data.t02[i] - tg.TV) / (t02is - tg.TV);
            dlt1 = (dlt - dlt2);
            if (dlt2 > dlt) dlt1 = 0;
            data.tau01[i] = data.t01[i] + dlt1;
            data.tau02[i] = data.t02[i] - dlt2;

            if (data.tau01[i]<tg.TSMIN || data.tau01[i]>tg.TSMAX)
            {
                if (data.tau01[i] < tg.TSMIN)  ts = tg.TSMIN;
                if (data.tau01[i] > tg.TSMAX)  ts = tg.TSMAX;

                data.tau02[i] = ts - (ts - t) * (data.tau01[i] - data.tau02[i]) / (data.tau01[i] - t);

                if (data.tau01[i] < tg.TSMIN)  data.tau01[i] = tg.TSMIN;
                if (data.tau01[i] > tg.TSMAX)  data.tau01[i] = tg.TSMAX;
            }

            // Срезка для отопительного графика
            if (data.t01[i]<tg.TSMIN || data.t01[i]>tg.TSMAX)
            {
                if (data.t01[i] < tg.TSMIN)  ts = tg.TSMIN;
                if (data.t01[i] > tg.TSMAX)  ts = tg.TSMAX;

                data.t02[i] = ts - (ts - t) * (data.t01[i] - data.t02[i]) / (data.t01[i] - t);

                if (data.t01[i] < tg.TSMIN)  data.t01[i] = tg.TSMIN;
                if (data.t01[i] > tg.TSMAX)  data.t01[i] = tg.TSMAX;
            }

            tmp = tg.QOR;
            data.tg[i] = (tmp / DTAU * (dlt1 + dlt2)) / (1.2 * tg.QGW / (tg.TVRO - tg.TV)) + tg.TV;

            gg = dlt / DTAU;               // тепло на ГВ в II ступени
            go = (data.t01[i] - data.t02[i]) / DTAU;   // тепло на отопление
            qoc = go + gg;

            if (qoc > tg.QMAX && nn >= 1)
            {
                //  tbn=t+(tg.QMAX-gg)*(tbn-t)/qoc;
                tbn -= 0.05;
                qopc = (tbn - t) / (tg.TVR - tg.THOR);

                dq = (tg.QMAX - qoc) / tg.QMAX; if (dq < 0) dq = -dq;
                defn = (tg.QMAX - (qq[i] + gg)) * 100 / tg.QMAX;
                defn = (defn < 0) ? -defn : defn;   // if(defn<0) defn=-defn;
                data.defm = (data.defm < defn) ? defn : data.defm;  // if(defm<defn) defm=defn;
                psr = 1;
                nn--;
            }
            else dq = ex;

        } while (dq > ex);

        data.tb[i] = tbn;

        if (tg.V > 3)   // Учет ветра
        {
            data.tau01v[i] = data.tau01[i] + (data.tau01[i] - tbn) * (tg.V / 100.);
            if (psr == 1)
                data.tau01v[i] = data.tau01[i];
            else
            {
                qocnv = qoc;
                do {
                    qocv = qocnv;
                    tbv = t + (tbn - t) * qocv / qopc;
                    qocnv = (data.tau01v[i] - tbv) / ((0.5 + u) / (1 + u) * DTAU + DTR / pow(qocv, 0.2));
                    eq = (qocv - qocnv) / qocv; if (eq < 0) eq *= -1;
                } while (eq > ex);

                if (qocnv > tg.QMAX)
                {
                    qocnv = tg.QMAX;
                    tbn = t + qocnv * (tg.TB - t) / qq[i];
                    qocnv = (tbn - t) / (tg.TVR - tg.THOR);
                    data.tau01v[i] = t + qocnv * (tg.TVR - tg.THOR +
                        (0.5 + u) * DTAU / (1 + u) + DTR / pow(qocnv, 0.2));
                }
            }

            if (data.tau01[i] <= tg.TSMIN) data.tau01v[i] = tg.TSMIN;  //  срезка
            if (data.tau01v[i] > tg.TSMAX) data.tau01v[i] = tg.TSMAX;
        }
        else data.tau01v[i] = data.tau01[i];

        // Пересчет отопительного графика с учетом дефицита мощности
        qopc = (tg.TB - t) / (tg.TVR - tg.THOR);
        if (qopc > tg.QMAX)  qopc = tg.QMAX;

        data.t01[i] = t + qopc * (tg.TVR - tg.THOR + (0.5 + u) * DTAU / (1 + u) + DTR / pow(qopc, 0.2));
        data.t02[i] = data.t01[i] - qopc * DTAU;

        // Срезка для отопительного графика срезка
        if (data.t01[i]<tg.TSMIN || data.t01[i]>tg.TSMAX)
        {
            if (data.t01[i] < tg.TSMIN)  ts = tg.TSMIN;
            if (data.t01[i] > tg.TSMAX)  ts = tg.TSMAX;

            data.t02[i] = ts - (ts - t) * (data.t01[i] - data.t02[i]) / (data.t01[i] - t);

            if (data.t01[i] < tg.TSMIN)  data.t01[i] = tg.TSMIN;
            if (data.t01[i] > tg.TSMAX)  data.t01[i] = tg.TSMAX;
        }
    }
    //  free(qq);
    //  delete[] qq;

    tg.QMAX = tg.QMAX * tg.QOR;

    return Ok;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


#define ERR_SK IDS_ERROR+14
//----------------------------------------------------------------------------
bool CTempGraph::CheckInputSK(/*int cxema,*/ SKK& sk, CString& sError)
{
    SKK& tg = sk;
    bool Ok = true;

    double a1, a4, a5, a6;
    double qopc, t, t01, t02 = 0, ur;
    double DTAU, TETA, DTR;

    tg.THOR = -tg.THOR;

    if (tg.TVR == 0) a6 = tg.TB;
    else            a6 = tg.TB / tg.TVR;

    a1 = (tg.QOR - tg.QMAX) / tg.QOR;
    a4 = 1.95 * tg.KGUO + 1;
    a5 = tg.QGW / tg.QOR;

    if (tg.PSN       == 1)
    {
        ur = (tg.TAURP - tg.TAURS) / (tg.TAURS - tg.TAURO);
        DTAU = tg.TAURP - tg.TAURO;
        TETA = tg.TAURS + tg.TAURO;
        DTR = TETA / 2.0 - tg.TVR;
        TETA = tg.TAURS - tg.TAURO;
        t = tg.THK;
        qopc = (tg.TB - t) / (tg.TVR + tg.THOR);
        t01 = t + qopc * (tg.TVR - tg.THOR + (0.5 + ur) * DTAU /
            (1 + ur) + DTR / pow(qopc, 0.2));
        t02 = t01 - qopc * DTAU + 10.;
        if (t01 < tg.TSMIN)
        {
            t01 = tg.TSMIN; t02 = t01 - qopc * DTAU - 5;
        }
    }
    


    if (tg.THOR == 0 && tg.THK == 0) { AddError(ERR_SK + 0, sError); Ok = false; }
    if (tg.TVR <= tg.THK) { AddError(ERR_SK + 1, sError); Ok = false; }
    if (tg.TB < tg.THK) { AddError(ERR_SK + 2, sError); Ok = false; }
    if (a6 > 2) { AddError(ERR_SK + 3, sError); Ok = false; }
    if (tg.TAURP <= tg.TAURO ||
        tg.TAURP <= tg.TAURS) {
        AddError(ERR_SK + 4, sError); Ok = false;
    }
    if (tg.TAURS <= tg.TAURO) { AddError(ERR_SK + 5, sError); Ok = false; }
    if (tg.TSMIN >= tg.TAURP) { AddError(ERR_SK + 6, sError); Ok = false; }
    if (tg.TSMIN >= tg.TSMAX) { AddError(ERR_SK + 7, sError); Ok = false; }
    if (tg.T2MIN >= tg.TAURO) { AddError(ERR_SK + 8, sError); Ok = false; }
    if (a1 > 0.5) { AddError(ERR_SK + 9, sError); Ok = false; }

    if (tg.TVRO <= tg.TV) { AddError(ERR_SK + 10, sError); Ok = false; }
    if (tg.KGUP > a4) { AddError(ERR_SK + 11, sError); Ok = false; }
    if (a5 > 4) { AddError(ERR_SK + 12, sError); Ok = false; }
    if (tg.T2GW <= t02) { AddError(ERR_SK + 13, sError); Ok = false; }


    if (tg.QGW <= 0) { AddError(ERR_SK + 14, sError); Ok = false; } // a5
    if (tg.QOR <= 0) { AddError(ERR_SK + 15, sError); Ok = false; } // a5

    if (tg.PSN == 0 && tg.PSY == 0 && tg.PSP == 0 && tg.PSO == 0) {
        AddError(ERR_SK + 16, sError); Ok = false;
    }


    tg.THOR = -tg.THOR;

    return Ok;
}
/////////////////////////////////////////////////////////////////////////////
bool CTempGraph::CalculateSK(SKK& tg, SKDATA& data)
{
    bool Ok = true;

    int    i, nn = 6, iww;
    double t; // деффицит мощности
    double DTAU, TETA, DTR, NC;
    double aa, bb, a, b, ur, uf;
    double qopc, qoc, co, cn, dn, d0, m, n, fi;
    double qocn, tbn, qq, eq, dq, dt = 0, ex = 0.001;
    double qocv, qocnv, tbv;
    double tau02n, tgwn;
    double to1, to2;
    int    kl, prs = 0;
    int    tm, psr;
    int    ntm = 0;

    tg.QMAX = tg.QMAX / tg.QOR;
    //  kl = tg.THK - tg.THOR;
//    kl = tg.KOL;

//    for (i = 0; i <= kl; i++) data.tn[i] = tg.THK - i;

    data.n = tg.KOL = kl = init_tn_obr(tg.THOR, tg.THK, data.tn);
    kl += 1;


    ur = (tg.TAURP - tg.TAURS) / (tg.TAURS - tg.TAURO);
    if (tg.KSR != 0.0) uf = tg.KSR;
    else              uf = ur;

    DTAU = tg.TAURP - tg.TAURO;
    TETA = tg.TAURS + tg.TAURO;
    DTR = TETA / 2.0 - tg.TVR;
    TETA = tg.TAURS - tg.TAURO;
    NC = tg.QGW / tg.QOR;
    cn = NC * (1 - tg.KGUP) * (tg.TVRO - tg.TV);
    co = NC * tg.KGUO * (1 + 2 * uf) * (tg.TVRO - tg.TV);

    if (tg.IsPov) { m = 1 + NC * (1 - tg.KGUP);  n = 0; }  // Повышенный темпер-й график
    else { m = 1; n = NC * (1 - tg.KGUP); }  // Пониженный темпер-й график
    dn = tg.TV;
    iww = 0;
    tg.iw = kl; // iw- номер точки излома

    for (i = 0; i <= kl; i++)
    {
        nn = 100;
        if (i == kl) {
            data.tn[i] = data.tn[tg.iw];
        }
        t = data.tn[i];
        qopc = (tg.TB - t) / (tg.TVR - tg.THOR);
        tbn = tg.TB;
        qq = qopc;
        psr = 0;

        do {
            data.t01[i] = t + qopc * (tg.TVR - tg.THOR + (0.5 + ur) * DTAU / (1 + ur) +
                DTR / pow(qopc, 0.2));
            data.t02[i] = data.t01[i] - qopc * DTAU;
            to1 = data.t01[i];
            to2 = data.t02[i];
            qocn = qopc;

            do {  // нижняя верхняя срезка

                tm = 1;
                a = (data.t01[i] + (1 + 2 * ur) * data.t02[i]) / (2 * (1 + ur));
                b = (data.t01[i] - data.t02[i]) * (1 + 2 * uf) / (2 * (1 + uf));
                tau02n = data.t02[i];// n=tg.T2GW;

                do {  // температура обратной воды

                    data.tau02[i] = tau02n;

                    if ((tg.PSN == 1 && data.tau02[i] < tg.T2GW) || (tg.PSP == 1))
                        // PSN=1 Режим с переключением,водоразбор из подающего
                        // РSP=1 водоразбор только из подающего
                    {
                        aa = ((cn + b) / (2 * m) + (a - dn) / 2); aa *= aa;
                        aa = aa - cn * (a - dn) / m;
                        data.tau01[i] = dn + (cn + b) / (2 * m) + (a - dn) / 2 + pow(aa, 0.5);
                        fi = m - cn / (data.tau01[i] - dn);
                        tau02n = (2 * a * (1 + uf) - data.tau01[i]) / (1 + 2 * uf);
                        dt = (data.tau02[i] - tau02n) / data.tau02[i]; if (dt < 0) dt = -dt;
                        data.tgw[i] = data.tau01[i];
                    }

                    if ((tg.PSN == 1 && data.tau02[i] >= tg.T2GW) || (tg.PSN == 1 && i == kl))
                        // Режим с переключением, водоразбор из обратного
                    {
                        if (iww == 0) { tg.iw = i - 1; iww = 1; } // iw -номер точки излома
                        d0 = a + (1 + 2 * uf) * (a - tg.TV);
                        aa = ((co + b) / (2 * m) + (a - d0) / 2); bb = aa * aa;
                        bb = bb - co * (a - d0) / m;
                        bb = pow(bb, 0.5);
                        data.tau01[i] = d0 + aa - bb;
                        fi = m - co / (data.tau01[i] - d0);
                        tau02n = (2 * a * (1 + uf) - data.tau01[i]) / (1 + 2 * uf);
                        dt = (data.tau02[i] - tau02n) / data.tau02[i]; if (dt < 0) dt *= -1;
                        data.tgw[i] = tau02n;
                    }

                    if (tg.PSO == 1) // PSO=1 водоразбор только из обратного
                    {
                        d0 = a + (1 + 2 * uf) * (a - tg.TV);
                        aa = ((co + b) / (2 * m) + (a - d0) / 2); bb = aa * aa;
                        bb = bb - co * (a - d0) / m;
                        if (tg.PSO == 1 && bb < 0) bb = 0; else bb = pow(bb, 0.5);
                        data.tau01[i] = d0 + aa - bb;
                        fi = m - co / (data.tau01[i] - d0);
                        tau02n = (2 * a * (1 + uf) - data.tau01[i]) / (1 + 2 * uf);
                        dt = (data.tau02[i] - tau02n) / data.tau02[i]; if (dt < 0) dt = -dt;
                        data.tgw[i] = data.tau02[i];
                    }

                    if (tg.PSY == 1) // Водоразбор с узлом смешения
                    {
                        aa = 1 + NC * (1 - tg.KGUP + tg.KGUO) * ((1 + 2 * uf) / (2 * (1 + uf))) - n;
                        data.tau01[i] = tg.TVRO + (data.t01[i] - tg.TVRO - n * (a - tg.TVRO)) / aa;
                        aa = (tg.TVRO - a + (data.tau01[i] - tg.TVRO) / (2 * (1 + uf))) / (data.tau01[i] - a);

                        if (tg.IsPov) // повышенный - без учета ГВС
                            fi = 1 + NC * tg.KGUO - NC * (1 - tg.KGUP + tg.KGUO) * aa;
                        else           // пониженный - с учетом ГВС
                            fi = 1 + NC * (1 - tg.KGUP + tg.KGUO) - NC * (1 - tg.KGUP + tg.KGUO) * aa;

                        tau02n = (2 * a * (1 + uf) - data.tau01[i]) / (1 + 2 * uf);
                        dt = (data.tau02[i] - tau02n) / data.tau02[i]; if (dt < 0) dt *= -1;
                        if (data.tau01[i] <= tg.T2GW) tgwn = data.tau01[i];
                        if (data.tau02[i] >= tg.T2GW) tgwn = data.tau02[i];
                        data.tgw[i] = tgwn;
                    }

                    do { // Температура внутри помещения
                        qoc = qocn;
                        data.tb[i] = t + (tg.TB - t) * qoc / qq;
                        qocn = (data.tau01[i] - data.tb[i]) / ((0.5 + uf) / (1 + uf) *
                            DTAU / fi + DTR / pow(qoc, 0.2));
                        eq = (qoc - qocn) / qoc; if (eq < 0) eq = -eq;
                    } while (eq > ex);

                } while (dt > ex);

                if (data.tau01[i]<tg.TSMIN || data.tau01[i]>tg.TSMAX || data.tau02[i] < tg.T2MIN)
                {
                    if (data.tau01[i] < tg.TSMIN) { tm = 0; data.t01[i] += 0.2; }
                    if (data.tau01[i] > tg.TSMAX) { tm = 0; data.t01[i] -= 0.2; }
                    if (data.tau02[i] < tg.T2MIN)
                    {
                        ntm++;
                        if (ntm == 400) tm = 1;
                        else         tm = 0;
                        if (data.tau01[i] < tg.TSMAX) data.t01[i] = data.t01[i] + 0.05;
                        else tm = 1;
                    }
                    do {
                        qoc = qocn;
                        data.tb[i] = t + (tg.TB - t) * qoc / qq;
                        qocn = (data.t01[i] - data.tb[i]) / ((0.5 + uf) / (1 + uf) * DTAU + DTR / pow(qoc, 0.2));
                        eq = (qoc - qocn) / qoc; if (eq < 0) eq *= -1;
                    } while (eq > 0.0001);

                    data.t02[i] = data.t01[i] - qocn * DTAU;
                }

            } while (tm == 0);
            ntm = 0;

            if ((qocn + NC) > tg.QMAX && nn > 2)
            {
                nn--;
                qopc = qopc - qopc / 100;
                tbn = t + qopc * (data.tb[i] - t) / qocn;
                dq = (tg.QMAX - qocn - NC) / tg.QMAX; if (dq < 0) dq = -dq;
                data.defm = (tg.QMAX - qocn - NC) * 100 / tg.QMAX;
                if (data.defm < 0) data.defm = -data.defm;
                psr = 1;
            }
            else dq = ex;

        } while (dq > ex);

        data.t01[i] = to1;
        data.t02[i] = to2;

        if (tg.V > 3)  // Учет ветра
        {
            data.tau01v[i] = data.tau01[i] + (data.tau01[i] - data.tb[i]) * (tg.V / 100.);
            if (psr == 1) data.tau01v[i] = data.tau01[i];
            else
            {
                qocnv = qoc;
                do {
                    qocv = qocnv;
                    tbv = t + (tg.TB - t) * qocv / qq;
                    qocnv = (data.tau01v[i] - tbv) / ((0.5 + uf) / (1 + uf) * DTAU / fi +
                        DTR / pow(qocv, 0.2));
                    eq = (qocv - qocnv) / qocv; if (eq < 0) eq *= -1;
                } while (eq > ex);

                if (qocnv > tg.QMAX)
                {
                    qocnv = tg.QMAX;
                    tbn = t + qocnv * (tg.TB - t) / qq;
                    qocnv = (tbn - t) / (tg.TVR - tg.THOR);
                    data.tau01v[i] = t + qocnv * (tg.TVR - tg.THOR + (0.5 + uf) * DTAU / (1 + uf) / fi + DTR / pow(qocnv, 0.2));
                }
            }
            if ((data.tau01[i] - 1) <= tg.TSMIN) data.tau01v[i] = tg.TSMIN;      /*  срезка           */
            if (data.tau01v[i] > tg.TSMAX) data.tau01v[i] = tg.TSMAX;
        }
        else data.tau01v[i] = data.tau01[i];

        data.tau03[i] = (data.tau01[i] + uf * data.tau02[i]) / (1 + uf);
    }
    tg.QMAX = tg.QMAX * tg.QOR;

    i = tg.iw + 1;
    if (i < kl)
    {
        int num = tg.KOL - i+1;
        int size = num * sizeof(double);

        double t1, t2, t3, tv, tw, t11, t22, tbb;
        t = data.tn[kl];
        t1 = data.tau01[kl];
        t2 = data.tau02[kl];
        t3 = data.tau03[kl];
        tv = data.tau01v[kl];
        tw = data.tgw[kl];
        t11 = data.t01[kl];
        t22 = data.t02[kl];
        tbb = data.tb[kl];

        memmove(&data.tn[i + 1], &data.tn[i], num * sizeof(double));
        memmove(&data.tau01[i + 1], &data.tau01[i], size);
        memmove(&data.tau02[i + 1], &data.tau02[i], size);
        memmove(&data.tau03[i + 1], &data.tau03[i], size);
        memmove(&data.tau01v[i + 1], &data.tau01v[i], size);
        memmove(&data.tgw[i + 1], &data.tgw[i], size);
        memmove(&data.t01[i + 1], &data.t01[i], size);
        memmove(&data.t02[i + 1], &data.t02[i], size);
        memmove(&data.tb[i + 1], &data.tb[i], size);

        data.tn[i] = t;
        data.tau01[i] = t1;
        data.tau02[i] = t2;
        data.tau03[i] = t3;
        data.tau01v[i] = tv;
        data.tgw[i] = tw;
        data.t01[i] = t11;
        data.t02[i] = t22;
        data.tb[i] = tbb;
    }

    if (data.tn[tg.KOL] != 0)
    {
//        if ((data.tn[tg.KOL] >= tg.THOR) && (data.tn[tg.KOL] <= tg.THK)) data.n = tg.KOL + 1;
        if ((data.tn[tg.KOL] >= tg.THOR) && (data.tn[tg.KOL] <= tg.THK)) {
            data.n = tg.KOL+1;
        }
    }
    return Ok;
}
/////////////////////////////////////////////////////////////////////////////


