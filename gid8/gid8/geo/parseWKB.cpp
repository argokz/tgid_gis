#include <QByteArray>
#include <QDataStream>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <QVariant>
#include <QDebug>

#include <QByteArray>
#include <QDataStream>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <QVariant>
#include <QDebug>

static const quint32 EWKB_ZFLAG    = 0x80000000u;
static const quint32 EWKB_MFLAG    = 0x40000000u;
static const quint32 EWKB_SRIDFLAG = 0x20000000u;
static const quint32 WKB_TYPE_MASK = 0x000000FFu;

QVariant parseWKBStream(QDataStream &stream, int *sridOut);

// Внешняя функция: передаём hex-WKB и опционально получаем srid
QVariant parseWKB(const QString &hexWkb, int *sridOut = nullptr)
{
    QByteArray wkb = QByteArray::fromHex(hexWkb.toUtf8());
    if (wkb.isEmpty()) return QVariant();
    QDataStream stream(wkb);
    // Начальное byte order не важно — первый байт в WKB задаёт порядок
    return parseWKBStream(stream, sridOut);
}

QVariant parseWKBStream(QDataStream &stream, int *sridOut)
{
    // читаем порядок байтов (1 байт)
    quint8 byteOrder = 0;
    stream >> byteOrder;
    if (byteOrder == 0)
        stream.setByteOrder(QDataStream::BigEndian);
    else
        stream.setByteOrder(QDataStream::LittleEndian);

    // читаем тип (uint32)
    quint32 geomType = 0;
    stream >> geomType;

    bool hasZ = (geomType & EWKB_ZFLAG);
    bool hasM = (geomType & EWKB_MFLAG);
    bool hasSRID = (geomType & EWKB_SRIDFLAG);
    quint32 baseType = geomType & WKB_TYPE_MASK;

    // если есть SRID — прочитаем его
    if (hasSRID) {
        quint32 srid = 0;
        stream >> srid;
        if (sridOut) *sridOut = static_cast<int>(srid);
    } else {
        if (sridOut) *sridOut = 0;
    }

    // размер координат на вершину
    int coordDim = 2 + (hasZ ? 1 : 0) + (hasM ? 1 : 0);

    auto readCoordinate = [&](double &x, double &y){
        stream >> x; stream >> y;
        // пропускаем Z/M если есть
        for (int i = 2; i < coordDim; ++i) {
            double tmp;
            stream >> tmp;
        }
    };

    switch (baseType) {
    case 1: { // POINT
        double x = 0, y = 0;
        readCoordinate(x, y);
        return QPointF(x, y);
    }
    case 2: { // LINESTRING
        quint32 nPoints = 0;
        stream >> nPoints;
        QPolygonF poly;
        for (quint32 i = 0; i < nPoints; ++i) {
            double x=0,y=0;
            readCoordinate(x,y);
            poly.append(QPointF(x,y));
        }
        return QVariant::fromValue(poly);
    }
    case 3: { // POLYGON
        quint32 nRings = 0;
        stream >> nRings;
        QVector<QPolygonF> rings;
        for (quint32 r = 0; r < nRings; ++r) {
            quint32 nPoints = 0;
            stream >> nPoints;
            QPolygonF ring;
            for (quint32 i = 0; i < nPoints; ++i) {
                double x=0,y=0;
                readCoordinate(x,y);
                ring.append(QPointF(x,y));
            }
            rings.append(ring);
        }
        return QVariant::fromValue(rings);
    }
    case 4: // MULTIPOINT
    case 5: // MULTILINESTRING
    case 6: // MULTIPOLYGON
    {
        quint32 nGeoms = 0;
        stream >> nGeoms;
        QVector<QVariant> geoms;
        for (quint32 i = 0; i < nGeoms; ++i) {
            // каждый вложенный геометрический элемент имеет свою заголовочную часть (байт-орд., тип, возможно SRID)
            // при рекурсивном вызове мы не хотим перезаписывать внешний srid, поэтому передаём nullptr
            geoms.append(parseWKBStream(stream, nullptr));
        }
        return QVariant::fromValue(geoms);
    }
    default:
        qWarning() << "Unsupported geometry type:" << baseType;
        return QVariant();
    }
}


#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <QVariant>
#include <QDebug>

// ===== Для отдельных типов =====

QPointF centroidOfLine(const QPolygonF &line, bool *ok)
{
    if (line.isEmpty()) {
        if (ok) *ok = false;
        return QPointF();
    }
    double sx = 0, sy = 0;
    for (const QPointF &pt : line) {
        sx += pt.x();
        sy += pt.y();
    }
    if (ok) *ok = true;
    return QPointF(sx / line.size(), sy / line.size());
}

QPointF centroidOfPolygon(const QPolygonF &poly, bool *ok)
{
    int n = poly.size();
    if (n < 3) {
        if (ok) *ok = false;
        return QPointF();
    }

    double A = 0.0;
    double Cx = 0.0;
    double Cy = 0.0;

    for (int i = 0; i < n; ++i) {
        const QPointF &p1 = poly[i];
        const QPointF &p2 = poly[(i + 1) % n];
        double cross = p1.x() * p2.y() - p2.x() * p1.y();
        A  += cross;
        Cx += (p1.x() + p2.x()) * cross;
        Cy += (p1.y() + p2.y()) * cross;
    }

    A *= 0.5;
    if (A == 0.0) {
        if (ok) *ok = false;
        return QPointF();
    }

    Cx /= (6.0 * A);
    Cy /= (6.0 * A);
    if (ok) *ok = true;
    return QPointF(Cx, Cy);
}

// ===== Универсальная обёртка =====

QPointF centroid(const QVariant &geom, bool *ok = nullptr)
{
    if (geom.canConvert<QPointF>()) {
        if (ok) *ok = true;
        return geom.value<QPointF>(); // POINT
    }
    else if (geom.canConvert<QPolygonF>()) {
        return centroidOfLine(geom.value<QPolygonF>(), ok); // LINESTRING
    }
    else if (geom.canConvert<QVector<QPolygonF>>()) {
        QVector<QPolygonF> rings = geom.value<QVector<QPolygonF>>();
        if (!rings.isEmpty())
            return centroidOfPolygon(rings[0], ok); // POLYGON (берём внешний контур)
    }
    else if (geom.canConvert<QVector<QVariant>>()) {
        QVector<QVariant> geoms = geom.value<QVector<QVariant>>();
        if (!geoms.isEmpty())
            return centroid(geoms[0], ok); // MULTI-*, берём первый элемент
    }

    if (ok) *ok = false;
    return QPointF(); // неизвестный тип
}
