#include <QSqlQuery>
#include <QSqlDatabase>
#include <QString>

#include <db/db.h>

bool reset_shape_node_ms(QSqlDatabase & db, int id)
{
//    if (!ado->is_shape_gid) return false;
    if (id <= 0) return false;

    QSqlQuery query(db);

    QString q1 = QString(

        " UPDATE nodes"
        " SET"
        " shape = geometry::STGeomFromText(CONCAT('POINT(', "
        " try_convert(decimal(20,2), x)/100 "
        ", ' ',  "
        " try_convert(decimal(20,2), -y)/100 "
        ", ')'), 0)"

        " WHERE internalNodeID IS NULL "
        "   AND (x <> 0 OR y <> 0) "
        "   AND removed=0 AND id=%1").arg(id);

    bool ret1 = query_exec(db, query, q1);

    QString q2 = QString(

        " UPDATE  l"
        " SET"
        " l.shape = geometry::STGeomFromText(CONCAT('LINESTRING(', "
//        " n1.x/100" 
        " try_convert(decimal(20,2), n1.x)/100 "
        ", ' ',  "
        " try_convert(decimal(20,2), -n1.y)/100 "
        ", ',',  dbo.str_coords(REPLACE(l.coords, ',', ' ')),"
        " try_convert(decimal(20,2), n2.x)/100 "
        ", ' ', "
        " try_convert(decimal(20,2), -n2.y)/100 "
        ", ')'), 0).MakeValid()"

        " FROM linesobj l"
        " JOIN nodes n1 ON n1.id=l.nodeID1"
        " JOIN nodes n2 ON n2.id=l.nodeID2"

        " WHERE n1.internalNodeID IS NULL "
        "   AND (n1.x <> n2.x OR n1.y <> n2.y) "
        "   AND (n1.x <> 0 OR n1.y <> 0) "
        "   AND (n2.x <> 0 OR n2.y <> 0) "
        "   AND l.removed=0 AND (n1.id=%1 OR n2.id=%1)").arg(id);

    return query_exec(db, query, q2);
}


bool reset_shape_node_pg(QSqlDatabase & db, int id)
{
//    if (!ado->is_shape_gid) return false;
    if (id <= 0) return false;

    QSqlQuery query(db);

    QString q1 = QString(

        " UPDATE nodes"
        " SET"
        " shape = ST_GeomFromText(CONCAT('POINT(', "
        " cast(x as decimal(20,2))/100 "
        ", ' ',  "
        " cast(-y as decimal(20,2))/100 "
        ", ')'), 0)"

        " WHERE internalNodeID IS NULL "
        "   AND (x <> 0 OR y <> 0) "
        "   AND removed=0 AND id=%1").arg(id);

    bool ret1 = query_exec(db, query, q1);

    QString q2 = QString(

        " UPDATE linesobj\n"
        " SET\n"
        " shape = ST_MakeValid(ST_GeomFromText(CONCAT('LINESTRING(',\n"
        " cast(n1.x as decimal(20,2))/100\n"
        ", ' ', \n"
        " cast(-n1.y as decimal(20,2))/100\n"
        ", ',',  str_coords(REPLACE(l.coords, ',', ' ')),\n"
        " cast(n2.x as decimal(20,2))/100 \n"
        ", ' ', \n"
        " cast(-n2.y as decimal(20,2))/100 \n"
        ", ')')))\n"

        " FROM linesobj l\n"
        " JOIN nodes n1 ON n1.id=l.nodeID1\n"
        " JOIN nodes n2 ON n2.id=l.nodeID2\n"

        " WHERE linesobj.id=l.id AND n1.internalNodeID IS NULL \n"
        "   AND (n1.x <> n2.x OR n1.y <> n2.y) \n"
        "   AND (n1.x <> 0 OR n1.y <> 0) \n"
        "   AND (n2.x <> 0 OR n2.y <> 0) \n"
        "   AND l.removed=0 AND (n1.id=%1 OR n2.id=%1)").arg(id);

    return query_exec(db, query, q2);
}

