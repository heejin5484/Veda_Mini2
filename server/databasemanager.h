#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DatabaseManager {
public:
    explicit DatabaseManager(const QString &dbName);
    ~DatabaseManager();

    bool init();
    bool saveUserData(const QString &username, const QString &password, const QString &name, const QString &phone, const QString &email);
    bool checkUser(const QString &username, const QString &password);

private:
    QString databaseName;
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
