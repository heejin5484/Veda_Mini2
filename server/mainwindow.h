#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTcpServer>
#include "chatserver.h"

class chatRoom;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTcpServer *server;


public slots:
    void on_Login_button_clicked();
    void UserConnected(USER* usr);
    void DataIncome(QByteArray data, USER* user);
    void UserDisconnected(USER *usr);

signals :
    void NewUserAdd(QString id);
    void DisconnectUser(QString id);

private:
    Ui::MainWindow *ui;
    void LoginSuccess();
    chatRoom *chatroom;
    bool ServerOpen(int address);
    QMap<QString, USER*> UserMap;

};
#endif // MAINWINDOW_H
