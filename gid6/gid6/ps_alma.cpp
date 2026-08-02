#include "stdafx.h"
#include "gid6.h"

#include "ado.h"


#include "ps_alma.h"

PS_ALMA ps_alma;


CString get_V(map<int, CString>& map_RE, int id)
{
  map<int, CString>::const_iterator it = map_RE.find(id);
  if (it != map_RE.end()) return it->second;
  return "";
}

CString get_V(map<CString, CString>& map_RE, CString id)
{
  map<CString, CString>::const_iterator it = map_RE.find(id);
  if (it != map_RE.end()) return it->second;
  return "";
}

CString _Q(const char *txt)
{
  CString q;
  q.Format("SELECT * FROM %s", txt);

  return q;
}

CString transl_l(const char *txt)
{
  CString s = transl(txt);
  s.MakeLower();
  return s;
}


CString transl(const char *txt)
{
  CString s = txt;

/*
  s.Replace("а", "a");
  s.Replace("б", "b");
  s.Replace("в", "v");
  s.Replace("г", "g");
  s.Replace("д", "d");
  s.Replace("е", "e");
  s.Replace("ё", "e");
  s.Replace("з", "z");
  s.Replace("и", "i");
  s.Replace("й", "j");
  s.Replace("к", "k");
  s.Replace("л", "l");
  s.Replace("м", "m");
  s.Replace("н", "n");
  s.Replace("о", "o");
  s.Replace("п", "p");
  s.Replace("р", "r");
  s.Replace("с", "s");
  s.Replace("т", "t");
  s.Replace("у", "u");
  s.Replace("ф", "f");
  s.Replace("х", "h");
  s.Replace("ъ", "'");
  s.Replace("ы", "y");
  s.Replace("ь", "'");
  s.Replace("э", "e");
  s.Replace("А", "A");
  s.Replace("Б", "B");
  s.Replace("В", "V");
  s.Replace("Г", "G");
  s.Replace("Д", "D");
  s.Replace("Е", "E");
  s.Replace("Ё", "E");
  s.Replace("З", "Z");
  s.Replace("И", "I");
  s.Replace("Й", "J");
  s.Replace("К", "K");
  s.Replace("Л", "L");
  s.Replace("М", "M");
  s.Replace("Н", "N");
  s.Replace("О", "O");
  s.Replace("П", "P");
  s.Replace("Р", "R");
  s.Replace("С", "S");
  s.Replace("Т", "T");
  s.Replace("У", "U");
  s.Replace("Ф", "F");
  s.Replace("Х", "H");
  s.Replace("Ъ", "'");
  s.Replace("Ы", "Y");
  s.Replace("Ь", "'");
  s.Replace("Э", "E");


  s.Replace("ж", "zh");
  s.Replace("ц", "ts");
  s.Replace("ч", "ch");
  s.Replace("ш", "sh");
  s.Replace("щ", "sch");
  s.Replace("ю", "ju");
  s.Replace("я", "ja");
  s.Replace("Ж", "Zh");
  s.Replace("Ц", "Ts");
  s.Replace("Ч", "Ch");
*/

        s.Replace("А", "A");
        s.Replace("Б", "B");
        s.Replace("В", "V");
        s.Replace("Г", "G");
        s.Replace("Д", "D");
        s.Replace("Е", "E");
        s.Replace("Ё", "E");
        s.Replace("Ж", "Zh");
        s.Replace("З", "Z");
        s.Replace("И", "I");
        s.Replace("Й", "Y");
        s.Replace("К", "K");
        s.Replace("Л", "L");
        s.Replace("М", "M");
        s.Replace("Н", "N");
        s.Replace("О", "O");
        s.Replace("П", "P");
        s.Replace("Р", "R");
        s.Replace("С", "S");
        s.Replace("Т", "T");
        s.Replace("У", "U");
        s.Replace("Ф", "F");
        s.Replace("Х", "H");
        s.Replace("Ц", "Ts");
        s.Replace("Ч", "Ch");
        s.Replace("Ш", "Sh");
        s.Replace("Щ", "Sch");
        s.Replace("Ъ", "");
        s.Replace("Ы", "Y");
        s.Replace("Ь", "");
        s.Replace("Э", "E");
        s.Replace("Ю", "Yu");
        s.Replace("Я", "Ya");
        s.Replace("а", "a");
        s.Replace("б", "b");
        s.Replace("в", "v");
        s.Replace("г", "g");
        s.Replace("д", "d");
        s.Replace("е", "e");
        s.Replace("ё", "e");
        s.Replace("ж", "zh");
        s.Replace("з", "z");
        s.Replace("и", "i");
        s.Replace("й", "y");
        s.Replace("к", "k");
        s.Replace("л", "l");
        s.Replace("м", "m");
        s.Replace("н", "n");
        s.Replace("о", "o");
        s.Replace("п", "p");
        s.Replace("р", "r");
        s.Replace("с", "s");
        s.Replace("т", "t");
        s.Replace("у", "u");
        s.Replace("ф", "f");
        s.Replace("х", "h");
        s.Replace("ц", "ts");
        s.Replace("ч", "ch");
        s.Replace("ш", "sh");
        s.Replace("щ", "sch");
        s.Replace("ъ", "");
        s.Replace("ы", "y");
        s.Replace("ь", "");
        s.Replace("э", "e");
        s.Replace("ю", "yu");
        s.Replace("я", "ya");



  s.Replace("/", "_");
  s.Replace(" ", "_");
  s.Replace("(", "_");
  s.Replace(")", "_");
  s.Replace(">", "gt");
  s.Replace("<", "lt");
  s.Replace(".", "_");
  s.Replace(",", "_");
  s.Replace(":", "_");
  s.Replace("\"", "_");
  
  if (s.GetLength() > 64) s = s.Left(64);


  return s;
}


