#include <QApplication>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QMediaDevices>
#include <QVBoxLayout>
#include <QWidget>
#include "cameraserver.h"

CameraServer::CameraServer(QObject *parent) : QObject(parent) {

    // 비디오 위젯 (카메라 영상 표시)
    QVideoWidget *videoWidget = new QVideoWidget;

    // 미디어 캡처 세션 설정
    QMediaCaptureSession captureSession;

    // 카메라 장치 설정
    QCamera *camera = new QCamera(QMediaDevices::defaultVideoInput());

    // 비디오 출력 설정
    captureSession.setCamera(camera);
    captureSession.setVideoOutput(videoWidget);

    // 레이아웃 설정
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget);

    // 카메라 시작
    camera->start();
}
