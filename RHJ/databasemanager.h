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
    databaseManager(const QString& dbName);
    ~databaseManager();
    bool init();
    bool saveMessage(const QString& message);

private:
    QSqlDatabase db;
    QString databaseName;
};

#endif // DATABASEMANAGER_H
