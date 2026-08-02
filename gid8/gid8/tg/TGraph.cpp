#include <QtGui>
#include <QtWidgets>

#include "std.h"

#include "TGraph.h"

#include <db/db.h>


#include "tempgraph.h"


#define RGB2(b,g,r) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))


#define _basic_color RGB2(0,0,0)
#define _setka_color RGB2( 180, 180,180 )
#define _POD_color RGB2(255,0,0)
#define _OBR_color RGB2( 128,128,255 )
#define _SMESH_color RGB2( 0, 213,106 )
#define _VETER_color RGB2(200,200,0)

static bool is_smesh;


int round_to_dig(double val)
{
    long lVal = (val);
    if (((lVal - val) >= 0.5) && (val < 0))lVal--;
    if (((val - lVal) >= 0.5) && (val >= 0))lVal++;
    return lVal;
}

void FindMinMax(double &min, double &max, double *arr, long kol)
{
    min = max = arr[0];
    for (int i = 1; i < kol; i++)
    {
        if (arr[i] > max) max = arr[i];
        if (arr[i] < min) min = arr[i];
    }
}


TGraph::TGraph(QWidget *parent)
    : QWidget(parent)
{
    stepX = 20; // шаг между линиями сетки по X
    stepY = 14; // ... по Y

    nGridX = 27; // количество линий сетки по X
    nGridY = 17; // ... по Y
    llx = 30;
    lly = 10;

    llx = 0;
    lly = 0;

    lX = llx + 45,     // значение X левого верхнего угла
    lY = lly + 30,     // значение Y ...
    rX = lX + stepX * nGridX, // значение X проавого нижнего угла
    rY = lY + stepY * nGridY; // значение Y ...
}


void TGraph::DrawLine(QPainter &painter, GR_PARAM2 &gr)
{
    double * _x = gr.x;
    double * _y = gr.y;

    int pen_width = 2;
    long kol = gr.kol;
    
    painter.setPen(QPen(QColor(gr.pen), pen_width));



    if (gr.show)
    {
        qDebug() << "-------------------------------";
        for (long i = 1; i < kol; i++)
        {

            qDebug() << _x[i-1] << "  " << _y[i-1] << " --- " << _x[i] << "  " << _y[i] ;

//            painter.drawLine(rX - (_x[i - 1] - minX) / masX, rY - (_y[i - 1] - minY) / masY, rX - (_x[i] - minX) / masX, rY - (_y[i] - minY) / masY);

            painter.drawLine(
                rX - (_x[i - 1] - minX) / masX, rY - (_y[i - 1] - minY) / masY,
                rX - (_x[i] - minX) / masX, rY - (_y[i] - minY) / masY);

        }
        qDebug() << "-------------------------------";
    }
}

void TGraph::paintEvent(QPaintEvent * /* event */)
{
    minX = minY = masX = masY = 0;

    QPainter painter(this);

    painter.drawLine(lX, lY, lX, rY);
    painter.drawLine(lX, rY, rX, rY);
    painter.drawLine(rX, rY, rX, lY);
    painter.drawLine(rX, lY, lX, lY);

    DrawEdge(painter);
}

void TGraph::resizeEvent(QResizeEvent *event)
{    
    QSize sz = event->size();

    int w = sz.width()-90;
    int h = sz.height()-60;


//    stepX = 20; // шаг между линиями сетки по X
//    stepY = 14; // ... по Y

    nGridX = 27; // количество линий сетки по X
    nGridY = 17; // ... по Y

    stepX = w/nGridX;
    stepY = h/nGridY;

    lX = llx + 45,     // значение X левого верхнего угла
    lY = lly + 30,     // значение Y ...
    rX = lX + stepX * nGridX, // значение X проавого нижнего угла
    rY = lY + stepY * nGridY; // значение Y ...

    repaint();

    QWidget::resizeEvent(event);
}

