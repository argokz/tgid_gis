//#include "std.h"
#include <QtGui>
#include <QtWidgets>
#include "Scroll.h"
#include "mystd.h"

#include <cxema/coordlis.h>

Scroll::Scroll(QWidget *parent)
#ifdef OPENGL
    : QOpenGLWidget(parent)
#else
    : QWidget(parent)
#endif
{
    m_bIsMas = false;
    bOnlyCopyBackBuffer = false;
    m_pt1 = CFPoint(0, 0);
    m_pt2 = CFPoint(0, 0);

    m_mouse_press = false;

    m_dX = 10;
    m_dX = 500;
//    m_dX = 0;

    m_dx_pix = 0;
    m_dy_pix = 0;

    geom.bx = 0, geom.by = 0, geom.masx = 1, geom.masy = 1;
    mas_otn = 1.;
    mas_1 = 1.;
    m_h = 1.;

//    qreal scale = 1.0;
    m_scale = 1.0;
    QWidget *window = this->window();
    if (window) {
        QWindow *windowHandle = window->windowHandle();
        if (windowHandle) {
            m_scale = windowHandle->devicePixelRatio();
        }
    }

//    geom.dmas = logicalDpiX()/2.54 * 1.75;
    geom.dmas = logicalDpiX()/2.54*m_scale;

    m_bk_color = Qt::white;

    m_NP_loc = LOC2;

    int heigh = m_font1.pointSize();

    if (heigh > 0) {
        int qq;
        qq = 1;
    }

    QFont font;
    font.setPointSize(8);
    setFont(font);

    setMouseTracking(true);

    scaleTimer = new QTimer(this);
    scaleTimer->setSingleShot(true);

    // Подключаем сигнал таймера к слоту завершения масштабирования
    connect(scaleTimer, &QTimer::timeout, this, [this]() {
        scaling = false;
        repaint();
    });
}

//QPainter *Scroll::beginPaint() {
//    return new QPainter(&backBuffer);
//}

//QPainter *Scroll::beginPaintX() {

//    QPainter * painter = new QPainter(&backBuffer);

//    painter->setCompositionMode(QPainter::RasterOp_NotSourceXorDestination);
//    painter->setPen(QPen(Qt::black));

//    return painter;
//}

//void Scroll::endPaint(QPainter *painter)
//{
//    delete painter;
//    repaintBackBuffer();
//}


void Scroll::repaintBackBuffer(int dx, int dy)
{
    bOnlyCopyBackBuffer = true;
    m_dx_pix = dx;
    m_dy_pix = dy;
    repaint();
    bOnlyCopyBackBuffer = false;
    m_dx_pix = 0;
    m_dy_pix = 0;
}


CFRect Scroll::ScreenToCoord(const CFRect& rect)
{
    return CFRect(
               ScreenToCoord(rect.TopLeft()),
               ScreenToCoord(rect.BottomRight()));
}

CFRect Scroll::CoordToScreen(const CFRect& rect)
{
    return CFRect(
               CoordToScreen(rect.TopLeft()),
               CoordToScreen(rect.BottomRight()));
}


/*
bool Scroll::moveRect(const CFRect& rect)
{
    CFPoint pt((rect.left+rect.right)/2, (rect.top+rect.bottom)/2);
    moveXY(pt);
    return true;
}
*/

bool Scroll::moveXY(const CFPoint& pt)
{
//  bool is_ob = m_is_ob;

    if (pt == CFPoint(0,0)) return false;

    CFRect rectScr = ScreenToCoord(CFRect(0, 0, width(), height()));

    if (!rectScr.PtInRect(pt)) {
        geom.bx = pt.x/geom.masx-width()/2;
        geom.by = pt.y/geom.masy-height()/2;
        repaint();
    }
    CFPoint p = CoordToScreen(pt);
    QCursor::setPos(mapToGlobal(QPoint(p.x, p.y)));

//  m_is_ob = is_ob;
    return true;
}


void Scroll::ScrollWindow(int dx, int dy)
{
    qDebug() << "myCheck ScrollWindow";
    geom.bx -= dx;
    geom.by -= dy;
    repaint();

    m_pt2.x -= dx*geom.masx;
    m_pt2.y -= dy*geom.masy;
}


