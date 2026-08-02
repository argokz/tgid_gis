#include <iostream>
#include <list>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>

#include "cxema/coordlis.h"


namespace bg = boost::geometry;

// Предполагаемая структура CFPoint
/*

struct CFPoint {
    double x, y;
    CFPoint(double x_, double y_) : x(x_), y(y_) {}
};
*/

// Тип точки: 2D декартова координата
using Point = bg::model::d2::point_xy<double>;
using Linestring = bg::model::linestring<Point>;

// Конвертация std::list<CFPoint> в Linestring
void convert_to_linestring(const std::list<CFPoint>& cf_points, Linestring& linestring) ;
#if 0
{
    linestring.clear();
    for (const auto& p : cf_points) {
        bg::append(linestring, Point(p.x, p.y));
    }
}
#endif

// Вычисление направленного расстояния Хаусдорфа от A к B
double directed_hausdorff(const Linestring& A, const Linestring& B) {
    double max_min_distance = 0.0;
    
    // Для каждой точки в A
    for (const auto& a : A) {
        double min_distance = std::numeric_limits<double>::infinity();
        
        // Находим минимальное расстояние до любой точки в B
        for (const auto& b : B) {
            double dist = bg::distance(a, b);
            min_distance = std::min(min_distance, dist);
        }
        
        // Обновляем максимальное из минимальных расстояний
        max_min_distance = std::max(max_min_distance, min_distance);
    }
    
    return max_min_distance;
}

// Вычисление расстояния Хаусдорфа
double hausdorff_distance(const Linestring& A, const Linestring& B) {
    if (A.empty() || B.empty()) {
        return std::numeric_limits<double>::infinity();
    }
    
    // Вычисляем направленные расстояния и берем максимум
    return std::max(directed_hausdorff(A, B), directed_hausdorff(B, A));
}


double hausdorff_distance(CCoordList &cl1, CCoordList &cl2) 
{

 // Конвертируем в linestring
    Linestring A, B;
    convert_to_linestring(cl1, A);
    convert_to_linestring(cl2, B);
    
    // Вычисляем расстояние Хаусдорфа
    double distance = hausdorff_distance(A, B);

    return distance;
}



#if 0
int main() {
    // Пример: две полилинии как std::list<CFPoint>
    std::list<CFPoint> cf_points_A = {{1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}};
    std::list<CFPoint> cf_points_B = {{1.0, 1.1}, {2.0, 2.1}, {3.0, 3.1}};
    
    // Конвертируем в linestring
    Linestring A, B;
    convert_to_linestring(cf_points_A, A);
    convert_to_linestring(cf_points_B, B);
    
    // Вычисляем расстояние Хаусдорфа
    double distance = hausdorff_distance(A, B);
    
    // Вывод результата
    std::cout << "Hausdorff distance: " << distance << std::endl;
    
    return 0;
}
#endif