void PS_ALMA::init()
{
  CString s;

  set_MAG.clear();
  map_RE.clear();
  map_UMS.clear();
  map_URS.clear();
  map_NACH.clear();
  map_UE.clear();
  map_Konstr.clear();
  mapMSRS.clear();
//  mapMSRS2.clear();
  mapMAG.clear();

  CAdoFile *ado = getAdo(getPsAdoName());

  if (ado) {
    if (ado->openTable0(_Q(transl("Район эксплуатации")))) {
      while ( !ado->isEOF() ) {
        int id = ado->read_long(transl("ID"));
        s = ado->readStr(transl("Наименование района эксплуатации источника тепла"));
        map_RE[id] = s;
        ado->MoveNext();
      }
      ado->closeTable();
    }  
    else {
//      AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
    }

    if (ado->openTable0(_Q(transl("Участки эксплуатации")))) {
      while ( !ado->isEOF() ) {
        UE ue;

        ue.id = ado->read_long(transl("ID"));
        ue.nach = ado->read_long(transl("Начальник участка"));
        ue.nomer = ado->readStr(transl("Номер участка"));
        ue.re = ado->read_long(transl("Район эксплуатации"));

//          s = ado->readStr(transl("Наименование района эксплуатации источника тепла"));
        map_UE[ue.id] = ue;
        ado->MoveNext();
      }
      ado->closeTable();
    }  

    if (ado->openTable0(_Q(transl("Магистрали")))) {
      while ( !ado->isEOF() ) {
        UMAG ms;
        ms.id = ado->read_long(transl("ID"));
        ms.name = ado->readStr(transl("Наименование магистрали"));
        ms.opis = ado->readStr(transl("Описание"));
//          ms.kod = ado->readStr(transl("Код ТС"));

/*
        ms.name = ado->readStr(transl("Магистраль"));
        ms.uch = ado->readStr(transl("Участок эксплуатации МС"));
        ms.nach = ado->read_long(transl("Начальник участка"));
        ms.re = ado->read_long(transl("Район эксплуатации"));
        ms.ue = ado->read_long(transl("Номер участка"));

        ms.kod1 = ado->readStr(transl("kod1"));
        ms.uzel1 = ado->readStr(transl("uzel1"));
        ms.kod2 = ado->readStr(transl("kod2"));
        ms.uzel2 = ado->readStr(transl("uzel2"));
*/

        if (ms.opis == "") ms.opis = ms.name;
//          if (ms.opis == "") ms.opis = ms.kod;

        ado->MoveNext();

        mapMAG[ms.id] = ms;
      }
      ado->closeTable();
    }  


    if (ado->openTable0(_Q(transl("Участок МС")))) {
      while ( !ado->isEOF() ) {
        UMS ms;
        ms.is_ms = 1;
        ms.id = ado->read_long(transl("ID"));
        ms.opis = ado->readStr(transl("Описание участка МС"));
//          ms.kod = ado->readStr(transl("Код ТС"));
        ms.mag = ado->read_long(transl("Магистраль"));

        ms.name = ado->readStr(transl("Магистраль"));   

/**
        ms.uch = ado->readStr(transl("Участок эксплуатации МС"));
        ms.nach = ado->read_long(transl("Начальник участка"));
*/

//          ms.re = ado->read_long(transl("Район эксплуатации"));
        ms.ue = ado->read_long(transl("Номер участка"));

        ms.kod1 = ado->readStr(transl("kod1"));
        ms.uzel1 = ado->readStr(transl("uzel1"));
        ms.kod2 = ado->readStr(transl("kod2"));
        ms.uzel2 = ado->readStr(transl("uzel2"));

//          if (ms.opis == "") ms.opis = ms.kod;

        ado->MoveNext();

        map_UMS[ms.id] = ms;
        set_MAG.insert(ms.name);
      }
      ado->closeTable();
    }  

    if (ado->openTable0(_Q(transl("Участок РС")))) {
      while ( !ado->isEOF() ) {
        URS rs;
        rs.is_ms = 0;
        rs.id = ado->read_long(transl("ID"));


/*
        rs.name = ado->readStr(transl("Участок РС"));
        rs.nach = ado->read_long(transl("Начальник участка"));
*/


//          rs.re = ado->read_long(transl("Район эксплуатации"));
        rs.ue = ado->read_long(transl("Номер участка"));
        rs.uzel = ado->readStr(transl("Узел подключения"));

        rs.opis = ado->readStr(transl("Наименование участка РС"));
//          rs.kod = ado->readStr(transl("Код ТС"));

        rs.kod1 = ado->readStr(transl("kod1"));
        rs.uzel1 = ado->readStr(transl("uzel1"));
        rs.kod2 = ado->readStr(transl("kod2"));
        rs.uzel2 = ado->readStr(transl("uzel2"));

//          if (rs.opis == "") rs.opis = rs.kod;
//        if (rs.opis == "") rs.opis = rs.name;
      
        ado->MoveNext();

        map_URS[rs.id] = rs;
      }
      ado->closeTable();
    }  

    if (ado->openTable0(_Q(transl("Начальники участков")))) {
      while ( !ado->isEOF() ) {
        NACH na;
        na.id = ado->read_long(transl("ID"));
        na.name = ado->readStr(transl("ФИО"));
        na.typ = ado->readStr(transl("Вид сети"));
        ado->MoveNext();

        map_NACH[na.id] = na;
      }
      ado->closeTable();
    }


    if (ado->openTable0(_Q(transl("Плиты перекрытия тепловых камер")))) {
      while ( !ado->isEOF() ) {
        int id = ado->read_long(transl("ID"));
        CString s = ado->readStr(transl("Конструкция перекрытия"));
        ado->MoveNext();

        map_Konstr[id] = s;
      }
      ado->closeTable();
    }  
//    ado->closeConnect();
  }


  UMSRS ms;
  mapMSRS.clear();

  int id = 0;

  {
    map<int, UMS>::iterator it = ps_alma.map_UMS.begin();

    for (; it !=  ps_alma.map_UMS.end(); it++) {
      UMS rs = it->second;

      it->second.id2 = id;

      ms.id = rs.id;
      ms.id2 = id;
      ms.name = rs.name;
      ms.opis = rs.opis;
//      ms.kod = rs.kod;
//      ms.re = rs.re;
      ms.ue = rs.ue;
      ms.kod1 = rs.kod1;      ms.uzel1 = rs.uzel1;
      ms.kod2 = rs.kod2;      ms.uzel2 = rs.uzel2;

//      ms.nach = rs.nach;
      ms.mag = 0;
   
      mapMSRS[id] = ms;
//      mapMSRS2[ms.kod] = ms;
      id++;
    }
  }

  {
    map<int, URS>::iterator it = ps_alma.map_URS.begin();

    for (; it !=  ps_alma.map_URS.end(); it++) {
      URS rs = it->second;

      it->second.id2 = id;

      ms.id = rs.id;
      ms.id2 = id;
//      ms.name = rs.name;
      ms.opis = rs.opis;
//      ms.kod = rs.kod;
//      ms.re = rs.re;
      ms.ue = rs.ue;

      ms.kod1 = rs.kod1;      ms.uzel1 = rs.uzel1;
      ms.kod2 = rs.kod2;      ms.uzel2 = rs.uzel2;
    
//      ms.nach = rs.nach;
      ms.mag = 1;
    
      mapMSRS[id] = ms;
//      mapMSRS2[ms.kod] = ms;
      id++;
    }
  }
}