void Scroll::init_draw(QPainter *painter, double MasPic)
{
//    double d105 = physicalDpiX();
    double d96 = logicalDpiX();
    m_h = d96/96.;
    if (m_h < 1) m_h = 1.;


//  if (IsMain() && m_cxema->isGeobaza()) {

    double stm = MasPic;
//  if (!IsMain() || !m_cxema->isGeobaza()) stm = 37 * (m_MasPic/1500);
//  if (!IsMain() || !m_cxema->isGeobaza())

//    if (!IsMain()) stm = 37 * MasPic/1500;

    mas_otn = geom.masx*geom.dmas/stm;

    if (!m_bIsMas && mas_otn < 1) mas_otn = 1;

    int heigh8 = m_font1.pointSize();

    m_lfHeight = heigh8*m_h;
//    if (mas_otn > 1.) m_lfHeight /= mas_otn;

    m_lfHeight /= mas_otn;

    mas_1 = mas_otn/m_h;
    if ( mas_1 < 1) mas_1 = 1;
//  bool m_bIsMas = true;

    QFont font = m_font1;
    font.setPixelSize(max(3., m_lfHeight));

/*

    if ( lfHeight() > 3 ) {
        font.setPixelSize(m_lfHeight);
//        painter->setFont(QFont("Helvetica", lfHeight()));
    }
    else {
        font.setPixelSize(3);
//                painter->setFont(QFont("Helvetica", 3));
    }
*/

    painter->setFont(font);

}

void Scroll::drawToBuffer()
{
    backBuffer.fill();
    QPainter painter(&backBuffer);
    init_draw(&painter, getStdMas());
    draw(&painter);
    return;
}

void qt_assert1(const char *assertion, const char *file, int line) noexcept
{
    QMessageBox::warning(nullptr, "", "!");
 //   QMessageLogger(file, line, nullptr).fatal("ASSERT: \"%s\" in file %s, line %d", assertion, file, line);
}



void Scroll::drawToPixmap(QPixmap &backBuffer, double MasPic)
{
    backBuffer.fill(m_bk_color);
    QPainter painter(&backBuffer);
    painter.setRenderHint(QPainter::Antialiasing, true);
/*
    double x1 = 537.0404023741329;
    double x2 = 545.7756763479766;
    double y1 = -0.33426811747600355;
    double y2 = -1.7431832745475366;

    painter.setPen(QPen(Qt::yellow, 1));
    painter.drawLine(QPointF(x1, y1), QPoint(x2, y2));
*/

    init_draw(&painter, MasPic);
    draw(&painter);
}



void Scroll::paintEvent(QPaintEvent * /* event */)
{
    if (!bOnlyCopyBackBuffer) {
//        qreal scale = 1.0;
        m_scale = 1.0;
        QWidget *window = this->window();
        if (window) {
            QWindow *windowHandle = window->windowHandle();
            if (windowHandle) {
                m_scale = windowHandle->devicePixelRatio();
            }
        }

        backBuffer = QPixmap ( width()*m_scale, height()*m_scale );


        backBuffer.setDevicePixelRatio(m_scale);
        drawToPixmap(backBuffer, getStdMas());
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPixmap(0+m_dx_pix, 0+m_dy_pix, backBuffer, 0, 0, 0, 0 );

    XDraw(&painter, 0+m_dx_pix, 0+m_dy_pix);
}


void Scroll::onF5()  // Перерисовать схему
{
    qDebug() << "F5()";
    repaint();
}

/*
void Scroll::onCtrlF5()  // Перерисовать схему
{
    qDebug() << "onCtrlF5()";
    m_reread = true;
    repaint();
}
*/

/*
void Scroll::SetBorder(const CFRect & rect)
{
  m_rectBorder = rect;
  m_rectBorder.NormalizeRect();
}

*/

void saveImageToClipboard() {
    // Создание растрового изображения
    QImage image(200, 100, QImage::Format_ARGB32);
    image.fill(Qt::white); // Заполнение фона белым цветом

    // Создание QPainter для рисования на изображении
    QPainter painter(&image);
    painter.drawText(image.rect(), Qt::AlignCenter, "Hello, Qt!");
    painter.end();

    // Преобразование изображения в массив байтов
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG"); // Сохранение изображения в буфер в формате PNG
    buffer.close();

    // Создание MIME-данных и установка изображения
    QMimeData *mimeData = new QMimeData;
    mimeData->setImageData(image);

    // Копирование MIME-данных в буфер обмена
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);

    qDebug() << "Image saved to clipboard, size:" << byteArray.size();
}

void savePictureToClipboard1() {
    // Создание метафайла
    QPicture picture;

    // Создание QPainter для рисования на метафайле
    QPainter picturePainter(&picture);
    // Пример рисования
    picturePainter.drawText(0, 0, 200, 100, Qt::AlignCenter, "Hello, Qt!");
    picturePainter.end();

    // Преобразование метафайла в массив байтов
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    picture.save(&buffer); // Сохранение метафайла в буфер
    buffer.close();

    // Создание MIME-данных и установка метафайла
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-qt-picture", byteArray);

    // Копирование MIME-данных в буфер обмена
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);

    qDebug() << "Picture saved to clipboard, size:" << byteArray.size();
}

