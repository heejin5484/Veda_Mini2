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
    explicit chatRoom(QWidget *parent = nullptr, ChatServer *server = nullptr);
    ~chatRoom();

public slots:
    void showContextMenu(const QPoint &pos);
    void addUserList(QString id);
    void deleteUserList(QString id);
    void onImageCaptured(int id, const QImage &image);
    void captureImage();

private:
    Ui::chatRoom *ui;
    QCamera* camera;
    QImageCapture* imageCapture;
    QMediaCaptureSession* captureSession;
    ChatServer *chatserver;


    QImage capturedImage;
};

#endif // CHATROOM_H
