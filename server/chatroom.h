#ifndef CHATROOM_H
#define CHATROOM_H

#include <QWidget>
#include "chatserver.h"

class QMediaCaptureSession;
class QCamera;
class QImageCapture;
class ChatServer;

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
    void onImageCaptured(int id, const QImage &image);


private:
    Ui::chatRoom *ui;

    ChatServer& chatserver;


    QImage capturedImage;
};

#endif // CHATROOM_H
