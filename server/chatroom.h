#ifndef CHATROOM_H
#define CHATROOM_H

#include <QWidget>
#include "chatserver.h"

namespace Ui {
class chatRoom;
}

class chatRoom : public QWidget
{
    Q_OBJECT

public:
    explicit chatRoom(QWidget *parent = nullptr);
    ~chatRoom();


public slots:
    void showContextMenu(const QPoint &pos);
    void addUserList(QString id);
    void deleteUserList(QString id);
    void onNewFrameAvailable(const QImage& frame);
    void onMessageReceived(const QString &message);

private slots:
    void on_managerButton_clicked();

private:
    Ui::chatRoom *ui;
    ChatServer *chatServer;
};

#endif // CHATROOM_H
