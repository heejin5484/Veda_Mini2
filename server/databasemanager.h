#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DatabaseManager {
public:
    explicit DatabaseManager(const QString& dbName);
    bool init();
    bool saveUserData(const QString& name, const QString& phone, const QString& email, const QString& userid, const QString& password, const QString& type);
    QSqlQuery loadUsers();
    QSqlQuery loadMessages();
    ~DatabaseManager();

private:
    QSqlDatabase db;




};

#endif // DATABASEMANAGER_H
