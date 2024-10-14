#ifndef JOINWINDOW_H
#define JOINWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include "networkmanager.h"
#include "loginwindow.h"

namespace Ui {
class JoinWindow;
}

class JoinWindow : public QDialog
{
    Q_OBJECT

public:
    explicit JoinWindow(NetworkManager *networkManager, QWidget *parent = 0);
    ~JoinWindow();

private slots:
    void on_joinBtn_clicked();

    void on_signupResponse(bool success);
    void on_loginBtn_clicked();

private:
    Ui::JoinWindow *ui;
    NetworkManager *networkManager;

    bool isValidPassword(const QString &password);
};

#endif // JOINWINDOW_H
