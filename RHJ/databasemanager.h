#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>

class databaseManager
{
public:
    static databaseManager& getInstance(const QString& dbName = "logs.db") {
            static databaseManager instance(dbName);
            return instance;
        }

    databaseManager(const QString& dbName);
    ~databaseManager();
    bool init();
    bool saveMessage(const QString &message);

private:
    QSqlDatabase db;
    QString databaseName;
};

#endif // DATABASEMANAGER_H
