#pragma once

#include "coordlis.h"
#include "crack.h"


#pragma pack (push, 4)

struct SHP_HEAD 
{
  long File_Code;   //      9994                   Integer       Big
  long unused1[5];
  long File_Length; //      File Length            Integer       Big
  long Version; //          1000                   Integer       Little
  long Shape_Type;  //      Shape Type             Integer       Little
  double Xmin; //                                  Double        Little
  double Ymin; //                                  Double        Little
  double Xmax; //                                  Double        Little
  double Ymax; //                                  Double        Little
  long unused2[8]; 
};

struct DBASE_HEAD {
  unsigned char   version;     /*03 for dbIII and 83 for dbIII w/memo file*/
  unsigned char   l_update[3];                    /*yymmdd for last update*/
  unsigned long   count;                       /*number of records in file*/
  unsigned short  header;                           /*length of the header
                                                     *includes the \r at end
                                                     */
  unsigned short  lrecl;                            /*length of a record
                                                     *includes the delete
                                                     *byte
                                                     */
  unsigned char   reserv[20];
};

struct DBASE_FIELD {
    char    name[11];                                           /*field name*/
    char    type;                                               /*field type*/
#define DB_FLD_CHAR  'C'
#define DB_FLD_NUM   'N'
#define DB_FLD_FLOAT 'F'
#define DB_FLD_LOGIC 'L'
#define DB_FLD_MEMO  'M'
#define DB_FLD_DATE  'D'
    /* A-T uses large data model but drop it for now */
    char            *data_ptr;                         /*pointer into buffer*/
    unsigned char   length;                                   /*field length*/
    unsigned char   dec_point;                         /*field decimal point*/
    unsigned char   fill[14];
    };


#pragma pack (pop)


class Cshp_dbf
{
public:
  Cshp_dbf(const char *fn);
  ~Cshp_dbf();
  int first();
  int next();
  int next1();
  int go_to(long n);
  int go_to_shp(long n);
  int NFlds() { return nf;};
  long NRecs() { return nrecs;}
  int field_number(const char *fn);
  int read_coor();
  int read_coor1(double &x, double &y);
  int read_coor2(double &x1, double &y1, double &x2, double &y2, CCoordList &cl);
  
	int read_coor_xy(double &x1, double &y1, double &x2, double &y2);

  BOOL FieldName(int fH, char *s);

	int LenFld(int i) {return lenf[i-1];	}
	int TypFld(int i) {return typf[i-1];	}

  COleVariant read(int i) { return lvar[i];};
  COleVariant read(const char *fn);

  CString readStr(int i) { return CCrack::strVARIANT(read(i));}
  CString readStr(const char *fn) { return CCrack::strVARIANT(read(fn));}
  
  COleVariant lvar[256];
  char *pb;
  long loc;
  long nrecs;
private:
  long lenf[256], typf[256], nf, lrecl;
  DBASE_FIELD m_field[256];
  FILE *fdbf, *fshp;
  DBASE_HEAD head;
  SHP_HEAD hd;
  long *off_shx;
  CString m_fn;
};
