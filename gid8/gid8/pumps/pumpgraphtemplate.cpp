#include "pumpgraphtemplate.h"
#include <cmath>

void PumpGraphTemplate::setStandardPumps(QSqlQuery &query, GRAPHR_PARAM &grp, int id) {
    QString q = QString("SELECT * FROM standardPumps WHERE id = %1").arg(id);

    Show_H_chart = true;
    Show_Hr_chart = true;
    Show_N_chart = true;
    Show_Nr_chart = true;
    Show_KPD_chart = true;
    Show_KPDr_chart = true;

    if (query.exec(q)) {
        if (query.next()) { // Если найдена строка
            // Читаем основные параметры насоса
            grp.gr.Name = query.value("name").toString();
            grp.gr.Tip = query.value("tip_nas").toString();

            grp.gr.Q_min = query.value("q_min").toDouble();
            grp.gr.Q_max = query.value("q_max").toDouble();

            grp.diam = query.value("d_nomin").toDouble();
            grp.diam1 = query.value("d_nomin").toDouble();
            grp.c_vr1 = query.value("rate_nomin").toDouble();
            grp.S = 0;
            grp.Spr = false;
            grp.Nmin = 0;
            grp.Nmax = 0;
            grp.IsDnom = false;
            grp.IsFnom = false;

            grp.gr.k_kol = 1;

            // Читаем значения для графиков (H, N, KPD, Q)
            for (int i = 0; i < 10; ++i) {
                grp.gr.H[i] = query.value(QString("h%1").arg(i + 1)).toDouble();
                grp.gr.N[i] = query.value(QString("n%1").arg(i + 1)).toDouble();
                grp.gr.KPD[i] = query.value(QString("k%1").arg(i + 1)).toDouble();
                grp.gr.Q[i] = query.value(QString("q%1").arg(i + 1)).toDouble();
            }

        } else {
            qDebug() << "standardPumps: No data found for id:" << id;
        }
    } else {
        qDebug() << "standardPumps: Query failed:" << query.lastError().text();
    }
}

void PumpGraphTemplate::initData(GRAPH_PARAM * GR){
    DiamGR=(*GR);
    gr_rap=0;


    double sm = 1.0;

    stepX  = 20*sm; // шаг между линиями сетки по X
    stepY  = 14*sm; // ... по Y

    nGridX = 20; // количество линий сетки по X
    nGridY = 24; // ... по Y
    llx=20;
    lly=10;
    lX =llx*sm+45*sm,     // значение X левого верхнего угла
        lY =lly*sm+30*sm,     // значение Y ...
        rX = lX+stepX*nGridX, // значение X проавого нижнего угла
        rY = lY+stepY*nGridY; // значение Y ...


    inihq( DiamGR.Q, DiamGR.Q, DiamGR.k_kol ); //масштабирование к кол-ву насосов
    DiamGR.Q_min=DiamGR.Q_min*DiamGR.k_kol;//масштабирование к кол-ву насосов
    DiamGR.Q_max=DiamGR.Q_max*DiamGR.k_kol;//масштабирование к кол-ву насосов
}


void PumpGraphTemplate::initData2(GRAPH_PARAM * GR){
    DiamGR=(*GR);
    gr_rap=0;



    double sm = 1.0;

    stepX  = 20*sm; // шаг между линиями сетки по X
    stepY  = 14*sm; // ... по Y

    nGridX = 20; // количество линий сетки по X
    nGridY = 24; // ... по Y
    llx=20;
    lly=10;
    lX =llx*sm+45*sm,     // значение X левого верхнего угла
        lY =lly*sm+30*sm,     // значение Y ...
        rX = lX+stepX*nGridX, // значение X проавого нижнего угла
        rY = lY+stepY*nGridY; // значение Y ...
    inihq( DiamGR.Q, DiamGR.Q, DiamGR.k_kol ); //масштабирование к кол-ву насосов
    DiamGR.Q_min=DiamGR.Q_min*DiamGR.k_kol;//масштабирование к кол-ву насосов
    DiamGR.Q_max=DiamGR.Q_max*DiamGR.k_kol;//масштабирование к кол-ву насосов

    //----------------

    //_rr2 = -1.0;
    //p.x = oldp.x = pmode.x = -1;
    //p.y = oldp.y = pmode.y = -1;
    //qq2 = hh2 = kb = del_p = del_d = d_obt = 0.0;
    //fl = 0;
    //oldMode = 0;
    //rButtonDown = lButtonDown = 0;
    //frst = 0;


    masX = 0, masY = 0;

    //анализ
    minX = minY = maxX = maxY = 0;
    inihq(DiamGR.Q, DiamGR.Q, DiamGR.k_kol); //масштабирование к кол-ву насосов
    DiamGR.Q_min = DiamGR.Q_min*DiamGR.k_kol;//масштабирование к кол-ву насосов
    DiamGR.Q_max = DiamGR.Q_max*DiamGR.k_kol;//масштабирование к кол-ву насосов
    maxX = FindMax(DiamGR.Q);
    maxY = FindMax(DiamGR.H);
    maxX += maxX * 0.2;
    maxY += maxY * 0.2;
    double pog;
    Appr(r0, r1, r2, DiamGR.H, DiamGR.Q, &pog);
}