bool reset_shape_line_ms(QSqlDatabase & db, const QString & qq)
{
    QString q = QString(

        " UPDATE  l"
        " SET"
        " l.shape = geometry::STGeomFromText(CONCAT('LINESTRING(', "
        " try_convert(decimal(20,2), n1.x)/100 "
        " , ' ',  " 
        " try_convert(decimal(20,2), -n1.y)/100 "
        ", ',',  dbo.str_coords(REPLACE(l.coords, ',', ' ')),"
        " try_convert(decimal(20,2), n2.x)/100 "
        ", ' ', "
        " try_convert(decimal(20,2), -n2.y)/100 "
        ", ')'), 0).MakeValid()"

        " FROM linesobj l"
        " JOIN nodes n1 ON n1.id=l.nodeID1"
        " JOIN nodes n2 ON n2.id=l.nodeID2"

        " WHERE n1.internalNodeID IS NULL "
        "   AND (n1.x <> n2.x OR n1.y <> n2.y) "
        "   AND (n1.x <> 0 OR n1.y <> 0) "
        "   AND (n2.x <> 0 OR n2.y <> 0) "
        "   AND l.removed=0 AND (%1)").arg(qq);

    QSqlQuery query(db);
    return query_exec(db, query, q);
}


bool reset_shape_line_pg(QSqlDatabase & db, const QString & qq)
{
    QString q = QString(

        " UPDATE linesobj\n"
        " SET\n"
        " shape = ST_MakeValid(ST_GeomFromText(CONCAT('LINESTRING(', \n"
        " cast(n1.x as decimal(20,2))/100 \n"
        " , ' ', \n" 
        " cast(-n1.y as decimal(20,2))/100 \n"
        ", ',',  str_coords(REPLACE(l.coords, ',', ' ')),\n"
        " cast(n2.x as decimal(20,2))/100 \n"
        ", ' ', \n"
        " cast(-n2.y as decimal(20,2))/100 \n"
        ", ')'), 0))\n"

        " FROM linesobj l\n"
        " JOIN nodes n1 ON n1.id=l.nodeID1\n"
        " JOIN nodes n2 ON n2.id=l.nodeID2\n"

        " WHERE l.id=linesobj.id AND n1.internalNodeID IS NULL \n"
        "   AND (n1.x <> n2.x OR n1.y <> n2.y) \n"
        "   AND (n1.x <> 0 OR n1.y <> 0) \n"
        "   AND (n2.x <> 0 OR n2.y <> 0) \n"
        "   AND l.removed=0 AND (%1)").arg(qq);

    QSqlQuery query(db);
    return query_exec(db, query, q);
}


//------------------------------------------------------

bool reset_shape_line(QSqlDatabase & db, const QString & qq)
{
    if (is_POSTGRESQL()) {
        return reset_shape_line_pg(db, qq);
    }

    return reset_shape_line_ms(db, qq);
}



bool reset_shape_line(QSqlDatabase & db, int id)
{
//    if (!ado->is_shape_gid) return false;
    if (id <= 0) return false;

    QString qq = QString("l.id=%1").arg(id);

    return reset_shape_line(db, qq);
}


bool reset_shape_line(QSqlDatabase & db, int idP, int idO)
{
//    if (!ado->is_shape_gid) return false;
    if (idP <= 0 && idO <= 0) return false;

//    QString qq = QString("l.id=%1").arg(id);
    QString qq = QString("l.id=%1 OR l.id=%2").arg(idP).arg(idO);

    return reset_shape_line(db, qq);
}




bool reset_shape_node(QSqlDatabase & db, int id)
{
    if (is_POSTGRESQL()) {
        return reset_shape_node_pg(db, id);
    }

    return reset_shape_node_ms(db, id);
}
