#include <QtCore>

QString transl(const QString & s0)
{
    QString s = s0;


        s.replace("А", "A");
        s.replace("Б", "B");
        s.replace("В", "V");
        s.replace("Г", "G");
        s.replace("Д", "D");
        s.replace("Е", "E");
        s.replace("Ё", "E");
        s.replace("Ж", "Zh");
        s.replace("З", "Z");
        s.replace("И", "I");
        s.replace("Й", "Y");
        s.replace("К", "K");
        s.replace("Л", "L");
        s.replace("М", "M");
        s.replace("Н", "N");
        s.replace("О", "O");
        s.replace("П", "P");
        s.replace("Р", "R");
        s.replace("С", "S");
        s.replace("Т", "T");
        s.replace("У", "U");
        s.replace("Ф", "F");
        s.replace("Х", "H");
        s.replace("Ц", "Ts");
        s.replace("Ч", "Ch");
        s.replace("Ш", "Sh");
        s.replace("Щ", "Sch");
        s.replace("Ъ", "");
        s.replace("Ы", "Y");
        s.replace("Ь", "");
        s.replace("Э", "E");
        s.replace("Ю", "Yu");
        s.replace("Я", "Ya");
        s.replace("а", "a");
        s.replace("б", "b");
        s.replace("в", "v");
        s.replace("г", "g");
        s.replace("д", "d");
        s.replace("е", "e");
        s.replace("ё", "e");
        s.replace("ж", "zh");
        s.replace("з", "z");
        s.replace("и", "i");
        s.replace("й", "y");
        s.replace("к", "k");
        s.replace("л", "l");
        s.replace("м", "m");
        s.replace("н", "n");
        s.replace("о", "o");
        s.replace("п", "p");
        s.replace("р", "r");
        s.replace("с", "s");
        s.replace("т", "t");
        s.replace("у", "u");
        s.replace("ф", "f");
        s.replace("х", "h");
        s.replace("ц", "ts");
        s.replace("ч", "ch");
        s.replace("ш", "sh");
        s.replace("щ", "sch");
        s.replace("ъ", "");
        s.replace("ы", "y");
        s.replace("ь", "");
        s.replace("э", "e");
        s.replace("ю", "yu");
        s.replace("я", "ya");



  s.replace("/", "_");
  s.replace(" ", "_");
  s.replace("(", "_");
  s.replace(")", "_");
  s.replace(">", "gt");
  s.replace("<", "lt");
  s.replace(".", "_");
  s.replace(",", "_");
  s.replace(":", "_");
  s.replace("\"", "_");
  
  if (s.length() > 64) s = s.left(64);

  s = s.toLower();

  return s;
}

/*

usigned int bgr_to_rgb(usigned int c) 
{
    unsigned int r = (v & 0xFF) << 16;
    unsigned int g = v & 0xFF00;
    unsigned int b = (v & 0xFF0000) >> 16;

    c = r | g | b;

    return;
}
*/


bool isNul(double q) 
{
//  return fabs(q) == 0;
  return fabs(q) < 1e-2;
}


/*


static QString log_file = "";

void log1(const QString & text)
{
    if (log_file == "") return;

    QTime currentTime = QTime::currentTime();
    QString timeString = currentTime.toString("yyyy-MM-dd hh:mm:ss");

    FILE *f = fopen(log_file, "a");
    if (f) {
        fprintf(f, "%s %s\n", ss, text);
        fclose(f);
    }
}

void log1_clear()
{
    if (log_file == "") return;

    FILE *f = fopen(log_file, "w");
    if (f) {
        fprintf(f, "--------------------------\n");
        fclose(f);
    }
}

void set_log1(const QString & fn)
{
    log_file = fn;
    log1_clear();
}
*/


#include <QProcess>

bool ExecCmd(const QString &command) 
{
//    QCoreApplication a(argc, argv);

    // Команда, которую вы хотите выполнить
//    QString command = "ваша_команда";

    // Создаем процесс
    QProcess process;

    // Устанавливаем параметры для запуска
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(command);

    // Проверяем, запустился ли процесс успешно
    if (!process.waitForStarted()) {
        qDebug() << "Ошибка запуска процесса:" << process.errorString();
        return false;
    }

    // Если нужно, ожидаем завершения процесса
    process.waitForFinished();

    // Получаем вывод процесса (если нужно)
    QString output = process.readAll();
    qDebug() << "Вывод:" << output;

//    return a.exec();
    return true;
}
