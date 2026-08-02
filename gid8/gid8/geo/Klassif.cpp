#include <QtCore>
#include <QtSql>
#include <QMessageBox>

#include "Klassif.h"
#include <geo/geofile.h>

//#include "std.h"
#include <map>
#include <algorithm>


KlList m_kl_list;

unsigned long rgb_bgr(unsigned long col)
{
  return ((col&0xFF)<<16)|(col&0x00FF00)|((col&0xFF0000)>>16);
}

QString GetFName(const QString & path);

Klassif::Klassif()
{
//  CFont font;
//  font.CreatePointFont(100, "Arial");
//  font.GetLogFont(&lf);
    m_db = nullptr;
}

Klassif::~Klassif()
{
}

#if 0

void Klassif::Copy(const Klassif& mc)
{
//  memcpy(this, &mc, sizeof(this));
    
  IdxOriginX = mc.IdxOriginX;
  IdxOriginY = mc.IdxOriginY;

  IdxGridSize = mc.IdxGridSize;
  x1 = mc.x1;
  x2 = mc.x2;
  y1 = mc.y1;
  y2 = mc.y2;
  coli = mc.coli;
  colb = mc.colb;
  loc = mc.loc;
  prio = mc.prio;
  typ = mc.typ;
  col3 = mc.col3;
  punct = mc.punct;
  ins = mc.ins;
  ins2 = mc.ins2;
  nazv = mc.nazv;
  nazv_alias = mc.nazv_alias;
  xy_table = mc.xy_table;
  codlabel = mc.codlabel;
  coord_name = mc.coord_name;
  label = mc.label;
  lf = mc.lf;
  color = mc.color;
  is_edit = mc.is_edit;
  is_potr = mc.is_potr;

  baza = mc.baza;

  ramka = mc.ramka;
  no_mas = mc.no_mas;

  m_db = mc.m_db;
}


Klassif::Klassif(const Klassif& mc)
{
  Copy(mc);
}


Klassif& Klassif::operator=(const Klassif& mc)
{
  Copy(mc);
  return *this;
}

#endif

bool Klassif::operator==(const Klassif& mc) const
{
  return loc == mc.loc && nazv == mc.nazv;
}

bool Klassif::operator<(const Klassif& mc) const
{
  if (nazv < mc.nazv) return true;
  if (nazv == mc.nazv) return loc < mc.loc;
  return false;
}

Klassif * KlList::findKlN(const QString & name) 
{
  iterator p = find(name.toLower());
  if (p != end()) {
    return &p.value();
  }

  QString s1 = name.toLower();

  for (p = begin(); p != end(); p++) {
    QString s2 = p.key().toLower();
    if (s1 == s2) return &p.value();
  }

  return NULL;
}


Klassif * KlList::findKlN2(const QString & name) 
{
  QString s1 = name.toUpper();

  for (iterator p = begin(); p != end(); p++) {
    QString s2 = p.key().toUpper();

//    QMessageBox::information(0, "", QString("%1\n%2").arg(s1, s2));

    if (s1 == s2) {
//      QMessageBox::information(0, "", QString("Нашли!\n%1\n%2").arg(s1, s2));
      return &p.value();
    }
    if (s1.indexOf(s2+" ") == 0) {
//      QMessageBox::information(0, "", QString("Нашли!\n%1\n%2").arg(s1, s2));
      return &p.value();
    }
  }

  return NULL;
}

bool KlList::save(const QString & kln)
{
  return true;
}


