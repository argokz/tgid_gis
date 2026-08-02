#include <QCoreApplication>
#include <QtSql>
#include <QDebug>
#include <QVector>

class DatabaseManager
{
public:
    static DatabaseManager& instance()
    {
        static DatabaseManager instance;
        return instance;
    }

    QSqlDatabase acquireDatabase() const
    {
        QMutexLocker locker(&m_mutex);
        
        // Если в пуле есть доступное соединение, возвращаем его
        if (!m_connectionPool.isEmpty()) {
            QSqlDatabase db = m_connectionPool.takeLast();
            if (db.isOpen()) {
                return db;
            }
        }

        // Если пул пуст или соединение не открыто, создаем новое соединение
        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
        db.setDatabaseName("DRIVER={SQL Server};SERVER=your_server;DATABASE=your_database;UID=your_username;PWD=your_password");

        if (!db.open()) {
            qDebug() << "Failed to open database connection!";
        }

        return db;
    }

    void releaseDatabase(const QSqlDatabase& db) const
    {
        QMutexLocker locker(&m_mutex);
        
        // Возвращаем соединение в пул
        m_connectionPool.append(db);
    }

private:
    mutable QMutex m_mutex;
    mutable QVector<QSqlDatabase> m_connectionPool;

    DatabaseManager() {} // Запрещаем создание экземпляров класса
    ~DatabaseManager() {} // Запрещаем удаление экземпляров класса
    DatabaseManager(const DatabaseManager&); // Запрещаем копирование
    DatabaseManager& operator=(const DatabaseManager&); // Запрещаем присваивание
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Получаем экземпляр менеджера базы данных
    DatabaseManager& manager = DatabaseManager::instance();

    // Получаем соединение из пула
    QSqlDatabase db = manager.acquireDatabase();

    if (db.isOpen()) {
        qDebug() << "Database connection is open!";
        
        // Здесь можно выполнять запросы к базе данных
        
        // После использования соединения, освобождаем его
        manager.releaseDatabase(db);
    } else {
        qDebug() << "Failed to acquire database connection!";
    }

    return a.exec();
}
