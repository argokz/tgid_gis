#include <QCoreApplication>
#include <QByteArray>
#include <QBitArray>
#include <QString>
#include <QDebug>
#include <QFile>

void Zip (const QString & filename , const QString & zipfilename)
{
    QFile infile(filename);
    QFile outfile(zipfilename);
    infile.open(QIODevice::ReadOnly);
    outfile.open(QIODevice::WriteOnly);
    QByteArray uncompressedData = infile.readAll();
    QByteArray compressedData = qCompress(uncompressedData,9);
    outfile.write(compressedData);
    infile.close();
    outfile.close();
}

void Unzip (const QString & zipfilename , const QString & filename)
{
    QFile infile(zipfilename);
    QFile outfile(filename);
    infile.open(QIODevice::ReadOnly);
    outfile.open(QIODevice::WriteOnly);
    QByteArray uncompressedData = infile.readAll();
    QByteArray compressedData = qUncompress(uncompressedData);
    outfile.write(compressedData);
    infile.close();
    outfile.close();
}



void Zip(const QString & filename , const QString & zipfilename);
void Unzip(const QString & zipfilename , const QString & filename);

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
 
    //zipping
    Zip("D:\\programs\\demo.txt","D:\\programs\\demo.zip");
    qDebug() << "Done zipping";

    //unzipping
    Unzip("D:\\programs\\demo.zip","D:\\programs\\demo_unzipped.txt");
    qDebug() << "Done unzipping";

    return a.exec();
}
