import pymssql
import re

def parse_sql(filename):
    data = open(filename, 'r', encoding="utf8").readlines()
    stmts = []
    DELIMITER = ';'
    stmt = ''

    for lineno, line in enumerate(data):
        if not line.strip():
            continue

        if line.startswith('--'):
            continue

        if "--" in line:
            line = line.split("--")[0]+"\n"


        if 'DELIMITER' in line:
            DELIMITER = line.split()[1]
            continue


#            line = line.replace(DELIMITER, ';')

#        if DELIMITER in line :

        linestr = line.strip() 
 
        if linestr.endswith(DELIMITER):
            line = linestr[: -len(DELIMITER)]
            line += ";\n"
        else :
            stmt += line
            continue
 
        if stmt:
            stmt += line
            stmts.append(stmt.strip())
            stmt = ''
        else:
            stmts.append(line.strip())
    return stmts


def sql_exec(cursor, sql) :
    is_IDENTITY_INSERT = False

    sql = sql.replace("tinyint(1)", "bit")
    sql = sql.replace("tinyint", "bit")

    m = re.match(r"\s*INSERT INTO ([a-z0-9_]+)\s*\(id", sql, flags=re.IGNORECASE)
    if m :
      is_IDENTITY_INSERT = True
      tab = m.group(1)

    m = re.match(r"\s*CREATE TABLE ([a-z0-9_]+)\s*\(", sql, flags=re.IGNORECASE)
    if m :
      tab2 = m.group(1)
#      print(tab2)
#      if tab2 == "fragments" :
#        print(sql)


    m = re.search(r"CREATE TRIGGER", sql, flags=re.IGNORECASE)
    if m : return;


#    m = re.match(r"\s*INSERT INTO ([a-z0-9_]+)\s+\(id", sql, flags=re.IGNORECASE)

#    print("<<<<<<<<<<<<<<<<")
#    print(sql)
#    print(">>>>>>>>>>>>>>>>")

    try :
       if is_IDENTITY_INSERT : cursor.execute("SET IDENTITY_INSERT {} ON".format(tab))
       cursor.execute(sql)
       if is_IDENTITY_INSERT : cursor.execute("SET IDENTITY_INSERT {} OFF".format(tab))
    except pymssql.Error as e:
       print("-----------------")
       print(sql)
       print("-----------------")
       print(str(e.args[1], "utf8"))
       print("-----------------")
       exit(0)


import sqlparse
from sqlparse.tokens import Keyword, DML, Name



def parse_sql_AS(sl, level, dict_select) :
    is_as = False
    
    for m in sl :
        if m.ttype is None :
            isa = parse_sql_AS(m, level+1, dict_select)
            if (isa) :
               m2 = re.match(r"(.+)\s+as\s+(.+)", m.value, flags=re.IGNORECASE)
               if m2 :  
                  dict_select[m2.group(2)] = m2.group(1)
#                  print("'{}'->'{}'".format( m2.group(2), m2.group(1)))

        else :
            if m.ttype is Keyword and m.value.upper() == "AS" :  
                 is_as = True;

    return is_as



def ispr_sql(sql) :
#    m = re.match(r"(.*as)", sql, flags=re.IGNORECASE | re.MULTILINE)
    sql = sql.replace("\n", " ")
    sl = sqlparse.parse(sql)

    dict_select = dict()

    parse_sql_AS(sl, 1, dict_select)

#    for i in dict_select :
#        print(i, dict_select.get(i, 0))
  

    list_select = re.findall(r"\(SELECT [a-zA-Z0-9_]+?\)", sql, flags=re.IGNORECASE)

    for m1 in list_select :
       m = re.match(r"\(SELECT (.+?)\)", m1, flags=re.IGNORECASE)
       if m :  
          n_alias = m.group(1)
          k = m1
          v = dict_select.get(n_alias, "!!Error!!")
          print("<<", n_alias)
          print(k)
          print("({})".format(v))

          sql = sql.replace(k, "({})".format(v))

    return sql;



def sqlfile_show(sql_file) :
    print(sql_file)

    stmts = parse_sql(sql_file)

    for stmt in stmts:
        stmt = ispr_sql(stmt)
        stmt = ispr_sql(stmt)
        print(stmt)

