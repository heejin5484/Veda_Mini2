#include "chatroom.h"
#include "ui_chatroom.h"
#include <QMenu>
#include <QCamera>
#include "camerathread.h"

#include <QPainter>
#include <QImage>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QImageCapture>
#include <QMediaDevices>

chatRoom::chatRoom(QWidget *parent, ChatServer *server)
    : QWidget(parent)
    , ui(new Ui::chatRoom)
    , chatserver(server)
{
    ui->setupUi(this);

    // 카메라 및 캡처 객체는 메인 스레드에서 생성
    QCamera *camera = new QCamera(QMediaDevices::defaultVideoInput(), this);
    QImageCapture *imageCapture = new QImageCapture(this);

    QMediaCaptureSession *captureSession = new QMediaCaptureSession();
    captureSession->setCamera(camera);
    captureSession->setImageCapture(imageCapture);

    // 스레드 생성 및 실행
    CameraThread *cameraThread = new CameraThread(camera, imageCapture, this);
    connect(imageCapture, &QImageCapture::imageCaptured, this, &chatRoom::onImageCaptured);
    connect(imageCapture, &QImageCapture::imageCaptured, chatserver, &ChatServer::onImageCaptured);

    cameraThread->start();  // 스레드 시작

    ui->userList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->userList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

chatRoom::~chatRoom()
{
    delete ui;
}


void chatRoom::onImageCaptured(int id, const QImage &image)
{
    Q_UNUSED(id);
    qDebug() << "Image Captured:" << !image.isNull();

    if (!image.isNull()) {
        // QLabel의 크기
        QSize labelSize = ui->cameraView->size();

        // 이미지의 크기
        QSize imageSize = image.size();

        // 이미지와 QLabel의 종횡비 비교
        QRect targetRect;
        if (static_cast<float>(imageSize.width()) / imageSize.height() > static_cast<float>(labelSize.width()) / labelSize.height()) {
            // 이미지가 가로로 더 길면 좌우를 잘라냄
            int newWidth = static_cast<int>(imageSize.height() * static_cast<float>(labelSize.width()) / labelSize.height());
            int xOffset = (imageSize.width() - newWidth) / 2;
            targetRect = QRect(xOffset, 0, newWidth, imageSize.height());
        } else {
            // 이미지가 세로로 더 길면 위아래를 잘라냄
            int newHeight = static_cast<int>(imageSize.width() * static_cast<float>(labelSize.height()) / labelSize.width());
            int yOffset = (imageSize.height() - newHeight) / 2;
            targetRect = QRect(0, yOffset, imageSize.width(), newHeight);
        }

        // 이미지를 잘라냄
        QImage croppedImage = image.copy(targetRect);

        // 잘린 이미지를 QLabel 크기에 맞게 설정
        QPixmap pixmap = QPixmap::fromImage(croppedImage).scaled(labelSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        ui->cameraView->setPixmap(pixmap);
    }
}

void chatRoom::showContextMenu(const QPoint &pos) {
    QListWidgetItem *item = ui->userList->itemAt(pos);
    if (item) {
        QMenu contextMenu;
        QAction *privateChat = contextMenu.addAction("1:1 채팅");
        QAction *kickUser = contextMenu.addAction("강퇴");

        connect(privateChat, &QAction::triggered, this, [item]() {
            // 1:1 채팅 기능
            qDebug() << "Starting 1:1 chat with" << item->text();
        });

        connect(kickUser, &QAction::triggered, this, [item]() {
            // 강퇴 기능
            qDebug() << "Kicking" << item->text();
        });

        contextMenu.exec(ui->userList->mapToGlobal(pos));
    }
}

void chatRoom::addUserList(QString id){
    qDebug() << id;
    ui->userList->addItem(id);
}

void chatRoom::deleteUserList(QString id){
    qDebug() << id << " disconnected.";

    // QListWidget에서 해당 ID를 찾음
    QList<QListWidgetItem*> items = ui->userList->findItems(id, Qt::MatchExactly);

    // 찾은 항목이 있을 경우 제거
    if (!items.isEmpty()) {
        // 첫 번째 항목을 가져와서 제거
        delete ui->userList->takeItem(ui->userList->row(items[0]));
        qDebug() << id << " removed from user list.";
    } else {
        qDebug() << id << " not found in user list.";
    }
}