bool KlList::save2(const QString & fn, const QString & baza)
{
//    char s[512], ss[512], mdbn[256];
//    QFile file(fn+"qqq");
    QFile file(fn);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    MyLOGFONT lf, lf1;

    QTextStream outputStream(&file);

    auto it = begin();

    for ( ; it != end(); ++it ) {
        if (it->baza != baza) continue;

//        Klassif *kl = &*it;
//        Klassif *kl = &*it; // findKlN(i);

        Klassif *kl = findKlN(it->nazv);


        lf1 = kl->lf;

        outputStream << QString("*,%1,%2,%3,%4,%5,%6,%7,\"%8\",\"\",\"%9\",%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34,%35\n")
            .arg(kl->loc)                //   1
            .arg(kl->prio)               //   2
            .arg(rgb_bgr(kl->coli), 8)   //   3
            .arg(rgb_bgr(kl->colb), 8)   //   4
            .arg(kl->typ, 3)             //   5
            .arg(rgb_bgr(kl->col3))      //   6
            .arg(kl->punct, 3)           //   7
            .arg(kl->nazv)               //   8
                                         //   -
            .arg(kl->codlabel)           //   9
            .arg(kl->label)              //   10
           
            .arg(lf1.lfFaceName)         //   11
            .arg(lf1.lfHeight)           //   12
            .arg(lf1.lfWidth)            //   13
            .arg(lf1.lfEscapement)       //   14
            .arg(lf1.lfOrientation)      //   15
            .arg(lf1.lfWeight)           //   16
            .arg(lf1.lfItalic)           //   17
            .arg(lf1.lfUnderline)        //   18
            .arg(lf1.lfStrikeOut)        //   19
            .arg(lf1.lfCharSet)          //   20
            .arg(lf1.lfOutPrecision)     //   21
            .arg(lf1.lfClipPrecision)    //   22
            .arg(lf1.lfQuality)          //   23
            .arg(lf1.lfPitchAndFamily)   //   24

            .arg(kl->color)              //   25
            .arg(kl->is_edit)            //   26
            .arg(kl->ins)                //   27
            .arg(kl->ins2)               //   28
            .arg(kl->is_potr)            //   29

            .arg(kl->ramka)              //   30
            .arg(kl->codlabel_color)     //   31
            .arg(kl->label_color)        //   32
            .arg(kl->n_label_color)      //   33
            .arg(kl->n_label)            //   34
            .arg(kl->no_mas)             //   35
            ;
    }

    file.close();

    return true;
}


bool KlList::save()
{
  return true;
}

bool KlList::open(const QString & kln)
{
  return true;
}

Prim *rs;

void OpenRas() {
  FILE *f;
  int nrs;

  rs = new Prim[256];
  nrs = 0;

//  f = fopenexe("flibrt.ras", "rb");
  f = fopen("flibrt.ras", "rb");
  if (f) {
    while (!feof(f) && nrs < 256) {
      rs[nrs].x = getc(f);
      rs[nrs].y = getc(f);
      fread(rs[nrs].s,32,1,f);
      nrs++;
    }
    fclose(f);
  }
}



bool KlList::open0(const QString & tn, const QString & db)
{
#if 0
  Klassif kl;

  m_nMaxPrio = 0;

  kl.loc = 3;
  kl.prio = 0;
  kl.typ = 0;

  m_nMaxPrio = std::max((int)m_nMaxPrio, (int)kl.prio);

//  kl.coli = 4671375;
//  kl.colb = 427007;
  kl.colb = 0xcFcFcF;
  kl.colb = 10669053;
  kl.coli = 0;
  kl.typ2 = 255;

//  kl.nazv = tn;
  kl.codlabel = "house_numer";
  kl.label = 1;

//  kl.lf.lfFaceName[0] = 0;

  kl.ins = 1;


//  CFont font;
//  font.CreatePointFont(100, _T("Arial"));
//  font.GetLogFont(&kl.lf);
//  font.DeleteObject();

//  LoadFont(_T("Font Gidr"), kl.lf, kl.color);

  (*this)[tn] = kl;
#endif
  return true;
}


QString Klassif::TableName() const
{
  if (nazv == nazv_alias) {
    return QString(nazv);
  }
  return QString(nazv)+" "+QString(nazv_alias);
}


QString Klassif::FileName() const
{
  return nazv;
}


CGeoObject *Klassif::getGeoObjectById(int id) 
{
  int nnn = geo4.size();
  for (int i = 0; i < nnn; i++) {
    int nn = geo4[i]->nom;

    if (nn == id) {
      return geo4[i];
    }
  }
  return NULL;
}


int Klassif::GetNom2(int n) 
{
  int nnn = geo4.size();
  for (int i = 0; i < nnn; i++) {
    int nn = geo4[i]->nom;

    if (nn == n) {
      return i+1;
    }
  }
  return 0;
}

QString findTableRusName(const QString & d, const QString & n);

QString Klassif::RusName() const
{
    return findTableRusName(baza, nazv);
}


