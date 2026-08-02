#include <QApplication>
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <cairo.h>
#include <cairo-win32.h>
#include <librsvg/rsvg.h>

bool convertSvgToEmf(const QString &svgFilePath, const QString &emfFilePath) 
{
    // Инициализация GDK и Cairo
    gdk_init(nullptr, nullptr);

    // Открываем SVG файл
    QFile svgFile(svgFilePath);
    if (!svgFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть SVG файл:" << svgFile.errorString();
        return false;
    }

    QByteArray svgData = svgFile.readAll();
    svgFile.close();

    // Создаем RSVG обработчик
    RsvgHandle *handle = rsvg_handle_new_from_data(reinterpret_cast<const guint8 *>(svgData.constData()), svgData.size(), nullptr);
    if (!handle) {
        qWarning() << "Не удалось создать RSVG обработчик.";
        return false;
    }

    // Получаем размер SVG изображения
    RsvgDimensionData dimensions;
    rsvg_handle_get_dimensions(handle, &dimensions);

    // Создаем устройство вывода Cairo для EMF файла
    HDC hdc = CreateEnhMetaFileA(nullptr, emfFilePath.toStdString().c_str(), nullptr, nullptr);
    if (!hdc) {
        qWarning() << "Не удалось создать EMF файл.";
        g_object_unref(handle);
        return false;
    }

    cairo_surface_t *surface = cairo_win32_printing_surface_create(hdc);
    cairo_t *cr = cairo_create(surface);

    // Устанавливаем размер холста
    cairo_scale(cr, dimensions.width, dimensions.height);

    // Рендерим SVG на поверхность Cairo
    if (!rsvg_handle_render_cairo(handle, cr)) {
        qWarning() << "Не удалось отрендерить SVG.";
    }

    // Завершаем работу с Cairo
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    // Завершаем работу с EMF файлом
    HENHMETAFILE hemf = CloseEnhMetaFile(hdc);
    if (!hemf) {
        qWarning() << "Не удалось закрыть EMF файл.";
    }

    // Освобождаем RSVG обработчик
    g_object_unref(handle);

    return true;

}

