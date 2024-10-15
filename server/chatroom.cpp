#include "chatroom.h"
#include "ui_chatroom.h"
#include <QMenu>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QImageCapture>
#include <QPainter>
#include <QImage>
#include <QMediaCaptureSession>
#include <QTimer>

chatRoom::chatRoom(QWidget *parent, ChatServer *server)
    : QWidget(parent)
    , ui(new Ui::chatRoom)
    , camera(nullptr)
    , imageCapture(nullptr)
    , captureSession(new QMediaCaptureSession)
    , chatserver(server)
{
    ui->setupUi(this);
    //ui->cameraView->setMinimumSize(640, 480);

    // 카메라 장치 설정
    camera = new QCamera(QMediaDevices::defaultVideoInput(), this);

    // 미디어 캡처 세션 설정
    captureSession->setCamera(camera);

    // 이미지 캡처 객체 설정
    imageCapture = new QImageCapture(this);
    captureSession->setImageCapture(imageCapture);

    // 카메라 시작
    qDebug() << "Starting camera...";
    camera->start();

    // 주기적으로 이미지 캡쳐
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &chatRoom::captureImage);
    timer->start(100);  // 0.1초(100ms) 간격으로 이미지 캡처


    // 이미지 캡처 후 화면에 그리기
    connect(imageCapture, &QImageCapture::imageCaptured, this, &chatRoom::onImageCaptured);
    // 이미지 캡쳐 후 서버에 그리기
    connect(imageCapture, &QImageCapture::imageCaptured, chatserver, &ChatServer::onImageCaptured);

    ui->userList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->userList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

chatRoom::~chatRoom()
{
    delete ui;
    delete camera;
    delete imageCapture;
    delete captureSession;
}

void chatRoom::captureImage()
{
    if (imageCapture->isReadyForCapture()) {
        qDebug() << "Capturing image...";
        imageCapture->capture();  // 이미지를 캡처
    } else {
        qDebug() << "Image capture is not ready.";
    }
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