void PumpGraphTemplate::inihq(double *n, double *mas, short kol )
{
    for( int j = 0; j < 10; j ++ )   mas[j] = ( *n ++ ) *kol;
}

void PumpGraphTemplate::Draw(QPainter &painter) {
    QPen pen(QColor(0,0,0));
    pen.setWidth(1);
    painter.setPen(pen);

    // Рисуем графики
    DrawChart(painter, H_chart, &apH, false);
    DrawChart(painter, N_chart, &apN, true);
    DrawChart(painter, KPD_chart, &apKPD, true);

    // Рисуем обрамление
    DrawEdge(painter);
}


void PumpGraphTemplate::DrawCalc2(QPainter &painter){

    QFontMetrics metrics(painter.font()); // Получаем метрики шрифта
    int ascent = metrics.ascent();
    QPen pen;
    QFont font = painter.font();
    font.setPointSize(10); // Настраиваем размер шрифта
    painter.setFont(font);

    char buf[80];

    // Заголовок
    painter.drawText(llx + 270, lly + 5, "Расчет диаметра обточки");

    if (!gr_rap) // График
    {
        int n = 0;
        const char* legend[4] = { "H, м", "Q, т/ч", "КПД, %", "N, Квт" };
        const char* legend1[4] = { "H:", "Q:", "КПД:", "N:" };

        painter.setPen(QColor(0,0,0));
        painter.drawText(rX, rY + stepY / 2 + ascent, legend[1]);

        painter.setPen(QColor(255,0,0));
        painter.drawText(llx + 0, lly + stepY + ascent, legend[0]);

        strcpy(legX, legend1[1]);
        strcpy(legY, legend1[0]);

        MasSh(minX, maxX, nGridX, &masX);
        MasSh(minY, maxY, nGridY, &masY);

        qDebug()<<"myMasX : " + QString::number(masX);
        // Зона рабочей области
        int left = static_cast<int>(lX + static_cast<double>(DiamGR.Q_min) / (static_cast<double>(masX) / stepX));
        int right = static_cast<int>(lX + static_cast<double>(DiamGR.Q_max) / (static_cast<double>(masX) / stepX));
        QRect rect(QPoint(left, lY), QPoint(right, rY));
        painter.fillRect(rect, QColor(255, 133, 44));

        // Рисование сетки
        QPen gridPen(QColor(180,180,180),1);
        painter.setPen(gridPen);
        for (int i = 0; i < nGridY; i++) {
            n = ( i % 2 ) * 5;
            if (i < nGridX) {
                painter.drawLine(QPoint(lX + stepX + (stepX * i), lY), QPoint(lX + stepX+(stepX * i), rY+n));
            }
            painter.drawLine(QPoint(lX-n, rY - stepY-(stepY * i)), QPoint(rX+n, rY -stepY- (stepY * i)));
        }

        // Оси
        pen.setWidth(2);
        pen.setColor(QColor(0,0,0));
        painter.setPen(pen);
        painter.drawLine(lX, lY, lX, rY); // Ось Y
        painter.drawLine(lX, rY, rX, rY); // Ось X

        // Подписи осей
        painter.setPen(QColor(0,0,0));
        for (int i = 0; i < 10; ++i)
        {
            sprintf(buf, "%6.0lf", i * masX * 2);
            if (i)
                painter.drawText(lX - stepX / 2 + (stepX * 2 * i) - 10, rY + stepY / 2 + ascent, buf);
        }

        for (int i = 0; i < nGridY / 2; ++i)
        {
            sprintf(buf, "%6.0lf", i * masY * 2);
            painter.drawText(lX - 35, rY - stepY / 2 - (stepY * 2 * i) + ascent, buf);
        }

        // График
        pen.setWidth(2);
        pen.setColor(QColor(255,0,0));
        painter.setPen(pen);
        DrawGraph(r0, r1, r2, &painter, lX, rY);

        if (frst == 1)
        {
            static QPoint oldP = pmode;
            oldP = p;
            p.setX((p.x() - lX) * masX / stepX);
            p.setY((rY - p.y()) * masY / stepY);
            pmode = p;

            if (pmode.x() > 0 && pmode.y() > 0)
                prdim(pmode, &painter);

            p = oldP;
        }

        // **Добавляем функционал для отображения линий**
        // **Добавляем функционал для отображения линий**
        if (showGuidelines) {
            QPen dashPen(QColor(0, 0, 255), 1, Qt::DashLine);
            painter.setPen(dashPen);

            // Рисуем вертикальную линию
            painter.drawLine(QPoint(p.x(), lY), QPoint(p.x(), rY));

            // Рисуем горизонтальную линию
            painter.drawLine(QPoint(lX, p.y()), QPoint(rX, p.y()));

            // Вычисляем координаты для текста
            double Q = ((double)(p.x() - lX) * (double)masX / stepX);
            double H = ((double)(rY - p.y()) * (double)masY / stepY);

            // Разделяем текст на две строки
            QString textQ = QString("Q: %1").arg(Q, 0, 'f', 0);
            QString textH = QString("H: %1").arg(H, 0, 'f', 0);

            // Метрики шрифта для более точного позиционирования
            QFontMetrics metrics(painter.font());
            int textHeight = metrics.height(); // Высота строки текста
            int textWidth = metrics.horizontalAdvance(textQ); // Ширина текста
            int textSpacing = 5;              // Расстояние между строками

            // Определяем положение текста
            int textX, textY;
            if (p.x() > (lX + rX) / 2) { // Правая половина графика
                textX = p.x() - 70; // Смещаем текст влево
            } else { // Левая половина графика
                textX = p.x() + 10; // Смещаем текст вправо
            }

            if (p.y() > (lY + rY) / 2) { // Нижняя половина графика
                textY = p.y() - (textHeight * 2) + textSpacing;
            } else { // Верхняя половина графика
                textY = p.y() + textHeight; // Смещаем текст вниз
            }

            // Рисуем белый фон для текста
            painter.setBrush(Qt::white);
            painter.setPen(Qt::NoPen);
            painter.drawRect(textX - 5, textY - metrics.ascent(), textWidth + 10, textHeight*2+5);

            // Рисуем рамку
            painter.setPen(Qt::black);
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(textX - 5, textY - metrics.ascent(), textWidth + 10, textHeight*2+5);

            // Устанавливаем цвет текста
            QPen textPen(QColor(0, 0, 255));
            QFont font = painter.font();
            font.setPointSize(font.pointSize() + 2);
            painter.setPen(textPen);

            // Рисуем текст (две строки)
            painter.drawText(textX, textY, textQ);             // Первая строка
            painter.drawText(textX, textY + textHeight, textH); // Вторая строка с отступом
        }
    }
}

