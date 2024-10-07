#include "databasemanager.h"
#include <QDateTime>

databaseManager::databaseManager(const QString& dbName)
    : databaseName(dbName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databaseName);
}


databaseManager::~databaseManager()
{
    if (db.isOpen()) {
        db.close();
    }
}

// DB table create && init
bool databaseManager::init()
{
    if (!db.open()) {
        qDebug() << "Database connection failed: " << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    QString createTable = "CREATE TABLE IF NOT EXISTS log_messages ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "timestamp TEXT, "
                          "message TEXT)";

    if (!query.exec(createTable)) {
        qDebug() << "Failed to create table: " << query.lastError().text();
        return false;
    }

    return true;
}

bool databaseManager::saveMessage(const QString &message) {
    if (!db.isOpen()) {
        qDebug() << "Database is not open.";
        return false;
    }

    QSqlQuery query;
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    query.prepare("INSERT INTO log_messages (timestamp, message) VALUES (?, ?)");
    query.addBindValue(dateTime);
    query.addBindValue(message);

    if (!query.exec()) {
        qDebug() << "Failed to insert message: " << query.lastError().text();
        return false;
    }
    else {
        qDebug() << "Message saved to DB: " << message;
        return true;
    }
}

