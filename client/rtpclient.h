#ifndef RTPCLIENT_H
#define RTPCLIENT_H

#include <QObject>
class QProcess;
class QLabel;

class rtpClient : public QObject {
Q_OBJECT

public:
    static rtpClient* instance();

    // FFmpeg 프로세스 시작 및 데이터 읽기 메서드
    void startFFmpegProcess();
    void readFFmpegOutput();

    // FFmpeg 프로세스 관리용 변수
    QProcess *ffmpegProcess = nullptr;
    QLabel *videoLabel = nullptr;

private:
    rtpClient();
    rtpClient(const rtpClient &) = delete;
    rtpClient &operator=(const rtpClient &) = delete;
};

#endif // RTPCLIENT_H
