#include "data.h"

data::data() {}

data::~data()
{
    if(db.isOpen()) {
        db.close();
    }

}

bool data::connectDB(const QString &path)
{
    QString dbpath = "C:\\Program Files\\SQLiteStudio\\newexample.db";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbpath);

    if(!db.open()) {
        qDebug() << "Error Failed to connect to database.";
        return false;
    }

    qDebug() << "Database connected successfuly.";
    return true;
}

bool data::createTable()
{
    QSqlQuery query;
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS people ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "name TEXT NOT NULL, "
                               "age INTEGER NOT NULL)";

    if(!query.exec(createTableQuery)) {
        qDebug() << "Error: Faild to create table - " << query.lastError();
        return false;
    }

    qDebug() << "Table created successfully.";
    return true;
}

bool data::insertData(const QString &name, int age)
{
    QSqlQuery query;
    query.prepare("INSERT INTO people (name, age) VALUES (:name, :age)");
    query.bindValue(":name", name);
    query.bindValue(":age", age);

    if (!query.exec()) {
        qDebug() << "Error: Failed to insert data -" << query.lastError();
        return false;
    }

    qDebug() << "Data inserted successfully.";
    return true;
}

bool data::updateData(int id, const QString &name, int age)
{
    QSqlQuery query;
    query.prepare("UPDATE people SET name = :name, age = :age WHERE id = :id");
    query.bindValue(":name", name);
    query.bindValue(":age", age);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error: Failed to update data -" << query.lastError();
        return false;
    }

    qDebug() << "Data updated successfully.";
    return true;
}

bool data::deleteData(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM people WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error: Failed to delete data -" << query.lastError();
        return false;
    }

    qDebug() << "Data deleted successfully.";
    return true;
}

void data::selectData()
{
    QSqlQuery query;
    if(!query.exec("SELECT id, name, age FROM people")) {
        qDebug() << "Error: Failed to selected data -" << query.lastError();
        return;
    }

    while(query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        int age = query.value(2).toInt();
        qDebug() << id << ": " << name << " - " << age;

    }
}

