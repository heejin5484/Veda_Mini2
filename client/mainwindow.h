#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_chat.h"
#include "chat.h"
#include "joinwindow.h"
#include "loginwindow.h"
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

typedef struct user {
    QString userid;
    QString password;
    QString name;
    QTcpSocket *usersocket;
} USER;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTcpSocket *clientSocket;
    QString id;
    USER currentUser; // currentuser val
    void sendMsg(QString msg);
    void setConnectButtonEnabled(bool enabled);

private slots:

    void on_connectButton_clicked();
    void onConnected();
    void failedConnect();
    void readMsg();
    void on_LoginButton_clicked();

private:

    void on_JoinButton_clicked();
    Ui::MainWindow *ui;
    JoinWindow *joinWindow;
    LoginWindow *loginWindow;
    NetworkManager *networkManager;
    chat *ui_chat;
    void tryConnect(QString ip, int port);

signals:
    void deliverMsg(QByteArray msg);

};
#endif // MAINWINDOW_H