UMSRS *PS_ALMA::getMsRs(int id)
{
  map<int, UMSRS>::iterator it = ps_alma.mapMSRS.find(id);
  if (it !=  ps_alma.mapMSRS.end()) {
    return &it->second;
  }
  return NULL;
}



/* Вернуть номер по порядку */


int PS_ALMA::getMsRs2(int id, int ms_rs)
{
    if (ms_rs == 0) {  // МС
        UMS *ums = getMs(id);
        if (ums) return ums->id2;
    }
    else {
        URS *urs = getRs(id);
        if (urs) return urs->id2;
    }

    return 0;
}


UMS *PS_ALMA::getMs(int ms)
{
  map<int, UMS>::iterator it = ps_alma.map_UMS.find(ms);
  if (it !=  ps_alma.map_UMS.end()) {
    return &it->second;
  }
  return NULL;
}


URS *PS_ALMA::getRs(int rs)
{
  map<int, URS>::iterator it = ps_alma.map_URS.find(rs);
  if (it !=  ps_alma.map_URS.end()) {
    return &it->second;
  }
  return NULL;
}

UMAG *PS_ALMA::getMag(int mag)
{
  map<int, UMAG>::iterator it = ps_alma.mapMAG.find(mag);
  if (it !=  ps_alma.mapMAG.end()) {
    return &it->second;
  }
  return NULL;
}


CString UMS::kod() 
{ 
  return opis;
}


CString URS::kod() 
{ 
  return opis;
}


CString UMAG::kod() 
{ 
  return opis;
}


const UE *PS_ALMA::getUE(int ue)
{
  map<int, UE>::const_iterator itUE = map_UE.find(ue);

  if (itUE != map_UE.end()) {
    return &itUE->second;
  }
  return NULL;
}