void PumpGraphTemplate::prdim(QPoint p, QPainter *painter)
{
    double pp = 0.0, pp3 = 0.0, pp4 = 0.0;

    // double _rr0 = 0.0, _rr1 = 0.0; // Сразу подставляем нули (в DrawGraph)
    _rr2 = p.y() / pow(p.x(), 2);

    QPen pen(QColor(215,0,215));
    pen.setWidth(2);
    painter->setPen(pen);
    DrawGraph(0.0, 0.0, _rr2, painter, lX, rY);

    pp = r1 / (r2 - _rr2);
    pp4 = r0 / (r2 - _rr2);
    pp3 = pow(pp, 2) / 4.0 - pp4;
    qq2 = (-pp / 2.0) + pow(pp3, 0.5); // QQ
    hh2 = r0 + r1 * qq2 + r2 * pow(qq2, 2);  // HH

    pp4 = pow(p.x(), 0.5) / 60.0;
    pp = pow(p.y(), 0.75);

    kb = 3.65 * GrApr->c_vr1 * pp4 / pp;
    if (kb < 150.0) {
        d_obt = static_cast<double>(GrApr->diam1) * p.x() / qq2;
    } else {
        d_obt = static_cast<double>(GrApr->diam1) * pow(p.x() / qq2, 0.5);
    }

    del_p = (GrApr->diam1 - d_obt) / GrApr->diam1;
    if (kb <= 120.0) {
        del_d = 0.20;
    } else if (kb > 350.0) {
        del_d = 0.0;
    } else if (kb < 200.0) {
        del_d = 0.15;
    } else if (kb < 300.0) {
        del_d = 0.11;
    } else if (kb < 350.0) {
        del_d = 0.09;
    } else if (kb == 350.0) {
        del_d = 0.07;
    }
}

