#include <QString>
#include <QFile>
#include <QByteArray>
#include <QDebug>

extern "C" {
#include "lib/minizip/zip.h"    // из minizip (обычно в zlib/contrib/minizip/)
#include "lib/minizip/unzip.h"    // из minizip (обычно в zlib/contrib/minizip/)
//#include "lib/minizip/ioapi.h"
}

bool zip_create(const QString &zipFileName, const QString &fileToAdd, const QString &insideName)
{
    // Создаём zip
    zipFile zf = zipOpen(zipFileName.toUtf8().constData(), APPEND_STATUS_CREATE);
    if (!zf) {
        qWarning() << "Не удалось создать zip:" << zipFileName;
        return false;
    }

    zip_fileinfo zi = {};
    if (zipOpenNewFileInZip(zf,
                            insideName.toUtf8().constData(),
                            &zi,
                            nullptr, 0, nullptr, 0, nullptr,
                            Z_DEFLATED, Z_DEFAULT_COMPRESSION) != ZIP_OK)
    {
        qWarning() << "Ошибка: zipOpenNewFileInZip";
        zipClose(zf, nullptr);
        return false;
    }

    QFile f(fileToAdd);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл:" << fileToAdd;
        zipCloseFileInZip(zf);
        zipClose(zf, nullptr);
        return false;
    }

    QByteArray buffer;
    while (!(buffer = f.read(8192)).isEmpty()) {
        if (zipWriteInFileInZip(zf, buffer.constData(), buffer.size()) < 0) {
            qWarning() << "Ошибка записи";
            f.close();
            zipCloseFileInZip(zf);
            zipClose(zf, nullptr);
            return false;
        }
    }
    f.close();

    zipCloseFileInZip(zf);
    zipClose(zf, nullptr);
    return true;
}


bool zip_extract(const QString &zipFileName, const QString &outputDir)
{
    unzFile uf = unzOpen(zipFileName.toUtf8().constData());
    if (uf == nullptr) {
        qWarning() << "Cannot open zip file" << zipFileName;
        return false;
    }

    if (unzGoToFirstFile(uf) != UNZ_OK) {
        qWarning() << "Zip is empty";
        unzClose(uf);
        return false;
    }

    do {
        char filename[512];
        unz_file_info fileInfo;
        if (unzGetCurrentFileInfo(uf, &fileInfo, filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK) {
            qWarning() << "Error reading file info";
            unzClose(uf);
            return false;
        }

        QString outPath = outputDir + "/" + QString::fromUtf8(filename);

        if (filename[strlen(filename)-1] == '/') {
            // это директория
            QDir().mkpath(outPath);
        } else {
            if (unzOpenCurrentFile(uf) != UNZ_OK) {
                qWarning() << "Cannot open file inside zip:" << filename;
                unzClose(uf);
                return false;
            }

            QDir().mkpath(QFileInfo(outPath).absolutePath());

            QFile outFile(outPath);
            if (!outFile.open(QIODevice::WriteOnly)) {
                qWarning() << "Cannot create file:" << outPath;
                unzCloseCurrentFile(uf);
                unzClose(uf);
                return false;
            }

            char buf[4096];
            int readBytes = 0;
            while ((readBytes = unzReadCurrentFile(uf, buf, sizeof(buf))) > 0) {
                outFile.write(buf, readBytes);
            }

            outFile.close();
            unzCloseCurrentFile(uf);
        }
    } while (unzGoToNextFile(uf) == UNZ_OK);

    unzClose(uf);
    return true;
}
