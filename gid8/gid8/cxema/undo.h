#pragma once

//int addRemoved(QSqlDatabase & db, int typeID, int id, const QString & text);
#include "qsqldatabase.h"
int addRemoved(QSqlDatabase & db, int m_userRemoved, int typeID, int id, const QString & text);

QString change_group_start(QSqlDatabase &db, const QString &comment);
bool change_group_end(QSqlDatabase & db);


enum changeTypeID
{
  CH_T_DELETE_LINE = 1,  
  CH_T_DELETE_NODE = 2,
  CH_T_DELETE_CXEMA_BEGIN = 3,  
  CH_T_DELETE_CXEMA_END = 4,
  CH_T_MOVE_NODE = 5,
  CH_T_MOVE_LINE = 6,
  CH_T_EDIT_NODE = 7,
  CH_T_EDIT_LINE = 8,
//  CH_T_MOVE_LINE_GEO = 9,

  CH_T_EDIT = 9,

  CH_T_ADD_LINE = 10,
  CH_T_ADD_NODE = 11,

  CH_T_RENAME_LINE = 12,
  CH_T_RENAME_NODE = 13,

  CH_T_DELETE_GEO = 14,
  CH_T_ADD_GEO = 15,
  CH_T_MOVE_GEO = 16,


  CH_T_BEGIN = 23,  
  CH_T_END = 24,
};
