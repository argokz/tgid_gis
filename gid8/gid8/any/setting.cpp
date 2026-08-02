#include <QCoreApplication>
#include <QFile>
#include <QSettings>
#include <QCryptographicHash>
#include <QDebug>

// Простейший метод шифрования/дешифрования (заменить на AES для безопасности)
QByteArray simpleEncryptDecrypt(const QByteArray &data, const QByteArray &key) {
    QByteArray result = data;
    for (int i = 0; i < data.size(); ++i)
        result[i] = data[i] ^ key[i % key.size()];
    return result;
}

// Функция для записи зашифрованного файла
void writeEncryptedSettings(const QString &filename, const QByteArray &key) {
    QSettings settings(filename, QSettings::IniFormat);

    // Настройки
    settings.beginGroup("Window");
    settings.setValue("width", 800);
    settings.setValue("height", 600);
    settings.setValue("fullscreen", false);
    settings.endGroup();

    settings.beginGroup("User");
    settings.setValue("username", "admin");
    settings.setValue("password", "12345");
    settings.endGroup();

    // Считываем файл и шифруем
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QByteArray encryptedData = simpleEncryptDecrypt(data, key);

        // Записываем зашифрованный файл
        if (file.open(QIODevice::WriteOnly)) {
            file.write(encryptedData);
            file.close();
        }
    }
}

// Функция для чтения зашифрованного файла
void readEncryptedSettings(const QString &filename, const QByteArray &key) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray encryptedData = file.readAll();
        QByteArray decryptedData = simpleEncryptDecrypt(encryptedData, key);

        // Временный файл для расшифрованного содержимого
        QString tempFile = filename + ".tmp";
        QFile temp(tempFile);
        if (temp.open(QIODevice::WriteOnly)) {
            temp.write(decryptedData);
            temp.close();
        }

        // Чтение данных через QSettings
        QSettings settings(tempFile, QSettings::IniFormat);
        settings.beginGroup("Window");
        qDebug() << "Width:" << settings.value("width").toInt();
        qDebug() << "Height:" << settings.value("height").toInt();
        qDebug() << "Fullscreen:" << settings.value("fullscreen").toBool();
        settings.endGroup();

        settings.beginGroup("User");
        qDebug() << "Username:" << settings.value("username").toString();
        settings.endGroup();

        // Удаляем временный файл
        temp.remove();
    }
}

#if 0
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    QString filename = "config.ini";
    QByteArray key = QCryptographicHash::hash("my_secret_key", QCryptographicHash::Sha256);

    writeEncryptedSettings(filename, key);
    readEncryptedSettings(filename, key);

    return app.exec();
}
#endif