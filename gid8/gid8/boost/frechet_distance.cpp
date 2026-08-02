#include <iostream>
#include <list>
#include <vector>
#include <algorithm> // для std::reverse
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>

#include "cxema/coordlis.h"


namespace bg = boost::geometry;


/*
// Предполагаемая структура CFPoint
struct CFPoint {
    double x, y;
    CFPoint(double x_, double y_) : x(x_), y(y_) {}
};

*/

// Тип точки: 2D декартова координата
using Point = bg::model::d2::point_xy<double>;
using Linestring = bg::model::linestring<Point>;

// Конвертация std::list<CFPoint> в Linestring
void convert_to_linestring(const std::list<CFPoint>& cf_points, Linestring& linestring) {
    linestring.clear();
    for (const auto& p : cf_points) {
        if (p.x != C_SPR) {
            bg::append(linestring, Point(p.x, p.y));
        }
    }
}

// Вычисление расстояния Фреше для двух linestring
double frechet_distance(const Linestring& A, const Linestring& B) {
    if (A.empty() || B.empty()) {
        return std::numeric_limits<double>::infinity();
    }

    size_t n = A.size();
    size_t m = B.size();

    // Матрица для динамического программирования
    std::vector<std::vector<double>> dp(n, std::vector<double>(m, -1.0));

    // Функция для вычисления евклидова расстояния
    auto dist = [&](size_t i, size_t j) {
        return bg::distance(A[i], B[j]);
    };

    // Рекурсивная функция с мемоизацией
    std::function<double(size_t, size_t)> compute_frechet = [&](size_t i, size_t j) -> double {
        if (dp[i][j] >= 0) {
            return dp[i][j];
        }

        double d = dist(i, j);

        if (i == 0 && j == 0) {
            dp[i][j] = d;
        } else if (i == 0) {
            dp[i][j] = std::max(d, compute_frechet(i, j - 1));
        } else if (j == 0) {
            dp[i][j] = std::max(d, compute_frechet(i - 1, j));
        } else {
            dp[i][j] = std::max(d, std::min({
                compute_frechet(i - 1, j),
                compute_frechet(i, j - 1),
                compute_frechet(i - 1, j - 1)
            }));
        }

        return dp[i][j];
    };

    return compute_frechet(n - 1, m - 1);
}

// Вычисление расстояния Фреше с учетом возможного разворота одной полилинии
double frechet_distance_with_reverse(const Linestring& A, Linestring B) {
    // Вычисляем расстояние для исходного направления
    double direct_distance = frechet_distance(A, B);

    // Разворачиваем полилинию B
    Linestring B_reversed;
    std::reverse_copy(B.begin(), B.end(), std::back_inserter(B_reversed));

    // Вычисляем расстояние для развернутой полилинии B
    double reverse_distance = frechet_distance(A, B_reversed);

    // Возвращаем минимальное расстояние
    return std::min(direct_distance, reverse_distance);
}

// Вычисление длины полилинии
    double calculate_linestring_length(const Linestring& linestring) {
    return bg::length(linestring);
}


double frechet_distance(CCoordList &cl1, CCoordList &cl2) 
{

 // Конвертируем в linestring
    Linestring A, B;
    convert_to_linestring(cl1, A);
    convert_to_linestring(cl2, B);
    
    // Вычисляем расстояние Хаусдорфа
    double distance = frechet_distance_with_reverse(A, B);
    double length = calculate_linestring_length(A);

//    if (length == 0) return 1e10;

    return distance;
}



#if 0

int main() {
    // Пример: две полилинии как std::list<CFPoint>
    std::list<CFPoint> cf_points_A = {{1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}};
    std::list<CFPoint> cf_points_B = {{3.0, 3.1}, {2.0, 2.1}, {1.0, 1.1}}; // B идет в обратном направлении

    // Конвертируем в linestring
    Linestring A, B;
    convert_to_linestring(cf_points_A, A);
    convert_to_linestring(cf_points_B, B);

    // Вычисляем расстояние Фреше с учетом возможного разворота
    double distance = frechet_distance_with_reverse(A, B);

    // Вывод результата
    std::cout << "Frechet distance (with reverse): " << distance << std::endl;

    return 0;
}

#endif
