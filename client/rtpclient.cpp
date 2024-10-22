#include "rtpclient.h"
#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QImage>
#include <QLabel>
#include <QDir>
rtpClient* rtpClient::instance() {
    static rtpClient instance;
    return &instance;
}

rtpClient::rtpClient() {}

void rtpClient::readFFmpegOutput() {
    QByteArray data = ffmpegProcess->readAllStandardOutput();

    // 데이터의 크기 확인
    //qDebug() << "Received data size:" << data.size();

    // raw RGB 데이터로 가정하고 QImage로 변환
    if (data.size() >= 640 * 480 * 3) {  // 해상도 640x480에 대해 RGB는 3바이트 픽셀
        QImage image(reinterpret_cast<const uchar*>(data.data()), 640, 480, QImage::Format_RGB888);

        // QLabel의 크기
        QSize labelSize = videoLabel->size();

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
        QPixmap pixmap =  QPixmap::fromImage(croppedImage).scaled(labelSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        videoLabel->setPixmap(pixmap);
    }
}


void rtpClient::startFFmpegProcess() {
    ffmpegProcess = new QProcess();

    #if 1 // window환경
    // FFmpeg 실행 경로 및 명령어 설정
    QString program = QDir::currentPath() + "/bin/ffmpeg.exe";
    qDebug() << program;
    QStringList arguments;
    #else // linux환경
    // 추가필요
    #endif

    // FFmpeg 명령어 설정 (rawvideo를 stdout으로 출력하도록)
    arguments << "-protocol_whitelist" << "file,udp,rtp"
              << "-i" << "C:/Users/3kati/Desktop/Qt/rtsp/rtsp/build/Desktop_Qt_6_7_2_MSVC2019_64bit-Debug/stream.sdp"
              << "-s" << "640x480"
              << "-pix_fmt" << "rgb24"  // 픽셀 포맷을 raw RGB로 설정
              << "-f" << "rawvideo"  // 출력을 raw 비디오로 설정
              << "-";  // stdout으로 출력

    //  FFmpeg 실행
    ffmpegProcess->start(program, arguments);

    if (!ffmpegProcess->waitForStarted()) {
        qDebug() << "FFmpeg 실행 실패: " << ffmpegProcess->errorString();
    } else {
        qDebug() << "FFmpeg 스트리밍 시작 중...";
        QObject::connect(ffmpegProcess, &QProcess::readyReadStandardOutput, this, &rtpClient::readFFmpegOutput);
    }
}
