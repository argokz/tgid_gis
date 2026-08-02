#pragma once

#include <QCoreApplication>

#define MY_N3 5
#define MY_N10 255

#define D5 8.
#define N8 8

#define NO_ALL_FIND

#define N65 20
#define LEN100 30

#define LOC1 1
#define LOC2 2
#define LOC3 3
#define LOC6 6
// Точка с поворотом angle
#define LOC7 7
#define LOC11 11
#define LOC12 12
#define LOC13 13
#define LOC14 14

#include <QString>
#include <QFile>

void mkdir_for_file(const QString & fn);

QString argpath();
QString argpath_2();
QString argpath_local();
QString argpath_0();
void set_argpath(const QString & path);

QString GetTitle(const QString & path);

QString get_baza_gid9();


enum FilePlace {
    fp_root,
    fp_users,
    fp_absolute
};

bool is_gid9();

void set_gid9(bool yes);
void set_baza_gid9(const QString & gid9);

QString readFile(const QString & fn, FilePlace fp = fp_root);
QString readFileUTF8(const QString & fn, FilePlace fp = fp_root);

QString readQSS(const QString & fn, FilePlace fp = fp_root);

QString nameQ(const QString & fn);

QString readQ(const QString & fn);
QString readQUTF8(const QString & fn);
QString readLineCode(QFile &file, const QString & codepage);


QString readCP1251Line(QFile &file);

enum PrTypes
{
    pr_type_any,
    pr_type_node0,  // Узел
    pr_type_node,   // Все узлы кроме узла
    pr_type_line,
    pr_type_node_out,
    pr_type_line_out,
    pr_type_geo
};

#define BUILDTM_YEAR (\
    __DATE__[7] == '?' ? 1900 \
    : (((__DATE__[7] - '0') * 1000 ) \
    + (__DATE__[8] - '0') * 100 \
    + (__DATE__[9] - '0') * 10 \
    + __DATE__[10] - '0'))

#define BUILDTM_MONTH (\
    __DATE__ [2] == '?' ? 1 \
    : __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
    : __DATE__ [2] == 'b' ? 2 \
    : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
    : __DATE__ [2] == 'y' ? 5 \
    : __DATE__ [2] == 'l' ? 7 \
    : __DATE__ [2] == 'g' ? 8 \
    : __DATE__ [2] == 'p' ? 9 \
    : __DATE__ [2] == 't' ? 10 \
    : __DATE__ [2] == 'v' ? 11 \
    : 12)

#define BUILDTM_DAY (\
    __DATE__[4] == '?' ? 1 \
    : ((__DATE__[4] == ' ' ? 0 : \
    ((__DATE__[4] - '0') * 10)) + __DATE__[5] - '0'))


// Проверка списка идентификаторов фрагментов перед подстановкой в SQL.
//
// Фильтр дописывается к тексту запроса конкатенацией строк
// (cxema/read_nodes.cpp, cxema/read_lines.cpp), поэтому список обязан
// состоять только из целых чисел через запятую. Без проверки сюда
// проходит произвольный текст.
bool isFragmentList(const QString & par);



