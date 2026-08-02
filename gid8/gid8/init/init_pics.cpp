#include <QFile>
#include <QDir>
#include <QString>
#include <QRegularExpression>
#include <map>
#include <mystd.h>

static int buf[1024*10];

const int *get_pics_file(int typ, double coef)
{
  QString fn = QString("%1/pics/qq%2.pic").arg(argpath()).arg(typ, 3, 10, QLatin1Char('0'));

  buf[0] = 'q';

  QFile file(fn);

  if (!file.open(QIODevice::ReadOnly))
          return nullptr;

    QDataStream data(&file);
    int n = data.readRawData((char*)buf,1024);

    return (const int *) buf;

}


static std::map <int, int*> map_pics;



const int *get_pics_map(int typ)
{
    auto it = map_pics.find(typ);

    if (it != map_pics.end()) {
        return it->second;
    }
    return nullptr;
}

void init_pics(int n)
{
    const int *pics = get_pics_file(n, 1);
    if (pics) {
        int *pics2 = new int[1024];

        memmove(pics2, pics, 1024*sizeof(int));

        map_pics[n] = pics2;
    }
}

void init_pics()
{
    QStringList files = QDir(argpath()+"pics").entryList(QStringList() << "qq*.pic", QDir::Files);

    static QRegularExpression re("^qq([0-9]+)\\.pic$");

    for (QString &fn : files) {
        QRegularExpressionMatch match = re.match(fn);
        int n = 0;
        if (match.hasMatch()) {
            n = match.captured(1).toInt();
        }
        if (n) {
            init_pics(n);
        }
    }
}