void PumpGraphTemplate::DrawCalc(QPainter &painter) {
    QPen pen(QColor(0,0,0));
    pen.setWidth(1);
    painter.setPen(pen);


    char buf[1000];


    bool prepared=false,Temp_chart;
    if (Show_H_chart)
    {
        Temp_chart=Show_Hr_chart;
        Show_Hr_chart=false;
        DrawChart(painter,H_chart,&apH,prepared);
        Show_Hr_chart=Temp_chart;
        prepared=true;
    }
    if (Show_Hr_chart)
    {
        DrawChart(painter,H_chart,&apHr,prepared);
        prepared=true;
    }
    if (Show_N_chart)
    {
        Temp_chart=Show_Nr_chart;
        Show_Nr_chart=false;
        DrawChart(painter,N_chart,&apN,prepared);
        Show_Nr_chart=Temp_chart;
        prepared=true;
    }
    if (Show_Nr_chart)
    {
        DrawChart(painter,N_chart,&apNr,prepared);
        prepared=true;
    }

    if (Show_KPD_chart)
    {
        Temp_chart=Show_KPDr_chart;
        Show_KPDr_chart=false;
        DrawChart(painter,KPD_chart,&apKPD,prepared);
        Show_KPDr_chart=Temp_chart;
        prepared=true;
    }
    if (Show_KPDr_chart)
    {
        DrawChart(painter,KPD_chart,&apKPDr,prepared);
        prepared=true;
    }

    if (prepared) DrawEdge(painter);

}


void PumpGraphTemplate::DrawContent(QPainter &painter) {
    QString str = "Тип насоса"; // Замените "Type of Graph" на текст, соответствующий `IDS_GRAPHR_TYPE` в ресурсе
    painter.drawText(QPoint(llx + 55, lly + 5 ), str);

    // Рисуем вторую строку текста
    painter.drawText(QPoint(llx + 145, lly + 5 ), DiamGR.Tip);

    int posit = 40;
    const int ___Shift = 400;
    char buf[100];

    painter.drawText(llx + 15, lly + posit, "Количество насосов:");
    sprintf(buf, "%4d", GrApr->gr.k_kol);
    painter.drawText(llx + ___Shift, lly + posit, buf);

    posit += 20;
    painter.drawText(llx + 15, lly + posit, "Номинальный диаметр рабочего колеса, мм");
    sprintf(buf, "%4.3f", GrApr->diam1);
    painter.drawText(llx + ___Shift, lly + posit, buf);

    posit += 20;
    painter.drawText(llx + 15, lly + posit, "Заданный диаметр рабочего колеса, мм");
    sprintf(buf, "%4.3f", GrApr->diam);
    painter.drawText(llx + ___Shift, lly + posit, buf);

    posit += 20;
    painter.drawText(llx + 15, lly + posit, "Номинальная частота вращения, 1/мин");
    sprintf(buf, "%4.3f", GrApr->c_vr1);
    painter.drawText(llx + ___Shift, lly + posit, buf);

    posit += 20;
    painter.drawText(llx + 15, lly + posit, "Заданная частота вращения, 1/мин");
    sprintf(buf, "%4.3f", GrApr->c_vr);
    painter.drawText(llx + ___Shift, lly + posit, buf);

    painter.drawText(llx + 200, lly + 140, "Рабочая зона:");
    painter.drawText(llx + 15, lly + 160, "Левая граница:");
    painter.drawText(llx + 15, lly + 180, "Правая граница:");
    painter.drawText(llx + 130, lly + 160, "напор -              м,");
    painter.drawText(llx + 130, lly + 180, "напор -              м,");
    painter.drawText(llx + 250, lly + 160, "расход -                  т/ч");
    painter.drawText(llx + 250, lly + 180, "расход -                  т/ч");

    double a = DiamGR.Q_min;
//    double b = apH.r0 + apH.r1 * a + apH.r2 * pow(a, 2);
    double b = apHr.r0 + apHr.r1 * a + apHr.r2 * pow(a, 2);

    for (int i = 0; i < 2; ++i) {
        sprintf(buf, "%5.2f", a);
        painter.drawText(llx + 300, lly + 160 + 20 * i, buf);
        sprintf(buf, "%5.2f", b);
        painter.drawText(llx + 170, lly + 160 + 20 * i, buf);

        a = DiamGR.Q_max;
//        b = apH.r0 + apH.r1 * a + apH.r2 * pow(a, 2);
        b = apHr.r0 + apHr.r1 * a + apHr.r2 * pow(a, 2);
    }

    posit += 80;
    painter.drawText(llx + 170, lly + posit, "Уравнения характеристик:");

    posit += 20;
    painter.drawText(llx + 50, lly + posit, "Номинальный режим:");
    painter.drawText(llx + 350, lly + posit, "Заданный режим:");

    posit += 20;
    sprintf(buf, "H = %4.2f + %4.2f Q + %4.2f Q^2", apH.r0, apH.r1, apH.r2);
    painter.drawText(llx + 15, lly + posit, buf);
    sprintf(buf, "H = %4.2f + %4.2f Q + %4.2f Q^2", apHr.r0, apHr.r1, apHr.r2);
    painter.drawText(llx + 300, lly + posit, buf);

    posit += 20;
    sprintf(buf, "(Макс. ошибка аппроксимации - %4.6f)", apH.pog);
    painter.drawText(llx + 15, lly + posit, buf);
    sprintf(buf, "(Макс. ошибка аппроксимации - %4.6f)", apHr.pog);
    painter.drawText(llx + 250, lly + posit, buf);

    posit += 20;
    sprintf(buf, "N = %4.2f + %4.2f Q + %4.2f Q^2", apN.r0, apN.r1, apN.r2);
    painter.drawText(llx + 15, lly + posit, buf);
    sprintf(buf, "N = %4.2f + %4.2f Q + %4.2f Q^2", apNr.r0, apNr.r1, apNr.r2);
    painter.drawText(llx + 300, lly + posit, buf);

    posit += 20;
    sprintf(buf, "(Макс. ошибка аппроксимации - %4.6f)", apN.pog);
    painter.drawText(llx + 15, lly + posit, buf);
    sprintf(buf, "(Макс. ошибка аппроксимации - %4.6f)", apNr.pog);
    painter.drawText(llx + 250, lly + posit, buf);

    posit += 20;
    sprintf(buf, "КПД = %4.2f + %4.2f Q + %4.2f Q^2", apKPD.r0, apKPD.r1, apKPD.r2);
    painter.drawText(llx + 15, lly + posit, buf);
    sprintf(buf, "КПД = %4.2f + %4.2f Q + %4.2f Q^2", apKPDr.r0, apKPDr.r1, apKPDr.r2);
    painter.drawText(llx + 300, lly + posit, buf);

    posit += 20;
    sprintf(buf, "(Макс. ошибка аппроксимации - %4.6f)", apKPD.pog);
    painter.drawText(llx + 15, lly + posit, buf);
    sprintf(buf, "(Макс. ошибка аппроксимации - %4.6f)", apKPDr.pog);
    painter.drawText(llx + 250, lly + posit, buf);

    posit += 20;
    painter.drawText(llx + 120, lly + posit, "где    [H] = м.вод.ст;    [Q] = т/ч;    [КПД] = проценты");
}

