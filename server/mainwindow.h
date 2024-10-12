#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include "chatroom.h"
#include "ui_chatroom.h"
#include <QTCPServer>
#include "chatserver.h"

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
    void UserConnected(USER usr);
    void DataIncome(QByteArray data);


private:
    Ui::MainWindow *ui;
    void LoginSuccess();
    chatRoom *chatroom;
    void ServerOpen(int address);
};
#endif // MAINWINDOW_H
