#ifndef DATABASEWINDOW_H
#define DATABASEWINDOW_H

#include <QDialog>
#include <QSqlQueryModel>
#include "databasemanager.h"

namespace Ui {
class DatabaseWindow;
}

class DatabaseWindow : public QDialog {
    Q_OBJECT

public:
    explicit DatabaseWindow(QWidget *parent = nullptr);
    ~DatabaseWindow();

private slots:
    void onFilterChanged(const QString &filter);
    void loadUsers();
    void loadMessages();
    void loadMessagesByUser();

private:
    Ui::DatabaseWindow *ui;
    DatabaseManager *dbManager;
    QSqlQueryModel *model;
    void updateUserTable();
};

#endif // DATABASEWINDOW_H
