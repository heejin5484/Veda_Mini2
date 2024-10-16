#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QJsonObject>
#include "networkmanager.h"


namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_LoginButton_clicked();

private:
    Ui::LoginWindow *ui;
    NetworkManager *networkManager;
    void handleServerResponse(bool success, const QString &message);
};

#endif // LOGINWINDOW_H
