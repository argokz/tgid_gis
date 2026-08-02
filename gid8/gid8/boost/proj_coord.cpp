#include <iostream>
#include <string>

#include <boost/variant/variant.hpp>

#include <boost/geometry/srs/projection.hpp>

#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/algorithms/make.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/strategies/transform/srs_transformer.hpp>
#include <boost/geometry/srs/transformation.hpp>
#include <boost/geometry/srs/epsg.hpp>

#include <proj.h>


namespace bg = boost::geometry;
namespace srs = bg::srs;
namespace trans = bg::strategy::transform;

const std::string pr_almaty = "+proj=tmerc +lat_0=0 +lon_0=76.9166666666667 +k=1 +x_0=-3021 +y_0=-4791536 +ellps=krass +towgs84=24,-121,-76,0,0,0,0 +units=m +no_defs +type=crs";


//template <typename Parameters1, typename Parameters2>
class GIS {
    srs::transformation<> tr1;
    srs::transformation<> tr2;
public:        
/*
    GIS(Parameters1 const& parameters1, Parameters2 const& parameters2) :
        tr1(parameters1, parameters2),
        tr2(parameters2, parameters1)
    {
    }
*/
    GIS(int epsg, const std::string & proj4) :
        tr1((srs::epsg(epsg)), (srs::proj4(proj4))),
        tr2((srs::proj4(proj4)), (srs::epsg(epsg)))
    {
    }


    bool convert_dm_1(double x, double y, double &xx, double &yy)
    {
        bg::model::point<double, 2, bg::cs::geographic<bg::degree>> point1(x, y);
        bg::model::point<double, 2, bg::cs::cartesian> point2;

        tr1.forward(point1, point2);

        xx = bg::get<0>(point2);
        yy = bg::get<1>(point2);

        return false;
    }

    bool convert_md_2(double x, double y, double &xx, double &yy)
    {
        bg::model::point<double, 2, bg::cs::cartesian> point1(x, y);
        bg::model::point<double, 2, bg::cs::geographic<bg::degree>> point2;

        tr2.forward(point1, point2);

        xx = bg::get<0>(point2);
        yy = bg::get<1>(point2);

        return false;
    }

};

static GIS *gis_convert = nullptr;
//static map<int, GIS *> map_convert;

bool init_gis_convert(int epsg, const std::string & proj4)
{
    gis_convert = new GIS(epsg, proj4);
    return gis_convert != nullptr;
}

bool gis_convert1(double x, double y, double &xx, double &yy) 
{
    if (!gis_convert) return false;

    return gis_convert->convert_dm_1(x, y, xx, yy);
}

bool gis_convert2(double x, double y, double &xx, double &yy) 
{
    if (!gis_convert) return false;

    return gis_convert->convert_md_2(x, y, xx, yy);
}

#if 0

int main()
{
    double x0 = 0, y0 = 0;
    x0 =  76.9532469884731;
    y0 =  43.2588344224203;
    double x2, y2;

    init_gis_convert(4326, pr_almaty);

//    GIS gis((srs::epsg(4326)), (srs::proj4(pr_almaty)));

//    gis.convert_dm_1(x0, y0, x2, y2);

//    std::cout << std::setprecision (15) << "x " << x2 << " y " << y2 << std::endl;


    gis_convert1(x0, y0, x2, y2);

    std::cout << std::setprecision (15) << "x " << x2 << " y " << y2 << std::endl;


    return 0;
}

#endif
