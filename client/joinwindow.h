#ifndef JOINWINDOW_H
#define JOINWINDOW_H

#include <QDialog>
#include <QRegularExpression>
#include <QJsonObject>
#include <QMessageBox>
#include "networkmanager.h"

namespace Ui {
class JoinWindow;
}

class JoinWindow : public QDialog
{
    Q_OBJECT

public:
    explicit JoinWindow(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~JoinWindow();

private slots:
    void on_joinBtn_clicked();

private:
    Ui::JoinWindow *ui;
    NetworkManager *networkManager;
    bool isValidPassword(const QString &password);
    void on_signupResponse(bool success, const QString &message);
};

#endif // JOINWINDOW_H
