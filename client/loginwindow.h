#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QJsonObject>
class MainWindow;
class NetworkManager;

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
    void togglePasswordVisibility();
    //void handleServerResponse(bool success, const QString &message);
    void handleServerResponse(bool success, const QString &message, const QString &userId, const QString &name);

private:
    Ui::LoginWindow *ui;
    NetworkManager *networkManager;
    void applyStyles();
    //void handleServerResponse(bool success, const QString &message);
};

#endif // LOGINWINDOW_H
