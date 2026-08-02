#ifndef PCH_H
#define PCH_H

// Стандартные библиотеки
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <algorithm>
#include <string>

// Qt (все, что часто используется)


#include <QtGlobal>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QBuffer>
#include <QDir>
#include <QtDebug>

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtNetwork>
//#include <QtConcurrent>

// Boost (если нужно)
//#include <boost/asio.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/variant/variant.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/format.hpp>


#include <boost/geometry/srs/projection.hpp>

#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/algorithms/make.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/strategies/transform/srs_transformer.hpp>
#include <boost/geometry/srs/transformation.hpp>
#include <boost/geometry/srs/epsg.hpp>


//#include <proj.h>

#endif // PCH_H
