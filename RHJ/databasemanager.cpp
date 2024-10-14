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
    QSqlQuery query;
    query.prepare("INSERT INTO log_messages (message, timestamp) VALUES (:message, CURRENT_TIMESTAMP)");
    query.bindValue(":message", message);

    if (!query.exec()) {
        qDebug() << "Failed to save message to database: " << query.lastError().text();
        return false;
    }
    return true;
}



