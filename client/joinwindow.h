#ifndef JOINWINDOW_H
#define JOINWINDOW_H

#include <QDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QRegularExpression>
#include <QJsonObject>
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
    void togglePasswordVisibility();
    void onDomainSelected(int index);

private:
    Ui::JoinWindow *ui;
    NetworkManager *networkManager;
    bool isValidPassword(const QString &password);
    void on_signupResponse(bool success, const QString &message);
    void setupEmailDomainComboBox();

};

#endif // JOINWINDOW_H