void PumpGraphTemplate::DrawContent2(QPainter &painter){
    QFontMetrics metrics(painter.font());
    int ascent = metrics.ascent(); // Высота шрифта
    int offsetY = ascent + 5; // Смещение по вертикали для следующей строки

    char buf[128];

    // Текст 1: Номинальный диаметр рабочего колеса
    painter.drawText(llx + 15, lly + 60, "Номинальный диаметр рабочего колеса, мм");
    sprintf(buf, "%4d", (int)GrApr->diam1);
    painter.drawText(llx + 440, lly + 60, buf);

    // Текст 2: Номинальная частота вращения
    painter.drawText(llx + 15, lly + 60 + offsetY, "Номинальная частота вращения, 1/мин");
    sprintf(buf, "%4d", (int)GrApr->c_vr1);
    painter.drawText(llx + 440, lly + 60 + offsetY, buf);

    // Текст 3: Диаметр обточенного рабочего колеса
    painter.drawText(llx + 15, lly + 60 + 2 * offsetY, "Диаметр обточенного рабочего колеса, мм");
    sprintf(buf, "%4.02f", d_obt);
    painter.drawText(llx + 440, lly + 60 + 2 * offsetY, buf);

    // Текст 4: Режимная точка
    sprintf(buf, "Режимная точка :  Q = %5d т/ч ;    H = %5d м", pmode.x(), pmode.y());
    painter.drawText(llx + 15, lly + 60 + 4 * offsetY, buf);

    // Текст 5: Точка на номинальной характеристике
    sprintf(buf, "Точка на номинальной характеристике : Q = %5.0lf т/ч ;    H = %5.0lf м", qq2, hh2);
    painter.drawText(llx + 15, lly + 60 + 5 * offsetY, buf);

    // Текст 6: Коэффициент быстроходности
    painter.drawText(llx + 15, lly + 60 + 7 * offsetY, "Коэффициент быстроходности, об/мин");
    sprintf(buf, "%5.0lf", kb);
    painter.drawText(llx + 440, lly + 60 + 7 * offsetY, buf);

    // Текст 7: Полученная относительная величина обточки
    painter.drawText(llx + 15, lly + 60 + 8 * offsetY, "Полученная относительная величина обточки :");
    sprintf(buf, "%8.6lf", del_p);
    painter.drawText(llx + 440, lly + 60 + 8 * offsetY, buf);

    // Текст 8: Допустимая относительная величина обточки
    painter.drawText(llx + 15, lly + 60 + 9 * offsetY, "Допустимая относительная величина обточки :");
    sprintf(buf, "%8.4lf", del_d);
    painter.drawText(llx + 440, lly + 60 + 9 * offsetY, buf);

    // Проверка и вывод текста обточки
    if (del_p <= del_d) {
        painter.setPen(QColor(0,255,0));
        painter.drawText(llx + 150, lly + 60 + 11 * offsetY, "Обточка допустима");
    } else {
        painter.setPen(QColor(255,0,0));
        painter.drawText(llx + 150, lly + 60 + 11 * offsetY, "Обточка недопустима");
    }
}

