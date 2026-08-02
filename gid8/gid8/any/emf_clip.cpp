#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QBuffer>
#include <QMimeData>

#if 0
#include <windows.h>


bool copyEmfToClipboard(const QString &filePath) 
{
    // Открываем EMF файл
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Не удалось открыть файл: %s", qPrintable(file.errorString()));
        return false;
    }
    QByteArray emfData = file.readAll();
    file.close();

    // Создаем метафайл из данных EMF
    HENHMETAFILE hEMF = SetEnhMetaFileBits(emfData.size(), reinterpret_cast<const BYTE*>(emfData.constData()));
    if (!hEMF) {
        qWarning("Не удалось создать метафайл.");
        return false;
    }

    // Открываем буфер обмена
    if (!OpenClipboard(nullptr)) {
        qWarning("Не удалось открыть буфер обмена.");
        DeleteEnhMetaFile(hEMF);
        return false;
    }

    // Очищаем буфер обмена
    EmptyClipboard();

    // Устанавливаем EMF данные в буфер обмена
    SetClipboardData(CF_ENHMETAFILE, hEMF);

    // Закрываем буфер обмена
    CloseClipboard();

    // Удаляем дескриптор метафайла (данные в буфере обмена сохраняются)
    DeleteEnhMetaFile(hEMF);

    return true;
}

/*
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QString filePath = "path_to_your_file.emf";
    copyEmfToClipboard(filePath);

    return app.exec();
}
*/

#endif