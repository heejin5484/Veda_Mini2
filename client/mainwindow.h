#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_chat.h"
#include "chat.h"
#include <QTcpSocket>

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
    QTcpSocket *clientSocket;
    QTcpSocket *videoSocket;
    QString id;

    void sendMsg(QString msg);

private slots:

    void on_connectButton_clicked();
    void onConnected();
    void onConnected_vid();
    void failedConnect();
    void readMsg();

private:
    Ui::MainWindow *ui;
    chat *ui_chat;
    void tryConnect(QString ip, int port);

signals:
    void deliverMsg(QByteArray msg);

};
#endif // MAINWINDOW_H
