#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DatabaseManager {
public:
    DatabaseManager(const QString& userdbName, const QString &chatDbName);
    ~DatabaseManager();


    bool saveUserData(const QString& name, const QString& phone, const QString& email, const QString& userid, const QString& password, const QString& type);
    bool saveMessage(const QString& name, const QString& userid, const QString &message);
    QSqlQuery loadUsers();
    QSqlQuery loadMessages();

    bool init();
    QSqlDatabase userDatabase() const;
    QSqlDatabase chatDatabase() const;
    bool openDatabase();
    void close();

private:
    QSqlDatabase userDb;
    QSqlDatabase chatDb;


};

#endif // DATABASEMANAGER_H