double PumpGraphTemplate::FindMax( double *arr )
{
    double _maxY_=arr[0];
    for( int i=1; i<10; i++ ) if( arr[i] > _maxY_) _maxY_= arr[i];
    return _maxY_;
}

void PumpGraphTemplate::DrawChart(QPainter &painter, int type, APPROX_PARAM *apr_par, bool prepared) {
    QFontMetrics metrics(painter.font()); // Получаем метрики шрифта
    int ascent = metrics.ascent();
    if (!prepared) {
        painter.setPen(QColor(0,0,0));
        // Масштаб
        minX = minY = 0;
        maxX = FindMax(DiamGR.Q);
        MasSh(minX, maxX, nGridX, &masX);

        int left = static_cast<int>(lX + static_cast<double>(DiamGR.Q_min) / (static_cast<double>(masX) / stepX));
        int right = static_cast<int>(lX + static_cast<double>(DiamGR.Q_max) / (static_cast<double>(masX) / stepX));
        QRect rect(QPoint(left, lY), QPoint(right, rY));
        painter.fillRect(rect, QColor(255, 133, 44));

        // Сетка
        int i,n = 0;
        QPen gridPen(QColor(180,180,180),1);
        painter.setPen(gridPen);
        for (i = 0; i < nGridY; i++) {
            n = ( i % 2 ) * 5;
            if (i < nGridX) {
                painter.drawLine(QPoint(lX + stepX + (stepX * i), lY), QPoint(lX + stepX+(stepX * i), rY+n));
            }
            painter.drawLine(QPoint(lX-n, rY - stepY-(stepY * i)), QPoint(rX+n, rY -stepY- (stepY * i)));
        }

        painter.setPen(QColor(0,0,0));
        // Подписи осей
        // Рисуем первую строку текста
        QString str = "Тип насоса"; // Замените "Type of Graph" на текст, соответствующий `IDS_GRAPHR_TYPE` в ресурсе
        painter.drawText(QPoint(llx + 55, lly + 5 + ascent), str);

        // Рисуем вторую строку текста
        painter.drawText(QPoint(llx + 145, lly + 5 + ascent), DiamGR.Tip);



        for (int i = 0; i < 10; ++i) {
            QPoint f1 = QPoint(lX - stepX / 2 + (stepX * 2 * i), rY + stepY / 2 + ascent);
            painter.drawText(f1, QString::number(i * masX * 2));
        }
        painter.drawText(QPoint( rX,(long)(rY+stepY/2 + ascent)), "Q, т/ч");
    }

    // Рисование графика на основе типа
    QPen * graphPen=NULL;

    switch (type) {
    case H_chart:
        maxY = FindMax(DiamGR.H);
        MasSh(minY, maxY, nGridY-10+1, &masY);

        painter.setPen(QColor(255,0,0)); // Устанавливаем цвет текста

        for (int i = 0; i < 8; i++) { // Значения по оси H
            QString text = QString::asprintf("%4g", i * masY * 2); // Форматируем текст
            int x = lX - 35; // Координата X
            int y = rY - stepY / 2 - (stepY * 2 * i); // Координата Y
            painter.drawText(QPoint(x, y +ascent), text); // Рисуем текст
        }

        painter.drawText(QPoint(lX - 1.5 * stepX, rY - 16 * stepY +ascent), "H, м");
        Appr( apr_par->r0 , apr_par->r1 , apr_par->r2 , DiamGR.H, DiamGR.Q, &apr_par->pog );
        if (calcMode)
            ChangeDrawParams(type,apr_par,graphPen);
        else
            ChangeDrawParams2(type,apr_par,graphPen);
        painter.setPen(*graphPen);
        DrawGraph(apr_par->r0, apr_par->r1, apr_par->r2, &painter, lX, rY);
        break;

    case N_chart:
        maxY = FindMax(DiamGR.N);
        MasSh(minY, maxY, nGridY / 2, &masY);
        painter.setPen(QColor(0, 213,106)); // Устанавливаем цвет текста

        for (int i = 0; i < 6; ++i) { // Значения по оси N
            QString text = QString::asprintf("%4g", i * masY * 2); // Форматируем текст
            int x = rX + 7; // Координата X
            int y = rY - stepY / 2 - (stepY * 2 * i); // Координата Y
            painter.drawText(QPoint(x, y+ascent), text); // Рисуем текст
        }

        painter.drawText(QPoint(rX + 3, rY - 11.9 * stepY + ascent), "N, кВт");
        Appr( apr_par->r0 , apr_par->r1 , apr_par->r2 , DiamGR.N, DiamGR.Q, &apr_par->pog  );
        if (calcMode)
            ChangeDrawParams(type,apr_par,graphPen);
        else
            ChangeDrawParams2(type,apr_par,graphPen);
        painter.setPen(*graphPen);
        DrawGraph(apr_par->r0, apr_par->r1, apr_par->r2, &painter, lX, rY);
        break;

    case KPD_chart:
        masY = 10;
        painter.setPen(QColor( 128,128,255)); // Устанавливаем цвет текста

        for (int i = 0; i <= 5; ++i) { // Значения по оси КПД
            QString text = QString::asprintf("%3d", i * 20); // Форматируем текст
            int x = rX + 7; // Координата X
            int y = rY - stepY * 14 - stepY / 2 - (stepY * 2 * i); // Координата Y
            painter.drawText(QPoint(x, y+ascent), text); // Рисуем текст
        }
        painter.drawText(QPoint(rX + 3, lY - 1.5 * stepY +ascent), "КПД, %");
        Appr( apr_par->r0 , apr_par->r1 , apr_par->r2 ,DiamGR.KPD, DiamGR.Q, &apr_par->pog  );

        if (calcMode)
            ChangeDrawParams(type,apr_par,graphPen);
        else
            ChangeDrawParams2(type,apr_par,graphPen);
        painter.setPen(*graphPen);
        DrawGraph(apr_par->r0, apr_par->r1, apr_par->r2, &painter, lX, rY - stepY * 14);
        break;
    }
}

