#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QDialog>
#include <QDebug>
#include <QFileDialog>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDateTime>
#include "chatwindow.h"
#include "networkmanager.h"

class JoinWindow;

namespace Ui {
class loginWindow;  // 대문자로 유지
}

class loginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit loginWindow(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~loginWindow();

private slots:
    void on_loginButton_clicked();
    void on_logoutBtn_clicked();
    void on_joinBtn_clicked();
    void on_inviteBtn_clicked();
    void on_withdrawalBtn_clicked();

private:
    void sendMessageToServer(const QJsonObject &json);
    void processResponse();
    Ui::loginWindow *ui;
    QTcpSocket *socket;
    chatWindow *chatWindoww;
    NetworkManager *networkManager;
    JoinWindow *joinWindow;
};

#endif // LOGINWINDOW_H
