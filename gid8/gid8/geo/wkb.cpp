#include <cxema/coordlis.h>

#pragma pack(1)

// Basic Type definitions
// byte : 1 byte
// uint32 : 32 bit unsigned integer  (4 bytes)
// double : double precision number (8 bytes)

typedef unsigned char byte;

//typedef unsigned long uint32;

typedef uint32_t uint32;


// Building Blocks : Point_, LinearRing

struct Point_ {
    double x;
    double y;
};

struct LinearRing {
    uint32 numPoints;
    Point_  points[1];
};

enum wkbGeometryType {
    wkbPoint = 1,
    wkbLineString = 2,
    wkbPolygon = 3,
    wkbMultiPoint = 4,
    wkbMultiLineString = 5,
    wkbMultiPolygon = 6,
    wkbGeometryCollection = 7
};

enum wkbByteOrder {
    wkbXDR = 0,             // Big Endian
    wkbNDR = 1           // Little Endian
};



struct WKBPoint {
    unsigned char             byteOrder;
    uint32        wkbType;                // 1
    Point_            point;
};

struct WKBLineString {
    unsigned char             byteOrder;
    uint32        wkbType;                       // 2
    uint32        numPoints;
    Point_            points[1];
};

struct WKBPolygon {
    unsigned char             byteOrder;
    uint32        wkbType;                       // 3
    uint32        numRings;
    LinearRing    rings[1];
};

struct WKBMultiPoint {
    unsigned char             byteOrder;
    uint32        wkbType;                       // 4
    uint32        num_wkbPoints;
    WKBPoint         WKBPoints[1];
};

struct WKBMultiLineString {
    unsigned char             byteOrder;
    uint32        wkbType;                       // 5
    uint32        num_wkbLineStrings;
    WKBLineString WKBLineStrings[1];
};

struct WKBMultiPolygon {
    unsigned char             byteOrder;
    uint32        wkbType;                       // 6
    uint32        num_wkbPolygons;
    WKBPolygon    wkbPolygons[1];
};

//struct WKBGeometryCollection;


struct WKBGeometry {
    union {
        WKBPoint                   point;
        WKBLineString           linestring;
        WKBPolygon              polygon;
        //    WKBGeometryCollection   collection;
        WKBMultiPoint           mpoint;
        WKBMultiLineString      mlinestring;
        WKBMultiPolygon         mpolygon;
    };
};

/*

struct WKBGeometryCollection {
byte             byte_order;
uint32        wkbType;                       // 7
uint32        num_wkbGeometries;
WKBGeometry      wkbGeometries[1];
};
*/

#pragma pack() 

int readPoints(Point_ *points, int len, int numPoints, CCoordList &cl)
{
    for (int i = 0; i < numPoints; i++) {
        Point_ *pt = &points[i];

        cl.push_back(CFPoint(pt->x*100, -pt->y*100));
    }
    return numPoints * sizeof(Point_);
}

int readLinearRing(char *buf, int len, CCoordList &cl)
{
    LinearRing *ring = (LinearRing *) buf;
    int n = readPoints(ring->points, len, ring->numPoints, cl);
    return n + sizeof(LinearRing) - sizeof(Point_);
}


int readWKB(const char *buf, int len, CCoordList &cl)
{
    WKBPoint *point = (WKBPoint *)buf;

    cl.text = "";
    cl.is_color = false;
    cl.is_date = false;
    cl.color = 0;

    if (cl.size() > 0) {
      cl.push_back(CFPoint(C_SPR, C_SPR));
    }

    WKBGeometry *geometry = (WKBGeometry *)buf;

    switch (point->wkbType) {
    case wkbPoint:
    {
        WKBPoint *point = (WKBPoint *)buf;
        int n = readPoints(&point->point, len, 1, cl);
        cl.loc = 1;
        return n + sizeof(WKBPoint) - sizeof(Point_);
    }
    break;

    case wkbMultiPoint:
    {
        WKBMultiPoint *m_point = (WKBMultiPoint *)buf;

        if (m_point->num_wkbPoints > 0) {
            WKBPoint *point = &m_point->WKBPoints[0];
            int n = readPoints(&point->point, len, 1, cl);
            cl.loc = 1;
            return n + sizeof(WKBPoint) - sizeof(Point_);
        }
    }
    break;


    case wkbLineString:
    {
        WKBLineString *ls = (WKBLineString *)buf;
        char *buf1 = (char *)&ls->points[0];
        int n = readPoints(ls->points, len, ls->numPoints, cl);
        cl.loc = 2;
        return n + sizeof(WKBLineString) - sizeof(Point_);
    }
    break;




    case wkbPolygon:
    {
        WKBPolygon *ls = (WKBPolygon *)buf;
        char *buf1 = (char *)&ls->rings[0];

        for (int i = 0; i < ls->numRings; i++) {
            if (i > 0) cl.push_back(CFPoint(C_SPR, C_SPR));
            int n = readLinearRing(buf1, len, cl);
            buf1 += n;
        }
        cl.loc = 3;
        return buf1 - buf;
    }
    break;
    case wkbMultiLineString:
    {
        WKBMultiLineString *ml = (WKBMultiLineString *)buf;
        char *buf1 = (char*)&ml->WKBLineStrings[0];

        for (int i = 0; i < ml->num_wkbLineStrings; i++) {
            int n = readWKB(buf1, len, cl);
            buf1 += n;
        }
        cl.loc = 2;
        return buf1 - buf;
    }
    break;
    case wkbMultiPolygon:
    {
        WKBMultiPolygon *mp = (WKBMultiPolygon *)buf;
        char *buf1 = (char*)&mp->wkbPolygons[0];

        for (int i = 0; i < mp->num_wkbPolygons; i++) {
            int n = readWKB(buf1, len, cl);
            buf1 += n;
        }
        cl.loc = 3;
        return buf1 - buf;
    }
    break;
    case wkbGeometryCollection:
        break;
    }

    return 0;
}



