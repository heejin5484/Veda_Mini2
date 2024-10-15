#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H
#include <QThread>

class QCamera;
class QImageCapture;
class QMediaCaptureSession;

class CameraThread : public QThread
{
    Q_OBJECT

public:
    explicit CameraThread(QCamera *camera, QImageCapture *imageCapture, QObject *parent);
    ~CameraThread();
    void run() override;

private :
    QCamera *camera;
    QImageCapture* imageCapture;
    QMediaCaptureSession* captureSession;
};

#endif // CAMERATHREAD_H