void TGraph::DrawEdge(QPainter &painter)
{
    painter.setPen(QPen(QColor(_basic_color), 2)); //окантовка

    painter.drawLine(lX, lY, lX, rY);
    painter.drawLine(lX, rY, rX, rY);

    painter.setPen(QPen(QColor(_basic_color), 1)); //подписи

    QString Str;
    minX = minY = masX = masY = 0;

    double tempminX, tempmaxX;
    FindMinMax(tempminX, tempmaxX, array1[0].x, array1[0].kol);
    if (minX > tempminX) minX = tempminX;
    if (maxX < tempmaxX) maxX = tempmaxX;

    MasShX(minX, maxX, nGridX, &masX);
    minY = 0;
    maxY = 200;
    MasShY(minY, maxY, nGridY, &masY);

    painter.setPen(QPen(QColor(_setka_color), 1));

    for (int i = 0; i < nGridX; i++) // сетка
    {
        int n = (i % 2) * 5;
        if (i < nGridY)
        {
            painter.drawLine(lX - n, rY - stepY - (stepY*i), rX, rY - stepY - (stepY*i));
        }
        painter.drawLine(lX + stepX + (stepX*i), lY, lX + stepX + (stepX*i), rY + n);
    }

    painter.setPen(QPen(QColor(_basic_color), 1)); //окантовка
    
    for (int i = 0; i <= nGridX / 2; i++) //подписи по оси X
    {
        painter.drawText(lX - stepX / 2 + (2 * stepX*i), rY + stepY / 2, QString("%1").arg(round_to_dig(maxX - (2 * stepX*i)*masX)));
    }
    painter.drawText(rX, (long)(rY + stepY / 2), "T возд., 'С");

    for (int i = 0; i<nGridY; i++) //подписи по оси Y
    {
        painter.drawText(lX - 1.5*stepX, rY - stepY * i, QString("%1").arg(round_to_dig(minY + (stepY*i)*masY)));
    }
    painter.drawText(lX - 1.5*stepX, lY , "T воды, 'С");

    painter.setPen(QPen(QColor(_setka_color), 2));

    //Если showGraphOT тогда отрисовывать дополнительно отопительный температурный график
    if (m_ch5 && showGraphOT) {
        if (m_ch1) DrawLineDotted(painter, arrayOT[0]);
        if (m_ch2) DrawLineDotted(painter, arrayOT[1]);
        if (m_ch3) DrawLineDotted(painter, arrayOT[2]);
        if (m_ch4) DrawLineDotted(painter, arrayOT[3]);
    }
   

    if (m_ch1) DrawLine(painter, array1[0]);
    if (m_ch2) DrawLine(painter, array1[1]);
    if (m_ch3) if (is_smesh) DrawLine(painter, array1[2]);
    if (m_ch4) DrawLine(painter, array1[3]);
}

void TGraph::DrawLineDotted(QPainter &painter, GR_PARAM2& gr)
{
    double* _x = gr.x;
    double* _y = gr.y;
    long kol = gr.kol;

    if (gr.show)
    {
        painter.setPen(QPen(QColor(_setka_color), 2));

        for (long i = 1; i < kol; i++)
        {
            double xM = (_x[i - 1] + _x[i])/2;
            double yM = (_y[i - 1] + _y[i]) / 2;

            painter.drawLine(rX - (_x[i - 1] - minX) / masX, rY - (_y[i - 1] - minY) / masY, rX - (xM - minX) / masX, rY - (yM - minY) / masY);
        }
    }
}


//----------------------------------------------------------------------------
void TGraph::MasShX(double  min, double max, short kol, double *mas)
{
    double masd = (max - min) / ((rX - lX)*1.0);
    *mas = masd;
}
//----------------------------------------------------------------------------
void TGraph::MasShY(double  min, double max, short kol, double *mas)
{
    double masd = (max - min) / ((rY - lY)*1.0);
    *mas = masd;
}




bool readTG(QSqlDatabase &db, QSqlQuery &query);


