#ifndef RTPPROCESS_H
#define RTPPROCESS_H

#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QTimer>

class rtpProcess
{
public:
    static rtpProcess* instance();

    void startFFmpegProcess();
    void stopFFmpegProcess();

private:
    rtpProcess();
    rtpProcess(const rtpProcess &) = delete;
    rtpProcess &operator=(const rtpProcess &) = delete;

    // FFmpeg 프로세스 관리용 변수
    QProcess *ffmpegProcess = nullptr;
};

#endif // RTPPROCESS_H