void PumpGraphTemplate::DrawGraph(double r0, double r1, double r2, QPainter* painter, int col, int row) {
    double p1 = static_cast<double>(maxX - minX) / (rX - lX);
    double Q = minX;
    double H = r0 + r1 * Q + r2 * pow(Q, 2);

    double sX = col + Q / (static_cast<double>(masX) / stepX);
    double sY = row - H / (static_cast<double>(masY) / stepY);
    double a1 = static_cast<double>(masX) / stepX;
    double a2 = static_cast<double>(masY) / stepY;

    int iter = rX - lX;

    // Ограничиваем значения sY в допустимых пределах
    if (sY >= rY) sY = rY;
    if (sY <= lY) sY = lY;

    QPointF prevPoint(sX, sY); // Начальная точка

    for (int i = 0; i < iter; i++) {
        H = r0 + r1 * Q + r2 * pow(Q, 2);
        sX = col + Q / a1;
        sY = row - H / a2;

        if (sY <= rY && sY >= lY) {
            QPointF currentPoint(sX, sY);
            painter->drawLine(prevPoint, currentPoint); // Рисуем линию
            prevPoint = currentPoint; // Обновляем начальную точку
        }

        Q += p1;
    }
}

void PumpGraphTemplate::Appr( double &r0,double &r1,double &r2,double *arr1, double *arr2, double* pog )
{
    double b1=0,b2=0,b3=0;
    double am11=10,am12=0,am13=0,am21=0,am22=0,am23=0,
        am31=0,am32=0,am33=0;
    double av11=0,av12=0,av13=0,av21=0,av22=0,av23=0,
        av31=0,av32=0,av33=0;
    double d=0;
    int i;

    for(i=0; i<10; i++)  {
        b1 += (double)arr1[i];
        b2 += (double)arr1[i]*arr2[i];
        b3 += arr1[i]*pow( arr2[i], 2 );
        am12 += arr2[i];
        am13 += pow( arr2[i], 2 );
        am23 += pow( arr2[i], 3 );
        am33 += pow( arr2[i], 4 );
    }
    am21=am12;  am22=am13;
    am31=am13;  am32=am23;

    av11=am22*am33-am32*am23;    av12=-(am21*am33-am31*am23); av13=am21*am32-am31*am22;
    av21=-(am12*am33-am32*am13); av22=am11*am33-am31*am13;    av23=-(am11*am32-am31*am12);
    av31=am12*am23-am22*am13;    av32=-(am11*am23-am21*am13); av33=am11*am22-am21*am12;

    d=am11*am22*am33+am12*am23*am31+am21*am32*am13-am13*am22*am31-am12*am21*am33-am11*am23*am32;
    if (d == 0) {
        return;
    }

    r0=(av11*b1+av12*b2+av13*b3)/d;
    r1=(av21*b1+av22*b2+av23*b3)/d;
    r2=(av31*b1+av32*b2+av33*b3)/d;
    d=0.0;
    for( i=0; i<10; i++ ) if(arr1[i]>d) d=arr1[i];
    b3=200.0;
    for(i=0; i<10; i++)  {
        b1 = r0 + r1*arr2[i] + r2*pow( arr2[i], 2 );
        b2 = (b1 - arr1[i]) * 100 / d;
        if(b2<0.0) b2=-b2;
        if(b3>b2) b3=b2;
    }
    *pog=(double)b3;
}

