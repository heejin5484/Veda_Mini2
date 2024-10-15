#ifndef CAMERASERVER_H
#define CAMERASERVER_H

#include <QCamera>
#include <QImageCapture>
#include <QImageCapture>
#include <QActionGroup>
#include <QMediaDevices>

class CameraServer : public QObject{
    Q_OBJECT
public:
    CameraServer(QObject *parent = nullptr);

private slots:
    void processCapturedImage(int id, const QImage &image) {
        // 이미지 캡처 처리
        qDebug() << "Image captured, processing...";
        // 여기에 클라이언트로 이미지를 보내는 코드 추가
    }
    void init();
    void updateCameras();

private:
    QImageCapture *imageCapture;
    QScopedPointer<QCamera> m_camera;

    QActionGroup *videoDevicesGroup = nullptr;
    QMediaDevices m_devices;

};

#endif // CAMERASERVER_H
