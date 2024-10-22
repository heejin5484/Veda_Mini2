#ifndef RTPPROCESS_H
#define RTPPROCESS_H

#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QObject>
class rtpProcess : public QObject
{
    Q_OBJECT
public:
    static rtpProcess* instance();

    void startFFmpegProcess();
    void stopFFmpegProcess();    
signals:
    void newFrameAvailable(const QImage& frame);
private:
    rtpProcess();
    rtpProcess(const rtpProcess &) = delete;
    rtpProcess &operator=(const rtpProcess &) = delete;

    // FFmpeg 프로세스 관리용 변수
    QProcess *ffmpegProcess = nullptr;

    QByteArray buffer;
private slots:
    void processFFmpegOutput();
};

#endif // RTPPROCESS_H
