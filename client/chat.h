#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include "networkmanager.h"

namespace Ui {
class chat;
}

class chat : public QWidget
{
    Q_OBJECT

public:
    explicit chat(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~chat();

private:
    Ui::chat *ui;
    NetworkManager *networkManager;

signals :
    void sendMsg_sig(const QString &msg);

private slots:
    void onSendButtonClicked();
    void recieveMsg(QByteArray msg);
};

#endif // CHAT_H
