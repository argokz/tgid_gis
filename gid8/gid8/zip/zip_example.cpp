#include <QtCore>
 
#include "qzipreader_p.h"
#include "qzipwriter_p.h"
 
int main ()
{
 
    {// Архивация файла
        QZipWriter zip(QLatin1String("file.zip"));
        if (zip.status() != QZipWriter::NoError)
            return -1;
 
        zip.setCompressionPolicy(QZipWriter::AutoCompress);
 
        QString file_name = QLatin1String("file.txt");
        QString file_path = QLatin1String("/path/to/file/file.txt");
        QFile file(file_path);
        if (!file.open(QIODevice::ReadOnly))
            return -1;
 
        zip.setCreationPermissions(QFile::permissions(file_path));
        zip.addFile(file_name, file.readAll());
        zip.close();
    }
 
//-----------------------------------------------------------------------
    {// Архивация файла внутри директории
        QZipWriter zip(QLatin1String("file_in_dir.zip"));
        if (zip.status() != QZipWriter::NoError)
            return -1;
 
        zip.setCompressionPolicy(QZipWriter::AutoCompress);
 
        QString dir_name  = QLatin1String("directory");
        QString file_name = QLatin1String("file.txt");
        QString file_path = QLatin1String("/path/to/file/file.txt");
        QFile file(file_path);
        if (!file.open(QIODevice::ReadOnly))
            return -1;
 
        //обязательно выставить атрибуты для директории
        zip.setCreationPermissions(QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner |
                                   QFile::ReadGroup|QFile::ExeGroup |
                                   QFile::ExeOther);
        zip.addDirectory(dir_name);
 
        zip.setCreationPermissions(QFile::permissions(file_path));
        zip.addFile(dir_name + QDir::separator() + file_name, file.readAll());
        zip.close();
    }
 
//-----------------------------------------------------------------------
    {// Рекурсивная архивация директории
        QZipWriter zip(QLatin1String("dir.zip"));
        if (zip.status() != QZipWriter::NoError)
            return -1;
 
        zip.setCompressionPolicy(QZipWriter::AutoCompress);
 
        QString path = QLatin1String("/path/to/dir/"); // в данном случае завершающий '/' очень важен 
        QDirIterator it(path, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
                        QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString file_path = it.next();
            if (it.fileInfo().isDir()) {
                zip.setCreationPermissions(QFile::permissions(file_path));
                zip.addDirectory(file_path.remove(path));
            } else if (it.fileInfo().isFile()) {
                QFile file(file_path);
                if (!file.open(QIODevice::ReadOnly))
                    continue;
 
                zip.setCreationPermissions(QFile::permissions(file_path));
                QByteArray ba = file.readAll();
                zip.addFile(file_path.remove(path), ba);
            }
        }
        zip.close();
    }
 
    return 0;
}