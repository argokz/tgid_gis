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

        " UPDATE net.v_nodes"
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

        " FROM net.v_linesobj l"
        " JOIN net.v_nodes n1 ON n1.id=l.nodeID1"
        " JOIN net.v_nodes n2 ON n2.id=l.nodeID2"

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

    // На PG geom узла/линии пересчитывает INSTEAD OF-триггер
    // (x/y → Point, coords+концы → LineString). Колонка shape в SET
    // игнорируется. Достаточно «тронуть» строку с корректным алиасом —
    // старый SQL ссылался на linesobj.id / nodes без алиаса и падал.

    QString q1 = QString(
        " UPDATE net.v_nodes AS n\n"
        " SET x = n.x\n"
        " WHERE n.internalNodeID IS NULL\n"
        "   AND (n.x <> 0 OR n.y <> 0)\n"
        "   AND n.removed = 0 AND n.id = %1").arg(id);

    bool ret1 = query_exec(db, query, q1);

    QString q2 = QString(
        " UPDATE net.v_linesobj AS l\n"
        " SET coords = l.coords\n"
        " FROM net.v_nodes n1, net.v_nodes n2\n"
        " WHERE n1.id = l.nodeID1 AND n2.id = l.nodeID2\n"
        "   AND n1.internalNodeID IS NULL\n"
        "   AND (n1.x <> n2.x OR n1.y <> n2.y)\n"
        "   AND (n1.x <> 0 OR n1.y <> 0)\n"
        "   AND (n2.x <> 0 OR n2.y <> 0)\n"
        "   AND l.removed = 0 AND (n1.id = %1 OR n2.id = %1)").arg(id);

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

        " FROM net.v_linesobj l"
        " JOIN net.v_nodes n1 ON n1.id=l.nodeID1"
        " JOIN net.v_nodes n2 ON n2.id=l.nodeID2"

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
    // qq обычно вида "l.id=…" / "l.id=… OR l.id=…" — алиас l обязателен.
    // Триггер v_linesobj_update пересоберёт geom из coords и координат концов.
    QString q = QString(
        " UPDATE net.v_linesobj AS l\n"
        " SET coords = l.coords\n"
        " FROM net.v_nodes n1, net.v_nodes n2\n"
        " WHERE n1.id = l.nodeID1 AND n2.id = l.nodeID2\n"
        "   AND n1.internalNodeID IS NULL\n"
        "   AND (n1.x <> n2.x OR n1.y <> n2.y)\n"
        "   AND (n1.x <> 0 OR n1.y <> 0)\n"
        "   AND (n2.x <> 0 OR n2.y <> 0)\n"
        "   AND l.removed = 0 AND (%1)").arg(qq);

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