#include <QSvgGenerator>

void savePictureToClipboard()
{
//    QBuffer b;
    QSvgGenerator p;

    p.setFileName("d:/qq.svg");
//    p.setOutputDevice(&b);
    p.setSize(QSize(800,600));
    p.setViewBox(QRect(0,0,800,600));
    p.setTitle("SVG Generator Example Drawing");
    p.setDescription("An SVG drawing created by the SVG Generator "
                                "Example provided with Qt.");

    QPainter painter;
    painter.begin(&p);
    painter.setRenderHint(QPainter::Antialiasing);
//    this->scene->render(&painter);
    painter.drawText(0, 0, 200, 100, Qt::AlignCenter, "Hello, Qt!");
    painter.end();

/*
    // Создание MIME-данных и установка метафайла
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("image/svg+xml", b.buffer());

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);

    qDebug() << "Picture saved to clipboard, size:" << b.buffer().size();
*/
/*

    QMimeData * d = new QMimeData();
    d->setData("image/svg+xml",b.buffer());
    QApplication::clipboard()->setMimeData(d,QClipboard::Clipboard);
*/
}


#define SVG_SAVE 0


bool convertSvgToEmf(const QString &svgFilePath, const QString &emfFilePath);
bool copyEmfToClipboard(const QString &filePath);


void Scroll::draw_painter(QPainter &painter)
{
    init_draw(&painter, getStdMas());
    draw(&painter);
}

#if 0

void Scroll::copyBuffer(CFRect rect)
{
///     copyEmfToClipboard("D:/Qt/python/qq.emf");
///     return;


    if (rect.Width() == 0 && rect.Height() == 0) return;
    rect.NormalizeRect();

    double m_scale_old = m_scale;
    m_scale = 1.;

    double mx = geom.masx;
    double my = geom.masy;

//    m *= 2;

    geom_old = geom;

    geom.masx = mx;
    geom.masy = my;
    geom.bx = rect.left/geom.masx;
    geom.by = rect.top/geom.masy;

    double w = rect.Width()/mx;
    double h = rect.Height()/my;

    QSvgGenerator p;

#if SVG_SAVE
    p.setFileName("d:/qq.svg");
#else

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);

    p.setOutputDevice(&buffer);
#endif


    p.setSize(QSize(w,h));
    p.setViewBox(QRect(0,0,w,h));
//    p.setTitle("SVG Generator Example Drawing");
//    p.setDescription("An SVG drawing created by the SVG Generator");

    QPainter painter;
    painter.begin(&p);
    painter.setRenderHint(QPainter::Antialiasing);

    init_draw(&painter, getStdMas());
    draw(&painter);

    painter.end();

    geom = geom_old;
    m_scale = m_scale_old;


#if SVG_SAVE
#else

    // Создаем QMimeData для хранения SVG данных
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("image/svg+xml", byteArray);

    // Сохранение в буфер обмена
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);

    qDebug() << "Picture saved to clipboard, size:" << buffer.size();
#endif
    }


#else


bool Scroll::setRect(CFRect rect)
{
    geom_old = geom;

    if (rect.Width() == 0 && rect.Height() == 0) return false;
    rect.NormalizeRect();

    double m_scale_old = m_scale;
    m_scale = 1.;

    double mx = geom.masx;
    double my = geom.masy;

    geom.masx = mx;
    geom.masy = my;
    geom.bx = rect.left/geom.masx;
    geom.by = rect.top/geom.masy;

    double w = rect.Width()/mx;
    double h = rect.Height()/my;

    return true;
}


void Scroll::copyBuffer(CFRect rect)
{

    if (rect.Width() == 0 && rect.Height() == 0) return;
    rect.NormalizeRect();

    double m_scale_old = m_scale;
    m_scale = 1.;

    double mx = geom.masx;
    double my = geom.masy;

//    m *= 2;

    geom_old = geom;

    geom.masx = mx;
    geom.masy = my;
    geom.bx = rect.left/geom.masx;
    geom.by = rect.top/geom.masy;

    double w = rect.Width()/mx;
    double h = rect.Height()/my;


    QPixmap pixmap(QSize(w, h));

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    init_draw(&painter, getStdMas());
    draw(&painter);

    painter.end();


    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(pixmap);


    geom = geom_old;
    m_scale = m_scale_old;


}


#endif