void PumpGraphTemplate::MasSh(double  min, double max, short kol, double *mas )
{
    double p = (max - min) / kol;  // Рассчитываем шаг
    int masd = static_cast<int>(p) + 1;

    if (masd > 10) {
        // Исправленный цикл
        int j;
        do {
            int temp = masd / 10;  // Чёткое деление
            j = masd - (temp * 10); // Остаток от деления
            ++masd;                // Инкремент
        } while (j != 0);
        --masd;  // Сокращаем значение на 1 после выхода
    }

    *mas = masd; // Сохраняем результат
}

void PumpGraphTemplate::DrawEdge(QPainter &painter) {
    QPen edgePen(QColor(0,0,0));
    edgePen.setWidth(2);
    painter.setPen(edgePen);

    painter.drawLine(QPoint(lX, lY), QPoint(lX, rY));
    painter.drawLine(QPoint(lX, rY), QPoint(rX, rY));
    painter.drawLine(QPoint(rX, rY - (stepY * 12)), QPoint(rX, rY));
    painter.drawLine(QPoint(rX, rY - (stepY * 14)), QPoint(rX, lY));
}

void PumpGraphTemplate::ChangeDrawParams(int type, APPROX_PARAM *apr_par, QPen* &pen)
{
    double p1 = 1.0, p2 = 1.0;
    if (!GrApr->IsFnom) p2 = (double)GrApr->c_vr / (double)GrApr->c_vr1;
    if (!GrApr->IsDnom) p1 = (double)GrApr->diam / (double)GrApr->diam1;

    switch (type)
    {
    case H_chart :
    {
        if (Show_Hr_chart)
        {
            if( p1 && p2 )
            {
                apr_par->r0 = apr_par->r0 * pow( p1, 2 ) * pow( p2, 2 );
                apr_par->r1 = apr_par->r1 * p1 * p2;
                if (GrApr->Spr)apr_par->r2 = apr_par->r2 - GrApr->S;
                else apr_par->r2 = apr_par->r2;
                if (pen==NULL)
                {

                    pen=new QPen(QColor(205,0,0), 1);

                }
            }
            else ChangeDrawParams2(type,apr_par,pen);
        }
        else
        {
            ChangeDrawParams2(type,apr_par,pen);
        }
        break;
    }
    case N_chart :
    {
        if (Show_Nr_chart)
        {
            if( p1 && p2 )
            {
                apr_par->r0 = apr_par->r0 * pow( p2, 3 ) * pow( p1, 5 );
                apr_par->r1 = apr_par->r1 * pow( p2, 2 ) * pow( p1, 4 );
                apr_par->r2 = apr_par->r2  * p2 * pow( p1, 3 );
                if (pen==NULL)
                {
                    pen=new QPen(QColor(0, 113,6), 1);//График NQ_zad
                }
            }
            else ChangeDrawParams2(type,apr_par,pen);
        }
        else
        {
            ChangeDrawParams2(type,apr_par,pen);
        }
        break;
    }
    case KPD_chart :
    {
        if (Show_KPDr_chart)
        {
            if( p1 && p2 )
            {
                apr_par->r0 = apr_par->r0;
                apr_par->r1 = apr_par->r1  / ( p1 * p2 );
                apr_par->r2 = apr_par->r2  / ( pow( p1, 2 ) * pow( p2, 2 ) );
                if (pen==NULL)
                {

                    pen=new QPen(QColor(108,108,205), 1);//График NQ_zad
                }
            }
            else ChangeDrawParams2(type,apr_par,pen);

        }
        else
        {
            ChangeDrawParams2(type,apr_par,pen);
        }
        break;
    }
    }
}

void PumpGraphTemplate::ChangeDrawParams2(int type, APPROX_PARAM *apr_par, QPen* &pen) {

    if (pen==NULL)
        switch (type) {
        case H_chart:
            pen = new QPen(QColor(255,0,0), 3);
            break;
        case N_chart:
            pen = new QPen(QColor(0, 213,106), 3);
            break;
        case KPD_chart:
            pen = new QPen(QColor(128,128,255), 3);
            break;
        }
}
