#include "camerathread.h"
#include <QCamera>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QTimer>

CameraThread::CameraThread(QCamera *camera, QImageCapture *imageCapture, QObject *parent)
    : QThread(parent), camera(camera), imageCapture(imageCapture)
{
}

CameraThread::~CameraThread(){
    // 카메라 스레드를 안전하게 종료
    if (this->isRunning()) {
        requestInterruption();
        this->quit();  // 스레드의 실행을 중단
        this->wait();  // 스레드가 완전히 종료될 때까지 대기
    }
}

void CameraThread::run(){

    if (!camera || !imageCapture) {
        qWarning() << "Camera or imageCapture not set!";
        return;
    }

    // 카메라 시작
    qDebug() << "Starting camera...";
    camera->start();

    while(!isInterruptionRequested()){
        if (imageCapture->isReadyForCapture()) {
            imageCapture->capture();  // 이미지를 캡처
        }
        QThread::msleep(100); // 100ms간격으로 캡쳐
    }

    qDebug() << "Stopping camera...";
    camera->stop();

}
