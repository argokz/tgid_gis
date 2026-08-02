#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include "GidWidget.h"

bool isPoint(int loc);


QPointF findPointOnCurve(const std::vector<QPointF>& points, double targetLength) {
    if (points.size() < 2) return QPointF();  // Проверка на наличие как минимум двух точек

    double currentLength = 0.0;

    // Идем по сегментам и находим нужный
    for (size_t i = 1; i < points.size(); ++i) {
        QLineF segment(points[i - 1], points[i]);
        double segmentLength = segment.length();

        if (currentLength + segmentLength >= targetLength) {
            // Целевая длина лежит на текущем сегменте
            double remainingLength = targetLength - currentLength;
            return segment.pointAt(remainingLength / segmentLength);
        }

        currentLength += segmentLength;
    }

    // Если targetLength больше длины кривой, продолжаем в направлении последнего сегмента
    QLineF lastSegment(points[points.size() - 2], points.back());
    double overshoot = targetLength - currentLength;
    return lastSegment.pointAt(1.0 + (overshoot / lastSegment.length()));
}

QPointF calculateNewStartPoint(const QPointF& startPoint, const QPointF& endPoint, double length) {
    QLineF originalLine(startPoint, endPoint);
    double originalLength = originalLine.length();

    // Вычисляем масштабный коэффициент для новой длины
    double scaleFactor = length / originalLength;

    // Находим центр исходной линии
    QPointF midPoint = originalLine.pointAt(0.5);

    // Вектор направления с нормализованной длиной
    QPointF direction = (endPoint - startPoint) * 0.5 * scaleFactor;

    // Вычисляем новые начальную и конечную точки
    QPointF newStartPoint = midPoint - direction;
    return newStartPoint;
}

bool isLineInRenderArea(const QWidget& widget, const QPointF& point1, const QPointF& point2) {
    // Получаем границы виджета
    QRectF renderArea(0, 0, widget.width(), widget.height());

    // Проверяем, входит ли хотя бы одна из точек в область виджета
    if (renderArea.contains(point1) || renderArea.contains(point2)) {
        return true;
    }

    // Проверяем, пересекает ли линия границы области рендеринга
    QLineF line(point1, point2);
    QVector<QLineF> edges = {
        QLineF(renderArea.topLeft(), renderArea.topRight()),    // Верхняя граница
        QLineF(renderArea.topRight(), renderArea.bottomRight()), // Правая граница
        QLineF(renderArea.bottomRight(), renderArea.bottomLeft()), // Нижняя граница
        QLineF(renderArea.bottomLeft(), renderArea.topLeft())   // Левая граница
    };

    for (const QLineF& edge : edges) {
        if (line.intersects(edge, nullptr) == QLineF::BoundedIntersection) {
            return true;
        }
    }

    // Линия не входит в область рендеринга
    return false;
}

QPointF getParallelLineStartPoint(const QPointF& startPoint, const QPointF& endPoint, double s) {
    // Вычисляем вектор направления линии
    QLineF line(startPoint, endPoint);

    // Вычисляем вектор, перпендикулярный исходной линии (на 90 градусов)
    QPointF direction = line.p2() - line.p1();

    // Перпендикулярный вектор (направление вверх или вниз)
    QPointF perpendicular(-direction.y(), direction.x());  // Поворот на 90 градусов

    // Нормализуем вектор и масштабируем его на расстояние s
    double length = std::sqrt(perpendicular.x() * perpendicular.x() + perpendicular.y() * perpendicular.y());
    perpendicular *= s / length;

    // Перемещаем точку startPoint на расстояние s в сторону параллельной линии
    QPointF parallelStart = startPoint + perpendicular;

    return parallelStart;
}