void TGraph::init0(QSqlDatabase &db, int id)
{
//  SetWindowText("Температурный график");

  m_id = id;
  
  showGraphOT = false;

  QSqlQuery query(db);
  query.setForwardOnly(true);

  readTG(db, query);
   
  InitArray();

  QString q = QString("SELECT * FROM heatSources WHERE ID = %1").arg(id);
//  QString q = QString("SELECT ist.* FROM heatSources ist JOIN nodes n ON n.id=ist.nodeID WHERE n.id = %1").arg(id);

  if (query_exec(db, query, q)) {
      while (query.next()) {
          int type = query.value("graphTypeID").toInt();

          if (!(type == 1 || type == 0)) {
              showGraphOT = true;
              InitArrayOT(db, query);
          }
      }
  }

//  InitLine();

  is_smesh = false;

  for (long i = 1; i<array1[2].kol; i++)
  {
      if (array1[2].y[i] != 0) {
          is_smesh = true;
          break;
      }
  }

  if (!is_smesh)
  {
      array1[2].show = FALSE;
//      m_smesh.ShowWindow(SW_HIDE);
  }

//  init();
  repaint();
}

#include "tg.h"

int getTG_n_col(long hSourceID);
bool getTG_n(long hSourceID, int n, double &tn, double &t1, double &t2, double &t3, double &tv);

void TGraph::InitArray()
{
    QString q;
    long Kol, pen_width = 2;
    std::set <TG> tgl;

    bool getTG_tn1_tn2(long hSourceID, double &tn1, double &tn2);

//    double tn1, tn2;

    TG tg;
    Kol = getTG_n_col(m_id);

    array1[0].pen = _POD_color;
    array1[1].pen = _OBR_color;
    array1[2].pen = _SMESH_color;
    array1[3].pen = _VETER_color;

    for (int j = 0; j < 4; j++) {
        array1[j].x = new double[Kol + 1];
        array1[j].y = new double[Kol + 1];
        array1[j].kol = Kol;
        array1[j].show = TRUE;
    }

    for (int i = 0; i < Kol; i++) {
        double tn, t1, t2, t3, tv;
        getTG_n(m_id, i, tn, t1, t2, t3, tv);

        tg.tn = tn;
        tg.t1 = t1;
        tg.t2 = t2;
        tg.t3 = t3;
        tg.tv = tv;
        for (int j = 0; j < 4; j++) {
            array1[j].x[i] = tg.tn;
        }

        array1[0].y[i] = tg.t1;
        array1[1].y[i] = tg.t2;
        array1[2].y[i] = tg.t3;
        array1[3].y[i] = tg.tv;
    }
    //----------------------------------------------------------------------------
}

void TGraph::InitArrayOT(QSqlDatabase & db, QSqlQuery &query)
{
    long Kol, pen_width = 2;

    arrayOT[0].pen = _POD_color;
    arrayOT[1].pen = _OBR_color;
    arrayOT[2].pen = _SMESH_color;
    arrayOT[3].pen = _VETER_color;
    
    CTempGraph* tempGraph = new CTempGraph(db, query, m_id, false);

    COMMON_TEMP_STRUCTURE * tempStruct = tempGraph->TempStruct;
    Kol = tempStruct->ot_data->n;

    for (int j = 0; j < 4; j++) {
        arrayOT[j].x = new double[Kol + 1];
        arrayOT[j].y = new double[Kol + 1];
        arrayOT[j].kol = Kol;
        arrayOT[j].show = TRUE;
    }

    for (int i = 0; i < Kol; i++) {
//        double tn, t1, t2, t3, tv;
       
        for (int j = 0; j < 4; j++) {
            arrayOT[j].x[i] = tempStruct->array_ptr[0][i]; //tn
        }

        arrayOT[0].y[i] = tempStruct->array_ptr[2][i]; //t1
        arrayOT[1].y[i] = tempStruct->array_ptr[3][i]; //t2
        arrayOT[2].y[i] = tempStruct->array_ptr[4][i]; //t3
        arrayOT[3].y[i] = tempStruct->array_ptr[5][i];  //tv
    }
    delete tempGraph->TempStruct;
    delete tempGraph->TempStruct_norm;
    delete tempGraph;
}


void TGraph::copyToClipboard()
{
    QPixmap pixmap = this->grab();

        // Копируем pixmap в буфер обмена
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(pixmap);
}