void GidWidget::DrawLabel2(QPainter* painter, const CCoordList& cl, const QString & text, double x, double y, double alpha, double coef, int loc, const Klassif *m_kls)
{
    // double x1, y1, x2, y2;
    // cl.napr(x1, y1, x2, y2);

    // double dx = (x2-x1)/hypot(x2-x1, y2-y1);
    // double dy = (y2-y1)/hypot(x2-x1, y2-y1);

    // QPointF pt = CoordToQPointF(CFPoint((x1+x2)/2, (y1+y2)/2));

    // int flags = Qt::AlignHCenter|Qt::AlignBottom|Qt::TextDontClip;

    // QRectF rect = QRectF(pt.x(), pt.y(), 0, 0);
    // double angle = atan2(dy, dx) * 180 / M_PI;

    // painter->save();
    // painter->translate(pt);
    // painter->rotate(angle);
    // painter->translate(-pt);

    // painter->boundingRect ( rect, flags, text);
    // painter->drawText(rect, flags, text, &rect);

    // painter->restore();
    //return;
    //newnewnenwenwenwnew

    // Устанавливаем шрифт для вычисления размеров текста

    QFontMetrics fm(painter->font());

    // Размер текста
    int textWidth = fm.horizontalAdvance(text);
    int textHeight = fm.height();
    int textSpacing = textWidth*4;  // Расстояние между дублирующимся текстом

    double offsetAboveLine = -10.0;

    // Преобразуем список точек в массив для доступа по индексу
    std::vector<QPointF> pathPoints;
    for (const auto& p : cl) {
        if (p.x != C_SPR) {
            pathPoints.push_back(QPointF(p.x/geom.masx-geom.bx, p.y/geom.masy-geom.by));
        }
    }


    double totalLength = 0.0;
    std::vector<double> segmentLengths;

    // Вычисляем длину каждого сегмента и общую длину пути
    for (size_t i = 1; i < pathPoints.size(); ++i) {
        double segmentLength = QLineF(pathPoints[i-1], pathPoints[i]).length();
        segmentLengths.push_back(segmentLength);
        totalLength += segmentLength;
    }



    // Определяем, сколько раз можно нарисовать название улицы

    int repeatCount = std::max(1, static_cast<int>(totalLength / (textWidth + textSpacing)));
    double startingOffset = 0;(totalLength - repeatCount * (textWidth + textSpacing)) / 2;

//    qDebug() << "repeatCount: " << repeatCount << " " << text;


    double myOffset = 0;
    double s1 = repeatCount * (textWidth + textSpacing) - textSpacing;
    if (s1 < totalLength)
        myOffset = (totalLength - s1)/2;
    double myLenght = textWidth;
    double myCurrentPosition = myOffset;

    //width()*m_scale, height()*m_scale


    for (int repeat = 0; repeat < repeatCount; ++repeat) {

        QPointF startPos, endPos;
        if (myCurrentPosition==0)
            startPos = pathPoints[0];
        else
            startPos = findPointOnCurve(pathPoints, myCurrentPosition);

        endPos = findPointOnCurve(pathPoints, myLenght);


        // painter->save();
        // QPen angleLinePen(Qt::DashLine);
        // angleLinePen.setColor(Qt::red);
        // painter->setPen(angleLinePen);
        // painter->drawLine(startPos, endPos);  // Рисуем линию угла
        // painter->restore();

        if(isLineInRenderArea(*parentWidget(),startPos, endPos))
        {
            double angle = QLineF(startPos, endPos).angle();
            //QString text2 = text + QString::number(angle);
            if ((angle >= 0 && angle <= 90) || (angle>=270 && angle<=360 ))
            {
                QPointF startTextPoint = calculateNewStartPoint(startPos, endPos, textWidth);
                QPointF spanPoint = getParallelLineStartPoint(startTextPoint, endPos, offsetAboveLine);
                painter->save();
                painter->translate(spanPoint);
                painter->rotate(-angle);
                painter->drawText(QPointF(0, 0), text);
                painter->restore();
            }
            else{
                double angle = QLineF(endPos, startPos).angle();
                QPointF startTextPoint = calculateNewStartPoint(endPos, startPos, textWidth);
                QPointF spanPoint = getParallelLineStartPoint(startTextPoint, startPos, offsetAboveLine);
                painter->save();
                painter->translate(spanPoint);
                painter->rotate(-angle);
                painter->drawText(QPointF(0, 0), text);
                painter->restore();
            }
        }


        myCurrentPosition += textWidth + textSpacing;
        myLenght = myCurrentPosition + textWidth;
    }
}



void GidWidget::DrawLabel3(QPainter* painter, const CCoordList& cl, const QString & text, double x, double y, double alpha, double coef, int loc, const Klassif *m_kls)
{
    if (text == "") return;

    MyLOGFONT lf = m_kls->lf;

    double mas_otn1 = geom.masx*geom.dmas/(500);
    double lh = fabs(lf.lfHeight*coef/mas_otn1);

    if (lh < 2.) return;

    painter->setPen(QPen(Qt::black));
    painter->setBrush(palette().window());

    QFont font(lf.lfFaceName, lh);
    painter->setFont(font);

    if (loc == 2) {
        //если масштаб не позволяет увидеть улицу то не рисовать ее название
        //qDebug() << "drawLabel" << text;
        if (geom.masx < 70)
            DrawLabel2(painter, cl, text, x, y, alpha, coef, loc, m_kls);
    }

    if (loc == 3) {
        CFPoint ptf = cl.centroid();
        QPointF pt = CoordToQPointF(ptf);

        int flags = Qt::AlignHCenter|Qt::AlignVCenter|Qt::TextDontClip;

        QRectF rect = QRectF(pt.x(), pt.y(), 0, 0);
        painter->boundingRect ( rect, flags, text);
        painter->drawText(rect, flags, text, &rect);
    }
    if (isPoint(loc)) {
        CFPoint ptf = cl.first();
        QPointF pt = CoordToQPointF(ptf);
        pt.setY(pt.y() + lh/2.);
        int flags = Qt::AlignHCenter| Qt::AlignTop|Qt::TextDontClip;
        QRectF rect = QRectF(pt.x(), pt.y(), 0, 0);
        painter->boundingRect ( rect, flags, text);
        painter->drawText(rect, flags, text, &rect);
    }
}
